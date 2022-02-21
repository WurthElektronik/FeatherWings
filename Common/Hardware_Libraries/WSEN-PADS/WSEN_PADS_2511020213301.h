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
* COPYRIGHT (c) 2019 Würth Elektronik eiSos GmbH & Co. KG
*
***************************************************************************************************
**/

#ifndef _WSEN_PADS_H
#define _WSEN_PADS_H

/**         Includes         */

#include <stdint.h> /* for support of uint8_t datatypes etc */
#include "ConfigPlatform.h"

/**         Available PADS 2511020213301 I2C Slave addresses         */

#define PADS_ADDRESS_I2C_0 (uint8_t)0x5C /* when SAO of PADS is connected to logic LOW level */
#define PADS_ADDRESS_I2C_1 (uint8_t)0x5D /* when SAO of PADS is connected to logic HIGH level */

/**         PADS 2511020213301 DEVICE_ID         */

#define PADS_DEVICE_ID_VALUE (uint8_t)0xB3 /* Device ID of PADS 2511020213301 Sensor */

/**         Register address definitions         */

#define PADS_INT_CFG_REG (uint8_t)0x0B		  /* Interrupt configuration register */
#define PADS_THR_P_L_REG (uint8_t)0x0C		  /* Pressure threshold  LSB register */
#define PADS_THR_P_H_REG (uint8_t)0x0D		  /* Pressure threshold  MSB register */
#define PADS_INTERFACE_CTRL_REG (uint8_t)0x0E /* Interface control register */
#define PADS_DEVICE_ID_REG (uint8_t)0x0F	  /* Device ID register */
#define PADS_CTRL_1_REG (uint8_t)0x10		  /* Control Register 1 */
#define PADS_CTRL_2_REG (uint8_t)0x11		  /* Control Register 2 */
#define PADS_CTRL_3_REG (uint8_t)0x12		  /* Control Register 3 */
#define PADS_FIFO_CTRL_REG (uint8_t)0x13	  /* FIFO Control Register */
#define PADS_FIFO_WTM_REG (uint8_t)0x14		  /* FIFO threshold Register */
#define PADS_REF_P_L_REG (uint8_t)0x15		  /* reference pressure LSB value Register */
#define PADS_REF_P_H_REG (uint8_t)0x16		  /* reference pressure MSB value Register */
#define PADS_OPC_P_L_REG (uint8_t)0x18		  /* pressure offset LSB value Register */
#define PADS_OPC_P_H_REG (uint8_t)0x19		  /* pressure offset LSB value Register */
#define PADS_INT_SOURCE_REG (uint8_t)0x24	  /* Interrupt Register */
#define PADS_FIFO_STATUS1_REG (uint8_t)0x25	  /* FIFO status Register 1 */
#define PADS_FIFO_STATUS2_REG (uint8_t)0x26	  /* FIFO status Register 2 */
#define PADS_STATUS_REG (uint8_t)0x27		  /* Status Register */
#define PADS_DATA_P_XL_REG (uint8_t)0x28	  /* Pressure Output LSB value Register */
#define PADS_DATA_P_L_REG (uint8_t)0x29		  /* Pressure Output MID value Register */
#define PADS_DATA_P_H_REG (uint8_t)0x2A		  /* Pressure Output MSB value Register */
#define PADS_DATA_T_L_REG (uint8_t)0x2B		  /* Temperature Output LSB value Register */
#define PADS_DATA_T_H_REG (uint8_t)0x2C		  /* Temperature Output MSB value Register */
#define PADS_FIFO_DATA_P_XL_REG (uint8_t)0x78 /* Pressure LSB data in FIFO buffer */
#define PADS_FIFO_DATA_P_L_REG (uint8_t)0x79  /* Pressure MID data in FIFO buffer */
#define PADS_FIFO_DATA_P_H_REG (uint8_t)0x7A  /* Pressure MSB data in FIFO buffer */
#define PADS_FIFO_DATA_T_L_REG (uint8_t)0x7B  /* Temperature LSB data in FIFO buffer */
#define PADS_FIFO_DATA_T_H_REG (uint8_t)0x7C  /* Temperature MSB data in FIFO buffer */

/**         Register type definitions         */

/**
* Interrupt_CFG
* Address 0x0B
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t highPresInt : 1;	   /* PHE: enable/disable interrupt on pressure low event (0: disabled; 1:enabled) */
	uint8_t lowPresInt : 1;		   /* PLE: enable/disable interrupt on pressure high event (0:disabled; 1:enabled) */
	uint8_t latchedInt : 1;		   /* LIR: enable/disable latched Interrupt.(0: normal; 1:enabled) */
	uint8_t diffInt : 1;		   /* DIFF_EN  : enable/disable Differential Interrupt generation. (0: disabled; 1:enabled) */
	uint8_t resetAutoZeroFunc : 1; /* RESET_AZ : Reset Autozero function; reset Reference Pressure Register.(0: normal mode; 1:reset Autozero) */
	uint8_t autoZeroFunc : 1;	   /* AUTPZERO : enable/disable Autozero Mode to obtain a reference Pressure stored in Register (0x15,0x16) (0: normal; 1:enabled) */
	uint8_t resetAutoRefp : 1;	   /* RESET_ARP: reset Reference Pressure Register. (0: normal mode; 1:enabled) */
	uint8_t autoRefp : 1;		   /* AUTOREP  : enable AUTOREFP function. (0: disabled; 1:enabled) */
} PADS_interrupt_configuration_t;

/**
* Pressure threshold  LSB register
* Address 0x0C
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t presThresholdLsb : 8; /* THR[7:0] This register contains the low part of threshold value for pressure interrupt */
} PADS_threshold_p_l_t;

/**
* Pressure threshold  MSB register
* Address 0x0D
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t presThresholdMsb : 7; /* THR[14:8] This register contains the high part of threshold value for pressure interrupt */
	uint8_t notUsed01 : 1;		  /* This bit must be set to 0 for proper operation of the device */
} PADS_threshold_p_h_t;

/**
* Interface control register
* Address 0x0B
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t disableI2C : 1;			 /* i2cDisable : enable/disable I2C digital Interface.(0: disabled; 1:enabled) */
	uint8_t notUsed01 : 1;			 /* This bit must be set to 0 for proper operation of the device*/
	uint8_t disPullDownOnIntPin : 1; /* PD_DIS_INT : enable/disable pull down on the INT pin.(0:enabled; 1:disabled) */
	uint8_t pullUpOnSAOpin : 1;		 /* SAO_PU_EN enable/disable  pull-up on the SAO pin.(0: disabled; 1:enabled) */
	uint8_t pullUpOnSDApin : 1;		 /* SDA_PU_EN enable/disable  pull-up on the SDA pin.(0: disabled; 1:enabled) */
	uint8_t notUsed02 : 2;			 /* This bit must be set to 0 for proper operation of the device */
	uint8_t notUsed03 : 1;			 /* This bit must be set to 0 for proper operation of the device */
} PADS_interface_ctrl_t;

/**
* Control Register 1
* Address 0x0F
* Type  R/W
* Default value: 0x00
*
*      ODR2  | ODR1  | ODR0   | Pressure/Temperature output data-rate(Hz)
*   ---------------- ----------------------------------------------------
*       0    |  0    |  0     |            Single Conversion
*       0    |  0    |  1     |                 1
*       0    |  1    |  0     |                 10
*       0    |  1    |  1     |                 25
*       1    |  0    |  0     |                 50
*       1    |  0    |  1     |                 75
*       1    |  1    |  0     |                 100
*       1    |  1    |  1     |                 200
*
*    -------------------------------------------------------------------
*
*    EN_LPFP  |    LPFP_CFG      |       LPF2 status         | Device Bandwidth | Samples to be discarded
*     -------------------------------------------------------------------------------------------
*        0    |  x(don't care)   |     Disabled/reset filter |       ODR/2      |             0
*        1    |      0           |     Enabled               |       ODR/9      |             2
*        1    |      1           |     Enabled               |       ODR/20     |             2
*
*/
typedef struct
{
	uint8_t notUsed01 : 1;			 /* This bit must be set to 0 for proper operation of the device */
	uint8_t blockDataUpdate : 1;	 /* BDU : block data update. 0 - continuous update; 1 - output registers not update till both MSB and LSB are read */
	uint8_t lowPassFilterConfig : 1; /* LPFP_CFG : Configure Low Pass filter on Pressure data */
	uint8_t enLowPassFilter : 1;	 /* EN_LPFP : enable and Low Pass filter on Pressure data */
	uint8_t outputDataRate : 3;		 /* ODR[2:0]: output data rate.Default '000' */
	uint8_t notUsed02 : 1;			 /* This bit must be set to 0 for proper operation of the device */
} PADS_ctrl_1_t;

/**
* Control Register 2
* Address 0x11
* Type  R/W
* Default value: 0x10
*/
typedef struct
{
	uint8_t oneShotbit : 1;		   /* ONE_SHOT : 0  Nornal Operation; 1: start Measurement */
	uint8_t lowNoiseMode : 1;	   /* LOW_NOISE_EN : Enables low noise mode (used only if ODR is lower than 100 Hz). Default value: 0 (0: low-power mode; 1: low-noise mode) */
	uint8_t softwareReset : 1;	   /* swreset :Software reset. 0: normal mode; 1: SW reset;  Self-clearing upon completion */
	uint8_t notUsed01 : 1;		   /* This bit must be set to 0 for proper operation of the device*/
	uint8_t autoAddIncr : 1;	   /* IF_ADD_INC : Register address automatically incremented during a multiple byte access with I2C interface. Default value 1.( 0: disable; 1: enable) */
	uint8_t openDrainOnINTpin : 1; /* PP_OD: Push-pull/open-drain selection on interrupt pad. Default value: 0 (0: push-pull; 1: open-drain) */
	uint8_t intActiveLevel : 1;	   /* INT_H_L : Interrupt active High , active low . Default value : 0 (0:active High; 1:active low ) */
	uint8_t boot : 1;			   /* BOOT :Reboot memory content. 0: normal mode; 1: reboot memory content. Self-clearing upon completion */
} PADS_ctrl_2_t;

/**
* Control Register 3
* Address 0x12
* Type  R/W
* Default value: 0x00
*                     Interrupt configurations
*
*        INT_S1  |   INT_S0    |   INT pin configuration
*   ------------------------------------------------------
*        0       |      0      |     Data signal (in order of priority: DRDY or INT_F_WTM or INT_F_OVR or INT_F_FULL)
*        0       |      1      |     Pressure high event
*        1       |      0      |     Pressure low event
*        1       |      1      |     Pressure low or High event
*
*/
typedef struct
{
	uint8_t intEventCtrl : 2;	  /* INT_S:data signal on INT pad control bits :Default value: 00 */
	uint8_t dataReadyInt : 1;	  /* DRDY : Data-ready signal on INT pin.Default value: 0. (0: Disable; 1 : Enable) */
	uint8_t fifoOverrunInt : 1;	  /* INT_F_OVR  :enable  FIFO overrun Interrupt. Default value: 0.(0: Disable; 1 : Enable) */
	uint8_t fifoThresholdInt : 1; /* INT_F_WTM  : enable  FIFO threshold (watermark). Default value: 0 (0: Disable; 1 : Enable) */
	uint8_t fifoFullInt : 1;	  /* INT_F_FULL : enable  FIFO FUll .Default value: 0; (0: Disable; 1 : Enable) */
	uint8_t notUsed01 : 2;		  /* This bit must be set to 0 for proper operation of the device */
} PADS_ctrl_3_t;

/**
* Control FIFO control register
* Address 0x13
* Type  R/W
* Default value: 0x00
*						FIFO mode selection
*
* TRIG_MODES    |    F_MODE[1:0] |      Mode
* -------------------------------------------------------
*   x           |00              |Bypass
*   0           |01              |FIFO mode
*   0           |1x              |Continuous
*   1           |01              |Bypass-to-FIFO
*   1           |10              |Bypass-to-Continuous
*   1           |11              |Continuous-to-FIFO
*/
typedef struct
{
	uint8_t fifoMode : 3;		 /* [ TRIG_MODES;FMODE[1:0] ]  : select fifo_mode */
	uint8_t stopOnThreshold : 1; /* STOP_ON_WTM : Enable  FIFO watermark level use. Default value: 0(0: disabled; 1: enabled) */
	uint8_t notUsed01 : 4;		 /* These 4 bits must be set to 0 for proper operation of the device */
} PADS_fifo_ctrl_t;

/**
* FIFO threshold setting register
* Address 0x14
* Type  R/W
* Default value: 0x00
*/
typedef struct
{
	uint8_t fifoThreshold : 7; /* WTM[6:0] : FIFO threshold level setting. Default value: 0x00 */
	uint8_t notUsed01 : 1;	   /* This bit must be set to 0 for proper operation of the device */
} PADS_fifo_wtm_t;

/**
* Interrupt source register
* Address 0x24
* read only
* Default value: Output; 0x00
*/
typedef struct
{
	uint8_t diffPresHighEvent : 1; /* PH : Differential pressure High.(0: no interrupt; 1: high differential pressure event has occurred) */
	uint8_t diffPresLowEvent : 1;  /* PL : Differential pressure Low.(0: no interrupt;1: low differential pressure event has occurred) */
	uint8_t intStatus : 1;		   /* IA  : Interrupt active.(0: no interrupt;1: one or more interrupt events have been generated) */
	uint8_t notUsed01 : 4;		   /* These 4 bits must be set to 0 for proper operation of the device*/
	uint8_t bootOn : 1;			   /* BOOT_ON : Indication of Boot phase. (0: Boot phase has ended; 1: Boot phase is running) */
} PADS_intSource_t;

/**
* FIFO Status register 2
* Address 0x26
* read only
* Default value: Output; 0x00
*/
typedef struct
{
	uint8_t notUsed01 : 5;	 /* These 5 bits must be set to 0 for proper operation of the device */
	uint8_t fifoFull : 1;	 /* FIFO_FULL_IA :FIFO full status.(0: FIFO not full; 1: FIFO is full, no samples overwritten) */
	uint8_t fifoOverrun : 1; /* FIFO_OVR_IA :FIFO overrun status. (0: FIFO is not overrun; 1: FIFO is full and at least one sample has been overwritten) */
	uint8_t fifoWtm : 1;	 /* FIFO_WTM_IA : threshold (watermark) status.(0: FIFO level below threshold; 1: FIFO  level equal or higher than treshold) */
} PADS_fifo_status2_t;

/**
* Status register
* Address 0x27
* read only
* Default value: Output; 0x00
*/
typedef struct
{
	uint8_t presDataAvailable : 1; /* P_DA : Pressure data available.(0: pressure data not yet available; 1: a new pressure data is available) */
	uint8_t tempDataAvailable : 1; /* T_DA :Temperature data available. (0: temperature not yet available; 1: a new temperature data is available) */
	uint8_t notUsed01 : 2;		   /* These 2 bits must be set to 0 for proper operation of the device */
	uint8_t presDataOverrun : 1;   /* P_OR : Pressure data overrun. (0: no overrun; 1: pressure data overwritten) */
	uint8_t tempDataOverrun : 1;   /* T_OR : Temperature data overrun. (0: no overrun; 1: Temperature data overwritten) */
	uint8_t notUsed02 : 2;		   /* This 2 bits must be set to 0 for proper operation of the device */
} PADS_status_t;

/**         functional type definition         */

typedef enum
{
	PADS_disable = 0,
	PADS_enable = 1
} PADS_state_t;

typedef enum
{
	PADS_outputDataRatePowerDown = 0, /* single conversion/power down */
	PADS_outputDataRate1HZ = 1,		  /* 1Hz */
	PADS_outputDataRate10HZ = 2,	  /* 10Hz */
	PADS_outputDataRate25HZ = 3,	  /* 25Hz */
	PADS_outputDataRate50HZ = 4,	  /* 50Hz */
	PADS_outputDataRate75HZ = 5,	  /* 75Hz */
	PADS_outputDataRate100HZ = 6,	  /* 100Hz */
	PADS_outputDataRate200HZ = 7	  /* 200Hz */
} PADS_output_data_rate_t;

typedef enum
{
	lPfilterBW1 = 0, /* Bandwidth = outputDataRate / 9 */
	lPfilterBW2 = 1	 /* Bandwidth = outputDataRate / 20 */
} PADS_filter_conf_t;

typedef enum
{
	activeHigh = 0,
	activeLow = 1
} PADS_interrupt_level_t;

typedef enum
{
	pushPull = 0,
	openDrain = 1
} PADS_interrupt_pin_t;

typedef enum
{
	PADS_lowPower = 0, /* Low power mode */
	PADS_lowNoise = 1, /* Low Noise mode */
} PADS_pwr_mode_t;

typedef enum
{
	dataReady = 0, /* Data signal (in order of priority: DRDY or INT_F_WTM or INT_F_OVR or INT_F_FULL) */
	pressureHigh = 1,
	pressureLow = 2,
	pressureHighOrLow = 3
} PADS_interrupt_event_t;

typedef enum
{
	PADS_bypassMode = 0,
	PADS_fifoModes = 1,
	PADS_ContinuousMode = 2,
	PADS_bypassToFifo = 5,
	PADS_bypassToContinuous = 6,
	PADS_continuousToFifo = 7
} PADS_fifo_mode_t;

#ifdef __cplusplus
extern "C"
{
#endif

	/**         Functions definition         */

	int8_t PADS_getDeviceID(uint8_t *devID);

	/* definition of Interrupt functions  */
	int8_t PADS_setAutoRefp(PADS_state_t autorep);
	int8_t PADS_getAutoRefpState(PADS_state_t *autoRefp);
	int8_t PADS_resetAutoRefp(PADS_state_t autorep);

	int8_t PADS_setAutoZeroFunction(PADS_state_t autozero);
	int8_t PADS_getAutoZeroState(PADS_state_t *autoZero);
	int8_t PADS_resetAutoZeroFunction(PADS_state_t autozeroRst);

	int8_t PADS_setDiffPresInt(PADS_state_t diffEn);
	int8_t PADS_getDiffPresIntState(PADS_state_t *diffIntState);

	int8_t PADS_setLatchInt(PADS_state_t state);
	int8_t PADS_getLatchIntState(PADS_state_t *latchInt);

	int8_t PADS_setLowPresInt(PADS_state_t state);
	int8_t PADS_getLowPresIntState(PADS_state_t *lpint);
	int8_t PADS_setHighPresInt(PADS_state_t state);
	int8_t PADS_getHighPresIntState(PADS_state_t *hpint);

	int8_t PADS_getIntStatus(PADS_state_t *intState);
	int8_t PADS_getLowPresIntStatus(PADS_state_t *lpState);
	int8_t PADS_getHighPresIntStatus(PADS_state_t *hpState);

	int8_t PADS_setFifoFullInt(PADS_state_t fullState);
	int8_t PADS_setFifoWtmInt(PADS_state_t WtmState);
	int8_t PADS_setFifoOvrInt(PADS_state_t OvrState);

	int8_t PADS_setFifoOvrInterrupt(PADS_state_t OvrState);

	int8_t PADS_getFifoFullState(PADS_state_t *fifoFull);
	int8_t PADS_getFifoWtmState(PADS_state_t *fifoWtm);
	int8_t PADS_getFifoOvrState(PADS_state_t *fifoOvr);

	int8_t PADS_setDrdyInt(PADS_state_t DRDYStatus);
	int8_t PADS_getDrdyIntState(PADS_state_t *drdy);

	int8_t PADS_setIntEventCtrl(PADS_interrupt_event_t ctr);
	int8_t PADS_getIntEventCtrlState(PADS_interrupt_event_t *intEvent);

	int8_t PADS_setPresThrLSB(uint8_t thr);
	int8_t PADS_setPresThrMSB(uint8_t thr);
	int8_t PADS_getPresThrLSB(uint8_t *thrLSB);
	int8_t PADS_getPresThrMSB(uint8_t *thrMSB);

	/* Standard Configurations */
	int8_t PADS_disI2C(PADS_state_t I2CStatus);
	int8_t PADS_getI2CState(PADS_state_t *i2cState);
	int8_t PADS_disPullDownIntPin(PADS_state_t pullDown);
	int8_t PADS_getPullDownIntPinState(PADS_state_t *pinState);
	int8_t PADS_setSAOPullUp(PADS_state_t SaoStatus);
	int8_t PADS_getSAOPullUpState(PADS_state_t *saoPinState);
	int8_t PADS_setSDAPullUp(PADS_state_t SdaStatus);
	int8_t PADS_getSDAPullUpState(PADS_state_t *sdaPinState);

	int8_t PADS_setOutputDataRate(PADS_output_data_rate_t odr);
	int8_t PADS_getOutputDataRate(PADS_output_data_rate_t *odr);

	int8_t PADS_setLowPassFilter(PADS_state_t filterStatus);
	int8_t PADS_getLowPassFilterStatus(PADS_state_t *filterStatus);
	int8_t PADS_setLowPassFilterConf(PADS_filter_conf_t conf);
	int8_t PADS_getLowPassFilterConf(PADS_filter_conf_t *conf);

	int8_t PADS_setBlockDataUpdate(PADS_state_t bdu);
	int8_t PADS_getBlockDataUpdateState(PADS_state_t *bdu);

	int8_t PADS_rebootMemory(PADS_state_t reboot);
	int8_t PADS_getRebootMemoryState(PADS_state_t *reboot);
	int8_t PADS_getBootStatus(PADS_state_t *boot);

	int8_t PADS_setIntActiveLevel(PADS_interrupt_level_t intLvl);
	int8_t PADS_getIntActiveLevel(PADS_interrupt_level_t *intLvl);

	int8_t PADS_setIntPinType(PADS_interrupt_pin_t intr);
	int8_t PADS_getIntPinType(PADS_interrupt_pin_t *intr);

	int8_t PADS_setAutoIncrement(PADS_state_t inc);
	int8_t PADS_getAutoIncrementState(PADS_state_t *inc);

	int8_t PADS_setSwreset(PADS_state_t mode);
	int8_t PADS_getSwresetState(PADS_state_t *mode);

	int8_t PADS_setPowerMode(PADS_pwr_mode_t mode);
	int8_t PADS_getPowerMode(PADS_pwr_mode_t *mode);

	int8_t PADS_setSingleConvMode(PADS_state_t mode);
	int8_t PADS_getSingleConvModeState(PADS_state_t *mode);

	/* pressure offset value */
	int8_t PADS_setPresOffsetLSB(uint8_t offSet);
	int8_t PADS_getPresOffsetLSB(uint8_t *offSet);
	int8_t PADS_setPresOffsetMSB(uint8_t offSet);
	int8_t PADS_getPresOffsetMSB(uint8_t *offSet);

	/* SET FIFO CTRL_REG */
	int8_t PADS_setFifoMode(PADS_fifo_mode_t fifoMode);
	int8_t PADS_getFifoMode(PADS_fifo_mode_t *fifoMode);

	int8_t PADS_setStopOnThr(PADS_state_t state);
	int8_t PADS_getStopOnThrStatus(PADS_state_t *state);

	int8_t PADS_setFifoThr(uint8_t fifoThr);
	int8_t PADS_getFifoThr(uint8_t *fifoThr);

	int8_t PADS_getFifoFillLevel(uint8_t *fifoLevel);

	/* Getting Reference pressure value */
	int8_t PADS_getPresRefLSB(uint8_t *lowReferenceValue);
	int8_t PADS_getPresRefMSB(uint8_t *highReferenceValue);

	/* Temperature and Pressure data overrun State */
	int8_t PADS_getTemperOverrunStatus(PADS_state_t *state);
	int8_t PADS_getPressureOverrunStatus(PADS_state_t *state);
	/* Temperature and Pressure data available State */
	int8_t PADS_getTempStatus(PADS_state_t *state);
	int8_t PADS_getPresStatus(PADS_state_t *state);

	/* standard Data Out */
	int8_t PADS_getRAWPressure(int32_t *rawPres);
	int8_t PADS_getRAWTemperature(int16_t *rawTemp);
	int8_t PADS_getPressure(float *presskPa);	 // Pressure Value in kPa
	int8_t PADS_getTemperature(float *tempdegC); // Temperature Value in °C

	/*Fifo Data Out */
	int8_t PADS_getFifoRAWTemperature(int16_t *rawTemp);
	int8_t PADS_getFifoRAWPressure(int32_t *rawPres);
	int8_t PADS_getFifoTemperature(float *tempdegC); // Temperature Value in °C
	int8_t PADS_getFifoPressure(float *presskPa);	 // Pressure Value in kPa

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_PADS_H */
	   /**         EOF         */