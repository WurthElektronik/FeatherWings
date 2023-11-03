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
 * @brief AT event definitions.
 */

#ifndef AT_COMMON_H_INCLUDED
#define AT_COMMON_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported Access Technologies
 */
typedef enum ATCommon_AcT_t
{
	ATCommon_AcT_Invalid = -1,
	ATCommon_AcT_GSM,
	ATCommon_AcT_GSM_Compact,
	ATCommon_AcT_UTRAN,
	ATCommon_AcT_GSM_EGPRS,
	ATCommon_AcT_UTRAN_HSDPA,
	ATCommon_AcT_UTRAN_HSUPA,
	ATCommon_AcT_UTRAN_HSDPA_HSUPA,
	ATCommon_AcT_E_UTRAN,
	ATCommon_AcT_E_UTRAN_NB_S1 = 9,
	ATCommon_AcT_NumberOfValues
} ATCommon_AcT_t;

/**
 * @brief IP Address
 */
typedef char ATCommon_IP_Addr_t[128];

/**
 * @brief Time
 */
typedef struct ATCommon_Time_t
{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
} ATCommon_Time_t;

/**
 * @brief Date
 */
typedef struct ATCommon_Date_t
{
	uint8_t Day;
	uint8_t Month;
	uint16_t Year;
} ATCommon_Date_t;

typedef double ATCommon_Latitude_t, ATCommon_Longitude_t, ATCommon_Altitude_t;

typedef uint64_t ATCommon_UTC_Timestamp_t;

/**
 * @brief Unsolicited Event States
 */
typedef enum ATCommon_Event_State_t
{
	ATCommon_Event_State_Invalid = -1,
	ATCommon_Event_State_Disable,
	ATCommon_Event_State_Enable,
	ATCommon_Event_State_NumberOfValues
} ATCommon_Event_State_t;

typedef char ATCommon_Auth_Username_t[64], ATCommon_Auth_Password_t[64];

/**
 * @brief SSL/TLS Authentication Modes
 */
typedef enum
{
	ATCommon_Auth_Mode_Invalid = -1,
	ATCommon_Auth_Mode_Mutual,
	ATCommon_Auth_Mode_Client_Side_Only,
	ATCommon_Auth_Mode_Server_Side_Only,
	ATCommon_Auth_Mode_NumberOfValues
} ATCommon_TLS_Auth_Mode_t, ATCommon_SSL_Auth_Mode_t;

typedef uint8_t ATCommon_TLS_Profile_ID_t, ATCommon_SSL_Profile_ID_t, ATCommon_TLS_Profile_ID_Count_t;

typedef uint16_t ATCommon_Port_Number_t;

#define ATCommon_Port_Number_Invalid 0

typedef uint8_t ATCommon_Session_ID_t;

#define ATCommon_Session_ID_Invalid 0

typedef char ATCommon_APN_Name_t[64];

#ifdef __cplusplus
}
#endif

#endif // AT_COMMON_H_INCLUDED
