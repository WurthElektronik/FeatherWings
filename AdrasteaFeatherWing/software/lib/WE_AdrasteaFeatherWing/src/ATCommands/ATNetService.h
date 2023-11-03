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
 * @brief AT commands for Net Service functionality.
 */

#ifndef AT_NETSERVICE_H_INCLUDED
#define AT_NETSERVICE_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t ATNetService_Operator_Numeric_t;

typedef char ATNetService_Operator_String_t[64];

/**
 * @brief Operator Numeric and Alpha
 */
typedef struct ATNetService_Operator_t
{
	ATNetService_Operator_Numeric_t operatorNumeric;
	ATNetService_Operator_String_t operatorString;
} ATNetService_Operator_t;

/**
 * @brief Public Land Mobile Network Selection Mode
 */
typedef enum ATNetService_PLMN_Selection_Mode_t
{
	ATNetService_PLMN_Selection_Mode_Automatic = 0,
	ATNetService_PLMN_Selection_Mode_Manual,
	ATNetService_PLMN_Selection_Mode_Deregister,
	ATNetService_PLMN_Selection_Mode_NumberOfValues
} ATNetService_PLMN_Selection_Mode_t;

/**
 * @brief Public Land Mobile Network Format
 */
typedef enum ATNetService_PLMN_Format_t
{
	ATNetService_PLMN_Format_Invalid = -1,
	ATNetService_PLMN_Format_Long_AlphaNumeric = 0,
	ATNetService_PLMN_Format_Short_AlphaNumeric,
	ATNetService_PLMN_Format_Numeric,
	ATNetService_PLMN_Format_NumberOfValues
} ATNetService_PLMN_Format_t;

/**
 * @brief Public Land Mobile Network
 */
typedef struct ATNetService_PLMN_t
{
	ATNetService_PLMN_Selection_Mode_t selectionMode;
	ATNetService_PLMN_Format_t format;
	ATNetService_Operator_t operator;
	ATCommon_AcT_t accessTechnology;
} ATNetService_PLMN_t;

typedef uint8_t ATNetService_Signal_Quality_RSSI_t;
typedef uint8_t ATNetService_Signal_Quality_Bit_Error_Rate_t;

/**
 * @brief Signal Quality
 */
typedef struct ATNetService_Signal_Quality_t
{
	ATNetService_Signal_Quality_RSSI_t rssi;
	ATNetService_Signal_Quality_Bit_Error_Rate_t ber;
} ATNetService_Signal_Quality_t;

typedef uint8_t ATNetService_Extended_Signal_Quality_rxlev_t;
typedef uint8_t ATNetService_Extended_Signal_Quality_rscp_t;
typedef uint8_t ATNetService_Extended_Signal_Quality_ecno_t;
typedef uint8_t ATNetService_Extended_Signal_Quality_rsrq_t;
typedef uint8_t ATNetService_Extended_Signal_Quality_rsrp_t;

/**
 * @brief Extended Signal Quality
 */
typedef struct ATNetService_Extended_Signal_Quality_t
{
	ATNetService_Extended_Signal_Quality_rxlev_t rxlev;
	ATNetService_Signal_Quality_Bit_Error_Rate_t ber;
	ATNetService_Extended_Signal_Quality_rscp_t rscp;
	ATNetService_Extended_Signal_Quality_ecno_t ecno;
	ATNetService_Extended_Signal_Quality_rsrq_t rsrq;
	ATNetService_Extended_Signal_Quality_rsrp_t rsrp;
} ATNetService_Extended_Signal_Quality_t;

/**
 * @brief Power Saving Mode
 */
typedef enum ATNetService_Power_Saving_Mode_State_t
{
	ATNetService_Power_Saving_Mode_State_Disable,
	ATNetService_Power_Saving_Mode_State_Enable,
	ATNetService_Power_Saving_Mode_State_NumberOfValues
} ATNetService_Power_Saving_Mode_State_t;

typedef uint8_t ATNetService_Power_Saving_Mode_Periodic_TAU_Value_t;

/**
 * @brief Power Saving Mode Periodic TAU Units
 */
typedef enum ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t
{
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_10m,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_1h,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_10h,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_2s,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_30s,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_1m,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_320h,
	ATNetService_Power_Saving_Mode_Periodic_TAU_Deactivated,
} ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t;

typedef uint8_t ATNetService_Power_Saving_Mode_Active_Time_Value_t;

/**
 * @brief Power Saving Mode Active Time Units
 */
typedef enum ATNetService_Power_Saving_Mode_Active_Time_Unit_t
{
	ATNetService_Power_Saving_Mode_Active_Time_Unit_2s,
	ATNetService_Power_Saving_Mode_Active_Time_Unit_1m,
	ATNetService_Power_Saving_Mode_Active_Time_Unit_Decih,
	ATNetService_Power_Saving_Mode_Active_Time_Deactivated = 7,
} ATNetService_Power_Saving_Mode_Active_Time_Unit_t;

/**
 * @brief Power Saving Mode Periodic TAU
 */
typedef union ATNetService_Power_Saving_Mode_Periodic_TAU_t
{
	uint8_t periodicTAU;
	struct
	{
		ATNetService_Power_Saving_Mode_Periodic_TAU_Value_t value :5;
		ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t unit :3;
	} periodicTAUValues;
} ATNetService_Power_Saving_Mode_Periodic_TAU_t;

/**
 * @brief Power Saving Mode Active Time
 */
typedef union ATNetService_Power_Saving_Mode_Active_Time_t
{
	uint8_t activeTime;
	struct
	{
		ATNetService_Power_Saving_Mode_Active_Time_Value_t value :5;
		ATNetService_Power_Saving_Mode_Active_Time_Unit_t unit :3;
	} activeTimeValues;
} ATNetService_Power_Saving_Mode_Active_Time_t;

/**
 * @brief Power Saving Mode
 */
typedef struct ATNetService_Power_Saving_Mode_t
{
	ATNetService_Power_Saving_Mode_State_t state;
	ATNetService_Power_Saving_Mode_Periodic_TAU_t periodicTAU;
	ATNetService_Power_Saving_Mode_Active_Time_t activeTime;
} ATNetService_Power_Saving_Mode_t;

/**
 * @brief eDRX Access Technology
 */
typedef enum ATNetService_eDRX_AcT_t
{
	ATNetService_eDRX_AcT_Invalid = -1,
	ATNetService_eDRX_AcT_NotUsingeDRX = 0,
	ATNetService_eDRX_AcT_EC_GSM_IoT_AGb = 1,
	ATNetService_eDRX_AcT_GSM_AGb = 2,
	ATNetService_eDRX_AcT_UTRAN_LU = 3,
	ATNetService_eDRX_AcT_E_UTRAN_WB_S1 = 4,
	ATNetService_eDRX_AcT_E_UTRAN_NB_S1 = 5,
	ATNetService_eDRX_AcT_NumberOfValues
} ATNetService_eDRX_AcT_t;

/**
 * @brief eDRX Value
 */
typedef enum ATNetService_eDRX_Value_t
{
	ATNetService_eDRX_Value_Invalid = -1,
	ATNetService_eDRX_Value_5s_120ms,
	ATNetService_eDRX_Value_10s_240ms,
	ATNetService_eDRX_Value_20s_480ms,
	ATNetService_eDRX_Value_40s_960ms,
	ATNetService_eDRX_Value_61s_440ms,
	ATNetService_eDRX_Value_81s_920ms,
	ATNetService_eDRX_Value_102s_400ms,
	ATNetService_eDRX_Value_122s_880ms,
	ATNetService_eDRX_Value_143s_360ms,
	ATNetService_eDRX_Value_163s_840ms,
	ATNetService_eDRX_Value_327s_680ms,
	ATNetService_eDRX_Value_655s_360ms,
	ATNetService_eDRX_Value_1310s_720ms,
	ATNetService_eDRX_Value_2621s_440ms,
	ATNetService_eDRX_Value_5242s_880ms,
	ATNetService_eDRX_Value_10485s_760ms,
	ATNetService_eDRX_Value_NumberOfValues
} ATNetService_eDRX_Value_t;

/**
 * @brief eDRX Mode
 */
typedef enum ATNetService_eDRX_Mode_t
{
	ATNetService_eDRX_Mode_Disable,
	ATNetService_eDRX_Mode_Enable,
	ATNetService_eDRX_Mode_Disable_Reset,
	ATNetService_eDRX_Mode_NumberOfValues
} ATNetService_eDRX_Mode_t;

typedef uint8_t ATNetService_eDRX_Paging_Time_Window_t;

/**
 * @brief eDRX
 */
typedef struct ATNetService_eDRX_t
{
	ATNetService_eDRX_AcT_t AcT;
	ATNetService_eDRX_Value_t requestedValue;
	ATNetService_eDRX_Value_t networkProvidedValue;
	ATNetService_eDRX_Paging_Time_Window_t pagingTime;
} ATNetService_eDRX_t;

/**
 * @brief Coverage Enhancement Access Technology
 */
typedef enum ATNetService_CES_AcT_t
{
	ATNetService_CES_AcT_Invalid = -1,
	ATNetService_CES_AcT_No_Enhancement,
	ATNetService_CES_AcT_E_UTRAN,
	ATNetService_CES_AcT_EC_GSM_IoT_AGb,
	ATNetService_CES_AcT_E_UTRAN_NB_S1,
	ATNetService_CES_AcT_NumberOfValues
} ATNetService_CES_AcT_t;

/**
 * @brief Coverage Enhancement Level
 */
typedef enum ATNetService_CES_CEL_t
{
	ATNetService_CES_CEL_Invalid = -1,
	ATNetService_CES_CEL_No_Enhancement,
	ATNetService_CES_CEL_Level0,
	ATNetService_CES_CEL_Level1,
	ATNetService_CES_CEL_Level2,
	ATNetService_CES_CEL_Level3,
	ATNetService_CES_CEL_NumberOfValues
} ATNetService_CES_CEL_t;

/**
 * @brief Coverage Enhancement Class
 */
typedef enum ATNetService_CES_CC_t
{
	ATNetService_CES_CC_Invalid = -1,
	ATNetService_CES_CC_No_Coverage_Class,
	ATNetService_CES_CC_Coverage_Class1,
	ATNetService_CES_CC_Coverage_Class2,
	ATNetService_CES_CC_Coverage_Class3,
	ATNetService_CES_CC_Coverage_Class4,
	ATNetService_CES_CC_Coverage_Class5,
	ATNetService_CES_CC_NumberOfValues
} ATNetService_CES_CC_t;

/**
 * @brief Coverage Enhancement
 */
typedef struct ATNetService_CES_t
{
	ATNetService_CES_AcT_t AcT;
	ATNetService_CES_CEL_t coverageEnhacementLevel;
	ATNetService_CES_CC_t coverageClass;
} ATNetService_CES_t;

extern bool ATNetService_ReadOperators();

extern bool ATNetService_ParseOperatorReadEvent(char *pEventArguments, ATNetService_Operator_t *dataP);

extern bool ATNetService_SetPLMNReadFormat(ATNetService_PLMN_Format_t format);

extern bool ATNetService_ReadPLMN(ATNetService_PLMN_t *plmnP);

extern bool ATNetService_SetPLMN(ATNetService_PLMN_t plmn);

extern bool ATNetService_ReadSignalQuality(ATNetService_Signal_Quality_t *sq);

extern bool ATNetService_ReadExtendedSignalQuality(ATNetService_Extended_Signal_Quality_t *esq);

extern bool ATNetService_ReadPowerSavingMode(ATNetService_Power_Saving_Mode_t *psmP);

extern bool ATNetService_SetPowerSavingMode(ATNetService_Power_Saving_Mode_t psm);

extern bool ATNetService_SeteDRXDynamicParameters(ATNetService_eDRX_Mode_t mode, ATNetService_eDRX_t edrx);

extern bool ATNetService_ReadeDRXDynamicParameters(ATNetService_eDRX_t *edrxP);

extern bool ATNetService_ReadCoverageEnhancementStatus(ATNetService_CES_t *cesP);

#ifdef __cplusplus
}
#endif

#endif /* AT_NETSERVICE_H_INCLUDED */
