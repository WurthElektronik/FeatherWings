/*
 ***************************************************************************************************
 * This file is part of Sensors SDK:
 * https://www.we-online.com/sensors, https://github.com/WurthElektronik/Sensors-SDK_STM32
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
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE (license_terms_wsen_sdk.pdf)
 * LOCATED IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Header file for the WSEN-HIDS-2525020210002 sensor driver.
 */

#ifndef _WSEN_HIDS_2525020210002_H
#define _WSEN_HIDS_2525020210002_H

/*         Includes         */

#include <stdint.h>
#include "../WeSensorsSDK.h"

/*         Register address definitions         */
#define CRC8_INIT 0xFF
#define CRC8_POLYNOMIAL 0x31
#define CRC8_LEN 1
#define HIDS_WORD_SIZE 2

typedef enum
{
	HIDS_ADDRESS = 0x44,  /**< Sensor Address */
	HIDS_MEASURE_HPM = 0xFD,  /**< High precision measurement for T & RH */
	HIDS_MEASURE_MPM = 0xF6,  /**< Medium precision measurement for T & RH */
	HIDS_MEASURE_LPM = 0xE0,  /**< Low precision measurement for T & RH */
	HIDS_SOFT_RESET = 0x94,  /**< Soft reset */
	HIDS_HEATER_200_MW_01_S = 0x39,  /**< activate heater with 200mW for 1s, including a high precision measurement just before deactivation */
	HIDS_HEATER_200_MW_100_MS = 0x32,  /**< activate heater with 200mW for 0.1s, including a high precision measurement just before deactivation */
	HIDS_HEATER_110_MW_01_S = 0x2F,  /**< activate heater with 110mW for 1s, including a high precision measurement just before deactivation */
	HIDS_HEATER_110_MW_100_MS = 0x24,  /**< activate heater with 110mW for 0.1s, including a high precision measurement just before deactivation */
	HIDS_HEATER_20_MW_01_S = 0x1E,  /**< activate heater with 20mW for 01s, including a high precision measurement just before deactivation */
	HIDS_HEATER_20_MW_100_MS = 0x15,  /**< activate heater with 20mW for 0.1s, including a high precision measurement just before deactivation */
	HIDS_MEASURE_SERIAL_NUMBER = 0x89 /**< Device */
} hids_measureCmd_t;

#ifdef __cplusplus
extern "C"
{
#endif
int8_t HIDS_Set_Measurement_Type(WE_sensorInterface_t* sensorInterface, hids_measureCmd_t precision);
int8_t HIDS_Sensor_Read_SlNo(WE_sensorInterface_t* sensorInterface, uint32_t* serialNo);
int8_t HIDS_Sensor_Init(WE_sensorInterface_t* sensorInterface);
int8_t HIDS_Reset(WE_sensorInterface_t* sensorInterface);
int8_t HIDS_Sensor_Measure_Raw(WE_sensorInterface_t* sensorInterface, hids_measureCmd_t meausureCmd, int32_t* temperatureRaw, int32_t* humidityRaw);
int8_t HIDS_Get_Default_Interface(WE_sensorInterface_t* sensorInterface);
#ifdef __cplusplus
}
#endif

#endif /* _WSEN_HIDSNEW_H */
