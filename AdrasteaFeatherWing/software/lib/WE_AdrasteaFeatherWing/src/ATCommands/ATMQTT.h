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
 * @brief AT commands for MQTT functionality.
 */

#ifndef AT_MQTT_H_INCLUDED
#define AT_MQTT_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MQTT Connection IDs
 */
typedef enum ATMQTT_Conn_ID_t
{
	ATMQTT_Conn_ID_Invalid = -1,
	ATMQTT_Conn_ID_Single_Connectivity_Mode,
	ATMQTT_Conn_ID_1,
	ATMQTT_Conn_ID_2,
	ATMQTT_Conn_ID_3,
	ATMQTT_Conn_ID_4,
	ATMQTT_Conn_ID_5,
	ATMQTT_Conn_ID_NumberOfValues
} ATMQTT_Conn_ID_t;

typedef char ATMQTT_Client_ID_t[64];

typedef enum ATMQTT_IP_Session_ID_t
{
	ATMQTT_IP_Session_ID_Invalid = -1,
	ATMQTT_IP_Session_ID_Default_Data_PDN,
	ATMQTT_IP_Session_ID_Max_Value_in_NP_Config,
	ATMQTT_IP_Session_ID_NumberOfValues
} ATMQTT_IP_Session_ID_t;

/**
 * @brief MQTT Will Message Presence
 */
typedef enum ATMQTT_WILL_Presence_t
{
	ATMQTT_WILL_Presence_Invalid = -1,
	ATMQTT_WILL_Presence_Disable,
	ATMQTT_WILL_Presence_Enable,
	ATMQTT_WILL_Presence_NumberOfValues
} ATMQTT_WILL_Presence_t;

/**
 * @brief MQTT Quality of Service
 */
typedef enum ATMQTT_QoS_t
{
	ATMQTT_QoS_Invalid = -1,
	ATMQTT_QoS_At_Most_Once,
	ATMQTT_QoS_At_Least_Once,
	ATMQTT_QoS_Exactly_Once,
	ATMQTT_QoS_NumberOfValues
} ATMQTT_QoS_t;

/**
 * @brief MQTT Retain after disconnect
 */
typedef enum ATMQTT_Retain_t
{
	ATMQTT_Retain_Invalid = -1,
	ATMQTT_Retain_Not_Retained,
	ATMQTT_Retain_Retained,
	ATMQTT_Retain_NumberOfValues
} ATMQTT_Retain_t;

typedef uint16_t ATMQTT_Keep_Alive_t;

#define ATMQTT_Keep_Alive_No_Timeout 0

/**
 * @brief MQTT Clean Session
 */
typedef enum ATMQTT_Clean_Session_t
{
	ATMQTT_Clean_Session_Invalid = -1,
	ATMQTT_Clean_Session_Store_Subscriptions_After_Disconnect,
	ATMQTT_Clean_Session_Discard_After_Disconnect,
	ATMQTT_Clean_Session_NumberOfValues
} ATMQTT_Clean_Session_t;

/**
 * @brief MQTT Events
 */
typedef enum ATMQTT_Event_t
{
	ATMQTT_Event_Invalid = -1,
	ATMQTT_Event_Connect_Confirmation,
	ATMQTT_Event_Disconnect_Confirmation,
	ATMQTT_Event_Subscribe_Confirmation,
	ATMQTT_Event_Unsubscribe_Confirmation,
	ATMQTT_Event_Publication_Confirmation,
	ATMQTT_Event_Publication_Receive,
	ATMQTT_Event_Connect_Failure,
	ATMQTT_Event_All,
	ATMQTT_Event_NumberOfValues
} ATMQTT_Event_t;

/**
 * @brief MQTT Event Result Codes
 */
typedef enum ATMQTT_Event_Result_Code_t
{
	ATMQTT_Event_Result_Code_Invalid = -1,
	ATMQTT_Event_Result_Code_Success,
	ATMQTT_Event_Result_Code_Fail,
	ATMQTT_Event_Result_Code_NumberOfValues
} ATMQTT_Event_Result_Code_t;

/**
 * @brief MQTT Event Error Codes
 */
typedef enum ATMQTT_Event_Error_Code_t
{
	ATMQTT_Event_Error_Code_Invalid = -1,
	ATMQTT_Event_Error_Code_No_Error,
	ATMQTT_Event_Error_Code_Error,
	ATMQTT_Event_Error_Code_NumberOfValues
} ATMQTT_Event_Error_Code_t;

/**
 * @brief MQTT Connection Result
 */
typedef struct ATMQTT_Connection_Result_t
{
	ATMQTT_Conn_ID_t connID;
	ATMQTT_Event_Result_Code_t resultCode;
	ATMQTT_Event_Error_Code_t errorCode;
} ATMQTT_Connection_Result_t;

typedef uint16_t ATMQTT_Message_ID_t;

/**
 * @brief MQTT Subscription/Publication Confirmation Result
 */
typedef struct
{
	ATMQTT_Conn_ID_t connID;
	ATMQTT_Message_ID_t msgID;
	ATMQTT_Event_Result_Code_t resultCode;
	ATMQTT_Event_Error_Code_t errorCode;
} ATMQTT_Subscription_Result_t, ATMQTT_Publication_Confirmation_Result_t;

typedef char ATMQTT_Topic_Name_t[128];

typedef uint8_t ATMQTT_Payload_Size_t;

/**
 * @brief MQTT Publication Received Result
 */
typedef struct ATMQTT_Publication_Received_Result_t
{
	ATMQTT_Conn_ID_t connID;
	ATMQTT_Message_ID_t msgID;
	ATMQTT_Topic_Name_t topicName;
	ATMQTT_Payload_Size_t payloadSize;
	char *payload;
	uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
} ATMQTT_Publication_Received_Result_t;

/**
 * @brief MQTT IP Address Format
 */
typedef enum ATMQTT_IP_Addr_Format_t
{
	ATMQTT_IP_Addr_Format_Invalid = -1,
	ATMQTT_IP_Addr_Format_IPv4v6,
	ATMQTT_IP_Addr_Format_IPv4,
	ATMQTT_IP_Addr_Format_IPv6,
	ATMQTT_IP_Addr_Format_NumberOfValues
} ATMQTT_IP_Addr_Format_t;

/**
 * @brief AWSIOT Quality of Service
 */
typedef enum ATMQTT_AWSIOT_QoS_t
{
	ATMQTT_AWSIOT_QoS_Invalid = -1,
	ATMQTT_AWSIOT_QoS_No_Confirmation,
	ATMQTT_AWSIOT_QoS_Confirmed,
	ATMQTT_AWSIOT_QoS_NumberOfValues
} ATMQTT_AWSIOT_QoS_t;

/**
 * @brief AWSIOT Subscription/Publication Confirmation Result
 */
typedef struct
{
	ATMQTT_Message_ID_t msgID;
	ATMQTT_Event_Result_Code_t resultCode;
} ATMQTT_AWSIOT_Subscription_Result_t, ATMQTT_AWSIOT_Publication_Confirmation_Result_t;

/**
 * @brief AWSIOT Publication Received Result
 */
typedef struct ATMQTT_AWSIOT_Publication_Received_Result_t
{
	ATMQTT_Topic_Name_t topicName;
	char *payload;
	uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
} ATMQTT_AWSIOT_Publication_Received_Result_t;

extern bool ATMQTT_ConfigureNodes(ATMQTT_Conn_ID_t connID, ATMQTT_Client_ID_t clientID, ATCommon_IP_Addr_t addr, ATCommon_Auth_Username_t username, ATCommon_Auth_Password_t password);

extern bool ATMQTT_ConfigureTLS(ATMQTT_Conn_ID_t connID, ATCommon_TLS_Auth_Mode_t authMode, ATCommon_TLS_Profile_ID_t profileID);

extern bool ATMQTT_ConfigureIP(ATMQTT_Conn_ID_t connID, ATMQTT_IP_Session_ID_t sessionID, ATMQTT_IP_Addr_Format_t ipFormat, ATCommon_Port_Number_t port);

extern bool ATMQTT_ConfigureWillMessage(ATMQTT_Conn_ID_t connID, ATMQTT_WILL_Presence_t presence, ATMQTT_QoS_t qos, ATMQTT_Retain_t retain, ATMQTT_Topic_Name_t topic, char *message);

extern bool ATMQTT_ConfigureProtocol(ATMQTT_Conn_ID_t connID, ATMQTT_Keep_Alive_t keepAlive, ATMQTT_Clean_Session_t cleanSession);

extern bool ATMQTT_SetMQTTUnsolicitedNotificationEvents(ATMQTT_Event_t event, ATCommon_Event_State_t state);

extern bool ATMQTT_ParseConnectionConfirmationEvent(char *pEventArguments, ATMQTT_Connection_Result_t *dataP);

extern bool ATMQTT_ParseDisconnectionConfirmationEvent(char *pEventArguments, ATMQTT_Connection_Result_t *dataP);

extern bool ATMQTT_ParseSubscriptionConfirmationEvent(char *pEventArguments, ATMQTT_Subscription_Result_t *dataP);

extern bool ATMQTT_ParseUnsubscriptionConfirmationEvent(char *pEventArguments, ATMQTT_Subscription_Result_t *dataP);

extern bool ATMQTT_ParsePublicationConfirmationEvent(char *pEventArguments, ATMQTT_Publication_Confirmation_Result_t *dataP);

extern bool ATMQTT_ParsePublicationReceivedEvent(char *pEventArguments, ATMQTT_Publication_Received_Result_t *dataP);

extern bool ATMQTT_Connect(ATMQTT_Conn_ID_t connID);

extern bool ATMQTT_Disconnect(ATMQTT_Conn_ID_t connID);

extern bool ATMQTT_Subscribe(ATMQTT_Conn_ID_t connID, ATMQTT_QoS_t QoS, ATMQTT_Topic_Name_t topicName);

extern bool ATMQTT_Unsubscribe(ATMQTT_Conn_ID_t connID, ATMQTT_Topic_Name_t topicName);

extern bool ATMQTT_Publish(ATMQTT_Conn_ID_t connID, ATMQTT_QoS_t qos, ATMQTT_Retain_t retain, ATMQTT_Topic_Name_t topicName, char *payload, ATMQTT_Payload_Size_t payloadSize);

extern bool ATMQTT_AWSIOTConfigureConnection(ATCommon_IP_Addr_t url, ATCommon_TLS_Profile_ID_t profileID, ATMQTT_Client_ID_t clientID);

extern bool ATMQTT_AWSIOTConfigureIP(ATMQTT_IP_Session_ID_t sessionID, ATMQTT_IP_Addr_Format_t ipFormat);

extern bool ATMQTT_AWSIOTConfigureProtocol(ATMQTT_Keep_Alive_t keepAlive, ATMQTT_AWSIOT_QoS_t qos);

extern bool ATMQTT_SetAWSIOTUnsolicitedNotificationEvents(ATMQTT_Event_t event, ATCommon_Event_State_t state);

extern bool ATMQTT_ParseAWSIOTConnectionConfirmationEvent(char *pEventArguments, ATMQTT_Event_Result_Code_t *dataP);

extern bool ATMQTT_ParseAWSIOTDisconnectionConfirmationEvent(char *pEventArguments, ATMQTT_Event_Result_Code_t *dataP);

extern bool ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(char *pEventArguments, ATMQTT_AWSIOT_Subscription_Result_t *dataP);

extern bool ATMQTT_ParseAWSIOTUnsubscriptionConfirmationEvent(char *pEventArguments, ATMQTT_AWSIOT_Subscription_Result_t *dataP);

extern bool ATMQTT_ParseAWSIOTPublicationConfirmationEvent(char *pEventArguments, ATMQTT_AWSIOT_Publication_Confirmation_Result_t *dataP);

extern bool ATMQTT_ParseAWSIOTPublicationReceivedEvent(char *pEventArguments, ATMQTT_AWSIOT_Publication_Received_Result_t *dataP);

extern bool ATMQTT_AWSIOTConnect();

extern bool ATMQTT_AWSIOTDisconnect();

extern bool ATMQTT_AWSIOTSubscribe(ATMQTT_Topic_Name_t topicName);

extern bool ATMQTT_AWSIOTUnsubscribe(ATMQTT_Topic_Name_t topicName);

extern bool ATMQTT_AWSIOTPublish(ATMQTT_Topic_Name_t topicName, char *payload);

#ifdef __cplusplus
}
#endif

#endif /* AT_MQTT_H_INCLUDED */
