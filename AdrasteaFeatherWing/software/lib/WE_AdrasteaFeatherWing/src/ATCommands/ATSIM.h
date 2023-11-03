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
 * @brief AT commands for SIM functionality.
 */

#ifndef AT_SIM_H_INCLUDED
#define AT_SIM_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char ATSIM_IMSI_t[16];

/**
 * @brief Facility Lock
 */
typedef enum ATSIM_Facility_t
{
	ATSIM_Facility_Invalid = -1,
	ATSIM_Facility_SC,
	ATSIM_Facility_P2,
	ATSIM_Facility_PN,
	ATSIM_Facility_PU,
	ATSIM_Facility_PS,
	ATSIM_Facility_NumberOfValues
} ATSIM_Facility_t;

/**
 * @brief Facility Lock Mode
 */
typedef enum ATSIM_Lock_Mode_t
{
	ATSIM_Lock_Mode_Invalid = -1,
	ATSIM_Lock_Mode_Unlock,
	ATSIM_Lock_Mode_Lock,
	ATSIM_Lock_Mode_NumberOfValues
} ATSIM_Lock_Mode_t;

/**
 * @brief Facility Lock Status
 */
typedef enum ATSIM_Lock_Status_t
{
	ATSIM_Lock_Status_Invalid = -1,
	ATSIM_Lock_Status_Not_Active,
	ATSIM_Lock_Status_Active,
	ATSIM_Lock_Status_NumberOfValues
} ATSIM_Lock_Status_t;

typedef char ATSIM_PIN_t[32];

/**
 * @brief PIN Status
 */
typedef enum ATSIM_PIN_Status_t
{
	ATSIM_PIN_Status_Invalid = -1,
	ATSIM_PIN_Status_Ready,
	ATSIM_PIN_Status_SIM_PIN,
	ATSIM_PIN_Status_SIM_PUK,
	ATSIM_PIN_Status_PH_SIM_PIN,
	ATSIM_PIN_Status_PH_FSIM_PIN,
	ATSIM_PIN_Status_PH_FSIM_PUK,
	ATSIM_PIN_Status_SIM_PIN2,
	ATSIM_PIN_Status_SIM_PUK2,
	ATSIM_PIN_Status_PH_NET_PIN,
	ATSIM_PIN_Status_PH_NET_PUK,
	ATSIM_PIN_Status_PH_NETSUB_PIN,
	ATSIM_PIN_Status_PH_NETSUB_PUK,
	ATSIM_PIN_Status_PH_SP_PIN,
	ATSIM_PIN_Status_PH_SP_PUK,
	ATSIM_PIN_Status_PH_CORP_PIN,
	ATSIM_PIN_Status_PH_CORP_PUK,
	ATSIM_PIN_Status_NumberOfValues
} ATSIM_PIN_Status_t;

/**
 * @brief ICCID
 */
typedef char ATSIM_ICCID_t[24];

/**
 * @brief SIM Restricted Access Commands
 */
typedef enum ATSIM_Restricted_Access_Command_t
{
	ATSIM_Restricted_Access_Command_Invalid = -1,
	ATSIM_Restricted_Access_Command_Read_Binary = 176,
	ATSIM_Restricted_Access_Command_Read_Record = 178,
	ATSIM_Restricted_Access_Command_Get_Response = 192,
	ATSIM_Restricted_Access_Command_Update_Binary = 214,
	ATSIM_Restricted_Access_Command_Update_Record = 220,
	ATSIM_Restricted_Access_Command_Status = 242,
	ATSIM_Restricted_Access_Command_Retrieve_Data = 203,
	ATSIM_Restricted_Access_Command_Set_Data = 219,
	ATSIM_Restricted_Access_Command_NumberOfValues
} ATSIM_Restricted_Access_Command_t;

typedef uint16_t ATSIM_Restricted_Access_File_ID;

typedef uint8_t ATSIM_Restricted_Access_P1, ATSIM_Restricted_Access_P2, ATSIM_Restricted_Access_P3;

typedef uint8_t ATSIM_Restricted_Access_SW1, ATSIM_Restricted_Access_SW2;

/**
 * @brief SIM Restricted Access Response
 */
typedef struct ATSIM_Restricted_Access_Response_t
{
	ATSIM_Restricted_Access_SW1 sw1;
	ATSIM_Restricted_Access_SW2 sw2;
	char *responseRead;
	uint16_t responseReadMaxBufferSize; //Buffer size need to be specified before passing to function
} ATSIM_Restricted_Access_Response_t;

typedef char ATSIM_Number_t[64];

/**
 * @brief SIM Number Type
 */
typedef enum ATSIM_Number_Type_t
{
	ATSIM_Number_Type_Invalid = -1,
	ATSIM_Number_Type_National_Number = 129,
	ATSIM_Number_Type_International_Number = 145,
	ATSIM_Number_Type_NumberOfValues
} ATSIM_Number_Type_t;

/**
 * @brief Subscriber Number
 */
typedef struct ATSIM_Subscriber_Number_t
{
	ATSIM_Number_t number;
	ATSIM_Number_Type_t numberType;
} ATSIM_Subscriber_Number_t;

extern bool ATSIM_RequestInternationalMobileSubscriberIdentity(ATSIM_IMSI_t *imsiP);

extern bool ATSIM_SetFacilityLock(ATSIM_Facility_t facility, ATSIM_Lock_Mode_t mode, ATSIM_PIN_t pin);

extern bool ATSIM_ReadFacilityLock(ATSIM_Facility_t facility, ATSIM_Lock_Status_t *statusP);

extern bool ATSIM_ReadSubscriberNumber();

extern bool ATSIM_ReadPinStatus(ATSIM_PIN_Status_t *statusP);

extern bool ATSIM_EnterPin(ATSIM_PIN_t pin1, ATSIM_PIN_t pin2);

extern bool ATSIM_ChangePassword(ATSIM_Facility_t facility, ATSIM_PIN_t oldpassword, ATSIM_PIN_t newpassword);

extern bool ATSIM_RestrictedSIMAccess(ATSIM_Restricted_Access_Command_t cmd, ATSIM_Restricted_Access_File_ID fileID, ATSIM_Restricted_Access_P1 p1, ATSIM_Restricted_Access_P2 p2, ATSIM_Restricted_Access_P3 p3, char *dataWritten, ATSIM_Restricted_Access_Response_t *cmdResponse);

extern bool ATSIM_RequestIntegratedCircuitCardIdentifier(ATSIM_ICCID_t *iccidP);

extern bool ATSMS_ParseSubscriberNumberEvent(char *pEventArguments, ATSIM_Subscriber_Number_t *dataP);

#ifdef __cplusplus
}
#endif

#endif /* AT_SIM_H_INCLUDED */
