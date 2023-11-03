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
 * @brief AT commands for Socket functionality.
 */

#ifndef AT_SOCKET_H_INCLUDED
#define AT_SOCKET_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Socket Events
 */
typedef enum ATSocket_Event_t
{
	ATSocket_Event_Invalid = -1,
	ATSocket_Event_All,
	ATSocket_Event_Data_Received,
	ATSocket_Event_Deactivated_Idle_Timer,
	ATSocket_Event_Terminated_By_Peer,
	ATSocket_Event_New_Socket_Accepted,
	ATSocket_Event_NumberOfValues
} ATSocket_Event_t;

typedef uint8_t ATSocket_ID_t;

#define ATSocket_ID_Invalid 0

/**
 * @brief Socket States
 */
typedef enum ATSocket_State_t
{
	ATSocket_State_Invalid = -1,
	ATSocket_State_Deactivated,
	ATSocket_State_Activated,
	ATSocket_State_Listening,
	ATSocket_State_NumberOfValues
} ATSocket_State_t;

/**
 * @brief Socket Read Result
 */
typedef struct ATSocket_Read_Result_t
{
	ATSocket_ID_t socketID;
	ATSocket_State_t socketState;
} ATSocket_Read_Result_t;

typedef uint8_t ATSocket_Error_Code_t;

/**
 * @brief Socket Types
 */
typedef enum ATSocket_Type_t
{
	ATSocket_Type_Invalid = -1,
	ATSocket_Type_TCP,
	ATSocket_Type_UDP,
	ATSocket_Type_NumberOfValues
} ATSocket_Type_t;

/**
 * @brief Socket Direction
 */
typedef enum ATSocket_Direction_t
{
	ATSocket_Direction_Invalid = -1,
	ATSocket_Direction_Not_Set,
	ATSocket_Direction_Dialer,
	ATSocket_Direction_Listener,
	ATSocket_Direction_NumberOfValues
} ATSocket_Direction_t;

typedef uint16_t ATSocket_Timeout_t;

#define ATSocket_Timeout_Invalid 0

/**
 * @brief Socket Info
 */
typedef struct ATSocket_Info_t
{
	ATSocket_State_t socketState;
	ATSocket_Type_t socketType;
	ATCommon_IP_Addr_t sourceIPAddress;
	ATCommon_IP_Addr_t destinationIPAddress;
	ATCommon_Port_Number_t sourcePortNumber;
	ATCommon_Port_Number_t destinationPortNumber;
	ATSocket_Direction_t tcpSocketDirection;
	ATSocket_Timeout_t socketTimeout;
} ATSocket_Info_t;

/**
 * @brief Socket Behaviour
 */
typedef enum ATSocket_Behaviour_t
{
	ATSocket_Behaviour_Invalid = -1,
	ATSocket_Behaviour_Open_Connection,
	ATSocket_Behaviour_Listen,
	ATSocket_Behaviour_ListenMultiple,
	ATSocket_Behaviour_NumberOfValues
} ATSocket_Behaviour_t;

typedef uint16_t ATSocket_Data_Length_t;

#define ATSocket_Data_Length_Automatic 0

/**
 * @brief Socket Data Read
 */
typedef struct ATSocket_Data_Read_t
{
	ATSocket_ID_t socketID;
	ATSocket_Data_Length_t dataLength;
	char *data;
	ATSocket_Data_Length_t dataLeftLength;
	ATCommon_IP_Addr_t sourceIPAddress;
	ATCommon_Port_Number_t sourcePortNumber;
} ATSocket_Data_Read_t;

/**
 * @brief Socket IP Address Format
 */
typedef enum ATSocket_IP_Addr_Format_t
{
	ATSocket_IP_Addr_Format_Invalid = -1,
	ATSocket_IP_Addr_Format_IPv4v6,
	ATSocket_IP_Addr_Format_IPv4,
	ATSocket_IP_Addr_Format_IPv6,
	ATSocket_IP_Addr_Format_NumberOfValues
} ATSocket_IP_Addr_Format_t;

typedef struct ATSocket_SSL_Info_t
{
	ATCommon_SSL_Auth_Mode_t SSLMode;
	ATCommon_SSL_Profile_ID_t profileID;
} ATSocket_SSL_Info_t;

typedef uint16_t ATSocket_Aggregation_Time_t;

typedef uint16_t ATSocket_Aggregation_Buffer_Size_t;

typedef uint16_t ATSocket_TCP_Idle_Time_t;

extern bool ATSocket_ReadCreatedSocketsStates();

extern bool ATSocket_AllocateSocket(ATCommon_Session_ID_t sessionID, ATSocket_Type_t socketType, ATSocket_Behaviour_t socketBehaviour, ATCommon_IP_Addr_t destinationIPAddress, ATCommon_Port_Number_t destinationPortNumber, ATCommon_Port_Number_t sourcePortNumber, ATSocket_Data_Length_t packetSize, ATSocket_Timeout_t socketTimeout, ATSocket_IP_Addr_Format_t addressFormat, ATSocket_ID_t *socketIDP);

extern bool ATSocket_ActivateSocket(ATSocket_ID_t socketID, ATCommon_Session_ID_t SSLSessionID);

extern bool ATSocket_ReadSocketInfo(ATSocket_ID_t socketID, ATSocket_Info_t *infoP);

extern bool ATSocket_DeactivateSocket(ATSocket_ID_t socketID);

extern bool ATSocket_SetSocketOptions(ATSocket_ID_t socketID, ATSocket_Aggregation_Time_t aggregationTime, ATSocket_Aggregation_Buffer_Size_t aggregationBufferSize, ATSocket_TCP_Idle_Time_t idleTime);

extern bool ATSocket_DeleteSocket(ATSocket_ID_t socketID);

extern bool ATSocket_ReadSocketLastError(ATSocket_ID_t socketID, ATSocket_Error_Code_t *errorCodeP);

extern bool ATSocket_AddSSLtoSocket(ATSocket_ID_t socketID, ATCommon_SSL_Auth_Mode_t authMode, ATCommon_SSL_Profile_ID_t profileID);

extern bool ATSocket_ReadSocketSSLInfo(ATSocket_ID_t socketID, ATSocket_SSL_Info_t *infoP);

extern bool ATSocket_KeepSocketSSLSession(ATSocket_ID_t socketID);

extern bool ATSocket_DeleteSocketSSLSession(ATSocket_ID_t socketID);

extern bool ATSocket_ReceiveFromSocket(ATSocket_ID_t socketID, ATSocket_Data_Read_t *dataReadP, uint16_t maxBufferLength);

extern bool ATSocket_SendToSocket(ATSocket_ID_t socketID, char *data, ATSocket_Data_Length_t dataLength);

extern bool ATSocket_SetSocketUnsolicitedNotificationEvents(ATSocket_Event_t event, ATCommon_Event_State_t state);

extern bool ATSocket_ParseDataReceivedEvent(char *pEventArguments, ATSocket_ID_t *dataP);

extern bool ATSocket_ParseSocketTerminatedEvent(char *pEventArguments, ATSocket_ID_t *dataP);

extern bool ATSocket_ParseSocketsReadEvent(char *pEventArguments, ATSocket_Read_Result_t *dataP);

#ifdef __cplusplus
}
#endif

#endif /* AT_SOCKET_H_INCLUDED */
