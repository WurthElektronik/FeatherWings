/**
***************************************************************************************************
* This file is part of Sensors SDK:
* https://www.we-online.com/sensors, https://github.com/WurthElektronik/Sensors-SDK
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
* COPYRIGHT (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
*
***************************************************************************************************
**/

#ifndef _WSEN_TIDS_H
#define _WSEN_TIDS_H

/* 
 * #### INFORMATIVE ####
 * This sensor only has a i2c communication interface.
 */

/**         Includes         */
#include <stdint.h> /* for support of uint8_t datatypes etc */
#include "ConfigPlatform.h"

/**         Available TIDS 2521020222501 I2C Slave addresses         */
#define TIDS_ADDRESS_I2C_0 (uint8_t)0x3F /* when SAO of TIDS is connected to logic LOW level */
#define TIDS_ADDRESS_I2C_1 (uint8_t)0x38 /* when SAO of TIDS is connected to logic HIGH level */

/**         TIDS 2521020222501 DEVICE_ID         */
#define TIDS_DEVICE_ID_VALUE (uint8_t)0xA0 /* Device ID of TIDS 2521020222501 Sensor */

/**         Register address definitions         */
#define TIDS_DEVICE_ID_REG (uint8_t)0x01  /* Device ID register */
#define TIDS_LIMIT_T_H_REG (uint8_t)0x02  /* Temperature high limit register */
#define TIDS_LIMIT_T_L_REG (uint8_t)0x03  /* Temperature low limit register */
#define TIDS_CTRL_REG (uint8_t)0x04		  /* Control Register*/
#define TIDS_STATUS_REG (uint8_t)0x05	  /* Status Register */
#define TIDS_DATA_T_L_REG (uint8_t)0x06	  /* Temperature Output LSB value Register */
#define TIDS_DATA_T_H_REG (uint8_t)0x07	  /* Temperature Output MSB value Register */
#define TIDS_SOFT_RESET_REG (uint8_t)0x0C /* Software reset register */

/**         Register type definitions         */
/**
* Control Register
* Address 0x04
* Type  R/W
* Default value: 0x00
*
*      AVG1  | AVG0  | Temperature output data-rate(Hz)
*   ---------------- ----------------------------------------------------
*       0    |  0    |      25
*       0    |  1    |      50
*       1    |  0    |      100
*       1    |  1    |      200
*
*/
typedef struct
{
	uint8_t oneShotbit : 1;		 /* Trigger a single measurement by setting this bit to 1; Bit automatically reset to 0*/
	uint8_t reserved01 : 1;		 /* Must be set to 0 */
	uint8_t freeRunBit : 1;		 /* FREERUN : 1:Enable continuous mode 0:Disable continuous mode */
	uint8_t autoAddIncr : 1;	 /* IF_ADD_INC : Register address automatically incremented during a multiple byte access with I2C interface. Default value 1.( 0: disable; 1: enable) */
	uint8_t outputDataRate : 2;	 /* AVG[1:0]: output data rate in continuous mode.Default '00' */
	uint8_t blockDataUpdate : 1; /* BDU : block data update. 0 - continuous update; 1 - output registers not update till both MSB and LSB are read */
	uint8_t reserved02 : 1;		 /* Must be set to 0 */
} TIDS_ctrl_t;

/**
* Status register
* Address 0x05
* read only
* Default value: Output; 0x00
*/
typedef struct
{
	uint8_t busy : 1;		/* BUSY : Temperature conversion status.(0: data conversion complete; 1: data conversion in progress) */
	uint8_t overLimit : 1;	/* OVER_THL :Temperature higher limit status. (0: temperature is below limit or disabled; 1: temperature exceeded high limit*/
	uint8_t underlimit : 2; /* UNDER_TLL :Temperature lower limit status. (0: temperature is below limit or disabled; 1: temperature exceeded low limit*/
	uint8_t reserved05 : 5; /* Must be set to 0 */
} TIDS_status_t;

/**
* Software reset register
* Address 0x0C
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t reserved01 : 1; /* Must be set to 0 */
	uint8_t reset : 1;		/* SOFT_RESET : Reset the digital block by setting this bit to 1 */
	uint8_t reserved06 : 6; /* Must be set to 0 */
} TIDS_soft_reset_t;

/**         Functional type definition         */
typedef enum
{
	TIDS_disable = 0,
	TIDS_enable = 1
} TIDS_state_t;

typedef enum
{
	TIDS_outputDataRate25HZ = 0,  /* 25 Hz */
	TIDS_outputDataRate50HZ = 1,  /* 50 Hz */
	TIDS_outputDataRate100HZ = 2, /* 100 Hz */
	TIDS_outputDataRate200HZ = 3, /* 200 Hz */
} TIDS_output_data_rate_t;

#ifdef __cplusplus
extern "C"
{
#endif

	/**         Functions definition         */

	/* Device ID */
	int8_t TIDS_getDeviceID(uint8_t *devID);

	/* Temperature limits */
	int8_t TIDS_setTempHighLimit(uint8_t hLimit);
	int8_t TIDS_setTempLowLimit(uint8_t lLimit);

	int8_t TIDS_getTempHighLimit(uint8_t *hLimit);
	int8_t TIDS_getTempLowLimit(uint8_t *lLimit);

	/* Block data update */
	int8_t TIDS_setBlockDataUpdate(TIDS_state_t bdu);
	int8_t TIDS_getBlockDataUpdate(TIDS_state_t *bdu);

	/* Output data rate */
	int8_t TIDS_setOutputDataRate(TIDS_output_data_rate_t odr);
	int8_t TIDS_getOutputDataRate(TIDS_output_data_rate_t *odr);

	/* Address auto increment */
	int8_t TIDS_setAutoIncrement(TIDS_state_t inc);
	int8_t TIDS_getAutoIncrement(TIDS_state_t *inc);

	/* Free run mode */
	int8_t TIDS_setContinuousMode(TIDS_state_t mode);
	int8_t TIDS_getContinuousMode(TIDS_state_t *mode);

	/* One shot mode */
	int8_t TIDS_setSingleConvMode(TIDS_state_t mode);
	int8_t TIDS_getSingleConvMode(TIDS_state_t *mode);

	/* Status */
	int8_t TIDS_getOverHighLimStatus(TIDS_state_t *state);
	int8_t TIDS_getUnderLowLimStatus(TIDS_state_t *state);
	int8_t TIDS_getBusyStatus(TIDS_state_t *state);

	/* Software reset */
	int8_t TIDS_setSwReset(TIDS_state_t mode);
	int8_t TIDS_getSwReset(TIDS_state_t *mode);

	/* standard Data Out */
	int8_t TIDS_getRAWTemperature(int16_t *rawTemp);
	int8_t TIDS_getTemperature(float *tempdegC); // Temperature Value in °C

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_TIDS_H */
/**         EOF         */
