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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT event definitions.
 */

#ifndef AT_EVENT_H_INCLUDED
#define AT_EVENT_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define ATEvent_GNSS_NumberOfValues      3
#define ATEvent_MQTT_NumberOfValues      7
#define ATEvent_MQTT_AWSIOT_NumberOfValues      7
#define ATEvent_HTTP_NumberOfValues      5
#define ATEvent_Socket_NumberOfValues      4

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AT event IDs.
 */
typedef enum ATEvent_t
{
	ATEvent_Invalid,
	ATEvent_NetService_Operator_Read,
	ATEvent_Proprietary_Ping_Result,
	ATEvent_Proprietary_Domain_Name_Resolve,
	ATEvent_PacketDomain_Network_Registration_Status,
	ATEvent_PacketDomain_PDP_Context,
	ATEvent_PacketDomain_PDP_Context_State,
	ATEvent_GNSS_Satalite_Query,

	ATEvent_GNSS,
	ATEvent_GNSS_NMEA = ATEvent_GNSS,
	ATEvent_GNSS_Session_Status_Change,
	ATEvent_GNSS_Allowed_Status_Change,

	ATEvent_MQTT,
	ATEvent_MQTT_Connection_Confirmation = ATEvent_MQTT,
	ATEvent_MQTT_Disconnection_Confirmation,
	ATEvent_MQTT_Subscription_Confirmation,
	ATEvent_MQTT_Unsubsrciption_Confimration,
	ATEvent_MQTT_Publication_Confirmation,
	ATEvent_MQTT_Publication_Received,
	ATEvent_MQTT_Connection_Failure,

	ATEvent_MQTT_AWSIOT,
	ATEvent_MQTT_AWSIOT_Connection_Confirmation = ATEvent_MQTT_AWSIOT,
	ATEvent_MQTT_AWSIOT_Disconnection_Confirmation,
	ATEvent_MQTT_AWSIOT_Subscription_Confirmation,
	ATEvent_MQTT_AWSIOT_Unsubsrciption_Confimration,
	ATEvent_MQTT_AWSIOT_Publication_Confirmation,
	ATEvent_MQTT_AWSIOT_Publication_Received,
	ATEvent_MQTT_AWSIOT_Connection_Failure,

	ATEvent_HTTP,
	ATEvent_HTTP_PUT_Confirmation = ATEvent_HTTP,
	ATEvent_HTTP_POST_Confirmation,
	ATEvent_HTTP_DELETE_Confirmation,
	ATEvent_HTTP_GET_Receive,
	ATEvent_HTTP_Session_Termination,

	ATEvent_SMS_Read_Message,
	ATEvent_SMS_List_Messages,
	ATEvent_SMS_Message_Received,
	ATEvent_SMS_Error,

	ATEvent_Socket,
	ATEvent_Socket_Data_Received = ATEvent_Socket,
	ATEvent_Socket_Deactivated_Idle_Timer,
	ATEvent_Socket_Terminated_By_Peer,
	ATEvent_Socket_New_Socket_Accepted,
	ATEvent_Socket_Sockets_Read,

	ATEvent_SIM_Subscriber_Number,

	ATEvent_NumberOfValues
} ATEvent_t;

extern bool ATEvent_ParseEventType(char **pAtCommand, ATEvent_t *pEvent);

extern bool ATEvent_GetEventName(ATEvent_t event, char *pEventName);

#ifdef __cplusplus
}
#endif

#endif // AT_EVENTS_H_INCLUDED
