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
 * @brief AT commands for Proprietary functionality.
 */

#ifndef AT_PROPRIETARY_H_INCLUDED
#define AT_PROPRIETARY_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network Attachment State
 */
typedef enum ATProprietary_Network_Attachment_State_t
{
	ATProprietary_Network_State_Invalid = -1,
	ATProprietary_Network_State_Detach,
	ATProprietary_Network_State_Attach,
	ATProprietary_Network_State_NumberOfValues
} ATProprietary_Network_Attachment_State_t;

typedef uint8_t ATProprietary_PIN_Attempts_t, ATProprietary_PUK_Attempts_t, ATProprietary_PIN2_Attempts_t, ATProprietary_PUK2_Attempts_t;

/**
 * @brief PIN PUK Attempts
 */
typedef struct ATProprietary_PIN_PUK_Attempts_t
{
	ATProprietary_PIN_Attempts_t pinAttempts;
	ATProprietary_PUK_Attempts_t pukAttempts;
	ATProprietary_PIN2_Attempts_t pin2Attempts;
	ATProprietary_PUK2_Attempts_t puk2Attempts;
} ATProprietary_PIN_PUK_Attempts_t;

/**
 * @brief RAT
 */
typedef enum ATProprietary_RAT_t
{
	ATProprietary_RAT_Invalid = -1,
	ATProprietary_RAT_DEFAULT,
	ATProprietary_RAT_CATM,
	ATProprietary_RAT_NB_IOT,
	ATProprietary_RAT_NumberOfValues
} ATProprietary_RAT_t;

/**
 * @brief RAT Storage
 */
typedef enum ATProprietary_RAT_Storage_t
{
	ATProprietary_RAT_Storage_Invalid = -1,
	ATProprietary_RAT_Storage_Non_Persistant,
	ATProprietary_RAT_Storage_Persistant,
	ATProprietary_RAT_Storage_NumberOfValues
} ATProprietary_RAT_Storage_t;

/**
 * @brief RAT Source
 */
typedef enum ATProprietary_RAT_Source_t
{
	ATProprietary_RAT_Source_Invalid = -1,
	ATProprietary_RAT_Source_None,
	ATProprietary_RAT_Source_Host,
	ATProprietary_RAT_Source_NumberOfValues
} ATProprietary_RAT_Source_t;

/**
 * @brief RAT Mode
 */
typedef enum ATProprietary_RAT_Mode_t
{
	ATProprietary_RAT_Mode_Invalid = -1,
	ATProprietary_RAT_Mode_Single,
	ATProprietary_RAT_Mode_Multiple,
	ATProprietary_RAT_Mode_NumberOfValues
} ATProprietary_RAT_Mode_t;

/**
 * @brief RAT Status
 */
typedef struct ATProprietary_RAT_Status_t
{
	ATProprietary_RAT_t rat;
	ATProprietary_RAT_Mode_t mode;
	ATProprietary_RAT_Source_t source;
} ATProprietary_RAT_Status_t;

typedef uint8_t ATProprietary_Boot_Delay_t;

typedef int16_t ATProprietary_Ping_Packet_Count_t, ATProprietary_Ping_Packet_Size_t, ATProprietary_Ping_Timeout_t;

#define ATProprietary_Ping_Packet_Count_Invalid -1

#define ATProprietary_Ping_Packet_Size_Invalid -1

#define ATProprietary_Ping_Timeout_Invalid -1

typedef uint8_t ATProprietary_Ping_ID_t;

typedef uint16_t ATProprietary_Ping_TTL_t, ATProprietary_Ping_RTT_t;

/**
 * @brief Ping Result
 */
typedef struct ATProprietary_Ping_Result_t
{
	ATProprietary_Ping_ID_t id;
	ATCommon_IP_Addr_t addr;
	ATProprietary_Ping_TTL_t ttl;
	ATProprietary_Ping_RTT_t rtt;
} ATProprietary_Ping_Result_t;

typedef char ATProprietary_Domain_Name_t[128];

/**
 * @brief Proprietary IP Address format
 */
typedef enum ATProprietary_IP_Addr_Format_t
{
	ATProprietary_IP_Addr_Format_Invalid = -1,
	ATProprietary_IP_Addr_Format_IPv4,
	ATProprietary_IP_Addr_Format_IPv6,
	ATProprietary_IP_Addr_Format_IPv4v6,
	ATProprietary_IP_Addr_Format_NumberOfValues
} ATProprietary_IP_Addr_Format_t;

/**
 * @brief Domain Name Result
 */
typedef struct ATProprietary_Domain_Name_Resolve_Result_t
{
	ATProprietary_IP_Addr_Format_t format;
	ATCommon_IP_Addr_t addr;
} ATProprietary_Domain_Name_Resolve_Result_t;

typedef char ATProprietary_File_Name_t[64];

/**
 * @brief Credential Format
 */
typedef enum ATProprietary_Credential_Format_t
{
	ATProprietary_Credential_Format_Invalid = -1,
	ATProprietary_Credential_Format_Certificate,
	ATProprietary_Credential_Format_Private_Key,
	ATProprietary_Credential_Format_PSK_ID,
	ATProprietary_Credential_Format_PSK_Key,
	ATProprietary_Credential_Format_NumberOfValues
} ATProprietary_Credential_Format_t;

typedef uint8_t ATProprietary_File_Name_Count_t;

/**
 * @brief File Names List
 */
typedef struct ATProprietary_File_Names_List_t
{
	ATProprietary_File_Name_Count_t count;
	ATProprietary_File_Name_t *filenames;
} ATProprietary_File_Names_List_t;

typedef char ATProprietary_File_Path_t[128];

/**
 * @brief TLS Profile ID list
 */
typedef struct ATProprietary_TLS_Profile_ID_List_t
{
	ATCommon_TLS_Profile_ID_Count_t count;
	ATCommon_TLS_Profile_ID_t *profileIDs;
} ATProprietary_TLS_Profile_ID_List_t;

/**
 * @brief PDN Parameters
 */
typedef struct ATProprietary_PDN_Parameters_t
{
	ATCommon_Session_ID_t sessionID;
	ATCommon_APN_Name_t apnName;
	ATProprietary_IP_Addr_Format_t ipFormat;
} ATProprietary_PDN_Parameters_t;

extern bool ATProprietary_ReadNetworkAttachmentState(ATProprietary_Network_Attachment_State_t *stateP);

extern bool ATProprietary_SetNetworkAttachmentState(ATProprietary_Network_Attachment_State_t state);

extern bool ATProprietary_ReadRemainingPINPUKAttempts(ATProprietary_PIN_PUK_Attempts_t *attemptsP);

extern bool ATProprietary_SwitchToRATWithoutFullReboot(ATProprietary_RAT_t rat, ATProprietary_RAT_Storage_t storage, ATProprietary_RAT_Source_t source);

extern bool ATProprietary_ReadRATStatus(ATProprietary_RAT_Status_t *ratstatusP);

extern bool ATProprietary_SetBootDelay(ATProprietary_Boot_Delay_t delay);

extern bool ATProprietary_ResolveDomainName(ATCommon_Session_ID_t sessionid, ATProprietary_Domain_Name_t domain, ATProprietary_IP_Addr_Format_t format);

extern bool ATProprietary_ParseResolveDomainNameEvent(char *pEventArguments, ATProprietary_Domain_Name_Resolve_Result_t *dataP);

extern bool ATProprietary_Ping(ATProprietary_IP_Addr_Format_t format, ATCommon_IP_Addr_t destaddr, ATProprietary_Ping_Packet_Count_t packetcount, ATProprietary_Ping_Packet_Size_t packetsize, ATProprietary_Ping_Timeout_t timeout);

extern bool ATProprietary_ParsePingResultEvent(char *pEventArguments, ATProprietary_Ping_Result_t *dataP);

extern bool ATProprietary_ReadCredential(ATProprietary_File_Name_t filename, char *dataP, uint16_t dataMaxBufferSize);

extern bool ATProprietary_WriteCredential(ATProprietary_File_Name_t filename, ATProprietary_Credential_Format_t format, char *data);

extern bool ATProprietary_ListCredentials(ATProprietary_File_Names_List_t *filenamesList);

extern bool ATProprietary_DeleteCredential(ATProprietary_File_Name_t filename);

extern bool ATProprietary_ListTLSProfiles(ATProprietary_TLS_Profile_ID_List_t *profileIDsList);

extern bool ATProprietary_AddTLSProfile(ATCommon_TLS_Profile_ID_t profileID, ATProprietary_File_Name_t CA, ATProprietary_File_Path_t CAPath, ATProprietary_File_Name_t deviceCert, ATProprietary_File_Name_t deviceKey, ATProprietary_File_Name_t pskID, ATProprietary_File_Name_t pskKey);

extern bool ATProprietary_DeleteTLSProfile(ATCommon_TLS_Profile_ID_t profileID);

extern bool ATProprietary_SetPDNParameters(ATProprietary_PDN_Parameters_t parameters);

extern bool ATProprietary_ReadPDNParameters(ATProprietary_PDN_Parameters_t *parameters);

#ifdef __cplusplus
}
#endif

#endif /* AT_PROPRIETARY_H_INCLUDED */
