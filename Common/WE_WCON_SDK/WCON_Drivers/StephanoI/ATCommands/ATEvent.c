/**
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
 **/

/**
 * @file
 * @brief AT event definitions.
 */

#include <StephanoI/ATCommands/ATEvent.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

static const ATCommand_Event_t MQTTSubEvents[] = {EVENTENTRY("OK", StephanoI_ATEvent_MQTT_PublishOK) LASTEVENTENTRY("FAIL", StephanoI_ATEvent_MQTT_PublishFailed)};

static const ATCommand_Event_t WildCardSubEvents[] = {EVENTENTRY("CONNECT", StephanoI_ATEvent_Socket_Connected) LASTEVENTENTRY("CLOSED", StephanoI_ATEvent_Socket_Closed)};

static const ATCommand_Event_t moduleMainEvents[] = {
    EVENTENTRY("ready", StephanoI_ATEvent_Startup) EVENTENTRY("busy p...", StephanoI_ATEvent_Busy) EVENTENTRY("+BLEGATTSSRV", StephanoI_ATEvent_BLE_Peripheral_DiscoverService) EVENTENTRY("+BLEGATTSCHAR", StephanoI_ATEvent_BLE_Peripheral_DiscoverCharacteristics) EVENTENTRY("+BLECONN", StephanoI_ATEvent_BLE_Connection) EVENTENTRY("+BLEDISCONN", StephanoI_ATEvent_BLE_Disconnection)
        EVENTENTRY("+BLECFGMTU", StephanoI_ATEvent_BLE_MTU) EVENTENTRY("+WRITE", StephanoI_ATEvent_BLE_Write) EVENTENTRY("+BLEGATTCRD", StephanoI_ATEvent_BLE_Read) EVENTENTRY("+NOTIFY", StephanoI_ATEvent_BLE_Notify) EVENTENTRY("+INDICATE", StephanoI_ATEvent_BLE_Indicate) EVENTENTRY("+BLESCAN", StephanoI_ATEvent_BLE_Central_Scan)
            EVENTENTRY("+BLEGATTCPRIMSRV", StephanoI_ATEvent_BLE_Central_DiscoverPrimaryService) EVENTENTRY("+BLEGATTCINCLSRV", StephanoI_ATEvent_BLE_Central_DiscoverIncludedServices) EVENTENTRY("+BLEGATTCCHAR", StephanoI_ATEvent_BLE_Central_DiscoverCharacteristics) EVENTENTRY("+BLESETPHY", StephanoI_ATEvent_BLE_SetPhy) EVENTENTRY("+BLESECREQ", StephanoI_ATEvent_BLE_ReceivedEncryptionRequest)
                EVENTENTRY("+BLEAUTHCMPL", StephanoI_ATEvent_BLE_AuthenticationComplete) EVENTENTRY("+BLESECNTFYKEY", StephanoI_ATEvent_BLE_NotifySecurityKey) EVENTENTRY("+BLESECKEYREQ", StephanoI_ATEvent_BLE_SecurityKeyRequest) EVENTENTRY("+BLESECNCREQ", StephanoI_ATEvent_BLE_SecurityConfirmKeyRequest) EVENTENTRY("+BLEENCDEV", StephanoI_ATEvent_BLE_BondingInformation)
                    EVENTENTRY("+CWMODE", StephanoI_ATEvent_Wifi_Mode) EVENTENTRY("+CWLAP", StephanoI_ATEvent_Wifi_Scan) EVENTENTRY("+CWSTATE", StephanoI_ATEvent_Wifi_State) EVENTENTRY("+CWLIF", StephanoI_ATEvent_Wifi_GetConnectedIPs) EVENTENTRY("+CIPSTA", StephanoI_ATEvent_Wifi_GetStationIP) EVENTENTRY("+CIPAP", StephanoI_ATEvent_Wifi_GetAPIP)
                        EVENTENTRY("+CIPSTAMAC", StephanoI_ATEvent_Wifi_GetStationMAC) EVENTENTRY("+CIPAPMAC", StephanoI_ATEvent_Wifi_GetAPMAC) EVENTENTRY("+DIST_STA_IP", StephanoI_ATEvent_Wifi_APAssignIP) EVENTENTRY("+STA_CONNECTED", StephanoI_ATEvent_Wifi_StationhasConnected) EVENTENTRY("+STA_DISCONNECTED", StephanoI_ATEvent_Wifi_StationhasDisconnected)
                            EVENTENTRY("+CIPSTATE", StephanoI_ATEvent_Socket_State) EVENTENTRY("+LINK_CONN", StephanoI_ATEvent_Socket_ConnectionInformation) EVENTENTRY("+PING", StephanoI_ATEvent_Socket_Ping) EVENTENTRY("+CIPDOMAIN", StephanoI_ATEvent_Socket_DomainResolved) EVENTENTRY("+CIPSENDL", StephanoI_ATEvent_Socket_SendState) EVENTENTRY("+CIPSNTPTIME", StephanoI_ATEvent_SNTP_Time)
                                EVENTENTRY("+CIPRECVDATA", StephanoI_ATEvent_Socket_ReceiveData) EVENTENTRY("+CIPRECVLEN", StephanoI_ATEvent_Socket_ReceiveLen) EVENTENTRY("+CIPMUX", StephanoI_ATEvent_Socket_GetMultiple) EVENTENTRY("+MQTTCONNECTED", StephanoI_ATEvent_MQTT_Connected) EVENTENTRY("+MQTTDISCONNECTED", StephanoI_ATEvent_MQTT_Disconnected)
                                    EVENTENTRY("+MQTTCONN", StephanoI_ATEvent_MQTT_ConnectionInfo) EVENTENTRY("+MQTTSUBRECV", StephanoI_ATEvent_MQTT_SubscriptionReceive) EVENTENTRY("+MQTTSUB", StephanoI_ATEvent_MQTT_Subscriptions) EVENTENTRY("+HTTPCLIENT", StephanoI_ATEvent_HTTP_Client) EVENTENTRY("+HTTPGETSIZE", StephanoI_ATEvent_HTTP_GetSize) EVENTENTRY("+HTTPCGET", StephanoI_ATEvent_HTTP_Get)
                                        EVENTENTRY("+WEBSERVERRSP", StephanoI_ATEvent_Webserver_Response) EVENTENTRY("+SYSMFG", StephanoI_ATEvent_Device_SYSMFG) EVENTENTRY("+SYSTIMESTAMP", StephanoI_ATEvent_Device_SYSTimeStamp) EVENTENTRY("+SYSSTORE", StephanoI_ATEvent_Device_SYSStore) EVENTENTRY("+SYSTEMP", StephanoI_ATEvent_Device_SYSTemp) EVENTENTRY("ERR CODE", StephanoI_ATEvent_ErrorCode)
                                            EVENTENTRY("WIFI DISCONNECT", StephanoI_ATEvent_Wifi_Disconnected) EVENTENTRY("WIFI CONNECTED", StephanoI_ATEvent_Wifi_Connected) EVENTENTRY("WIFI GOT IP", StephanoI_ATEvent_Wifi_IPAquired) EVENTENTRY("WIFI GOT IPv6 LL", StephanoI_ATEvent_Wifi_IPv6LLAquired) EVENTENTRY("WIFI GOT IPv6 GL", StephanoI_ATEvent_Wifi_IPv6GLAquired)
                                                EVENTENTRY("Will force to restart!!!", StephanoI_ATEvent_RestartForced) EVENTENTRY("CONNECT", StephanoI_ATEvent_Socket_Connected) EVENTENTRY("CLOSED", StephanoI_ATEvent_Socket_Closed) EVENTENTRY("+IPD", StephanoI_ATEvent_Socket_Receive) // +IPD,<link ID>,<len> or +IPD,<len>
    PARENTEVENTENTRY("+MQTTPUB", MQTTSubEvents, ATCOMMAND_STRING_TERMINATE) LASTPARENTEVENTENTRY(NULL, WildCardSubEvents, ATCOMMAND_STRING_TERMINATE)                                                                                                                                        // <link ID>,CONNECT or <link ID>,CLOSED
};

/**
 * @brief Parses the received AT command and returns the corresponding StephanoI_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent StephanoI_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATEvent_ParseEventType(char** pAtCommand, StephanoI_ATEvent_t* pEvent)
{
    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE, ATCOMMAND_ARGUMENT_DELIM, '\r'};

    if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*)pEvent))
    {
        *pEvent = StephanoI_ATEvent_Invalid;
        return false;
    }

    return true;
}
