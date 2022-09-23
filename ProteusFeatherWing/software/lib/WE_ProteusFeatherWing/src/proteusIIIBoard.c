/**
 * \file
 * \brief File for the Proteus-III board of the WE IoT design kit.
 *
 * \copyright (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */
#include <stdlib.h>
#include <stdbool.h>
#include "proteusIIIBoard.h"

#define CMD_ARRAY_SIZE()                                     \
    ((((uint16_t)CMD_Array[CMD_POSITION_LENGTH_LSB] << 0) |  \
      ((uint16_t)CMD_Array[CMD_POSITION_LENGTH_MSB] << 8)) + \
     LENGTH_CMD_OVERHEAD)
char CMD_Array[MAX_CMD_LENGTH];

static uint16_t RxByteCounter = 0;
static bool packetReceived = false;
static uint16_t BytesToReceive = 0;
static uint8_t pRxBuffer[MAX_CMD_LENGTH];

static PROTEUSIII_cb proteus_cb;

#define CMDCONFIRMATIONARRAY_LENGTH 2
ProteusIII_CMD_Confirmation_t
    cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];

void PROTEUSIII_sendBytes(PROTEUSIII *self, const char *data, int dataLength);

bool PROTEUSIII_waitForReply(PROTEUSIII *self, uint8_t expectedCmdConfirmation,
                             ProteusIII_CMD_Status_t expectedStatus,
                             bool reset_confirmstate);

bool FillChecksum(char *pArray, uint16_t length);

void PROTEUSIII_receiveBytes(PROTEUSIII *self);

void HandleRxPacket(PROTEUSIII *self, uint8_t *pRxBuffer);

/**
 * @brief  Allocate memory and initialize the Proteus object
 * @param  serialDebug Pointer to the serial debug
 * @param  serialProteusIII Pointer to the Proteus serial object
 * @param  settings Proteus settings
 * @retval Proteus object
 */
PROTEUSIII *PROTEUSIII_Create(TypeSerial *serialDebug,
                              TypeHardwareSerial *serialProteusIII,
                              PROTEUSIII_cb *callbacks) {
    PROTEUSIII *allocateInit = (PROTEUSIII *)malloc(sizeof(PROTEUSIII));
    allocateInit->serialDebug = serialDebug;
    allocateInit->serialProteusIII = serialProteusIII;
    RxByteCounter = 0;
    allocateInit->channelopen = false;
    if (callbacks) {
        proteus_cb.connected = callbacks->connected;
        proteus_cb.disconnected = callbacks->disconnected;
        proteus_cb.datareceived = callbacks->datareceived;
        proteus_cb.channelopen = callbacks->channelopen;
    }
    return allocateInit;
}

/**
 * @brief  Proteus send data over Proteus serial port
 * @param  self Pointer to the Proteus object.
 * @param  data Pointer to the bytes to send
 * @param  dataLength Length of the data
 * @retval none
 */
void PROTEUSIII_sendBytes(PROTEUSIII *self, const char *data, int dataLength) {
#if SERIAL_DEBUG
    SSerial_printf(self->serialDebug, "Sending to Proteus III: \n");
    for (int i = 0; i < dataLength; i++) {
        SSerial_printf(self->serialDebug, "%02X ", data[i]);
    }
    SSerial_printf(self->serialDebug, "\n");
#endif

    int written = 1;

    while (written) {
        while (HSerial_available(self->serialProteusIII)) {
            HSerial_read(self->serialProteusIII);
        }
        if (dataLength <= HSerial_availableForWrite(self->serialProteusIII)) {
            HSerial_writeB(self->serialProteusIII, data, dataLength);
            written = 0;
        }
    }
}

/**
 * @brief Transmit data if a connection is open
 *
 * @param[in] payloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool PROTEUSIII_Transmit(PROTEUSIII *self, uint8_t *payloadP, uint16_t length) {
    bool ret = false;
    if (self->channelopen && length <= PROTEUSIII_MAX_PAYLOAD_LENGTH) {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = PROTEUSIII_CMD_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
            PROTEUSIII_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            return PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_DATA_CNF,
                                           CMD_Status_Success, true);
            //                            &&
            //    PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_TXCOMPLETE_RSP,
            //                            CMD_Status_Success, true);
        }
    }
    return ret;
}

bool PROTEUSIII_Receive(PROTEUSIII *self) {
    if (HSerial_available(self->serialProteusIII)) {
        packetReceived = false;
        while (!packetReceived) {
            PROTEUSIII_receiveBytes(self);
        }
        return true;
    }
    return false;
}

/**
 * @brief  Wait for response from Proteus
 * @param  self Pointer to the Proteus object.
 * @param  expectedCmdConfirmation expected confirmation
 * @param  expectedStatus expected status
 * @param  reset_confirmstate reset status
 * @retval true if successful false in case of failure
 */
bool PROTEUSIII_waitForReply(PROTEUSIII *self, uint8_t expectedCmdConfirmation,
                             ProteusIII_CMD_Status_t expectedStatus,
                             bool reset_confirmstate) {
    unsigned long startTime = micros();
    unsigned long interval = 0;
    packetReceived = false;
    RxByteCounter = 0;
    if (reset_confirmstate) {
        for (int i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }

    while (1) {
        interval = micros() - startTime;
        PROTEUSIII_receiveBytes(self);
        if ((interval) >= (TIMEOUT * 1000)) /*ms to microseconds*/
        {
            break;
        }
        for (int i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd) {
                SSerial_printf(self->serialDebug, "cmd stats %02X \n",
                               cmdConfirmation_array[i].status);
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }
    }
    return false;
}

/**
 * @brief  Fill checksum
 * @param  pArray Pointer to the packet
 * @param  dataLength Length of the packet
 * @retval none
 */
bool FillChecksum(char *pArray, uint16_t length) {
    bool ret = false;

    if ((length >= LENGTH_CMD_OVERHEAD) && (pArray[0] == CMD_STX)) {
        uint8_t checksum = (uint8_t)0;
        uint16_t payload_length =
            (uint16_t)(pArray[CMD_POSITION_LENGTH_MSB] << 8) +
            pArray[CMD_POSITION_LENGTH_LSB];
        uint16_t i = 0;
        for (i = 0; i < (payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC);
             i++) {
            checksum ^= pArray[i];
        }
        pArray[payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC] = checksum;
        ret = true;
    }
    return ret;
}

/**
 * @brief  Receive data from the serialProteusIII port
 * @param  self Pointer to the Proteus object.
 * @retval none
 */
void PROTEUSIII_receiveBytes(PROTEUSIII *self) {
    uint8_t checksum = 0;
    uint8_t readBuffer;

    while (HSerial_available(self->serialProteusIII)) {
        readBuffer = (uint8_t)HSerial_read(self->serialProteusIII);

        /* interpret received byte */
        pRxBuffer[RxByteCounter] = readBuffer;
        switch (RxByteCounter) {
            case 0:
                SSerial_printf(self->serialDebug,
                               "received from Proteus III: \n");
                /* wait for start byte of frame */
                if (pRxBuffer[RxByteCounter] == CMD_STX) {
                    BytesToReceive = 0;
                    RxByteCounter = 1;
                }
                break;

            case 1:
                /* CMD */
                RxByteCounter++;
                break;

            case 2:
                /* length field lsb */
                RxByteCounter++;
                BytesToReceive = (uint16_t)(pRxBuffer[RxByteCounter - 1]);
                break;

            case 3:
                /* length field msb */
                RxByteCounter++;
                BytesToReceive +=
                    (((uint16_t)pRxBuffer[RxByteCounter - 1] << 8) +
                     LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd +
                                                  cmd */
                break;

            default:
                /* data field */
                RxByteCounter++;
                if (RxByteCounter == BytesToReceive) {
                    /* check CRC */
                    checksum = 0;
                    int i = 0;
                    for (i = 0; i < (BytesToReceive - 1); i++) {
                        checksum ^= pRxBuffer[i];
                    }

                    if (checksum == pRxBuffer[BytesToReceive - 1]) {
                        /* received frame ok, interpret it now */
                        HandleRxPacket(self, pRxBuffer);
                    }
                    RxByteCounter = 0;
                    packetReceived = true;
                }
                break;
        }
        SSerial_printf(self->serialDebug, "%02X ", readBuffer);
        if (packetReceived) {
            SSerial_printf(self->serialDebug, "\n");
            return;
        }
    }
}

/**
 * @brief  Parse the received packet
 * @param  self Pointer to the Proteus object.
 * @param  data Pointer to the received packet
 * @retval true if successful false in case of failure
 */
void HandleRxPacket(PROTEUSIII *self, uint8_t *pRxBuffer) {
    ProteusIII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    switch (pRxBuffer[CMD_POSITION_CMD]) {
        case PROTEUSIII_CMD_DATA_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case PROTEUSIII_CMD_TXCOMPLETE_RSP: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case PROTEUSIII_CMD_GET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            int length = ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) +
                         ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8);
            memcpy(&self->usersettingsbuffer, &pRxBuffer[CMD_POSITION_DATA + 1],
                   length - 1);
            self->usersettingsbufferdatalength = length - 1;
            break;
        }

        case PROTEUSIII_CMD_SET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case PROTEUSIII_CMD_GETSTATE_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_Success;
            break;
        }

        case PROTEUSIII_CMD_DATA_IND: {
            int length = ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) +
                         ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8);
            if (proteus_cb.datareceived) {
                proteus_cb.datareceived(&pRxBuffer[CMD_POSITION_DATA], length);
            }
            break;
        }

        case PROTEUSIII_CMD_CONNECT_IND: {
            if (proteus_cb.connected) {
                proteus_cb.connected();
            }
            break;
        }

        case PROTEUSIII_CMD_DISCONNECT_IND: {
            if (proteus_cb.disconnected) {
                self->channelopen = false;
                proteus_cb.disconnected();
            }
            break;
        }

        case PROTEUSIII_CMD_CHANNELOPEN_RSP: {
            if (proteus_cb.channelopen) {
                self->channelopen = true;
                proteus_cb.channelopen();
            }
            break;
        }

        default: {
            /* invalid*/
            break;
        }
    }

    int i = 0;
    for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        if (cmdConfirmation_array[i].cmd == CNFINVALID) {
            cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
            cmdConfirmation_array[i].status = cmdConfirmation.status;
            break;
        }
    }
}

bool PROTEUSIII_Get(PROTEUSIII *self, ProteusIII_UserSettings_t userSetting,
                    uint8_t *ResponseP) {
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = PROTEUSIII_CMD_GET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
        /* now send CMD_ARRAY */
        PROTEUSIII_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        if (PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_GET_CNF,
                                    CMD_Status_Success, true)) {
            memcpy(ResponseP, &self->usersettingsbuffer,
                   self->usersettingsbufferdatalength);
            ret = true;
        }
    }
    return ret;
}

bool PROTEUSIII_GetConnectionTiming(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Get(self, ProteusIII_USERSETTING_INDEX_CONNECTION_TIMING,
                          value);
}

bool PROTEUSIII_GetDeviceName(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Get(self, ProteusIII_USERSETTING_INDEX_DEVICE_NAME,
                          value);
}

bool PROTEUSIII_GetBTMacAddress(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Get(self, ProteusIII_USERSETTING_INDEX_BT_MAC_ADDRESS,
                          value);
}

bool PROTEUSIII_GetFWVersion(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Get(self, ProteusIII_USERSETTING_INDEX_FWVersion, value);
}

bool PROTEUSIII_GetCFGFlags(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Get(self, ProteusIII_USERSETTING_INDEX_CFG_FLAGS, value);
}

bool PROTEUSIII_Set(PROTEUSIII *self, ProteusIII_UserSettings_t userSetting,
                    uint8_t *ValueP, uint8_t length) {
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = PROTEUSIII_CMD_SET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(1 + length);
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], ValueP, length);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
        /* now send CMD_ARRAY */
        PROTEUSIII_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_SET_CNF,
                                       CMD_Status_Success, true) &&
               PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_GETSTATE_CNF,
                                       CMD_Status_Success, true);
    }
    return ret;
}

bool PROTEUSIII_SetConnectionTiming(PROTEUSIII *self, uint8_t value) {
    return PROTEUSIII_Set(self, ProteusIII_USERSETTING_INDEX_CONNECTION_TIMING,
                          &value, 1);
}

bool PROTEUSIII_SetDeviceName(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Set(self, ProteusIII_USERSETTING_INDEX_DEVICE_NAME, value,
                          (uint8_t)8);
}

bool PROTEUSIII_SetCFGFlags(PROTEUSIII *self, uint8_t *value) {
    return PROTEUSIII_Set(self, ProteusIII_USERSETTING_INDEX_CFG_FLAGS, value,
                          (uint8_t)2);
}

bool PROTEUSIII_SetAdvertisingFlags(PROTEUSIII *self, uint8_t value) {
    return PROTEUSIII_Set(
        self, ProteusIII_USERSETTING_INDEX_RF_AdvertisingFlags, &value, 1);
}

bool PROTEUSIII_SetBeaconData(PROTEUSIII *self, uint8_t *value, int length) {
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = PROTEUSIII_CMD_SETBEACON_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length);
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    memcpy(&CMD_Array[CMD_POSITION_DATA], value, length);
    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
        /* now send CMD_ARRAY */
        PROTEUSIII_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return PROTEUSIII_waitForReply(self, PROTEUSIII_CMD_SETBEACON_CNF,
                                       CMD_Status_Success, true);
    }
    return false;
}