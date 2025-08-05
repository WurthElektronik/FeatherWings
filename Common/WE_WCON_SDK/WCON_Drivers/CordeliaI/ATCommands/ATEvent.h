/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
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

#ifndef CORDELIAI_AT_EVENTS_H_INCLUDED
#define CORDELIAI_AT_EVENTS_H_INCLUDED
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT event IDs.
 */
    typedef enum CordeliaI_ATEvent_t
    {
        CordeliaI_ATEvent_Invalid = (uint16_t)0,
        CordeliaI_ATEvent_Startup,
        CordeliaI_ATEvent_GeneralResetRequest,
        CordeliaI_ATEvent_GeneralError,
        CordeliaI_ATEvent_WlanConnect,
        CordeliaI_ATEvent_WlanDisconnect,
        CordeliaI_ATEvent_WlanStaAdded,
        CordeliaI_ATEvent_WlanStaRemoved,
        CordeliaI_ATEvent_SocketTxFailed,
        CordeliaI_ATEvent_SocketAsyncEvent,
        CordeliaI_ATEvent_NetappIP4Acquired,
        CordeliaI_ATEvent_MQTTError,
        CordeliaI_ATEvent_MQTTRecv,
        CordeliaI_ATEvent_MQTTInfo,
        CordeliaI_ATEvent_IoTInfo,
        CordeliaI_ATEvent_IoTError,
        CordeliaI_ATEvent_FatalErrorDeviceAbort,
        CordeliaI_ATEvent_FatalErrorDriverAbort,
        CordeliaI_ATEvent_FatalErrorSyncLost,
        CordeliaI_ATEvent_FatalErrorNoCmdAck,
        CordeliaI_ATEvent_FatalErrorCmdTimeout,
        CordeliaI_ATEvent_NumberOfValues,
        CordeliaI_ATEvent_Max = UINT16_MAX
    } CordeliaI_ATEvent_t;

    /**
 * @brief Custom event IDs (first argument of "+eventcustom" event).
 */
    typedef enum CordeliaI_ATEvent_CustomEventID_t
    {
        CordeliaI_ATEvent_CustomEventID_GPIO,
        CordeliaI_ATEvent_CustomEventID_HTTPPost,
        CordeliaI_ATEvent_CustomEventID_NumberOfValues
    } CordeliaI_ATEvent_CustomEventID_t;

    /**
 * @brief Parameters of startup event (CordeliaI_ATEvent_Startup).
 */
    typedef struct CordeliaI_ATEvent_Startup_t
    {
        char articleNr[16];
        char chipID[12];
        char MACAddress[18];
        uint8_t firmwareVersion[3];
    } CordeliaI_ATEvent_Startup_t;

    /**
 * @brief Parameters of IoT event (CordeliaI_ATEvent_Startup).
 */
    typedef struct CordeliaI_ATEvent_IoT_t
    {
        char statusMessage[32];
        uint16_t statusCode;
    } CordeliaI_ATEvent_IoT_t;

    /**
 * @brief Parameters of MQTT event (CordeliaI_ATEvent_Startup).
 */
    typedef struct CordeliaI_ATEvent_MQTT_t
    {
        char statusMessage[32];
        uint16_t statusCode;
    } CordeliaI_ATEvent_MQTT_t;

    /**
 * @brief Parameters of MQTT recv event (CordeliaI_ATEvent_Startup).
 */
    typedef struct CordeliaI_ATEvent_MQTTRecv_t
    {
        char topic[32];
        char qos[8];
        char message[128];
    } CordeliaI_ATEvent_MQTTRecv_t;

    /**
 * @brief Parameters of IPv4 acquired event (CordeliaI_ATEvent_NetappIP4Acquired).
 */
    typedef struct CordeliaI_ATEvent_NetappIP4Acquired_t
    {
        char address[16];
        char gateway[16];
        char DNS[16];
    } CordeliaI_ATEvent_NetappIP4Acquired_t;

    extern bool CordeliaI_ATEvent_ParseEventType(char** pAtCommand, CordeliaI_ATEvent_t* pEvent);
    extern bool CordeliaI_ATEvent_ParseStartUpEvent(char** pEventArguments, CordeliaI_ATEvent_Startup_t* CordeliaI_Examples_startupEvent);
    extern bool CordeliaI_ATEvent_ParseIoTEvent(char** pEventArguments, CordeliaI_ATEvent_IoT_t* CordeliaI_Examples_IoTEvent);
    extern bool CordeliaI_ATEvent_ParseMQTTEvent(char** pEventArguments, CordeliaI_ATEvent_MQTT_t* CordeliaI_Examples_MQTTEvent);
    extern bool CordeliaI_ATEvent_ParseNetappIP4AcquiredEvent(char** pEventArguments, CordeliaI_ATEvent_NetappIP4Acquired_t* ipv4Event);
    extern bool CordeliaI_ATEvent_ParseHttpGetEvent(char** pEventArguments, char* id, uint16_t maxIdLength);
    extern bool CordeliaI_ATEvent_ParseSocketMQTTRcvdEvent(char** pEventArguments, CordeliaI_ATEvent_MQTTRecv_t* CordeliaI_Examples_MQTTRecvEvent);

#ifdef __cplusplus
}
#endif

#endif // CORDELIAI_AT_EVENTS_H_INCLUDED
