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
 * @brief AT commands for Socket functionality.
 */

#ifndef ADRASTEAI_AT_SOCKET_H_INCLUDED
#define ADRASTEAI_AT_SOCKET_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Socket Events
 */
    typedef enum AdrasteaI_ATSocket_Event_t
    {
        AdrasteaI_ATSocket_Event_Invalid = -1,
        AdrasteaI_ATSocket_Event_All,
        AdrasteaI_ATSocket_Event_Data_Received,
        AdrasteaI_ATSocket_Event_Deactivated_Idle_Timer,
        AdrasteaI_ATSocket_Event_Terminated_By_Peer,
        AdrasteaI_ATSocket_Event_New_Socket_Accepted,
        AdrasteaI_ATSocket_Event_NumberOfValues
    } AdrasteaI_ATSocket_Event_t;

    typedef uint8_t AdrasteaI_ATSocket_ID_t;

#define AdrasteaI_ATSocket_ID_Invalid 0

    /**
 * @brief Socket States
 */
    typedef enum AdrasteaI_ATSocket_State_t
    {
        AdrasteaI_ATSocket_State_Invalid = -1,
        AdrasteaI_ATSocket_State_Deactivated,
        AdrasteaI_ATSocket_State_Activated,
        AdrasteaI_ATSocket_State_Listening,
        AdrasteaI_ATSocket_State_NumberOfValues
    } AdrasteaI_ATSocket_State_t;

    /**
 * @brief Socket Read Result
 */
    typedef struct AdrasteaI_ATSocket_Read_Result_t
    {
        AdrasteaI_ATSocket_ID_t socketID;
        AdrasteaI_ATSocket_State_t socketState;
    } AdrasteaI_ATSocket_Read_Result_t;

    typedef uint8_t AdrasteaI_ATSocket_Error_Code_t;

    /**
 * @brief Socket Types
 */
    typedef enum AdrasteaI_ATSocket_Type_t
    {
        AdrasteaI_ATSocket_Type_Invalid = -1,
        AdrasteaI_ATSocket_Type_TCP,
        AdrasteaI_ATSocket_Type_UDP,
        AdrasteaI_ATSocket_Type_NumberOfValues
    } AdrasteaI_ATSocket_Type_t;

    /**
 * @brief Socket Direction
 */
    typedef enum AdrasteaI_ATSocket_Direction_t
    {
        AdrasteaI_ATSocket_Direction_Invalid = -1,
        AdrasteaI_ATSocket_Direction_Not_Set,
        AdrasteaI_ATSocket_Direction_Dialer,
        AdrasteaI_ATSocket_Direction_Listener,
        AdrasteaI_ATSocket_Direction_NumberOfValues
    } AdrasteaI_ATSocket_Direction_t;

    typedef uint16_t AdrasteaI_ATSocket_Timeout_t;

#define AdrasteaI_ATSocket_Timeout_Invalid 0

    /**
 * @brief Socket Info
 */
    typedef struct AdrasteaI_ATSocket_Info_t
    {
        AdrasteaI_ATSocket_State_t socketState;
        AdrasteaI_ATSocket_Type_t socketType;
        AdrasteaI_ATCommon_IP_Addr_t sourceIPAddress;
        AdrasteaI_ATCommon_IP_Addr_t destinationIPAddress;
        AdrasteaI_ATCommon_Port_Number_t sourcePortNumber;
        AdrasteaI_ATCommon_Port_Number_t destinationPortNumber;
        AdrasteaI_ATSocket_Direction_t tcpSocketDirection;
        AdrasteaI_ATSocket_Timeout_t socketTimeout;
    } AdrasteaI_ATSocket_Info_t;

    /**
 * @brief Socket Behaviour
 */
    typedef enum AdrasteaI_ATSocket_Behaviour_t
    {
        AdrasteaI_ATSocket_Behaviour_Invalid = -1,
        AdrasteaI_ATSocket_Behaviour_Open_Connection,
        AdrasteaI_ATSocket_Behaviour_Listen,
        AdrasteaI_ATSocket_Behaviour_ListenMultiple,
        AdrasteaI_ATSocket_Behaviour_NumberOfValues
    } AdrasteaI_ATSocket_Behaviour_t;

    typedef uint16_t AdrasteaI_ATSocket_Data_Length_t;

#define AdrasteaI_ATSocket_Data_Length_Automatic 0

    /**
 * @brief Socket Data Read
 */
    typedef struct AdrasteaI_ATSocket_Data_Read_t
    {
        AdrasteaI_ATSocket_ID_t socketID;
        AdrasteaI_ATSocket_Data_Length_t dataLength;
        char* data;
        AdrasteaI_ATSocket_Data_Length_t dataLeftLength;
        AdrasteaI_ATCommon_IP_Addr_t sourceIPAddress;
        AdrasteaI_ATCommon_Port_Number_t sourcePortNumber;
    } AdrasteaI_ATSocket_Data_Read_t;

    /**
 * @brief Socket IP Address Format
 */
    typedef enum AdrasteaI_ATSocket_IP_Addr_Format_t
    {
        AdrasteaI_ATSocket_IP_Addr_Format_Invalid = -1,
        AdrasteaI_ATSocket_IP_Addr_Format_IPv4v6,
        AdrasteaI_ATSocket_IP_Addr_Format_IPv4,
        AdrasteaI_ATSocket_IP_Addr_Format_IPv6,
        AdrasteaI_ATSocket_IP_Addr_Format_NumberOfValues
    } AdrasteaI_ATSocket_IP_Addr_Format_t;

    typedef struct AdrasteaI_ATSocket_SSL_Info_t
    {
        AdrasteaI_ATCommon_SSL_Auth_Mode_t SSLMode;
        AdrasteaI_ATCommon_SSL_Profile_ID_t profileID;
    } AdrasteaI_ATSocket_SSL_Info_t;

    typedef uint16_t AdrasteaI_ATSocket_Aggregation_Time_t;

    typedef uint16_t AdrasteaI_ATSocket_Aggregation_Buffer_Size_t;

    typedef uint16_t AdrasteaI_ATSocket_TCP_Idle_Time_t;

    extern bool AdrasteaI_ATSocket_ReadCreatedSocketsStates();

    extern bool AdrasteaI_ATSocket_AllocateSocket(AdrasteaI_ATCommon_Session_ID_t sessionID, AdrasteaI_ATSocket_Type_t socketType, AdrasteaI_ATSocket_Behaviour_t socketBehaviour, AdrasteaI_ATCommon_IP_Addr_t destinationIPAddress, AdrasteaI_ATCommon_Port_Number_t destinationPortNumber, AdrasteaI_ATCommon_Port_Number_t sourcePortNumber, AdrasteaI_ATSocket_Data_Length_t packetSize,
                                                  AdrasteaI_ATSocket_Timeout_t socketTimeout, AdrasteaI_ATSocket_IP_Addr_Format_t addressFormat, AdrasteaI_ATSocket_ID_t* socketIDP);

    extern bool AdrasteaI_ATSocket_ActivateSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_Session_ID_t SSLSessionID);

    extern bool AdrasteaI_ATSocket_ReadSocketInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Info_t* infoP);

    extern bool AdrasteaI_ATSocket_DeactivateSocket(AdrasteaI_ATSocket_ID_t socketID);

    extern bool AdrasteaI_ATSocket_SetSocketOptions(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Aggregation_Time_t aggregationTime, AdrasteaI_ATSocket_Aggregation_Buffer_Size_t aggregationBufferSize, AdrasteaI_ATSocket_TCP_Idle_Time_t idleTime);

    extern bool AdrasteaI_ATSocket_DeleteSocket(AdrasteaI_ATSocket_ID_t socketID);

    extern bool AdrasteaI_ATSocket_ReadSocketLastError(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Error_Code_t* errorCodeP);

    extern bool AdrasteaI_ATSocket_AddSSLtoSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_SSL_Auth_Mode_t authMode, AdrasteaI_ATCommon_SSL_Profile_ID_t profileID);

    extern bool AdrasteaI_ATSocket_ReadSocketSSLInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_SSL_Info_t* infoP);

    extern bool AdrasteaI_ATSocket_KeepSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID);

    extern bool AdrasteaI_ATSocket_DeleteSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID);

    extern bool AdrasteaI_ATSocket_ReceiveFromSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Data_Read_t* dataReadP, uint16_t maxBufferLength);

    extern bool AdrasteaI_ATSocket_SendToSocket(AdrasteaI_ATSocket_ID_t socketID, char* data, AdrasteaI_ATSocket_Data_Length_t dataLength);

    extern bool AdrasteaI_ATSocket_SetSocketUnsolicitedNotificationEvents(AdrasteaI_ATSocket_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

    extern bool AdrasteaI_ATSocket_ParseDataReceivedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP);

    extern bool AdrasteaI_ATSocket_ParseSocketTerminatedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP);

    extern bool AdrasteaI_ATSocket_ParseSocketsReadEvent(char* pEventArguments, AdrasteaI_ATSocket_Read_Result_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_SOCKET_H_INCLUDED */
