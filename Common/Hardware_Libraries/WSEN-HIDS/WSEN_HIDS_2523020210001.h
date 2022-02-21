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

#ifndef _WSEN_HIDS_H
#define _WSEN_HIDS_H

/**         Includes         */

#include <stdint.h> /* for support of uint8_t datatypes etc */
#include "ConfigPlatform.h"

/**         Available HIDS I2C Slave addresses         */

#define HIDS_ADDRESS_I2C_0 (uint8_t)0x5F

/**         HIDS 2511020213301 DEVICE_ID         */

#define HIDS_DEVICE_ID_VALUE (uint8_t)0xBC /* Device ID of HIDS Sensor */

/**         Register address definitions         */

#define HIDS_DEVICE_ID_REG (uint8_t)0x0F /* Device ID register */
#define HIDS_Average_REG (uint8_t)0x10	 /* average configuration register */
#define HIDS_CTRL_REG_1 (uint8_t)0x20	 /* Control Register 1 */
#define HIDS_CTRL_REG_2 (uint8_t)0x21	 /* Control Register 2 */
#define HIDS_CTRL_REG_3 (uint8_t)0x22	 /* Control Register 3 */
#define HIDS_STATUS_REG (uint8_t)0x27	 /* Status Register */
#define HIDS_H_OUT_L_REG (uint8_t)0x28	 /* Humidity Output MID value Register */
#define HIDS_H_OUT_H_REG (uint8_t)0x29	 /* Humidity Output MSB value Register */
#define HIDS_T_OUT_L_REG (uint8_t)0x2A	 /* Temperature Output LSB value Register */
#define HIDS_T_OUT_H_REG (uint8_t)0x2B	 /* Temperature Output MSB value Register */
#define HIDS_H0_RH_X2 (uint8_t)0x30		 /* H0_RH_X2 calibration register*/
#define HIDS_H1_RH_X2 (uint8_t)0x31		 /* H1_RH_X2 calibration register*/
#define HIDS_T0_DEGC_X8 (uint8_t)0x32	 /* T0_DEGC_X8 calibration register*/
#define HIDS_T1_DEGC_X8 (uint8_t)0x33	 /* T1_DEGC_X8 calibration register*/
#define HIDS_T0_T1_DEGC_H2 (uint8_t)0x35 /* T0_T1_DEGC_H2 calibration register*/
#define HIDS_H0_T0_OUT_L (uint8_t)0x36	 /* H0_T0_OUT_LSB calibration register*/
#define HIDS_H0_T0_OUT_H (uint8_t)0x37	 /* H0_T0_OUT_H MSB calibration register*/
#define HIDS_H1_T0_OUT_L (uint8_t)0x3A	 /* H1_T0_OUT_LSB calibration register*/
#define HIDS_H1_T0_OUT_H (uint8_t)0x3B	 /* H1_T0_OUT_H MSB calibration register*/
#define HIDS_T0_OUT_L (uint8_t)0x3C		 /* T0_OUT_LSB calibration register*/
#define HIDS_T0_OUT_H (uint8_t)0x3D		 /* T0_OUT_H MSB calibration register*/
#define HIDS_T1_OUT_L (uint8_t)0x3E		 /* T1_OUT_LSB  calibration register*/
#define HIDS_T1_OUT_H (uint8_t)0x3F		 /* T1_OUT_H MSB calibration register*/

/**         Register type definitions         */

/**
* Humidity and temperature average configuration
* Address 0x10
* Type  R/W
* Default value: 0x1B

*
*   AVG 2:0   |  (AVGT) | (AVGH) |
* ----------------------------------
*       000   |  2    	|  4     |
*       001   |  4      |  8     |
*       010   |  8      |  16    |
*       011   |  16     |  32    |
*       100   |  32     |  64    |
*       101   |  64    	|  128   |
*       110   |  128    |  256   |
*       111   |  256    |  512   |
*
*    -------------------------------
*/

typedef struct
{
	uint8_t avgHum : 3;	   /* AVGH	: select the numbers of averaged humidity samples (4 - 512),*/
	uint8_t avgTemp : 3;   /* AVGT : select the numbers of averaged temperature samples */
	uint8_t notUsed01 : 2; /* This bit must be set to 0 for proper operation of the device */
} HIDS_average_config_t;

/**
* Control Register 1
* Address 0x20
* Type  R/W
* Default value: 0x00

*      ODR1  | ODR0   | Humidity/Temperature output data-rate(Hz)
*   ---------------------------------------------------------------------
*       0    |  0     |            		One-shot
*       0    |  1     |                 1
*       1    |  0     |                 7
*       1    |  1     |                 12.5

*/
typedef struct
{
	uint8_t odr : 2;			  /* ODR : output data rate selection */
	uint8_t bdu : 1;			  /* BDU : block data update. 0 - continuous update; 1 - output registers not update till both MSB and LSB are read */
	uint8_t notUsed01 : 4;		  /* This bit must be set to 0 for proper operation of the device */
	uint8_t powerControlMode : 1; /* PD : (0: power-down mode; 1: continuous mode) */
} HIDS_ctrl_1_t;

/**
* Control Register 2
* Address 0x21
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t oneShotBit : 1;	  /* One-shot enable : (0: waiting for start of conversion; 1: start for a new dataset) */
	uint8_t heater : 1;		  /* Heater : (0: heater disable; 1: heater enable)*/
	uint8_t notUsed01 : 5;	  /* This bit must be set to 0 for proper operation of the device */
	uint8_t rebootMemory : 1; /* BOOT: : ((0: normal mode; 1: reboot memory content)*/
} HIDS_ctrl_2_t;

/**
* Control Register 3
* Address 0x22
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t notUsed01 : 2;			/* This bit must be set to 0 for proper operation of the device */
	uint8_t enDataReady : 1;		/* (0: Data Ready disabled - default;1: Data Ready signal available on pin 3)*/
	uint8_t notUsed02 : 3;			/* This bit must be set to 0 for proper operation of the device */
	uint8_t interruptPinConfig : 1; /* PP_OD : : (0: push-pull - default; 1: open drain)*/
	uint8_t drdyOutputLevel : 1;	/* DRDY_H_L :(0: active high - default;1: active low)*/
} HIDS_ctrl_3_t;

/**
* Status register
* Address 0x27
* read only
* Default value: Output; 0x00
*/
typedef struct
{
	uint8_t humDataAvailable : 1;  /* H_DA : Pressure data available.(0: new data for humidity is not yet available; 1: new data for humidity is available */
	uint8_t tempDataAvailable : 1; /* T_DA :Temperature data available. (0: temperature not yet available; 1: a new temperature data is available) */
	uint8_t notUsed01 : 6;		   /* This bit must be set to 0 for proper operation of the device */
} HIDS_status_t;

/**         functional type definition         */

typedef enum
{
	HIDS_disable = 0,
	HIDS_enable = 1
} HIDS_state_t;

typedef enum
{
	powerDownMode = 0,
	activeMode = 1
} HIDS_power_mode_t;

typedef enum
{
	oneShot = 0,   /* One-Shot */
	odr1HZ = 1,	   /* 1Hz */
	odr7HZ = 2,	   /* 7Hz */
	odr12_5HZ = 3, /* 12.5Hz */
} HIDS_output_data_rate_t;

typedef enum
{
	average0 = 0,
	average1 = 1,
	average2 = 2,
	average3 = 3,
	average4 = 4,
	average5 = 5,
	average6 = 6,
	average7 = 7
} HIDS_average_conf_t;

typedef enum
{
	HIDS_activeHigh = 0,
	HIDS_activeLow = 1
} HIDS_active_level_t;

typedef enum
{
	HIDS_pushPull = 0,
	HIDS_openDrain = 1
} HIDS_PP_OD_t;

/**         Functions definition         */

#ifdef __cplusplus
extern "C"
{
#endif

	int8_t HIDS_getDeviceID(uint8_t *devID);

	/* Standard Configurations */
	uint8_t HIDS_setHumAvg(HIDS_average_conf_t avgH);
	uint8_t HIDS_getHumAvg(HIDS_average_conf_t *avgH);
	uint8_t HIDS_setTempAvg(HIDS_average_conf_t avgT);
	uint8_t HIDS_getTempAvg(HIDS_average_conf_t *avgT);

	int8_t HIDS_setOdr(HIDS_output_data_rate_t odr);
	int8_t HIDS_getOdr(HIDS_output_data_rate_t *odr);

	int8_t HIDS_setBdu(HIDS_state_t bdu);
	int8_t HIDS_getBduState(HIDS_state_t *bdu);

	int8_t HIDS_setPowerMode(HIDS_power_mode_t pd);
	int8_t HIDS_getPowerMode(HIDS_power_mode_t *pd);
	int8_t HIDS_enOneShot(HIDS_state_t oneShot);
	int8_t HIDS_getOneShotState(HIDS_state_t *oneShot);

	int8_t HIDS_enHeater(HIDS_state_t heater);
	int8_t HIDS_getHeaterState(HIDS_state_t *heater);

	int8_t HIDS_enRebootMemory(HIDS_state_t reboot);
	int8_t HIDS_getRebootMemoryState(HIDS_state_t *reboot);

	int8_t HIDS_enInt(HIDS_state_t Drdy);
	int8_t HIDS_getIntState(HIDS_state_t *Drdy);

	int8_t HIDS_setIntPinType(HIDS_PP_OD_t PP_OD);
	int8_t HIDS_getIntPinType(HIDS_PP_OD_t *PP_OD);

	int8_t HIDS_setIntLevel(HIDS_active_level_t Drdy_H_L);
	int8_t HIDS_getIntLevel(HIDS_active_level_t *Drdy_H_L);

	/*get the temperature and Humidity Output data  */

	uint16_t HIDS_get_H_T_out();
	uint16_t HIDS_get_T_OUT();

	/*get the calibration parameters */

	uint16_t HIDS_get_H0_T0_out();
	uint16_t HIDS_get_H1_T0_out();
	uint16_t HIDS_get_H0_rh();
	uint16_t HIDS_get_H1_rh();

	uint16_t HIDS_get_T1_OUT();
	uint16_t HIDS_get_T0_OUT();
	/* Temperature and Humidity data available State */

	int8_t HIDS_getTempStatus(HIDS_state_t *temp_state);
	int8_t HIDS_getHumStatus(HIDS_state_t *humidity_state);

	int8_t HIDS_getRAWValues(int16_t *rawHumidity, int16_t *rawTemp);
	int8_t HIDS_getHumidity(float *humidity);
	int8_t HIDS_getTemperature(float *tempDegC);

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_HIDS_H */
/**         EOF         */
