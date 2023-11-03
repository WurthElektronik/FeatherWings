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
 * @brief AT commands for basic device functionality.
 */

#ifndef AT_DEVICE_H_INCLUDED
#define AT_DEVICE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char ATDevice_Manufacturer_Identity_t[64];

typedef char ATDevice_Model_Identity_t[64];

typedef uint8_t ATDevice_Revision_Identity_Major_t;

typedef uint16_t ATDevice_Revision_Identity_Minor_t;

/**
 * @brief Revision Version
 */
typedef struct ATDevice_Revision_Identity_t
{
	ATDevice_Revision_Identity_Major_t major;
	ATDevice_Revision_Identity_Minor_t minor;
} ATDevice_Revision_Identity_t;

typedef char ATDevice_Serial_Number_t[10];

typedef char ATDevice_IMEI_t[16];

typedef char ATDevice_IMEISV_t[17];

typedef char ATDevice_SVN_t[3];

/**
 * @brief Supported TE Character Sets
 */
typedef enum ATDevice_Character_Set_t
{
	ATDevice_Character_Set_Invalid = -1,
	ATDevice_Character_Set_UCS2,
	ATDevice_Character_Set_8859n,
	ATDevice_Character_Set_IRA,
	ATDevice_Character_Set_HEX,
	ATDevice_Character_Set_PCCP437,
	ATDevice_Character_Set_NumberOfValues
} ATDevice_Character_Set_t;

/**
 * @brief Phone Functionality Options
 */
typedef enum ATDevice_Phone_Functionality_t
{
	ATDevice_Phone_Functionality_Invalid = -1,
	ATDevice_Phone_Functionality_Min,
	ATDevice_Phone_Functionality_Full,
	ATDevice_Phone_Functionality_Disable_Transmit_Only,
	ATDevice_Phone_Functionality_Disable_Receive_Only,
	ATDevice_Phone_Functionality_Disable_Transmit_Receive,
	ATDevice_Phone_Functionality_NumberOfValues
} ATDevice_Phone_Functionality_t;

/**
 * @brief Reset type when changing phone functionality
 */
typedef enum ATDevice_Phone_Functionality_Reset_t
{
	ATDevice_Phone_Functionality_Reset_Invalid = -1,
	ATDevice_Phone_Functionality_Reset_Do_Not_Reset,
	ATDevice_Phone_Functionality_Reset_Reset,
	ATDevice_Phone_Functionality_Reset_NumberOfValues
} ATDevice_Phone_Functionality_Reset_t;

/**
 * @brief Result code formats
 */
typedef enum ATDevice_Result_Code_Format_t
{
	ATDevice_Result_Code_Format_Invalid = -1,
	ATDevice_Result_Code_Format_Numeric,
	ATDevice_Result_Code_Format_Verbose,
	ATDevice_Result_Code_Format_NumberOfValues
} ATDevice_Result_Code_Format_t;

extern bool ATDevice_Test();

extern bool ATDevice_RequestManufacturerIdentity(ATDevice_Manufacturer_Identity_t *manufacturerIdentityP);

extern bool ATDevice_RequestModelIdentity(ATDevice_Model_Identity_t *modelIdentityP);

extern bool ATDevice_RequestRevisionIdentity(ATDevice_Revision_Identity_t *revisionIdentityP);

extern bool ATDevice_RequestIMEI(ATDevice_IMEI_t *imeiP);

extern bool ATDevice_RequestIMEISV(ATDevice_IMEISV_t *imeisvP);

extern bool ATDevice_RequestSVN(ATDevice_SVN_t *svnP);

extern bool ATDevice_RequestSerialNumber(ATDevice_Serial_Number_t *serialNumberP);

extern bool ATDevice_GetTECharacterSet(ATDevice_Character_Set_t *charsetP);

extern bool ATDevice_SetTECharacterSet(ATDevice_Character_Set_t charset);

extern bool ATDevice_GetCapabilitiesList(char *capListP, uint8_t maxBufferSize);

extern bool ATDevice_GetPhoneFunctionality(ATDevice_Phone_Functionality_t *phoneFunP);

extern bool ATDevice_SetPhoneFunctionality(ATDevice_Phone_Functionality_t phoneFun, ATDevice_Phone_Functionality_Reset_t resetType);

extern bool ATDevice_Reset();

extern bool ATDevice_FactoryReset();

extern bool ATDevice_SetResultCodeFormat(ATDevice_Result_Code_Format_t format);

#ifdef __cplusplus
}
#endif

#endif /* AT_DEVICE_H_INCLUDED */
