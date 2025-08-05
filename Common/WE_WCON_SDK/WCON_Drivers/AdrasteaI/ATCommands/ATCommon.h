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
 * @brief AT event definitions.
 */

#ifndef ADRASTEAI_AT_COMMON_H_INCLUDED
#define ADRASTEAI_AT_COMMON_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Supported Access Technologies
 */
    typedef enum AdrasteaI_ATCommon_AcT_t
    {
        AdrasteaI_ATCommon_AcT_Invalid = -1,
        AdrasteaI_ATCommon_AcT_GSM,
        AdrasteaI_ATCommon_AcT_GSM_Compact,
        AdrasteaI_ATCommon_AcT_UTRAN,
        AdrasteaI_ATCommon_AcT_GSM_EGPRS,
        AdrasteaI_ATCommon_AcT_UTRAN_HSDPA,
        AdrasteaI_ATCommon_AcT_UTRAN_HSUPA,
        AdrasteaI_ATCommon_AcT_UTRAN_HSDPA_HSUPA,
        AdrasteaI_ATCommon_AcT_E_UTRAN,
        AdrasteaI_ATCommon_AcT_E_UTRAN_NB_S1 = 9,
        AdrasteaI_ATCommon_AcT_NumberOfValues
    } AdrasteaI_ATCommon_AcT_t;

    /**
 * @brief IP Address
 */
    typedef char AdrasteaI_ATCommon_IP_Addr_t[128];

    /**
 * @brief Time
 */
    typedef struct AdrasteaI_ATCommon_Time_t
    {
        uint8_t Seconds;
        uint8_t Minutes;
        uint8_t Hours;
    } AdrasteaI_ATCommon_Time_t;

    /**
 * @brief Date
 */
    typedef struct AdrasteaI_ATCommon_Date_t
    {
        uint8_t Day;
        uint8_t Month;
        uint16_t Year;
    } AdrasteaI_ATCommon_Date_t;

    typedef double AdrasteaI_ATCommon_Latitude_t, AdrasteaI_ATCommon_Longitude_t, AdrasteaI_ATCommon_Altitude_t;

    typedef uint64_t AdrasteaI_ATCommon_UTC_Timestamp_t;

    /**
 * @brief Unsolicited Event States
 */
    typedef enum AdrasteaI_ATCommon_Event_State_t
    {
        AdrasteaI_ATCommon_Event_State_Invalid = -1,
        AdrasteaI_ATCommon_Event_State_Disable,
        AdrasteaI_ATCommon_Event_State_Enable,
        AdrasteaI_ATCommon_Event_State_NumberOfValues
    } AdrasteaI_ATCommon_Event_State_t;

    typedef char AdrasteaI_ATCommon_Auth_Username_t[64], AdrasteaI_ATCommon_Auth_Password_t[64];

    /**
 * @brief SSL/TLS Authentication Modes
 */
    typedef enum
    {
        AdrasteaI_ATCommon_Auth_Mode_Invalid = -1,
        AdrasteaI_ATCommon_Auth_Mode_Mutual,
        AdrasteaI_ATCommon_Auth_Mode_Client_Side_Only,
        AdrasteaI_ATCommon_Auth_Mode_Server_Side_Only,
        AdrasteaI_ATCommon_Auth_Mode_NumberOfValues
    } AdrasteaI_ATCommon_TLS_Auth_Mode_t,
        AdrasteaI_ATCommon_SSL_Auth_Mode_t;

    typedef uint8_t AdrasteaI_ATCommon_TLS_Profile_ID_t, AdrasteaI_ATCommon_SSL_Profile_ID_t, AdrasteaI_ATCommon_TLS_Profile_ID_Count_t;

    typedef uint16_t AdrasteaI_ATCommon_Port_Number_t;

#define AdrasteaI_ATCommon_Port_Number_Invalid 0

    typedef uint8_t AdrasteaI_ATCommon_Session_ID_t;

#define AdrasteaI_ATCommon_Session_ID_Invalid 0

    typedef char AdrasteaI_ATCommon_APN_Name_t[64];

#ifdef __cplusplus
}
#endif

#endif // ADRASTEAI_AT_COMMON_H_INCLUDED
