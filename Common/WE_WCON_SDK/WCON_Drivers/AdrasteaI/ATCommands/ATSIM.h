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
 * @brief AT commands for SIM functionality.
 */

#ifndef ADRASTEAI_AT_SIM_H_INCLUDED
#define ADRASTEAI_AT_SIM_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef char AdrasteaI_ATSIM_IMSI_t[16];

    /**
 * @brief Facility Lock
 */
    typedef enum AdrasteaI_ATSIM_Facility_t
    {
        AdrasteaI_ATSIM_Facility_Invalid = -1,
        AdrasteaI_ATSIM_Facility_SC,
        AdrasteaI_ATSIM_Facility_P2,
        AdrasteaI_ATSIM_Facility_PN,
        AdrasteaI_ATSIM_Facility_PU,
        AdrasteaI_ATSIM_Facility_PS,
        AdrasteaI_ATSIM_Facility_NumberOfValues
    } AdrasteaI_ATSIM_Facility_t;

    /**
 * @brief Facility Lock Mode
 */
    typedef enum AdrasteaI_ATSIM_Lock_Mode_t
    {
        AdrasteaI_ATSIM_Lock_Mode_Invalid = -1,
        AdrasteaI_ATSIM_Lock_Mode_Unlock,
        AdrasteaI_ATSIM_Lock_Mode_Lock,
        AdrasteaI_ATSIM_Lock_Mode_NumberOfValues
    } AdrasteaI_ATSIM_Lock_Mode_t;

    /**
 * @brief Facility Lock Status
 */
    typedef enum AdrasteaI_ATSIM_Lock_Status_t
    {
        AdrasteaI_ATSIM_Lock_Status_Invalid = -1,
        AdrasteaI_ATSIM_Lock_Status_Not_Active,
        AdrasteaI_ATSIM_Lock_Status_Active,
        AdrasteaI_ATSIM_Lock_Status_NumberOfValues
    } AdrasteaI_ATSIM_Lock_Status_t;

    typedef char AdrasteaI_ATSIM_PIN_t[32];

    /**
 * @brief PIN Status
 */
    typedef enum AdrasteaI_ATSIM_PIN_Status_t
    {
        AdrasteaI_ATSIM_PIN_Status_Invalid = -1,
        AdrasteaI_ATSIM_PIN_Status_Ready,
        AdrasteaI_ATSIM_PIN_Status_SIM_PIN,
        AdrasteaI_ATSIM_PIN_Status_SIM_PUK,
        AdrasteaI_ATSIM_PIN_Status_PH_SIM_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_FSIM_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_FSIM_PUK,
        AdrasteaI_ATSIM_PIN_Status_SIM_PIN2,
        AdrasteaI_ATSIM_PIN_Status_SIM_PUK2,
        AdrasteaI_ATSIM_PIN_Status_PH_NET_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_NET_PUK,
        AdrasteaI_ATSIM_PIN_Status_PH_NETSUB_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_NETSUB_PUK,
        AdrasteaI_ATSIM_PIN_Status_PH_SP_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_SP_PUK,
        AdrasteaI_ATSIM_PIN_Status_PH_CORP_PIN,
        AdrasteaI_ATSIM_PIN_Status_PH_CORP_PUK,
        AdrasteaI_ATSIM_PIN_Status_NumberOfValues
    } AdrasteaI_ATSIM_PIN_Status_t;

    /**
 * @brief ICCID
 */
    typedef char AdrasteaI_ATSIM_ICCID_t[24];

    /**
 * @brief SIM Restricted Access Commands
 */
    typedef enum AdrasteaI_ATSIM_Restricted_Access_Command_t
    {
        AdrasteaI_ATSIM_Restricted_Access_Command_Invalid = -1,
        AdrasteaI_ATSIM_Restricted_Access_Command_Read_Binary = 176,
        AdrasteaI_ATSIM_Restricted_Access_Command_Read_Record = 178,
        AdrasteaI_ATSIM_Restricted_Access_Command_Get_Response = 192,
        AdrasteaI_ATSIM_Restricted_Access_Command_Update_Binary = 214,
        AdrasteaI_ATSIM_Restricted_Access_Command_Update_Record = 220,
        AdrasteaI_ATSIM_Restricted_Access_Command_Status = 242,
        AdrasteaI_ATSIM_Restricted_Access_Command_Retrieve_Data = 203,
        AdrasteaI_ATSIM_Restricted_Access_Command_Set_Data = 219,
        AdrasteaI_ATSIM_Restricted_Access_Command_NumberOfValues
    } AdrasteaI_ATSIM_Restricted_Access_Command_t;

    typedef uint16_t AdrasteaI_ATSIM_Restricted_Access_File_ID;

    typedef uint8_t AdrasteaI_ATSIM_Restricted_Access_P1, AdrasteaI_ATSIM_Restricted_Access_P2, AdrasteaI_ATSIM_Restricted_Access_P3;

    typedef uint8_t AdrasteaI_ATSIM_Restricted_Access_SW1, AdrasteaI_ATSIM_Restricted_Access_SW2;

    /**
 * @brief SIM Restricted Access Response
 */
    typedef struct AdrasteaI_ATSIM_Restricted_Access_Response_t
    {
        AdrasteaI_ATSIM_Restricted_Access_SW1 sw1;
        AdrasteaI_ATSIM_Restricted_Access_SW2 sw2;
        char* responseRead;
        uint16_t responseReadMaxBufferSize; //Buffer size need to be specified before passing to function
    } AdrasteaI_ATSIM_Restricted_Access_Response_t;

    typedef char AdrasteaI_ATSIM_Number_t[64];

    /**
 * @brief SIM Number Type
 */
    typedef enum AdrasteaI_ATSIM_Number_Type_t
    {
        AdrasteaI_ATSIM_Number_Type_Invalid = -1,
        AdrasteaI_ATSIM_Number_Type_National_Number = 129,
        AdrasteaI_ATSIM_Number_Type_International_Number = 145,
        AdrasteaI_ATSIM_Number_Type_NumberOfValues
    } AdrasteaI_ATSIM_Number_Type_t;

    /**
 * @brief Subscriber Number
 */
    typedef struct AdrasteaI_ATSIM_Subscriber_Number_t
    {
        AdrasteaI_ATSIM_Number_t number;
        AdrasteaI_ATSIM_Number_Type_t numberType;
    } AdrasteaI_ATSIM_Subscriber_Number_t;

    extern bool AdrasteaI_ATSIM_RequestInternationalMobileSubscriberIdentity(AdrasteaI_ATSIM_IMSI_t* imsiP);

    extern bool AdrasteaI_ATSIM_SetFacilityLock(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_Lock_Mode_t mode, AdrasteaI_ATSIM_PIN_t pin);

    extern bool AdrasteaI_ATSIM_ReadFacilityLock(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_Lock_Status_t* statusP);

    extern bool AdrasteaI_ATSIM_ReadSubscriberNumber();

    extern bool AdrasteaI_ATSIM_ReadPinStatus(AdrasteaI_ATSIM_PIN_Status_t* statusP);

    extern bool AdrasteaI_ATSIM_EnterPin(AdrasteaI_ATSIM_PIN_t pin1, AdrasteaI_ATSIM_PIN_t pin2);

    extern bool AdrasteaI_ATSIM_ChangePassword(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_PIN_t oldpassword, AdrasteaI_ATSIM_PIN_t newpassword);

    extern bool AdrasteaI_ATSIM_RestrictedSIMAccess(AdrasteaI_ATSIM_Restricted_Access_Command_t cmd, AdrasteaI_ATSIM_Restricted_Access_File_ID fileID, AdrasteaI_ATSIM_Restricted_Access_P1 p1, AdrasteaI_ATSIM_Restricted_Access_P2 p2, AdrasteaI_ATSIM_Restricted_Access_P3 p3, char* dataWritten, AdrasteaI_ATSIM_Restricted_Access_Response_t* cmdResponse);

    extern bool AdrasteaI_ATSIM_RequestIntegratedCircuitCardIdentifier(AdrasteaI_ATSIM_ICCID_t* iccidP);

    extern bool AdrasteaI_ATSMS_ParseSubscriberNumberEvent(char* pEventArguments, AdrasteaI_ATSIM_Subscriber_Number_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_SIM_H_INCLUDED */
