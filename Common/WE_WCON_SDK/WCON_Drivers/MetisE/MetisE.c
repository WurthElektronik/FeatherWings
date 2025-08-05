/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Metis-E driver source file.
 */

#include <MetisE/MetisE.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CMD_FACTORY_RESET_WAIT_TIME 1500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 224
#define MAX_DATA_BUFFER 255
#define TXPOWERINVALID -128

#define CMD_STX 0x02
#define METIS_E_CMD_TYPE_REQ (0 << 6)
#define METIS_E_CMD_TYPE_CNF (1 << 6)
#define METIS_E_CMD_TYPE_IND (2 << 6)
#define METIS_E_CMD_TYPE_RSP (3 << 6)

#define METIS_E_CMD_DATAEX 0x01
#define METIS_E_CMD_DATAEX_REQ (METIS_E_CMD_DATAEX | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_DATAEX_CNF (METIS_E_CMD_DATAEX | METIS_E_CMD_TYPE_CNF)
#define METIS_E_CMD_DATAEX_IND (METIS_E_CMD_DATAEX | METIS_E_CMD_TYPE_IND)

#define METIS_E_CMD_RXSTART 0x02
#define METIS_E_CMD_RXSTART_REQ (METIS_E_CMD_RXSTART | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_RXSTART_CNF (METIS_E_CMD_RXSTART | METIS_E_CMD_TYPE_CNF)
#define METIS_E_CMD_RXSTART_IND (METIS_E_CMD_RXSTART | METIS_E_CMD_TYPE_IND)

#define METIS_E_CMD_RXSTOP 0x03
#define METIS_E_CMD_RXSTOP_REQ (METIS_E_CMD_RXSTOP | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_RXSTOP_CNF (METIS_E_CMD_RXSTOP | METIS_E_CMD_TYPE_CNF)
#define METIS_E_CMD_RXSTOP_IND (METIS_E_CMD_RXSTOP | METIS_E_CMD_TYPE_IND)

#define METIS_E_CMD_SET 0x04
#define METIS_E_CMD_SET_REQ (METIS_E_CMD_SET | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_SET_CNF (METIS_E_CMD_SET | METIS_E_CMD_TYPE_CNF)

#define METIS_E_CMD_GET 0x05
#define METIS_E_CMD_GET_REQ (METIS_E_CMD_GET | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_GET_CNF (METIS_E_CMD_GET | METIS_E_CMD_TYPE_CNF)

#define METIS_E_CMD_RESET 0x06
#define METIS_E_CMD_RESET_REQ (METIS_E_CMD_RESET | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_RESET_CNF (METIS_E_CMD_RESET | METIS_E_CMD_TYPE_CNF)
#define METIS_E_CMD_RESET_IND (METIS_E_CMD_RESET | METIS_E_CMD_TYPE_IND)

#define METIS_E_CMD_STANDBY 0x07
#define METIS_E_CMD_STANDBY_REQ (METIS_E_CMD_STANDBY | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_STANDBY_CNF (METIS_E_CMD_STANDBY | METIS_E_CMD_TYPE_CNF)
#define METIS_E_CMD_STANDBY_IND (METIS_E_CMD_STANDBY | METIS_E_CMD_TYPE_IND)

#define METIS_E_CMD_SHUTDOWN 0x08
#define METIS_E_CMD_SHUTDOWN_REQ (METIS_E_CMD_SHUTDOWN | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_SHUTDOWN_CNF (METIS_E_CMD_SHUTDOWN | METIS_E_CMD_TYPE_CNF)

#define METIS_E_CMD_FACTORY_RESET 0x10
#define METIS_E_CMD_FACTORY_RESET_REQ (METIS_E_CMD_FACTORY_RESET | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_FACTORY_RESET_CNF (METIS_E_CMD_FACTORY_RESET | METIS_E_CMD_TYPE_CNF)

#define METIS_E_CMD_SET_RAM 0x12
#define METIS_E_CMD_SET_RAM_REQ (METIS_E_CMD_SET_RAM | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_SET_RAM_CNF (METIS_E_CMD_SET_RAM | METIS_E_CMD_TYPE_CNF)

#define METIS_E_CMD_GET_RAM 0x13
#define METIS_E_CMD_GET_RAM_REQ (METIS_E_CMD_GET_RAM | METIS_E_CMD_TYPE_REQ)
#define METIS_E_CMD_GET_RAM_CNF (METIS_E_CMD_GET_RAM | METIS_E_CMD_TYPE_CNF)

/**
 * @brief Available options for command payload
 */
typedef enum MetisE_Options_t
{
    MetisE_OPTION_STATUS = 0x01,
    MetisE_OPTION_RESERVED2 = 0x02,
    MetisE_OPTION_WMBUS_RXMODE = 0x03,
    MetisE_OPTION_WMBUS_TXMODE = 0x04,
    MetisE_OPTION_WMBUS_FRAMEFORMAT = 0x05,
    MetisE_OPTION_WMBUS_DATA = 0x06,
    MetisE_OPTION_WMBUS_ROLE = 0x07,
    MetisE_OPTION_RESERVED8 = 0x08,
    MetisE_OPTION_PARAMETER_INDEX = 0x09,
    MetisE_OPTION_UART_BAUDRATE = 0x0A,
    MetisE_OPTION_TXPOWER = 0x0B,
    MetisE_OPTION_CFGFLAGS = 0x0C,
    MetisE_OPTION_RSSI = 0x0D,
    MetisE_OPTION_TIMESTAMP = 0x0E,
    MetisE_OPTION_FACTORYSETTINGS = 0x80,
    MetisE_OPTION_FWVERSION = 0x81,
    MetisE_OPTION_PRODUCT_NAME = 0x82,
    MetisE_OPTION_Invalid = 0xFF,
} MetisE_Options_t;

/**
 * @brief Type used to check the response, when a command was sent to the MetisE
 */
typedef enum MetisE_CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed = 0x01,
    CMD_Status_Cmd_Invalid = 0x02, // e.g. wrong parameters
    CMD_Status_Cmd_Not_Supported = 0x03,
    CMD_Status_Cmd_Option_Invalid = 0x04,       // e.g. invalid value for the option
    CMD_Status_Cmd_Option_Not_Supported = 0x05, // e.g. option is not supported by the command
    CMD_Status_Cmd_Not_Permitted = 0x06,        // e.g when trying to transmit while wmBus_Role is Gateway
    CMD_Status_Invalid = 0xFF,
} MetisE_CMD_Status_t;

#define LENGTH_CMD_OVERHEAD (uint16_t)5 /* STX, CMD, LENGTH, CS */
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(MAX_DATA_BUFFER + LENGTH_CMD_OVERHEAD)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[MAX_DATA_BUFFER + 1]; /* +1 for the CS */
} MetisE_CMD_Frame_t;

typedef struct
{
    uint8_t OptionByte;
    uint8_t Length;
    uint8_t* pValue;
} MetisE_CMD_Option_t;

typedef struct
{
    uint8_t cmd;                /* variable to check if correct CMD has been confirmed */
    MetisE_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the MetisE */
} MetisE_CMD_Confirmation_t;

static bool MetisE_GetSetting(MetisE_UserSettings_t us, bool getDefault, uint8_t* response, uint8_t* response_length);
static bool MetisE_SetSetting(MetisE_UserSettings_t us, bool getDefault, uint8_t* value, uint8_t length);
void MetisE_HandleRxByte(uint8_t* dataP, size_t size);
void MetisE_GetOptionPointer(MetisE_CMD_Frame_t* pCmdFrame, MetisE_Options_t option, MetisE_CMD_Option_t* pOptionOut);
static WE_UART_HandleRxByte_t byteRxCallback = MetisE_HandleRxByte;

/**************************************
 *          Static variables          *
 **************************************/
static MetisE_ReceivedData_t receivedFrame;                         /* */
static MetisE_CMD_Frame_t rxPacket;                                 /* data buffer for RX */
static MetisE_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static MetisE_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
/**
 * @brief Pin configuration struct pointer.
 */
static MetisE_Pins_t* MetisE_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* MetisE_uartP = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(MetisE_CMD_Frame_t)]; /* data buffer for RX */
static void (*RxCallback)(MetisE_ReceivedData_t);    /* callback function */

/**************************************
 *         Static functions           *
 **************************************/
/**
 * @brief Get a pointer to the specified option within the command frame
 *
 * @param[in] pCmdFrame : pointer to the command frame to search
 * @param[in] option    : option to look for in the data
 *
 * @return pointer to the requested option if present. Null otherwise
 */
void MetisE_GetOptionPointer(MetisE_CMD_Frame_t* pCmdFrame, MetisE_Options_t option, MetisE_CMD_Option_t* pOptionOut)
{
    uint8_t* currentOption = pCmdFrame->Data;
    uint8_t* endOfFrame = currentOption + pCmdFrame->Length;

    pOptionOut->OptionByte = MetisE_OPTION_Invalid;

    while (currentOption < endOfFrame)
    {
        if (option == (*currentOption))
        {
            /* option found */
            pOptionOut->OptionByte = *currentOption;
            pOptionOut->Length = *(currentOption + 1);
            pOptionOut->pValue = currentOption + 2;

            break;
        }

        /* move pointer by the the length of the current option which is
		 * 2 (option name byte, option length byte) plus the option length */
        currentOption += (*(currentOption + 1) + 2);
    }
}

/**
 * @brief Interpret the valid received UART data packet
 */
static void HandleRxPacket(uint8_t* rxBuffer)
{
    MetisE_CMD_Confirmation_t cmdConfirmation;

    uint16_t cmd_length = rxBuffer[2];
    cmd_length = ((uint16_t)rxBuffer[3] << 8) + cmd_length;
    memcpy((uint8_t*)&rxPacket, rxBuffer, cmd_length + LENGTH_CMD_OVERHEAD);

    cmdConfirmation.cmd = rxPacket.Cmd;
    cmdConfirmation.status = CMD_Status_Invalid;

    MetisE_CMD_Option_t currentOption;
    currentOption.OptionByte = MetisE_OPTION_Invalid;
    MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_STATUS, &currentOption);

    if (currentOption.OptionByte != MetisE_OPTION_Invalid)
    {
        cmdConfirmation.status = currentOption.pValue[0];
    }

    switch (rxPacket.Cmd)
    {
        case METIS_E_CMD_RESET_IND:
        case METIS_E_CMD_STANDBY_IND:
        {
            /* Commands with length 0 - they have no status option. Receiving them is always a success */
            cmdConfirmation.status = CMD_Status_Success;
        }
        break;
        case METIS_E_CMD_DATAEX_IND:
        {
            /* Indication has no status option - but receiving it */
            cmdConfirmation.status = CMD_Status_Success;

            MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_RSSI, &currentOption);
            if (currentOption.OptionByte != MetisE_OPTION_Invalid)
            {
                receivedFrame.rssi = currentOption.pValue[0];
            }

            MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_TIMESTAMP, &currentOption);
            if (currentOption.OptionByte != MetisE_OPTION_Invalid)
            {
                receivedFrame.timestamp = currentOption.pValue[0];
                receivedFrame.timestamp = (receivedFrame.timestamp << 8) + currentOption.pValue[1];
                receivedFrame.timestamp = (receivedFrame.timestamp << 8) + currentOption.pValue[2];
                receivedFrame.timestamp = (receivedFrame.timestamp << 8) + currentOption.pValue[3];
            }

            MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_WMBUS_RXMODE, &currentOption);
            receivedFrame.wmBusModeRx = currentOption.pValue[0];

            MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_WMBUS_FRAMEFORMAT, &currentOption);
            receivedFrame.wmBusModeFrameFormat = currentOption.pValue[0];

            MetisE_GetOptionPointer(&rxPacket, MetisE_OPTION_WMBUS_DATA, &currentOption);
            receivedFrame.payloadLength = currentOption.Length;
            receivedFrame.pPayload = currentOption.pValue;

            /* data received, give it to the RxCallback function */
            if (RxCallback != NULL)
            {
                RxCallback(receivedFrame);
            }
        }
        break;

        case METIS_E_CMD_GET_CNF:
        case METIS_E_CMD_FACTORY_RESET_CNF:
        case METIS_E_CMD_RESET_CNF:
        case METIS_E_CMD_SHUTDOWN_CNF:
        case METIS_E_CMD_STANDBY_CNF:
        case METIS_E_CMD_DATAEX_CNF:
        case METIS_E_CMD_RXSTART_CNF:
        case METIS_E_CMD_RXSTOP_CNF:
        case METIS_E_CMD_SET_CNF:
        default:
        {
            /* Nothing to do - data is handled in respective functions */
        }
        break;
    }

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
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
 * @brief Function that waits for the return value of MetisE (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, MetisE_CMD_Status_t expectedStatus, bool reset_confirmstate)
{
    int count = 0;
    int time_step_ms = 5; /* 5ms */
    int max_count = max_time_ms / time_step_ms;

    if (reset_confirmstate)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }

    while (1)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd)
            {
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }

        if (count >= max_count)
        {
            /* received no correct response within timeout */
            break;
        }

        /* wait */
        count++;
        WE_Delay(time_step_ms);
    }

    // if we get here, there was a timeout. Otherwise function returns from within the while-loop
    return false;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static void FillChecksum(MetisE_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;

    for (uint8_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }

    cmd->Data[cmd->Length] = checksum;
}

void MetisE_HandleRxByte(uint8_t* dataP, size_t size)
{
    for (; size > 0; size--, dataP++)
    {
        if (rxByteCounter < sizeof(rxBuffer))
        {
            rxBuffer[rxByteCounter] = *dataP;
        }
        else
        {
            rxByteCounter = 0;
        }

        switch (rxByteCounter)
        {
            case 0:
                /* wait for start byte of frame */
                if (rxBuffer[rxByteCounter] == CMD_STX)
                {
                    bytesToReceive = 0;
                    rxByteCounter = 1;
                }
                break;

            case 1:
                /* CMD */
                rxByteCounter++;
                break;

            case 2:
                /* length field */
                rxByteCounter++;
                bytesToReceive = (rxBuffer[rxByteCounter - 1] + LENGTH_CMD_OVERHEAD); /* len + crc + sfd + cmd */
                break;

            case 3:
                /* length field */
                rxByteCounter++;
                bytesToReceive = bytesToReceive + (((uint16_t)rxBuffer[rxByteCounter - 1] << 8));
                break;
            default:
                /* data field */
                rxByteCounter++;
                if (rxByteCounter >= bytesToReceive)
                {
                    /* check CRC */
                    checksum = 0;
                    for (uint8_t i = 0; i < (bytesToReceive - 1); i++)
                    {
                        checksum ^= rxBuffer[i];
                    }

                    if (checksum == rxBuffer[bytesToReceive - 1])
                    {
                        /* received frame ok, interpret it now */
                        HandleRxPacket(rxBuffer);
                    }

                    rxByteCounter = 0;
                    bytesToReceive = 0;
                }
                break;
        }
    }
}

/**************************************
 *         Global functions           *
 **************************************/
/**
 * @brief Transmitting the data via UART.
 *
 * @param[in] data    :  pointer to the data.
 * @param[in] dataLength : length of the data.
 *
 * @return true if transmission succeeded,
 *         false otherwise
 */
bool MetisE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }

    return MetisE_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Initialize the MetisE for serial interface.
 *
 * Caution: The parameter baudrate  must match the configured UserSettings of the MetisE.
 *          The baudrate parameter must match to perform a successful UART communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP :         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] RXcb:           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool MetisE_Init(WE_UART_t* uartP, MetisE_Pins_t* pinoutP, void (*RXcb)(MetisE_ReceivedData_t))
{
    /* set RX callback function */
    RxCallback = RXcb;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    MetisE_pinsP = pinoutP;
    MetisE_pinsP->MetisE_Pin_Reset.type = WE_Pin_Type_Output;
    MetisE_pinsP->MetisE_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    MetisE_pinsP->MetisE_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    MetisE_pinsP->MetisE_Pin_SleepWakeUp.initial_value.output = WE_Pin_Level_Low;
    MetisE_pinsP->MetisE_Pin_Boot.type = WE_Pin_Type_Output;
    MetisE_pinsP->MetisE_Pin_Boot.initial_value.output = WE_Pin_Level_Low;

    WE_Pin_t pins[sizeof(MetisE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_Boot, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    MetisE_uartP = uartP;
    if (!MetisE_uartP->uartInit(MetisE_uartP->baudrate, MetisE_uartP->flowControl, MetisE_uartP->parity, &byteRxCallback))
    {
        return false;
    }

    return true;
}

/**
 * @brief Deinitialize the MetisE interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool MetisE_Deinit()
{
    WE_Pin_t pins[sizeof(MetisE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &MetisE_pinsP->MetisE_Pin_Boot, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    RxCallback = NULL;

    return MetisE_uartP->uartDeinit();
}

/**
 * @brief Wakeup the MetisE from standby or shutdown by pin
 *
 * @return true if wakeup succeeded,
 *         false otherwise
 */
bool MetisE_PinWakeup()
{
    if (!WE_SetPin(MetisE_pinsP->MetisE_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    WE_Delay(5);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }

    if (!WE_SetPin(MetisE_pinsP->MetisE_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_RESET_IND, CMD_Status_Success, false);
}

/**
 * @brief Reset the MetisE by pin
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool MetisE_PinReset()
{
    if (!WE_SetPin(MetisE_pinsP->MetisE_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(MetisE_pinsP->MetisE_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_RESET_IND, CMD_Status_Success, true);
}

/**
 * @brief Reset the MetisE by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool MetisE_Reset()
{
    txPacket.Cmd = METIS_E_CMD_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_RESET_IND, CMD_Status_Success, true);
}

/**
 * @brief Factory reset the MetisE
 *
 * Note: use only in rare cases, since flash can be updated only a limited number of times
 *
 * @return true if factory reset succeeded,
 *         false otherwise
 */
bool MetisE_FactoryReset()
{
    txPacket.Cmd = METIS_E_CMD_FACTORY_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_FACTORY_RESET_WAIT_TIME, METIS_E_CMD_RESET_IND, CMD_Status_Success, true);
    ;
}

/**
 * @brief Switch the module to standby mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool MetisE_Standby()
{
    txPacket.Cmd = METIS_E_CMD_STANDBY_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_STANDBY_CNF, CMD_Status_Success, true);
    ;
}

/**
 * @brief Switch the module to shutdown mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool MetisE_Shutdown()
{
    txPacket.Cmd = METIS_E_CMD_SHUTDOWN_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_SHUTDOWN_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the current MetisE settings
 *
 * @param[in] us: user setting to be requested
 * @param[in] getDefault: Determines where default value is read from flash or current value from RAM
 * @param[in/out] response_length: for input, the available size is expected. On success, actual length of the request content will be written as output
 * @param[out] response: pointer of the memory to put the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
static bool MetisE_GetSetting(MetisE_UserSettings_t us, bool getDefault, uint8_t* response, uint8_t* response_length)
{
    if (response == NULL || response_length == NULL || *response_length == 0)
    {
        return false;
    }

    txPacket.Cmd = getDefault ? METIS_E_CMD_GET_REQ : METIS_E_CMD_GET_RAM_REQ;
    txPacket.Length = 0x03;
    txPacket.Data[0] = MetisE_OPTION_PARAMETER_INDEX;
    txPacket.Data[1] = 1;
    txPacket.Data[2] = us;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, getDefault ? METIS_E_CMD_GET_CNF : METIS_E_CMD_GET_RAM_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    MetisE_CMD_Option_t option;
    MetisE_GetOptionPointer(&rxPacket, (MetisE_Options_t)us, &option);

    if (MetisE_OPTION_Invalid == option.OptionByte)
    {
        return false;
    }

    if (*response_length < option.Length)
    {
        /* response buffer is too small */
        return false;
    }

    memcpy(response, option.pValue, option.Length);
    *response_length = option.Length;

    return true;
}

/**
 * @brief Request the current MetisE settings
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetDefault(MetisE_UserSettings_t us, uint8_t* response, uint8_t* response_length) { return MetisE_GetSetting(us, true, response, response_length); }

/**
 * @brief Set a special user setting, but checks first if the value is already ok
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] userSetting:  user setting to be updated
 * @param[in] valueP:       pointer to the new settings value
 * @param[in] length:       length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_CheckNSet(MetisE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    uint8_t current_value[length];
    uint8_t current_length = length;

    if (!MetisE_GetDefault(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return MetisE_SetDefault(userSetting, valueP, length);
}

/**
 * @brief Set a MetisE setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] setDefault: Determines where value is written to - flash or RAM
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
static bool MetisE_SetSetting(MetisE_UserSettings_t us, bool setDefault, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = setDefault ? METIS_E_CMD_SET_REQ : METIS_E_CMD_SET_RAM_REQ;
    txPacket.Length = (2 + length);
    txPacket.Data[0] = us;
    txPacket.Data[1] = length;
    memcpy(&txPacket.Data[2], valueP, length);

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, setDefault ? METIS_E_CMD_SET_CNF : METIS_E_CMD_SET_RAM_CNF, CMD_Status_Success, true);
    ;
}

/**
 * @brief Set a MetisE default setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 * @param[in] writeToFlash: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */

bool MetisE_SetDefault(MetisE_UserSettings_t us, uint8_t* value, uint8_t length) { return MetisE_SetSetting(us, true, value, length); }

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] fw: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetFirmwareVersion(uint8_t* fw)
{
    if (fw == NULL)
    {
        return false;
    }

    uint8_t help[3];
    uint8_t help_length;

    if (!MetisE_GetDefault(MetisE_USERSETTING_FWVERSION, help, &help_length))
    {
        return false;
    }

    fw[0] = help[2];
    fw[1] = help[1];
    fw[2] = help[0];

    return true;
}

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] sn: pointer to the 4 byte serial number
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetSerialNumber(uint8_t* sn)
{
    if (sn == NULL)
    {
        return false;
    }

    uint8_t help[8];
    uint8_t help_length;

    if (!MetisE_GetDefault(MetisE_USERSETTING_FACTORYSETTINGS, help, &help_length))
    {
        return false;
    }

    sn[0] = help[3];
    sn[1] = help[2];
    sn[2] = help[1];
    sn[3] = help[0];

    return true;
}

/**
 * @brief Request the default TX power
 *
 * @param[out] txpower: pointer to the TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetDefault_TXPower(uint8_t* txpower)
{
    if (txpower == NULL)
    {
        return false;
    }

    *txpower = TXPOWERINVALID;
    uint8_t length = 1;

    return MetisE_GetDefault(MetisE_USERSETTING_TXPOWER, txpower, &length);
}

/**
 * @brief Request the default wmBus RX Mode
 *
 * @param[out] wmbusRxMode: pointer to the default wmBus RxMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetDefault_wmBusRxMode(uint8_t* wmbusRxMode)
{
    if (wmbusRxMode == NULL)
    {
        return false;
    }

    *wmbusRxMode = MetisE_wmBusMode_Undefined;
    uint8_t length = 1;

    return MetisE_GetDefault(MetisE_USERSETTING_WMBUS_RXMODE, wmbusRxMode, &length);
}

/**
 * @brief Request the default wmBus RX role
 *
 * @param[out] wmbusRxRole: pointer to the default wmBus role
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetDefault_wmBusRole(uint8_t* wmbusRole)
{
    if (wmbusRole == NULL)
    {
        return false;
    }

    *wmbusRole = MetisE_wmBusRole_Undefined;
    uint8_t length = 1;

    return MetisE_GetDefault(MetisE_USERSETTING_WMBUS_ROLE, wmbusRole, &length);
}

/**
 * @brief Request the default CFG flags
 *
 * @param[out] cfgFlags: pointer to the default cfg Flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetDefault_CFGFlags(uint8_t* cfgFlags)
{
    if (cfgFlags == NULL)
    {
        return false;
    }

    *cfgFlags = 0;
    uint8_t length = 1;

    return MetisE_GetDefault(MetisE_USERSETTING_CFGFLAGS, cfgFlags, &length);
}

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetDefault_TXPower(uint8_t txpower)
{
    /* check for invalid power */
    if (txpower > 14)
    {
        /*invalid power*/
        return false;
    }

    return MetisE_SetDefault(MetisE_USERSETTING_TXPOWER, &txpower, 1);
}

/**
 * @brief Set the default Rx mode
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_wmBusRxMode for frequent adaption of the Rx mode.
 *
 * @param[in] wmbusRxMode : wmBus Rx Mode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetDefault_wmBusRxMode(uint8_t wmbusRxMode)
{
    /* check for invalid mode */
    if (MetisE_wmBusMode_MAX <= wmbusRxMode)
    {
        /*invalid mode*/
        return false;
    }

    return MetisE_SetDefault(MetisE_USERSETTING_WMBUS_RXMODE, &wmbusRxMode, 1);
}

/**
 * @brief Set the default Rx Role
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_wmBusRxRole for frequent adaption of the Rx role.
 *
 * @param[in] wmbusRxRole : wmBus Rx role
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetDefault_wmBusRole(uint8_t wmbusRxRole)
{
    /* check for invalid role */
    if (MetisE_wmBusRole_MAX <= wmbusRxRole)
    {
        /*invalid role*/
        return false;
    }

    return MetisE_SetDefault(MetisE_USERSETTING_WMBUS_ROLE, &wmbusRxRole, 1);
}

/**
 * @brief Set the default CFG flags
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_CFGFlags for frequent adaption of the CFG flags.
 *
 * @param[in] cfgFlags : configuration flags to set
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetDefault_CFGFlags(uint8_t cfgFlags)
{
    /* check for invalid cfg flags */
    if ((cfgFlags & ((uint8_t)0xFF ^ METIS_E_CFG_MASK)) != 0)
    {
        /*invalid cfg flags*/
        return false;
    }

    return MetisE_SetDefault(MetisE_USERSETTING_CFGFLAGS, &cfgFlags, 1);
}

/**
 * @brief Request the volatile TX power currently in use
 *
 * @param[out] txpower: pointer to the TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetVolatile_TXPower(uint8_t* txpower)
{
    if (txpower == NULL)
    {
        return false;
    }

    *txpower = TXPOWERINVALID;
    uint8_t length = 1;

    return MetisE_GetSetting(MetisE_USERSETTING_TXPOWER, false, txpower, &length);
}

/**
 * @brief Request the volatile wmBus RX Mode currently in use
 *
 * @param[out] wmbusRxMode: pointer to the wmBus RxMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetVolatile_wmBusRxMode(uint8_t* wmbusRxMode)
{
    if (wmbusRxMode == NULL)
    {
        return false;
    }

    *wmbusRxMode = MetisE_wmBusMode_Undefined;
    uint8_t length = 1;

    return MetisE_GetSetting(MetisE_USERSETTING_WMBUS_RXMODE, false, wmbusRxMode, &length);
}

/**
 * @brief Request the volatile wmBus role currently in use
 *
 * @param[out] wmbusRole: pointer to the wmBus role
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetVolatile_wmBusRole(uint8_t* wmbusRole)
{
    if (wmbusRole == NULL)
    {
        return false;
    }

    *wmbusRole = MetisE_wmBusRole_Undefined;
    uint8_t length = 1;

    return MetisE_GetSetting(MetisE_USERSETTING_WMBUS_ROLE, false, wmbusRole, &length);
}

/**
 * @brief Request the volatile CFG flags currently in use
 *
 * @param[out] cfgFlags: pointer to the cfg Flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_GetVolatile_CFGFlags(uint8_t* cfgFlags)
{
    if (cfgFlags == NULL)
    {
        return false;
    }

    *cfgFlags = 0;
    uint8_t length = 1;

    return MetisE_GetSetting(MetisE_USERSETTING_CFGFLAGS, false, cfgFlags, &length);
}

/**
 * @brief Set the volatile TX power to use immediately
 *
 * @param[in] txpower: TXpower to set
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetVolatile_TXPower(uint8_t txpower)
{
    /* check for invalid power */
    if (txpower > 14)
    {
        /*invalid power*/
        return false;
    }

    return MetisE_SetSetting(MetisE_USERSETTING_TXPOWER, false, &txpower, 1);
}

/**
 * @brief Set the volatile Rx mode to use immediately
 *
 * @param[in] wmbusRxMode : wmBus Rx Mode to set
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetVolatile_wmBusRxMode(uint8_t wmbusRxMode)
{
    /* check for invalid mode */
    if (MetisE_wmBusMode_MAX <= wmbusRxMode)
    {
        /*invalid mode*/
        return false;
    }

    return MetisE_SetSetting(MetisE_USERSETTING_WMBUS_RXMODE, false, &wmbusRxMode, 1);
}

/**
 * @brief Set the volatile Rx Role to use immediately
 *
 * @param[in] wmbusRxRole : wmBus Rx role to set
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetVolatile_wmBusRole(uint8_t wmbusRxRole)
{
    /* check for invalid role */
    if (MetisE_wmBusRole_MAX <= wmbusRxRole)
    {
        /*invalid role*/
        return false;
    }

    return MetisE_SetSetting(MetisE_USERSETTING_WMBUS_ROLE, false, &wmbusRxRole, 1);
}

/**
 * @brief Set the volatile CFG flags to use immediately
 *
 * @param[in] cfgFlags : configuration flags to set
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_SetVolatile_CFGFlags(uint8_t cfgFlags)
{
    /* check for invalid cfg flags */
    if ((cfgFlags & ((uint8_t)0xFF ^ METIS_E_CFG_MASK)) != 0)
    {
        /*invalid cfg flags*/
        return false;
    }

    return MetisE_SetSetting(MetisE_USERSETTING_CFGFLAGS, false, &cfgFlags, 1);
}

/**
 * @brief Start rx mode to receive data. Module will stay in rx mode until it is stopped with the ReceiveStop() Command.
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool MetisE_ReceiveStart()
{
    txPacket.Cmd = METIS_E_CMD_RXSTART_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_RXSTART_CNF, CMD_Status_Success, true);
}

/**
 * @brief Stops rx mode so the module will stop receiving data
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool MetisE_ReceiveStop()
{
    txPacket.Cmd = METIS_E_CMD_RXSTOP_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_RXSTOP_CNF, CMD_Status_Success, true);
}

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] payload: pointer to the data
 * @param[in] length: length of the data
 *
 * @return true if request succeeded,
 *         false otherwise
 */

bool MetisE_Transmit(MetisE_wmBusMode_t mode, MetisE_wmBusFrameFormat_t frameFormat, uint8_t* payload, uint8_t length)
{
    if ((payload == NULL) || (length == 0))
    {
        return false;
    }

    if (length > MAX_PAYLOAD_LENGTH)
    {
        WE_DEBUG_PRINT_INFO("Data exceeds maximal payload length\r\n");
        return false;
    }

    txPacket.Cmd = METIS_E_CMD_DATAEX;
    txPacket.Length = 0;

    txPacket.Data[txPacket.Length++] = MetisE_OPTION_WMBUS_TXMODE;
    txPacket.Data[txPacket.Length++] = 1;
    txPacket.Data[txPacket.Length++] = mode;

    txPacket.Data[txPacket.Length++] = MetisE_OPTION_WMBUS_FRAMEFORMAT;
    txPacket.Data[txPacket.Length++] = 1;
    txPacket.Data[txPacket.Length++] = frameFormat;

    txPacket.Data[txPacket.Length++] = MetisE_OPTION_WMBUS_DATA;
    txPacket.Data[txPacket.Length++] = length;
    memcpy(&txPacket.Data[txPacket.Length], payload, length);
    txPacket.Length += length;

    FillChecksum(&txPacket);
    if (!MetisE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_E_CMD_DATAEX_CNF, CMD_Status_Success, true);
}

/**
 * @brief Configure the MetisE
 *
 * @param[in] config: pointer to the configuration struct
 * @param[in] config_length: length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool MetisE_Configure(MetisE_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    if ((config == NULL) || (config_length == 0))
    {
        return false;
    }

    uint8_t help_length;
    uint8_t help[METIS_E_MAX_USERSETTING_LENGTH];

    if (factory_reset)
    {
        /* perform a factory reset */
        if (!MetisE_FactoryReset())
        {
            /* error */
            return false;
        }
    }
    WE_Delay(500);

    /* now check all settings and update them if necessary */
    for (uint8_t i = 0; i < config_length; i++)
    {
        /* read current value */
        if (!MetisE_GetDefault(config[i].usersetting, help, &help_length))
        {
            /* error */
            return false;
        }
        WE_Delay(200);

        /* check the value read out */
        if (help_length != config[i].value_length)
        {
            /* error, length does not match */
            return false;
        }
        if (memcmp(help, config[i].value, config[i].value_length) != 0)
        {
            /* read value is not up to date, thus write the new value */
            if (!MetisE_SetDefault(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    return MetisE_PinReset();
}
