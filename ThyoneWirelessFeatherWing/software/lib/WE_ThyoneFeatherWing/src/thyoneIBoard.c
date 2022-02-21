/**
 * \file
 * \brief File for the Thyone-i board of the WE IoT design kit.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
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
#include "thyoneIBoard.h"

#define CMD_ARRAY_SIZE()                                     \
    ((((uint16_t)CMD_Array[CMD_POSITION_LENGTH_LSB] << 0) |  \
      ((uint16_t)CMD_Array[CMD_POSITION_LENGTH_MSB] << 8)) + \
     LENGTH_CMD_OVERHEAD)
char CMD_Array[MAX_CMD_LENGTH];

static uint16_t RxByteCounter = 0;
static bool packetReceived = false;
static uint16_t BytesToReceive = 0;
static uint8_t pRxBuffer[MAX_CMD_LENGTH];

void THYONEI_printPacket(THYONEI *self, uint8_t *packet, int length);
void THYONEI_sendBytes(THYONEI *self, const char *data, int dataLength);
void HandleRxPacket(THYONEI *self, uint8_t *pRxBuffer);
bool FillChecksum(char *pArray, uint16_t length);
bool THYONEI_waitForReply(THYONEI *self, uint8_t expectedCmdConfirmation,
                          CMD_Status_t expectedStatus, bool reset_confirmstate);
void THYONEI_receiveBytes(THYONEI *self);
bool ThyoneI_Get(THYONEI *self, ThyoneI_UserSettings_t userSetting,
                 uint8_t *ResponseP, uint16_t *Response_LengthP);
bool ThyoneI_Set(THYONEI *self, ThyoneI_UserSettings_t userSetting,
                 uint8_t *ValueP, uint8_t length);

#define CMDCONFIRMATIONARRAY_LENGTH 3
CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];

/**
 * @brief  Allocate memory and initialize the Thyone object
 * @param  serialDebug Pointer to the serial debug
 * @param  serialThyoneI Pointer to the thyone serial object
 * @param  settings Thyone settings
 * @retval Thyone object
 */
THYONEI *THYONEI_Create(TypeSerial *serialDebug,
                        TypeHardwareSerial *serialThyoneI)
{
    THYONEI *allocateInit = (THYONEI *)malloc(sizeof(THYONEI));
    allocateInit->serialDebug = serialDebug;
    allocateInit->serialThyoneI = serialThyoneI;
    strncpy(allocateInit->bufferThyone.data, "0",
            sizeof(allocateInit->bufferThyone.data));

    allocateInit->settings.sourceAddress = 0;
    allocateInit->settings.destinationAddress = 0;
    packetReceived = false;
    RxByteCounter = 0;
    return allocateInit;
}

/**
 * @brief  Free up the memory assigned to the Thyone object
 * @param  thyone Pointer to the object.
 * @retval none
 */
void THYONEIDestroy(THYONEI *thyone)
{
    if (thyone)
    {
        packetReceived = false;
        RxByteCounter = 0;
        free(thyone);
    }
}

/**
 * @brief  Reboot Thyone and check comms
 * @param  self Pointer to the Thyone object.
 * @retval true if successful false in case of failure
 */
bool THYONEI_simpleInit(THYONEI *self)
{
#if SERIAL_DEBUG
    SSerial_printf(self->serialDebug, "Starting Thyone_I...\r\n");
#endif
    if (ThyoneI_Reset(self))
    {
        uint8_t src[4] = {0};
        uint32_t dest = 0;
        /*By default, the serial number of the module is the source address*/
        if (ThyoneI_GetSerialNumber(self, src))
        {
            self->settings.sourceAddress = (((uint32_t)src[3]) << 24 | ((uint32_t)src[2]) << 16 |
                                            ((uint32_t)src[1]) << 8 | ((uint32_t)src[0]) << 0);
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "Thyone source id %lu\r\n",
                           self->settings.sourceAddress);
#endif
        }
        else
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug,
                           "Getting Thyone source id failed\r\n");
#endif
        }
        if (ThyoneI_GetDestinationAddress(self, &dest))
        {
            self->settings.destinationAddress = dest;
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "Thyone destination id %lu\r\n",
                           self->settings.destinationAddress);
#endif
        }
        else
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug,
                           "Getting Thyone source id failed\r\n");
#endif
        }
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Init Thyone_I done!\r\n");
#endif
        return true;
    }
    return false;
}

/**
 * @brief  Reboot Thyone module
 * @param  self Pointer to the Thyone object.
 * @retval true if successful false in case of failure
 */
bool ThyoneI_Reset(THYONEI *self)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_RESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return THYONEI_waitForReply(self, ThyoneI_CMD_START_IND,
                                    CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief  Set Thyone to sleep mode
 * @param  self Pointer to the Thyone object.
 * @retval true if successful false in case of failure
 */
bool ThyoneI_Sleep(THYONEI *self)
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_SLEEP_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = THYONEI_waitForReply(self, ThyoneI_CMD_SLEEP_CNF,
                                   CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief  Broadcast data
 * @param  self Pointer to the Thyone object.
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval true if successful false in case of failure
 */
bool ThyoneI_TransmitBroadcast(THYONEI *self, uint8_t *payloadP,
                               uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_BROADCAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            ret = THYONEI_waitForReply(self, ThyoneI_CMD_TXCOMPLETE_RSP,
                                       CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief  Multicast data to pre-configured group ID
 * @param  self Pointer to the Thyone object.
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval true if successful false in case of failure
 */
bool ThyoneI_TransmitMulticast(THYONEI *self, uint8_t *payloadP,
                               uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_MULTICAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            ret = THYONEI_waitForReply(self, ThyoneI_CMD_TXCOMPLETE_RSP,
                                       CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief  Unicast data to pre-configured address
 * @param  self Pointer to the Thyone object.
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval true if successful false in case of failure
 */
bool ThyoneI_TransmitUnicast(THYONEI *self, uint8_t *payloadP,
                             uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_UNICAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            ret = THYONEI_waitForReply(self, ThyoneI_CMD_TXCOMPLETE_RSP,
                                       CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief  Multicast data to specified group ID
 * @param  self Pointer to the Thyone object.
 * @param  groupID Multicast address
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval true if successful false in case of failure
 */
bool ThyoneI_TransmitMulticastExtended(THYONEI *self, uint8_t groupID,
                                       uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH_MULTICAST_EX)
    {
        uint16_t cmdLength = length + 1;
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_MULTICAST_DATA_EX_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(cmdLength >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(cmdLength >> 8);
        CMD_Array[CMD_POSITION_CMD] = groupID;

        memcpy(&CMD_Array[CMD_POSITION_DATA + 1], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            ret = THYONEI_waitForReply(self, ThyoneI_CMD_TXCOMPLETE_RSP,
                                       CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief  Unicast data to specified address
 * @param  self Pointer to the Thyone object.
 * @param  address Destination address
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval true if successful false in case of failure
 */
bool ThyoneI_TransmitUnicastExtended(THYONEI *self, uint32_t address,
                                     uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH_UNICAST_EX)
    {
        uint16_t cmdLength = length + 4;
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_UNICAST_DATA_EX_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(cmdLength >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(cmdLength >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], &address, 4);
        memcpy(&CMD_Array[CMD_POSITION_DATA + 4], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());
            ret = THYONEI_waitForReply(self, ThyoneI_CMD_TXCOMPLETE_RSP,
                                       CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief  Receive data from Thyone
 * @param  self Pointer to the Thyone object.
 * @retval true if successful false in case of failure
 */
PacketThyoneI THYONEI_receiveData(THYONEI *self)
{
    unsigned long startTime = micros();
    unsigned long interval = 0;
    self->bufferThyone.length = 0;
    packetReceived = false;
    RxByteCounter = 0;
    while (!packetReceived)
    {
        interval = micros() - startTime;
        THYONEI_receiveBytes(self);
        if ((interval) >= (TIMEOUT * 1000)) /*ms to microseconds*/
        {
            break;
        }
    }
    return self->bufferThyone;
}

/**
 * @brief  Set source address on the Thyone module
 * @param  self Pointer to the Thyone object.
 * @param  sourceAddress Address to set
 * @retval true if successful false in case of failure
 */
bool ThyoneI_SetSourceAddress(THYONEI *self, uint32_t sourceAddress)
{
    return ThyoneI_Set(self, ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS,
                       (uint8_t *)&sourceAddress, 4);
}

/**
 * @brief  Get source address on the Thyone module
 * @param  self Pointer to the Thyone object.
 * @param  sourceAddressP Pointer to address
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetSourceAddress(THYONEI *self, uint32_t *sourceAddressP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS,
                       (uint8_t *)sourceAddressP, &length);
}

/**
 * @brief  Set destination address on the Thyone module
 * @param  self Pointer to the Thyone object.
 * @param  destinationAddress Address to set
 * @retval true if successful false in case of failure
 */
bool ThyoneI_SetDestinationAddress(THYONEI *self, uint32_t destinationAddress)
{
    return ThyoneI_Set(self, ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS,
                       (uint8_t *)&destinationAddress, 4);
}

/**
 * @brief  Get source address on the Thyone module
 * @param  self Pointer to the Thyone object.
 * @param  destinationAddressP Pointer to address
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetDestinationAddress(THYONEI *self,
                                   uint32_t *destinationAddressP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS,
                       (uint8_t *)destinationAddressP, &length);
}

/**
 * @brief  Set transmit power
 * @param  self Pointer to the Thyone object.
 * @param  txPower Transmit power
 * @retval true if successful false in case of failure
 */
bool ThyoneI_SetTXPower(THYONEI *self, ThyoneI_TXPower_t txPower)
{
    return ThyoneI_Set(self, ThyoneI_USERSETTING_INDEX_RF_TX_POWER,
                       (uint8_t *)&txPower, 1);
}

/**
 * @brief  Get transmit power
 * @param  self Pointer to the Thyone object.
 * @param  txPowerP Pointer to transmit power
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetTXPower(THYONEI *self, ThyoneI_TXPower_t *txpowerP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_RF_TX_POWER,
                       (uint8_t *)txpowerP, &length);
}

/**
 * @brief  Set RF channel
 * @param  self Pointer to the Thyone object.
 * @param  channel RF channel
 * @retval true if successful false in case of failure
 */
bool ThyoneI_SetRFChannel(THYONEI *self, uint8_t channel)
{
    /* permissible value for channel: 0-38*/
    if (channel < 38)
    {
        return ThyoneI_Set(self, ThyoneI_USERSETTING_INDEX_RF_CHANNEL,
                           (uint8_t *)&channel, 1);
    }
    else
    {
        return false;
    }
}

/**
 * @brief  Get RF channel
 * @param  self Pointer to the Thyone object.
 * @param  channelP Pointer to RF channel
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetRFChannel(THYONEI *self, uint8_t *channelP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_RF_CHANNEL,
                       (uint8_t *)channelP, &length);
}

/**
 * @brief  Get RF profile
 * @param  self Pointer to the Thyone object.
 * @param  channelP Pointer to RF profile
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetRFProfile(THYONEI *self, uint8_t *profileP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_RF_PROFILE,
                       (uint8_t *)profileP, &length);
}

/**
 * @brief  Set RF profile
 * @param  self Pointer to the Thyone object.
 * @param  profile RF profile
 * @retval true if successful false in case of failure
 */
bool ThyoneI_SetRFProfile(THYONEI *self, uint8_t profile)
{
    /* permissible value for channel: 0-38*/
    if (profile < 3)
    {
        return ThyoneI_Set(self, ThyoneI_USERSETTING_INDEX_RF_PROFILE,
                           (uint8_t *)&profile, 1);
    }
    else
    {
        return false;
    }
}
/**
 * @brief  Get Serial number
 * @param  self Pointer to the Thyone object.
 * @param  serialNumberP  Pointer to serial number
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetSerialNumber(THYONEI *self, uint8_t *serialNumberP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER,
                       serialNumberP, &length);
}

/**
 * @brief  Get firmware version
 * @param  self Pointer to the Thyone object.
 * @param  version  Pointer to serial number
 * @retval true if successful false in case of failure
 */
bool ThyoneI_GetFirmwareVersion(THYONEI *self, uint8_t *versionP)
{
    uint16_t length;
    return ThyoneI_Get(self, ThyoneI_USERSETTING_INDEX_FW_VERSION, versionP,
                       &length);
}
/**
 * @brief  Factory reset Thyone
 * @param  self Pointer to the Thyone object.
 * @retval true if successful false in case of failure
 */
bool ThyoneI_FactoryReset(THYONEI *self)
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_FACTORYRESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for reset after factory reset */
        return THYONEI_waitForReply(self, ThyoneI_CMD_START_IND,
                                    CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief  Thyone set parameter
 * @param  self Pointer to the Thyone object.
 * @param  userSetting Paramenter to set
 * @param  valueP Pointer to the value
 * @param  length Length of the value
 * @retval true if successful false in case of failure
 */
bool ThyoneI_Set(THYONEI *self, ThyoneI_UserSettings_t userSetting,
                 uint8_t *ValueP, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_SET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(1 + length);
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], ValueP, length);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return THYONEI_waitForReply(self, ThyoneI_CMD_START_IND,
                                    CMD_Status_Success, true);
    }
    return ret;
}
/**
 * @brief  Thyone get parameter
 * @param  self Pointer to the Thyone object.
 * @param  userSetting Paramenter to set
 * @param  ResponseP Pointer to the value
 * @param  Response_LengthP Pointer to the length of the value
 * @retval true if successful false in case of failure
 */
bool ThyoneI_Get(THYONEI *self, ThyoneI_UserSettings_t userSetting,
                 uint8_t *ResponseP, uint16_t *Response_LengthP)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        THYONEI_sendBytes(self, CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        if (THYONEI_waitForReply(self, ThyoneI_CMD_GET_CNF, CMD_Status_Success,
                                 true))
        {
            if (self->bufferThyone.length != 0)
            {
                memcpy(ResponseP, &self->bufferThyone.data[0],
                       self->bufferThyone.length); /* First Data byte is status,
                                                      following bytes response*/
                *Response_LengthP = self->bufferThyone.length;
                ret = true;
            }
        }
    }
    return ret;
}

/**
 * @brief  Thyone send data over Thyone serial port
 * @param  self Pointer to the Thyone object.
 * @param  data Pointer to the bytes to send
 * @param  dataLength Length of the data
 * @retval none
 */
void THYONEI_sendBytes(THYONEI *self, const char *data, int dataLength)
{
#if SERIAL_DEBUG
    // SSerial_printf(self->serialDebug, "Sending to ThyoneI: ");
    // THYONEI_printPacket(self, (uint8_t *)data, dataLength);
    // SSerial_printf(self->serialDebug, "\r\n");
#endif

    int written = 1;

    while (written)
    {
        while (HSerial_available(self->serialThyoneI))
        {
            HSerial_read(self->serialThyoneI);
        }
        if (dataLength <= HSerial_availableForWrite(self->serialThyoneI))
        {
            HSerial_writeB(self->serialThyoneI, data, dataLength);
            written = 0;
        }
    }
}

/**
 * @brief  Receive data from the Thyoneserial port
 * @param  self Pointer to the Thyone object.
 * @retval none
 */
void THYONEI_receiveBytes(THYONEI *self)
{
    uint8_t checksum = 0;
    uint8_t readBuffer;

    while (HSerial_available(self->serialThyoneI))
    {
        readBuffer = (uint8_t)HSerial_read(self->serialThyoneI);

#if SERIAL_DEBUG
        // SSerial_printf(self->serialDebug, "rx[%i] %02X \r\n", RxByteCounter,
        // readBuffer);
#endif
        /* interpret received byte */
        pRxBuffer[RxByteCounter] = readBuffer;
        switch (RxByteCounter)
        {
        case 0:
            /* wait for start byte of frame */
            if (pRxBuffer[RxByteCounter] == CMD_STX)
            {
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
            if (RxByteCounter == BytesToReceive)
            {
                /* check CRC */
                checksum = 0;
                int i = 0;
                for (i = 0; i < (BytesToReceive - 1); i++)
                {
                    checksum ^= pRxBuffer[i];
                }

                if (checksum == pRxBuffer[BytesToReceive - 1])
                {
                    /* received frame ok, interpret it now */
                    HandleRxPacket(self, pRxBuffer);
                }
                RxByteCounter = 0;
                packetReceived = true;
                return;
            }
            break;
        }
    }
}

/**
 * @brief  Parse the received packet
 * @param  self Pointer to the Thyone object.
 * @param  data Pointer to the received packet
 * @retval true if successful false in case of failure
 */
void HandleRxPacket(THYONEI *self, uint8_t *pRxBuffer)
{
    CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

#if SERIAL_DEBUG
    // uint16_t cmd_length = (uint16_t)(pRxBuffer[CMD_POSITION_LENGTH_LSB] +
    //                                  (pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8));
    // THYONEI_printPacket(self,
    //                     pRxBuffer, cmd_length + LENGTH_CMD_OVERHEAD);
#endif
    switch (pRxBuffer[CMD_POSITION_CMD])
    {
    case ThyoneI_CMD_RESET_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }

    case ThyoneI_CMD_DATA_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GET_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        int length = ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) +
                     ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8);
        memcpy(
            &self->bufferThyone.data[0], &pRxBuffer[CMD_POSITION_DATA + 1],
            length -
                1); /* First Data byte is status, following bytes response*/
        self->bufferThyone.length = length - 1;
        break;
    }

    case ThyoneI_CMD_SET_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GETSTATE_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }

    case ThyoneI_CMD_FACTORYRESET_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_SLEEP_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_LOCAL_READ_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];

        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_READ_CNF:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_Invalid;

        break;
    }

    case ThyoneI_CMD_START_IND:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }

    case ThyoneI_CMD_DATA_IND:
    {
        uint16_t payload_length =
            ((((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) |
              ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8)));
        uint32_t src = (uint32_t)pRxBuffer[CMD_POSITION_DATA + 3];
        src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA + 2];
        src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA + 1];
        src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA];
        self->bufferThyone.sourceAddress = src;
        self->bufferThyone.length = payload_length - 5;
        self->bufferThyone.RSSI =
            (int)(pRxBuffer[CMD_POSITION_DATA + 4] - 255);
        /*Copy payload to the thyone buffer*/
        memcpy(&self->bufferThyone.data[0],
               pRxBuffer + CMD_POSITION_DATA + 5, payload_length - 5);
        break;
    }

    case ThyoneI_CMD_TXCOMPLETE_RSP:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }

    case ThyoneI_CMD_GPIO_REMOTE_READ_RSP:
    {
        cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }
    default:
    {
        /* invalid*/
        break;
    }
    }

    int i = 0;
    for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        if (cmdConfirmation_array[i].cmd == CNFINVALID)
        {
            cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
            cmdConfirmation_array[i].status = cmdConfirmation.status;
            break;
        }
    }
}
/**
 * @brief  Wait for response from Thyone
 * @param  self Pointer to the Thyone object.
 * @param  expectedCmdConfirmation expected confirmation
 * @param  expectedStatus expected status
 * @param  reset_confirmstate reset status
 * @retval true if successful false in case of failure
 */
bool THYONEI_waitForReply(THYONEI *self, uint8_t expectedCmdConfirmation,
                          CMD_Status_t expectedStatus,
                          bool reset_confirmstate)
{
    unsigned long startTime = micros();
    unsigned long interval = 0;
    self->bufferThyone.length = 0;
    packetReceived = false;
    RxByteCounter = 0;
    if (reset_confirmstate)
    {
        for (int i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }

    while (1)
    {
        interval = micros() - startTime;
        THYONEI_receiveBytes(self);
        if ((interval) >= (TIMEOUT * 1000)) /*ms to microseconds*/
        {
            break;
        }
        for (int i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd)
            {
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }
    }
    return false;
}

/**
 * @brief  Thyone print packet
 * @param  self Pointer to the Thyone object.
 * @param  data Pointer to the packet
 * @param  dataLength Length of the packet
 * @retval none
 */
void THYONEI_printPacket(THYONEI *self, uint8_t *packet, int length)
{
    for (int idx = 0; idx < length; idx++)
    {
        SSerial_printf(self->serialDebug, "%02X", packet[idx]);
    }
    SSerial_printf(self->serialDebug, "\r\n");
}

/**
 * @brief  Fill checksum
 * @param  pArray Pointer to the packet
 * @param  dataLength Length of the packet
 * @retval none
 */
bool FillChecksum(char *pArray, uint16_t length)
{
    bool ret = false;

    if ((length >= LENGTH_CMD_OVERHEAD) && (pArray[0] == CMD_STX))
    {
        uint8_t checksum = (uint8_t)0;
        uint16_t payload_length =
            (uint16_t)(pArray[CMD_POSITION_LENGTH_MSB] << 8) +
            pArray[CMD_POSITION_LENGTH_LSB];
        uint16_t i = 0;
        for (i = 0; i < (payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC);
             i++)
        {
            checksum ^= pArray[i];
        }
        pArray[payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC] = checksum;
        ret = true;
    }
    return ret;
}
