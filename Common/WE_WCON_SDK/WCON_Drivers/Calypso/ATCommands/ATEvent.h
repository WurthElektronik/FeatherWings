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
 * @brief AT event definitions.
 */

#ifndef CALYPSO_AT_EVENTS_H_INCLUDED
#define CALYPSO_AT_EVENTS_H_INCLUDED
#include <Calypso/ATCommands/ATMQTT.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT event IDs.
 */
    typedef enum Calypso_ATEvent_t
    {
        Calypso_ATEvent_Invalid = (uint16_t)0,

        Calypso_ATEvent_Startup,
        Calypso_ATEvent_WakeUp,

        Calypso_ATEvent_Ping,

        Calypso_ATEvent_GeneralResetRequest,
        Calypso_ATEvent_GeneralError,

        Calypso_ATEvent_WlanConnect,
        Calypso_ATEvent_WlanDisconnect,
        Calypso_ATEvent_WlanStaAdded,
        Calypso_ATEvent_WlanStaRemoved,
        Calypso_ATEvent_WlanP2PConnect,
        Calypso_ATEvent_WlanP2PDisconnect,
        Calypso_ATEvent_WlanP2PClientAdded,
        Calypso_ATEvent_WlanP2PClientRemoved,
        Calypso_ATEvent_WlanP2PDevFound,
        Calypso_ATEvent_WlanP2PRequest,
        Calypso_ATEvent_WlanP2PConnectFail,
        Calypso_ATEvent_WlanProvisioningStatus,
        Calypso_ATEvent_WlanProvisioningProfileAdded,

        Calypso_ATEvent_SocketTxFailed,
        Calypso_ATEvent_SocketAsyncEvent,
        Calypso_ATEvent_SocketTCPConnect,
        Calypso_ATEvent_SocketTCPAccept,
        Calypso_ATEvent_SocketSelect,
        Calypso_ATEvent_SocketRcvd,
        Calypso_ATEvent_SocketRcvdFrom,

        Calypso_ATEvent_NetappIP4Acquired,
        Calypso_ATEvent_NetappIP6Acquired,
        Calypso_ATEvent_NetappIPCollision,
        Calypso_ATEvent_NetappDHCPv4_leased,
        Calypso_ATEvent_NetappDHCPv4_released,
        Calypso_ATEvent_NetappIPv4Lost,
        Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout,
        Calypso_ATEvent_NetappIPv6Lost,

        Calypso_ATEvent_MQTTOperation,
        Calypso_ATEvent_MQTTRecv,
        Calypso_ATEvent_MQTTDisconnect,

        Calypso_ATEvent_MQTTConnack,
        Calypso_ATEvent_MQTTPuback,
        Calypso_ATEvent_MQTTSuback,
        Calypso_ATEvent_MQTTUnsuback,

        Calypso_ATEvent_FileListEntry,

        Calypso_ATEvent_HTTPGet,

        Calypso_ATEvent_CustomGPIO,
        Calypso_ATEvent_CustomHTTPPost,

        Calypso_ATEvent_FatalErrorDeviceAbort,
        Calypso_ATEvent_FatalErrorDriverAbort,
        Calypso_ATEvent_FatalErrorSyncLost,
        Calypso_ATEvent_FatalErrorNoCmdAck,
        Calypso_ATEvent_FatalErrorCmdTimeout,

        Calypso_ATEvent_NumberOfValues,
        Calypso_ATEvent_Max = UINT16_MAX
    } Calypso_ATEvent_t;

    /**
 * @brief Custom event IDs (first argument of "+eventcustom" event).
 */
    typedef enum Calypso_ATEvent_CustomEventID_t
    {
        Calypso_ATEvent_CustomEventID_GPIO,
        Calypso_ATEvent_CustomEventID_HTTPPost,
        Calypso_ATEvent_CustomEventID_NumberOfValues
    } Calypso_ATEvent_CustomEventID_t;

    /**
 * @brief Parameters of startup event (Calypso_ATEvent_Startup).
 */
    typedef struct Calypso_ATEvent_Startup_t
    {
        char articleNr[16];
        char chipID[12];
        char MACAddress[18];
        uint8_t firmwareVersion[3];
    } Calypso_ATEvent_Startup_t;

    /**
 * @brief Parameters of ping event (Calypso_ATEvent_Ping).
 */
    typedef struct Calypso_ATEvent_Ping_t
    {
        uint16_t packetsSent;
        uint16_t packetsReceived;
        uint16_t roundTripTimeMs;
    } Calypso_ATEvent_Ping_t;

    /**
 * @brief Parameters of Socket tx failed event (Calypso_ATEvent_SocketTXFailed_t).
 */
    typedef struct Calypso_ATEvent_SocketTXFailed_t
    {
        uint8_t socketID;
        int16_t errorCode;
    } Calypso_ATEvent_SocketTXFailed_t;

    /**
 * @brief Parameters of TCP connect event (Calypso_ATEvent_SocketTCPConnect).
 */
    typedef struct Calypso_ATEvent_SocketTCPConnect_t
    {
        uint16_t serverPort;
        char serverAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
    } Calypso_ATEvent_SocketTCPConnect_t;

    /**
 * @brief Parameters of TCP accept event (Calypso_ATEvent_SocketTCPAccept).
 */
    typedef struct Calypso_ATEvent_SocketTCPAccept_t
    {
        uint8_t socketID;
        Calypso_ATSocket_Family_t family;
        uint16_t clientPort;
        char clientAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
    } Calypso_ATEvent_SocketTCPAccept_t;

    /**
 * @brief Parameters of TCP data received event (Calypso_ATEvent_SocketRcvd).
 */
    typedef struct Calypso_ATEvent_SocketRcvd_t
    {
        uint8_t socketID;
        uint8_t format;
        uint16_t length;
        char data[CALYPSO_RECEIVE_BUFFER_SIZE];
    } Calypso_ATEvent_SocketRcvd_t;

    /**
 * @brief Parameters of MQTT data received event (+eventmqtt:recv).
 */
    typedef struct Calypso_ATEvent_MQTTRcvd_t
    {
        char topic[CALYPSO_MAX_HOST_NAME_LENGTH];
        Calypso_ATMQTT_QoS_t qos;
        uint8_t retain;
        uint8_t duplicate;
        Calypso_DataFormat_t dataFormat;
        uint16_t dataLength;
        char data[CALYPSO_RECEIVE_BUFFER_SIZE];
    } Calypso_ATEvent_MQTTRcvd_t;

    /**
 * @brief Parameters of IPv4 acquired event (Calypso_ATEvent_NetappIP4Acquired).
 */
    typedef struct Calypso_ATEvent_NetappIP4Acquired_t
    {
        char address[16];
        char gateway[16];
        char DNS[16];
    } Calypso_ATEvent_NetappIP4Acquired_t;

    /**
 * @brief Connack return codes.
 */
    typedef enum MQTTConnack_Return_Code_t
    {
        MQTTConnack_Return_Code_Accepted = 0x0,
        MQTTConnack_Return_Code_Identifier_Rejected = 0x1,
        MQTTConnack_Return_Code_Server_Unavailable = 0x2,
        MQTTConnack_Return_Code_Bad_Username_Password = 0x3,
        MQTTConnack_Return_Code_Not_Authorised = 0x4,
    } MQTTConnack_Return_Code_t;

    /**
 * @brief Parameters of MQTT connect event (Calypso_ATEvent_MQTTConnack).
 */
    typedef struct Calypso_ATEvent_MQTTConnack_t
    {
        uint8_t ackFlags;
        MQTTConnack_Return_Code_t returnCode;
    } Calypso_ATEvent_MQTTConnack_t;

    extern bool Calypso_ATEvent_ParseEventType(char** pAtCommand, Calypso_ATEvent_t* pEvent);

    extern bool Calypso_ATEvent_ParseStartUpEvent(char** pEventArguments, Calypso_ATEvent_Startup_t* Calypso_Examples_startupEvent);
    extern bool Calypso_ATEvent_ParsePingEvent(char** pEventArguments, Calypso_ATEvent_Ping_t* pingEvent);
    extern bool Calypso_ATEvent_ParseSocketTCPConnectEvent(char** pEventArguments, Calypso_ATEvent_SocketTCPConnect_t* connectEvent);
    extern bool Calypso_ATEvent_ParseSocketTCPAcceptEvent(char** pEventArguments, Calypso_ATEvent_SocketTCPAccept_t* acceptEvent);
    extern bool Calypso_ATEvent_ParseSocketTXFailedEvent(char** pEventArguments, Calypso_ATEvent_SocketTXFailed_t* txFailedtEvent);
    extern bool Calypso_ATEvent_ParseSocketRcvdEvent(char** pEventArguments, bool decodeBase64, Calypso_ATEvent_SocketRcvd_t* rcvdEvent);
    extern bool Calypso_ATEvent_ParseNetappIP4AcquiredEvent(char** pEventArguments, Calypso_ATEvent_NetappIP4Acquired_t* ipv4Event);
    extern bool Calypso_ATEvent_ParseHttpGetEvent(char** pEventArguments, char* id, uint16_t maxIdLength);
    extern bool Calypso_ATEvent_ParseFileListEntryEvent(char** pEventArguments, Calypso_ATFile_FileListEntry_t* fileListEntry);
    extern bool Calypso_ATEvent_ParseCustomGPIOEvent(char** pEventArguments, uint8_t* gpioId);
    extern bool Calypso_ATEvent_ParseCustomHTTPPostEvent(char** pEventArguments, char* id, char* value, uint16_t maxIdLength, uint16_t maxValueLength);
    bool Calypso_ATEvent_ParseSocketMQTTRcvdEvent(char** pEventArguments, Calypso_ATEvent_MQTTRcvd_t* rcvdEvent);
    bool Calypso_ATEvent_ParseMQTTConnackEvent(char** pEventArguments, Calypso_ATEvent_MQTTConnack_t* connackEvent);

#ifdef __cplusplus
}
#endif

#endif // CALYPSO_AT_EVENTS_H_INCLUDED
