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
 * @brief AT commands for Packet Domain functionality.
 */

#ifndef AT_PACKETDOMAIN_H_INCLUDED
#define AT_PACKETDOMAIN_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network Registration Result Codes
 */
typedef enum ATPacketDomain_Network_Registration_Result_Code_t
{
	ATPacketDomain_Network_Registration_Result_Code_Invalid = -1,
	ATPacketDomain_Network_Registration_Result_Code_Disable,
	ATPacketDomain_Network_Registration_Result_Code_Enable,
	ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info,
	ATPacketDomain_Network_Registration_Result_Code_NumberOfValues
} ATPacketDomain_Network_Registration_Result_Code_t;

/**
 * @brief Network Registration States
 */
typedef enum ATPacketDomain_Network_Registration_State_t
{
	ATPacketDomain_Network_Registration_State_Invalid = -1,
	ATPacketDomain_Network_Registration_State_Not_Registered_Not_Searching,
	ATPacketDomain_Network_Registration_State_Registered_Home_Network,
	ATPacketDomain_Network_Registration_State_Not_Registered_Searching,
	ATPacketDomain_Network_Registration_State_Registration_Denied,
	ATPacketDomain_Network_Registration_State_Unknown,
	ATPacketDomain_Network_Registration_State_Registered_Roaming,
	ATPacketDomain_Network_Registration_State_Registered_SMS_Only_Home_Network,
	ATPacketDomain_Network_Registration_State_Registered_SMS_Only_Roaming,
	ATPacketDomain_Network_Registration_State_Attached_For_Emergency_Bearer_Services_Only,
	ATPacketDomain_Network_Registration_State_Registered_For_CSFB_Not_Preferred_Home_Network,
	ATPacketDomain_Network_Registration_State_Registered_For_CSFB_Not_Preferred_Roaming,
	ATPacketDomain_Network_Registration_State_NumberOfValues
} ATPacketDomain_Network_Registration_State_t;

typedef char ATPacketDomain_TAC_t[5];

typedef char ATPacketDomain_ECI_t[9];

typedef uint8_t ATPacketDomain_Network_Registration_Cause_Value_t;

/**
 * @brief Network Registration Status
 */
typedef struct ATPacketDomain_Network_Registration_Status_t
{
	ATPacketDomain_Network_Registration_Result_Code_t resultCode;
	ATPacketDomain_Network_Registration_State_t state;
	ATPacketDomain_TAC_t TAC;
	ATPacketDomain_ECI_t ECI;
	ATCommon_AcT_t AcT;
} ATPacketDomain_Network_Registration_Status_t;

/**
 * @brief PDP Context States
 */
typedef enum ATPacketDomain_PDP_Context_State_t
{
	ATPacketDomain_PDP_Context_State_Invalid = -1,
	ATPacketDomain_PDP_Context_State_Deactivated,
	ATPacketDomain_PDP_Context_State_Activated,
	ATPacketDomain_PDP_Context_State_NumberOfValues
} ATPacketDomain_PDP_Context_State_t;

typedef uint8_t ATPacketDomain_PDP_Context_CID_t;

/**
 * @brief PDP Context State link with Context CID
 */
typedef struct ATPacketDomain_PDP_Context_CID_State_t
{
	ATPacketDomain_PDP_Context_CID_t cid;
	ATPacketDomain_PDP_Context_State_t state;
} ATPacketDomain_PDP_Context_CID_State_t;

/**
 * @brief Event Reporting Modes
 */
typedef enum ATPacketDomain_Event_Reporting_Mode_t
{
	ATPacketDomain_Event_Reporting_Mode_Invalid = -1,
	ATPacketDomain_Event_Reporting_Mode_Buffer_Unsolicited_Result_Codes_if_Full_Discard,
	ATPacketDomain_Event_Reporting_Mode_Discard_Unsolicited_Result_Codes_when_Link_is_Reserved_Otherwise_Forward,
	ATPacketDomain_Event_Reporting_Mode_Buffer_Unsolicited_Result_Codes_when_Link_is_Reserved_Otherwise_Forward,
	ATPacketDomain_Event_Reporting_Mode_NumberOfValues
} ATPacketDomain_Event_Reporting_Mode_t;

/**
 * @brief Event Reporting Buffer
 */
typedef enum ATPacketDomain_Event_Reporting_Buffer_t
{
	ATPacketDomain_Event_Reporting_Buffer_Invalid = -1,
	ATPacketDomain_Event_Reporting_Buffer_Clear,
	ATPacketDomain_Event_Reporting_Buffer_Flush,
	ATPacketDomain_Event_Reporting_Buffer_NumberOfValues
} ATPacketDomain_Event_Reporting_Buffer_t;

/**
 * @brief Event Reporting
 */
typedef struct ATPacketDomain_Event_Reporting_t
{
	ATPacketDomain_Event_Reporting_Mode_t mode;
	ATPacketDomain_Event_Reporting_Buffer_t buffer;
} ATPacketDomain_Event_Reporting_t;

/**
 * @brief PDP Types
 */
typedef enum ATPacketDomain_PDP_Type_t
{
	ATPacketDomain_PDP_Type_Invalid = -1,
	ATPacketDomain_PDP_Type_IPv4,
	ATPacketDomain_PDP_Type_IPv6,
	ATPacketDomain_PDP_Type_IPv4v6,
	ATPacketDomain_PDP_Type_Non_IP,
	ATPacketDomain_PDP_Type_NumberOfValues
} ATPacketDomain_PDP_Type_t;

/**
 * @brief PDP Context
 */
typedef struct ATPacketDomain_PDP_Context_t
{
	ATPacketDomain_PDP_Context_CID_t cid;
	ATPacketDomain_PDP_Type_t pdpType;
	ATCommon_APN_Name_t apnName;
} ATPacketDomain_PDP_Context_t;

extern bool ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_t resultcode);

extern bool ATPacketDomain_ReadNetworkRegistrationStatus(ATPacketDomain_Network_Registration_Status_t *statusP);

extern bool ATPacketDomain_ParseNetworkRegistrationStatusEvent(char *pEventArguments, ATPacketDomain_Network_Registration_Status_t *dataP);

extern bool ATPacketDomain_SetPacketDomainEventReporting(ATPacketDomain_Event_Reporting_t reporting);

extern bool ATPacketDomain_ReadPacketDomainEventReporting(ATPacketDomain_Event_Reporting_t *reportingP);

extern bool ATPacketDomain_DefinePDPContext(ATPacketDomain_PDP_Context_t context);

extern bool ATPacketDomain_ReadPDPContexts();

extern bool ATPacketDomain_ParsePDPContextEvent(char *pEventArguments, ATPacketDomain_PDP_Context_t *dataP);

extern bool ATPacketDomain_SetPDPContextState(ATPacketDomain_PDP_Context_CID_State_t cidstate);

extern bool ATPacketDomain_ReadPDPContextsState();

extern bool ATPacketDomain_ParsePDPContextStateEvent(char *pEventArguments, ATPacketDomain_PDP_Context_CID_State_t *dataP);

#ifdef __cplusplus
}
#endif

#endif /* AT_PACKETDOMAIN_H_INCLUDED */
