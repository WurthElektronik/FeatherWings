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
 * @brief AT event definitions.
 */

#ifndef AT_EVENTS_H_INCLUDED
#define AT_EVENTS_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdint.h>

#include "ATSocket.h"
#include "ATMQTT.h"

#define ATEvent_General_NumberOfValues 2
#define ATEvent_WLAN_NumberOfValues 13
#define ATEvent_Socket_NumberOfValues 6
#define ATEvent_Netapp_NumberOfValues 8
#define ATEvent_MQTT_NumberOfValues 3
#define ATEvent_FatalError_NumberOfValues 5

/**
 * @brief Size of receive buffer in ATEvent_SocketRcvd_t struct.
 */
#define ATEVENT_RCVDEVENTBUFFERSIZE CALYPSO_RECEIVE_BUFFER_SIZE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AT event IDs.
 */
typedef enum ATEvent_t {
    ATEvent_Invalid,

    ATEvent_Startup,
    ATEvent_WakeUp,

    ATEvent_Ping,

    ATEvent_General, /**< Offset for general events */
    ATEvent_GeneralResetRequest = ATEvent_General,
    ATEvent_GeneralError,

    ATEvent_Wlan, /**< Offset for WLAN events */
    ATEvent_WlanConnect = ATEvent_Wlan,
    ATEvent_WlanDisconnect,
    ATEvent_WlanStaAdded,
    ATEvent_WlanStaRemoved,
    ATEvent_WlanP2PConnect,
    ATEvent_WlanP2PDisconnect,
    ATEvent_WlanP2PClientAdded,
    ATEvent_WlanP2PClientRemoved,
    ATEvent_WlanP2PDevFound,
    ATEvent_WlanP2PRequest,
    ATEvent_WlanP2PConnectFail,
    ATEvent_WlanProvisioningStatus,
    ATEvent_WlanProvisioningProfileAdded,

    ATEvent_Socket, /**< Offset for socket events */
    ATEvent_SocketTxFailed = ATEvent_Socket,
    ATEvent_SocketAsyncEvent,
    ATEvent_SocketTCPConnect,
    ATEvent_SocketTCPAccept,
    ATEvent_SocketRcvd,
    ATEvent_SocketRcvdFrom,

    ATEvent_Netapp, /**< Offset for network application events */
    ATEvent_NetappIP4Acquired = ATEvent_Netapp,
    ATEvent_NetappIP6Acquired,
    ATEvent_NetappIPCollision,
    ATEvent_NetappDHCPv4_leased,
    ATEvent_NetappDHCPv4_released,
    ATEvent_NetappIPv4Lost,
    ATEvent_NetappDHCPIPv4AcquireTimeout,
    ATEvent_NetappIPv6Lost,

    ATEvent_MQTT, /**< Offset for MQTT events */
    ATEvent_MQTTOperation = ATEvent_MQTT,
    ATEvent_MQTTRecv,
    ATEvent_MQTTDisconnect,

    ATEvent_MQTTConnack,
    ATEvent_MQTTPuback,
    ATEvent_MQTTSuback,
    ATEvent_MQTTUnsuback,

    ATEvent_FileListEntry,

    ATEvent_HTTPGet,

    ATEvent_Custom,
    ATEvent_CustomGPIO = ATEvent_Custom,
    ATEvent_CustomHTTPPost,

    ATEvent_FatalError, /**< Offset for fatal error events */
    ATEvent_FatalErrorDeviceAbort = ATEvent_FatalError,
    ATEvent_FatalErrorDriverAbort,
    ATEvent_FatalErrorSyncLost,
    ATEvent_FatalErrorNoCmdAck,
    ATEvent_FatalErrorCmdTimeout,

    ATEvent_NumberOfValues
} ATEvent_t;

/**
 * @brief Custom event IDs (first argument of "+eventcustom" event).
 */
typedef enum ATEvent_CustomEventID_t {
    ATEvent_CustomEventID_GPIO,
    ATEvent_CustomEventID_HTTPPost,
    ATEvent_CustomEventID_NumberOfValues
} ATEvent_CustomEventID_t;

/**
 * @brief Parameters of startup event (ATEvent_Startup).
 */
typedef struct ATEvent_Startup_t {
    char articleNr[16];
    char chipID[12];
    char MACAddress[18];
    uint8_t firmwareVersion[3];
} ATEvent_Startup_t;

/**
 * @brief Parameters of ping event (ATEvent_Ping).
 */
typedef struct ATEvent_Ping_t {
    uint16_t packetsSent;
    uint16_t packetsReceived;
    uint16_t roundTripTimeMs;
} ATEvent_Ping_t;

/**
 * @brief Parameters of TCP connect event (ATEvent_SocketTCPConnect).
 */
typedef struct ATEvent_SocketTCPConnect_t {
    uint16_t serverPort;
    char serverAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
} ATEvent_SocketTCPConnect_t;

/**
 * @brief Parameters of TCP accept event (ATEvent_SocketTCPAccept).
 */
typedef struct ATEvent_SocketTCPAccept_t {
    uint8_t socketID;
    ATSocket_Family_t family;
    uint16_t clientPort;
    char clientAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
} ATEvent_SocketTCPAccept_t;

/**
 * @brief Parameters of TCP data received event (ATEvent_SocketRcvd).
 */
typedef struct ATEvent_SocketRcvd_t {
    uint8_t socketID;
    uint8_t format;
    uint16_t length;
    char data[ATEVENT_RCVDEVENTBUFFERSIZE];
} ATEvent_SocketRcvd_t;

/**
 * @brief Parameters of MQTT data received event (+eventmqtt:recv).
 */
typedef struct ATEvent_MQTTRcvd_t {
    char topic[CALYPSO_MAX_HOST_NAME_LENGTH];
    ATMQTT_QoS_t qos;
    uint8_t retain;
    uint8_t duplicate;
    Calypso_DataFormat_t dataFormat;
    uint16_t dataLength;
    char data[ATEVENT_RCVDEVENTBUFFERSIZE];
} ATEvent_MQTTRcvd_t;

/**
 * @brief Parameters of IPv4 acquired event (ATEvent_NetappIP4Acquired).
 */
typedef struct ATEvent_NetappIP4Acquired_t {
    char address[16];
    char gateway[16];
    char DNS[16];
} ATEvent_NetappIP4Acquired_t;

/**
 * @brief Connack return codes.
 */
typedef enum MQTTConnack_Return_Code_t {
    MQTTConnack_Return_Code_Accepted = 0x0,
    MQTTConnack_Return_Code_Identifier_Rejected = 0x1,
    MQTTConnack_Return_Code_Server_Unavailable = 0x2,
    MQTTConnack_Return_Code_Bad_Username_Password = 0x3,
    MQTTConnack_Return_Code_Not_Authorised = 0x4,
} MQTTConnack_Return_Code_t;

/**
 * @brief Parameters of MQTT connect event (ATEvent_MQTTConnack).
 */
typedef struct ATEvent_MQTTConnack_t {
    uint8_t ackFlags;
    MQTTConnack_Return_Code_t returnCode;
} ATEvent_MQTTConnack_t;

extern bool ATEvent_ParseEventType(char **pAtCommand, ATEvent_t *pEvent);

extern bool ATEvent_GetEventName(ATEvent_t event, char *pEventName);

extern bool ATEvent_ParseStartUpEvent(
    char **pEventArguments, ATEvent_Startup_t *Calypso_Examples_startupEvent);
extern bool ATEvent_ParsePingEvent(char **pEventArguments,
                                   ATEvent_Ping_t *pingEvent);
extern bool ATEvent_ParseSocketTCPConnectEvent(
    char **pEventArguments, ATEvent_SocketTCPConnect_t *connectEvent);
extern bool ATEvent_ParseSocketTCPAcceptEvent(
    char **pEventArguments, ATEvent_SocketTCPAccept_t *acceptEvent);
extern bool ATEvent_ParseSocketRcvdEvent(char **pEventArguments,
                                         bool decodeBase64,
                                         ATEvent_SocketRcvd_t *rcvdEvent);
extern bool ATEvent_ParseNetappIP4AcquiredEvent(
    char **pEventArguments, ATEvent_NetappIP4Acquired_t *ipv4Event);
extern bool ATEvent_ParseHttpGetEvent(char **pEventArguments, char *id,
                                      uint16_t maxIdLength);
extern bool ATEvent_ParseFileListEntryEvent(
    char **pEventArguments, ATFile_FileListEntry_t *fileListEntry);
extern bool ATEvent_ParseCustomGPIOEvent(char **pEventArguments,
                                         uint8_t *gpioId);
extern bool ATEvent_ParseCustomHTTPPostEvent(char **pEventArguments, char *id,
                                             char *value, uint16_t maxIdLength,
                                             uint16_t maxValueLength);
bool ATEvent_ParseSocketMQTTRcvdEvent(char **pEventArguments,
                                      ATEvent_MQTTRcvd_t *rcvdEvent);
bool ATEvent_ParseMQTTConnackEvent(char **pEventArguments,
                                   ATEvent_MQTTConnack_t *connackEvent);

#ifdef __cplusplus
}
#endif

#endif  // AT_EVENTS_H_INCLUDED
