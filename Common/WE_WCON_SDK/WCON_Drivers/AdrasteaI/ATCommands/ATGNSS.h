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
 * @brief AT commands for GNSS functionality.
 */

#ifndef ADRASTEAI_AT_GNSS_H_INCLUDED
#define ADRASTEAI_AT_GNSS_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Supported GNSS Start Modes
 */
    typedef enum AdrasteaI_ATGNSS_Start_Mode_t
    {
        AdrasteaI_ATGNSS_Start_Mode_Invalid = -1,
        AdrasteaI_ATGNSS_Start_Mode_Cold = 1,
        AdrasteaI_ATGNSS_Start_Mode_Hot = 2,
        AdrasteaI_ATGNSS_Start_Mode_NumberOfValues
    } AdrasteaI_ATGNSS_Start_Mode_t;

    typedef uint32_t AdrasteaI_ATGNSS_Tolerance_t;

    /**
 * @brief GNSS Active Modes
 */
    typedef enum AdrasteaI_ATGNSS_Active_Mode_t
    {
        AdrasteaI_ATGNSS_Active_Mode_Invalid = -1,
        AdrasteaI_ATGNSS_Active_Mode_Not_Active,
        AdrasteaI_ATGNSS_Active_Mode_Active,
        AdrasteaI_ATGNSS_Active_Mode_NumberOfValues
    } AdrasteaI_ATGNSS_Active_Mode_t;

    /**
 * @brief Runtime Mode States
 */
    typedef enum AdrasteaI_ATGNSS_Runtime_Mode_State_t
    {
        AdrasteaI_ATGNSS_Runtime_Mode_State_Not_Set,
        AdrasteaI_ATGNSS_Runtime_Mode_State_Set,
    } AdrasteaI_ATGNSS_Runtime_Mode_State_t;

    /**
 * @brief Supported GNSS Satellite Systems
 */
    typedef union AdrasteaI_ATGNSS_Satellite_Systems_t
    {
        uint8_t satSystemsStates;
        struct
        {
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GPS : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GLONASS : 1;
        } systems;
    } AdrasteaI_ATGNSS_Satellite_Systems_t;

#define AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues 2

    /**
 * @brief Supported GNSS NMEA Sentences
 */
    typedef union AdrasteaI_ATGNSS_NMEA_Sentences_t
    {
        uint16_t nmeaSentencesStates;
        struct
        {
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GGA : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GLL : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GSA : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GSV : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GNS : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t RMC : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t PIDX : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t VTG : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t ZDA : 1;
            AdrasteaI_ATGNSS_Runtime_Mode_State_t GST : 1;
        } sentences;
    } AdrasteaI_ATGNSS_NMEA_Sentences_t;

#define AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues 10

    typedef uint8_t AdrasteaI_ATGNSS_Satellite_Count_t;

    typedef uint8_t AdrasteaI_ATGNSS_Satellite_PRN_t, AdrasteaI_ATGNSS_Satellite_Elevation_t, AdrasteaI_ATGNSS_Satellite_SNR_t;

    typedef uint16_t AdrasteaI_ATGNSS_Satellite_Azimuth_t;

    /**
 * @brief GNSS Satellite
 */
    typedef struct AdrasteaI_ATGNSS_Satellite_t
    {
        AdrasteaI_ATGNSS_Satellite_PRN_t prn;
        AdrasteaI_ATGNSS_Satellite_Elevation_t elevation;
        AdrasteaI_ATGNSS_Satellite_Azimuth_t azimuth;
        AdrasteaI_ATGNSS_Satellite_SNR_t snr;
    } AdrasteaI_ATGNSS_Satellite_t;

    /**
 * @brief GNSS Fix Relevancy
 */
    typedef enum AdrasteaI_ATGNSS_Fix_Relavancy_t
    {
        AdrasteaI_ATGNSS_Fix_Relavancy_Invalid = -1,
        AdrasteaI_ATGNSS_Fix_Relavancy_Current,
        AdrasteaI_ATGNSS_Fix_Relavancy_Last,
        AdrasteaI_ATGNSS_Fix_Relavancy_NumberOfValues
    } AdrasteaI_ATGNSS_Fix_Relavancy_t;

    /**
 * @brief GNSS Fix Types
 */
    typedef enum AdrasteaI_ATGNSS_Fix_Type_t
    {
        AdrasteaI_ATGNSS_Fix_Type_Invalid = -1,
        AdrasteaI_ATGNSS_Fix_Type_No_Fix,
        AdrasteaI_ATGNSS_Fix_Type_MSA,
        AdrasteaI_ATGNSS_Fix_Type_MSB,
        AdrasteaI_ATGNSS_Fix_Type_NumberOfValues
    } AdrasteaI_ATGNSS_Fix_Type_t;

    typedef float AdrasteaI_ATGNSS_Radius_Accuracy_t, AdrasteaI_ATGNSS_Speed_t;

    typedef enum AdrasteaI_ATGNSS_Ephemeris_t
    {
        AdrasteaI_ATGNSS_Ephemeris_Invalid = -1,
        AdrasteaI_ATGNSS_Ephemeris_BEP,
        AdrasteaI_ATGNSS_Ephemeris_CEP,
        AdrasteaI_ATGNSS_Ephemeris_NumberOfValues
    } AdrasteaI_ATGNSS_Ephemeris_t;

    /**
 * @brief GNSS Fix
 */
    typedef struct AdrasteaI_ATGNSS_Fix_t
    {
        AdrasteaI_ATGNSS_Fix_Type_t fixType;
        AdrasteaI_ATCommon_Time_t time;
        AdrasteaI_ATCommon_Date_t date;
        AdrasteaI_ATCommon_Latitude_t latitude;
        AdrasteaI_ATCommon_Longitude_t longitude;
        AdrasteaI_ATCommon_Altitude_t altitude;
        AdrasteaI_ATCommon_UTC_Timestamp_t utcTimestamp;
        AdrasteaI_ATGNSS_Radius_Accuracy_t accuracy;
        AdrasteaI_ATGNSS_Speed_t speed;
        AdrasteaI_ATGNSS_Ephemeris_t ephType;
    } AdrasteaI_ATGNSS_Fix_t;

    typedef float AdrasteaI_ATGNSS_TTFF_t;

    typedef enum AdrasteaI_ATGNSS_Ephemeris_Status_t
    {
        AdrasteaI_ATGNSS_Ephemeris_Status_Invalid = -1,
        AdrasteaI_ATGNSS_Ephemeris_Status_Not_Valid,
        AdrasteaI_ATGNSS_Ephemeris_Status_Valid,
        AdrasteaI_ATGNSS_Ephemeris_Status_NumberOfValues
    } AdrasteaI_ATGNSS_Ephemeris_Status_t;

    /**
 * @brief GNSS Event Types
 */
    typedef enum AdrasteaI_ATGNSS_Event_t
    {
        AdrasteaI_ATGNSS_Event_Invalid = -1,
        AdrasteaI_ATGNSS_Event_NMEA,
        AdrasteaI_ATGNSS_Event_Session_Status_Change,
        AdrasteaI_ATGNSS_Event_Allowed_Status_Change,
        AdrasteaI_ATGNSS_Event_NumberOfValues
    } AdrasteaI_ATGNSS_Event_t;

    /**
 * @brief GNSS Session Status
 */
    typedef enum AdrasteaI_ATGNSS_Session_Status_t
    {
        AdrasteaI_ATGNSS_Session_Status_Invalid = -1,
        AdrasteaI_ATGNSS_Session_Status_None,
        AdrasteaI_ATGNSS_Session_Status_Session_Begin,
        AdrasteaI_ATGNSS_Session_Status_Session_End,
        AdrasteaI_ATGNSS_Session_Status_NumberOfValues
    } AdrasteaI_ATGNSS_Session_Status_t;

    /**
 * @brief GNSS Allowed Status
 */
    typedef enum AdrasteaI_ATGNSS_Allowed_Status_t
    {
        AdrasteaI_ATGNSS_Allowed_Status_Invalid = -1,
        AdrasteaI_ATGNSS_Allowed_Status_Not_Allowed,
        AdrasteaI_ATGNSS_Allowed_Status_Allowed,
        AdrasteaI_ATGNSS_Allowed_Status_Started_Automatically_or_Delay,
        AdrasteaI_ATGNSS_Allowed_Status_NumberOfValues
    } AdrasteaI_ATGNSS_Allowed_Status_t;

    typedef char AdrasteaI_ATGNSS_NMEA_Sentence_t[128];

    typedef enum AdrasteaI_ATGNSS_Deletion_Option_t
    {
        AdrasteaI_ATGNSS_Deletion_Option_Invalid = -1,
        AdrasteaI_ATGNSS_Deletion_Option_All = 0,
        AdrasteaI_ATGNSS_Deletion_Option_Ephemeris,
        AdrasteaI_ATGNSS_Deletion_Option_Almanac,
        AdrasteaI_ATGNSS_Deletion_Option_Position,
        AdrasteaI_ATGNSS_Deletion_Option_Time,
        AdrasteaI_ATGNSS_Deletion_Option_NumberOfValues
    } AdrasteaI_ATGNSS_Deletion_Option_t;

    typedef enum AdrasteaI_ATGNSS_CEP_Number_of_Days_t
    {
        AdrasteaI_ATGNSS_CEP_Number_of_Days_Invalid = -1,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_1Day = 1,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_2Days = 2,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_3Days = 3,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_7Days = 7,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_14Days = 14,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_28Days = 28,
        AdrasteaI_ATGNSS_CEP_Number_of_Days_NumberOfValues
    } AdrasteaI_ATGNSS_CEP_Number_of_Days_t;

    typedef enum AdrasteaI_ATGNSS_CEP_File_Validity_t
    {
        AdrasteaI_ATGNSS_CEP_File_Validity_Invalid = -1,
        AdrasteaI_ATGNSS_CEP_File_Validity_Expired,
        AdrasteaI_ATGNSS_CEP_File_Validity_Valid,
        AdrasteaI_ATGNSS_CEP_File_Validity_NumberOfValues
    } AdrasteaI_ATGNSS_CEP_File_Validity_t;

    typedef uint8_t AdrasteaI_ATGNSS_CEP_Remaining_Days_t, AdrasteaI_ATGNSS_CEP_Remaining_Hours_t, AdrasteaI_ATGNSS_CEP_Remaining_Minutes_t;

    /**
 * @brief CEP File Status
 */
    typedef struct AdrasteaI_ATGNSS_CEP_Status_t
    {
        AdrasteaI_ATGNSS_CEP_File_Validity_t validity;
        AdrasteaI_ATGNSS_CEP_Remaining_Days_t remDays;
        AdrasteaI_ATGNSS_CEP_Remaining_Hours_t remHours;
        AdrasteaI_ATGNSS_CEP_Remaining_Minutes_t remMinutes;
    } AdrasteaI_ATGNSS_CEP_Status_t;

    extern bool AdrasteaI_ATGNSS_StopGNSS();

    extern bool AdrasteaI_ATGNSS_StartGNSS(AdrasteaI_ATGNSS_Start_Mode_t startMode);

    extern bool AdrasteaI_ATGNSS_StartGNSSWithTolerance(AdrasteaI_ATGNSS_Tolerance_t tolerance);

    extern bool AdrasteaI_ATGNSS_ReadGNSSActiveMode(AdrasteaI_ATGNSS_Active_Mode_t* activeModeP);

    extern bool AdrasteaI_ATGNSS_SetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t satSystems);

    extern bool AdrasteaI_ATGNSS_GetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t* satSystemsP);

    extern bool AdrasteaI_ATGNSS_SetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t nmeaSentences);

    extern bool AdrasteaI_ATGNSS_GetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t* nmeaSentencesP);

    extern bool AdrasteaI_ATGNSS_QueryGNSSSatellites(AdrasteaI_ATGNSS_Satellite_Count_t* satCountP);

    extern bool AdrasteaI_ATGNSS_ParseSatelliteQueryEvent(char* pEventArguments, AdrasteaI_ATGNSS_Satellite_t* dataP);

    extern bool AdrasteaI_ATGNSS_QueryGNSSFix(AdrasteaI_ATGNSS_Fix_Relavancy_t relevancy, AdrasteaI_ATGNSS_Fix_t* fixP);

    extern bool AdrasteaI_ATGNSS_QueryGNSSTTFF(AdrasteaI_ATGNSS_TTFF_t* ttffP);

    extern bool AdrasteaI_ATGNSS_QueryGNSSEphemerisStatus(AdrasteaI_ATGNSS_Ephemeris_Status_t* statusP);

    extern bool AdrasteaI_ATGNSS_SetGNSSUnsolicitedNotificationEvents(AdrasteaI_ATGNSS_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

    extern bool AdrasteaI_ATGNSS_ParseSessionStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Session_Status_t* dataP);

    extern bool AdrasteaI_ATGNSS_ParseAllowedStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Allowed_Status_t* dataP);

    extern bool AdrasteaI_ATGNSS_ParseNMEAEvent(char* pEventArguments, AdrasteaI_ATGNSS_NMEA_Sentence_t* dataP);

    extern bool AdrasteaI_ATGNSS_DeleteData(AdrasteaI_ATGNSS_Deletion_Option_t deleteOption);

    extern bool AdrasteaI_ATGNSS_DownloadCEPFile(AdrasteaI_ATGNSS_CEP_Number_of_Days_t numDays);

    extern bool AdrasteaI_ATGNSS_EraseCEPFile();

    extern bool AdrasteaI_ATGNSS_QueryCEPFileStatus(AdrasteaI_ATGNSS_CEP_Status_t* status);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_GNSS_H_INCLUDED */
