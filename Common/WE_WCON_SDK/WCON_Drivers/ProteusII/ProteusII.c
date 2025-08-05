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
 * @brief Proteus-II driver source file.
 */

#include <ProteusII/ProteusII.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define BTMAC_LENGTH 6

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(PROTEUSII_MAX_CMD_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[PROTEUSII_MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ProteusII_CMD_Frame_t;

#define CMD_STX 0x02

#define PROTEUSII_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define PROTEUSII_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define PROTEUSII_CMD_TYPE_IND (uint8_t)(2 << 6)
#define PROTEUSII_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define PROTEUSII_CMD_RESET (uint8_t)0x00
#define PROTEUSII_CMD_RESET_REQ (PROTEUSII_CMD_RESET | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_RESET_CNF (PROTEUSII_CMD_RESET | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_GETSTATE (uint8_t)0x01
#define PROTEUSII_CMD_GETSTATE_REQ (PROTEUSII_CMD_GETSTATE | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_GETSTATE_CNF (PROTEUSII_CMD_GETSTATE | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_GETSTATE_IND (PROTEUSII_CMD_GETSTATE | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_SLEEP (uint8_t)0x02
#define PROTEUSII_CMD_SLEEP_REQ (PROTEUSII_CMD_SLEEP | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_SLEEP_CNF (PROTEUSII_CMD_SLEEP | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_SLEEP_IND (PROTEUSII_CMD_SLEEP | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_UART_DISABLE (uint8_t)0x1B
#define PROTEUSII_CMD_UART_DISABLE_REQ (PROTEUSII_CMD_UART_DISABLE | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_UART_DISABLE_CNF (PROTEUSII_CMD_UART_DISABLE | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_UART_ENABLE_IND (uint8_t)0x9B

#define PROTEUSII_CMD_DATA (uint8_t)0x04
#define PROTEUSII_CMD_DATA_REQ (PROTEUSII_CMD_DATA | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_DATA_CNF (PROTEUSII_CMD_DATA | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_DATA_IND (PROTEUSII_CMD_DATA | PROTEUSII_CMD_TYPE_IND)
#define PROTEUSII_CMD_TXCOMPLETE_RSP (PROTEUSII_CMD_DATA | PROTEUSII_CMD_TYPE_RSP)

#define PROTEUSII_CMD_CONNECT (uint8_t)0x06
#define PROTEUSII_CMD_CONNECT_REQ (PROTEUSII_CMD_CONNECT | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_CONNECT_CNF (PROTEUSII_CMD_CONNECT | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_CONNECT_IND (PROTEUSII_CMD_CONNECT | PROTEUSII_CMD_TYPE_IND)
#define PROTEUSII_CMD_CHANNELOPEN_RSP (PROTEUSII_CMD_CONNECT | PROTEUSII_CMD_TYPE_RSP)

#define PROTEUSII_CMD_DISCONNECT (uint8_t)0x07
#define PROTEUSII_CMD_DISCONNECT_REQ (PROTEUSII_CMD_DISCONNECT | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_DISCONNECT_CNF (PROTEUSII_CMD_DISCONNECT | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_DISCONNECT_IND (PROTEUSII_CMD_DISCONNECT | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_SECURITY_IND (uint8_t)0x88

#define PROTEUSII_CMD_SCANSTART (uint8_t)0x09
#define PROTEUSII_CMD_SCANSTART_REQ (PROTEUSII_CMD_SCANSTART | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_SCANSTART_CNF (PROTEUSII_CMD_SCANSTART | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_SCANSTART_IND (PROTEUSII_CMD_SCANSTART | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_SCANSTOP (uint8_t)0x0A
#define PROTEUSII_CMD_SCANSTOP_REQ (PROTEUSII_CMD_SCANSTOP | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_SCANSTOP_CNF (PROTEUSII_CMD_SCANSTOP | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_SCANSTOP_IND (PROTEUSII_CMD_SCANSTOP | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_GETDEVICES (uint8_t)0x0B
#define PROTEUSII_CMD_GETDEVICES_REQ (PROTEUSII_CMD_GETDEVICES | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_GETDEVICES_CNF (PROTEUSII_CMD_GETDEVICES | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_SETBEACON (uint8_t)0x0C
#define PROTEUSII_CMD_SETBEACON_REQ (PROTEUSII_CMD_SETBEACON | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_SETBEACON_CNF (PROTEUSII_CMD_SETBEACON | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_BEACON_IND (PROTEUSII_CMD_SETBEACON | PROTEUSII_CMD_TYPE_IND)
#define PROTEUSII_CMD_BEACON_RSP (PROTEUSII_CMD_SETBEACON | PROTEUSII_CMD_TYPE_RSP)
#define PROTEUSII_CMD_RSSI_IND (uint8_t)0x8B

#define PROTEUSII_CMD_PASSKEY (uint8_t)0x0D
#define PROTEUSII_CMD_PASSKEY_REQ (PROTEUSII_CMD_PASSKEY | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_PASSKEY_CNF (PROTEUSII_CMD_PASSKEY | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_PASSKEY_IND (PROTEUSII_CMD_PASSKEY | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_GET (uint8_t)0x10
#define PROTEUSII_CMD_GET_REQ (PROTEUSII_CMD_GET | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_GET_CNF (PROTEUSII_CMD_GET | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_SET (uint8_t)0x11
#define PROTEUSII_CMD_SET_REQ (PROTEUSII_CMD_SET | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_SET_CNF (PROTEUSII_CMD_SET | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_PHYUPDATE (uint8_t)0x1A
#define PROTEUSII_CMD_PHYUPDATE_REQ (PROTEUSII_CMD_PHYUPDATE | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_PHYUPDATE_CNF (PROTEUSII_CMD_PHYUPDATE | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_PHYUPDATE_IND (PROTEUSII_CMD_PHYUPDATE | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_FACTORYRESET (uint8_t)0x1C
#define PROTEUSII_CMD_FACTORYRESET_REQ (PROTEUSII_CMD_FACTORYRESET | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_FACTORYRESET_CNF (PROTEUSII_CMD_FACTORYRESET | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_DTMSTART (uint8_t)0x1D
#define PROTEUSII_CMD_DTMSTART_REQ (PROTEUSII_CMD_DTMSTART | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_DTMSTART_CNF (PROTEUSII_CMD_DTMSTART | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_DTM (uint8_t)0x1E
#define PROTEUSII_CMD_DTM_REQ (PROTEUSII_CMD_DTM | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_DTM_CNF (PROTEUSII_CMD_DTM | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_NUMERIC_COMP (uint8_t)0x24
#define PROTEUSII_CMD_NUMERIC_COMP_REQ (PROTEUSII_CMD_NUMERIC_COMP | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_NUMERIC_COMP_CNF (PROTEUSII_CMD_NUMERIC_COMP | PROTEUSII_CMD_TYPE_CNF)
#define PROTEUSII_CMD_DISPLAY_PASSKEY_IND (PROTEUSII_CMD_NUMERIC_COMP | PROTEUSII_CMD_TYPE_IND)

#define PROTEUSII_CMD_GET_BONDS (uint8_t)0x0F
#define PROTEUSII_CMD_GET_BONDS_REQ (PROTEUSII_CMD_GET_BONDS | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_GET_BONDS_CNF (PROTEUSII_CMD_GET_BONDS | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_DELETE_BONDS (uint8_t)0x0E
#define PROTEUSII_CMD_DELETE_BONDS_REQ (PROTEUSII_CMD_DELETE_BONDS | PROTEUSII_CMD_TYPE_REQ)
#define PROTEUSII_CMD_DELETE_BONDS_CNF (PROTEUSII_CMD_DELETE_BONDS | PROTEUSII_CMD_TYPE_CNF)

#define PROTEUSII_CMD_ERROR_IND (uint8_t)0xA2

/**
 * @brief Type used to check the response, when a command was sent to the ProteusII
 */
typedef enum ProteusII_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ProteusII_CMD_Status_t;

/**
 * @brief Command confirmation.
 */
typedef struct
{
    uint8_t cmd;                   /**< Variable to check if correct CMD has been confirmed */
    ProteusII_CMD_Status_t status; /**< Variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ProteusII */
} ProteusII_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ProteusII_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ProteusII_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ProteusII_CMD_Confirmation_t cmdConfirmationArray[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusII_OperationMode_t operationMode = ProteusII_OperationMode_CommandMode;
static ProteusII_GetDevices_t* ProteusII_getDevicesP = NULL;
static ProteusII_DriverState_t bleState;
/**
 * @brief Pin configuration struct pointer.
 */
static ProteusII_Pins_t* ProteusII_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ProteusII_uartP = NULL;

static ProteusII_CallbackConfig_t callbacks;
static WE_UART_HandleRxByte_t byteRxCallback = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ProteusII_CMD_Frame_t)]; /* For UART RX from module */

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
    ProteusII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ProteusII_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case PROTEUSII_CMD_GETDEVICES_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            if ((cmdConfirmation.status == CMD_Status_Success) && (ProteusII_getDevicesP != NULL))
            {
                uint8_t size = rxPacket.Data[1];
                if (size >= PROTEUSII_MAX_NUMBER_OF_DEVICES)
                {
                    size = PROTEUSII_MAX_NUMBER_OF_DEVICES;
                }
                ProteusII_getDevicesP->numberOfDevices = size;

                uint16_t len = 2;
                for (uint8_t i = 0; i < ProteusII_getDevicesP->numberOfDevices; i++)
                {
                    memcpy(&ProteusII_getDevicesP->devices[i].btmac[0], &rxPacket.Data[len], 6);
                    ProteusII_getDevicesP->devices[i].rssi = rxPacket.Data[len + 6];
                    ProteusII_getDevicesP->devices[i].txPower = rxPacket.Data[len + 7];
                    ProteusII_getDevicesP->devices[i].deviceNameLength = rxPacket.Data[len + 8];
                    memcpy(&ProteusII_getDevicesP->devices[i].deviceName[0], &rxPacket.Data[len + 9], ProteusII_getDevicesP->devices[i].deviceNameLength);
                    len += (9 + ProteusII_getDevicesP->devices[i].deviceNameLength);
                }
            }
            break;
        }
        case PROTEUSII_CMD_RESET_CNF:
        case PROTEUSII_CMD_SCANSTART_CNF:
        case PROTEUSII_CMD_SCANSTOP_CNF:
        case PROTEUSII_CMD_GET_CNF:
        case PROTEUSII_CMD_SET_CNF:
        case PROTEUSII_CMD_SETBEACON_CNF:
        case PROTEUSII_CMD_PASSKEY_CNF:
        case PROTEUSII_CMD_PHYUPDATE_CNF:
        case PROTEUSII_CMD_CONNECT_CNF:
        case PROTEUSII_CMD_DATA_CNF:
        case PROTEUSII_CMD_DISCONNECT_CNF:
        case PROTEUSII_CMD_FACTORYRESET_CNF:
        case PROTEUSII_CMD_SLEEP_CNF:
        case PROTEUSII_CMD_UART_DISABLE_CNF:
        case PROTEUSII_CMD_UART_ENABLE_IND:
        case PROTEUSII_CMD_GET_BONDS_CNF:
        case PROTEUSII_CMD_DELETE_BONDS_CNF:
        case PROTEUSII_CMD_TXCOMPLETE_RSP:
        case PROTEUSII_CMD_NUMERIC_COMP_CNF:
        case PROTEUSII_CMD_DTMSTART_CNF:
        case PROTEUSII_CMD_DTM_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case PROTEUSII_CMD_GETSTATE_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            /* GETSTATE_CNF has no status field */
            cmdConfirmation.status = CMD_Status_NoStatus;

            switch (rxPacket.Data[1])
            {
                case ProteusII_BLE_Action_Idle:
                {
                    bleState = ProteusII_DriverState_BLE_Idle;
                }
                break;
                case ProteusII_BLE_Action_Connected:
                case ProteusII_BLE_Action_None:
                case ProteusII_BLE_Action_Scanning:
                case ProteusII_BLE_Action_Sleep:
                case ProteusII_BLE_Action_DTM:
                default:
                {
                    /* do not use this information */
                }
                break;
            }

            break;
        }

        case PROTEUSII_CMD_CHANNELOPEN_RSP:
        {
            /* Payload of CHANNELOPEN_RSP: Status (1 byte), BTMAC (6 byte), Max Payload (1byte)*/
            bleState = ProteusII_DriverState_BLE_ChannelOpen;
            if (callbacks.channelOpenCb != NULL)
            {
                callbacks.channelOpenCb(&rxPacket.Data[1], (uint16_t)rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSII_CMD_CONNECT_IND:
        {
            bool success = (rxPacket.Data[0] == CMD_Status_Success);
            if (success)
            {
                bleState = ProteusII_DriverState_BLE_Connected;
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

        case PROTEUSII_CMD_DISCONNECT_IND:
        {
            bleState = ProteusII_DriverState_BLE_Idle;
            if (callbacks.disconnectCb != NULL)
            {
                ProteusII_DisconnectReason_t reason = ProteusII_DisconnectReason_Unknown;
                switch (rxPacket.Data[0])
                {
                    case 0x08:
                        reason = ProteusII_DisconnectReason_ConnectionTimeout;
                        break;

                    case 0x13:
                        reason = ProteusII_DisconnectReason_UserTerminatedConnection;
                        break;

                    case 0x16:
                        reason = ProteusII_DisconnectReason_HostTerminatedConnection;
                        break;

                    case 0x3B:
                        reason = ProteusII_DisconnectReason_ConnectionIntervalUnacceptable;
                        break;

                    case 0x3D:
                        reason = ProteusII_DisconnectReason_MicFailure;
                        break;

                    case 0x3E:
                        reason = ProteusII_DisconnectReason_ConnectionSetupFailed;
                        break;
                }
                callbacks.disconnectCb(reason);
            }
            break;
        }

        case PROTEUSII_CMD_DATA_IND:
        {
            if (callbacks.rxCb != NULL)
            {
                callbacks.rxCb(&rxPacket.Data[7], rxPacket.Length - 7, &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSII_CMD_BEACON_IND:
        case PROTEUSII_CMD_BEACON_RSP:
        {
            if (callbacks.beaconRxCb != NULL)
            {
                callbacks.beaconRxCb(&rxPacket.Data[7], rxPacket.Length - 7, &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSII_CMD_RSSI_IND:
        {
            if (callbacks.rssiCb != NULL)
            {
                if (rxPacket.Length >= 8)
                {
                    callbacks.rssiCb(&rxPacket.Data[0], rxPacket.Data[6], rxPacket.Data[7]);
                }
            }
            break;
        }

        case PROTEUSII_CMD_SECURITY_IND:
        {
            if (callbacks.securityCb != NULL)
            {
                callbacks.securityCb(&rxPacket.Data[1], rxPacket.Data[0]);
            }
            break;
        }

        case PROTEUSII_CMD_PASSKEY_IND:
        {
            if (callbacks.passkeyCb != NULL)
            {
                callbacks.passkeyCb(&rxPacket.Data[1]);
            }
            break;
        }

        case PROTEUSII_CMD_DISPLAY_PASSKEY_IND:
        {
            if (callbacks.displayPasskeyCb != NULL)
            {
                callbacks.displayPasskeyCb((ProteusII_DisplayPasskeyAction_t)rxPacket.Data[0], &rxPacket.Data[1], &rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSII_CMD_PHYUPDATE_IND:
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
                callbacks.phyUpdateCb((rxPacket.Data[0] == CMD_Status_Success), btMac, (ProteusII_Phy_t)rxPacket.Data[1], (ProteusII_Phy_t)rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSII_CMD_SLEEP_IND:
        {
            if (callbacks.sleepCb != NULL)
            {
                callbacks.sleepCb();
            }
            break;
        }

        case PROTEUSII_CMD_ERROR_IND:
        {
            if (callbacks.errorCb != NULL)
            {
                callbacks.errorCb(rxPacket.Data[0]);
            }
            break;
        }

        default:
        {
            /* invalid*/
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

static void ProteusII_HandleRxByte(uint8_t* dataP, size_t size)
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
                /* length field lsb */
                rxByteCounter++;
                bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
                break;

            case 3:
                /* length field msb */
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
 * @brief Function that waits for the return value of ProteusII (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(uint32_t maxTimeMs, uint8_t expectedCmdConfirmation, ProteusII_CMD_Status_t expectedStatus, bool resetConfirmState)
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
static bool FillChecksum(ProteusII_CMD_Frame_t* cmd)
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
bool ProteusII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return ProteusII_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Initialize the ProteusII for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ProteusII.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP :         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] opMode:           operation mode
 * @param[in] callbackConfig:   Callback configuration
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ProteusII_Init(WE_UART_t* uartP, ProteusII_Pins_t* pinoutP, ProteusII_OperationMode_t opMode, ProteusII_CallbackConfig_t callbackConfig)
{
    operationMode = opMode;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ProteusII_pinsP = pinoutP;
    ProteusII_pinsP->ProteusII_Pin_Reset.type = WE_Pin_Type_Output;
    ProteusII_pinsP->ProteusII_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ProteusII_pinsP->ProteusII_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    ProteusII_pinsP->ProteusII_Pin_SleepWakeUp.initial_value.output = WE_Pin_Level_High;
    ProteusII_pinsP->ProteusII_Pin_Boot.type = WE_Pin_Type_Output;
    ProteusII_pinsP->ProteusII_Pin_Boot.initial_value.output = WE_Pin_Level_High;
    ProteusII_pinsP->ProteusII_Pin_Mode.type = WE_Pin_Type_Output;
    ProteusII_pinsP->ProteusII_Pin_Mode.initial_value.output = ((operationMode == ProteusII_OperationMode_PeripheralOnlyMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);
    ProteusII_pinsP->ProteusII_Pin_Busy.type = WE_Pin_Type_Input;
    ProteusII_pinsP->ProteusII_Pin_Busy.initial_value.input_pull = WE_Pin_PullType_No;
    ProteusII_pinsP->ProteusII_Pin_StatusLed2.type = WE_Pin_Type_Input;
    ProteusII_pinsP->ProteusII_Pin_StatusLed2.initial_value.input_pull = WE_Pin_PullType_No;

    WE_Pin_t pins[sizeof(ProteusII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Busy, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_StatusLed2, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    /* set callback functions */
    callbacks = callbackConfig;
    byteRxCallback = ProteusII_HandleRxByte;

    ProteusII_uartP = uartP;
    if (false == ProteusII_uartP->uartInit(ProteusII_uartP->baudrate, ProteusII_uartP->flowControl, ProteusII_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (!ProteusII_PinReset())
    {
        WE_DEBUG_PRINT_INFO("Pin reset failed\r\n");
        ProteusII_Deinit();
        return false;
    }

    bleState = ProteusII_DriverState_BLE_Idle;
    ProteusII_getDevicesP = NULL;

    return true;
}

/**
 * @brief Deinitialize the ProteusII interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ProteusII_Deinit()
{
    WE_Pin_t pins[sizeof(ProteusII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_Busy, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusII_pinsP->ProteusII_Pin_StatusLed2, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    /* reset callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    return ProteusII_uartP->uartDeinit();
}

/**
 * @brief Wake up the ProteusII from sleep by pin.
 *
 * Please note that the WAKE_UP pin is also used for re-enabling the UART using
 * the function ProteusII_PinUartEnable(). In that case, the module answers
 * with a different response. The two functions are therefore not interchangeable.
 *
 * @return true if wake-up succeeded,
 *         false otherwise
 */
bool ProteusII_PinWakeup()
{
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

/**
 * @brief Re-enables the module's UART using the WAKE_UP pin after having disabled the
 * UART using ProteusII_UartDisable().
 *
 * Please note that the WAKE_UP pin is also used for waking up the module from sleep
 * mode using the function ProteusII_PinWakeup(). In that case, the module answers
 * with a different response. The two functions are therefore not interchangeable.
 *
 * @return true if enabling UART succeeded,
 *         false otherwise
 */
bool ProteusII_PinUartEnable()
{
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(15);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for UART enable indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_UART_ENABLE_IND, CMD_Status_Success, false);
}

/**
 * @brief Reset the ProteusII by pin.
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusII_PinReset()
{
    /* set to output mode */
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();
    if (!WE_SetPin(ProteusII_pinsP->ProteusII_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ProteusII_OperationMode_PeripheralOnlyMode)
    {
        WE_Delay(PROTEUSII_BOOT_DURATION);
        /* peripheral only mode is ready (the module doesn't send a "ready for operation" message in peripheral only mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

/**
 * @brief Reset the ProteusII by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusII_Reset()
{
    txPacket.Cmd = PROTEUSII_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

/**
 * @brief Disconnect the ProteusII connection if open.
 *
 * @return true if disconnect succeeded,
 *         false otherwise
 */
bool ProteusII_Disconnect()
{
    txPacket.Cmd = PROTEUSII_CMD_DISCONNECT_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* Confirmation is sent before performing the disconnect. After disconnect, the module sends a disconnect indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_DISCONNECT_CNF, CMD_Status_Success, true);
}

/**
 * @brief Put the ProteusII into sleep mode.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusII_Sleep()
{
    txPacket.Cmd = PROTEUSII_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_SLEEP_CNF, CMD_Status_Success, true);
}

/**
 * @brief Disables the UART of the Proteus-II.
 *
 * It will be re-enabled when the module has to send data to the host (e.g. data was received
 * via radio or a state is indicated) or it can be manually re-enabled using ProteusII_PinWakeup().
 *
 * @return true if disable succeeded,
 *         false otherwise
 */
bool ProteusII_UartDisable()
{
    txPacket.Cmd = PROTEUSII_CMD_UART_DISABLE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_UART_DISABLE_CNF, CMD_Status_Success, true);
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
bool ProteusII_Transmit(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length == 0) || (length > PROTEUSII_MAX_RADIO_PAYLOAD_LENGTH) || (ProteusII_DriverState_BLE_ChannelOpen != ProteusII_GetDriverState()))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

/**
 * @brief Places user data in the scan response packet.
 *
 * @param[in] beaconDataP: pointer to the data to put in scan response packet
 * @param[in] length:      length of the data to put in scan response packet
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusII_SetBeacon(uint8_t* beaconDataP, uint16_t length)
{
    if ((beaconDataP == NULL) || (length > PROTEUSII_MAX_BEACON_LENGTH) || (ProteusII_DriverState_BLE_Idle != ProteusII_GetDriverState()))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_SETBEACON_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], beaconDataP, length);

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_SETBEACON_CNF, CMD_Status_Success, true);
}

/*
 * @brief Factory reset of the module.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusII_FactoryReset()
{
    txPacket.Cmd = PROTEUSII_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for reset after factory reset */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
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
bool ProteusII_CheckNSet(ProteusII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    uint8_t current_value[length];
    uint16_t current_length = length;

    if (true == ProteusII_Get(userSetting, current_value, &current_length))
    {
        if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
        {
            /* value is already set, no need to set it again */
            return true;
        }
        else
        {
            /* value differs, and thus must be set */
            return ProteusII_Set(userSetting, valueP, length);
        }
    }
    else
    {
        /* failed reading current value */
        return false;
    }
}

/**
 * @brief Set a special user setting.
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
bool ProteusII_Set(ProteusII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_SET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

/**
 * @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] deviceNameP: pointer to the device name
 * @param[in] nameLength:  length of the device name
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength) { return ProteusII_Set(ProteusII_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLength); }

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
bool ProteusII_SetAdvertisingTimeout(uint16_t advTimeout)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&advTimeout, 2);
    return ProteusII_Set(ProteusII_USERSETTING_RF_ADVERTISING_TIMEOUT, help, 2);
}

/**
 * @brief Set the advertising flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] advFlags: Advertising flags
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_SetAdvertisingFlags(ProteusII_AdvertisingFlags_t advFlags)
{
    uint8_t flags = (uint8_t)advFlags;
    return ProteusII_Set(ProteusII_USERSETTING_RF_ADVERTISING_FLAGS, &flags, 1);
}

/**
 * @brief Set the scan flags (see ProteusII_ScanFlags_t).
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanFlags: Scan flags (see ProteusII_ScanFlags_t)
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_SetScanFlags(uint8_t scanFlags) { return ProteusII_Set(ProteusII_USERSETTING_RF_SCAN_FLAGS, &scanFlags, 1); }

/**
 * @brief Set the beacon flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] beaconFlags: Beacon flags
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_SetBeaconFlags(ProteusII_BeaconFlags_t beaconFlags)
{
    uint8_t flags = (uint8_t)beaconFlags;
    return ProteusII_Set(ProteusII_USERSETTING_RF_BEACON_FLAGS, &flags, 1);
}

/**
 * @brief Set the CFG flags (see ProteusII_CfgFlags_t)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] cfgFlags: CFG flags (see ProteusII_CfgFlags_t)
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_SetCFGFlags(uint16_t cfgFlags)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&cfgFlags, 2);
    return ProteusII_Set(ProteusII_USERSETTING_RF_CFGFLAGS, help, 2);
}

/**
 * @brief Set the BLE connection timing.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] connectionTiming: connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetConnectionTiming(ProteusII_ConnectionTiming_t connectionTiming) { return ProteusII_Set(ProteusII_USERSETTING_RF_CONNECTION_TIMING, (uint8_t*)&connectionTiming, 1); }

/**
 * @brief Set the BLE scan timing
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanTiming: scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetScanTiming(ProteusII_ScanTiming_t scanTiming) { return ProteusII_Set(ProteusII_USERSETTING_RF_SCAN_TIMING, (uint8_t*)&scanTiming, 1); }

/**
 * @brief Set the BLE scan factor
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanFactor: scan factor
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetScanFactor(uint8_t scanFactor)
{
    if ((scanFactor > 10) || (scanFactor < 1))
    {
        return false;
    }
    return ProteusII_Set(ProteusII_USERSETTING_RF_SCAN_FACTOR, &scanFactor, 1);
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
bool ProteusII_SetTXPower(ProteusII_TXPower_t txPower) { return ProteusII_Set(ProteusII_USERSETTING_RF_TX_POWER, (uint8_t*)&txPower, 1); }

/**
 * @brief Set the BLE security flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetSecFlags(ProteusII_SecFlags_t secFlags) { return ProteusII_Set(ProteusII_USERSETTING_RF_SEC_FLAGS, (uint8_t*)&secFlags, 1); }

/**
 * @brief Set the BLE security flags for peripheral only mode.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetSecFlagsPeripheralOnly(ProteusII_SecFlags_t secFlags) { return ProteusII_Set(ProteusII_USERSETTING_RF_SECFLAGSPERONLY, (uint8_t*)&secFlags, 1); }

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_SetBaudrateIndex(ProteusII_BaudRate_t baudrate)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    return ProteusII_Set(ProteusII_USERSETTING_UART_BAUDRATEINDEX, (uint8_t*)&baudrateIndex, 1);
}

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
bool ProteusII_SetStaticPasskey(uint8_t* staticPasskeyP) { return ProteusII_Set(ProteusII_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, 6); }

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
bool ProteusII_SetAppearance(uint16_t appearance)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&appearance, 2);
    return ProteusII_Set(ProteusII_USERSETTING_RF_APPEARANCE, help, 2);
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
bool ProteusII_SetSppBaseUuid(uint8_t* uuidP) { return ProteusII_Set(ProteusII_USERSETTING_RF_SPPBASEUUID, uuidP, 16); }

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
bool ProteusII_Get(ProteusII_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    if ((responseP == NULL) || (responseLengthP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GET_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;
        memcpy(responseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
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
bool ProteusII_GetFWVersion(uint8_t* versionP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_FS_FWVersion, versionP, &length);
}

/**
 * @brief Request device info.
 *
 * @param[out] deviceInfoP: pointer to the device info structure
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_GetDeviceInfo(ProteusII_DeviceInfo_t* deviceInfoP)
{
    if (deviceInfoP == NULL)
    {
        return false;
    }

    uint8_t help[12];
    uint16_t length;
    if (!ProteusII_Get(ProteusII_USERSETTING_FS_DEVICE_INFO, help, &length))
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
bool ProteusII_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length;
    if (!ProteusII_Get(ProteusII_USERSETTING_FS_SERIAL_NUMBER, serialNumberP, &length))
    {
        return false;
    }
    return true;
}

/**
 * @brief Request the current BLE device name.
 *
 * @param[out] deviceNameP: pointer to device name
 * @param[out] nameLengthP: pointer to the length of the device name
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP) { return ProteusII_Get(ProteusII_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLengthP); }

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: pointer to the MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetMAC(uint8_t* macP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_FS_MAC, macP, &length);
}

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: pointer to the Bluetooth MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetBTMAC(uint8_t* btMacP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_FS_BTMAC, btMacP, &length);
}

/**
 * @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: pointer to the advertising timeout
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetAdvertisingTimeout(uint16_t* advTimeoutP)
{
    if (advTimeoutP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusII_Get(ProteusII_USERSETTING_RF_ADVERTISING_TIMEOUT, help, &length))
    {
        memcpy((uint8_t*)advTimeoutP, help, 2);
        return true;
    }

    return false;
}

/**
 * @brief Request the advertising flags.
 *
 * @param[out] advFlagsP: pointer to the advertising flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetAdvertisingFlags(ProteusII_AdvertisingFlags_t* advFlagsP)
{
    if (advFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t flags;
    if (ProteusII_Get(ProteusII_USERSETTING_RF_ADVERTISING_FLAGS, &flags, &length))
    {
        *advFlagsP = flags;
        return true;
    }
    return false;
}

/**
 * @brief Request the scan flags (see ProteusII_ScanFlags_t).
 *
 * @param[out] scanFlagsP: pointer to the scan flags (see ProteusII_ScanFlags_t)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetScanFlags(uint8_t* scanFlagsP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SCAN_FLAGS, scanFlagsP, &length);
}

/**
 * @brief Request the beacon flags.
 *
 * @param[out] beaconFlagsP: pointer to the beacon flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetBeaconFlags(ProteusII_BeaconFlags_t* beaconFlagsP)
{
    if (beaconFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t flags;
    if (ProteusII_Get(ProteusII_USERSETTING_RF_BEACON_FLAGS, &flags, &length))
    {
        *beaconFlagsP = flags;
        return true;
    }
    return false;
}

/**
 * @brief Request the connection timing
 *
 * @param[out] connectionTimingP: pointer to the connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetConnectionTiming(ProteusII_ConnectionTiming_t* connectionTimingP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_CONNECTION_TIMING, (uint8_t*)connectionTimingP, &length);
}

/**
 * @brief Request the scan timing
 *
 * @param[out] scanTimingP: pointer to the scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetScanTiming(ProteusII_ScanTiming_t* scanTimingP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SCAN_TIMING, (uint8_t*)scanTimingP, &length);
}

/**
 * @brief Request the scan factor
 *
 * @param[out] scanFactorP: pointer to the scan factor
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetScanFactor(uint8_t* scanFactorP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SCAN_FACTOR, scanFactorP, &length);
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetTXPower(ProteusII_TXPower_t* txPowerP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_TX_POWER, (uint8_t*)txPowerP, &length);
}

/**
 * @brief Request the security flags
 *
 * @param[out] secFlagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetSecFlags(ProteusII_SecFlags_t* secFlagsP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SEC_FLAGS, (uint8_t*)secFlagsP, &length);
}

/**
 * @brief Request the security flags for peripheral only mode
 *
 * @param[out] secFlagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetSecFlagsPeripheralOnly(ProteusII_SecFlags_t* secFlagsP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SECFLAGSPERONLY, (uint8_t*)secFlagsP, &length);
}

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateP: pointer to the UART baudrate index
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusII_GetBaudrateIndex(ProteusII_BaudRate_t* baudrateP)
{
    if (baudrateP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t uartIndex;
    if (ProteusII_Get(ProteusII_USERSETTING_UART_BAUDRATEINDEX, (uint8_t*)&uartIndex, &length))
    {
        *baudrateP = (ProteusII_BaudRate_t)uartIndex;
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
bool ProteusII_GetStaticPasskey(uint8_t* staticPasskeyP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, &length);
}

/**
 * @brief Request the Bluetooth appearance of the device
 *
 * @param[out] appearanceP: pointer to the Bluetooth appearance
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetAppearance(uint16_t* appearanceP)
{
    if (appearanceP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];
    if (ProteusII_Get(ProteusII_USERSETTING_RF_APPEARANCE, help, &length))
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
bool ProteusII_GetSppBaseUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusII_Get(ProteusII_USERSETTING_RF_SPPBASEUUID, uuidP, &length);
}

/**
 * @brief Request the CFG flags (see ProteusII_CfgFlags_t)
 *
 * @param[out] cfgFlags: pointer to the CFG flags (see ProteusII_CfgFlags_t)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetCFGFlags(uint16_t* cfgFlagsP)
{
    if (cfgFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];
    if (ProteusII_Get(ProteusII_USERSETTING_RF_CFGFLAGS, help, &length))
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
bool ProteusII_GetState(ProteusII_ModuleState_t* moduleStateP)
{
    if (moduleStateP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        uint16_t length = rxPacket.Length;
        moduleStateP->role = rxPacket.Data[0];
        moduleStateP->action = rxPacket.Data[1];

        if (moduleStateP->action == ProteusII_BLE_Action_Connected && length >= 8)
        {
            memcpy(moduleStateP->connectedDeviceBtMac, &rxPacket.Data[2], 6);
        }
        else
        {
            memset(moduleStateP->connectedDeviceBtMac, 0, 6);
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
ProteusII_DriverState_t ProteusII_GetDriverState() { return bleState; }

/**
 * @brief Start scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_ScanStart()
{
    txPacket.Cmd = PROTEUSII_CMD_SCANSTART_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_SCANSTART_CNF, CMD_Status_Success, true);
}

/**
 * @brief Stop a scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_ScanStop()
{
    txPacket.Cmd = PROTEUSII_CMD_SCANSTOP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_SCANSTOP_CNF, CMD_Status_Success, true);
}

/**
 * @brief Request the scan results
 *
 * @param[out] devicesP: pointer to scan result struct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetDevices(ProteusII_GetDevices_t* devicesP)
{
    ProteusII_getDevicesP = devicesP;
    if (ProteusII_getDevicesP != NULL)
    {
        ProteusII_getDevicesP->numberOfDevices = 0;
    }

    txPacket.Cmd = PROTEUSII_CMD_GETDEVICES_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        ProteusII_getDevicesP = NULL;
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETDEVICES_CNF, CMD_Status_Success, true);
    ProteusII_getDevicesP = NULL;
    return ret;
}

/**
 * @brief Connect to the BLE device with the corresponding BTMAC
 *
 * @param[in] btMacP: pointer to btmac
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_Connect(uint8_t* btMacP)
{
    if (btMacP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_CONNECT_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], btMacP, 6);

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(3000, PROTEUSII_CMD_CONNECT_CNF, CMD_Status_Success, true);
}

/**
 * @brief Answer on a passkey request with a passkey to setup a connection
 *
 * @param[in] passkey: pointer to passkey
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_Passkey(uint8_t* passkey)
{
    if (passkey == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_PASSKEY_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], passkey, 6);

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_PASSKEY_CNF, CMD_Status_Success, true);
}

/**
 * @brief Answer on a numeric comparison request
 *
 * @param[in] keyIsOk: boolean to confirm if the key shown is correct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_NumericCompareConfirm(bool keyIsOk)
{
    txPacket.Cmd = PROTEUSII_CMD_NUMERIC_COMP_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = keyIsOk ? 0x00 : 0x01;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_NUMERIC_COMP_CNF, CMD_Status_Success, true);
}

/**
 * @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_PhyUpdate(ProteusII_Phy_t phy)
{
    if (ProteusII_DriverState_BLE_ChannelOpen == ProteusII_GetDriverState())
    {
        txPacket.Cmd = PROTEUSII_CMD_PHYUPDATE_REQ;
        txPacket.Length = 1;
        txPacket.Data[0] = (uint8_t)phy;

        FillChecksum(&txPacket);
        if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
        {
            return false;
        }

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_PHYUPDATE_CNF, CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Returns the current level of the status pin.
 *
 * @param[out] statusLed2PinLevelP: status pin level
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetStatusLed2PinLevel(WE_Pin_Level_t* statusLed2LevelP) { return WE_GetPinLevel(ProteusII_pinsP->ProteusII_Pin_StatusLed2, statusLed2LevelP); }

/**
 * @brief Returns the current level of the BUSY pin.
 *
 * @param[out] busyStateP: busy pin state
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_IsPeripheralOnlyModeBusy(bool* busyStateP)
{
    if (busyStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(ProteusII_pinsP->ProteusII_Pin_Busy, &pin_level))
    {
        return false;
    }

    *busyStateP = (pin_level == WE_Pin_Level_High);

    return true;
}

/**
 * @brief Sets the callback function which is executed if a byte has been received from Proteus-II.
 *
 * The default callback is ProteusII_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default callback is used if NULL)
 */
void ProteusII_SetByteRxCallback(WE_UART_HandleRxByte_t callback) { byteRxCallback = (callback == NULL) ? ProteusII_HandleRxByte : callback; }

/**
 * @brief Requests the BTMAC addresses of all bonded devices.
 *
 * Note that this function supports a maximum of PROTEUSII_MAX_BOND_DEVICES
 * returned devices. The Proteus-II module itself might be capable of
 * bonding more devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_GetBonds(ProteusII_BondDatabase_t* bondDatabaseP)
{
    if (bondDatabaseP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSII_CMD_GET_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GET_BONDS_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    bondDatabaseP->nrOfDevices = rxPacket.Data[1];
    if (bondDatabaseP->nrOfDevices > PROTEUSII_MAX_BOND_DEVICES)
    {
        bondDatabaseP->nrOfDevices = PROTEUSII_MAX_BOND_DEVICES;
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
bool ProteusII_DeleteBonds()
{
    txPacket.Cmd = PROTEUSII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

/**
 * @brief Removes the bonding information for a single device.
 *
 * @param[in] bondId: bond ID of the device to be removed
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DeleteBond(uint8_t bondId)
{
    txPacket.Cmd = PROTEUSII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = bondId;
    txPacket.Data[1] = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

/**
 * @brief Enable the direct test mode (DTM)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DTMEnable()
{
    txPacket.Cmd = PROTEUSII_CMD_DTMSTART_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        if ((rxPacket.Data[0] == (uint8_t)ProteusII_BLE_Role_DTM) && (rxPacket.Data[1] == (uint8_t)ProteusII_BLE_Action_DTM))
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
bool ProteusII_DTMRun(ProteusII_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload)
{
    txPacket.Cmd = PROTEUSII_CMD_DTM_REQ;
    txPacket.Length = 4;
    txPacket.Data[0] = command;
    txPacket.Data[1] = channel_vendoroption;
    txPacket.Data[2] = length_vendorcmd;
    txPacket.Data[3] = payload;

    FillChecksum(&txPacket);
    if (!ProteusII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSII_CMD_DTM_CNF, CMD_Status_Success, true);
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
bool ProteusII_DTMStartTX(uint8_t channel, uint8_t length, ProteusII_DTMTXPattern_t pattern) { return ProteusII_DTMRun(ProteusII_DTMCommand_StartTX, channel, length, (uint8_t)pattern); }

/**
 * @brief Start the direct test mode (DTM) TX carrier test
 *
 * @param[in] channel: Channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DTMStartTXCarrier(uint8_t channel) { return ProteusII_DTMRun(ProteusII_DTMCommand_StartTX, channel, 0x00, 0x03); }

/**
 * @brief Stop the current direct test mode (DTM) test
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DTMStop() { return ProteusII_DTMRun(ProteusII_DTMCommand_Stop, 0x00, 0x00, 0x01); }

/**
 * @brief Set the phy for direct test mode (DTM) test
 *
 * @param[in] phy: Phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DTMSetPhy(ProteusII_Phy_t phy) { return ProteusII_DTMRun(ProteusII_DTMCommand_Setup, 0x02, (uint8_t)phy, 0x00); }

/**
 * @brief Set the TX power for direct test mode (DTM) test
 *
 * @param[in] power: Tx power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusII_DTMSetTXPower(ProteusII_TXPower_t power) { return ProteusII_DTMRun(ProteusII_DTMCommand_StartTX, (uint8_t)power, 0x02, 0x03); }
