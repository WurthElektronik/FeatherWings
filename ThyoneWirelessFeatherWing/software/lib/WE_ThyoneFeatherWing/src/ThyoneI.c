/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 *THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 *KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 *INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 *INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 *REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 *PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT
 *RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED.
 *INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS
 *OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE
 *SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Thyone-I driver source file.
 */

#include "ThyoneI.h"

#include <stdio.h>
#include <string.h>

#include "../global/global.h"

#define CMD_WAIT_TIME 1500
#define CMD_WAIT_TIME_STEP_MS 0
#define CNFINVALID 255

/* Normal overhead: Start signal + Command + Length + CS = 1+1+2+1=5 bytes */
#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)

#define MAX_RADIO_PAYLOAD_LENGTH (uint16_t)224
#define MAX_RADIO_PAYLOAD_LENGTH_MULTICAST_EX (uint16_t)223
#define MAX_RADIO_PAYLOAD_LENGTH_UNICAST_EX (uint16_t)220

#define MAX_CMD_PAYLOAD_LENGTH (uint16_t)(MAX_RADIO_PAYLOAD_LENGTH + 6)

typedef struct {
    uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ThyoneI_CMD_Frame_t;

#define CMD_STX 0x02

#define THYONEI_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define THYONEI_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define THYONEI_CMD_TYPE_IND (uint8_t)(2 << 6)
#define THYONEI_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define THYONEI_CMD_RESET (uint8_t)0x00
#define THYONEI_CMD_RESET_REQ (THYONEI_CMD_RESET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_RESET_CNF (THYONEI_CMD_RESET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GETSTATE (uint8_t)0x01
#define THYONEI_CMD_GETSTATE_REQ (THYONEI_CMD_GETSTATE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GETSTATE_CNF (THYONEI_CMD_GETSTATE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_SLEEP (uint8_t)0x02
#define THYONEI_CMD_SLEEP_REQ (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SLEEP_CNF (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_SLEEP_IND (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_IND)

#define THYONEI_CMD_START_IND (uint8_t)0x73

#define THYONEI_CMD_UNICAST_DATA (uint8_t)0x04
#define THYONEI_CMD_UNICAST_DATA_REQ \
    (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_REQ)

/* Transmissions of any kind will be confirmed and indicated by the same message
 * CMD_DATA_CNF or CMD_DATA_IND */
#define THYONEI_CMD_DATA_CNF (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_DATA_IND (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_IND)
#define THYONEI_CMD_TXCOMPLETE_RSP \
    (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_RSP)

#define THYONEI_CMD_MULTICAST_DATA (uint8_t)0x05
#define THYONEI_CMD_MULTICAST_DATA_REQ \
    (THYONEI_CMD_MULTICAST_DATA | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_BROADCAST_DATA (uint8_t)0x06
#define THYONEI_CMD_BROADCAST_DATA_REQ \
    (THYONEI_CMD_BROADCAST_DATA | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_UNICAST_DATA_EX (uint8_t)0x07
#define THYONEI_CMD_UNICAST_DATA_EX_REQ \
    (THYONEI_CMD_UNICAST_DATA_EX | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_MULTICAST_DATA_EX (uint8_t)0x08
#define THYONEI_CMD_MULTICAST_DATA_EX_REQ \
    (THYONEI_CMD_MULTICAST_DATA_EX | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_SNIFFER_IND (uint8_t)0x99

#define THYONEI_CMD_SETCHANNEL (uint8_t)0x09
#define THYONEI_CMD_SETCHANNEL_REQ \
    (THYONEI_CMD_SETCHANNEL | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SETCHANNEL_CNF \
    (THYONEI_CMD_SETCHANNEL | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GET (uint8_t)0x10
#define THYONEI_CMD_GET_REQ (THYONEI_CMD_GET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GET_CNF (THYONEI_CMD_GET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_SET (uint8_t)0x11
#define THYONEI_CMD_SET_REQ (THYONEI_CMD_SET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SET_CNF (THYONEI_CMD_SET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_FACTORYRESET (uint8_t)0x1C
#define THYONEI_CMD_FACTORYRESET_REQ \
    (THYONEI_CMD_FACTORYRESET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_FACTORYRESET_CNF \
    (THYONEI_CMD_FACTORYRESET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG (uint8_t)0x25
#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG_REQ \
    (THYONEI_CMD_GPIO_LOCAL_SETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF \
    (THYONEI_CMD_GPIO_LOCAL_SETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG (uint8_t)0x26
#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG_REQ \
    (THYONEI_CMD_GPIO_LOCAL_GETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF \
    (THYONEI_CMD_GPIO_LOCAL_GETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_WRITE (uint8_t)0x27
#define THYONEI_CMD_GPIO_LOCAL_WRITE_REQ \
    (THYONEI_CMD_GPIO_LOCAL_WRITE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_WRITE_CNF \
    (THYONEI_CMD_GPIO_LOCAL_WRITE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_READ (uint8_t)0x28
#define THYONEI_CMD_GPIO_LOCAL_READ_REQ \
    (THYONEI_CMD_GPIO_LOCAL_READ | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_READ_CNF \
    (THYONEI_CMD_GPIO_LOCAL_READ | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG (uint8_t)0x29
#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG_REQ \
    (THYONEI_CMD_GPIO_REMOTE_SETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF \
    (THYONEI_CMD_GPIO_REMOTE_SETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG (uint8_t)0x2A
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_REQ \
    (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_CNF \
    (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP \
    (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_RSP)

#define THYONEI_CMD_GPIO_REMOTE_WRITE (uint8_t)0x2B
#define THYONEI_CMD_GPIO_REMOTE_WRITE_REQ \
    (THYONEI_CMD_GPIO_REMOTE_WRITE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_WRITE_CNF \
    (THYONEI_CMD_GPIO_REMOTE_WRITE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_READ (uint8_t)0x2C
#define THYONEI_CMD_GPIO_REMOTE_READ_REQ \
    (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_READ_CNF \
    (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_GPIO_REMOTE_READ_RSP \
    (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_RSP)

/**
 * @brief Type used to check the response, when a command was sent to the
 * ThyoneI
 */
typedef enum ThyoneI_CMD_Status_t {
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ThyoneI_CMD_Status_t;

typedef struct {
    uint8_t cmd; /* variable to check if correct CMD has been confirmed */
    ThyoneI_CMD_Status_t
        status; /* variable used to check the response (*_CNF), when a request
                   (*_REQ) was sent to the ThyoneI */
} ThyoneI_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ThyoneI_CMD_Frame_t txPacket = {
    .Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ThyoneI_CMD_Frame_t rxPacket = {
    .Stx = CMD_STX,
    .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 3
static ThyoneI_CMD_Confirmation_t
    cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static ThyoneI_Pins_t *ThyoneI_pinsP = NULL;

static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t
    rxBuffer[sizeof(ThyoneI_CMD_Frame_t)]; /* For UART RX from module */
void (*RxCallback)(uint8_t *, uint16_t, uint32_t,
                   int8_t); /* callback function */
static ThyoneI_OperationMode_t operationMode =
    ThyoneI_OperationMode_CommandMode;

/**************************************
 *         Static functions           *
 **************************************/

static void HandleRxPacket(uint8_t *prxBuffer) {
    ThyoneI_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ThyoneI_CMD_Frame_t *)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

#ifdef WE_DEBUG
    WE_DEBUG_PRINT("< %02X%02X%02X%02X", rxPacket.Stx, rxPacket.Cmd,
                   (uint8_t)(0x00FF & rxPacket.Length),
                   (uint8_t)((0xFF00 & rxPacket.Length) >> 8));
    for (uint16_t i = 0; i < rxPacket.Length; i++) {
        WE_DEBUG_PRINT("%02X", rxPacket.Data[i]);
    }
    WE_DEBUG_PRINT("\r\n");
#endif

    switch (rxPacket.Cmd) {
        case THYONEI_CMD_RESET_CNF:
        case THYONEI_CMD_START_IND:
        case THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP:
        case THYONEI_CMD_GPIO_REMOTE_READ_RSP: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case THYONEI_CMD_DATA_CNF:
        case THYONEI_CMD_GET_CNF:
        case THYONEI_CMD_SET_CNF:
        case THYONEI_CMD_SETCHANNEL_CNF:
        case THYONEI_CMD_FACTORYRESET_CNF:
        case THYONEI_CMD_SLEEP_CNF:
        case THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF:
        case THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF:
        case THYONEI_CMD_GPIO_LOCAL_WRITE_CNF:
        case THYONEI_CMD_GPIO_LOCAL_READ_CNF:
        case THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF:
        case THYONEI_CMD_GPIO_REMOTE_GETCONFIG_CNF:
        case THYONEI_CMD_GPIO_REMOTE_WRITE_CNF:
        case THYONEI_CMD_TXCOMPLETE_RSP:
        case THYONEI_CMD_GETSTATE_CNF: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case THYONEI_CMD_GPIO_REMOTE_READ_CNF: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = CMD_Status_Invalid;

            break;
        }

        case THYONEI_CMD_DATA_IND: {
            if (RxCallback != NULL) {
                uint32_t src_address;
                memcpy(&src_address, &rxPacket.Data[0], 4);
                RxCallback(&rxPacket.Data[5], rxPacket.Length - 5, src_address,
                           rxPacket.Data[4]);
            }
            break;
        }

        case THYONEI_CMD_SNIFFER_IND: {
            if (RxCallback != NULL) {
                uint32_t src_address;
                memcpy(&src_address, &rxPacket.Data[0], 4);
                RxCallback(&rxPacket.Data[6], rxPacket.Length - 6, src_address,
                           rxPacket.Data[4]);
            }
            break;
        }

        default: {
            /* invalid*/
            break;
        }
    }

    for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        if (cmdConfirmation_array[i].cmd == CNFINVALID) {
            cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
            cmdConfirmation_array[i].status = cmdConfirmation.status;
            break;
        }
    }
}

/**
 * @brief Function that waits for the return value of ThyoneI (*_CNF), when a
 * command (*_REQ) was sent before
 */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation,
                     ThyoneI_CMD_Status_t expectedStatus,
                     bool reset_confirmstate) {
    uint32_t t0 = WE_GetTick();

    if (reset_confirmstate) {
        for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }
    while (1) {
        for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd) {
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > max_time_ms) {
            /* received no correct response within timeout */
            return false;
        }

        if (CMD_WAIT_TIME_STEP_MS > 0) {
            /* wait */
            WE_Delay(CMD_WAIT_TIME_STEP_MS);
        }
    }
    return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(ThyoneI_CMD_Frame_t *cmd) {
    if ((cmd->Length >= 0) && (cmd->Stx == CMD_STX)) {
        uint8_t checksum = (uint8_t)0;
        uint8_t *pArray = (uint8_t *)cmd;
        uint16_t i = 0;
        for (i = 0; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++) {
            checksum ^= pArray[i];
        }
        cmd->Data[cmd->Length] = checksum;
        return true;
    }
    return false;
}

/**
 * @brief Sends raw data to Thyone via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending commands to ProteusIII.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 */
static void ThyoneI_UART_Transmit(uint8_t *data, uint16_t dataLength) {
#ifdef WE_DEBUG
    WE_DEBUG_PRINT("> ");
    for (uint16_t i = 0; i < dataLength; i++) {
        WE_DEBUG_PRINT("%02X", data[i]);
    }
    WE_DEBUG_PRINT("\r\n");
#endif
    WE_UART_RXPin11_TXPin10_Transmit((const char *)data, dataLength);
}

/**************************************
 *         Global functions           *
 **************************************/

void WE_UART_RXPin11_TXPin10_HandleRxByte(uint8_t receivedByte) {
    if (rxByteCounter < sizeof(rxBuffer)) {
        rxBuffer[rxByteCounter] = receivedByte;
    }

    switch (rxByteCounter) {
        case 0:
            /* wait for start byte of frame */
            if (rxBuffer[rxByteCounter] == CMD_STX) {
                bytesToReceive = 0;
                rxByteCounter = 1;
            }
            break;

        case 1:
            /* CMD */
            rxByteCounter++;
            break;

        case 2:
            /* length field LSB */
            rxByteCounter++;
            bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
            break;

        case 3:
            /* length field MSB */
            rxByteCounter++;
            bytesToReceive +=
                (((uint16_t)rxBuffer[rxByteCounter - 1] << 8) +
                 LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd + cmd */
            break;

        default:
            /* data field */
            rxByteCounter++;
            if (rxByteCounter >= bytesToReceive) {
                /* check CRC */
                checksum = 0;
                for (uint16_t i = 0; i < (bytesToReceive - 1); i++) {
                    checksum ^= rxBuffer[i];
                }

                if (checksum == rxBuffer[bytesToReceive - 1]) {
                    /* received frame ok, interpret it now */
                    HandleRxPacket(rxBuffer);
                }

                rxByteCounter = 0;
                bytesToReceive = 0;
            }
            break;
    }
}

/**
 * @brief Initialize the ThyoneI interface for serial interface
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the
 * ThyoneI. The baudrate parameter must match to perform a successful FTDI
 * communication. Updating this parameter during runtime may lead to
 * communication errors.
 *
 * @param[in] pinout:           definition of the gpios connected to the module
 * @param[in] baudrate:       baudrate of the interface
 * @param[in] flow_control:   enable/disable flowcontrol
 * @param[in] opMode:   	  operation mode
 * @param[in] RXcb:           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ThyoneI_Init(ThyoneI_Pins_t *pinoutP, uint32_t baudrate,
                  WE_FlowControl_t flow_control, ThyoneI_OperationMode_t opMode,
                  void (*RXcb)(uint8_t *, uint16_t, uint32_t, int8_t)) {
    /* set RX callback function */
    RxCallback = RXcb;

    operationMode = opMode;

    /* initialize the pins */
    if (pinoutP == NULL) {
        return false;
    }

    ThyoneI_pinsP = pinoutP;
    ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    ThyoneI_pinsP->ThyoneI_Pin_Mode.type = WE_Pin_Type_Output;

    WE_Pin_t pins[sizeof(ThyoneI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;

    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp,
           sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Mode,
           sizeof(WE_Pin_t));

    if (false == WE_InitPins(pins, pin_count)) {
        /* error */
        return false;
    }
    WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, WE_Pin_Level_High);
    WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_Mode,
              (operationMode == ThyoneI_OperationMode_TransparentMode)
                  ? WE_Pin_Level_High
                  : WE_Pin_Level_Low);

    WE_UART_RXPin11_TXPin10_Init(baudrate, flow_control, WE_Parity_None);
    WE_Delay(10);

    uint8_t fwVersion;
    ThyoneI_GetFWVersion(&fwVersion);

    /* reset module */
    if (ThyoneI_Reset()) {
        WE_Delay(THYONEI_BOOT_DURATION);
    } else {
        WE_DEBUG_PRINT("reset failed\n");
        ThyoneI_Deinit();
        return false;
    }

    WE_Delay(100);

    return true;
}

/**
 * @brief Deinitialize the ThyoneI interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ThyoneI_Deinit() {
    /* close the communication interface to the module */
    WE_UART_RXPin11_TXPin10_DeInit();

    /* deinit pins */
    WE_DeinitPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp);
    WE_DeinitPin(ThyoneI_pinsP->ThyoneI_Pin_Mode);

    RxCallback = NULL;

    return true;
}

/**
 * @brief Wakeup the ThyoneI from sleep by pin
 *
 * @return true if wakeup succeeded,
 *         false otherwise
 */
bool ThyoneI_PinWakeup() {
    WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, WE_Pin_Level_Low);
    WE_Delay(5);
    for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }
    WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, WE_Pin_Level_High);

    if (operationMode == ThyoneI_OperationMode_TransparentMode) {
        /* transparent mode is ready (the module doesn't send a "ready for
         * operation" message in transparent mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus,
                    false);
}

/**
 * @brief Reset the ThyoneI by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ThyoneI_Reset() {
    bool ret = false;

    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_RESET_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND,
                        CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief Put the ThyoneI into sleep mode
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_Sleep() {
    bool ret = false;
    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SLEEP_CNF, CMD_Status_Success,
                       true);
    }
    return ret;
}

/**
 * @brief Transmit data as broadcast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_TransmitBroadcast(uint8_t *payloadP, uint16_t length) {
    bool ret = false;
    if (length <= MAX_RADIO_PAYLOAD_LENGTH) {
        txPacket.Cmd = THYONEI_CMD_BROADCAST_DATA_REQ;
        txPacket.Length = length;

        memcpy(&txPacket.Data[0], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                                  txPacket.Length + LENGTH_CMD_OVERHEAD);
            ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP,
                           CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief Transmit data as multicast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_TransmitMulticast(uint8_t *payloadP, uint16_t length) {
    bool ret = false;
    if (length <= MAX_RADIO_PAYLOAD_LENGTH) {
        txPacket.Cmd = THYONEI_CMD_MULTICAST_DATA_REQ;
        txPacket.Length = length;

        memcpy(&txPacket.Data[0], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                                  txPacket.Length + LENGTH_CMD_OVERHEAD);
            ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP,
                           CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief Transmit data as unicast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_TransmitUnicast(uint8_t *payloadP, uint16_t length) {
    bool ret = false;
    if (length <= MAX_RADIO_PAYLOAD_LENGTH) {
        txPacket.Cmd = THYONEI_CMD_UNICAST_DATA_REQ;
        txPacket.Length = length;

        memcpy(&txPacket.Data[0], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                                  txPacket.Length + LENGTH_CMD_OVERHEAD);
            ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP,
                           CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief Transmit data as multicast
 *
 * @param[in] groupID : groupID to multicast
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_TransmitMulticastExtended(uint8_t groupID, uint8_t *payloadP,
                                       uint16_t length) {
    bool ret = false;
    if (length <= MAX_RADIO_PAYLOAD_LENGTH_MULTICAST_EX) {
        txPacket.Cmd = THYONEI_CMD_MULTICAST_DATA_EX_REQ;
        txPacket.Length = length + 1;
        txPacket.Data[0] = groupID;

        memcpy(&txPacket.Data[1], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                                  txPacket.Length + LENGTH_CMD_OVERHEAD);
            ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP,
                           CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief Transmit data as unicast
 *
 * @param[in] address: address to sent to
 * @param[in] payloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_TransmitUnicastExtended(uint32_t address, uint8_t *payloadP,
                                     uint16_t length) {
    bool ret = false;
    if (length <= MAX_RADIO_PAYLOAD_LENGTH_UNICAST_EX) {
        txPacket.Cmd = THYONEI_CMD_UNICAST_DATA_EX_REQ;
        txPacket.Length = length + 4;

        memcpy(&txPacket.Data[0], &address, 4);
        memcpy(&txPacket.Data[4], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                                  txPacket.Length + LENGTH_CMD_OVERHEAD);
            ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP,
                           CMD_Status_Success, true);
        }
    }

    return ret;
}

/**
 * @brief Factory reset the module
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneI_FactoryReset() {
    bool ret = false;
    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for reset after factory reset */
        return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND,
                        CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief Set a special user setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_Set(ThyoneI_UserSettings_t userSetting, uint8_t *ValueP,
                 uint8_t length) {
    bool ret = false;

    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], ValueP, length);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SET_CNF, CMD_Status_Success,
                        true);
    }
    return ret;
}

/**
 * @brief Set the tx power
 *
 * @param[in] txPower: transmit power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetTXPower(ThyoneI_TXPower_t txPower) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_TX_POWER,
                       (uint8_t *)&txPower, 1);
}

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] baudrateindex: UART baudrate index
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetBaudrateIndex(ThyoneI_BaudRateIndex_t baudrate,
                              ThyoneI_UartParity_t parity,
                              bool flowcontrolEnable) {
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flowcontrol is to be enabled UART index has to be increased by one in
     * regard to base value */
    if (flowcontrolEnable) {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of
     * base value*/
    if (ThyoneI_UartParity_Even == parity) {
        baudrateIndex += 64;
    }

    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_UART_CONFIG,
                       (uint8_t *)&baudrateIndex, 1);
}

/**
 * @brief Set the RF channel
 *
 * @param[in] channel: Radio channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetRFChannel(uint8_t channel) {
    /* permissible value for channel: 0-38 */
    if (channel <= 38) {
        return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_CHANNEL,
                           (uint8_t *)&channel, 1);
    } else {
        return false;
    }
}

/**
 * @brief Set the RF channel on-the-fly without changing the user settings
 * (volatile, channel is reverted to the value stored in flash after a reset of
 * the module).
 *
 * @param[in] channel: Radio channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetRFChannelRuntime(uint8_t channel) {
    /* permissible value for channel: 0-38 */
    if (channel <= 38) {
        txPacket.Cmd = THYONEI_CMD_SETCHANNEL_REQ;
        txPacket.Length = 1;
        txPacket.Data[0] = channel;

        if (!FillChecksum(&txPacket)) {
            return false;
        }
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);
        return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SETCHANNEL_CNF,
                        CMD_Status_Success, true);
    } else {
        return false;
    }
}

/**
 * @brief Set the encryption mode
 *
 * @param[in] encryptionMode: encryptionMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetEncryptionMode(ThyoneI_EncryptionMode_t encryptionMode) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE,
                       (uint8_t *)&encryptionMode, 1);
}

/**
 * @brief Set the rf profile
 *
 * @param[in] profile: rf profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetRfProfile(ThyoneI_Profile_t profile) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_PROFILE,
                       (uint8_t *)&profile, 1);
}

/**
 * @brief Set the number of retries
 *
 * @param[in] numRetries: number of retries
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetNumRetries(uint8_t numRetries) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES,
                       (uint8_t *)&numRetries, 1);
}

/**
 * @brief Set the number of time slots
 *
 * @param[in] numSlots: number of time slots
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetRpNumSlots(uint8_t numSlots) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS,
                       (uint8_t *)&numSlots, 1);
}

/**
 * @brief Set the source address
 *
 * @param[in] sourceAddress: source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetSourceAddress(uint32_t sourceAddress) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS,
                       (uint8_t *)&sourceAddress, 4);
}

/**
 * @brief Set the destination address
 *
 * @param[in] destinationAddress: destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetDestinationAddress(uint32_t destinationAddress) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS,
                       (uint8_t *)&destinationAddress, 4);
}

/**
 * @brief Set the group id
 *
 * @param[in] groupid: groupID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetGroupID(uint8_t groupId) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID,
                       (uint8_t *)&groupId, 1);
}

/**
 * @brief Set the encryption key
 *
 * @param[in] keyP: pointer to 16-byte key
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetEncryptionKey(uint8_t *keyP) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY, keyP, 16);
}

/**
 * @brief Set the time-to-live defining the maximum of hops if repeating
 *
 * @param[in] ttl: time-to-live value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetTimeToLive(uint8_t ttl) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_TLL, &ttl, 1);
}

/**
 * @brief Set clear channel assessement mode
 *
 * @param[in] ccaMode: clear channel assessment mode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetCCAMode(uint8_t ccaMode) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_MODE, &ccaMode, 1);
}

/**
 * @brief Set threshold for clear channel assessement
 *
 * @param[in] ccaThreshold: threshold for clear channel assessement
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetCCAThreshold(uint8_t ccaThreshold) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, &ccaThreshold,
                       1);
}

/**
 * @brief Set remote gpio config
 *
 * @param[in] remoteConfig: remote gpio config
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetGPIOBlockRemoteConfig(uint8_t remoteConfig) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG,
                       &remoteConfig, 1);
}

/**
 * @brief Set module mode
 *
 * @param[in] moduleMode: operation mode of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_SetModuleMode(ThyoneI_ModuleMode_t moduleMode) {
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MODULE_MODE,
                       (uint8_t *)&moduleMode, 1);
}

/**
 * @brief Request the current user settings
 *
 * @param[in] userSetting: user setting to be requested
 * @param[out] responseP: pointer of the memory to put the requested content
 * @param[out] response_lengthP: length of the requested content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_Get(ThyoneI_UserSettings_t userSetting, uint8_t *ResponseP,
                 uint16_t *Response_LengthP) {
    bool ret = false;

    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GET_CNF, CMD_Status_Success,
                     true)) {
            uint16_t length = rxPacket.Length;
            memcpy(
                ResponseP, &rxPacket.Data[1],
                length -
                    1); /* First Data byte is status, following bytes response*/
            *Response_LengthP = length - 1;
            ret = true;
        }
    }
    return ret;
}

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] versionP: pointer to the 4 byte serial number
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetSerialNumber(uint8_t *serialNumberP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER, serialNumberP,
                       &length);
}

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] versionP: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetFWVersion(uint8_t *versionP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_FW_VERSION, versionP, &length);
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetTXPower(ThyoneI_TXPower_t *txpowerP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_TX_POWER,
                       (uint8_t *)txpowerP, &length);
}

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateIndexP: pointer to the UART baudrate index
 * @param[out] parityP: pointer to the UART parity
 * @param[out] flowcontrolEnableP: pointer to the UART flow control parameter
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetBaudrateIndex(ThyoneI_BaudRateIndex_t *baudrateP,
                              ThyoneI_UartParity_t *parityP,
                              bool *flowcontrolEnableP) {
    bool ret = false;
    uint16_t length;
    uint8_t uartIndex;

    if (ThyoneI_Get(ThyoneI_USERSETTING_INDEX_UART_CONFIG,
                    (uint8_t *)&uartIndex, &length)) {
        /* if index is even, flow control is off.
         * If flow control is on, decrease index by one to later determine the
         * base baudrate */
        if (0x01 == (uartIndex & 0x01)) {
            /* odd */
            *flowcontrolEnableP = true;
            uartIndex--;
        } else {
            /* even */
            *flowcontrolEnableP = false;
        }

        /* If baudrate index is greater than or equal to 64, parity bit is
         * even*/
        if (uartIndex < 64) {
            *parityP = ThyoneI_UartParity_None;
        } else {
            *parityP = ThyoneI_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ThyoneI_BaudRateIndex_t)uartIndex;
        ret = true;
    }

    return ret;
}

/**
 * @brief Get the encryption mode
 *
 * @param[out] encryptionModeP: Pointer to encryptionMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetEncryptionMode(ThyoneI_EncryptionMode_t *encryptionModeP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE,
                       (uint8_t *)encryptionModeP, &length);
}

/**
 * @brief Get the rf profile
 *
 * *output:
 * @param[out] profileP: Pointer to rf profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetRfProfile(ThyoneI_Profile_t *profileP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_PROFILE,
                       (uint8_t *)profileP, &length);
}

/**
 * @brief Get the rf channel
 *
 * @param[out] channelP: Pointer to rf channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetRFChannel(uint8_t *channelP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_CHANNEL,
                       (uint8_t *)channelP, &length);
}

/**
 * @brief Get the number of retries
 *
 * @param[out] numRetriesP: Pointer to number of retries
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetNumRetries(uint8_t *numRetriesP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES, numRetriesP,
                       &length);
}

/**
 * @brief Get the number of time slots
 *
 * @param[out] numSlotsP: pointer of number of time slots
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetRpNumSlots(uint8_t *numSlotsP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS, numSlotsP,
                       &length);
}

/**
 * @brief Get the source address
 *
 * @param[out] sourceAddressP: pointer to source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetSourceAddress(uint32_t *sourceAddressP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS,
                       (uint8_t *)sourceAddressP, &length);
}

/**
 * @brief Get the destination address
 *
 * @param[out] destinationAddressP: pointer to destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetDestinationAddress(uint32_t *destinationAddressP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS,
                       (uint8_t *)destinationAddressP, &length);
}

/**
 * @brief Get the group id
 *
 * @param[out] groupIdP: pointer to groupID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetGroupID(uint8_t *groupIdP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID, groupIdP,
                       &length);
}

/**
 * @brief Get the time-to-live defining the maximum of hops if repeating
 *
 * @param[out] ttlP: pointer to time-to-live
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetTimeToLive(uint8_t *ttlP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_TLL, ttlP, &length);
}

/**
 * @brief Get clear channel assessment mode
 *
 * @param[out] ccaModeP: pointer to clear channel assessment mode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetCCAMode(uint8_t *ccaModeP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_MODE, ccaModeP, &length);
}

/**
 * @brief Get threshold for clear channel assessment
 *
 * @param[out] ccaThreshold: threshold for clear channel assessement
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetCCAThreshold(uint8_t *ccaThresholdP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, ccaThresholdP,
                       &length);
}

/**
 * @brief Get remote gpio config
 *
 * @param[out] remoteConfigP: pointer to remote gpio config
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetGPIOBlockRemoteConfig(uint8_t *remoteConfigP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG,
                       remoteConfigP, &length);
}

/**
 * @brief Get module mode
 *
 * @param[out] moduleMode: operation mode of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetModuleMode(ThyoneI_ModuleMode_t *moduleModeP) {
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MODULE_MODE,
                       (uint8_t *)moduleModeP, &length);
}

/**
 * @brief Request the module state
 *
 * @param[out] state:   current state of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GetState(ThyoneI_States_t *state) {
    bool ret = false;

    /* fill CMD_ARRAY packet */

    txPacket.Cmd = THYONEI_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);
        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GETSTATE_CNF,
                     CMD_Status_Success, true)) {
            *state = rxPacket.Data[1];
            ret = true;
        }
    }
    return ret;
}

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] number_of_configs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIOLocalSetConfig(ThyoneI_GPIOConfigBlock_t *configP,
                                uint16_t number_of_configs) {
    bool ret = false;
    uint16_t length = 0;

    for (uint16_t i = 0; i < number_of_configs; i++) {
        switch (configP->function) {
            case ThyoneI_GPIO_IO_Disconnected: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_Input: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_Output: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_PWM: {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period,
                       2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
            } break;
            default: {
            } break;
        }
        configP++;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_SETCONFIG_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] number_of_configsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIOLocalGetConfig(ThyoneI_GPIOConfigBlock_t *configP,
                                uint16_t *number_of_configsP) {
    bool ret = false;

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_GETCONFIG_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF,
                       CMD_Status_Success, true);

        if (ret == true) {
            uint16_t length = rxPacket.Length;

            *number_of_configsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ThyoneI_GPIOConfigBlock_t *configP_running = configP;
            while (uartP < &rxPacket.Data[length]) {
                switch (*(uartP + 2)) {
                    case ThyoneI_GPIO_IO_Disconnected: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_Input: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.input = *(uartP + 3);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_Output: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.output = *(uartP + 3);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_PWM: {
                        if (*uartP == 5) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            memcpy(&configP_running->value.pwm.period,
                                   (uartP + 3), 2);
                            configP_running->value.pwm.ratio = *(uartP + 5);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    default: {
                    } break;
                }
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
}

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ThyoneI_GPIOLocalWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIOLocalWrite(ThyoneI_GPIOControlBlock_t *controlP,
                            uint16_t number_of_controls) {
    bool ret = false;
    uint16_t length = 0;

    for (uint16_t i = 0; i < number_of_controls; i++) {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->GPIO_ID;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;
        controlP++;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_WRITE_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ThyoneI_GPIOLocalWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of
 * GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP
 * array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIOLocalRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead,
                           ThyoneI_GPIOControlBlock_t *controlP,
                           uint16_t *number_of_controlsP) {
    bool ret = false;

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_READ_CNF,
                       CMD_Status_Success, true);

        if (ret) {
            uint16_t length = rxPacket.Length;

            *number_of_controlsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ThyoneI_GPIOControlBlock_t *controlP_running = controlP;
            while (uartP < &rxPacket.Data[length]) {
                if (*uartP == 2) {
                    controlP_running->GPIO_ID = *(uartP + 1);
                    controlP_running->value.output = *(uartP + 2);

                    controlP_running++;
                    *number_of_controlsP += 1;
                }
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
}

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] number_of_configs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIORemoteSetConfig(uint32_t destAddress,
                                 ThyoneI_GPIOConfigBlock_t *configP,
                                 uint16_t number_of_configs) {
    bool ret = false;
    uint16_t length = 4;

    for (uint16_t i = 0; i < number_of_configs; i++) {
        switch (configP->function) {
            case ThyoneI_GPIO_IO_Disconnected: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_Input: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_Output: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
            } break;
            case ThyoneI_GPIO_IO_PWM: {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period,
                       2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
            } break;
            default: {
            } break;
        }
        configP++;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_SETCONFIG_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], &destAddress, 4);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] destAddress: Destination address of the remote Thyone-I device
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] number_of_configsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIORemoteGetConfig(uint32_t destAddress,
                                 ThyoneI_GPIOConfigBlock_t *configP,
                                 uint16_t *number_of_configsP) {
    bool ret = false;

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_GETCONFIG_REQ;
    txPacket.Length = 4;
    memcpy(&txPacket.Data[0], &destAddress, 4);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP,
                       CMD_Status_NoStatus, true);

        if (ret) {
            uint16_t length = rxPacket.Length;

            *number_of_configsP = 0;
            uint8_t *uartP = &rxPacket.Data[1 + 4];
            ThyoneI_GPIOConfigBlock_t *configP_running = configP;
            while (uartP < &rxPacket.Data[length]) {
                switch (*(uartP + 2)) {
                    case ThyoneI_GPIO_IO_Disconnected: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_Input: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.input = *(uartP + 3);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_Output: {
                        if (*uartP == 3) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.output = *(uartP + 3);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    case ThyoneI_GPIO_IO_PWM: {
                        if (*uartP == 5) {
                            configP_running->GPIO_ID = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            memcpy(&configP_running->value.pwm.period,
                                   (uartP + 3), 2);
                            configP_running->value.pwm.ratio = *(uartP + 5);

                            configP_running++;
                            *number_of_configsP += 1;
                        }
                    } break;
                    default: {
                    } break;
                }
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
}

/**
 * @brief Set the output value of the remote pin. Pin has to be configured
 * first. See ThyoneI_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIORemoteWrite(uint32_t destAddress,
                             ThyoneI_GPIOControlBlock_t *controlP,
                             uint16_t number_of_controls) {
    bool ret = false;
    uint16_t length = 4;

    for (uint16_t i = 0; i < number_of_controls; i++) {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->GPIO_ID;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;
        controlP += sizeof(ThyoneI_GPIOControlBlock_t);
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], &destAddress, 4);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_WRITE_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ThyoneI_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of
 * GPIOToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP
 * array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneI_GPIORemoteRead(uint32_t destAddress, uint8_t *GPIOToReadP,
                            uint8_t amountGPIOToRead,
                            ThyoneI_GPIOControlBlock_t *controlP,
                            uint16_t *number_of_controlsP) {
    bool ret = false;

    /* payload is 4-byte destination address + pin configuration*/
    uint16_t commandLength = 1 + amountGPIOToRead + 4;

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = commandLength;

    memcpy(&txPacket.Data[0], &destAddress, 4);
    txPacket.Data[4] = amountGPIOToRead;
    memcpy(&txPacket.Data[5], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(&txPacket)) {
        ThyoneI_UART_Transmit((uint8_t *)&txPacket,
                              txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(1000, THYONEI_CMD_GPIO_REMOTE_READ_RSP,
                       CMD_Status_NoStatus, true);

        if (ret) {
            uint16_t length = rxPacket.Length;

            *number_of_controlsP = 0;
            uint8_t *uartP = &rxPacket.Data[1 + 4];
            ThyoneI_GPIOControlBlock_t *controlP_running = controlP;
            while (uartP < &rxPacket.Data[length]) {
                if (*uartP == 2) {
                    controlP_running->GPIO_ID = *(uartP + 1);
                    controlP_running->value.output = *(uartP + 2);

                    controlP_running++;
                    *number_of_controlsP += 1;
                }
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
}
