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
 * @brief AT commands for SMS functionality.
 */

#ifndef ADRASTEAI_AT_SMS_H_INCLUDED
#define ADRASTEAI_AT_SMS_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint8_t AdrasteaI_ATSMS_Message_Index_t;

    /**
 * @brief Message States
 */
    typedef enum AdrasteaI_ATSMS_Message_State_t
    {
        AdrasteaI_ATSMS_Message_State_Invalid = -1,
        AdrasteaI_ATSMS_Message_State_Received_Unread,
        AdrasteaI_ATSMS_Message_State_Received_Read,
        AdrasteaI_ATSMS_Message_State_Stored_Unsent,
        AdrasteaI_ATSMS_Message_State_Stored_Sent,
        AdrasteaI_ATSMS_Message_State_All,
        AdrasteaI_ATSMS_Message_State_NumberOfValues
    } AdrasteaI_ATSMS_Message_State_t;

    /**
 * @brief Message Storage Locations
 */
    typedef enum AdrasteaI_ATSMS_Storage_Location_t
    {
        AdrasteaI_ATSMS_Storage_Location_Invalid = -1,
        AdrasteaI_ATSMS_Storage_Location_Broadcast,
        AdrasteaI_ATSMS_Storage_Location_ME,
        AdrasteaI_ATSMS_Storage_Location_MT,
        AdrasteaI_ATSMS_Storage_Location_SIM,
        AdrasteaI_ATSMS_Storage_Location_TA,
        AdrasteaI_ATSMS_Storage_Location_Status_Report,
        AdrasteaI_ATSMS_Storage_Location_NumberOfValues
    } AdrasteaI_ATSMS_Storage_Location_t;

    typedef uint8_t AdrasteaI_ATSMS_Message_Count_t;

    /**
 * @brief Storage Usage
 */
    typedef struct AdrasteaI_ATSMS_Storage_Usage_t
    {
        AdrasteaI_ATSMS_Storage_Location_t storageLocation;
        AdrasteaI_ATSMS_Message_Count_t usedMessages;
        AdrasteaI_ATSMS_Message_Count_t maxMessages;
    } AdrasteaI_ATSMS_Storage_Usage_t;

    /**
 * @brief Message Storage Usage
 */
    typedef struct AdrasteaI_ATSMS_Message_Storage_Usage_t
    {
        AdrasteaI_ATSMS_Storage_Usage_t readDeleteStorageUsage;
        AdrasteaI_ATSMS_Storage_Usage_t writeSendStorageUsage;
        AdrasteaI_ATSMS_Storage_Usage_t receiveStorageUsage;
    } AdrasteaI_ATSMS_Message_Storage_Usage_t;

    typedef uint16_t AdrasteaI_ATSMS_Error_t;

    typedef char AdrasteaI_ATSMS_Address_t[64];

    /**
 * @brief Address Type
 */
    typedef enum AdrasteaI_ATSMS_Address_Type_t
    {
        AdrasteaI_ATSMS_Address_Type_Invalid = -1,
        AdrasteaI_ATSMS_Address_Type_National_Number = 129,
        AdrasteaI_ATSMS_Address_Type_International_Number = 145,
        AdrasteaI_ATSMS_Address_Type_NumberOfValues
    } AdrasteaI_ATSMS_Address_Type_t;

    /**
 * @brief Service Center Address
 */
    typedef struct AdrasteaI_ATSMS_Service_Center_Address_t
    {
        AdrasteaI_ATSMS_Address_t address;
        AdrasteaI_ATSMS_Address_Type_t addressType;
    } AdrasteaI_ATSMS_Service_Center_Address_t;

    typedef char AdrasteaI_ATSMS_Message_Payload_t[128];

    typedef uint8_t AdrasteaI_ATSMS_Message_Reference_t;

    /**
 * @brief SMS Message
 */
    typedef struct AdrasteaI_ATSMS_Message_t
    {
        AdrasteaI_ATSMS_Message_Index_t messageIndex;
        AdrasteaI_ATSMS_Message_State_t messageState;
        AdrasteaI_ATSMS_Address_t address;
        AdrasteaI_ATSMS_Message_Payload_t payload;
    } AdrasteaI_ATSMS_Message_t;

    /**
 * @brief SMS Message Received result
 */
    typedef struct AdrasteaI_ATSMS_Message_Received_Result_t
    {
        AdrasteaI_ATSMS_Storage_Location_t storageLocation;
        AdrasteaI_ATSMS_Message_Index_t messageIndex;
    } AdrasteaI_ATSMS_Message_Received_Result_t;

    extern bool AdrasteaI_ATSMS_DeleteMessage(AdrasteaI_ATSMS_Message_Index_t index);

    extern bool AdrasteaI_ATSMS_DeleteAllMessages();

    extern bool AdrasteaI_ATSMS_ListMessages(AdrasteaI_ATSMS_Message_State_t listType);

    extern bool AdrasteaI_ATSMS_ReadMessage(AdrasteaI_ATSMS_Message_Index_t index);

    extern bool AdrasteaI_ATSMS_SetMessageStorageLocations(AdrasteaI_ATSMS_Storage_Location_t readDeleteStorage, AdrasteaI_ATSMS_Storage_Location_t writeSendStorage, AdrasteaI_ATSMS_Storage_Location_t receiveStorage);

    extern bool AdrasteaI_ATSMS_ReadMessageStorageUsage(AdrasteaI_ATSMS_Message_Storage_Usage_t* storageUsageP);

    extern bool AdrasteaI_ATSMS_SetServiceCenterAddress(AdrasteaI_ATSMS_Service_Center_Address_t serviceCenterAddress);

    extern bool AdrasteaI_ATSMS_ReadServiceCenterAddress(AdrasteaI_ATSMS_Service_Center_Address_t* serviceCenterAddressP);

    extern bool AdrasteaI_ATSMS_SendMessage(AdrasteaI_ATSMS_Address_t address, AdrasteaI_ATSMS_Address_Type_t addressType, AdrasteaI_ATSMS_Message_Payload_t message, AdrasteaI_ATSMS_Message_Reference_t* messageReferenceP);

    extern bool AdrasteaI_ATSMS_SendLargeMessage(AdrasteaI_ATSMS_Address_t address, AdrasteaI_ATSMS_Address_Type_t addressType, AdrasteaI_ATSMS_Message_Payload_t message, AdrasteaI_ATSMS_Message_Reference_t* messageReferenceP);

    extern bool AdrasteaI_ATSMS_WriteMessageToStorage(AdrasteaI_ATSMS_Address_t address, AdrasteaI_ATSMS_Address_Type_t addressType, AdrasteaI_ATSMS_Message_Payload_t message, AdrasteaI_ATSMS_Message_Index_t* messageIndexP);

    extern bool AdrasteaI_ATSMS_SendMessageFromStorage(AdrasteaI_ATSMS_Message_Index_t index, AdrasteaI_ATSMS_Message_Reference_t* messageReferenceP);

    extern bool AdrasteaI_ATSMS_SetSMSUnsolicitedNotificationEvents(AdrasteaI_ATCommon_Event_State_t eventState);

    extern bool AdrasteaI_ATSMS_ParseReadMessageEvent(char* pEventArguments, AdrasteaI_ATSMS_Message_t* dataP);

    extern bool AdrasteaI_ATSMS_ParseListMessagesEvent(char* pEventArguments, AdrasteaI_ATSMS_Message_t* dataP);

    extern bool AdrasteaI_ATSMS_ParseMessageReceivedEvent(char* pEventArguments, AdrasteaI_ATSMS_Message_Received_Result_t* dataP);

    extern bool AdrasteaI_ATSMS_ParseSMSErrorEvent(char* pEventArguments, AdrasteaI_ATSMS_Error_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_SMS_H_INCLUDED */
