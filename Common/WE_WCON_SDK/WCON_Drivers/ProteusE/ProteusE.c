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
 * @brief Proteus-e driver source file.
 */

#include <ProteusE/ProteusE.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define BTMAC_LENGTH 6

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(PROTEUSE_MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[PROTEUSE_MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ProteusE_CMD_Frame_t;

#define CMD_STX 0x02

#define PROTEUSE_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define PROTEUSE_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define PROTEUSE_CMD_TYPE_IND (uint8_t)(2 << 6)
#define PROTEUSE_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define PROTEUSE_CMD_RESET (uint8_t)0x00
#define PROTEUSE_CMD_RESET_REQ (PROTEUSE_CMD_RESET | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_RESET_CNF (PROTEUSE_CMD_RESET | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GETSTATE (uint8_t)0x01
#define PROTEUSE_CMD_GETSTATE_REQ (PROTEUSE_CMD_GETSTATE | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GETSTATE_CNF (PROTEUSE_CMD_GETSTATE | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_GETSTATE_IND (PROTEUSE_CMD_GETSTATE | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_SLEEP (uint8_t)0x02
#define PROTEUSE_CMD_SLEEP_REQ (PROTEUSE_CMD_SLEEP | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_SLEEP_CNF (PROTEUSE_CMD_SLEEP | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_SLEEP_IND (PROTEUSE_CMD_SLEEP | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_UART_DISABLE (uint8_t)0x1B
#define PROTEUSE_CMD_UART_DISABLE_REQ (PROTEUSE_CMD_UART_DISABLE | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_UART_DISABLE_CNF (PROTEUSE_CMD_UART_DISABLE | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_UART_ENABLE_IND (uint8_t)0x9B

#define PROTEUSE_CMD_DTMSTART (uint8_t)0x1D
#define PROTEUSE_CMD_DTMSTART_REQ (PROTEUSE_CMD_DTMSTART | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_DTMSTART_CNF (PROTEUSE_CMD_DTMSTART | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_DTM (uint8_t)0x1E
#define PROTEUSE_CMD_DTM_REQ (PROTEUSE_CMD_DTM | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_DTM_CNF (PROTEUSE_CMD_DTM | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_DATA (uint8_t)0x04
#define PROTEUSE_CMD_DATA_REQ (PROTEUSE_CMD_DATA | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_DATA_CNF (PROTEUSE_CMD_DATA | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_DATA_IND (PROTEUSE_CMD_DATA | PROTEUSE_CMD_TYPE_IND)
#define PROTEUSE_CMD_TXCOMPLETE_RSP (PROTEUSE_CMD_DATA | PROTEUSE_CMD_TYPE_RSP)

#define PROTEUSE_CMD_CONNECT_IND (uint8_t)0x86
#define PROTEUSE_CMD_CHANNELOPEN_RSP (uint8_t)0xC6

#define PROTEUSE_CMD_DISCONNECT (uint8_t)0x07
#define PROTEUSE_CMD_DISCONNECT_REQ (PROTEUSE_CMD_DISCONNECT | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_DISCONNECT_CNF (PROTEUSE_CMD_DISCONNECT | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_DISCONNECT_IND (PROTEUSE_CMD_DISCONNECT | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_SECURITY_IND (uint8_t)0x88

#define PROTEUSE_CMD_GET (uint8_t)0x10
#define PROTEUSE_CMD_GET_REQ (PROTEUSE_CMD_GET | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GET_CNF (PROTEUSE_CMD_GET | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GET_RAM (uint8_t)0x20
#define PROTEUSE_CMD_GET_RAM_REQ (PROTEUSE_CMD_GET_RAM | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GET_RAM_CNF (PROTEUSE_CMD_GET_RAM | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_SET (uint8_t)0x11
#define PROTEUSE_CMD_SET_REQ (PROTEUSE_CMD_SET | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_SET_CNF (PROTEUSE_CMD_SET | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_SET_RAM (uint8_t)0x21
#define PROTEUSE_CMD_SET_RAM_REQ (PROTEUSE_CMD_SET_RAM | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_SET_RAM_CNF (PROTEUSE_CMD_SET_RAM | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_PHYUPDATE (uint8_t)0x1A
#define PROTEUSE_CMD_PHYUPDATE_REQ (PROTEUSE_CMD_PHYUPDATE | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_PHYUPDATE_CNF (PROTEUSE_CMD_PHYUPDATE | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_PHYUPDATE_IND (PROTEUSE_CMD_PHYUPDATE | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_FACTORYRESET (uint8_t)0x1C
#define PROTEUSE_CMD_FACTORYRESET_REQ (PROTEUSE_CMD_FACTORYRESET | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_FACTORYRESET_CNF (PROTEUSE_CMD_FACTORYRESET | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG (uint8_t)0x25
#define PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG_REQ (PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG_CNF (PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GPIO_LOCAL_READCONFIG (uint8_t)0x2B
#define PROTEUSE_CMD_GPIO_LOCAL_READCONFIG_REQ (PROTEUSE_CMD_GPIO_LOCAL_READCONFIG | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_LOCAL_READCONFIG_CNF (PROTEUSE_CMD_GPIO_LOCAL_READCONFIG | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GPIO_LOCAL_WRITE (uint8_t)0x26
#define PROTEUSE_CMD_GPIO_LOCAL_WRITE_REQ (PROTEUSE_CMD_GPIO_LOCAL_WRITE | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_LOCAL_WRITE_CNF (PROTEUSE_CMD_GPIO_LOCAL_WRITE | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_GPIO_LOCAL_WRITE_IND (PROTEUSE_CMD_GPIO_LOCAL_WRITE | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_GPIO_LOCAL_READ (uint8_t)0x27
#define PROTEUSE_CMD_GPIO_LOCAL_READ_REQ (PROTEUSE_CMD_GPIO_LOCAL_READ | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_LOCAL_READ_CNF (PROTEUSE_CMD_GPIO_LOCAL_READ | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG (uint8_t)0x28
#define PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_REQ (PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_CNF (PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_IND (PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_GPIO_REMOTE_READCONFIG (uint8_t)0x2C
#define PROTEUSE_CMD_GPIO_REMOTE_READCONFIG_REQ (PROTEUSE_CMD_GPIO_REMOTE_READCONFIG | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_REMOTE_READCONFIG_CNF (PROTEUSE_CMD_GPIO_REMOTE_READCONFIG | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GPIO_REMOTE_WRITE (uint8_t)0x29
#define PROTEUSE_CMD_GPIO_REMOTE_WRITE_REQ (PROTEUSE_CMD_GPIO_REMOTE_WRITE | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_REMOTE_WRITE_CNF (PROTEUSE_CMD_GPIO_REMOTE_WRITE | PROTEUSE_CMD_TYPE_CNF)
#define PROTEUSE_CMD_GPIO_REMOTE_WRITE_IND (PROTEUSE_CMD_GPIO_REMOTE_WRITE | PROTEUSE_CMD_TYPE_IND)

#define PROTEUSE_CMD_GPIO_REMOTE_READ (uint8_t)0x2A
#define PROTEUSE_CMD_GPIO_REMOTE_READ_REQ (PROTEUSE_CMD_GPIO_REMOTE_READ | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GPIO_REMOTE_READ_CNF (PROTEUSE_CMD_GPIO_REMOTE_READ | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_GET_BONDS (uint8_t)0x0F
#define PROTEUSE_CMD_GET_BONDS_REQ (PROTEUSE_CMD_GET_BONDS | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_GET_BONDS_CNF (PROTEUSE_CMD_GET_BONDS | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_DELETE_BONDS (uint8_t)0x0E
#define PROTEUSE_CMD_DELETE_BONDS_REQ (PROTEUSE_CMD_DELETE_BONDS | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_DELETE_BONDS_CNF (PROTEUSE_CMD_DELETE_BONDS | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS (uint8_t)0x2D
#define PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS_REQ (PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSE_CMD_TYPE_REQ)
#define PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS_CNF (PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSE_CMD_TYPE_CNF)

#define PROTEUSE_CMD_ERROR_IND (uint8_t)0xA2

/**
 * @brief Type used to check the response, when a command was sent to the Proteus-e
 */
typedef enum ProteusE_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ProteusE_CMD_Status_t;

/**
 * @brief Command confirmation.
 */
typedef struct
{
    uint8_t cmd;                  /**< Variable to check if correct CMD has been confirmed */
    ProteusE_CMD_Status_t status; /**< Variable used to check the response (*_CNF), when a request (*_REQ) was sent to the Proteus-e */
} ProteusE_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ProteusE_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ProteusE_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ProteusE_CMD_Confirmation_t cmdConfirmationArray[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusE_OperationMode_t operationMode = ProteusE_OperationMode_CommandMode;
static ProteusE_DriverState_t bleState;
/**
 * @brief Pin configuration struct pointer.
 */
static ProteusE_Pins_t* ProteusE_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ProteusE_uartP = NULL;

static ProteusE_CallbackConfig_t callbacks;
static WE_UART_HandleRxByte_t byteRxCallback = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ProteusE_CMD_Frame_t)]; /* For UART RX from module */

/**************************************
 *         Static functions           *
 **************************************/

static void ClearReceiveBuffers()
{
    bytesToReceive = 0;
    rxByteCounter = 0;
    checksum = 0;
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
}

static void HandleRxPacket(uint8_t* prxBuffer)
{
    ProteusE_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ProteusE_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case PROTEUSE_CMD_RESET_CNF:
        case PROTEUSE_CMD_GET_CNF:
        case PROTEUSE_CMD_GET_RAM_CNF:
        case PROTEUSE_CMD_SET_CNF:
        case PROTEUSE_CMD_SET_RAM_CNF:
        case PROTEUSE_CMD_PHYUPDATE_CNF:
        case PROTEUSE_CMD_DATA_CNF:
        case PROTEUSE_CMD_DISCONNECT_CNF:
        case PROTEUSE_CMD_FACTORYRESET_CNF:
        case PROTEUSE_CMD_SLEEP_CNF:
        case PROTEUSE_CMD_UART_DISABLE_CNF:
        case PROTEUSE_CMD_UART_ENABLE_IND:
        case PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG_CNF:
        case PROTEUSE_CMD_GPIO_LOCAL_READCONFIG_CNF:
        case PROTEUSE_CMD_GPIO_LOCAL_WRITE_CNF:
        case PROTEUSE_CMD_GPIO_LOCAL_READ_CNF:
        case PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_CNF:
        case PROTEUSE_CMD_GPIO_REMOTE_READCONFIG_CNF:
        case PROTEUSE_CMD_GPIO_REMOTE_WRITE_CNF:
        case PROTEUSE_CMD_GPIO_REMOTE_READ_CNF:
        case PROTEUSE_CMD_GET_BONDS_CNF:
        case PROTEUSE_CMD_DELETE_BONDS_CNF:
        case PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS_CNF:
        case PROTEUSE_CMD_TXCOMPLETE_RSP:
        case PROTEUSE_CMD_DTMSTART_CNF:
        case PROTEUSE_CMD_DTM_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case PROTEUSE_CMD_GETSTATE_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            /* GETSTATE_CNF has no status field */
            cmdConfirmation.status = CMD_Status_NoStatus;

            switch (rxPacket.Data[1])
            {
                case ProteusE_BLE_Action_Idle:
                {
                    bleState = ProteusE_DriverState_BLE_Idle;
                }
                break;
                case ProteusE_BLE_Action_Connected:
                case ProteusE_BLE_Action_None:
                case ProteusE_BLE_Action_Sleep:
                case ProteusE_BLE_Action_DTM:
                default:
                {
                    /* do not use this information */
                }
                break;
            }

            break;
        }

        case PROTEUSE_CMD_CHANNELOPEN_RSP:
        {
            /* Payload of CHANNELOPEN_RSP: Status (1 byte), BTMAC (6 byte), max payload (1 byte) */
            bleState = ProteusE_DriverState_BLE_ChannelOpen;
            if (callbacks.channelOpenCb != NULL)
            {
                callbacks.channelOpenCb(&rxPacket.Data[1], (uint16_t)rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSE_CMD_CONNECT_IND:
        {
            bool success = (rxPacket.Data[0] == CMD_Status_Success);
            if (success)
            {
                bleState = ProteusE_DriverState_BLE_Connected;
            }
            if (callbacks.connectCb != NULL)
            {
                uint8_t btMac[BTMAC_LENGTH];
                if (rxPacket.Length >= 7)
                {
                    memcpy(btMac, &rxPacket.Data[1], sizeof(btMac));
                }
                else
                {
                    /* Packet doesn't contain BTMAC (e.g. connection failed) */
                    memset(btMac, 0, sizeof(btMac));
                }
                callbacks.connectCb(success, btMac);
            }
            break;
        }

        case PROTEUSE_CMD_DISCONNECT_IND:
        {
            bleState = ProteusE_DriverState_BLE_Idle;
            if (callbacks.disconnectCb != NULL)
            {
                ProteusE_DisconnectReason_t reason = ProteusE_DisconnectReason_Unknown;
                switch (rxPacket.Data[0])
                {
                    case 0x08:
                        reason = ProteusE_DisconnectReason_ConnectionTimeout;
                        break;

                    case 0x13:
                        reason = ProteusE_DisconnectReason_UserTerminatedConnection;
                        break;

                    case 0x16:
                        reason = ProteusE_DisconnectReason_HostTerminatedConnection;
                        break;

                    case 0x3B:
                        reason = ProteusE_DisconnectReason_ConnectionIntervalUnacceptable;
                        break;

                    case 0x3D:
                        reason = ProteusE_DisconnectReason_MicFailure;
                        break;

                    case 0x3E:
                        reason = ProteusE_DisconnectReason_ConnectionSetupFailed;
                        break;
                }
                callbacks.disconnectCb(reason);
            }
            break;
        }

        case PROTEUSE_CMD_DATA_IND:
        {
            if (callbacks.rxCb != NULL)
            {
                callbacks.rxCb(&rxPacket.Data[7], rxPacket.Length - 7, &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSE_CMD_SECURITY_IND:
        {
            if (callbacks.securityCb != NULL)
            {
                callbacks.securityCb(&rxPacket.Data[1], rxPacket.Data[0]);
            }
            break;
        }

        case PROTEUSE_CMD_PHYUPDATE_IND:
        {
            if (callbacks.phyUpdateCb != NULL)
            {
                uint8_t btMac[BTMAC_LENGTH];
                if (rxPacket.Length >= 9)
                {
                    memcpy(btMac, &rxPacket.Data[3], sizeof(btMac));
                }
                else
                {
                    /* Packet doesn't contain BTMAC (e.g. Phy update failed) */
                    memset(btMac, 0, sizeof(btMac));
                }
                callbacks.phyUpdateCb((rxPacket.Data[0] == CMD_Status_Success), btMac, (ProteusE_Phy_t)rxPacket.Data[1], (ProteusE_Phy_t)rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSE_CMD_SLEEP_IND:
        {
            if (callbacks.sleepCb != NULL)
            {
                callbacks.sleepCb();
            }
            break;
        }

        case PROTEUSE_CMD_GPIO_LOCAL_WRITE_IND:
        case PROTEUSE_CMD_GPIO_REMOTE_WRITE_IND:
        {
            if (callbacks.gpioWriteCb != NULL)
            {
                uint8_t pos = 0;
                while (pos < rxPacket.Length)
                {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;

                    /* Note that the gpioId parameter is of type uint8_t instead of ProteusE_GPIO_t, as the
				 * remote device may support other GPIOs than this device. */
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t value = rxPacket.Data[2 + pos];
                    callbacks.gpioWriteCb(PROTEUSE_CMD_GPIO_REMOTE_WRITE_IND == rxPacket.Cmd, gpioId, value);

                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_IND:
        {
            if (callbacks.gpioRemoteConfigCb != NULL)
            {
                uint8_t pos = 0;
                while (pos < rxPacket.Length)
                {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t function = rxPacket.Data[2 + pos];
                    uint8_t* value = &rxPacket.Data[3 + pos];

                    ProteusE_GPIOConfigBlock_t gpioConfig = {0};
                    gpioConfig.gpioId = (ProteusE_GPIO_t)gpioId;
                    gpioConfig.function = (ProteusE_GPIO_IO_t)function;
                    switch (gpioConfig.function)
                    {
                        case ProteusE_GPIO_IO_Disconnected:
                            break;

                        case ProteusE_GPIO_IO_Input:
                            gpioConfig.value.input = (ProteusE_GPIO_Input_t)*value;
                            break;

                        case ProteusE_GPIO_IO_Output:
                            gpioConfig.value.output = (ProteusE_GPIO_Output_t)*value;
                            break;
                    }
                    callbacks.gpioRemoteConfigCb(&gpioConfig);
                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSE_CMD_ERROR_IND:
        {
            if (callbacks.errorCb != NULL)
            {
                callbacks.errorCb(rxPacket.Data[0]);
            }
            break;
        }

        default:
        {
            /* invalid */
            break;
        }
    }

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        if (cmdConfirmationArray[i].cmd == CNFINVALID)
        {
            cmdConfirmationArray[i].cmd = cmdConfirmation.cmd;
            cmdConfirmationArray[i].status = cmdConfirmation.status;
            break;
        }
    }
}

void ProteusE_HandleRxByte(uint8_t* dataP, size_t size)
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
                /* length field LSB */
                rxByteCounter++;
                bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
                break;

            case 3:
                /* length field MSB */
                rxByteCounter++;
                bytesToReceive += (((uint16_t)rxBuffer[rxByteCounter - 1] << 8) + LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd + cmd */
                break;

            default:
                /* data field */
                rxByteCounter++;
                if (rxByteCounter >= bytesToReceive)
                {
                    /* check CRC */
                    checksum = 0;
                    for (uint16_t i = 0; i < (bytesToReceive - 1); i++)
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

/**
 * @brief Function that waits for the return value of Proteus-e (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(uint32_t maxTimeMs, uint8_t expectedCmdConfirmation, ProteusE_CMD_Status_t expectedStatus, bool resetConfirmState)
{
    int count = 0;
    int timeStepMs = 5; /* 5ms */
    int maxCount = maxTimeMs / timeStepMs;

    if (resetConfirmState)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmationArray[i].cmd = CNFINVALID;
        }
    }
    while (1)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            if (expectedCmdConfirmation == cmdConfirmationArray[i].cmd)
            {
                return (cmdConfirmationArray[i].status == expectedStatus);
            }
        }

        if (count >= maxCount)
        {
            /* received no correct response within timeout */
            return false;
        }

        /* wait */
        count++;
        WE_Delay(timeStepMs);
    }
    return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(ProteusE_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint16_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
    return true;
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
bool ProteusE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return ProteusE_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Initialize the Proteus-e for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the Proteus-e.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP :         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] opMode:           operation mode
 * @param[in] callbackConfig:   callback configuration
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ProteusE_Init(WE_UART_t* uartP, ProteusE_Pins_t* pinoutP, ProteusE_OperationMode_t opMode, ProteusE_CallbackConfig_t callbackConfig)
{
    operationMode = opMode;

    /* initialize the pins */
    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ProteusE_pinsP = pinoutP;
    ProteusE_pinsP->ProteusE_Pin_Reset.type = WE_Pin_Type_Output;
    ProteusE_pinsP->ProteusE_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ProteusE_pinsP->ProteusE_Pin_Mode.type = WE_Pin_Type_Output;
    ProteusE_pinsP->ProteusE_Pin_Mode.initial_value.output = ((operationMode == ProteusE_OperationMode_TransparentMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);
    ProteusE_pinsP->ProteusE_Pin_StatusLed1.type = WE_Pin_Type_Input;
    ProteusE_pinsP->ProteusE_Pin_StatusLed1.initial_value.input_pull = WE_Pin_PullType_No;
    if (operationMode == ProteusE_OperationMode_CommandMode)
    {
        ProteusE_pinsP->ProteusE_Pin_BusyUartEnable.type = WE_Pin_Type_Output;
        ProteusE_pinsP->ProteusE_Pin_BusyUartEnable.initial_value.output = WE_Pin_Level_High;
    }
    else
    {
        ProteusE_pinsP->ProteusE_Pin_BusyUartEnable.type = WE_Pin_Type_Input;
        ProteusE_pinsP->ProteusE_Pin_BusyUartEnable.initial_value.input_pull = WE_Pin_PullType_No;
    }

    WE_Pin_t pins[sizeof(ProteusE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_BusyUartEnable, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_StatusLed1, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    /* set callback functions */
    callbacks = callbackConfig;
    byteRxCallback = ProteusE_HandleRxByte;

    ProteusE_uartP = uartP;
    if (false == ProteusE_uartP->uartInit(ProteusE_uartP->baudrate, ProteusE_uartP->flowControl, ProteusE_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (!ProteusE_PinReset())
    {
        WE_DEBUG_PRINT_INFO("Pin reset failed\r\n");
        ProteusE_Deinit();
        return false;
    }

    bleState = ProteusE_DriverState_BLE_Idle;

    uint8_t driverVersion[3];
    if (WE_GetDriverVersion(driverVersion))
    {
        WE_DEBUG_PRINT_INFO("Proteus-e driver version %d.%d.%d\n", driverVersion[0], driverVersion[1], driverVersion[2]);
    }
    WE_Delay(100);

    return true;
}

/**
 * @brief Deinitialize the Proteus-e interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ProteusE_Deinit()
{
    WE_Pin_t pins[sizeof(ProteusE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_BusyUartEnable, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusE_pinsP->ProteusE_Pin_StatusLed1, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    /* reset callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    return ProteusE_uartP->uartDeinit();
}

/**
 * @brief Reset the Proteus-e by pin.
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusE_PinReset()
{
    /* set to output mode */
    if (!WE_SetPin(ProteusE_pinsP->ProteusE_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();
    if (!WE_SetPin(ProteusE_pinsP->ProteusE_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ProteusE_OperationMode_TransparentMode)
    {
        WE_Delay(PROTEUSE_BOOT_DURATION);
        /* transparent mode is ready (the module doesn't send a "ready for operation" message in transparent mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

/**
 * @brief Reset the Proteus-e by command.
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusE_Reset()
{
    txPacket.Cmd = PROTEUSE_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

/**
 * @brief Disables the UART of the Proteus-e.
 *
 * It will be re-enabled when the module has to send data to the host (e.g. data was received
 * via radio or a state is indicated) or it can be manually re-enabled using ProteusE_PinUartEnable().
 *
 * @return true if disable succeeded,
 *         false otherwise
 */
bool ProteusE_UartDisable()
{
    txPacket.Cmd = PROTEUSE_CMD_UART_DISABLE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_UART_DISABLE_CNF, CMD_Status_Success, true);
}

/**
 * @brief Re-enables the module's UART using the UART_ENABLE pin after having disabled
 * the UART using ProteusE_UartDisable().
 *
 * @return true if enabling UART succeeded,
 *         false otherwise
 */
bool ProteusE_PinUartEnable()
{
    if (operationMode != ProteusE_OperationMode_CommandMode)
    {
        return false;
    }

    if (!WE_SetPin(ProteusE_pinsP->ProteusE_Pin_BusyUartEnable, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(15);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    if (!WE_SetPin(ProteusE_pinsP->ProteusE_Pin_BusyUartEnable, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for UART enable indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_UART_ENABLE_IND, CMD_Status_Success, false);
}

/**
 * @brief Disconnect the Proteus-e connection if open.
 *
 * @return true if disconnect succeeded,
 *         false otherwise
 */
bool ProteusE_Disconnect()
{
    txPacket.Cmd = PROTEUSE_CMD_DISCONNECT_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* Confirmation is sent before performing the disconnect. After disconnect, the module sends a disconnect indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_DISCONNECT_CNF, CMD_Status_Success, true);
}

/**
 * @brief Put the Proteus-e into sleep mode.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusE_Sleep()
{
    txPacket.Cmd = PROTEUSE_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_SLEEP_CNF, CMD_Status_Success, true);
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
bool ProteusE_Transmit(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length == 0) || (length > PROTEUSE_MAX_RADIO_PAYLOAD_LENGTH) || (ProteusE_DriverState_BLE_ChannelOpen != ProteusE_GetDriverState()))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

/*
 * @brief Factory reset of the module.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusE_FactoryReset()
{
    txPacket.Cmd = PROTEUSE_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for reset after factory reset */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
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
bool ProteusE_CheckNSet(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    uint8_t current_value[length];
    uint16_t current_length = length;

    if (true == ProteusE_Get(userSetting, current_value, &current_length))
    {
        if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
        {
            /* value is already set, no need to set it again */
            return true;
        }
        else
        {
            /* value differs, and thus must be set */
            return ProteusE_Set(userSetting, valueP, length);
        }
    }
    else
    {
        /* failed reading current value */
        return false;
    }
}

/**
 * @brief Sets the supplied user setting.
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
bool ProteusE_Set(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_SET_REQ;
    txPacket.Length = (1 + length);
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_SET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

/**
 * @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] deviceNameP: pointer to the device name (allowed characters are 0x20 - 0x7E)
 * @param[in] nameLength:  length of the device name (max 31 characters)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength) { return ProteusE_Set(ProteusE_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLength); }

/**
 * @brief Set the BLE advertising timeout.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] advTimeout: advertising timeout in seconds (allowed values: 0-650, where 0 = infinite)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetAdvertisingTimeout(uint16_t advTimeout)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&advTimeout, 2);
    return ProteusE_Set(ProteusE_USERSETTING_RF_ADVERTISING_TIMEOUT, help, 2);
}

/**
 * @brief Sets the minimum and maximum connection interval, which is used to negotiate
 * the connection interval during connection setup.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] minIntervalMs: Minimum connection interval in ms (must be between 8 ms and 4000 ms)
 * @param[in] maxIntervalMs: Maximum connection interval in ms (must be between 8 ms and 4000 ms, must be >= minIntervalMs)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetConnectionInterval(uint16_t minIntervalMs, uint16_t maxIntervalMs)
{
    uint8_t help[4];
    memcpy(help, (uint8_t*)&minIntervalMs, 2);
    memcpy(help + 2, (uint8_t*)&maxIntervalMs, 2);
    return ProteusE_Set(ProteusE_USERSETTING_RF_CONNECTION_INTERVAL, help, 4);
}

/**
 * @brief Sets the advertising interval, which defines how often advertising packets are transmitted.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] intervalMs: Advertising interval in ms (must be between 20 ms and 10240 ms)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetAdvertisingInterval(uint16_t intervalMs)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&intervalMs, 2);
    return ProteusE_Set(ProteusE_USERSETTING_RF_ADVERTISING_INTERVAL, help, 2);
}

/**
 * @brief Set the CFG flags (see ProteusE_CfgFlags_t)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] cfgFlags: CFG flags (see ProteusE_CfgFlags_t)
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusE_SetCFGFlags(uint16_t cfgFlags)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&cfgFlags, 2);
    return ProteusE_Set(ProteusE_USERSETTING_RF_CFGFLAGS, help, 2);
}

/**
 * @brief Set the BLE TX power.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] txPower: TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetTXPower(ProteusE_TXPower_t txPower) { return ProteusE_Set(ProteusE_USERSETTING_RF_TX_POWER, (uint8_t*)&txPower, 1); }

/**
 * @brief Set the BLE security flags.
 *
 * Note: When updating this user setting (like enabling bonding or changing the security
 * mode) please remove all existing bonding data using ProteusE_DeleteBonds().
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetSecFlags(ProteusE_SecFlags_t secFlags) { return ProteusE_Set(ProteusE_USERSETTING_RF_SEC_FLAGS, (uint8_t*)&secFlags, 1); }

/**
 * @brief Sets the content of the advertising packet (persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in advertising packet
 * @param[in] length: Length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetAdvertisingData(uint8_t* dataP, uint16_t length) { return ProteusE_Set(ProteusE_USERSETTING_RF_ADVERTISING_DATA, dataP, length); }

/**
 * @brief Sets the content of the scan response packet (persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in scan response packet
 * @param[in] length: Length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetScanResponseData(uint8_t* dataP, uint16_t length) { return ProteusE_Set(ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA, dataP, length); }

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate
 * @param[in] parity: parity bit
 * @param[in] flowControlEnable: enable/disable flow control
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetBaudrateIndex(ProteusE_BaudRate_t baudrate, ProteusE_UartParity_t parity, bool flowControlEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flow control is to be enabled UART index has to be increased by one in regard to base value */
    if (flowControlEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value */
    if (ProteusE_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ProteusE_Set(ProteusE_USERSETTING_UART_CONFIG_INDEX, (uint8_t*)&baudrateIndex, 1);
}

/**
 * @brief Sets the Bluetooth MAC address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] btMacP: Pointer to the MAC (of 6 bytes)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetBTMAC(uint8_t* btMacP) { return ProteusE_Set(ProteusE_USERSETTING_FS_BTMAC, btMacP, 6); }

/**
 * @brief Set the BLE static passkey
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] staticPasskeyP: pointer to the static passkey (6 digits)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetStaticPasskey(uint8_t* staticPasskeyP) { return ProteusE_Set(ProteusE_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, 6); }

/**
 * @brief Sets the Bluetooth appearance of the device
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] appearance: 2 byte Bluetooth appearance value (please check the Bluetooth Core
 *                        Specification: Core Specification Supplement, Part A, section 1.12
 *                        for permissible values)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetAppearance(uint16_t appearance)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&appearance, 2);
    return ProteusE_Set(ProteusE_USERSETTING_RF_APPEARANCE, help, 2);
}

/**
 * @brief Sets the base UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 16 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetSppBaseUuid(uint8_t* uuidP) { return ProteusE_Set(ProteusE_USERSETTING_RF_SPPBASEUUID, uuidP, 16); }

/**
 * @brief Sets the service UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetSppServiceUuid(uint8_t* uuidP) { return ProteusE_Set(ProteusE_USERSETTING_RF_SPPServiceUUID, uuidP, 2); }

/**
 * @brief Sets the RX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetSppRxUuid(uint8_t* uuidP) { return ProteusE_Set(ProteusE_USERSETTING_RF_SPPRXUUID, uuidP, 2); }

/**
 * @brief Sets the TX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetSppTxUuid(uint8_t* uuidP) { return ProteusE_Set(ProteusE_USERSETTING_RF_SPPTXUUID, uuidP, 2); }

/**
 * @brief Request the current user settings
 *
 * @param[in] userSetting: user setting to be requested
 * @param[out] responseP: pointer of the memory to put the requested content
 * @param[out] responseLengthP: length of the requested content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_Get(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    if ((responseP == NULL) || (responseLengthP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GET_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;
        memcpy(responseP, &rxPacket.Data[1], length - 1); /* First data byte is status, following bytes response */
        *responseLengthP = length - 1;
        return true;
    }
    return false;
}

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: pointer to the 3 byte firmware version, version is returned MSB first
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetFWVersion(uint8_t* versionP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_FS_FWVersion, versionP, &length);
}

/**
 * @brief Request device info.
 *
 * @param[out] deviceInfoP: pointer to the device info structure
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusE_GetDeviceInfo(ProteusE_DeviceInfo_t* deviceInfoP)
{
    if (deviceInfoP == NULL)
    {
        return false;
    }

    uint8_t help[12];
    uint16_t length;
    if (!ProteusE_Get(ProteusE_USERSETTING_FS_DEVICE_INFO, help, &length))
    {
        return false;
    }
    memcpy(&deviceInfoP->osVersion, help, 2);
    memcpy(&deviceInfoP->buildCode, help + 2, 4);
    memcpy(&deviceInfoP->packageVariant, help + 6, 2);
    memcpy(&deviceInfoP->chipId, help + 8, 4);
    return true;
}

/**
 * @brief Request the 3 byte serial number.
 *
 * @param[out] serialNumberP: pointer to the 3 byte serial number (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length;
    if (!ProteusE_Get(ProteusE_USERSETTING_FS_SERIAL_NUMBER, serialNumberP, &length))
    {
        return false;
    }
    return true;
}

/**
 * @brief Request the current BLE device name.
 *
 * @param[out] deviceNameP: pointer to device name (allowed characters are 0x20 - 0x7E)
 * @param[out] nameLengthP: pointer to the length of the device name (max 31 characters)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP) { return ProteusE_Get(ProteusE_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLengthP); }

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: pointer to the MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetMAC(uint8_t* macP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_FS_MAC, macP, &length);
}

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: pointer to the Bluetooth MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetBTMAC(uint8_t* btMacP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_FS_BTMAC, btMacP, &length);
}

/**
 * @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: pointer to the advertising timeout
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetAdvertisingTimeout(uint16_t* advTimeoutP)
{
    if (advTimeoutP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusE_Get(ProteusE_USERSETTING_RF_ADVERTISING_TIMEOUT, help, &length))
    {
        memcpy((uint8_t*)advTimeoutP, help, 2);
        return true;
    }

    return false;
}

/**
 * @brief Requests the connection interval.
 *
 * @param[out] minIntervalMsP: Pointer to the minimum connection interval
 * @param[out] maxIntervalMsP: Pointer to the maximum connection interval
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetConnectionInterval(uint16_t* minIntervalMsP, uint16_t* maxIntervalMsP)
{
    if ((minIntervalMsP == NULL) || (maxIntervalMsP == NULL))
    {
        return false;
    }

    uint16_t length;
    uint8_t help[4];

    if (ProteusE_Get(ProteusE_USERSETTING_RF_CONNECTION_INTERVAL, help, &length))
    {
        memcpy((uint8_t*)minIntervalMsP, help, 2);
        memcpy((uint8_t*)maxIntervalMsP, help + 2, 2);
        return true;
    }

    return false;
}

/**
 * @brief Requests the advertising interval.
 *
 * @param[out] intervalMsP: Pointer to the advertising interval
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetAdvertisingInterval(uint16_t* intervalMsP)
{
    if (intervalMsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusE_Get(ProteusE_USERSETTING_RF_ADVERTISING_INTERVAL, help, &length))
    {
        memcpy((uint8_t*)intervalMsP, help, 2);
        return true;
    }

    return false;
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetTXPower(ProteusE_TXPower_t* txPowerP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_TX_POWER, (uint8_t*)txPowerP, &length);
}

/**
 * @brief Request the security flags
 *
 * @param[out] secFlagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSecFlags(ProteusE_SecFlags_t* secFlagsP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_SEC_FLAGS, (uint8_t*)secFlagsP, &length);
}

/**
 * @brief Request the (custom) content of the advertising packet (persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of advertising packet
 * @param[in] length: Pointer to length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetAdvertisingData(uint8_t* dataP, uint16_t* lengthP) { return ProteusE_Get(ProteusE_USERSETTING_RF_ADVERTISING_DATA, dataP, lengthP); }

/**
 * @brief Request the (custom) content of the scan response packet (persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of scan response packet
 * @param[in] length: Pointer to length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetScanResponseData(uint8_t* dataP, uint16_t* lengthP) { return ProteusE_Get(ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA, dataP, lengthP); }

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateP: pointer to the UART baudrate index
 * @param[out] parityP: pointer to the UART parity
 * @param[out] flowControlEnableP: pointer to the UART flow control parameter
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusE_GetBaudrateIndex(ProteusE_BaudRate_t* baudrateP, ProteusE_UartParity_t* parityP, bool* flowControlEnableP)
{
    if ((baudrateP == NULL) || (parityP == NULL) || (flowControlEnableP == NULL))
    {
        return false;
    }

    uint16_t length;
    uint8_t uartIndex;

    if (ProteusE_Get(ProteusE_USERSETTING_UART_CONFIG_INDEX, (uint8_t*)&uartIndex, &length))
    {
        /* if index is even, flow control is off.
		 * If flow control is on, decrease index by one to later determine the base baudrate */
        if (0x01 == (uartIndex & 0x01))
        {
            /* odd */
            *flowControlEnableP = true;
            uartIndex--;
        }
        else
        {
            /* even */
            *flowControlEnableP = false;
        }

        /* If baudrate index is greater than or equal to 64, parity bit is even*/
        if (uartIndex < 64)
        {
            *parityP = ProteusE_UartParity_None;
        }
        else
        {
            *parityP = ProteusE_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ProteusE_BaudRate_t)uartIndex;
        return true;
    }
    return false;
}

/**
 * @brief Request the BLE static passkey
 *
 * @param[out] staticPasskeyP: pointer to the static passkey (6 digits)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetStaticPasskey(uint8_t* staticPasskeyP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, &length);
}

/**
 * @brief Request the Bluetooth appearance of the device
 *
 * @param[out] appearanceP: pointer to the Bluetooth appearance
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetAppearance(uint16_t* appearanceP)
{
    if (appearanceP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusE_Get(ProteusE_USERSETTING_RF_APPEARANCE, help, &length))
    {
        memcpy((uint8_t*)appearanceP, help, 2);
        return true;
    }

    return false;
}

/**
 * @brief Request the base UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 16 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSppBaseUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_SPPBASEUUID, uuidP, &length);
}

/**
 * @brief Request the service UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSppServiceUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_SPPServiceUUID, uuidP, &length);
}

/**
 * @brief Request the RX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSppRxUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_SPPRXUUID, uuidP, &length);
}

/**
 * @brief Request the TX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetSppTxUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusE_Get(ProteusE_USERSETTING_RF_SPPTXUUID, uuidP, &length);
}

/**
 * @brief Request the CFG flags (see ProteusE_CfgFlags_t)
 *
 * @param[out] cfgFlagsP: pointer to the CFG flags (see ProteusE_CfgFlags_t)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetCFGFlags(uint16_t* cfgFlagsP)
{
    if (cfgFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusE_Get(ProteusE_USERSETTING_RF_CFGFLAGS, help, &length))
    {
        memcpy((uint8_t*)cfgFlagsP, help, 2);
        return true;
    }

    return false;
}

/**
 * @brief Request the module state
 *
 * @param[out] moduleStateP: Pointer to module state
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetState(ProteusE_ModuleState_t* moduleStateP)
{
    if (moduleStateP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        moduleStateP->role = rxPacket.Data[0];
        moduleStateP->action = rxPacket.Data[1];

        if (moduleStateP->action == ProteusE_BLE_Action_Connected && rxPacket.Length >= 9)
        {
            memcpy(moduleStateP->connectedDeviceBtMac, &rxPacket.Data[2], 6);
            moduleStateP->connectedDeviceMaxPayloadSize = rxPacket.Data[8];
        }
        else
        {
            memset(moduleStateP->connectedDeviceBtMac, 0, 6);
            moduleStateP->connectedDeviceMaxPayloadSize = 0;
        }

        return true;
    }
    return false;
}

/**
 * @brief Request the current state of the driver
 *
 * @return driver state
 */
ProteusE_DriverState_t ProteusE_GetDriverState() { return bleState; }

/**
 * @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_PhyUpdate(ProteusE_Phy_t phy)
{
    if (ProteusE_DriverState_BLE_ChannelOpen == ProteusE_GetDriverState())
    {
        txPacket.Cmd = PROTEUSE_CMD_PHYUPDATE_REQ;
        txPacket.Length = 1;
        txPacket.Data[0] = (uint8_t)phy;

        FillChecksum(&txPacket);
        if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
        {
            return false;
        }

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_PHYUPDATE_CNF, CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Returns the current level of the status pin.
 *
 * @param[out] statusPinLed1LevelP: status pin level
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetStatusPinLed1Level(WE_Pin_Level_t* statusPinLed1LevelP) { return WE_GetPinLevel(ProteusE_pinsP->ProteusE_Pin_StatusLed1, statusPinLed1LevelP); }

/**
 * @brief Returns the current level of the BUSY/UART_ENABLE pin.
 *
 * @param[out] busyStateP: busy pin state
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_IsTransparentModeBusy(bool* busyStateP)
{
    if (busyStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(ProteusE_pinsP->ProteusE_Pin_BusyUartEnable, &pin_level))
    {
        return false;
    }

    *busyStateP = (pin_level == WE_Pin_Level_High);

    return true;
}

/**
 * @brief Sets the callback function which is executed if a byte has been received from Proteus-e.
 *
 * The default callback is ProteusE_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default callback is used if NULL)
 */
void ProteusE_SetByteRxCallback(WE_UART_HandleRxByte_t callback) { byteRxCallback = (callback == NULL) ? ProteusE_HandleRxByte : callback; }

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIOLocalWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs)
{
    if (configP == NULL)
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++)
    {
        switch (configP->function)
        {
            case ProteusE_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusE_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusE_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            default:
            {
                break;
            }
        }

        /* Move pointer to next element. configP is increased by sizeof(ProteusE_GPIOConfigBlock_t)*/
        configP++;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_LOCAL_WRITECONFIG_CNF, CMD_Status_Success, true);
}

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIOLocalReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP)
{
    if ((configP == NULL) || (numberOfConfigsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_LOCAL_READCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_LOCAL_READCONFIG_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfConfigsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusE_GPIOConfigBlock_t* configP_running = configP;
        while (uartP < &rxPacket.Data[length])
        {
            switch (*(uartP + 2))
            {
                case ProteusE_GPIO_IO_Disconnected:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusE_GPIO_IO_Input:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.input = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusE_GPIO_IO_Output:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.output = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                default:
                    break;
            }

            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ProteusE_GPIOLocalWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIOLocalWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls)
{
    if ((controlP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;

        /* Move pointer to next element. configP is increased by sizeof(ProteusE_GPIOControlBlock_t)*/
        controlP++;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
}

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ProteusE_GPIOLocalWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIOLocalRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP)
{
    if ((gpioToReadP == NULL) || (controlP == NULL) || (numberOfControlsP == NULL) || (amountGPIOToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfControlsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusE_GPIOControlBlock_t* controlP_running = controlP;
        while (uartP < &rxPacket.Data[length])
        {
            /* each ControlBlock starts with length field which is currently fixed to "2" */
            if (*uartP == 2)
            {
                controlP_running->gpioId = *(uartP + 1);
                controlP_running->value.output = *(uartP + 2);

                /* Move pointer to next element. configP is increased by sizeof(ProteusE_GPIOControlBlock_t)*/
                controlP_running++;
                *numberOfControlsP += 1;
            }

            /* uartP points to length field of control block. So increase address by value of length + 1 */
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIORemoteWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs)
{
    if ((configP == NULL) || (numberOfConfigs == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++)
    {
        switch (configP->function)
        {
            case ProteusE_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusE_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusE_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            default:
                break;
        }
        configP++;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_REMOTE_WRITECONFIG_CNF, CMD_Status_Success, true);
}

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIORemoteReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP)
{
    if ((configP == NULL) || (numberOfConfigsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_REMOTE_READCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_REMOTE_READCONFIG_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfConfigsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusE_GPIOConfigBlock_t* configP_running = configP;
        while (uartP < &rxPacket.Data[length])
        {
            switch (*(uartP + 2))
            {
                case ProteusE_GPIO_IO_Disconnected:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusE_GPIO_IO_Input:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.input = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusE_GPIO_IO_Output:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.output = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                default:
                    break;
            }
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

/**
 * @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ProteusE_GPIORemoteWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIORemoteWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls)
{
    if ((controlP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;
        controlP++;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
}

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ProteusE_GPIORemoteWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of gpioToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GPIORemoteRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP)
{
    if ((gpioToReadP == NULL) || (controlP == NULL) || (numberOfControlsP == NULL) || (amountGPIOToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GPIO_REMOTE_READ_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfControlsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusE_GPIOControlBlock_t* controlP_running = controlP;
        while (uartP < &rxPacket.Data[length])
        {
            if (*uartP == 2)
            {
                controlP_running->gpioId = *(uartP + 1);
                controlP_running->value.output = *(uartP + 2);

                controlP_running++;
                *numberOfControlsP += 1;
            }
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

/**
 * @brief Requests the BTMAC addresses of all bonded devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetBonds(ProteusE_BondDatabase_t* bondDatabaseP)
{
    if (bondDatabaseP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSE_CMD_GET_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GET_BONDS_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    bondDatabaseP->nrOfDevices = rxPacket.Data[1];
    if (bondDatabaseP->nrOfDevices > PROTEUSE_MAX_BOND_DEVICES)
    {
        bondDatabaseP->nrOfDevices = PROTEUSE_MAX_BOND_DEVICES;
    }

    for (uint8_t i = 0; i < bondDatabaseP->nrOfDevices; i++)
    {
        uint8_t offset = 2 + i * 8;
        bondDatabaseP->devices[i].id = ((uint16_t)rxPacket.Data[offset] << 0) + ((uint16_t)rxPacket.Data[offset + 1] << 8);
        memcpy(bondDatabaseP->devices[i].btMac, &rxPacket.Data[offset + 2], 6);
    }

    return true;
}

/**
 * @brief Removes all bonding data.
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DeleteBonds()
{
    txPacket.Cmd = PROTEUSE_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

/**
 * @brief Removes the bonding information for a single device.
 *
 * @param[in] bondId: bond ID of the device to be removed
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DeleteBond(uint8_t bondId)
{
    txPacket.Cmd = PROTEUSE_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = bondId;
    txPacket.Data[1] = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

/**
 * @brief Temporarily allow unbonded connections, in case only bonded connections have been configured
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_AllowUnbondedConnections()
{
    txPacket.Cmd = PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_ALLOWUNBONDEDCONNECTIONS_CNF, CMD_Status_Success, true);
}

/**
 * @brief Sets the supplied user setting in RAM (not persistent).
 *
 * @param[in] userSetting:  user setting to be updated
 * @param[in] valueP:       pointer to the new settings value (not persistent)
 * @param[in] length:       length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetRAM(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    txPacket.Cmd = PROTEUSE_CMD_SET_RAM_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_SET_RAM_CNF, CMD_Status_Success, true);
}

/**
 * @brief Sets the content of the advertising packet in RAM (not persistent)
 *
 * @param[in] dataP: Content to put in advertising packet
 * @param[in] length: Length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetAdvertisingDataRAM(uint8_t* dataP, uint8_t length) { return ProteusE_SetRAM(ProteusE_USERSETTING_RF_ADVERTISING_DATA, dataP, length); }

/**
 * @brief Sets the content of the scan response packet in RAM (not persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in scan response packet
 * @param[in] length: Length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_SetScanResponseDataRAM(uint8_t* dataP, uint8_t length) { return ProteusE_SetRAM(ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA, dataP, length); }

/**
 * @brief Request the current user settings in RAM (not persistent)
 *
 * @param[in] userSetting: user setting to be requested (not persistent)
 * @param[out] responseP: pointer of the memory to put the requested content
 * @param[out] responseLengthP: length of the requested content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetRAM(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    txPacket.Cmd = PROTEUSE_CMD_GET_RAM_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GET_RAM_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;
        memcpy(responseP, &rxPacket.Data[1], length - 1); /* First data byte is status, following bytes response */
        *responseLengthP = length - 1;
        return true;
    }
    return false;
}

/**
 * @brief Request the (custom) content of the advertising packet from RAM (not persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of advertising packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetAdvertisingDataRAM(uint8_t* dataP, uint16_t* lengthP) { return ProteusE_GetRAM(ProteusE_USERSETTING_RF_ADVERTISING_DATA, dataP, lengthP); }

/**
 * @brief Request the (custom) content of the scan response packet from RAM (not persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of scan response packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_GetScanResponseDataRAM(uint8_t* dataP, uint16_t* lengthP) { return ProteusE_GetRAM(ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA, dataP, lengthP); }

/**
 * @brief Enable the direct test mode (DTM)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMEnable()
{
    txPacket.Cmd = PROTEUSE_CMD_DTMSTART_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        if ((rxPacket.Data[0] == (uint8_t)ProteusE_BLE_Role_DTM) && (rxPacket.Data[1] == (uint8_t)ProteusE_BLE_Action_DTM))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Run a direct test mode (DTM) command
 *
 * @param[in] command: Command
 * @param[in] channel_vendoroption: Channel or vendor option
 * @param[in] length_vendorcmd: Length or vendor command
 * @param[in] payload: Payload
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMRun(ProteusE_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload)
{
    txPacket.Cmd = PROTEUSE_CMD_DTM_REQ;
    txPacket.Length = 4;
    txPacket.Data[0] = command;
    txPacket.Data[1] = channel_vendoroption;
    txPacket.Data[2] = length_vendorcmd;
    txPacket.Data[3] = payload;

    FillChecksum(&txPacket);
    if (!ProteusE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSE_CMD_DTM_CNF, CMD_Status_Success, true);
}

/**
 * @brief Start the direct test mode (DTM) TX test
 *
 * @param[in] channel: Channel
 * @param[in] length: Number of patterns
 * @param[in] pattern: Pattern to send
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMStartTX(uint8_t channel, uint8_t length, ProteusE_DTMTXPattern_t pattern) { return ProteusE_DTMRun(ProteusE_DTMCommand_StartTX, channel, length, (uint8_t)pattern); }

/**
 * @brief Start the direct test mode (DTM) TX carrier test
 *
 * @param[in] channel: Channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMStartTXCarrier(uint8_t channel) { return ProteusE_DTMRun(ProteusE_DTMCommand_StartTX, channel, 0x00, 0x03); }

/**
 * @brief Stop the current direct test mode (DTM) test
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMStop() { return ProteusE_DTMRun(ProteusE_DTMCommand_Stop, 0x00, 0x00, 0x01); }

/**
 * @brief Set the phy for direct test mode (DTM) test
 *
 * @param[in] phy: Phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMSetPhy(ProteusE_Phy_t phy) { return ProteusE_DTMRun(ProteusE_DTMCommand_Setup, 0x02, (uint8_t)phy, 0x00); }

/**
 * @brief Set the TX power for direct test mode (DTM) test
 *
 * @param[in] power: Tx power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusE_DTMSetTXPower(ProteusE_TXPower_t power) { return ProteusE_DTMRun(ProteusE_DTMCommand_StartTX, (uint8_t)power, 0x02, 0x03); }
