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
 * @brief AT commands for HTTP functionality.
 */

#ifndef AT_HTTP_H_INCLUDED
#define AT_HTTP_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTP Profile IDs
 */
typedef enum ATHTTP_Profile_ID_t
{
	ATHTTP_Profile_ID_Invalid = -1,
	ATHTTP_Profile_ID_1 = 1,
	ATHTTP_Profile_ID_2,
	ATHTTP_Profile_ID_3,
	ATHTTP_Profile_ID_4,
	ATHTTP_Profile_ID_5,
	ATHTTP_Profile_ID_NumberOfValues
} ATHTTP_Profile_ID_t;

typedef int8_t ATHTTP_IP_Session_ID_t;

#define ATHTTP_IP_Session_ID_Invalid -1

/**
 * @brief HTTP IP Address formats
 */
typedef enum ATHTTP_IP_Addr_Format_t
{
	ATHTTP_IP_Addr_Format_Invalid = -1,
	ATHTTP_IP_Addr_Format_IPv4v6,
	ATHTTP_IP_Addr_Format_IPv4,
	ATHTTP_IP_Addr_Format_IPv6,
	ATHTTP_IP_Addr_Format_NumberOfValues
} ATHTTP_IP_Addr_Format_t;

/**
 * @brief HTTP Event Types
 */
typedef enum ATHTTP_Event_t
{
	ATHTTP_Event_Invalid = -1,
	ATHTTP_Event_PUT_Confirmation,
	ATHTTP_Event_POST_Confirmation,
	ATHTTP_Event_DELETE_Confirmation,
	ATHTTP_Event_GET_Receive,
	ATHTTP_Event_Session_Terminated,
	ATHTTP_Event_All,
	ATHTTP_Event_NumberOfValues
} ATHTTP_Event_t;

/**
 * @brief HTTP Event States
 */
typedef enum ATHTTP_Event_State_t
{
	ATHTTP_Event_State_Invalid = -1,
	ATHTTP_Event_State_Success,
	ATHTTP_Event_State_Fail,
	ATHTTP_Event_State_NumberOfValues
} ATHTTP_Event_State_t;

typedef uint16_t ATHTTP_Data_Length_t;

#define ATHTTP_Data_Length_Max 3000

/**
 * @brief HTTP Event Result
 */
typedef struct ATHTTP_Event_Result_t
{
	ATHTTP_Profile_ID_t profileID;
	ATHTTP_Event_State_t state;
} ATHTTP_Event_Result_t;

/**
 * @brief Presence of header in the request or response
 */
typedef enum ATHTTP_Header_Presence_t
{
	ATHTTP_Header_Presence_Invalid = -1,
	ATHTTP_Header_Presence_Disable,
	ATHTTP_Header_Presence_Enable,
	ATHTTP_Header_Presence_NumberOfValues
} ATHTTP_Header_Presence_t;

typedef uint8_t ATHTTP_Header_Count_t;

typedef uint16_t ATHTTP_Body_Size_t;

/**
 * @brief HTTP Response
 */
typedef struct ATHTTP_Response_t
{
	ATHTTP_Data_Length_t dataLength;
	ATHTTP_Data_Length_t receivedLength;
	char *responseBody;
} ATHTTP_Response_t;

typedef uint16_t ATHTTP_Timeout_t;

extern bool ATHTTP_ConfigureNodes(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATCommon_Auth_Username_t username, ATCommon_Auth_Password_t password);

extern bool ATHTTP_ConfigureTLS(ATHTTP_Profile_ID_t profileID, ATCommon_TLS_Auth_Mode_t authMode, ATCommon_TLS_Profile_ID_t tlsProfileID);

extern bool ATHTTP_ConfigureIP(ATHTTP_Profile_ID_t profileID, ATHTTP_IP_Session_ID_t sessionID, ATHTTP_IP_Addr_Format_t ipFormat, ATCommon_Port_Number_t destPort, ATCommon_Port_Number_t sourcePort);

extern bool ATHTTP_ConfigureFormat(ATHTTP_Profile_ID_t profileID, ATHTTP_Header_Presence_t responseHeader, ATHTTP_Header_Presence_t requestHeader);

extern bool ATHTTP_ConfigureTimeout(ATHTTP_Profile_ID_t profileID, ATHTTP_Timeout_t timeout);

extern bool ATHTTP_SetHTTPUnsolicitedNotificationEvents(ATHTTP_Event_t event, ATCommon_Event_State_t state);

extern bool ATHTTP_GET(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATHTTP_Header_Presence_t responseHeader, char *headers[], ATHTTP_Header_Count_t headersCount);

extern bool ATHTTP_DELETE(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATHTTP_Header_Presence_t responseHeader, char *headers[], ATHTTP_Header_Count_t headersCount);

extern bool ATHTTP_POST(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, char *body, ATHTTP_Body_Size_t bodySize, char *contentType, char *headers[], ATHTTP_Header_Count_t headersCount);

extern bool ATHTTP_PUT(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, char *body, ATHTTP_Body_Size_t bodySize, char *contentType, char *headers[], ATHTTP_Header_Count_t headersCount);

extern bool ATHTTP_ReadResponse(ATHTTP_Profile_ID_t profileID, ATHTTP_Data_Length_t maxLength, ATHTTP_Response_t *response);

extern bool ATHTTP_ParseGETEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP);

extern bool ATHTTP_ParseDELETEEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP);

extern bool ATHTTP_ParsePOSTEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP);

extern bool ATHTTP_ParsePUTEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP);

#ifdef __cplusplus
}
#endif

#endif /* AT_HTTP_H_INCLUDED */
