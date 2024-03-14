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
 * @brief Proteus-III driver source file.
 */

#include "ProteusIII.h"

#include "stdio.h"
#include "string.h"

#include "../global/global.h"

#define CMD_WAIT_TIME 500
#define CNFINVALID 255

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH \
    (uint16_t)(PROTEUSIII_MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

typedef struct {
    uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[PROTEUSIII_MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ProteusIII_CMD_Frame_t;

#define CMD_STX 0x02

#define PROTEUSIII_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define PROTEUSIII_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define PROTEUSIII_CMD_TYPE_IND (uint8_t)(2 << 6)
#define PROTEUSIII_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define PROTEUSIII_CMD_RESET (uint8_t)0x00
#define PROTEUSIII_CMD_RESET_REQ \
    (PROTEUSIII_CMD_RESET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_RESET_CNF \
    (PROTEUSIII_CMD_RESET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GETSTATE (uint8_t)0x01
#define PROTEUSIII_CMD_GETSTATE_REQ \
    (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GETSTATE_CNF \
    (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GETSTATE_IND \
    (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SLEEP (uint8_t)0x02
#define PROTEUSIII_CMD_SLEEP_REQ \
    (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SLEEP_CNF \
    (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SLEEP_IND \
    (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_UART_DISABLE (uint8_t)0x1B
#define PROTEUSIII_CMD_UART_DISABLE_REQ \
    (PROTEUSIII_CMD_UART_DISABLE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_UART_DISABLE_CNF \
    (PROTEUSIII_CMD_UART_DISABLE | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_UART_ENABLE_IND (uint8_t)0x9B

#define PROTEUSIII_CMD_DATA (uint8_t)0x04
#define PROTEUSIII_CMD_DATA_REQ (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DATA_CNF (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DATA_IND (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_TXCOMPLETE_RSP \
    (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_RSP)

#define PROTEUSIII_CMD_CONNECT (uint8_t)0x06
#define PROTEUSIII_CMD_CONNECT_REQ \
    (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_CONNECT_CNF \
    (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_CONNECT_IND \
    (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_CHANNELOPEN_RSP \
    (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_RSP)

#define PROTEUSIII_CMD_DISCONNECT (uint8_t)0x07
#define PROTEUSIII_CMD_DISCONNECT_REQ \
    (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DISCONNECT_CNF \
    (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DISCONNECT_IND \
    (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SECURITY_IND (uint8_t)0x88

#define PROTEUSIII_CMD_SCANSTART (uint8_t)0x09
#define PROTEUSIII_CMD_SCANSTART_REQ \
    (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SCANSTART_CNF \
    (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SCANSTART_IND \
    (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SCANSTOP (uint8_t)0x0A
#define PROTEUSIII_CMD_SCANSTOP_REQ \
    (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SCANSTOP_CNF \
    (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SCANSTOP_IND \
    (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GETDEVICES (uint8_t)0x0B
#define PROTEUSIII_CMD_GETDEVICES_REQ \
    (PROTEUSIII_CMD_GETDEVICES | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GETDEVICES_CNF \
    (PROTEUSIII_CMD_GETDEVICES | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_SETBEACON (uint8_t)0x0C
#define PROTEUSIII_CMD_SETBEACON_REQ \
    (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SETBEACON_CNF \
    (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_BEACON_IND \
    (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_BEACON_RSP \
    (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_RSP)
#define PROTEUSIII_CMD_RSSI_IND (uint8_t)0x8B

#define PROTEUSIII_CMD_PASSKEY (uint8_t)0x0D
#define PROTEUSIII_CMD_PASSKEY_REQ \
    (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_PASSKEY_CNF \
    (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_PASSKEY_IND \
    (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GET (uint8_t)0x10
#define PROTEUSIII_CMD_GET_REQ (PROTEUSIII_CMD_GET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GET_CNF (PROTEUSIII_CMD_GET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_SET (uint8_t)0x11
#define PROTEUSIII_CMD_SET_REQ (PROTEUSIII_CMD_SET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SET_CNF (PROTEUSIII_CMD_SET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_PHYUPDATE (uint8_t)0x1A
#define PROTEUSIII_CMD_PHYUPDATE_REQ \
    (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_PHYUPDATE_CNF \
    (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_PHYUPDATE_IND \
    (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_FACTORYRESET (uint8_t)0x1C
#define PROTEUSIII_CMD_FACTORYRESET_REQ \
    (PROTEUSIII_CMD_FACTORYRESET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_FACTORYRESET_CNF \
    (PROTEUSIII_CMD_FACTORYRESET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DTMSTART (uint8_t)0x1D
#define PROTEUSIII_CMD_DTMSTART_REQ \
    (PROTEUSIII_CMD_DTMSTART | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DTMSTART_CNF \
    (PROTEUSIII_CMD_DTMSTART | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DTM (uint8_t)0x1E
#define PROTEUSIII_CMD_DTM_REQ (PROTEUSIII_CMD_DTM | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DTM_CNF (PROTEUSIII_CMD_DTM | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_NUMERIC_COMP (uint8_t)0x24
#define PROTEUSIII_CMD_NUMERIC_COMP_REQ \
    (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_NUMERIC_COMP_CNF \
    (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DISPLAY_PASSKEY_IND \
    (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG (uint8_t)0x25
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ \
    (PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF \
    (PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG (uint8_t)0x2B
#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_REQ \
    (PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF \
    (PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE (uint8_t)0x26
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_REQ \
    (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF \
    (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_IND \
    (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_LOCAL_READ (uint8_t)0x27
#define PROTEUSIII_CMD_GPIO_LOCAL_READ_REQ \
    (PROTEUSIII_CMD_GPIO_LOCAL_READ | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF \
    (PROTEUSIII_CMD_GPIO_LOCAL_READ | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG (uint8_t)0x28
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_IND \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG (uint8_t)0x2C
#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_REQ \
    (PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF \
    (PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE (uint8_t)0x29
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_REQ \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND \
    (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_REMOTE_READ (uint8_t)0x2A
#define PROTEUSIII_CMD_GPIO_REMOTE_READ_REQ \
    (PROTEUSIII_CMD_GPIO_REMOTE_READ | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF \
    (PROTEUSIII_CMD_GPIO_REMOTE_READ | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GET_BONDS (uint8_t)0x0F
#define PROTEUSIII_CMD_GET_BONDS_REQ \
    (PROTEUSIII_CMD_GET_BONDS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GET_BONDS_CNF \
    (PROTEUSIII_CMD_GET_BONDS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DELETE_BONDS (uint8_t)0x0E
#define PROTEUSIII_CMD_DELETE_BONDS_REQ \
    (PROTEUSIII_CMD_DELETE_BONDS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DELETE_BONDS_CNF \
    (PROTEUSIII_CMD_DELETE_BONDS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS (uint8_t)0x2D
#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ \
    (PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF \
    (PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_ERROR_IND (uint8_t)0xA2

/**
 * @brief Type used to check the response, when a command was sent to the
 * ProteusIII
 */
typedef enum ProteusIII_CMD_Status_t {
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ProteusIII_CMD_Status_t;

/**
 * @brief Command confirmation.
 */
typedef struct {
    uint8_t cmd; /**< Variable to check if correct CMD has been confirmed */
    ProteusIII_CMD_Status_t
        status; /**< Variable used to check the response (*_CNF), when a request
                   (*_REQ) was sent to the ProteusIII */
} ProteusIII_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ProteusIII_CMD_Frame_t txPacket = {
    .Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ProteusIII_CMD_Frame_t rxPacket = {
    .Stx = CMD_STX,
    .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ProteusIII_CMD_Confirmation_t
    cmdConfirmationArray[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusIII_OperationMode_t operationMode =
    ProteusIII_OperationMode_CommandMode;
static ProteusIII_GetDevices_t *ProteusIII_getDevicesP = NULL;
static ProteusIII_DriverState_t bleState;
static ProteusIII_Pins_t *ProteusIII_pinsP = NULL;

static ProteusIII_CallbackConfig_t callbacks;
static ProteusIII_ByteRxCallback byteRxCallback = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t
    rxBuffer[sizeof(ProteusIII_CMD_Frame_t)]; /* For UART RX from module */

/**************************************
 *         Static functions           *
 **************************************/

static void ClearReceiveBuffers() {
    bytesToReceive = 0;
    rxByteCounter = 0;
    checksum = 0;
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
}

static void HandleRxPacket(uint8_t *prxBuffer) {
    ProteusIII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ProteusIII_CMD_Frame_t *)prxBuffer)->Length;
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
        case PROTEUSIII_CMD_GETDEVICES_CNF: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            if ((cmdConfirmation.status == CMD_Status_Success) &&
                (ProteusIII_getDevicesP != NULL)) {
                uint8_t size = rxPacket.Data[1];
                if (size >= PROTEUSIII_MAX_NUMBER_OF_DEVICES) {
                    size = PROTEUSIII_MAX_NUMBER_OF_DEVICES;
                }
                ProteusIII_getDevicesP->numberOfDevices = size;

                uint16_t len = 2;
                for (uint8_t i = 0; i < ProteusIII_getDevicesP->numberOfDevices;
                     i++) {
                    memcpy(&ProteusIII_getDevicesP->devices[i].btmac[0],
                           &rxPacket.Data[len], 6);
                    ProteusIII_getDevicesP->devices[i].rssi =
                        rxPacket.Data[len + 6];
                    ProteusIII_getDevicesP->devices[i].txPower =
                        rxPacket.Data[len + 7];
                    ProteusIII_getDevicesP->devices[i].deviceNameLength =
                        rxPacket.Data[len + 8];
                    memcpy(&ProteusIII_getDevicesP->devices[i].deviceName[0],
                           &rxPacket.Data[len + 9],
                           ProteusIII_getDevicesP->devices[i].deviceNameLength);
                    len +=
                        (9 +
                         ProteusIII_getDevicesP->devices[i].deviceNameLength);
                }
            }
            break;
        }
        case PROTEUSIII_CMD_RESET_CNF:
        case PROTEUSIII_CMD_SCANSTART_CNF:
        case PROTEUSIII_CMD_SCANSTOP_CNF:
        case PROTEUSIII_CMD_GET_CNF:
        case PROTEUSIII_CMD_SET_CNF:
        case PROTEUSIII_CMD_SETBEACON_CNF:
        case PROTEUSIII_CMD_PASSKEY_CNF:
        case PROTEUSIII_CMD_PHYUPDATE_CNF:
        case PROTEUSIII_CMD_CONNECT_CNF:
        case PROTEUSIII_CMD_DATA_CNF:
        case PROTEUSIII_CMD_DISCONNECT_CNF:
        case PROTEUSIII_CMD_FACTORYRESET_CNF:
        case PROTEUSIII_CMD_SLEEP_CNF:
        case PROTEUSIII_CMD_UART_DISABLE_CNF:
        case PROTEUSIII_CMD_UART_ENABLE_IND:
        case PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF:
        case PROTEUSIII_CMD_GET_BONDS_CNF:
        case PROTEUSIII_CMD_DELETE_BONDS_CNF:
        case PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF:
        case PROTEUSIII_CMD_TXCOMPLETE_RSP:
        case PROTEUSIII_CMD_NUMERIC_COMP_CNF:
        case PROTEUSIII_CMD_DTMSTART_CNF:
        case PROTEUSIII_CMD_DTM_CNF: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case PROTEUSIII_CMD_GETSTATE_CNF: {
            cmdConfirmation.cmd = rxPacket.Cmd;
            /* GETSTATE_CNF has no status field*/
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case PROTEUSIII_CMD_CHANNELOPEN_RSP: {
            /* Payload of CHANNELOPEN_RSP: Status (1 byte), BTMAC (6 byte), Max
             * Payload (1byte)*/
            bleState = ProteusIII_DriverState_BLE_ChannelOpen;
            if (callbacks.channelOpenCb != NULL) {
                callbacks.channelOpenCb(&rxPacket.Data[1],
                                        (uint16_t)rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSIII_CMD_CONNECT_IND: {
            bool success = (rxPacket.Data[0] == CMD_Status_Success);
            if (success) {
                bleState = ProteusIII_DriverState_BLE_Connected;
            }
            if (callbacks.connectCb != NULL) {
                uint8_t btMac[6];
                if (rxPacket.Length >= 7) {
                    memcpy(btMac, &rxPacket.Data[1], 6);
                } else {
                    /* Packet doesn't contain BTMAC (e.g. connection failed) */
                    memset(btMac, 0, 6);
                }
                callbacks.connectCb(success, btMac);
            }
            break;
        }

        case PROTEUSIII_CMD_DISCONNECT_IND: {
            bleState = ProteusIII_DriverState_BLE_Invalid;
            if (callbacks.disconnectCb != NULL) {
                ProteusIII_DisconnectReason_t reason =
                    ProteusIII_DisconnectReason_Unknown;
                switch (rxPacket.Data[0]) {
                    case 0x08:
                        reason = ProteusIII_DisconnectReason_ConnectionTimeout;
                        break;

                    case 0x13:
                        reason =
                            ProteusIII_DisconnectReason_UserTerminatedConnection;
                        break;

                    case 0x16:
                        reason =
                            ProteusIII_DisconnectReason_HostTerminatedConnection;
                        break;

                    case 0x3B:
                        reason =
                            ProteusIII_DisconnectReason_ConnectionIntervalUnacceptable;
                        break;

                    case 0x3D:
                        reason = ProteusIII_DisconnectReason_MicFailure;
                        break;

                    case 0x3E:
                        reason =
                            ProteusIII_DisconnectReason_ConnectionSetupFailed;
                        break;
                }
                callbacks.disconnectCb(reason);
            }
            break;
        }

        case PROTEUSIII_CMD_DATA_IND: {
            if (callbacks.rxCb != NULL) {
                callbacks.rxCb(&rxPacket.Data[7], rxPacket.Length - 7,
                               &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSIII_CMD_BEACON_IND:
        case PROTEUSIII_CMD_BEACON_RSP: {
            if (callbacks.beaconRxCb != NULL) {
                callbacks.beaconRxCb(&rxPacket.Data[7], rxPacket.Length - 7,
                                     &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSIII_CMD_RSSI_IND: {
            if (callbacks.rssiCb != NULL) {
                if (rxPacket.Length >= 8) {
                    callbacks.rssiCb(&rxPacket.Data[0], rxPacket.Data[6],
                                     rxPacket.Data[7]);
                }
            }
            break;
        }

        case PROTEUSIII_CMD_SECURITY_IND: {
            if (callbacks.securityCb != NULL) {
                callbacks.securityCb(&rxPacket.Data[1], rxPacket.Data[0]);
            }
            break;
        }

        case PROTEUSIII_CMD_PASSKEY_IND: {
            if (callbacks.passkeyCb != NULL) {
                callbacks.passkeyCb(&rxPacket.Data[1]);
            }
            break;
        }

        case PROTEUSIII_CMD_DISPLAY_PASSKEY_IND: {
            if (callbacks.displayPasskeyCb != NULL) {
                callbacks.displayPasskeyCb(
                    (ProteusIII_DisplayPasskeyAction_t)rxPacket.Data[0],
                    &rxPacket.Data[1], &rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSIII_CMD_PHYUPDATE_IND: {
            if (callbacks.phyUpdateCb != NULL) {
                bool success = (rxPacket.Data[0] == CMD_Status_Success);
                uint8_t btMac[6];
                if (rxPacket.Length >= 9) {
                    memcpy(btMac, &rxPacket.Data[3], 6);
                } else {
                    /* Packet doesn't contain BTMAC (e.g. Phy update failed) */
                    memset(btMac, 0, 6);
                }
                callbacks.phyUpdateCb(success, btMac,
                                      (ProteusIII_Phy_t)rxPacket.Data[1],
                                      (ProteusIII_Phy_t)rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSIII_CMD_SLEEP_IND: {
            if (callbacks.sleepCb != NULL) {
                callbacks.sleepCb();
            }
            break;
        }

        case PROTEUSIII_CMD_GPIO_LOCAL_WRITE_IND:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND: {
            if (callbacks.gpioWriteCb != NULL) {
                uint8_t pos = 0;
                while (pos < rxPacket.Length) {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;

                    /* Note that the gpioId parameter is of type uint8_t instead
                     * of ProteusIII_GPIO_t, as the remote device may support
                     * other GPIOs than this device. */
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t value = rxPacket.Data[2 + pos];
                    callbacks.gpioWriteCb(
                        PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND == rxPacket.Cmd,
                        gpioId, value);

                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_IND: {
            if (callbacks.gpioRemoteConfigCb != NULL) {
                uint8_t pos = 0;
                while (pos < rxPacket.Length) {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t function = rxPacket.Data[2 + pos];
                    uint8_t *value = &rxPacket.Data[3 + pos];

                    ProteusIII_GPIOConfigBlock_t gpioConfig = {0};
                    gpioConfig.gpioId = (ProteusIII_GPIO_t)gpioId;
                    gpioConfig.function = (ProteusIII_GPIO_IO_t)function;
                    switch (gpioConfig.function) {
                        case ProteusIII_GPIO_IO_Disconnected:
                            break;

                        case ProteusIII_GPIO_IO_Input:
                            gpioConfig.value.input =
                                (ProteusIII_GPIO_Input_t)*value;
                            break;

                        case ProteusIII_GPIO_IO_Output:
                            gpioConfig.value.output =
                                (ProteusIII_GPIO_Output_t)*value;
                            break;

                        case ProteusIII_GPIO_IO_PWM:
                            gpioConfig.value.pwm.period =
                                (((uint16_t)value[1]) << 8) |
                                ((uint16_t)value[0]);
                            gpioConfig.value.pwm.ratio = value[2];
                            break;
                    }
                    callbacks.gpioRemoteConfigCb(&gpioConfig);
                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSIII_CMD_ERROR_IND: {
            if (callbacks.errorCb != NULL) {
                callbacks.errorCb(rxPacket.Data[0]);
            }
            break;
        }

        default: {
            /* invalid*/
            break;
        }
    }

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        if (cmdConfirmationArray[i].cmd == CNFINVALID) {
            cmdConfirmationArray[i].cmd = cmdConfirmation.cmd;
            cmdConfirmationArray[i].status = cmdConfirmation.status;
            break;
        }
    }
}

void ProteusIII_HandleRxByte(uint8_t receivedByte) {
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
            /* length field lsb */
            rxByteCounter++;
            bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
            break;

        case 3:
            /* length field msb */
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
 * @brief Function that waits for the return value of ProteusIII (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(int maxTimeMs, uint8_t expectedCmdConfirmation,
                     ProteusIII_CMD_Status_t expectedStatus,
                     bool resetConfirmState) {
    int count = 0;
    int timeStepMs = 5; /* 5ms */
    int maxCount = maxTimeMs / timeStepMs;
    int i = 0;

    if (resetConfirmState) {
        for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            cmdConfirmationArray[i].cmd = CNFINVALID;
        }
    }
    while (1) {
        for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            if (expectedCmdConfirmation == cmdConfirmationArray[i].cmd) {
                return (cmdConfirmationArray[i].status == expectedStatus);
            }
        }

        if (count >= maxCount) {
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
static bool FillChecksum(ProteusIII_CMD_Frame_t *cmd) {
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
 * @brief Sends raw data to Proteus via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending commands to ProteusIII.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 */
static void ProteusIII_UART_Transmit(uint8_t *data, uint16_t dataLength) {
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
    byteRxCallback(receivedByte);
}

/**
 * @brief Initialize the ProteusIII for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the
 * ProteusIII. The baudrate parameter must match to perform a successful FTDI
 * communication. Updating this parameter during runtime may lead to
 * communication errors.
 *
 * @param[in] pinout:           definition of the gpios connected to the module
 * @param[in] baudrate:         baudrate of the interface
 * @param[in] flowControl:      enable/disable flowcontrol
 * @param[in] opMode:           operation mode
 * @param[in] callbackConfig:   Callback configuration
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ProteusIII_Init(ProteusIII_Pins_t *pinoutP, uint32_t baudrate,
                     WE_FlowControl_t flowControl,
                     ProteusIII_OperationMode_t opMode,
                     ProteusIII_CallbackConfig_t callbackConfig) {
    operationMode = opMode;

    /* initialize the pins */
    if (pinoutP == NULL) {
        return false;
    }

    ProteusIII_pinsP = pinoutP;
    ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    ProteusIII_pinsP->ProteusIII_Pin_Mode.type = WE_Pin_Type_Output;

    WE_Pin_t pins[sizeof(ProteusIII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp,
           sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Mode,
           sizeof(WE_Pin_t));

    if (false == WE_InitPins(pins, pin_count)) {
        /* error */
        return false;
    }
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_High);
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_Mode,
              (operationMode == ProteusIII_OperationMode_PeripheralOnlyMode)
                  ? WE_Pin_Level_High
                  : WE_Pin_Level_Low);

    /* set callback functions */
    callbacks = callbackConfig;
    byteRxCallback = ProteusIII_HandleRxByte;

    WE_UART_RXPin11_TXPin10_Init(baudrate, flowControl, WE_Parity_None);
    WE_Delay(10);

    uint8_t fwVersion;
    ProteusIII_GetFWVersion(&fwVersion);

    /* reset module */
    if (ProteusIII_Reset()) {
        WE_Delay(PROTEUSIII_BOOT_DURATION);
    } else {
        WE_DEBUG_PRINT("reset failed\n");
        ProteusIII_Deinit();
        return false;
    }

    bleState = ProteusIII_DriverState_BLE_Invalid;
    ProteusIII_getDevicesP = NULL;

    WE_Delay(100);

    return true;
}

/**
 * @brief Deinitialize the ProteusIII interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ProteusIII_Deinit() {
    /* close the communication interface to the module */
    WE_UART_RXPin11_TXPin10_DeInit();

    /* deinit pins */
    WE_DeinitPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp);
    WE_DeinitPin(ProteusIII_pinsP->ProteusIII_Pin_Mode);

    /* reset callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    return true;
}

/**
 * @brief Wake up the ProteusIII from sleep by pin.
 *
 * Please note that the WAKE_UP pin is also used for re-enabling the UART using
 * the function ProteusIII_PinUartEnable(). In that case, the module answers
 * with a different response. The two functions are therefore not
 * interchangeable.
 *
 * @return true if wake-up succeeded,
 *         false otherwise
 */
bool ProteusIII_PinWakeup() {
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_Low);
    WE_Delay(5);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                    CMD_Status_NoStatus, false);
}

/**
 * @brief Re-enables the module's UART using the WAKE_UP pin after having
 * disabled the UART using ProteusIII_UartDisable().
 *
 * Please note that the WAKE_UP pin is also used for waking up the module from
 * sleep mode using the function ProteusIII_PinWakeup(). In that case, the
 * module answers with a different response. The two functions are therefore not
 * interchangeable.
 *
 * @return true if enabling UART succeeded,
 *         false otherwise
 */
bool ProteusIII_PinUartEnable() {
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_Low);
    WE_Delay(15);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_High);

    /* wait for UART enable indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_UART_ENABLE_IND,
                    CMD_Status_Success, false);
}

/**
 * @brief Reset the ProteusIII by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusIII_Reset() {
    txPacket.Cmd = PROTEUSIII_CMD_RESET_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                        CMD_Status_NoStatus, true);
    }
    return false;
}

/**
 * @brief Disconnect the ProteusIII connection if open.
 *
 * @return true if disconnect succeeded,
 *         false otherwise
 */
bool ProteusIII_Disconnect() {
    txPacket.Cmd = PROTEUSIII_CMD_DISCONNECT_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* Confirmation is sent before performing the disconnect. After
         * disconnect, the module sends a disconnect indication */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DISCONNECT_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Put the ProteusIII into sleep mode.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_Sleep() {
    txPacket.Cmd = PROTEUSIII_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SLEEP_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Disables the UART of the Proteus-III.
 *
 * It will be re-enabled when the module has to send data to the host (e.g. data
 * was received via radio or a state is indicated) or it can be manually
 * re-enabled using ProteusIII_PinWakeup().
 *
 * @return true if disable succeeded,
 *         false otherwise
 */
bool ProteusIII_UartDisable() {
    txPacket.Cmd = PROTEUSIII_CMD_UART_DISABLE_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_UART_DISABLE_CNF,
                        CMD_Status_Success, true);
    }
    return false;
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
bool ProteusIII_Transmit(uint8_t *payloadP, uint16_t length) {
    if ((length <= PROTEUSIII_MAX_RADIO_PAYLOAD_LENGTH) &&
        (ProteusIII_DriverState_BLE_ChannelOpen ==
         ProteusIII_GetDriverState())) {
        txPacket.Cmd = PROTEUSIII_CMD_DATA_REQ;
        txPacket.Length = length;

        memcpy(&txPacket.Data[0], payloadP, length);

        if (FillChecksum(&txPacket)) {
            ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                     txPacket.Length + LENGTH_CMD_OVERHEAD);
            return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_TXCOMPLETE_RSP,
                            CMD_Status_Success, true);
        }
    }
    return false;
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
bool ProteusIII_SetBeacon(uint8_t *beaconDataP, uint16_t length) {
    if (length <= PROTEUSIII_MAX_BEACON_LENGTH) {
        txPacket.Cmd = PROTEUSIII_CMD_SETBEACON_REQ;
        txPacket.Length = length;

        memcpy(&txPacket.Data[0], beaconDataP, length);

        if (FillChecksum(&txPacket)) {
            ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                     txPacket.Length + LENGTH_CMD_OVERHEAD);
            return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SETBEACON_CNF,
                            CMD_Status_Success, true);
        }
    }
    return false;
}

/*
 * @brief Factory reset of the module.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_FactoryReset() {
    txPacket.Cmd = PROTEUSIII_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for reset after factory reset */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                        CMD_Status_NoStatus, true);
    }
    return false;
}

/**
 * @brief Set a special user setting.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] userSetting:  user setting to be updated
 * @param[in] valueP:       pointer to the new settings value
 * @param[in] length:       length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Set(ProteusIII_UserSettings_t userSetting, uint8_t *valueP,
                    uint8_t length) {
    txPacket.Cmd = PROTEUSIII_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        if(Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SET_CNF,
                        CMD_Status_Success, true))
        {
            return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                        CMD_Status_NoStatus, false);
        }

    }
    return false;
}

/**
 * @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 *effect. Note: Use this function only in rare case, since flash can be updated
 *only a limited number of times.
 *
 * @param[in] deviceNameP: pointer to the device name
 * @param[in] nameLength:  length of the device name
 *
 *note: reset the module after the adaption of the setting such that it can take
 *effect note: use this function only in rare case, since flash can be updated
 *only a limited number times
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetDeviceName(uint8_t *deviceNameP, uint8_t nameLength) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_DEVICE_NAME, deviceNameP,
                          nameLength);
}

/**
 * @brief Set the BLE advertising timeout.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] advTimeout: advertising timeout in seconds (allowed values: 0-650,
 * where 0 = infinite)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetAdvertisingTimeout(uint16_t advTimeout) {
    uint8_t help[2];
    memcpy(help, (uint8_t *)&advTimeout, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_ADVERTISING_TIMEOUT, help,
                          2);
}

/**
 * @brief Set the advertising flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] advFlags: Advertising flags
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_SetAdvertisingFlags(ProteusIII_AdvertisingFlags_t advFlags) {
    uint8_t flags = (uint8_t)advFlags;
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_ADVERTISING_FLAGS, &flags,
                          1);
}

/**
 * @brief Set the scan flags (see ProteusIII_ScanFlags_t).
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] scanFlags: Scan flags (see ProteusIII_ScanFlags_t)
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_SetScanFlags(uint8_t scanFlags) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_FLAGS, &scanFlags, 1);
}

/**
 * @brief Set the beacon flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] beaconFlags: Beacon flags
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_SetBeaconFlags(ProteusIII_BeaconFlags_t beaconFlags) {
    uint8_t flags = (uint8_t)beaconFlags;
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_BEACON_FLAGS, &flags, 1);
}

/**
 * @brief Set the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] cfgFlags: CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_SetCFGFlags(uint16_t cfgFlags) {
    uint8_t help[2];
    memcpy(help, (uint8_t *)&cfgFlags, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_CFGFLAGS, help, 2);
}

/**
 * @brief Set the BLE connection timing.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] connectionTiming: connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetConnectionTiming(
    ProteusIII_ConnectionTiming_t connectionTiming) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_CONNECTION_TIMING,
                          (uint8_t *)&connectionTiming, 1);
}

/**
 * @brief Set the BLE scan timing
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] scanTiming: scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetScanTiming(ProteusIII_ScanTiming_t scanTiming) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_TIMING,
                          (uint8_t *)&scanTiming, 1);
}

/**
 * @brief Set the BLE scan factor
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] scanFactor: scan factor
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetScanFactor(uint8_t scanFactor) {
    if (scanFactor > 10) {
        return false;
    }
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_FACTOR, &scanFactor,
                          1);
}

/**
 * @brief Set the BLE TX power.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] txPower: TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetTXPower(ProteusIII_TXPower_t txPower) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_TX_POWER,
                          (uint8_t *)&txPower, 1);
}

/**
 * @brief Set the BLE security flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSecFlags(ProteusIII_SecFlags_t secFlags) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SEC_FLAGS,
                          (uint8_t *)&secFlags, 1);
}

/**
 * @brief Set the BLE security flags for peripheral only mode.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t secFlags) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SECFLAGSPERONLY,
                          (uint8_t *)&secFlags, 1);
}

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate
 * @param[in] parity: parity bit
 * @param[in] flowControlEnable: enable/disable flow control
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetBaudrateIndex(ProteusIII_BaudRate_t baudrate,
                                 ProteusIII_UartParity_t parity,
                                 bool flowControlEnable) {
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flow control is to be enabled UART index has to be increased by one in
     * regard to base value */
    if (flowControlEnable) {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of
     * base value*/
    if (ProteusIII_UartParity_Even == parity) {
        baudrateIndex += 64;
    }

    return ProteusIII_Set(ProteusIII_USERSETTING_UART_CONFIG_INDEX,
                          (uint8_t *)&baudrateIndex, 1);
}

/**
 * @brief Set the BLE static passkey
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] staticPasskeyP: pointer to the static passkey (6 digits)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetStaticPasskey(uint8_t *staticPasskeyP) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_STATIC_PASSKEY,
                          staticPasskeyP, 6);
}

/**
 * @brief Sets the Bluetooth appearance of the device
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] appearance: 2 byte Bluetooth appearance value (please check the
 * Bluetooth Core Specification: Core Specification Supplement, Part A,
 * section 1.12 for permissible values)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetAppearance(uint16_t appearance) {
    uint8_t help[2];
    memcpy(help, (uint8_t *)&appearance, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_APPEARANCE, help, 2);
}

/**
 * @brief Sets the base UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] uuidP: 16 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSppBaseUuid(uint8_t *uuidP) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPBASEUUID, uuidP, 16);
}

/**
 * @brief Sets the service UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSppServiceUuid(uint8_t *uuidP) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPServiceUUID, uuidP, 2);
}

/**
 * @brief Sets the RX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSppRxUuid(uint8_t *uuidP) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPRXUUID, uuidP, 2);
}

/**
 * @brief Sets the TX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take
 * effect. Note: Use this function only in rare case, since flash can be updated
 * only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSppTxUuid(uint8_t *uuidP) {
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPTXUUID, uuidP, 2);
}

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
bool ProteusIII_Get(ProteusIII_UserSettings_t userSetting, uint8_t *responseP,
                    uint16_t *responseLengthP) {
    txPacket.Cmd = PROTEUSIII_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GET_CNF, CMD_Status_Success,
                     true)) {
            uint16_t length = rxPacket.Length;
            memcpy(
                responseP, &rxPacket.Data[1],
                length -
                    1); /* First Data byte is status, following bytes response*/
            *responseLengthP = length - 1;
            return true;
        }
    }
    return false;
}

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: pointer to the 3 byte firmware version, version is
 * returned MSB first
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetFWVersion(uint8_t *versionP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_FWVersion, versionP,
                          &length);
}

/**
 * @brief Request device info.
 *
 * @param[out] deviceInfoP: pointer to the device info structure
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_GetDeviceInfo(ProteusIII_DeviceInfo_t *deviceInfoP) {
    uint8_t help[12];
    uint16_t length;
    if (!ProteusIII_Get(ProteusIII_USERSETTING_FS_DEVICE_INFO, help, &length)) {
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
bool ProteusIII_GetSerialNumber(uint8_t *serialNumberP) {
    uint16_t length;
    if (!ProteusIII_Get(ProteusIII_USERSETTING_FS_SERIAL_NUMBER, serialNumberP,
                        &length)) {
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
bool ProteusIII_GetDeviceName(uint8_t *deviceNameP, uint16_t *nameLengthP) {
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_DEVICE_NAME, deviceNameP,
                          nameLengthP);
}

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: pointer to the MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetMAC(uint8_t *macP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_MAC, macP, &length);
}

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: pointer to the Bluetooth MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetBTMAC(uint8_t *btMacP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_BTMAC, btMacP, &length);
}

/**
 * @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: pointer to the advertising timeout
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetAdvertisingTimeout(uint16_t *advTimeoutP) {
    uint16_t length;
    bool ret = false;
    uint8_t help[2];

    ret = ProteusIII_Get(ProteusIII_USERSETTING_RF_ADVERTISING_TIMEOUT, help,
                         &length);
    memcpy((uint8_t *)advTimeoutP, help, 2);

    return ret;
}

/**
 * @brief Request the advertising flags.
 *
 * @param[out] advFlagsP: pointer to the advertising flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetAdvertisingFlags(ProteusIII_AdvertisingFlags_t *advFlagsP) {
    uint16_t length;
    uint8_t flags;
    bool ret = ProteusIII_Get(ProteusIII_USERSETTING_RF_ADVERTISING_FLAGS,
                              &flags, &length);
    if (ret) {
        *advFlagsP = flags;
    }
    return ret;
}

/**
 * @brief Request the scan flags (see ProteusIII_ScanFlags_t).
 *
 * @param[out] scanFlagsP: pointer to the scan flags (see
 * ProteusIII_ScanFlags_t)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetScanFlags(uint8_t *scanFlagsP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_FLAGS, scanFlagsP,
                          &length);
}

/**
 * @brief Request the beacon flags.
 *
 * @param[out] beaconFlagsP: pointer to the beacon flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetBeaconFlags(ProteusIII_BeaconFlags_t *beaconFlagsP) {
    uint16_t length;
    uint8_t flags;
    bool ret =
        ProteusIII_Get(ProteusIII_USERSETTING_RF_BEACON_FLAGS, &flags, &length);
    if (ret) {
        *beaconFlagsP = flags;
    }
    return ret;
}

/**
 * @brief Request the connection timing
 *
 * @param[out] connectionTimingP: pointer to the connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetConnectionTiming(
    ProteusIII_ConnectionTiming_t *connectionTimingP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_CONNECTION_TIMING,
                          (uint8_t *)connectionTimingP, &length);
}

/**
 * @brief Request the scan timing
 *
 * @param[out] scanTimingP: pointer to the scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetScanTiming(ProteusIII_ScanTiming_t *scanTimingP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_TIMING,
                          (uint8_t *)scanTimingP, &length);
}

/**
 * @brief Request the scan factor
 *
 * @param[out] scanFactorP: pointer to the scan factor
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetScanFactor(uint8_t *scanFactorP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_FACTOR, scanFactorP,
                          &length);
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetTXPower(ProteusIII_TXPower_t *txPowerP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_TX_POWER,
                          (uint8_t *)txPowerP, &length);
}

/**
 * @brief Request the security flags
 *
 * @param[out] secFlagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSecFlags(ProteusIII_SecFlags_t *secFlagsP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SEC_FLAGS,
                          (uint8_t *)secFlagsP, &length);
}

/**
 * @brief Request the security flags for peripheral only mode
 *
 * @param[out] secFlagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t *secFlagsP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SECFLAGSPERONLY,
                          (uint8_t *)secFlagsP, &length);
}

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
bool ProteusIII_GetBaudrateIndex(ProteusIII_BaudRate_t *baudrateP,
                                 ProteusIII_UartParity_t *parityP,
                                 bool *flowControlEnableP) {
    uint16_t length;
    uint8_t uartIndex;

    if (ProteusIII_Get(ProteusIII_USERSETTING_UART_CONFIG_INDEX,
                       (uint8_t *)&uartIndex, &length)) {
        /* if index is even, flow control is off.
         * If flow control is on, decrease index by one to later determine the
         * base baudrate */
        if (0x01 == (uartIndex & 0x01)) {
            /* odd */
            *flowControlEnableP = true;
            uartIndex--;
        } else {
            /* even */
            *flowControlEnableP = false;
        }

        /* If baudrate index is greater than or equal to 64, parity bit is
         * even*/
        if (uartIndex < 64) {
            *parityP = ProteusIII_UartParity_None;
        } else {
            *parityP = ProteusIII_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ProteusIII_BaudRate_t)uartIndex;
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
bool ProteusIII_GetStaticPasskey(uint8_t *staticPasskeyP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_STATIC_PASSKEY,
                          staticPasskeyP, &length);
}

/**
 * @brief Request the Bluetooth appearance of the device
 *
 * @param[out] appearanceP: pointer to the Bluetooth appearance
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetAppearance(uint16_t *appearanceP) {
    uint16_t length;
    bool ret = false;
    uint8_t help[2];

    ret = ProteusIII_Get(ProteusIII_USERSETTING_RF_APPEARANCE, help, &length);
    memcpy((uint8_t *)appearanceP, help, 2);

    return ret;
}

/**
 * @brief Request the base UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 16 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSppBaseUuid(uint8_t *uuidP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPBASEUUID, uuidP,
                          &length);
}

/**
 * @brief Request the service UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSppServiceUuid(uint8_t *uuidP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPServiceUUID, uuidP,
                          &length);
}

/**
 * @brief Request the RX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSppRxUuid(uint8_t *uuidP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPRXUUID, uuidP, &length);
}

/**
 * @brief Request the TX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: pointer to the 2 byte UUID (MSB first)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSppTxUuid(uint8_t *uuidP) {
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPTXUUID, uuidP, &length);
}

/**
 * @brief Request the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @param[out] cfgFlags: pointer to the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetCFGFlags(uint16_t *cfgFlagsP) {
    uint16_t length;
    bool ret = false;
    uint8_t help[2];

    ret = ProteusIII_Get(ProteusIII_USERSETTING_RF_CFGFLAGS, help, &length);
    memcpy((uint8_t *)cfgFlagsP, help, 2);

    return ret;
}

/**
 * @brief Request the module state
 *
 * @param[out] moduleStateP: Pointer to module state
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetState(ProteusIII_ModuleState_t *moduleStateP) {
    txPacket.Cmd = PROTEUSIII_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                     CMD_Status_NoStatus, true)) {
            uint16_t length = rxPacket.Length;
            moduleStateP->role = rxPacket.Data[0];
            moduleStateP->action = rxPacket.Data[1];

            if (moduleStateP->action == ProteusIII_BLE_Action_Connected &&
                length >= 8) {
                memcpy(moduleStateP->connectedDeviceBtMac, &rxPacket.Data[2],
                       6);
            } else {
                memset(moduleStateP->connectedDeviceBtMac, 0, 6);
            }

            return true;
        }
    }

    return false;
}

/**
 * @brief Request the current state of the driver
 *
 * @return driver state
 */
ProteusIII_DriverState_t ProteusIII_GetDriverState() { return bleState; }

/**
 * @brief Start scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_ScanStart() {
    txPacket.Cmd = PROTEUSIII_CMD_SCANSTART_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SCANSTART_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Stop a scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_ScanStop() {
    txPacket.Cmd = PROTEUSIII_CMD_SCANSTOP_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SCANSTOP_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Request the scan results
 *
 * @param[out] devicesP: pointer to scan result struct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetDevices(ProteusIII_GetDevices_t *devicesP) {
    bool ret = false;

    ProteusIII_getDevicesP = devicesP;
    if (ProteusIII_getDevicesP != NULL) {
        ProteusIII_getDevicesP->numberOfDevices = 0;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GETDEVICES_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETDEVICES_CNF,
                       CMD_Status_Success, true);
    }

    ProteusIII_getDevicesP = NULL;
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
bool ProteusIII_Connect(uint8_t *btMacP) {
    txPacket.Cmd = PROTEUSIII_CMD_CONNECT_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], btMacP, 6);

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(3000, PROTEUSIII_CMD_CONNECT_CNF, CMD_Status_Success,
                        true);
    }
    return false;
}

/**
 * @brief Answer on a passkey request with a passkey to setup a connection
 *
 * @param[in] passkey: pointer to passkey
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Passkey(uint8_t *passkey) {
    txPacket.Cmd = PROTEUSIII_CMD_PASSKEY_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], passkey, 6);

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_PASSKEY_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Answer on a numeric comparison request
 *
 * @param[in] keyIsOk: boolean to confirm if the key shown is correct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_NumericCompareConfirm(bool keyIsOk) {
    txPacket.Cmd = PROTEUSIII_CMD_NUMERIC_COMP_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = keyIsOk ? 0x00 : 0x01;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_NUMERIC_COMP_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_PhyUpdate(ProteusIII_Phy_t phy) {
    if (ProteusIII_DriverState_BLE_ChannelOpen == ProteusIII_GetDriverState()) {
        txPacket.Cmd = PROTEUSIII_CMD_PHYUPDATE_REQ;
        txPacket.Length = 1;
        txPacket.Data[0] = (uint8_t)phy;

        if (FillChecksum(&txPacket)) {
            ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                     txPacket.Length + LENGTH_CMD_OVERHEAD);

            /* wait for cnf */
            return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_PHYUPDATE_CNF,
                            CMD_Status_Success, true);
        }
    }
    return false;
}

/**
 * @brief Sets the callback function which is executed if a byte has been
 * received from Proteus-III.
 *
 * The default callback is ProteusIII_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default
 * callback is used if NULL)
 */
void ProteusIII_SetByteRxCallback(ProteusIII_ByteRxCallback callback) {
    byteRxCallback = (callback == NULL) ? ProteusIII_HandleRxByte : callback;
}

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIOLocalWriteConfig(ProteusIII_GPIOConfigBlock_t *configP,
                                     uint16_t numberOfConfigs) {
    bool ret = false;
    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++) {
        switch (configP->function) {
            case ProteusIII_GPIO_IO_Disconnected: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Input: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Output: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_PWM: {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period,
                       2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
                break;
            }
            default:
                break;
        }

        /* Move pointer to next element. configP is increased by
         * sizeof(ProteusIII_GPIOConfigBlock_t)*/
        configP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
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
bool ProteusIII_GPIOLocalReadConfig(ProteusIII_GPIOConfigBlock_t *configP,
                                    uint16_t *numberOfConfigsP) {
    bool ret = false;

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF,
                       CMD_Status_Success, true);

        if (ret == true) {
            uint16_t length = rxPacket.Length;

            *numberOfConfigsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ProteusIII_GPIOConfigBlock_t *configP_running = configP;
            while (uartP < &rxPacket.Data[length]) {
                switch (*(uartP + 2)) {
                    case ProteusIII_GPIO_IO_Disconnected: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_Input: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.input = *(uartP + 3);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_Output: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.output = *(uartP + 3);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_PWM: {
                        if (*uartP == 5) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            memcpy(&configP_running->value.pwm.period,
                                   (uartP + 3), 2);
                            configP_running->value.pwm.ratio = *(uartP + 5);

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
        }
    }

    return ret;
}

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIOLocalWrite(ProteusIII_GPIOControlBlock_t *controlP,
                               uint16_t numberOfControls) {
    bool ret = false;
    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfControls; i++) {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;

        /* Move pointer to next element. configP is increased by
         * sizeof(ProteusIII_GPIOControlBlock_t)*/
        controlP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF,
                       CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of
 * GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIOLocalRead(uint8_t *gpioToReadP, uint8_t amountGPIOToRead,
                              ProteusIII_GPIOControlBlock_t *controlP,
                              uint16_t *numberOfControlsP) {
    bool ret = false;

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF,
                       CMD_Status_Success, true);

        if (ret) {
            uint16_t length = rxPacket.Length;

            *numberOfControlsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ProteusIII_GPIOControlBlock_t *controlP_running = controlP;
            while (uartP < &rxPacket.Data[length]) {
                /* each ControlBlock starts with length field which is currently
                 * fixed to "2" */
                if (*uartP == 2) {
                    controlP_running->gpioId = *(uartP + 1);
                    controlP_running->value.output = *(uartP + 2);

                    /* Move pointer to next element. configP is increased by
                     * sizeof(ProteusIII_GPIOControlBlock_t)*/
                    controlP_running++;
                    *numberOfControlsP += 1;
                }

                /* uartP points to length field of control block. So increase
                 * address by value of length + 1 */
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
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
bool ProteusIII_GPIORemoteWriteConfig(ProteusIII_GPIOConfigBlock_t *configP,
                                      uint16_t numberOfConfigs) {
    bool ret = false;
    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++) {
        switch (configP->function) {
            case ProteusIII_GPIO_IO_Disconnected: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Input: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Output: {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_PWM: {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period,
                       2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
                break;
            }
            default:
                break;
        }
        configP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret =
            Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF,
                     CMD_Status_Success, true);
    }

    return ret;
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
bool ProteusIII_GPIORemoteReadConfig(ProteusIII_GPIOConfigBlock_t *configP,
                                     uint16_t *numberOfConfigsP) {
    bool ret = false;

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF,
                       CMD_Status_Success, true);

        if (ret == true) {
            uint16_t length = rxPacket.Length;

            *numberOfConfigsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ProteusIII_GPIOConfigBlock_t *configP_running = configP;
            while (uartP < &rxPacket.Data[length]) {
                switch (*(uartP + 2)) {
                    case ProteusIII_GPIO_IO_Disconnected: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_Input: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.input = *(uartP + 3);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_Output: {
                        if (*uartP == 3) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            configP_running->value.output = *(uartP + 3);

                            configP_running++;
                            *numberOfConfigsP += 1;
                        }
                        break;
                    }
                    case ProteusIII_GPIO_IO_PWM: {
                        if (*uartP == 5) {
                            configP_running->gpioId = *(uartP + 1);
                            configP_running->function = *(uartP + 2);
                            memcpy(&configP_running->value.pwm.period,
                                   (uartP + 3), 2);
                            configP_running->value.pwm.ratio = *(uartP + 5);

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
        }
    }

    return ret;
}

/**
 * @brief Set the output value of the remote pin. Pin has to be configured
 * first. See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteWrite(ProteusIII_GPIOControlBlock_t *controlP,
                                uint16_t numberOfControls) {
    uint16_t length = 0;
    for (uint16_t i = 0; i < numberOfControls; i++) {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.ratio;
        length += 3;
        controlP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of
 * gpioToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteRead(uint8_t *gpioToReadP, uint8_t amountGPIOToRead,
                               ProteusIII_GPIOControlBlock_t *controlP,
                               uint16_t *numberOfControlsP) {
    bool ret = false;

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF,
                       CMD_Status_Success, true);

        if (ret) {
            uint16_t length = rxPacket.Length;

            *numberOfControlsP = 0;
            uint8_t *uartP = &rxPacket.Data[1];
            ProteusIII_GPIOControlBlock_t *controlP_running = controlP;
            while (uartP < &rxPacket.Data[length]) {
                if (*uartP == 2) {
                    controlP_running->gpioId = *(uartP + 1);
                    controlP_running->value.ratio = *(uartP + 2);

                    controlP_running++;
                    *numberOfControlsP += 1;
                }
                uartP += *uartP + 1;
            }
        }
    }

    return ret;
}

/**
 * @brief Requests the BTMAC addresses of all bonded devices.
 *
 * Note that this function supports a maximum of PROTEUSIII_MAX_BOND_DEVICES
 * returned devices. The Proteus-III module itself might be capable of
 * bonding more devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetBonds(ProteusIII_BondDatabase_t *bondDatabaseP) {
    txPacket.Cmd = PROTEUSIII_CMD_GET_BONDS_REQ;
    txPacket.Length = 0;

    if (!FillChecksum(&txPacket)) {
        return false;
    }

    ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                             txPacket.Length + LENGTH_CMD_OVERHEAD);

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GET_BONDS_CNF,
                  CMD_Status_Success, true)) {
        return false;
    }

    bondDatabaseP->nrOfDevices = rxPacket.Data[1];
    if (bondDatabaseP->nrOfDevices > PROTEUSIII_MAX_BOND_DEVICES) {
        bondDatabaseP->nrOfDevices = PROTEUSIII_MAX_BOND_DEVICES;
    }

    for (uint8_t i = 0; i < bondDatabaseP->nrOfDevices; i++) {
        uint8_t offset = 2 + i * 8;
        bondDatabaseP->devices[i].id =
            ((uint16_t)rxPacket.Data[offset] << 0) +
            ((uint16_t)rxPacket.Data[offset + 1] << 8);
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
bool ProteusIII_DeleteBonds() {
    txPacket.Cmd = PROTEUSIII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DELETE_BONDS_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Removes the bonding information for a single device.
 *
 * @param[in] bondId: bond ID of the device to be removed
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DeleteBond(uint8_t bondId) {
    txPacket.Cmd = PROTEUSIII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = bondId;
    txPacket.Data[1] = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DELETE_BONDS_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Temporarily allow unbonded connections, in case only bonded
 * connections have been configured
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_AllowUnbondedConnections() {
    txPacket.Cmd = PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME,
                        PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF,
                        CMD_Status_Success, true);
    }
    return false;
}

/**
 * @brief Enable the direct test mode (DTM)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DTMEnable() {
    txPacket.Cmd = PROTEUSIII_CMD_DTMSTART_REQ;
    txPacket.Length = 0;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF,
                     CMD_Status_NoStatus, true)) {
            if ((rxPacket.Data[0] == (uint8_t)ProteusIII_BLE_Role_DTM) &&
                (rxPacket.Data[1] == (uint8_t)ProteusIII_BLE_Action_DTM)) {
                return true;
            }
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
bool ProteusIII_DTMRun(ProteusIII_DTMCommand_t command,
                       uint8_t channel_vendoroption, uint8_t length_vendorcmd,
                       uint8_t payload) {
    txPacket.Cmd = PROTEUSIII_CMD_DTM_REQ;
    txPacket.Length = 4;
    txPacket.Data[0] = command;
    txPacket.Data[1] = channel_vendoroption;
    txPacket.Data[2] = length_vendorcmd;
    txPacket.Data[3] = payload;

    if (FillChecksum(&txPacket)) {
        ProteusIII_UART_Transmit((uint8_t *)&txPacket,
                                 txPacket.Length + LENGTH_CMD_OVERHEAD);

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DTM_CNF,
                        CMD_Status_Success, true);
    }
    return false;
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
bool ProteusIII_DTMStartTX(uint8_t channel, uint8_t length,
                           ProteusIII_DTMTXPattern_t pattern) {
    return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, channel, length,
                             (uint8_t)pattern);
}

/**
 * @brief Start the direct test mode (DTM) TX carrier test
 *
 * @param[in] channel: Channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DTMStartTXCarrier(uint8_t channel) {
    return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, channel, 0x00,
                             0x03);
}

/**
 * @brief Stop the current direct test mode (DTM) test
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DTMStop() {
    return ProteusIII_DTMRun(ProteusIII_DTMCommand_Stop, 0x00, 0x00, 0x01);
}

/**
 * @brief Set the phy for direct test mode (DTM) test
 *
 * @param[in] phy: Phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DTMSetPhy(ProteusIII_Phy_t phy) {
    return ProteusIII_DTMRun(ProteusIII_DTMCommand_Setup, 0x02, (uint8_t)phy,
                             0x00);
}

/**
 * @brief Set the TX power for direct test mode (DTM) test
 *
 * @param[in] power: Tx power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_DTMSetTXPower(ProteusIII_TXPower_t power) {
    return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, (uint8_t)power,
                             0x02, 0x03);
}
