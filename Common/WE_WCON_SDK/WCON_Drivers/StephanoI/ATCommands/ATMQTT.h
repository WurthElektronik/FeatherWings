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

#ifndef STEPHANOI_MQTT_H_INCLUDED
#define STEPHANOI_MQTT_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MQTT_TOPIC_LENGTH 128

    /**
 * @brief MQTT scheme
 */
    typedef enum StephanoI_ATMQTT_Scheme_t
    {
        StephanoI_ATMQTT_Scheme_TCP = 1,
        StephanoI_ATMQTT_Scheme_TLS_nocert = 2,
        StephanoI_ATMQTT_Scheme_TLS_verifyserver = 3,
        StephanoI_ATMQTT_Scheme_TLS_provideclient = 4,
        StephanoI_ATMQTT_Scheme_TLS_verifyserverNprovideclient = 5,
        StephanoI_ATMQTT_Scheme_WebSocket = 6,
        StephanoI_ATMQTT_Scheme_SecureWebSocket_nocert = 7,
        StephanoI_ATMQTT_Scheme_SecureWebSocket_verifyserver = 8,
        StephanoI_ATMQTT_Scheme_SecureWebSocket_provideclient = 9,
        StephanoI_ATMQTT_Scheme_SecureWebSocket_verifyserverNprovideclient = 10,
    } StephanoI_ATMQTT_Scheme_t;

    /**
 * @brief MQTT state
 */
    typedef enum StephanoI_ATMQTT_State_t
    {
        StephanoI_ATMQTT_State_Uninitialized = 0,
        StephanoI_ATMQTT_State_AlreadySet1 = 1,
        StephanoI_ATMQTT_State_AlreadySet2 = 2,
        StephanoI_ATMQTT_State_Disconnected = 3,
        StephanoI_ATMQTT_State_Connected = 4,
        StephanoI_ATMQTT_State_Unsubscribed = 5,
        StephanoI_ATMQTT_State_ConnectedNSubscribed = 6,
    } StephanoI_ATMQTT_State_t;

    /**
 * @brief MQTT quality of service
 */
    typedef enum StephanoI_ATMQTT_QOS_t
    {
        StephanoI_ATMQTT_QOS_0 = 0,
        StephanoI_ATMQTT_QOS_1 = 1,
        StephanoI_ATMQTT_QOS_2 = 2,
    } StephanoI_ATMQTT_QOS_t;

    /**
 * @brief MQTT connection info event
 */
    typedef struct StephanoI_ATMQTT_ConnectionInfo_t
    {
        uint8_t link_ID;
        StephanoI_ATMQTT_State_t state;
        StephanoI_ATMQTT_Scheme_t scheme;
        char host[128];
        uint16_t port;
        char path[128];
        uint8_t reconnect;
    } StephanoI_ATMQTT_ConnectionInfo_t;

    /**
 * @brief MQTT get subscriptions event
 */
    typedef struct StephanoI_ATMQTT_GetSubscriptions_t
    {
        uint8_t link_ID;
        StephanoI_ATMQTT_State_t state;
        char topic[MQTT_TOPIC_LENGTH];
        StephanoI_ATMQTT_QOS_t qos;
    } StephanoI_ATMQTT_GetSubscriptions_t;

    /**
 * @brief MQTT receive subscriptions event
 */
    typedef struct StephanoI_ATMQTT_ReceiveSubscriptions_t
    {
        uint8_t link_ID;
        char topic[MQTT_TOPIC_LENGTH];
        uint16_t length;
        uint8_t* data;
    } StephanoI_ATMQTT_ReceiveSubscriptions_t;

    extern bool StephanoI_ATMQTT_Userconfig(uint8_t link_ID, StephanoI_ATMQTT_Scheme_t scheme, char* client_ID, char* username, char* password, uint8_t cert_key_ID, uint8_t CA_ID, char* path);
    extern bool StephanoI_ATMQTT_SetClientID(uint8_t link_ID, char* client_ID);
    extern bool StephanoI_ATMQTT_SetUsername(uint8_t link_ID, char* username);
    extern bool StephanoI_ATMQTT_SetPassword(uint8_t link_ID, char* password);
    extern bool StephanoI_ATMQTT_ConnectionConfig(uint8_t link_ID, uint16_t keep_alive, bool disable_clean_session, char* lwt_topics, char* lwt_msg, StephanoI_ATMQTT_QOS_t qos, bool lwt_retain);
    extern bool StephanoI_ATMQTT_Connect(uint8_t link_ID, char* host, uint16_t port, bool reconnect);
    extern bool StephanoI_ATMQTT_GetConnectionInfo();
    extern bool StephanoI_ATMQTT_Publish(uint8_t link_ID, char* topic, char* data, StephanoI_ATMQTT_QOS_t qos, uint8_t retain);
    extern bool StephanoI_ATMQTT_Subscribe(uint8_t link_ID, char* topic, StephanoI_ATMQTT_QOS_t qos);
    extern bool StephanoI_ATMQTT_GetSubscriptions();
    extern bool StephanoI_ATMQTT_Unsubscribe(uint8_t link_ID, char* topic);
    extern bool StephanoI_ATMQTT_Clean(uint8_t link_ID);

    extern bool StephanoI_ATMQTT_ParseConnectionInfo(char* EventArgumentsP, StephanoI_ATMQTT_ConnectionInfo_t* t);
    extern bool StephanoI_ATMQTT_ParseGetSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_GetSubscriptions_t* t);
    extern bool StephanoI_ATMQTT_ParseReceiveSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_ReceiveSubscriptions_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_MQTT_H_INCLUDED */
