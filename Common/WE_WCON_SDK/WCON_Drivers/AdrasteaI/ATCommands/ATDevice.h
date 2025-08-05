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
 * @brief AT commands for basic device functionality.
 */

#ifndef ADRASTEAI_AT_DEVICE_H_INCLUDED
#define ADRASTEAI_AT_DEVICE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef char AdrasteaI_ATDevice_Manufacturer_Identity_t[64];

    typedef char AdrasteaI_ATDevice_Model_Identity_t[64];

    typedef uint8_t AdrasteaI_ATDevice_Revision_Identity_Major_t;

    typedef uint16_t AdrasteaI_ATDevice_Revision_Identity_Minor_t;

    /**
 * @brief Revision Version
 */
    typedef struct AdrasteaI_ATDevice_Revision_Identity_t
    {
        AdrasteaI_ATDevice_Revision_Identity_Major_t major;
        AdrasteaI_ATDevice_Revision_Identity_Minor_t minor;
    } AdrasteaI_ATDevice_Revision_Identity_t;

    typedef char AdrasteaI_ATDevice_Serial_Number_t[10];

    typedef char AdrasteaI_ATDevice_IMEI_t[16];

    typedef char AdrasteaI_ATDevice_IMEISV_t[17];

    typedef char AdrasteaI_ATDevice_SVN_t[3];

    /**
 * @brief Supported TE Character Sets
 */
    typedef enum AdrasteaI_ATDevice_Character_Set_t
    {
        AdrasteaI_ATDevice_Character_Set_Invalid = -1,
        AdrasteaI_ATDevice_Character_Set_UCS2,
        AdrasteaI_ATDevice_Character_Set_8859n,
        AdrasteaI_ATDevice_Character_Set_IRA,
        AdrasteaI_ATDevice_Character_Set_HEX,
        AdrasteaI_ATDevice_Character_Set_PCCP437,
        AdrasteaI_ATDevice_Character_Set_NumberOfValues
    } AdrasteaI_ATDevice_Character_Set_t;

    /**
 * @brief Phone Functionality Options
 */
    typedef enum AdrasteaI_ATDevice_Phone_Functionality_t
    {
        AdrasteaI_ATDevice_Phone_Functionality_Invalid = -1,
        AdrasteaI_ATDevice_Phone_Functionality_Min,
        AdrasteaI_ATDevice_Phone_Functionality_Full,
        AdrasteaI_ATDevice_Phone_Functionality_Disable_Transmit_Only,
        AdrasteaI_ATDevice_Phone_Functionality_Disable_Receive_Only,
        AdrasteaI_ATDevice_Phone_Functionality_Disable_Transmit_Receive,
        AdrasteaI_ATDevice_Phone_Functionality_NumberOfValues
    } AdrasteaI_ATDevice_Phone_Functionality_t;

    /**
 * @brief Reset type when changing phone functionality
 */
    typedef enum AdrasteaI_ATDevice_Phone_Functionality_Reset_t
    {
        AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid = -1,
        AdrasteaI_ATDevice_Phone_Functionality_Reset_Do_Not_Reset,
        AdrasteaI_ATDevice_Phone_Functionality_Reset_Reset,
        AdrasteaI_ATDevice_Phone_Functionality_Reset_NumberOfValues
    } AdrasteaI_ATDevice_Phone_Functionality_Reset_t;

    /**
 * @brief Result code formats
 */
    typedef enum AdrasteaI_ATDevice_Result_Code_Format_t
    {
        AdrasteaI_ATDevice_Result_Code_Format_Invalid = -1,
        AdrasteaI_ATDevice_Result_Code_Format_Numeric,
        AdrasteaI_ATDevice_Result_Code_Format_Verbose,
        AdrasteaI_ATDevice_Result_Code_Format_NumberOfValues
    } AdrasteaI_ATDevice_Result_Code_Format_t;

    extern bool AdrasteaI_ATDevice_Test();

    extern bool AdrasteaI_ATDevice_RequestManufacturerIdentity(AdrasteaI_ATDevice_Manufacturer_Identity_t* manufacturerIdentityP);

    extern bool AdrasteaI_ATDevice_RequestModelIdentity(AdrasteaI_ATDevice_Model_Identity_t* modelIdentityP);

    extern bool AdrasteaI_ATDevice_RequestRevisionIdentity(AdrasteaI_ATDevice_Revision_Identity_t* revisionIdentityP);

    extern bool AdrasteaI_ATDevice_RequestIMEI(AdrasteaI_ATDevice_IMEI_t* imeiP);

    extern bool AdrasteaI_ATDevice_RequestIMEISV(AdrasteaI_ATDevice_IMEISV_t* imeisvP);

    extern bool AdrasteaI_ATDevice_RequestSVN(AdrasteaI_ATDevice_SVN_t* svnP);

    extern bool AdrasteaI_ATDevice_RequestSerialNumber(AdrasteaI_ATDevice_Serial_Number_t* serialNumberP);

    extern bool AdrasteaI_ATDevice_GetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t* charsetP);

    extern bool AdrasteaI_ATDevice_SetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t charset);

    extern bool AdrasteaI_ATDevice_GetCapabilitiesList(char* capListP, uint8_t maxBufferSize);

    extern bool AdrasteaI_ATDevice_GetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t* phoneFunP);

    extern bool AdrasteaI_ATDevice_SetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t phoneFun, AdrasteaI_ATDevice_Phone_Functionality_Reset_t resetType);

    extern bool AdrasteaI_ATDevice_Reset();

    extern bool AdrasteaI_ATDevice_FactoryReset();

    extern bool AdrasteaI_ATDevice_SetResultCodeFormat(AdrasteaI_ATDevice_Result_Code_Format_t format);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_DEVICE_H_INCLUDED */
