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
 * @brief AT commands for SMS functionality.
 */

#ifndef AT_SMS_H_INCLUDED
#define AT_SMS_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t ATSMS_Message_Index_t;

/**
 * @brief Message States
 */
typedef enum ATSMS_Message_State_t
{
	ATSMS_Message_State_Invalid = -1,
	ATSMS_Message_State_Received_Unread,
	ATSMS_Message_State_Received_Read,
	ATSMS_Message_State_Stored_Unsent,
	ATSMS_Message_State_Stored_Sent,
	ATSMS_Message_State_All,
	ATSMS_Message_State_NumberOfValues
} ATSMS_Message_State_t;

/**
 * @brief Message Storage Locations
 */
typedef enum ATSMS_Storage_Location_t
{
	ATSMS_Storage_Location_Invalid = -1,
	ATSMS_Storage_Location_Broadcast,
	ATSMS_Storage_Location_ME,
	ATSMS_Storage_Location_MT,
	ATSMS_Storage_Location_SIM,
	ATSMS_Storage_Location_TA,
	ATSMS_Storage_Location_Status_Report,
	ATSMS_Storage_Location_NumberOfValues
} ATSMS_Storage_Location_t;

typedef uint8_t ATSMS_Message_Count_t;

/**
 * @brief Storage Usage
 */
typedef struct ATSMS_Storage_Usage_t
{
	ATSMS_Storage_Location_t storageLocation;
	ATSMS_Message_Count_t usedMessages;
	ATSMS_Message_Count_t maxMessages;
} ATSMS_Storage_Usage_t;

/**
 * @brief Message Storage Usage
 */
typedef struct ATSMS_Message_Storage_Usage_t
{
	ATSMS_Storage_Usage_t readDeleteStorageUsage;
	ATSMS_Storage_Usage_t writeSendStorageUsage;
	ATSMS_Storage_Usage_t receiveStorageUsage;
} ATSMS_Message_Storage_Usage_t;

typedef uint16_t ATSMS_Error_t;

typedef char ATSMS_Address_t[64];

/**
 * @brief Address Type
 */
typedef enum ATSMS_Address_Type_t
{
	ATSMS_Address_Type_Invalid = -1,
	ATSMS_Address_Type_National_Number = 129,
	ATSMS_Address_Type_International_Number = 145,
	ATSMS_Address_Type_NumberOfValues
} ATSMS_Address_Type_t;

/**
 * @brief Service Center Address
 */
typedef struct ATSMS_Service_Center_Address_t
{
	ATSMS_Address_t address;
	ATSMS_Address_Type_t addressType;
} ATSMS_Service_Center_Address_t;

typedef char ATSMS_Message_Payload_t[128];

typedef uint8_t ATSMS_Message_Reference_t;

/**
 * @brief SMS Message
 */
typedef struct ATSMS_Message_t
{
	ATSMS_Message_Index_t messageIndex;
	ATSMS_Message_State_t messageState;
	ATSMS_Address_t address;
	ATSMS_Message_Payload_t payload;
} ATSMS_Message_t;

/**
 * @brief SMS Message Received result
 */
typedef struct ATSMS_Message_Received_Result_t
{
	ATSMS_Storage_Location_t storageLocation;
	ATSMS_Message_Index_t messageIndex;
} ATSMS_Message_Received_Result_t;

extern bool ATSMS_DeleteMessage(ATSMS_Message_Index_t index);

extern bool ATSMS_DeleteAllMessages();

extern bool ATSMS_ListMessages(ATSMS_Message_State_t listType);

extern bool ATSMS_ReadMessage(ATSMS_Message_Index_t index);

extern bool ATSMS_SetMessageStorageLocations(ATSMS_Storage_Location_t readDeleteStorage, ATSMS_Storage_Location_t writeSendStorage, ATSMS_Storage_Location_t receiveStorage);

extern bool ATSMS_ReadMessageStorageUsage(ATSMS_Message_Storage_Usage_t *storageUsageP);

extern bool ATSMS_SetServiceCenterAddress(ATSMS_Service_Center_Address_t serviceCenterAddress);

extern bool ATSMS_ReadServiceCenterAddress(ATSMS_Service_Center_Address_t *serviceCenterAddressP);

extern bool ATSMS_SendMessage(ATSMS_Address_t address, ATSMS_Address_Type_t addressType, ATSMS_Message_Payload_t message, ATSMS_Message_Reference_t *messageReferenceP);

extern bool ATSMS_SendLargeMessage(ATSMS_Address_t address, ATSMS_Address_Type_t addressType, ATSMS_Message_Payload_t message, ATSMS_Message_Reference_t *messageReferenceP);

extern bool ATSMS_WriteMessageToStorage(ATSMS_Address_t address, ATSMS_Address_Type_t addressType, ATSMS_Message_Payload_t message, ATSMS_Message_Index_t *messageIndexP);

extern bool ATSMS_SendMessageFromStorage(ATSMS_Message_Index_t index, ATSMS_Message_Reference_t *messageReferenceP);

extern bool ATSMS_SetSMSUnsolicitedNotificationEvents(ATCommon_Event_State_t eventState);

extern bool ATSMS_ParseReadMessageEvent(char *pEventArguments, ATSMS_Message_t *dataP);

extern bool ATSMS_ParseListMessagesEvent(char *pEventArguments, ATSMS_Message_t *dataP);

extern bool ATSMS_ParseMessageReceivedEvent(char *pEventArguments, ATSMS_Message_Received_Result_t *dataP);

extern bool ATSMS_ParseSMSErrorEvent(char *pEventArguments, ATSMS_Error_t *dataP);

#ifdef __cplusplus
}
#endif

#endif /* AT_SMS_H_INCLUDED */
