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
 * @brief AT commands for HTTP functionality.
 */

#ifndef ADRASTEAI_AT_HTTP_H_INCLUDED
#define ADRASTEAI_AT_HTTP_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief HTTP Profile IDs
 */
    typedef enum AdrasteaI_ATHTTP_Profile_ID_t
    {
        AdrasteaI_ATHTTP_Profile_ID_Invalid = -1,
        AdrasteaI_ATHTTP_Profile_ID_1 = 1,
        AdrasteaI_ATHTTP_Profile_ID_2,
        AdrasteaI_ATHTTP_Profile_ID_3,
        AdrasteaI_ATHTTP_Profile_ID_4,
        AdrasteaI_ATHTTP_Profile_ID_5,
        AdrasteaI_ATHTTP_Profile_ID_NumberOfValues
    } AdrasteaI_ATHTTP_Profile_ID_t;

    typedef int8_t AdrasteaI_ATHTTP_IP_Session_ID_t;

#define AdrasteaI_ATHTTP_IP_Session_ID_Invalid -1

    /**
 * @brief HTTP IP Address formats
 */
    typedef enum AdrasteaI_ATHTTP_IP_Addr_Format_t
    {
        AdrasteaI_ATHTTP_IP_Addr_Format_Invalid = -1,
        AdrasteaI_ATHTTP_IP_Addr_Format_IPv4v6,
        AdrasteaI_ATHTTP_IP_Addr_Format_IPv4,
        AdrasteaI_ATHTTP_IP_Addr_Format_IPv6,
        AdrasteaI_ATHTTP_IP_Addr_Format_NumberOfValues
    } AdrasteaI_ATHTTP_IP_Addr_Format_t;

    /**
 * @brief HTTP Event Types
 */
    typedef enum AdrasteaI_ATHTTP_Event_t
    {
        AdrasteaI_ATHTTP_Event_Invalid = -1,
        AdrasteaI_ATHTTP_Event_PUT_Confirmation,
        AdrasteaI_ATHTTP_Event_POST_Confirmation,
        AdrasteaI_ATHTTP_Event_DELETE_Confirmation,
        AdrasteaI_ATHTTP_Event_GET_Receive,
        AdrasteaI_ATHTTP_Event_Session_Terminated,
        AdrasteaI_ATHTTP_Event_All,
        AdrasteaI_ATHTTP_Event_NumberOfValues
    } AdrasteaI_ATHTTP_Event_t;

    /**
 * @brief HTTP Event States
 */
    typedef enum AdrasteaI_ATHTTP_Event_State_t
    {
        AdrasteaI_ATHTTP_Event_State_Invalid = -1,
        AdrasteaI_ATHTTP_Event_State_Success,
        AdrasteaI_ATHTTP_Event_State_Fail,
        AdrasteaI_ATHTTP_Event_State_NumberOfValues
    } AdrasteaI_ATHTTP_Event_State_t;

    typedef uint16_t AdrasteaI_ATHTTP_Data_Length_t;

#define AdrasteaI_ATHTTP_Data_Length_Max 3000

    /**
 * @brief HTTP Event Result
 */
    typedef struct AdrasteaI_ATHTTP_Event_Result_t
    {
        AdrasteaI_ATHTTP_Profile_ID_t profileID;
        AdrasteaI_ATHTTP_Event_State_t state;
    } AdrasteaI_ATHTTP_Event_Result_t;

    /**
 * @brief Presence of header in the request or response
 */
    typedef enum AdrasteaI_ATHTTP_Header_Presence_t
    {
        AdrasteaI_ATHTTP_Header_Presence_Invalid = -1,
        AdrasteaI_ATHTTP_Header_Presence_Disable,
        AdrasteaI_ATHTTP_Header_Presence_Enable,
        AdrasteaI_ATHTTP_Header_Presence_NumberOfValues
    } AdrasteaI_ATHTTP_Header_Presence_t;

    typedef uint8_t AdrasteaI_ATHTTP_Header_Count_t;

    typedef uint16_t AdrasteaI_ATHTTP_Body_Size_t;

    /**
 * @brief HTTP Response
 */
    typedef struct AdrasteaI_ATHTTP_Response_t
    {
        AdrasteaI_ATHTTP_Data_Length_t dataLength;
        AdrasteaI_ATHTTP_Data_Length_t receivedLength;
        char* responseBody;
    } AdrasteaI_ATHTTP_Response_t;

    typedef uint16_t AdrasteaI_ATHTTP_Timeout_t;

    extern bool AdrasteaI_ATHTTP_ConfigureNodes(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATCommon_Auth_Username_t username, AdrasteaI_ATCommon_Auth_Password_t password);

    extern bool AdrasteaI_ATHTTP_ConfigureTLS(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_TLS_Auth_Mode_t authMode, AdrasteaI_ATCommon_TLS_Profile_ID_t tlsProfileID);

    extern bool AdrasteaI_ATHTTP_ConfigureIP(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATHTTP_IP_Session_ID_t sessionID, AdrasteaI_ATHTTP_IP_Addr_Format_t ipFormat, AdrasteaI_ATCommon_Port_Number_t destPort, AdrasteaI_ATCommon_Port_Number_t sourcePort);

    extern bool AdrasteaI_ATHTTP_ConfigureFormat(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATHTTP_Header_Presence_t responseHeader, AdrasteaI_ATHTTP_Header_Presence_t requestHeader);

    extern bool AdrasteaI_ATHTTP_ConfigureTimeout(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATHTTP_Timeout_t timeout);

    extern bool AdrasteaI_ATHTTP_SetHTTPUnsolicitedNotificationEvents(AdrasteaI_ATHTTP_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

    extern bool AdrasteaI_ATHTTP_GET(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATHTTP_Header_Presence_t responseHeader, char* headers[], AdrasteaI_ATHTTP_Header_Count_t headersCount);

    extern bool AdrasteaI_ATHTTP_DELETE(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATHTTP_Header_Presence_t responseHeader, char* headers[], AdrasteaI_ATHTTP_Header_Count_t headersCount);

    extern bool AdrasteaI_ATHTTP_POST(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_IP_Addr_t addr, char* body, AdrasteaI_ATHTTP_Body_Size_t bodySize, char* contentType, char* headers[], AdrasteaI_ATHTTP_Header_Count_t headersCount);

    extern bool AdrasteaI_ATHTTP_PUT(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATCommon_IP_Addr_t addr, char* body, AdrasteaI_ATHTTP_Body_Size_t bodySize, char* contentType, char* headers[], AdrasteaI_ATHTTP_Header_Count_t headersCount);

    extern bool AdrasteaI_ATHTTP_ReadResponse(AdrasteaI_ATHTTP_Profile_ID_t profileID, AdrasteaI_ATHTTP_Data_Length_t maxLength, AdrasteaI_ATHTTP_Response_t* response);

    extern bool AdrasteaI_ATHTTP_ParseGETEvent(char* pEventArguments, AdrasteaI_ATHTTP_Event_Result_t* dataP);

    extern bool AdrasteaI_ATHTTP_ParseDELETEEvent(char* pEventArguments, AdrasteaI_ATHTTP_Event_Result_t* dataP);

    extern bool AdrasteaI_ATHTTP_ParsePOSTEvent(char* pEventArguments, AdrasteaI_ATHTTP_Event_Result_t* dataP);

    extern bool AdrasteaI_ATHTTP_ParsePUTEvent(char* pEventArguments, AdrasteaI_ATHTTP_Event_Result_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_HTTP_H_INCLUDED */
