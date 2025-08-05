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
 * @brief AT commands for Proprietary functionality.
 */

#ifndef ADRASTEAI_AT_PROPRIETARY_H_INCLUDED
#define ADRASTEAI_AT_PROPRIETARY_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Network Attachment State
 */
    typedef enum AdrasteaI_ATProprietary_Network_Attachment_State_t
    {
        AdrasteaI_ATProprietary_Network_State_Invalid = -1,
        AdrasteaI_ATProprietary_Network_State_Detach,
        AdrasteaI_ATProprietary_Network_State_Attach,
        AdrasteaI_ATProprietary_Network_State_NumberOfValues
    } AdrasteaI_ATProprietary_Network_Attachment_State_t;

    typedef uint8_t AdrasteaI_ATProprietary_PIN_Attempts_t, AdrasteaI_ATProprietary_PUK_Attempts_t, AdrasteaI_ATProprietary_PIN2_Attempts_t, AdrasteaI_ATProprietary_PUK2_Attempts_t;

    /**
 * @brief PIN PUK Attempts
 */
    typedef struct AdrasteaI_ATProprietary_PIN_PUK_Attempts_t
    {
        AdrasteaI_ATProprietary_PIN_Attempts_t pinAttempts;
        AdrasteaI_ATProprietary_PUK_Attempts_t pukAttempts;
        AdrasteaI_ATProprietary_PIN2_Attempts_t pin2Attempts;
        AdrasteaI_ATProprietary_PUK2_Attempts_t puk2Attempts;
    } AdrasteaI_ATProprietary_PIN_PUK_Attempts_t;

    /**
 * @brief RAT
 */
    typedef enum AdrasteaI_ATProprietary_RAT_t
    {
        AdrasteaI_ATProprietary_RAT_Invalid = -1,
        AdrasteaI_ATProprietary_RAT_DEFAULT,
        AdrasteaI_ATProprietary_RAT_CATM,
        AdrasteaI_ATProprietary_RAT_NB_IOT,
        AdrasteaI_ATProprietary_RAT_NumberOfValues
    } AdrasteaI_ATProprietary_RAT_t;

    /**
 * @brief RAT Storage
 */
    typedef enum AdrasteaI_ATProprietary_RAT_Storage_t
    {
        AdrasteaI_ATProprietary_RAT_Storage_Invalid = -1,
        AdrasteaI_ATProprietary_RAT_Storage_Non_Persistant,
        AdrasteaI_ATProprietary_RAT_Storage_Persistant,
        AdrasteaI_ATProprietary_RAT_Storage_NumberOfValues
    } AdrasteaI_ATProprietary_RAT_Storage_t;

    /**
 * @brief RAT Source
 */
    typedef enum AdrasteaI_ATProprietary_RAT_Source_t
    {
        AdrasteaI_ATProprietary_RAT_Source_Invalid = -1,
        AdrasteaI_ATProprietary_RAT_Source_None,
        AdrasteaI_ATProprietary_RAT_Source_Host,
        AdrasteaI_ATProprietary_RAT_Source_NumberOfValues
    } AdrasteaI_ATProprietary_RAT_Source_t;

    /**
 * @brief RAT Mode
 */
    typedef enum AdrasteaI_ATProprietary_RAT_Mode_t
    {
        AdrasteaI_ATProprietary_RAT_Mode_Invalid = -1,
        AdrasteaI_ATProprietary_RAT_Mode_Single,
        AdrasteaI_ATProprietary_RAT_Mode_Multiple,
        AdrasteaI_ATProprietary_RAT_Mode_NumberOfValues
    } AdrasteaI_ATProprietary_RAT_Mode_t;

    /**
 * @brief RAT Status
 */
    typedef struct AdrasteaI_ATProprietary_RAT_Status_t
    {
        AdrasteaI_ATProprietary_RAT_t rat;
        AdrasteaI_ATProprietary_RAT_Mode_t mode;
        AdrasteaI_ATProprietary_RAT_Source_t source;
    } AdrasteaI_ATProprietary_RAT_Status_t;

    typedef uint8_t AdrasteaI_ATProprietary_Boot_Delay_t;

    typedef int16_t AdrasteaI_ATProprietary_Ping_Packet_Count_t, AdrasteaI_ATProprietary_Ping_Packet_Size_t, AdrasteaI_ATProprietary_Ping_Timeout_t;

#define AdrasteaI_ATProprietary_Ping_Packet_Count_Invalid -1

#define AdrasteaI_ATProprietary_Ping_Packet_Size_Invalid -1

#define AdrasteaI_ATProprietary_Ping_Timeout_Invalid -1

    typedef uint8_t AdrasteaI_ATProprietary_Ping_ID_t;

    typedef uint16_t AdrasteaI_ATProprietary_Ping_TTL_t, AdrasteaI_ATProprietary_Ping_RTT_t;

    /**
 * @brief Ping Result
 */
    typedef struct AdrasteaI_ATProprietary_Ping_Result_t
    {
        AdrasteaI_ATProprietary_Ping_ID_t id;
        AdrasteaI_ATCommon_IP_Addr_t addr;
        AdrasteaI_ATProprietary_Ping_TTL_t ttl;
        AdrasteaI_ATProprietary_Ping_RTT_t rtt;
    } AdrasteaI_ATProprietary_Ping_Result_t;

    typedef char AdrasteaI_ATProprietary_Domain_Name_t[128];

    /**
 * @brief Proprietary IP Address format
 */
    typedef enum AdrasteaI_ATProprietary_IP_Addr_Format_t
    {
        AdrasteaI_ATProprietary_IP_Addr_Format_Invalid = -1,
        AdrasteaI_ATProprietary_IP_Addr_Format_IPv4,
        AdrasteaI_ATProprietary_IP_Addr_Format_IPv6,
        AdrasteaI_ATProprietary_IP_Addr_Format_IPv4v6,
        AdrasteaI_ATProprietary_IP_Addr_Format_NumberOfValues
    } AdrasteaI_ATProprietary_IP_Addr_Format_t;

    /**
 * @brief Domain Name Result
 */
    typedef struct AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t
    {
        AdrasteaI_ATProprietary_IP_Addr_Format_t format;
        AdrasteaI_ATCommon_IP_Addr_t addr;
    } AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t;

    typedef char AdrasteaI_ATProprietary_File_Name_t[64];

    /**
 * @brief Credential Format
 */
    typedef enum AdrasteaI_ATProprietary_Credential_Format_t
    {
        AdrasteaI_ATProprietary_Credential_Format_Invalid = -1,
        AdrasteaI_ATProprietary_Credential_Format_Certificate,
        AdrasteaI_ATProprietary_Credential_Format_Private_Key,
        AdrasteaI_ATProprietary_Credential_Format_PSK_ID,
        AdrasteaI_ATProprietary_Credential_Format_PSK_Key,
        AdrasteaI_ATProprietary_Credential_Format_NumberOfValues
    } AdrasteaI_ATProprietary_Credential_Format_t;

    typedef uint8_t AdrasteaI_ATProprietary_File_Name_Count_t;

    /**
 * @brief File Names List
 */
    typedef struct AdrasteaI_ATProprietary_File_Names_List_t
    {
        AdrasteaI_ATProprietary_File_Name_Count_t count;
        AdrasteaI_ATProprietary_File_Name_t* filenames;
    } AdrasteaI_ATProprietary_File_Names_List_t;

    typedef char AdrasteaI_ATProprietary_File_Path_t[128];

    /**
 * @brief TLS Profile ID list
 */
    typedef struct AdrasteaI_ATProprietary_TLS_Profile_ID_List_t
    {
        AdrasteaI_ATCommon_TLS_Profile_ID_Count_t count;
        AdrasteaI_ATCommon_TLS_Profile_ID_t* profileIDs;
    } AdrasteaI_ATProprietary_TLS_Profile_ID_List_t;

    /**
 * @brief PDN Parameters
 */
    typedef struct AdrasteaI_ATProprietary_PDN_Parameters_t
    {
        AdrasteaI_ATCommon_Session_ID_t sessionID;
        AdrasteaI_ATCommon_APN_Name_t apnName;
        AdrasteaI_ATProprietary_IP_Addr_Format_t ipFormat;
    } AdrasteaI_ATProprietary_PDN_Parameters_t;

    extern bool AdrasteaI_ATProprietary_ReadNetworkAttachmentState(AdrasteaI_ATProprietary_Network_Attachment_State_t* stateP);

    extern bool AdrasteaI_ATProprietary_SetNetworkAttachmentState(AdrasteaI_ATProprietary_Network_Attachment_State_t state);

    extern bool AdrasteaI_ATProprietary_ReadRemainingPINPUKAttempts(AdrasteaI_ATProprietary_PIN_PUK_Attempts_t* attemptsP);

    extern bool AdrasteaI_ATProprietary_SwitchToRATWithoutFullReboot(AdrasteaI_ATProprietary_RAT_t rat, AdrasteaI_ATProprietary_RAT_Storage_t storage, AdrasteaI_ATProprietary_RAT_Source_t source);

    extern bool AdrasteaI_ATProprietary_ReadRATStatus(AdrasteaI_ATProprietary_RAT_Status_t* ratstatusP);

    extern bool AdrasteaI_ATProprietary_SetBootDelay(AdrasteaI_ATProprietary_Boot_Delay_t delay);

    extern bool AdrasteaI_ATProprietary_ResolveDomainName(AdrasteaI_ATCommon_Session_ID_t sessionid, AdrasteaI_ATProprietary_Domain_Name_t domain, AdrasteaI_ATProprietary_IP_Addr_Format_t format);

    extern bool AdrasteaI_ATProprietary_ParseResolveDomainNameEvent(char* pEventArguments, AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t* dataP);

    extern bool AdrasteaI_ATProprietary_Ping(AdrasteaI_ATProprietary_IP_Addr_Format_t format, AdrasteaI_ATCommon_IP_Addr_t destaddr, AdrasteaI_ATProprietary_Ping_Packet_Count_t packetcount, AdrasteaI_ATProprietary_Ping_Packet_Size_t packetsize, AdrasteaI_ATProprietary_Ping_Timeout_t timeout);

    extern bool AdrasteaI_ATProprietary_ParsePingResultEvent(char* pEventArguments, AdrasteaI_ATProprietary_Ping_Result_t* dataP);

    extern bool AdrasteaI_ATProprietary_ReadCredential(AdrasteaI_ATProprietary_File_Name_t filename, char* dataP, uint16_t dataMaxBufferSize);

    extern bool AdrasteaI_ATProprietary_WriteCredential(AdrasteaI_ATProprietary_File_Name_t filename, AdrasteaI_ATProprietary_Credential_Format_t format, char* data);

    extern bool AdrasteaI_ATProprietary_ListCredentials(AdrasteaI_ATProprietary_File_Names_List_t* filenamesList);

    extern bool AdrasteaI_ATProprietary_DeleteCredential(AdrasteaI_ATProprietary_File_Name_t filename);

    extern bool AdrasteaI_ATProprietary_ListTLSProfiles(AdrasteaI_ATProprietary_TLS_Profile_ID_List_t* profileIDsList);

    extern bool AdrasteaI_ATProprietary_AddTLSProfile(AdrasteaI_ATCommon_TLS_Profile_ID_t profileID, AdrasteaI_ATProprietary_File_Name_t CA, AdrasteaI_ATProprietary_File_Path_t CAPath, AdrasteaI_ATProprietary_File_Name_t deviceCert, AdrasteaI_ATProprietary_File_Name_t deviceKey, AdrasteaI_ATProprietary_File_Name_t pskID, AdrasteaI_ATProprietary_File_Name_t pskKey);

    extern bool AdrasteaI_ATProprietary_DeleteTLSProfile(AdrasteaI_ATCommon_TLS_Profile_ID_t profileID);

    extern bool AdrasteaI_ATProprietary_SetPDNParameters(AdrasteaI_ATProprietary_PDN_Parameters_t parameters);

    extern bool AdrasteaI_ATProprietary_ReadPDNParameters(AdrasteaI_ATProprietary_PDN_Parameters_t* parameters);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_PROPRIETARY_H_INCLUDED */
