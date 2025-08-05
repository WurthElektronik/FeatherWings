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

#ifndef STEPHANOI_EVENTS_H_INCLUDED
#define STEPHANOI_EVENTS_H_INCLUDED

#include <StephanoI/ATCommands/ATBluetoothLE.h>
#include <StephanoI/ATCommands/ATHTTP.h>
#include <StephanoI/ATCommands/ATMQTT.h>
#include <StephanoI/ATCommands/ATSocket.h>
#include <StephanoI/ATCommands/ATWebServer.h>
#include <StephanoI/ATCommands/ATWifi.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT event IDs.
 */
    typedef enum StephanoI_ATEvent_t
    {
        StephanoI_ATEvent_Invalid = (uint16_t)0,

        StephanoI_ATEvent_Startup,
        StephanoI_ATEvent_Busy,
        StephanoI_ATEvent_RestartForced,
        StephanoI_ATEvent_ErrorCode,

        StephanoI_ATEvent_BLE_Peripheral_DiscoverService,
        StephanoI_ATEvent_BLE_Peripheral_DiscoverCharacteristics,
        StephanoI_ATEvent_BLE_Connection,
        StephanoI_ATEvent_BLE_Disconnection,
        StephanoI_ATEvent_BLE_MTU,
        StephanoI_ATEvent_BLE_Write,
        StephanoI_ATEvent_BLE_Read,
        StephanoI_ATEvent_BLE_Notify,
        StephanoI_ATEvent_BLE_Indicate,
        StephanoI_ATEvent_BLE_Central_Scan,
        StephanoI_ATEvent_BLE_Central_DiscoverPrimaryService,
        StephanoI_ATEvent_BLE_Central_DiscoverIncludedServices,
        StephanoI_ATEvent_BLE_Central_DiscoverCharacteristics,
        StephanoI_ATEvent_BLE_SetPhy,
        StephanoI_ATEvent_BLE_ReceivedEncryptionRequest,
        StephanoI_ATEvent_BLE_AuthenticationComplete,
        StephanoI_ATEvent_BLE_NotifySecurityKey,
        StephanoI_ATEvent_BLE_SecurityKeyRequest,
        StephanoI_ATEvent_BLE_SecurityConfirmKeyRequest,
        StephanoI_ATEvent_BLE_BondingInformation,

        StephanoI_ATEvent_Wifi_Mode,
        StephanoI_ATEvent_Wifi_Scan,
        StephanoI_ATEvent_Wifi_Connected,
        StephanoI_ATEvent_Wifi_Disconnected,
        StephanoI_ATEvent_Wifi_IPAquired,
        StephanoI_ATEvent_Wifi_IPv6LLAquired,
        StephanoI_ATEvent_Wifi_IPv6GLAquired,
        StephanoI_ATEvent_Wifi_State,
        StephanoI_ATEvent_Wifi_GetConnectedIPs,
        StephanoI_ATEvent_Wifi_GetStationIP,
        StephanoI_ATEvent_Wifi_GetAPIP,
        StephanoI_ATEvent_Wifi_GetStationMAC,
        StephanoI_ATEvent_Wifi_GetAPMAC,
        StephanoI_ATEvent_Wifi_StationhasConnected,
        StephanoI_ATEvent_Wifi_StationhasDisconnected,
        StephanoI_ATEvent_Wifi_APAssignIP,

        StephanoI_ATEvent_Socket_Connected,
        StephanoI_ATEvent_Socket_Closed,
        StephanoI_ATEvent_Socket_State,
        StephanoI_ATEvent_Socket_ConnectionInformation,
        StephanoI_ATEvent_Socket_Ping,
        StephanoI_ATEvent_Socket_DomainResolved,
        StephanoI_ATEvent_Socket_SendState,
        StephanoI_ATEvent_Socket_Receive,
        StephanoI_ATEvent_Socket_ReceiveLen,
        StephanoI_ATEvent_Socket_ReceiveData,
        StephanoI_ATEvent_Socket_GetMultiple,

        StephanoI_ATEvent_SNTP_Time,

        StephanoI_ATEvent_MQTT_Connected,
        StephanoI_ATEvent_MQTT_ConnectionInfo,
        StephanoI_ATEvent_MQTT_Disconnected,
        StephanoI_ATEvent_MQTT_SubscriptionReceive,
        StephanoI_ATEvent_MQTT_Subscriptions,
        StephanoI_ATEvent_MQTT_PublishOK,
        StephanoI_ATEvent_MQTT_PublishFailed,

        StephanoI_ATEvent_HTTP_Client,
        StephanoI_ATEvent_HTTP_GetSize,
        StephanoI_ATEvent_HTTP_Get,

        StephanoI_ATEvent_Webserver_Response,

        StephanoI_ATEvent_Device_SYSMFG,
        StephanoI_ATEvent_Device_SYSTimeStamp,
        StephanoI_ATEvent_Device_SYSStore,
        StephanoI_ATEvent_Device_SYSTemp,

        StephanoI_ATEvent_NumberOfValues,
        StephanoI_ATEvent_Max = UINT16_MAX
    } StephanoI_ATEvent_t;

    extern bool StephanoI_ATEvent_ParseEventType(char** pAtCommand, StephanoI_ATEvent_t* pEvent);

#ifdef __cplusplus
}
#endif

#endif // STEPHANOI_EVENTS_H_INCLUDED
