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
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>

static const ATCommand_Event_t GNSSSubEvents[] = {EVENTENTRY(" \"NMEA\"", AdrasteaI_ATEvent_GNSS_NMEA) EVENTENTRY(" \"SESSIONSTAT\"", AdrasteaI_ATEvent_GNSS_Session_Status_Change) LASTEVENTENTRY(" \"ALLOWSTAT\"", AdrasteaI_ATEvent_GNSS_Allowed_Status_Change)};

static const ATCommand_Event_t MQTTSubEvents[] = {EVENTENTRY("\"CONCONF\"", AdrasteaI_ATEvent_MQTT_Connection_Confirmation) EVENTENTRY("\"DISCONF\"", AdrasteaI_ATEvent_MQTT_Disconnection_Confirmation) EVENTENTRY("\"SUBCONF\"", AdrasteaI_ATEvent_MQTT_Subscription_Confirmation) EVENTENTRY("\"UNSCONF\"", AdrasteaI_ATEvent_MQTT_Unsubscription_Confirmation)
                                                      EVENTENTRY("\"PUBCONF\"", AdrasteaI_ATEvent_MQTT_Publication_Confirmation) EVENTENTRY("\"PUBRCV\"", AdrasteaI_ATEvent_MQTT_Publication_Received) LASTEVENTENTRY("\"CONFAIL\"", AdrasteaI_ATEvent_MQTT_Connection_Failure)};

static const ATCommand_Event_t MQTT_AWSIOTSubEvents[] = {EVENTENTRY("\"CONCONF\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Confirmation) EVENTENTRY("\"DISCONF\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Disconnection_Confirmation) EVENTENTRY("\"SUBCONF\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Subscription_Confirmation) EVENTENTRY("\"UNSCONF\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Unsubscription_Confirmation)
                                                             EVENTENTRY("\"PUBCONF\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Confirmation) EVENTENTRY("\"PUBRCV\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Received) LASTEVENTENTRY("\"CONFAIL\"", AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Failure)};

static const ATCommand_Event_t HTTPSubEvents[] = {EVENTENTRY("\"PUTCONF\"", AdrasteaI_ATEvent_HTTP_PUT_Confirmation) EVENTENTRY("\"POSTCONF\"", AdrasteaI_ATEvent_HTTP_POST_Confirmation) EVENTENTRY("\"DELCONF\"", AdrasteaI_ATEvent_HTTP_DELETE_Confirmation) EVENTENTRY("\"GETRCV\"", AdrasteaI_ATEvent_HTTP_GET_Receive) LASTEVENTENTRY("\"SESTERM\"", AdrasteaI_ATEvent_HTTP_Session_Termination)};

static const ATCommand_Event_t SocketSubEvents[] = {EVENTENTRY("1", AdrasteaI_ATEvent_Socket_Data_Received) EVENTENTRY("2", AdrasteaI_ATEvent_Socket_Deactivated_Idle_Timer) EVENTENTRY("3", AdrasteaI_ATEvent_Socket_Terminated_By_Peer) EVENTENTRY("4", AdrasteaI_ATEvent_Socket_New_Socket_Accepted) LASTEVENTENTRY("SOCKETCMD", AdrasteaI_ATEvent_Socket_Sockets_Read)};

static const ATCommand_Event_t moduleMainEvents[] = {EVENTENTRY("+COPN", AdrasteaI_ATEvent_NetService_Operator_Read) EVENTENTRY("%PINGCMD", AdrasteaI_ATEvent_Proprietary_Ping_Result) EVENTENTRY("%DNSRSLV", AdrasteaI_ATEvent_Proprietary_Domain_Name_Resolve) EVENTENTRY("+CEREG", AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status)
                                                         EVENTENTRY("+CGDCONT", AdrasteaI_ATEvent_PacketDomain_PDP_Context) EVENTENTRY("+CGACT", AdrasteaI_ATEvent_PacketDomain_PDP_Context_State) EVENTENTRY("%IGNSSINFO", AdrasteaI_ATEvent_GNSS_Satellite_Query) EVENTENTRY("+CMGR", AdrasteaI_ATEvent_SMS_Read_Message) EVENTENTRY("+CMGL", AdrasteaI_ATEvent_SMS_List_Messages)
                                                             EVENTENTRY("+CMTI", AdrasteaI_ATEvent_SMS_Message_Received) EVENTENTRY("+CMS ERROR", AdrasteaI_ATEvent_SMS_Error) EVENTENTRY("%SOCKETCMD", AdrasteaI_ATEvent_Socket_Sockets_Read) EVENTENTRY("+CNUM", AdrasteaI_ATEvent_SIM_Subscriber_Number) EVENTENTRY("%SCMNOTIFYEV", AdrasteaI_ATEvent_Ready)
                                                                 EVENTENTRY("%IGNSSCEP", AdrasteaI_ATEvent_GNSS_DataFileSaved) PARENTEVENTENTRY("%IGNSSEVU", GNSSSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("%MQTTEVU", MQTTSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("%AWSIOTEVU", MQTT_AWSIOTSubEvents, ATCOMMAND_ARGUMENT_DELIM)
                                                                     PARENTEVENTENTRY("%HTTPEVU", HTTPSubEvents, ATCOMMAND_ARGUMENT_DELIM) LASTPARENTEVENTENTRY("%SOCKETEV", SocketSubEvents, ATCOMMAND_ARGUMENT_DELIM)};

/**
 * @brief Parses the received AT command and returns the corresponding AdrasteaI_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+' or '%'
 * @param[out] pEvent AdrasteaI_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool AdrasteaI_ATEvent_ParseEventType(char** pAtCommand, AdrasteaI_ATEvent_t* pEvent)
{
    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE};

    if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*)pEvent))
    {
        *pEvent = AdrasteaI_ATEvent_Invalid;
        return false;
    }

    return true;
}
