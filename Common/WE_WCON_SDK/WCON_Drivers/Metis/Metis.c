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
 * @brief Metis driver source file.
 */

#include <Metis/Metis.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 254
#define TXPOWERINVALID -128
#define RSSIINVALID -128

#define LENGTH_CMD_OVERHEAD (uint16_t)4
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(Metis_MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

#define CMD_STX 0xFF
#define METIS_CMD_TYPE_REQ (0 << 6)
#define METIS_CMD_TYPE_CNF (2 << 6)

#define METIS_CMD_DATA 0x00
#define METIS_CMD_DATA_REQ (METIS_CMD_DATA | METIS_CMD_TYPE_REQ)
#define METIS_CMD_DATA_CNF (METIS_CMD_DATA | METIS_CMD_TYPE_CNF)

#define METIS_CMD_DATA_IND 0x03

#define METIS_CMD_SET_MODE 0x04
#define METIS_CMD_SET_MODE_REQ (METIS_CMD_SET_MODE | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SET_MODE_CNF (METIS_CMD_SET_MODE | METIS_CMD_TYPE_CNF)

#define METIS_CMD_RESET 0x05
#define METIS_CMD_RESET_REQ (METIS_CMD_RESET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_RESET_CNF (METIS_CMD_RESET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_SET 0x09
#define METIS_CMD_SET_REQ (METIS_CMD_SET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SET_CNF (METIS_CMD_SET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_SETUARTSPEED 0X10
#define METIS_CMD_SETUARTSPEED_REQ (METIS_CMD_SETUARTSPEED | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SETUARTSPEED_CNF (METIS_CMD_SETUARTSPEED | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET 0x0A
#define METIS_CMD_GET_REQ (METIS_CMD_GET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_CNF (METIS_CMD_GET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET_SERIALNO 0x0B
#define METIS_CMD_GET_SERIALNO_REQ (METIS_CMD_GET_SERIALNO | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_SERIALNO_CNF (METIS_CMD_GET_SERIALNO | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET_FWRELEASE 0x0C
#define METIS_CMD_GET_FWRELEASE_REQ (METIS_CMD_GET_FWRELEASE | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_FWRELEASE_CNF (METIS_CMD_GET_FWRELEASE | METIS_CMD_TYPE_CNF)

#define METIS_CMD_RSSI 0x0D
#define METIS_CMD_RSSI_REQ (METIS_CMD_RSSI | METIS_CMD_TYPE_REQ)
#define METIS_CMD_RSSI_CNF (METIS_CMD_RSSI | METIS_CMD_TYPE_CNF)

#define METIS_CMD_FACTORYRESET 0X11
#define METIS_CMD_FACTORYRESET_REQ (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_FACTORYRESET_CNF (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_CNF)

void Metis_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = Metis_HandleRxByte;

/**
 * @brief Type used to check the response, when a command was sent to the module
 */
typedef enum Metis_CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed,
    CMD_Status_Invalid,
    CMD_Status_Reset,
} Metis_CMD_Status_t;

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_PAYLOAD_LENGTH + 1]; /* +1 for the CS */
} Metis_CMD_Frame_t;

typedef struct
{
    uint8_t cmd;               /* variable to check if correct CMD has been confirmed */
    Metis_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the module */
} Metis_CMD_Confirmation_t;

typedef struct
{
    uint8_t memoryPosition;   /* memory position of requested usersetting */
    uint8_t lengthGetRequest; /* length of one or more requested usersetting */
} Metis_US_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/

/**
 * @brief Pin configuration struct pointer.
 */
static Metis_Pins_t* Metis_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* Metis_uartP = NULL;
static Metis_CMD_Frame_t rxPacket;                                 /* data buffer for RX */
static Metis_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static Metis_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static Metis_US_Confirmation_t usConfirmation; /* variable used to check if GET function was successful */
static Metis_Frequency_t frequency;            /* frequency used by module */
static bool rssi_enable = false;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint8_t bytesToReceive = 0;                  /* read buffer for next available byte */
static uint8_t rxBuffer[sizeof(Metis_CMD_Frame_t)]; /* data buffer for RX */
static Metis_RxCallback_t RxCallback;
/* callback function */

/**************************************
 *          Static functions          *
 **************************************/

/**
 * @brief Function to add the checksum at the end of the data packet. 
 */
static void FillChecksum(Metis_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint8_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
}

/**
 * @brief Function to calculate the rssi value from the rx level
 */
static int8_t CalculateRSSIValue(uint8_t rxLevel)
{
    const uint8_t offset = 74;
    return (rxLevel < 128) ? (rxLevel / 2 - offset) : ((rxLevel - 256) / 2 - offset);
}

/**
 * @brief Interpret the valid received data packet
 */
static void HandleRxPacket(uint8_t* packetData)
{
    Metis_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = packetData[2];
    memcpy((uint8_t*)&rxPacket, packetData, cmd_length + 4); /* payload + std + command + length byte + checksum */

    switch (rxPacket.Cmd)
    {
        case METIS_CMD_DATA_IND:
        {
            /* the call of the RxCallback strongly depends on the configuration of the module */
            if (RxCallback != NULL)
            {
                if (rssi_enable == 0x01)
                {
                    /* the following implementation expects that the RSSI_Enable usersetting is enabled */
                    rxPacket.Length = rxPacket.Length - 1;
                    RxCallback(&rxPacket.Length, rxPacket.Length + 1, CalculateRSSIValue(rxPacket.Data[rxPacket.Length]));
                }
                else
                {
                    /* the following implementation expects that the RSSI_Enable usersetting is disabled */
                    RxCallback(&rxPacket.Length, rxPacket.Length + 1, (int8_t)RSSIINVALID);
                }
            }
        }
        break;

        case METIS_CMD_GET_CNF:
        {
            /* Data[0] contains memory position of usersetting
		 * Data[1] contains length of parameter, which is depending on usersetting
		 * On success mode responds with usersetting, length of parameter and parameter
		 */
            switch (rxPacket.Data[0])
            {
                /* usersettings with value length of 1 byte */
                case (Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
                case (Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
                {
                    /* check if correct usersetting was changed and if length corresponds to usersetting */
                    if ((usConfirmation.memoryPosition == rxPacket.Data[0]) && (usConfirmation.lengthGetRequest == rxPacket.Data[1]))
                    {
                        cmdConfirmation.status = CMD_Status_Success;
                    }
                    else
                    {
                        cmdConfirmation.status = CMD_Status_Failed;
                    }
                    cmdConfirmation.cmd = rxPacket.Cmd;
                }
                break;
                    /* usersettings with value length of 2 byte*/
                case (Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
                {
                    /* check if correct usersetting was changed and if length corresponds to usersetting */
                    if ((usConfirmation.memoryPosition == rxPacket.Data[0]) && (usConfirmation.lengthGetRequest == rxPacket.Data[1]))
                    {
                        cmdConfirmation.status = CMD_Status_Success;
                    }
                    else
                    {
                        cmdConfirmation.status = CMD_Status_Failed;
                    }
                    cmdConfirmation.cmd = rxPacket.Cmd;
                }
                break;

                default:
                    break;
            }
        }
        break;

        case METIS_CMD_FACTORYRESET_CNF:
        case METIS_CMD_SET_CNF:
        case METIS_CMD_SET_MODE_CNF:
        case METIS_CMD_RESET_CNF:
        case METIS_CMD_DATA_CNF:
        case METIS_CMD_SETUARTSPEED_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[0] == 0x00) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case METIS_CMD_GET_SERIALNO_CNF:
        {
            cmdConfirmation.status = (rxPacket.Length == 4) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case METIS_CMD_GET_FWRELEASE_CNF:
        {
            cmdConfirmation.status = (rxPacket.Length == 3) ? CMD_Status_Success : CMD_Status_Failed;
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
 * @brief Function that waits for the return value of Metis (*_CNF), when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, Metis_CMD_Status_t expectedStatus, bool reset_confirmstate)
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

void Metis_HandleRxByte(uint8_t* dataP, size_t size)
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
                /* wait for SFD */
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

bool Metis_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }

    return Metis_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Initialize the AMB module for serial interface.
 *
 * Caution: The baudrate parameter must match the module's baudrate to perform a successful communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The mode parameter must match the other participant of the RF communication.
 *          Check manual of the wM-Bus AMB modules for the suitable modes.
 *
 * @param[in] uartP :         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] freq           frequency used by the AMBER module(AMB8xxx-M uses 868Mhz, AMB36xx-M uses 169MHz)
 * @param[in] mode           wM-Bus mode preselect of the AMBER module
 * @param[in] enable_rssi    enable rssi in data reception
 * @param[in] RXcb           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool Metis_Init(WE_UART_t* uartP, Metis_Pins_t* pinoutP, Metis_Frequency_t freq, Metis_Mode_Preselect_t mode, bool enable_rssi, Metis_RxCallback_t RXcb)
{
    /* set frequency used by module */
    frequency = freq;

    /* set RX callback function */
    RxCallback = RXcb;

    /* set rssi_enable */
    rssi_enable = enable_rssi;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    Metis_pinsP = pinoutP;
    Metis_pinsP->Metis_Pin_Reset.type = WE_Pin_Type_Output;
    Metis_pinsP->Metis_Pin_Reset.initial_value.output = WE_Pin_Level_High;

    if (!WE_InitPins(&Metis_pinsP->Metis_Pin_Reset, 1))
    {
        /* error */
        return false;
    }

    Metis_uartP = uartP;
    if (!Metis_uartP->uartInit(Metis_uartP->baudrate, Metis_uartP->flowControl, Metis_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(1000);

    /* set recommended settings as described in the manual section 5.1 */

    /* enable uartOutEnable to print out received frames
	 * Setting is written to flash so write only if necessary
	 */
    uint8_t uartEnable;
    if (!Metis_GetUartOutEnable(&uartEnable))
    {
        return false;
    }

    if (uartEnable != 1)
    {
        WE_Delay(50);
        if (Metis_SetUartOutEnable(1))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set UART_CMD_OUT_MODE failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* enable rssi to be added to received frames
	 * Setting is written to flash so write only if necessary
	 */
    uint8_t rssi;
    if (!Metis_GetRSSIEnable(&rssi))
    {
        return false;
    }

    if (rssi != rssi_enable)
    {
        WE_Delay(50);
        if (Metis_SetRSSIEnable(rssi_enable ? 1 : 0))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set RSSI failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* disable AES encryption
	 * Setting is written to flash so write only if necessary
	 */
    uint8_t aesEnable;
    if (!Metis_GetAESEnable(&aesEnable))
    {
        return false;
    }

    if (aesEnable != 0)
    {
        WE_Delay(50);
        if (Metis_SetAESEnable(0))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set AESEnable failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* set mode preselect
	 * Setting is written to flash so write only if necessary
	 */
    uint8_t modePreselect;
    if (!Metis_GetModePreselect(&modePreselect))
    {
        return false;
    }

    if (modePreselect != mode)
    {
        WE_Delay(50);
        if (Metis_SetModePreselect(mode))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set mode preselect failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* Reset module to apply changes */
    if (Metis_Reset())
    {
        WE_Delay(300);
    }
    else
    {
        WE_DEBUG_PRINT_INFO("Reset failed\r\n");
        Metis_Deinit();
        return false;
    }
    return true;
}

/**
 * @brief Deinitialize the Metis interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool Metis_Deinit()
{
    /* deinit pins */
    if (!WE_DeinitPins(&Metis_pinsP->Metis_Pin_Reset, 1))
    {
        return false;
    }

    /* deinit RX callback */
    RxCallback = NULL;

    return Metis_uartP->uartDeinit();
}

/**
 * @brief Reset the Metis by pin
 *
 * @return true if reset succeeded
 *         false otherwise
 */
bool Metis_PinReset()
{
    if (!WE_SetPin(Metis_pinsP->Metis_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    return WE_SetPin(Metis_pinsP->Metis_Pin_Reset, WE_Pin_Level_High);
    ;
}

/**
 * @brief Reset the Metis by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool Metis_Reset()
{
    txPacket.Cmd = METIS_CMD_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_RESET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Factory reset the Metis
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *       The factory reset must be followed by a reset such that the changes become effective.
 *
 * @return true if factory reset succeeded,
 *         false otherwise
 */
bool Metis_FactoryReset()
{
    txPacket.Cmd = METIS_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_FACTORYRESET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Sets the baud rate of the module
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate baud rate
 *
 * @return true if setting baud rate succeeded,
 *         false otherwise
 */
bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate)
{
    txPacket.Cmd = METIS_CMD_SETUARTSPEED;
    txPacket.Length = 0x01;
    txPacket.Data[0] = baudrate;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SETUARTSPEED_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the current Metis settings.
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Get(Metis_UserSettings_t us, uint8_t* response, uint8_t* response_length)
{
    if (response == NULL || response_length == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_REQ;
    txPacket.Length = 0x02;
    txPacket.Data[0] = us;

    switch (us)
    {
        /* usersettings with value length 1 */
        case (Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
        case (Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
        {
            txPacket.Data[1] = 0x01;
        }
        break;
            /* usersettings with value length 2 */
        case (Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
        {
            txPacket.Data[1] = 0x02;
        }
        break;
        default:
            break;
    }

    FillChecksum(&txPacket);

    usConfirmation.memoryPosition = us;
    usConfirmation.lengthGetRequest = txPacket.Data[1];

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true);

    if (ret)
    {
        int length = rxPacket.Length - 2;
        memcpy(response, &rxPacket.Data[2], length);
        *response_length = length;
    }

    usConfirmation.memoryPosition = -1;
    usConfirmation.lengthGetRequest = -1;

    return ret;
}

/**
 * @brief Request multiple of the current Metis settings
 *
 * @param[in] startAddress: first usersetting to be read
 * @param[in] lengthToRead: Length of memory to be read
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t* response, uint8_t* response_length)
{
    if ((response == NULL) || (response_length == NULL) || (lengthToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_REQ;
    txPacket.Length = 0x02;
    txPacket.Data[0] = startAddress;
    txPacket.Data[1] = lengthToRead;

    FillChecksum(&txPacket);

    usConfirmation.memoryPosition = startAddress;
    usConfirmation.lengthGetRequest = lengthToRead;

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true);

    if (ret)
    {
        int length = rxPacket.Length - 2;
        memcpy(response, &rxPacket.Data[2], length);
        *response_length = length;
    }

    usConfirmation.memoryPosition = -1;
    usConfirmation.lengthGetRequest = -1;

    return ret;
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
bool Metis_CheckNSet(Metis_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    uint8_t current_value[length];
    uint8_t current_length = length;

    if (!Metis_Get(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return Metis_Set(userSetting, valueP, length);
}

/**
 * @brief Set a special Metis setting
 *
 * Note: Reset the module after the adaption of the setting such that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Set(Metis_UserSettings_t us, uint8_t* value, uint8_t length)
{
    if (value == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_SET_REQ;
    txPacket.Length = (2 + length);
    txPacket.Data[0] = us;
    txPacket.Data[1] = length;
    memcpy(&txPacket.Data[2], value, length);

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] fw: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetFirmwareVersion(uint8_t* fw)
{
    if (fw == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_FWRELEASE;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_FWRELEASE_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    memcpy(fw, &rxPacket.Data[0], rxPacket.Length);
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
bool Metis_GetSerialNumber(uint8_t* sn)
{
    if (sn == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_SERIALNO;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_SERIALNO_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    memcpy(sn, &rxPacket.Data[0], rxPacket.Length);
    return true;
}

/**
 * @brief Request the default TX power.
 *
 * @param[out] txpower: pointer to the TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetDefaultTXPower(int8_t* txpower)
{
    if (txpower == NULL)
    {
        return false;
    }

    *txpower = TXPOWERINVALID;
    uint8_t length;

    return Metis_Get(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)txpower, &length);
}

/**
 * @brief Get the Uart Out Enable byte
 *
 * @param[out] uartEnable: pointer to uartEnable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetUartOutEnable(uint8_t* uartEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, uartEnable, &length);
}

/**
 * @brief Get the RSSI Enable byte
 *
 * @param[out] rssiEnable: pointer to rssiEnable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetRSSIEnable(uint8_t* rssiEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, rssiEnable, &length);
}

/**
 * @brief Get the M-Bus mode preselect byte
 *
 * @param[out] modePreselect: Pointer to mode Preselect
 *
 * @return true if request succeeded,
 *         false otherwise
 */

bool Metis_GetModePreselect(uint8_t* modePreselect)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, modePreselect, &length);
}

/**
 * @brief Get the AES Enable byte
 *
 * @param[out] aesEnable: pointer to aes enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetAESEnable(uint8_t* aesEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, aesEnable, &length);
}

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetDefaultTXPower(int8_t txpower)
{
    /* check for invalid power */
    if ((txpower < -11) || (txpower > 15))
    {
        /* invalid power */
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)&txpower, 1);
}

/**
 * @brief Set the Uart Out Enable
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times
 *
 * @param[in] uartEnable: uart enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetUartOutEnable(uint8_t uartEnable)
{
    if ((uartEnable != 0) && (uartEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, &uartEnable, 1);
}
/**
 * @brief Set the RSSI Enable byte
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] rssiEnable: rssi Enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetRSSIEnable(uint8_t rssiEnable)
{
    if ((rssiEnable != 0) && (rssiEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, &rssiEnable, 1);
}

/**
 * @brief Set the AES Enable byte
 *
 * Note: Functions for AES are not implemented. Can only be used for disabling AES.
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] aesEnable: AES enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetAESEnable(uint8_t aesEnable)
{
    if (aesEnable != 0)
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, &aesEnable, 1);
}

/**
 * @brief Set the default M-Bus mode preselect
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_ModePreselect for frequent adaption of the channel.
 *
 * @param[in] modePreselect: M-Bus mode preselect

 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect) { return Metis_Set(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, (uint8_t*)&modePreselect, 1); }

/**
 * @brief Set the volatile mode preselect
 *
 * @param[in] modePreselect: new mode preselect value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    txPacket.Cmd = METIS_CMD_SET_MODE;
    txPacket.Length = 0x01;
    txPacket.Data[0] = modePreselect;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf*/
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
}

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] payload: pointer to the data
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Transmit(uint8_t* payload)
{
    if (payload == NULL)
    {
        return false;
    }

    uint8_t length = *payload; /* first byte of wM-BUS frame is length field */

    if (length > MAX_PAYLOAD_LENGTH)
    {
        WE_DEBUG_PRINT_INFO("Data exceeds maximal payload length");
        return false;
    }

    /* mode preselect C2/T2 for frequency 868 is not suitable for sending frames */
    if ((frequency == Metis_Frequency_868))
    {
        uint8_t modePreselect;
        if (!Metis_GetModePreselect(&modePreselect))
        {
            return false;
        }
        if (modePreselect == Metis_Mode_Preselect_868_C2_T2_other)
        {
            /* module can not send in this mode. */
            WE_DEBUG_PRINT_INFO("Mode Preselect %x is not suitable for transmitting\n", modePreselect);
            return false;
        }
    }

    txPacket.Cmd = METIS_CMD_DATA_REQ;
    txPacket.Length = length;
    memcpy(&txPacket.Data[0], &payload[1], length);

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_DATA_CNF, CMD_Status_Success, true);
}

/**
 * @brief Configure the Metis
 *
 * @param[in] config: pointer to the configuration struct
 * @param[in] config_length: length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Configure(Metis_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    if ((config == NULL) || (config_length == 0))
    {
        return false;
    }

    uint8_t help_length;
    uint8_t help[METIS_MAX_USERSETTING_LENGTH];

    if (factory_reset)
    {
        /* perform a factory reset */
        if (!Metis_FactoryReset())
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
        if (!Metis_Get(config[i].usersetting, help, &help_length))
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
            if (!Metis_Set(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    return Metis_PinReset();
}
