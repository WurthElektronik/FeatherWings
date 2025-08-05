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

#ifndef ADRASTEAI_AT_EVENT_H_INCLUDED
#define ADRASTEAI_AT_EVENT_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT event IDs.
 */
    typedef enum AdrasteaI_ATEvent_t
    {
        AdrasteaI_ATEvent_Invalid = (uint16_t)0,

        AdrasteaI_ATEvent_Ready,

        AdrasteaI_ATEvent_NetService_Operator_Read,
        AdrasteaI_ATEvent_Proprietary_Ping_Result,
        AdrasteaI_ATEvent_Proprietary_Domain_Name_Resolve,
        AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status,
        AdrasteaI_ATEvent_PacketDomain_PDP_Context,
        AdrasteaI_ATEvent_PacketDomain_PDP_Context_State,
        AdrasteaI_ATEvent_GNSS_Satellite_Query,

        AdrasteaI_ATEvent_GNSS_NMEA,
        AdrasteaI_ATEvent_GNSS_Session_Status_Change,
        AdrasteaI_ATEvent_GNSS_Allowed_Status_Change,

        AdrasteaI_ATEvent_GNSS_DataFileSaved,

        AdrasteaI_ATEvent_MQTT_Connection_Confirmation,
        AdrasteaI_ATEvent_MQTT_Disconnection_Confirmation,
        AdrasteaI_ATEvent_MQTT_Subscription_Confirmation,
        AdrasteaI_ATEvent_MQTT_Unsubscription_Confirmation,
        AdrasteaI_ATEvent_MQTT_Publication_Confirmation,
        AdrasteaI_ATEvent_MQTT_Publication_Received,
        AdrasteaI_ATEvent_MQTT_Connection_Failure,

        AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Confirmation,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Disconnection_Confirmation,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Subscription_Confirmation,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Unsubscription_Confirmation,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Confirmation,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Received,
        AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Failure,

        AdrasteaI_ATEvent_HTTP_PUT_Confirmation,
        AdrasteaI_ATEvent_HTTP_POST_Confirmation,
        AdrasteaI_ATEvent_HTTP_DELETE_Confirmation,
        AdrasteaI_ATEvent_HTTP_GET_Receive,
        AdrasteaI_ATEvent_HTTP_Session_Termination,

        AdrasteaI_ATEvent_SMS_Read_Message,
        AdrasteaI_ATEvent_SMS_List_Messages,
        AdrasteaI_ATEvent_SMS_Message_Received,
        AdrasteaI_ATEvent_SMS_Error,

        AdrasteaI_ATEvent_Socket_Data_Received,
        AdrasteaI_ATEvent_Socket_Deactivated_Idle_Timer,
        AdrasteaI_ATEvent_Socket_Terminated_By_Peer,
        AdrasteaI_ATEvent_Socket_New_Socket_Accepted,
        AdrasteaI_ATEvent_Socket_Sockets_Read,

        AdrasteaI_ATEvent_SIM_Subscriber_Number,

        AdrasteaI_ATEvent_NumberOfValues,
        AdrasteaI_ATEvent_Max = UINT16_MAX
    } AdrasteaI_ATEvent_t;

    extern bool AdrasteaI_ATEvent_ParseEventType(char** pAtCommand, AdrasteaI_ATEvent_t* pEvent);

#ifdef __cplusplus
}
#endif

#endif // AT_EVENTS_H_INCLUDED
