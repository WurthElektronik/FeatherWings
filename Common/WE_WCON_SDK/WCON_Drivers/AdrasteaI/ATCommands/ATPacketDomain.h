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
 * @brief AT commands for Packet Domain functionality.
 */

#ifndef ADRASTEAI_AT_PACKETDOMAIN_H_INCLUDED
#define ADRASTEAI_AT_PACKETDOMAIN_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Network Registration Result Codes
 */
    typedef enum AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_t
    {
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Invalid = -1,
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Disable,
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable,
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info,
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_NumberOfValues
    } AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_t;

    /**
 * @brief Network Registration States
 */
    typedef enum AdrasteaI_ATPacketDomain_Network_Registration_State_t
    {
        AdrasteaI_ATPacketDomain_Network_Registration_State_Invalid = -1,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Not_Registered_Not_Searching,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Home_Network,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Not_Registered_Searching,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registration_Denied,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Unknown,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_SMS_Only_Home_Network,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_SMS_Only_Roaming,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Attached_For_Emergency_Bearer_Services_Only,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_For_CSFB_Not_Preferred_Home_Network,
        AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_For_CSFB_Not_Preferred_Roaming,
        AdrasteaI_ATPacketDomain_Network_Registration_State_NumberOfValues
    } AdrasteaI_ATPacketDomain_Network_Registration_State_t;

    typedef char AdrasteaI_ATPacketDomain_TAC_t[5];

    typedef char AdrasteaI_ATPacketDomain_ECI_t[9];

    typedef uint8_t AdrasteaI_ATPacketDomain_Network_Registration_Cause_Value_t;

    /**
 * @brief Network Registration Status
 */
    typedef struct AdrasteaI_ATPacketDomain_Network_Registration_Status_t
    {
        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_t resultCode;
        AdrasteaI_ATPacketDomain_Network_Registration_State_t state;
        AdrasteaI_ATPacketDomain_TAC_t TAC;
        AdrasteaI_ATPacketDomain_ECI_t ECI;
        AdrasteaI_ATCommon_AcT_t AcT;
    } AdrasteaI_ATPacketDomain_Network_Registration_Status_t;

    /**
 * @brief PDP Context States
 */
    typedef enum AdrasteaI_ATPacketDomain_PDP_Context_State_t
    {
        AdrasteaI_ATPacketDomain_PDP_Context_State_Invalid = -1,
        AdrasteaI_ATPacketDomain_PDP_Context_State_Deactivated,
        AdrasteaI_ATPacketDomain_PDP_Context_State_Activated,
        AdrasteaI_ATPacketDomain_PDP_Context_State_NumberOfValues
    } AdrasteaI_ATPacketDomain_PDP_Context_State_t;

    typedef uint8_t AdrasteaI_ATPacketDomain_PDP_Context_CID_t;

    /**
 * @brief PDP Context State link with Context CID
 */
    typedef struct AdrasteaI_ATPacketDomain_PDP_Context_CID_State_t
    {
        AdrasteaI_ATPacketDomain_PDP_Context_CID_t cid;
        AdrasteaI_ATPacketDomain_PDP_Context_State_t state;
    } AdrasteaI_ATPacketDomain_PDP_Context_CID_State_t;

    /**
 * @brief Event Reporting Modes
 */
    typedef enum AdrasteaI_ATPacketDomain_Event_Reporting_Mode_t
    {
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_Invalid = -1,
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_Buffer_Unsolicited_Result_Codes_if_Full_Discard,
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_Discard_Unsolicited_Result_Codes_when_Link_is_Reserved_Otherwise_Forward,
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_Buffer_Unsolicited_Result_Codes_when_Link_is_Reserved_Otherwise_Forward,
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_NumberOfValues
    } AdrasteaI_ATPacketDomain_Event_Reporting_Mode_t;

    /**
 * @brief Event Reporting Buffer
 */
    typedef enum AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_t
    {
        AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_Invalid = -1,
        AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_Clear,
        AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_Flush,
        AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_NumberOfValues
    } AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_t;

    /**
 * @brief Event Reporting
 */
    typedef struct AdrasteaI_ATPacketDomain_Event_Reporting_t
    {
        AdrasteaI_ATPacketDomain_Event_Reporting_Mode_t mode;
        AdrasteaI_ATPacketDomain_Event_Reporting_Buffer_t buffer;
    } AdrasteaI_ATPacketDomain_Event_Reporting_t;

    /**
 * @brief PDP Types
 */
    typedef enum AdrasteaI_ATPacketDomain_PDP_Type_t
    {
        AdrasteaI_ATPacketDomain_PDP_Type_Invalid = -1,
        AdrasteaI_ATPacketDomain_PDP_Type_IPv4,
        AdrasteaI_ATPacketDomain_PDP_Type_IPv6,
        AdrasteaI_ATPacketDomain_PDP_Type_IPv4v6,
        AdrasteaI_ATPacketDomain_PDP_Type_Non_IP,
        AdrasteaI_ATPacketDomain_PDP_Type_NumberOfValues
    } AdrasteaI_ATPacketDomain_PDP_Type_t;

    /**
 * @brief PDP Context
 */
    typedef struct AdrasteaI_ATPacketDomain_PDP_Context_t
    {
        AdrasteaI_ATPacketDomain_PDP_Context_CID_t cid;
        AdrasteaI_ATPacketDomain_PDP_Type_t pdpType;
        AdrasteaI_ATCommon_APN_Name_t apnName;
    } AdrasteaI_ATPacketDomain_PDP_Context_t;

    extern bool AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_t resultcode);

    extern bool AdrasteaI_ATPacketDomain_ReadNetworkRegistrationStatus(AdrasteaI_ATPacketDomain_Network_Registration_Status_t* statusP);

    extern bool AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(char* pEventArguments, AdrasteaI_ATPacketDomain_Network_Registration_Status_t* dataP);

    extern bool AdrasteaI_ATPacketDomain_SetPacketDomainEventReporting(AdrasteaI_ATPacketDomain_Event_Reporting_t reporting);

    extern bool AdrasteaI_ATPacketDomain_ReadPacketDomainEventReporting(AdrasteaI_ATPacketDomain_Event_Reporting_t* reportingP);

    extern bool AdrasteaI_ATPacketDomain_DefinePDPContext(AdrasteaI_ATPacketDomain_PDP_Context_t context);

    extern bool AdrasteaI_ATPacketDomain_ReadPDPContexts();

    extern bool AdrasteaI_ATPacketDomain_ParsePDPContextEvent(char* pEventArguments, AdrasteaI_ATPacketDomain_PDP_Context_t* dataP);

    extern bool AdrasteaI_ATPacketDomain_SetPDPContextState(AdrasteaI_ATPacketDomain_PDP_Context_CID_State_t cidstate);

    extern bool AdrasteaI_ATPacketDomain_ReadPDPContextsState();

    extern bool AdrasteaI_ATPacketDomain_ParsePDPContextStateEvent(char* pEventArguments, AdrasteaI_ATPacketDomain_PDP_Context_CID_State_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_PACKETDOMAIN_H_INCLUDED */
