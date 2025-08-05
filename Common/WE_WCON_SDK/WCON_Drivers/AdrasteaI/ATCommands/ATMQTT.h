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
 * @brief AT commands for MQTT functionality.
 */

#ifndef ADRASTEAI_AT_MQTT_H_INCLUDED
#define ADRASTEAI_AT_MQTT_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief MQTT Connection IDs
 */
    typedef enum AdrasteaI_ATMQTT_Conn_ID_t
    {
        AdrasteaI_ATMQTT_Conn_ID_Invalid = -1,
        AdrasteaI_ATMQTT_Conn_ID_Single_Connectivity_Mode,
        AdrasteaI_ATMQTT_Conn_ID_1,
        AdrasteaI_ATMQTT_Conn_ID_2,
        AdrasteaI_ATMQTT_Conn_ID_3,
        AdrasteaI_ATMQTT_Conn_ID_4,
        AdrasteaI_ATMQTT_Conn_ID_5,
        AdrasteaI_ATMQTT_Conn_ID_NumberOfValues
    } AdrasteaI_ATMQTT_Conn_ID_t;

    typedef char AdrasteaI_ATMQTT_Client_ID_t[64];

    typedef enum AdrasteaI_ATMQTT_IP_Session_ID_t
    {
        AdrasteaI_ATMQTT_IP_Session_ID_Invalid = -1,
        AdrasteaI_ATMQTT_IP_Session_ID_Default_Data_PDN,
        AdrasteaI_ATMQTT_IP_Session_ID_Max_Value_in_NP_Config,
        AdrasteaI_ATMQTT_IP_Session_ID_NumberOfValues
    } AdrasteaI_ATMQTT_IP_Session_ID_t;

    /**
 * @brief MQTT Will Message Presence
 */
    typedef enum AdrasteaI_ATMQTT_WILL_Presence_t
    {
        AdrasteaI_ATMQTT_WILL_Presence_Invalid = -1,
        AdrasteaI_ATMQTT_WILL_Presence_Disable,
        AdrasteaI_ATMQTT_WILL_Presence_Enable,
        AdrasteaI_ATMQTT_WILL_Presence_NumberOfValues
    } AdrasteaI_ATMQTT_WILL_Presence_t;

    /**
 * @brief MQTT Quality of Service
 */
    typedef enum AdrasteaI_ATMQTT_QoS_t
    {
        AdrasteaI_ATMQTT_QoS_Invalid = -1,
        AdrasteaI_ATMQTT_QoS_At_Most_Once,
        AdrasteaI_ATMQTT_QoS_At_Least_Once,
        AdrasteaI_ATMQTT_QoS_Exactly_Once,
        AdrasteaI_ATMQTT_QoS_NumberOfValues
    } AdrasteaI_ATMQTT_QoS_t;

    /**
 * @brief MQTT Retain after disconnect
 */
    typedef enum AdrasteaI_ATMQTT_Retain_t
    {
        AdrasteaI_ATMQTT_Retain_Invalid = -1,
        AdrasteaI_ATMQTT_Retain_Not_Retained,
        AdrasteaI_ATMQTT_Retain_Retained,
        AdrasteaI_ATMQTT_Retain_NumberOfValues
    } AdrasteaI_ATMQTT_Retain_t;

    typedef uint16_t AdrasteaI_ATMQTT_Keep_Alive_t;

#define AdrasteaI_ATMQTT_Keep_Alive_No_Timeout 0

    /**
 * @brief MQTT Clean Session
 */
    typedef enum AdrasteaI_ATMQTT_Clean_Session_t
    {
        AdrasteaI_ATMQTT_Clean_Session_Invalid = -1,
        AdrasteaI_ATMQTT_Clean_Session_Store_Subscriptions_After_Disconnect,
        AdrasteaI_ATMQTT_Clean_Session_Discard_After_Disconnect,
        AdrasteaI_ATMQTT_Clean_Session_NumberOfValues
    } AdrasteaI_ATMQTT_Clean_Session_t;

    /**
 * @brief MQTT Events
 */
    typedef enum AdrasteaI_ATMQTT_Event_t
    {
        AdrasteaI_ATMQTT_Event_Invalid = -1,
        AdrasteaI_ATMQTT_Event_Connect_Confirmation,
        AdrasteaI_ATMQTT_Event_Disconnect_Confirmation,
        AdrasteaI_ATMQTT_Event_Subscribe_Confirmation,
        AdrasteaI_ATMQTT_Event_Unsubscribe_Confirmation,
        AdrasteaI_ATMQTT_Event_Publication_Confirmation,
        AdrasteaI_ATMQTT_Event_Publication_Receive,
        AdrasteaI_ATMQTT_Event_Connect_Failure,
        AdrasteaI_ATMQTT_Event_All,
        AdrasteaI_ATMQTT_Event_NumberOfValues
    } AdrasteaI_ATMQTT_Event_t;

    /**
 * @brief MQTT Event Result Codes
 */
    typedef enum AdrasteaI_ATMQTT_Event_Result_Code_t
    {
        AdrasteaI_ATMQTT_Event_Result_Code_Invalid = -1,
        AdrasteaI_ATMQTT_Event_Result_Code_Success,
        AdrasteaI_ATMQTT_Event_Result_Code_Fail,
        AdrasteaI_ATMQTT_Event_Result_Code_NumberOfValues
    } AdrasteaI_ATMQTT_Event_Result_Code_t;

    /**
 * @brief MQTT Event Error Codes
 */
    typedef enum AdrasteaI_ATMQTT_Event_Error_Code_t
    {
        AdrasteaI_ATMQTT_Event_Error_Code_Invalid = -1,
        AdrasteaI_ATMQTT_Event_Error_Code_No_Error,
        AdrasteaI_ATMQTT_Event_Error_Code_Error,
        AdrasteaI_ATMQTT_Event_Error_Code_NumberOfValues
    } AdrasteaI_ATMQTT_Event_Error_Code_t;

    /**
 * @brief MQTT Connection Result
 */
    typedef struct AdrasteaI_ATMQTT_Connection_Result_t
    {
        AdrasteaI_ATMQTT_Conn_ID_t connID;
        AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
        AdrasteaI_ATMQTT_Event_Error_Code_t errorCode;
    } AdrasteaI_ATMQTT_Connection_Result_t;

    typedef uint16_t AdrasteaI_ATMQTT_Message_ID_t;

    /**
 * @brief MQTT Subscription/Publication Confirmation Result
 */
    typedef struct
    {
        AdrasteaI_ATMQTT_Conn_ID_t connID;
        AdrasteaI_ATMQTT_Message_ID_t msgID;
        AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
        AdrasteaI_ATMQTT_Event_Error_Code_t errorCode;
    } AdrasteaI_ATMQTT_Subscription_Result_t, AdrasteaI_ATMQTT_Publication_Confirmation_Result_t;

    typedef char AdrasteaI_ATMQTT_Topic_Name_t[128];

    typedef uint8_t AdrasteaI_ATMQTT_Payload_Size_t;

    /**
 * @brief MQTT Publication Received Result
 */
    typedef struct AdrasteaI_ATMQTT_Publication_Received_Result_t
    {
        AdrasteaI_ATMQTT_Conn_ID_t connID;
        AdrasteaI_ATMQTT_Message_ID_t msgID;
        AdrasteaI_ATMQTT_Topic_Name_t topicName;
        AdrasteaI_ATMQTT_Payload_Size_t payloadSize;
        char* payload;
        uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
    } AdrasteaI_ATMQTT_Publication_Received_Result_t;

    /**
 * @brief MQTT IP Address Format
 */
    typedef enum AdrasteaI_ATMQTT_IP_Addr_Format_t
    {
        AdrasteaI_ATMQTT_IP_Addr_Format_Invalid = -1,
        AdrasteaI_ATMQTT_IP_Addr_Format_IPv4v6,
        AdrasteaI_ATMQTT_IP_Addr_Format_IPv4,
        AdrasteaI_ATMQTT_IP_Addr_Format_IPv6,
        AdrasteaI_ATMQTT_IP_Addr_Format_NumberOfValues
    } AdrasteaI_ATMQTT_IP_Addr_Format_t;

    /**
 * @brief AWSIOT Quality of Service
 */
    typedef enum AdrasteaI_ATMQTT_AWSIOT_QoS_t
    {
        AdrasteaI_ATMQTT_AWSIOT_QoS_Invalid = -1,
        AdrasteaI_ATMQTT_AWSIOT_QoS_No_Confirmation,
        AdrasteaI_ATMQTT_AWSIOT_QoS_Confirmed,
        AdrasteaI_ATMQTT_AWSIOT_QoS_NumberOfValues
    } AdrasteaI_ATMQTT_AWSIOT_QoS_t;

    /**
 * @brief AWSIOT Subscription/Publication Confirmation Result
 */
    typedef struct
    {
        AdrasteaI_ATMQTT_Message_ID_t msgID;
        AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
    } AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t, AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t;

    /**
 * @brief AWSIOT Publication Received Result
 */
    typedef struct AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t
    {
        AdrasteaI_ATMQTT_Topic_Name_t topicName;
        char* payload;
        uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
    } AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t;

    extern bool AdrasteaI_ATMQTT_ConfigureNodes(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Client_ID_t clientID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATCommon_Auth_Username_t username, AdrasteaI_ATCommon_Auth_Password_t password);

    extern bool AdrasteaI_ATMQTT_ConfigureTLS(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATCommon_TLS_Auth_Mode_t authMode, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID);

    extern bool AdrasteaI_ATMQTT_ConfigureIP(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat, AdrasteaI_ATCommon_Port_Number_t port);

    extern bool AdrasteaI_ATMQTT_ConfigureWillMessage(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_WILL_Presence_t presence, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topic, char* message);

    extern bool AdrasteaI_ATMQTT_ConfigureProtocol(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_Clean_Session_t cleanSession);

    extern bool AdrasteaI_ATMQTT_SetMQTTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

    extern bool AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParsePublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Confirmation_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParsePublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Received_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_Connect(AdrasteaI_ATMQTT_Conn_ID_t connID);

    extern bool AdrasteaI_ATMQTT_Disconnect(AdrasteaI_ATMQTT_Conn_ID_t connID);

    extern bool AdrasteaI_ATMQTT_Subscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t QoS, AdrasteaI_ATMQTT_Topic_Name_t topicName);

    extern bool AdrasteaI_ATMQTT_Unsubscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Topic_Name_t topicName);

    extern bool AdrasteaI_ATMQTT_Publish(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload, AdrasteaI_ATMQTT_Payload_Size_t payloadSize);

    extern bool AdrasteaI_ATMQTT_AWSIOTConfigureConnection(AdrasteaI_ATCommon_IP_Addr_t url, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID, AdrasteaI_ATMQTT_Client_ID_t clientID);

    extern bool AdrasteaI_ATMQTT_AWSIOTConfigureIP(AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat);

    extern bool AdrasteaI_ATMQTT_AWSIOTConfigureProtocol(AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_AWSIOT_QoS_t qos);

    extern bool AdrasteaI_ATMQTT_SetAWSIOTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t* dataP);

    extern bool AdrasteaI_ATMQTT_AWSIOTConnect();

    extern bool AdrasteaI_ATMQTT_AWSIOTDisconnect();

    extern bool AdrasteaI_ATMQTT_AWSIOTSubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName);

    extern bool AdrasteaI_ATMQTT_AWSIOTUnsubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName);

    extern bool AdrasteaI_ATMQTT_AWSIOTPublish(AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_MQTT_H_INCLUDED */
