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
 * @brief Telesto-III driver source file.
 */

#include <TelestoIII/TelestoIII.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 224
#define MAX_DATA_BUFFER 255
#define TXPOWERINVALID -128
#define RSSIINVALID -128
#define CHANNELINVALID -1

#define CMD_STX 0x02
#define TELESTOIII_CMD_TYPE_REQ (0 << 6)
#define TELESTOIII_CMD_TYPE_CNF (1 << 6)
#define TELESTOIII_CMD_TYPE_IND (2 << 6)
#define TELESTOIII_CMD_TYPE_RSP (3 << 6)

#define TELESTOIII_CMD_DATA 0x00
#define TELESTOIII_CMD_DATA_REQ (TELESTOIII_CMD_DATA | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_DATA_CNF (TELESTOIII_CMD_DATA | TELESTOIII_CMD_TYPE_CNF)
#define TELESTOIII_CMD_REPEAT_IND (TELESTOIII_CMD_DATA | TELESTOIII_CMD_TYPE_IND)

#define TELESTOIII_CMD_DATAEX 0x01
#define TELESTOIII_CMD_DATAEX_REQ (TELESTOIII_CMD_DATAEX | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_DATAEX_IND (TELESTOIII_CMD_DATAEX | TELESTOIII_CMD_TYPE_IND)

#define TELESTOIII_CMD_RESET 0x05
#define TELESTOIII_CMD_RESET_REQ (TELESTOIII_CMD_RESET | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_RESET_CNF (TELESTOIII_CMD_RESET | TELESTOIII_CMD_TYPE_CNF)
#define TELESTOIII_CMD_RESET_IND (TELESTOIII_CMD_RESET | TELESTOIII_CMD_TYPE_IND)

#define TELESTOIII_CMD_SET_CHANNEL 0x06
#define TELESTOIII_CMD_SET_CHANNEL_REQ (TELESTOIII_CMD_SET_CHANNEL | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SET_CHANNEL_CNF (TELESTOIII_CMD_SET_CHANNEL | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_SET_DESTNETID 0x07
#define TELESTOIII_CMD_SET_DESTNETID_REQ (TELESTOIII_CMD_SET_DESTNETID | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SET_DESTNETID_CNF (TELESTOIII_CMD_SET_DESTNETID | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_SET_DESTADDR 0x08
#define TELESTOIII_CMD_SET_DESTADDR_REQ (TELESTOIII_CMD_SET_DESTADDR | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SET_DESTADDR_CNF (TELESTOIII_CMD_SET_DESTADDR | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_SET 0x09
#define TELESTOIII_CMD_SET_REQ (TELESTOIII_CMD_SET | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SET_CNF (TELESTOIII_CMD_SET | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_GET 0x0A
#define TELESTOIII_CMD_GET_REQ (TELESTOIII_CMD_GET | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_GET_CNF (TELESTOIII_CMD_GET | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_RSSI 0x0D
#define TELESTOIII_CMD_RSSI_REQ (TELESTOIII_CMD_RSSI | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_RSSI_CNF (TELESTOIII_CMD_RSSI | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_SHUTDOWN 0x0E
#define TELESTOIII_CMD_SHUTDOWN_REQ (TELESTOIII_CMD_SHUTDOWN | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SHUTDOWN_CNF (TELESTOIII_CMD_SHUTDOWN | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_STANDBY 0x0F
#define TELESTOIII_CMD_STANDBY_REQ (TELESTOIII_CMD_STANDBY | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_STANDBY_CNF (TELESTOIII_CMD_STANDBY | TELESTOIII_CMD_TYPE_CNF)
#define TELESTOIII_CMD_STANDBY_IND (TELESTOIII_CMD_STANDBY | TELESTOIII_CMD_TYPE_IND)

#define TELESTOIII_CMD_SET_PAPOWER 0x11
#define TELESTOIII_CMD_SET_PAPOWER_REQ (TELESTOIII_CMD_SET_PAPOWER | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_SET_PAPOWER_CNF (TELESTOIII_CMD_SET_PAPOWER | TELESTOIII_CMD_TYPE_CNF)

#define TELESTOIII_CMD_FACTORY_RESET 0x12
#define TELESTOIII_CMD_FACTORY_RESET_REQ (TELESTOIII_CMD_FACTORY_RESET | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_FACTORY_RESET_CNF (TELESTOIII_CMD_FACTORY_RESET | TELESTOIII_CMD_TYPE_CNF)

/* AMBER test commands */
#define TELESTOIII_CMD_PINGDUT 0x1F
#define TELESTOIII_CMD_PINGDUT_REQ (TELESTOIII_CMD_PINGDUT | TELESTOIII_CMD_TYPE_REQ)
#define TELESTOIII_CMD_PINGDUT_CNF (TELESTOIII_CMD_PINGDUT | TELESTOIII_CMD_TYPE_CNF)

/* Masks for FLAGS */
/* Sniffer mode is indicated by bit 1 of cfg-flags */
#define TELESTOIII_CFGFLAGS_SNIFFERMODEENABLE 0x0001
#define TELESTOIII_RPFLAGS_REPEATERENABLE 0X0001

void TelestoIII_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = TelestoIII_HandleRxByte;

/**
 * @brief Type used to check the response, when a command was sent to the TelestoIII
 */
typedef enum TelestoIII_CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed = 0x01,
    CMD_Status_Invalid_Channel = 0x02,
    CMD_Status_LBT_Channel_Busy = 0x03,
    CMD_Status_Busy = 0x04,
    CMD_Status_Invalid = 0xFF,
} TelestoIII_CMD_Status_t;

#define LENGTH_CMD_OVERHEAD (uint16_t)4
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(MAX_DATA_BUFFER + LENGTH_CMD_OVERHEAD)
typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_DATA_BUFFER + 1]; /* +1 for the CS */
} TelestoIII_CMD_Frame_t;

typedef struct
{
    uint8_t cmd;                    /* variable to check if correct CMD has been confirmed */
    TelestoIII_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the TelestoIII */
} TelestoIII_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/

static TelestoIII_CMD_Frame_t rxPacket;                                 /* data buffer for RX */
static TelestoIII_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static TelestoIII_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static uint8_t channelVolatile = CHANNELINVALID;                        /* variable used to check if setting the channel was successful */
static uint8_t powerVolatile = TXPOWERINVALID;                          /* variable used to check if setting the TXPower was successful */
static TelestoIII_AddressMode_t addressmode = TelestoIII_AddressMode_0; /* initial address mode */
/**
 * @brief Pin configuration struct pointer.
 */
static TelestoIII_Pins_t* TelestoIII_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* TelestoIII_uartP = NULL;
uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint8_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(TelestoIII_CMD_Frame_t)];                         /* data buffer for RX */
static void (*RxCallback)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t); /* callback function */

/**************************************
 *         Static functions           *
 **************************************/

/**
 * @brief Interpret the valid received UART data packet
 */
static void HandleRxPacket(uint8_t* rxBuffer)
{
    TelestoIII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = rxBuffer[2];
    memcpy((uint8_t*)&rxPacket, rxBuffer, cmd_length + 4);

    switch (rxPacket.Cmd)
    {
        case TELESTOIII_CMD_RESET_IND:
        case TELESTOIII_CMD_STANDBY_IND:
        {
            cmdConfirmation.status = CMD_Status_Success;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case TELESTOIII_CMD_FACTORY_RESET_CNF:
        case TELESTOIII_CMD_RESET_CNF:
        case TELESTOIII_CMD_SHUTDOWN_CNF:
        case TELESTOIII_CMD_STANDBY_CNF:
        case TELESTOIII_CMD_DATA_CNF:
        case TELESTOIII_CMD_GET_CNF:
        case TELESTOIII_CMD_SET_CNF:
        case TELESTOIII_CMD_SET_DESTADDR_CNF:
        case TELESTOIII_CMD_SET_DESTNETID_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[0] == 0x00) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case TELESTOIII_CMD_DATAEX_IND:
        {
            /* data received, give it to the RxCallback function */
            if (RxCallback != NULL)
            {
                switch (addressmode)
                {
                    case TelestoIII_AddressMode_0:
                    {
                        RxCallback(&rxPacket.Data[0], rxPacket.Length - 1, TELESTOIII_BROADCASTADDRESS, TELESTOIII_BROADCASTADDRESS, TELESTOIII_BROADCASTADDRESS, (int8_t)rxPacket.Data[rxPacket.Length - 1]);
                    }
                    break;

                    case TelestoIII_AddressMode_1:
                    {
                        RxCallback(&rxPacket.Data[1], rxPacket.Length - 2, TELESTOIII_BROADCASTADDRESS, rxPacket.Data[0], TELESTOIII_BROADCASTADDRESS, (int8_t)rxPacket.Data[rxPacket.Length - 1]);
                    }
                    break;

                    case TelestoIII_AddressMode_2:
                    {
                        RxCallback(&rxPacket.Data[2], rxPacket.Length - 3, rxPacket.Data[0], rxPacket.Data[1], TELESTOIII_BROADCASTADDRESS, (int8_t)rxPacket.Data[rxPacket.Length - 1]);
                    }
                    break;

                    case TelestoIII_AddressMode_3:
                    {
                        RxCallback(&rxPacket.Data[3], rxPacket.Length - 4, rxPacket.Data[0], rxPacket.Data[1], rxPacket.Data[2], (int8_t)rxPacket.Data[rxPacket.Length - 1]);
                    }
                    break;

                    default:
                        /* wrong address mode */
                        break;
                }
            }
        }
        break;

        case TELESTOIII_CMD_SET_CHANNEL_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[0] == channelVolatile) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;
        case TELESTOIII_CMD_SET_PAPOWER_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[0] == powerVolatile) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

            /* for internal use only */
        case TELESTOIII_CMD_PINGDUT_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[4] == 0x0A) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        default:
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
 * @brief function that waits for the return value of TelestoIII (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, TelestoIII_CMD_Status_t expectedStatus, bool reset_confirmstate)
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
            return false;
        }

        /* wait */
        count++;
        WE_Delay(time_step_ms);
    }
    return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet
 */
static void FillChecksum(TelestoIII_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;

    for (uint8_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }

    cmd->Data[cmd->Length] = checksum;
}

void TelestoIII_HandleRxByte(uint8_t* dataP, size_t size)
{
    for (; size > 0; size--, dataP++)
    {
        if (rxByteCounter < sizeof(rxBuffer))
        {
            rxBuffer[rxByteCounter] = *dataP;
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
                bytesToReceive = (rxBuffer[rxByteCounter - 1] + 4); /* len + crc + sfd + cmd */
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
bool TelestoIII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }

    return TelestoIII_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Initialize the TelestoIII for serial interface
 *
 * Caution: The parameters baudrate and addrmode must match the configured UserSettings of the TelestoIII.
 *          The baudrate parameter must match to perform a successful UART communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The addrmode must match when RF packet transmission or reception is performed.
 *          This parameter can be updated to the correct value (used in TelestoIII_Init function) as soon as no RF packet transmission or reception was performed.
 *
 * @param[in] uartP :         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] addrmode:       address mode of the TelestoIII
 * @param[in] RXcb:           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool TelestoIII_Init(WE_UART_t* uartP, TelestoIII_Pins_t* pinoutP, TelestoIII_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t))
{
    /* set address mode */
    addressmode = addrmode;

    /* set RX callback function */
    RxCallback = RXcb;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    TelestoIII_pinsP = pinoutP;
    TelestoIII_pinsP->TelestoIII_Pin_Reset.type = WE_Pin_Type_Output;
    TelestoIII_pinsP->TelestoIII_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp.initial_value.output = WE_Pin_Level_Low;
    TelestoIII_pinsP->TelestoIII_Pin_Boot.type = WE_Pin_Type_Output;
    TelestoIII_pinsP->TelestoIII_Pin_Boot.initial_value.output = WE_Pin_Level_Low;
    TelestoIII_pinsP->TelestoIII_Pin_Mode.type = WE_Pin_Type_Output;
    TelestoIII_pinsP->TelestoIII_Pin_Mode.initial_value.output = WE_Pin_Level_Low;

    WE_Pin_t pins[sizeof(TelestoIII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Mode, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    TelestoIII_uartP = uartP;
    if (!TelestoIII_uartP->uartInit(TelestoIII_uartP->baudrate, TelestoIII_uartP->flowControl, TelestoIII_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (TelestoIII_PinReset())
    {
        WE_Delay(300);
    }
    else
    {
        WE_DEBUG_PRINT_INFO("Pin reset failed\r\n");
        TelestoIII_Deinit();
        return false;
    }

    return true;
}

/**
 * @brief Deinitialize the TelestoIII interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool TelestoIII_Deinit()
{
    WE_Pin_t pins[sizeof(TelestoIII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &TelestoIII_pinsP->TelestoIII_Pin_Mode, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    addressmode = TelestoIII_AddressMode_0;
    RxCallback = NULL;

    return TelestoIII_uartP->uartDeinit();
}

/**
 * @brief Wakeup the TelestoIII from standby or shutdown mode by pin
 *
 * @param[in] standby:       true, if wake-up from standby mode is made
 * 							 false, if wake-up from shutdown mode is made
 *
 * @return true if wakeup succeeded,
 *         false otherwise
 */
bool TelestoIII_PinWakeup(bool standby)
{
    if (!WE_SetPin(TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    WE_Delay(5);

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }

    if (!WE_SetPin(TelestoIII_pinsP->TelestoIII_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, standby ? TELESTOIII_CMD_STANDBY_IND : TELESTOIII_CMD_RESET_IND, CMD_Status_Success, false);
}

/**
 * @brief Reset the TelestoIII by pin
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool TelestoIII_PinReset()
{
    /* set to output mode */
    if (!WE_SetPin(TelestoIII_pinsP->TelestoIII_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(TelestoIII_pinsP->TelestoIII_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_RESET_IND, CMD_Status_Success, true);
}

/**
 * @brief Reset the TelestoIII by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool TelestoIII_Reset()
{
    txPacket.Cmd = TELESTOIII_CMD_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_RESET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Factory reset the TelestoIII
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number times
 *
 * @return true if factory reset succeeded,
 *         false otherwise
 */
bool TelestoIII_FactoryReset()
{
    txPacket.Cmd = TELESTOIII_CMD_FACTORY_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(1500, TELESTOIII_CMD_FACTORY_RESET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Switch the module to standby mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool TelestoIII_Standby()
{
    txPacket.Cmd = TELESTOIII_CMD_STANDBY_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_STANDBY_CNF, CMD_Status_Success, true);
}

/**
 * @brief Switch the module to shutdown mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool TelestoIII_Shutdown()
{
    txPacket.Cmd = TELESTOIII_CMD_SHUTDOWN_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SHUTDOWN_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the current TelestoIII settings
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_Get(TelestoIII_UserSettings_t us, uint8_t* response, uint8_t* response_length)
{
    if (response == NULL || response_length == NULL)
    {
        return false;
    }

    txPacket.Cmd = TELESTOIII_CMD_GET_REQ;
    txPacket.Length = 0x01;
    txPacket.Data[0] = us;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_GET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    int length = rxPacket.Length - 1;
    memcpy(response, &rxPacket.Data[1], length);
    *response_length = length;

    return true;
}

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
bool TelestoIII_CheckNSet(TelestoIII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    uint8_t current_value[length];
    uint8_t current_length = length;

    if (!TelestoIII_Get(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return TelestoIII_Set(userSetting, valueP, length);
}

/**
 * @brief Set a special TelestoIII setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_Set(TelestoIII_UserSettings_t us, uint8_t* value, uint8_t length)
{
    if (value == NULL)
    {
        return false;
    }

    txPacket.Cmd = TELESTOIII_CMD_SET_REQ;
    txPacket.Length = (1 + length);
    txPacket.Data[0] = us;
    memcpy(&txPacket.Data[1], value, length);

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] fw: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetFirmwareVersion(uint8_t* fw)
{
    if (fw == NULL)
    {
        return false;
    }

    uint8_t help[3];
    uint8_t help_length;

    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_FWVERSION, help, &help_length))
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
bool TelestoIII_GetSerialNumber(uint8_t* sn)
{
    if (sn == NULL)
    {
        return false;
    }

    uint8_t help[8];
    uint8_t help_length;

    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_FACTORYSETTINGS, help, &help_length))
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
bool TelestoIII_GetDefaultTXPower(uint8_t* txpower)
{
    if (txpower == NULL)
    {
        return false;
    }

    *txpower = TXPOWERINVALID;
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER, txpower, &length);
}

/**
 * @brief Get the default destination address
 *
 * @param[out] destaddr_lsb: LSB of the destination address
 * @param[out] destaddr_msb: MSB of the destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* destaddr_msb)
{
    if (destaddr_lsb == NULL || destaddr_msb == NULL)
    {
        return false;
    }

    /* helper array */
    uint8_t help[2];
    uint8_t length;

    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTADDR, help, &length))
    {
        return false;
    }

    *destaddr_lsb = help[0];
    *destaddr_msb = help[1];

    return true;
}

/**
 * @brief Get the default destination address
 *
 * @param[out] destnetid: destination net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetDefaultDestNetID(uint8_t* destnetid)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTNETID, destnetid, &length);
}

/**
 * @brief Get the default source address
 *
 * @param[out] srcaddr_lsb: LSB of the source address
 * @param[out] srcaddr_msb: MSB of the source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb)
{
    if (srcaddr_lsb == NULL || srcaddr_msb == NULL)
    {
        return false;
    }

    /* helper array */
    uint8_t help[2];
    uint8_t length;

    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_SOURCEADDR, help, &length))
    {
        return false;
    }

    *srcaddr_lsb = help[0];
    *srcaddr_msb = help[1];

    return true;
}

/**
 * @brief Set the default source net id
 *
 * @param[out] srcnetid: source net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetSourceNetID(uint8_t* srcnetid)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_SOURCENETID, srcnetid, &length);
}

/**
 * @brief Get the default RF channel
 *
 * @param[out] channel: channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetDefaultRFChannel(uint8_t* channel)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL, channel, &length);
}

/**
 * @brief Get the default RF profile
 *
 * @param[out] profile: RF profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetDefaultRFProfile(uint8_t* profile)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFPROFILE, profile, &length);
}

/**
 * @brief Get the LBT observation period
 *
 * @param[out] threshold: LBT observation period in ms
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetLBTObservationPeriod(uint8_t* period)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD, period, &length);
}

/**
 * @brief Get the LBT threshold value
 *
 * @param[out] threshold: LBT threshold value in dBm
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_GetLBTThreshold(int8_t* threshold)
{
    uint8_t length;

    return TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_LBT_THRESHOLD, (uint8_t*)threshold, &length);
}

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TelestoIII_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetDefaultTXPower(uint8_t txpower)
{
    /* check for invalid power */
    if (txpower > 14)
    {
        /*invalid power*/
        return false;
    }

    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER, &txpower, 1);
}

/**
 * @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TelestoIII_SetVolatile_DestAddr for frequent adaption of the destination address.
 *
 * @param[in] destaddr_lsb: LSB of the destination address
 * @param[in] destaddr_msb: MSB of the destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb)
{
    /* fill array */
    uint8_t help[2];
    help[0] = destaddr_lsb;
    help[1] = destaddr_msb;
    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTADDR, help, 2);
}

/**
 * @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: use TelestoIII_SetVolatile_DestNetID for frequent adaption of the destination net id.
 *
 * @param[in] destnetid: destination net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetDefaultDestNetID(uint8_t destnetid) { return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTNETID, &destnetid, 1); }

/**
 * @brief Set the default source address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcaddr_lsb: LSB of the source address
 * @param[in] srcaddr_msb: MSB of the source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb)
{
    /* fill array */
    uint8_t help[2];
    help[0] = srcaddr_lsb;
    help[1] = srcaddr_msb;
    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_SOURCEADDR, help, 2);
}

/**
 * @brief Set the default source net id
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcnetid: source net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetSourceNetID(uint8_t srcnetid) { return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_SOURCENETID, &srcnetid, 1); }

/**
 * @brief Set the default RF channel
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TelestoIII_SetVolatile_Channel for frequent adaption of the channel
 *
 * @param[in] channel: channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetDefaultRFChannel(uint8_t channel)
{

    /* check for valid channel */
    if ((channel < 201) || (channel > 251))
    {
        /* invalid channel */
        return false;
    }
    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL, &channel, 1);
}

/**
 * @brief Set the default RF profile
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] profile: RF profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetDefaultRFProfile(uint8_t profile) { return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_DEFAULTRFPROFILE, &profile, 1); }

/**
 * @brief Set the LBT observation period
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] period: LBT observation period in ms
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetLBTObservationPeriod(uint8_t period)
{
    if (period > 15)
    {
        return false;
    }

    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD, &period, 1);
}

/**
 * @brief Set the LBT threshold
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] threshold: LBT threshold in dBm
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetLBTThreshold(int8_t threshold)
{
    if ((threshold < -100) || (threshold > -45))
    {
        return false;
    }

    return TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_LBT_THRESHOLD, (uint8_t*)&threshold, 1);
}

/**
 * @brief Enables the Sniffer mode
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_EnableSnifferMode()
{
    uint16_t rpFlags;
    uint16_t cfgFlags;
    uint8_t length;

    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_CFG_FLAGS, (uint8_t*)&cfgFlags, &length))
    {
        return false;
    }

    /* set sniffer mode if not set already */
    if (TELESTOIII_CFGFLAGS_SNIFFERMODEENABLE != (cfgFlags & TELESTOIII_CFGFLAGS_SNIFFERMODEENABLE))
    {
        cfgFlags |= TELESTOIII_CFGFLAGS_SNIFFERMODEENABLE;
        if (!TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_CFG_FLAGS, (uint8_t*)&cfgFlags, 2))
        {
            return false;
        }
    }

    /* Make sure repeater mode is disabled once sniffer mode is active. Sniffer mode and repeater mode can not be used simultaneously */
    if (!TelestoIII_Get(TelestoIII_CMD_SETGET_OPTION_RP_FLAGS, (uint8_t*)&rpFlags, &length))
    {
        return false;
    }

    if (TELESTOIII_RPFLAGS_REPEATERENABLE == (rpFlags & TELESTOIII_RPFLAGS_REPEATERENABLE))
    {
        rpFlags &= ~TELESTOIII_RPFLAGS_REPEATERENABLE;
        if (!TelestoIII_Set(TelestoIII_CMD_SETGET_OPTION_RP_FLAGS, (uint8_t*)&rpFlags, 2))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Set the volatile TX power
 *
 * @param[in] power: new TX power value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetVolatile_TXPower(uint8_t power)
{
    /* check for invalid power */
    if (power > 14)
    {
        /*invalid power*/
        return false;
    }

    txPacket.Cmd = TELESTOIII_CMD_SET_PAPOWER_REQ;
    txPacket.Length = 0x01;
    txPacket.Data[0] = power;

    FillChecksum(&txPacket);

    powerVolatile = power;

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SET_PAPOWER_CNF, CMD_Status_Success, true);

    powerVolatile = TXPOWERINVALID;

    return ret;
}

/**
 * @brief Set the volatile channel
 *
 * @param[in] channel: new channel value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetVolatile_Channel(uint8_t channel)
{
    /* check for valid channel */
    if ((channel < 201) || (channel > 251))
    {
        /* invalid channel */
        return false;
    }

    txPacket.Cmd = TELESTOIII_CMD_SET_CHANNEL_REQ;
    txPacket.Length = 0x01;
    txPacket.Data[0] = channel;

    FillChecksum(&txPacket);

    channelVolatile = channel;

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SET_CHANNEL_CNF, CMD_Status_Success, true);

    channelVolatile = CHANNELINVALID;

    return ret;
}

/**
 * @brief Set the volatile destination net ID
 *
 * @param[in] destnetid: new destination net ID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetVolatile_DestNetID(uint8_t destnetid)
{

    txPacket.Cmd = TELESTOIII_CMD_SET_DESTNETID_REQ;
    txPacket.Length = 0x01;
    txPacket.Data[0] = destnetid;

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SET_DESTNETID_CNF, CMD_Status_Success, true);
}

/**
 * @brief Set the volatile destination address
 *
 * @param[in] destaddr_lsb: lsb of the new destination address value
 * @param[in] destaddr_msb: msb of the new destination address value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb)
{
    switch (addressmode)
    {
        case TelestoIII_AddressMode_0:
        case TelestoIII_AddressMode_1:
        case TelestoIII_AddressMode_2:
        {
            txPacket.Cmd = TELESTOIII_CMD_SET_DESTADDR_REQ;
            txPacket.Length = 0x01;
            txPacket.Data[0] = destaddr_lsb;
        }
        break;
        case TelestoIII_AddressMode_3:
        {
            txPacket.Cmd = TELESTOIII_CMD_SET_DESTADDR_REQ;
            txPacket.Length = 0x02;
            txPacket.Data[0] = destaddr_lsb;
            txPacket.Data[1] = destaddr_msb;
        }
        break;
        default:
            return false;
    }

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_SET_DESTADDR_CNF, CMD_Status_Success, true);
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
bool TelestoIII_Transmit(uint8_t* payload, uint8_t length)
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

    txPacket.Cmd = TELESTOIII_CMD_DATA_REQ;
    txPacket.Length = length;
    memcpy(&txPacket.Data[0], payload, length);

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_DATA_CNF, CMD_Status_Success, true);
}

/**
 * @brief Transmit data
 *
 * @param[in] payload: pointer to the data
 * @param[in] length: length of the data
 * @param[in] channel: channel to be used
 * @param[in] dest_network_id: destination network ID
 * @param[in] dest_address_lsb: destination address lsb
 * @param[in] dest_address_msb: destination address msb
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb)
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

    txPacket.Cmd = TELESTOIII_CMD_DATAEX_REQ;

    switch (addressmode)
    {
        case TelestoIII_AddressMode_0:
        {
            txPacket.Length = (length + 1);
            txPacket.Data[0] = channel;
            memcpy(&txPacket.Data[1], payload, length);
        }
        break;

        case TelestoIII_AddressMode_1:
        {
            txPacket.Length = (length + 2);
            txPacket.Data[0] = channel;
            txPacket.Data[1] = dest_address_lsb;
            memcpy(&txPacket.Data[2], payload, length);
        }
        break;

        case TelestoIII_AddressMode_2:
        {
            txPacket.Length = (length + 3);
            txPacket.Data[0] = channel;
            txPacket.Data[1] = dest_network_id;
            txPacket.Data[2] = dest_address_lsb;
            memcpy(&txPacket.Data[3], payload, length);
        }
        break;

        case TelestoIII_AddressMode_3:
        {
            txPacket.Length = (length + 4);
            txPacket.Data[0] = channel;
            txPacket.Data[1] = dest_network_id;
            txPacket.Data[2] = dest_address_lsb;
            txPacket.Data[3] = dest_address_msb;
            memcpy(&txPacket.Data[4], payload, length);
        }
        break;

        default:
            /* wrong address mode */
            return false;
    }

    FillChecksum(&txPacket);

    if (!TelestoIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, TELESTOIII_CMD_DATA_CNF, CMD_Status_Success, true);
}

/**
 * @brief Use the ping test command
 *
 * Note: Do not use this command. Just used for internal purposes!
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_Ping()
{
    /* rf-Profil=6, ch201, +14dBm, 100 packets*/
    uint8_t ping_command[] = {0x02, 0x1F, 0x08, 0x20, 0x06, 0xC9, 0x0E, 0x64, 0xFF, 0xFF, 0xFF, 0x6F};

    /* now send the data */
    if (!TelestoIII_Transparent_Transmit(ping_command, sizeof(ping_command)))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(10000 /*10s*/, TELESTOIII_CMD_PINGDUT_CNF, CMD_Status_Success, true);
}

/**
 * @brief Configure the TelestoIII
 *
 * @param[in] config: pointer to the configuration struct
 * @param[in] config_length: length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool TelestoIII_Configure(TelestoIII_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    if ((config == NULL) || (config_length == 0))
    {
        return false;
    }

    uint8_t help_length;
    uint8_t help[TELESTOIII_MAX_USERSETTING_LENGTH];

    if (factory_reset)
    {
        /* perform a factory reset */
        if (!TelestoIII_FactoryReset())
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
        if (!TelestoIII_Get(config[i].usersetting, help, &help_length))
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
            if (!TelestoIII_Set(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    return TelestoIII_PinReset();
}
