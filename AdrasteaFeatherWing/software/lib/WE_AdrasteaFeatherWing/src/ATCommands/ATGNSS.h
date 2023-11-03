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
 * @brief AT commands for GNSS functionality.
 */

#ifndef AT_GNSS_H_INCLUDED
#define AT_GNSS_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ATCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported GNSS Start Modes
 */
typedef enum ATGNSS_Start_Mode_t
{
	ATGNSS_Start_Mode_Invalid = -1,
	ATGNSS_Start_Mode_Cold = 1,
	ATGNSS_Start_Mode_Hot = 2,
	ATGNSS_Start_Mode_NumberOfValues
} ATGNSS_Start_Mode_t;

typedef uint32_t ATGNSS_Tolerance_t;

/**
 * @brief GNSS Active Modes
 */
typedef enum ATGNSS_Active_Mode_t
{
	ATGNSS_Active_Mode_Invalid = -1,
	ATGNSS_Active_Mode_Not_Active,
	ATGNSS_Active_Mode_Active,
	ATGNSS_Active_Mode_NumberOfValues
} ATGNSS_Active_Mode_t;

/**
 * @brief Runtime Mode States
 */
typedef enum ATGNSS_Runtime_Mode_State_t
{
	ATGNSS_Runtime_Mode_State_Not_Set,
	ATGNSS_Runtime_Mode_State_Set,
} ATGNSS_Runtime_Mode_State_t;

/**
 * @brief Supported GNSS Satellite Systems
 */
typedef union ATGNSS_Satellite_Systems_t
{
	uint8_t satSystemsStates;
	struct
	{
		ATGNSS_Runtime_Mode_State_t GPS :1;
		ATGNSS_Runtime_Mode_State_t GLONASS :1;
	} systems;
} ATGNSS_Satellite_Systems_t;

#define ATGNSS_Satellite_Systems_Strings_NumberOfValues 2

/**
 * @brief Supported GNSS NMEA Sentences
 */
typedef union ATGNSS_NMEA_Sentences_t
{
	uint16_t nmeaSentencesStates;
	struct
	{
		ATGNSS_Runtime_Mode_State_t GGA :1;
		ATGNSS_Runtime_Mode_State_t GLL :1;
		ATGNSS_Runtime_Mode_State_t GSA :1;
		ATGNSS_Runtime_Mode_State_t GSV :1;
		ATGNSS_Runtime_Mode_State_t GNS :1;
		ATGNSS_Runtime_Mode_State_t RMC :1;
		ATGNSS_Runtime_Mode_State_t PIDX :1;
		ATGNSS_Runtime_Mode_State_t VTG :1;
		ATGNSS_Runtime_Mode_State_t ZDA :1;
		ATGNSS_Runtime_Mode_State_t GST :1;
	} sentences;
} ATGNSS_NMEA_Sentences_t;

#define ATGNSS_NMEA_Sentences_Strings_NumberOfValues 10

typedef uint8_t ATGNSS_Satellite_Count_t;

typedef uint8_t ATGNSS_Satellite_PRN_t, ATGNSS_Satellite_Elevation_t, ATGNSS_Satellite_SNR_t;

typedef uint16_t ATGNSS_Satellite_Azimuth_t;

/**
 * @brief GNSS Satellite
 */
typedef struct ATGNSS_Satellite_t
{
	ATGNSS_Satellite_PRN_t prn;
	ATGNSS_Satellite_Elevation_t elevation;
	ATGNSS_Satellite_Azimuth_t azimuth;
	ATGNSS_Satellite_SNR_t snr;
} ATGNSS_Satellite_t;

/**
 * @brief GNSS Fix Relevancy
 */
typedef enum ATGNSS_Fix_Relavancy_t
{
	ATGNSS_Fix_Relavancy_Invalid = -1,
	ATGNSS_Fix_Relavancy_Current,
	ATGNSS_Fix_Relavancy_Last,
	ATGNSS_Fix_Relavancy_NumberOfValues
} ATGNSS_Fix_Relavancy_t;

/**
 * @brief GNSS Fix Types
 */
typedef enum ATGNSS_Fix_Type_t
{
	ATGNSS_Fix_Type_Invalid = -1,
	ATGNSS_Fix_Type_No_Fix,
	ATGNSS_Fix_Type_MSA,
	ATGNSS_Fix_Type_MSB,
	ATGNSS_Fix_Type_NumberOfValues
} ATGNSS_Fix_Type_t;

typedef float ATGNSS_Radius_Accuracy_t, ATGNSS_Speed_t;

typedef enum ATGNSS_Ephemeris_t
{
	ATGNSS_Ephemeris_Invalid = -1,
	ATGNSS_Ephemeris_BEP,
	ATGNSS_Ephemeris_CEP,
	ATGNSS_Ephemeris_NumberOfValues
} ATGNSS_Ephemeris_t;

/**
 * @brief GNSS Fix
 */
typedef struct ATGNSS_Fix_t
{
	ATGNSS_Fix_Type_t fixType;
	ATCommon_Time_t time;
	ATCommon_Date_t date;
	ATCommon_Latitude_t latitude;
	ATCommon_Longitude_t longitude;
	ATCommon_Altitude_t altitude;
	ATCommon_UTC_Timestamp_t utcTimestamp;
	ATGNSS_Radius_Accuracy_t accuracy;
	ATGNSS_Speed_t speed;
	ATGNSS_Ephemeris_t ephType;
} ATGNSS_Fix_t;

typedef float ATGNSS_TTFF_t;

typedef enum ATGNSS_Ephemeris_Status_t
{
	ATGNSS_Ephemeris_Status_Invalid = -1,
	ATGNSS_Ephemeris_Status_Not_Valid,
	ATGNSS_Ephemeris_Status_Valid,
	ATGNSS_Ephemeris_Status_NumberOfValues
} ATGNSS_Ephemeris_Status_t;

/**
 * @brief GNSS Event Types
 */
typedef enum ATGNSS_Event_t
{
	ATGNSS_Event_Invalid = -1,
	ATGNSS_Event_NMEA,
	ATGNSS_Event_Session_Status_Change,
	ATGNSS_Event_Allowed_Status_Change,
	ATGNSS_Event_NumberOfValues
} ATGNSS_Event_t;

/**
 * @brief GNSS Session Status
 */
typedef enum ATGNSS_Session_Status_t
{
	ATGNSS_Session_Status_Invalid = -1,
	ATGNSS_Session_Status_None,
	ATGNSS_Session_Status_Session_Begin,
	ATGNSS_Session_Status_Session_End,
	ATGNSS_Session_Status_NumberOfValues
} ATGNSS_Session_Status_t;

/**
 * @brief GNSS Allowed Status
 */
typedef enum ATGNSS_Allowed_Status_t
{
	ATGNSS_Allowed_Status_Invalid = -1,
	ATGNSS_Allowed_Status_Not_Allowed,
	ATGNSS_Allowed_Status_Allowed,
	ATGNSS_Allowed_Status_Started_Automatically_or_Delay,
	ATGNSS_Allowed_Status_NumberOfValues
} ATGNSS_Allowed_Status_t;

typedef char ATGNSS_NMEA_Sentence_t[128];

typedef enum ATGNSS_Deletion_Option_t
{
	ATGNSS_Deletion_Option_Invalid = -1,
	ATGNSS_Deletion_Option_All = 0,
	ATGNSS_Deletion_Option_Ephemeris,
	ATGNSS_Deletion_Option_Almanac,
	ATGNSS_Deletion_Option_Position,
	ATGNSS_Deletion_Option_Time,
	ATGNSS_Deletion_Option_NumberOfValues
} ATGNSS_Deletion_Option_t;

typedef enum ATGNSS_CEP_Number_of_Days_t
{
	ATGNSS_CEP_Number_of_Days_Invalid = -1,
	ATGNSS_CEP_Number_of_Days_1Day = 1,
	ATGNSS_CEP_Number_of_Days_2Days = 2,
	ATGNSS_CEP_Number_of_Days_3Days = 3,
	ATGNSS_CEP_Number_of_Days_7Days = 7,
	ATGNSS_CEP_Number_of_Days_14Days = 14,
	ATGNSS_CEP_Number_of_Days_28Days = 28,
	ATGNSS_CEP_Number_of_Days_NumberOfValues
} ATGNSS_CEP_Number_of_Days_t;

typedef enum ATGNSS_CEP_File_Validity_t
{
	ATGNSS_CEP_File_Validity_Invalid = -1,
	ATGNSS_CEP_File_Validity_Expired,
	ATGNSS_CEP_File_Validity_Valid,
	ATGNSS_CEP_File_Validity_NumberOfValues
} ATGNSS_CEP_File_Validity_t;

typedef uint8_t ATGNSS_CEP_Remaining_Days_t, ATGNSS_CEP_Remaining_Hours_t, ATGNSS_CEP_Remaining_Minutes_t;

/**
 * @brief CEP File Status
 */
typedef struct ATGNSS_CEP_Status_t
{
	ATGNSS_CEP_File_Validity_t validity;
	ATGNSS_CEP_Remaining_Days_t remDays;
	ATGNSS_CEP_Remaining_Hours_t remHours;
	ATGNSS_CEP_Remaining_Minutes_t remMinutes;
} ATGNSS_CEP_Status_t;

extern bool ATGNSS_StopGNSS();

extern bool ATGNSS_StartGNSS(ATGNSS_Start_Mode_t startMode);

extern bool ATGNSS_StartGNSSWithTolerance(ATGNSS_Tolerance_t tolerance);

extern bool ATGNSS_ReadGNSSActiveMode(ATGNSS_Active_Mode_t *activeModeP);

extern bool ATGNSS_SetSatelliteSystems(ATGNSS_Satellite_Systems_t satSystems);

extern bool ATGNSS_GetSatelliteSystems(ATGNSS_Satellite_Systems_t *satSystemsP);

extern bool ATGNSS_SetNMEASentences(ATGNSS_NMEA_Sentences_t nmeaSentences);

extern bool ATGNSS_GetNMEASentences(ATGNSS_NMEA_Sentences_t *nmeaSentencesP);

extern bool ATGNSS_QueryGNSSSatellites(ATGNSS_Satellite_Count_t *satCountP);

extern bool ATGNSS_ParseSatelliteQueryEvent(char *pEventArguments, ATGNSS_Satellite_t *dataP);

extern bool ATGNSS_QueryGNSSFix(ATGNSS_Fix_Relavancy_t relevancy, ATGNSS_Fix_t *fixP);

extern bool ATGNSS_QueryGNSSTTFF(ATGNSS_TTFF_t *ttffP);

extern bool ATGNSS_QueryGNSSEphemerisStatus(ATGNSS_Ephemeris_Status_t *statusP);

extern bool ATGNSS_SetGNSSUnsolicitedNotificationEvents(ATGNSS_Event_t event, ATCommon_Event_State_t state);

extern bool ATGNSS_ParseSessionStatusChangedEvent(char *pEventArguments, ATGNSS_Session_Status_t *dataP);

extern bool ATGNSS_ParseAllowedStatusChangedEvent(char *pEventArguments, ATGNSS_Allowed_Status_t *dataP);

extern bool ATGNSS_ParseNMEAEvent(char *pEventArguments, ATGNSS_NMEA_Sentence_t *dataP);

extern bool ATGNSS_DeleteData(ATGNSS_Deletion_Option_t deleteOption);

extern bool ATGNSS_DownloadCEPFile(ATGNSS_CEP_Number_of_Days_t numDays);

extern bool ATGNSS_EraseCEPFile();

extern bool ATGNSS_QueryCEPFileStatus(ATGNSS_CEP_Status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* AT_GNSS_H_INCLUDED */
