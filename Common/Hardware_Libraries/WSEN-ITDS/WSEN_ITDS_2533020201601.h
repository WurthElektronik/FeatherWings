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

#ifndef _WSEN_ITDS_H
#define _WSEN_ITDS_H

#include <stdint.h>
#include "ConfigPlatform.h"

#define ITDS_DEVICE_ID_VALUE 0x44 /* this is the expected answer when requesting the ITDS_DEVICE_ID_REG */
#define ITDS_ADDRESS_I2C_0 0x18	  /* when SAO of ITDS is connected to logic HIGH level */
#define ITDS_ADDRESS_I2C_1 0x19	  /* when SAO of ITDS is connected to logic LOW level */

/** Register address definitions **/

#define ITDS_T_OUT_L_REG 0x0D	/* Temperature Output LSB value Register */
#define ITDS_T_OUT_H_REG 0x0E	/* Temperature Output MSB value Register */
#define ITDS_DEVICE_ID_REG 0x0F /* Device ID register */
/* Registers 0x10 - 0x1F are reserved. They contain Factory Calibraion values that shall not be changed */
#define ITDS_CTRL_1_REG 0x20		/* Control Register 1 */
#define ITDS_CTRL_2_REG 0x21		/* Control Register 2 */
#define ITDS_CTRL_3_REG 0x22		/* Control Register 3 */
#define ITDS_CTRL_4_REG 0x23		/* Control Register 4 */
#define ITDS_CTRL_5_REG 0x24		/* Control Register 5 */
#define ITDS_CTRL_6_REG 0x25		/* Control Register 6 */
#define ITDS_T_OUT_REG 0x26			/* Temperature Output Data in 8 bit resolution Register */
#define ITDS_STATUS_REG 0x27		/* Status Register */
#define ITDS_X_OUT_L_REG 0x28		/* X axis accelaration Output LSB value Register */
#define ITDS_X_OUT_H_REG 0x29		/* X axis accelaration Output MSB value Register */
#define ITDS_Y_OUT_L_REG 0x2A		/* Y axis accelaration Output LSB value Register */
#define ITDS_Y_OUT_H_REG 0x2B		/* Y axis accelaration Output MSB value Register */
#define ITDS_Z_OUT_L_REG 0x2C		/* Z axis accelaration Output LSB value Register */
#define ITDS_Z_OUT_H_REG 0x2D		/* Z axis accelaration Output MSB value Register */
#define ITDS_FIFO_CTRL_REG 0x2E		/* FIFO Control Register */
#define ITDS_FIFO_SAMPLES_REG 0x2F	/* FIFO Samples Register */
#define ITDS_TAP_X_TH_REG 0x30		/* Tap Recognition Threshold on X diraction Register */
#define ITDS_TAP_Y_TH_REG 0x31		/* Tap Recognition Threshold on Y diraction Register */
#define ITDS_TAP_Z_TH_REG 0x32		/* Tap Recognition Threshold on Z diraction Register */
#define ITDS_INT_DUR_REG 0x33		/* Interrupt duration Register */
#define ITDS_WAKE_UP_TH_REG 0x34	/* Wake up Threshold Register */
#define ITDS_WAKE_UP_DUR_REG 0x35	/* Wake up duration Register */
#define ITDS_FREE_FALL_REG 0x36		/* Free Fall Register */
#define ITDS_STATUS_DETECT_REG 0x37 /* Status Detect Register */
#define ITDS_WAKE_UP_EVENT_REG 0x38 /* Wake up Event Register */
#define ITDS_TAP_EVENT_REG 0x39		/* Tap Event Register  */
#define ITDS_6D_EVENT_REG 0x3A		/* 6D Event Register */
#define ITDS_ALL_INT_EVENT_REG 0x3B /* All Interrupt Event Register */
#define ITDS_X_OFS_USR_REG 0x3C		/* Offset value on X axis data Register */
#define ITDS_Y_OFS_USR_REG 0x3D		/* Offset value on Y axis data Register */
#define ITDS_Z_OFS_USR_REG 0x3E		/* Offset value on Z axis data Register */
#define ITDS_CTRL_7_REG 0x3F		/* Control Register 7 */

/** Register type definitions **/

/**
* CTR_1_REG
* Address 0x20
* Type  R/W
* Default value: 0x00

ODR[3:0]   |    Power down / data rate configuration
--------------------------------------------------------------------------
	0000    |                Power down
				High performance     Normal mode   Low power mode
	0001    |    12.5 Hz                12.5 Hz        1.6 Hz
	0010    |    12.5 Hz                12.5 Hz        12.5 Hz
	0011    |    25 Hz                  25 Hz          25 Hz
	0100    |    50 Hz                  50 Hz          50 Hz
	0101    |    100 Hz                 100 Hz         100 Hz
	0110    |    200 Hz                 200 Hz         200 Hz
	0111    |    400 Hz                 200 Hz         200 Hz
	1000    |    800 Hz                 800 Hz         200 Hz
	1001    |    1600Hz                 1600Hz         200 Hz


MODE[1:0] |             Operating mode and resolution
--------------------------------------------------------------------------------
	00      |       Normal mode (14-bit resolution) / Low power mode (12-bit resolution)
	01      |       High performance mode(14-bit resolution)
	10      |       Single data conversion on demand mode (12/14-bit resolution)
	11      |                          -
*/
typedef struct
{
	uint8_t powerMode : 2;		/* LP_MODE[1:0] : select Normal Mode or low power Mode. Default 00 [00:low power mode ; 10:normal mode] */
	uint8_t operatingMode : 2;	/* MODE[1:0] : select the operating Mode and Resolution. Default 00 */
	uint8_t outputDataRate : 4; /* ODR[3:0]:output data rate selection. Default 0000 */
} ITDS_ctrl_1_t;

/**
 * CTR_2_REG
 * Address 0x21
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t not_used_01 : 1;	 /* This bit must be set to 0 for proper operation of the device */
	uint8_t i2cDisable : 1;		 /* I2C_DISABLE : disable I2C digital Interface.default value:0(0: enabled ; 1:disabled) */
	uint8_t autoAddIncr : 1;	 /* IF_ADD_INC : Register address automatically incremented during a multiple byte access with I2C interface. Default value 0.( 0: disable; 1: enable) */
	uint8_t blockDataUpdate : 1; /* BDU : block data update. 0 - continuous update; 1 - output registers not updated until MSB and LSB reading. */
	uint8_t disCSPullup : 1;	 /* CP_PU_DISC  :disconnect pull up to CS pin .default value:0 (0:connected ; 1:disconnected) */
	uint8_t not_used_02 : 1;	 /* This bit must be set to 0 for proper operation of the device */
	uint8_t softReset : 1;		 /* SOFT_RESET :Software reset. 0: normal mode; 1: SW reset ;  Self-clearing upon completion */
	uint8_t boot : 1;			 /* BOOT : This bit is set to ’1’ during boot sequence.default value:0 (0: enabled ; 1:disabled) .Self-clearing upon completion */
} ITDS_ctrl_2_t;

/**
* CTR_3_REG
* Address 0x22
* Type  R/W
* Default value: 0x00


	ST[1:0]    |     Self-test mode
-------------------------------------------
	00         |     Normal mode
	01         |     Positive sign self-test
	10         |     Negative sign self-test
	11         |              -
*/
typedef struct
{
	uint8_t enSingleDataConv : 1;	/* SLP_MODE_1 : enable Single data conversion on demand mode */
	uint8_t singleConvTrigger : 1;	/* SLP_MODE_SEL :Single data conversion on demand selection. 0: enabled by external trigger signal on INT_1, 1: enabled by I2C writing SLP_MODE_1 to 1 */
	uint8_t not_used_01 : 1;		/* This bit must be set to 0 for proper operation of the device */
	uint8_t intActiveLevel : 1;		/* H_LACTIVE : Interrupt active High , active low . Default value : 0 (0:active High ; 1:active low ) */
	uint8_t enLatchedInterrupt : 1; /* LIR: enable latched Interrupt default value 0 .(0: normal; 1:enabled) */
	uint8_t intPinConf : 1;			/* PP_OD: Push-pull/open-drain selection on interrupt pad. Default value: 0 (0: push-pull; 1: open-drain) */
	uint8_t selfTestMode : 2;		/* ST[1:0] : select self test Mode.default value 00. */
} ITDS_ctrl_3_t;

/**
 * CTR_4_REG
 * Address 0x23
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t dataReadyINT0 : 1;	   /* INT0_DRDY : Data-Ready interrupt signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t fifoThresholdINT0 : 1; /* INT0_FTH : FIFO threshold interrupt signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled)) */
	uint8_t fifoFullINT0 : 1;	   /* INT0_DIFF5:FIFO full recognition signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t doubleTapINT0 : 1;	   /* INT0_TAP : Double-tap recognition signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t freeFallINT0 : 1;	   /* INT0_FF :Free-fall recognition signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t wakeupINT0 : 1;		   /* INT0_WU:Wakeup recognition signal is routed to INT_0 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t singleTapINT0 : 1;	   /* INT0_SINGLE_TAP :Single-tap recognition signal is routed to INT_0 pin: Default:0 (0: disabled, 1:enabled) */
	uint8_t sixDINT0 : 1;		   /* INT0_6D : 6D recognition signal is routed to INT_0 pin.Default:0 (0: disabled, 1:enabled) */
} ITDS_ctrl_4_t;

/**
 * CTR_5_REG
 * Address 0x24
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t dataReadyINT1 : 1;		/* INT1_DRDY : Data-Ready interrupt signal is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t fifoThresholdINT1 : 1;	/* INT1_FTH : FIFO threshold interrupt signal is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled)) */
	uint8_t fifoFullINT1 : 1;		/* INT1_DIFF5:FIFO full recognition signal is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t fifoOverrunINT1 : 1;	/* INT1_OVR :FIFO overrun interrupt signal is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t tempDataReadyINT1 : 1;	/* INT1_DRDY_T :Temperature data-ready interrupt signal  is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t bootStatusINT1 : 1;		/* INT1_BOOT : Boot status interrupt signal is routed to INT_1 pin. Default value: 0 (0: disabled, 1:enabled) */
	uint8_t activeStatusINT1 : 1;	/* INT1_SINGLE_CHG: Sleep change status interrupt signal is routed to INT_1 pin. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t inactiveStatusINT1 : 1; /* INT1_SLEEP_STATE Sleep state interrupt signal is routed to INT_1 pin. Default: 0 (0: disabled, 1:enabled) */
} ITDS_ctrl_5_t;

/**
* CTR_6_REG
* Address 0x25
* Type  R/W
* Default value: 0x00


	BW_FILT[1:0]    |          Bandwidth selection
-------------------------------------------------------------
		00          |    ODR/2 (except for ODR = 1600 Hz, 400 Hz)
		01          |    ODR/4 (High pass / Low pass filter)
		10          |    ODR/10 (High pass / Low pass filter)
		11          |    ODR/20 (High pass / Low pass filter)



		FS[1:0]    |   Full scale selection
	-------------------------------
			00     |      ±2g
			01     |      ±4g
			10     |      ±8g
			11     |      ±16g

*/
typedef struct
{
	uint8_t not_used_01 : 1;	 /* This bit must be set to 0 for proper operation of the device */
	uint8_t not_used_02 : 1;	 /* This bit must be set to 0 for proper operation of the device */
	uint8_t enLowNoise : 1;		 /* LOW_NOISE : low noise configuration (0:disabled;1:enabled) */
	uint8_t filterPath : 1;		 /* FDS : Filtered data type selection.default value:0 . (0:low pass filter ; 1:high pass filter) */
	uint8_t fullScale : 2;		 /* FS[1:0] : Full scale selection : */
	uint8_t filterBandwidth : 2; /* BW_FILT[1:0] : Filter Bandwidth Selection  */
} ITDS_ctrl_6_t;

/**
 * STATUS_REG
 * Address 0x27
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t dataReady : 1;	   /* DRDY : Data-ready status bit (0: not ready, 1: X-, Y- and Z-axis new data available) */
	uint8_t freeFall : 1;	   /* FF_IA: Free-fall event detection bit (0: free-fall event not detected; 1: free-fall event detected) */
	uint8_t sixDDetection : 1; /* 6D_IA :Source of change in position portrait/landscape/face-up/face-down. (0: no event detected, 1: a change in position detected) */
	uint8_t singleTap : 1;	   /* SINGLE_TAP : Single-tap event status bit (0: Single-tap event not detected, 1: Single-tap event detected) */
	uint8_t doubleTap : 1;	   /* DOUBLE_TAP :Double-tap event status bit (0: Double-tap event not detected, 1:Double-tap event detected) */
	uint8_t sleep : 1;		   /* SLEEP_STATE : Sleep event status bit (0: Sleep event not detected, 1: Sleep event detected) */
	uint8_t wakeup : 1;		   /* WU_IA : Wakeup event detection status bit (0: Wakeup event not detected, 1:Wakeup event detected) */
	uint8_t fifoThreshold : 1; /* FIFO_THS : FIFO threshold status bit (0: FIFO filling is lower than threshold level, 1:FIFO filling is equal to or higher than the threshold level.)) */
} ITDS_status_t;

/**
* FIFO_CTRL_REG
* Address 0x2E
* Type  R/W
* Default value: 0x00


	FMODE[2:0]     |             Mode Description
	--------------------------------------------------------------------
	000            |     Enable Bypass mode and FIFO buffer is turned off(not active)
	001            |     Enable FIFO mode
	010            |     Reserved
	011            |     Enable Continuous to FIFO mode
	100            |     Enable Bypass to Continuous mode
	101            |     Reserved
	110            |     Enable continuous mode
	111            |     Reserved
*/
typedef struct
{
	uint8_t fifoThresholdLevel : 5; /* FTH[4:0] : set the FIFO threshold Level */
	uint8_t fifoMode : 3;			/* FMODE[2:0] : select the FIFO Mode */
} ITDS_fifo_ctrl_t;

/**
 * FIFO_SAMPLES_REG
 * Address 0x2F
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t fifoFillLevel : 6;		/* Diff[5:0] :Defines the number of unread samples stored in FIFO. (’000000’ = FIFO empty, ’100000’ = FIFO full, 32 unread samples) */
	uint8_t fifoOverrunState : 1;	/* FIFO_OVR : FIFO overrun status. (0: FIFO is not completely filled, 1: FIFO is completely filled and at least one sample has been overwritten) */
	uint8_t fifoThresholdState : 1; /* FIFO_FTH : FIFO threshold status bit. (0: FIFO filling is lower than threshold level, 1:FIFO filling is equal to or higher than the threshold level) */
} ITDS_fifo_samples_t;

/**
* TAP_X_TH_REG
* Address 0x30
* Type  R/W
* Default value: 0x00


	6D_THS[1:0]  |   Threshold definition (degrees)
-------------------------------------------
		00       |           6 (80 degrees)
		01       |           11(70 degrees)
		10       |           16(60 degrees)
		11       |           21(50 degrees)

*/
typedef struct
{
	uint8_t XAxisTapThreshold : 5;	/* TAP_THSX_[4:0] :Threshold for TAP recognition at FS = ±2g on X direction */
	uint8_t sixDThreshold : 2;		/* 6D_THS[1:0] :Threshold definition (degrees) */
	uint8_t fourDPositionState : 1; /* 4D_EN :4D detection portrait/landscape position enable. (0: no position detected; 1: portrait/landscape detection and face-up/face-down position enabled) */
} ITDS_tap_x_th_t;

/**
* TAP_Y_TH_REG
* Address 0x31
* Type  R/W
* Default value: 0x00

	TAP_PRIOR[2:0]  |  Max Priority  | Mid Priority  | Min Priority
	------------------------------------------------------
		000         |    X           |     Y         |    Z
		001         |    Y           |     X         |    Z
		010         |    X           |     Z         |    Y
		011         |    Z           |     Y         |    X
		100         |    X           |     Y         |    Z
		101         |    Y           |     Z         |    X
		110         |    Z           |     X         |    Y
		111         |    Z           |     Y         |    X

*/
typedef struct
{
	uint8_t YAxisTapThreshold : 5; /* TAP_THSY_[4:0] :Threshold for TAP recognition at FS = ±2g on Y direction */
	uint8_t tapAxisPriority : 3;   /* TAP_PRIOR[2:0]: select the Axis priority for tap detection */
} ITDS_tap_y_th_t;

/**
 * TAP_Z_TH_REG
 * Address 0x32
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t ZAxisTapThreshold : 5; /* TAP_THSZ_[4:0] Threshold for tap recognition at FS: ±2g on Z direction. */
	uint8_t enTapZ : 1;			   /* TAP_Z_EN Enables Z direction in tap recognition. (0: disabled, 1: enabled) */
	uint8_t enTapY : 1;			   /* TAP_Y_EN Enables Y direction in tap recognition. (0: disabled, 1: enabled) */
	uint8_t enTapX : 1;			   /* TAP_X_EN Enables X direction in tap recognition. (0: disabled, 1: enabled) */
} ITDS_tap_z_th_t;

/**
 * INT_DUR_REG
 * Address 0x33
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t shock : 2;	 /* SHOCK[1:0]:It defines the maximum duration of over-threshold event*/
	uint8_t quiet : 2;	 /* QUIET[1:0]:It defines the expected quiet time after a tap detection*/
	uint8_t latency : 4; /* LATENCY[3:0]:It defines the maximum duration time gap for double-tap recognition*/
} ITDS_int_dur_t;

/**
 * WAKE_UP_TH_REG
 * Address 0x34
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t wakeupThreshold : 6;  /* WK_THS[5:0] : Defines Wakeup threshold, 6-bit unsigned 1 LSB = 1/64 of FS. Default value: 000000*/
	uint8_t enInactivtyEvent : 1; /* SLEEP_ON : enables inactivity(sleep). Default value: 0 (0: sleep disabled, 1: sleep enabled) */
	uint8_t enTapEvent : 1;		  /* SINGLE_DOUBLE_TAP:Enable single/double-tap event. Default value: 0 (0: enable only single-tap, 1: enable both : single and double-tap) */
} ITDS_wake_up_th_t;

/**
 * WAKE_UP_DUR_REG
 * Address 0x35
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t sleepDuration : 4;		 /* SLEEP_DUR[3:0] : Defines the sleep mode duration. Default value is SLEEP_DUR[3:0] = 0000 (which is 16 * 1/ODR) 1 LSB = 512 * 1/ODR */
	uint8_t enStationary : 1;		 /* STATIONARY :Enables stationary detection / motion detection with no automatic ODR change when detecting stationary state. Default value: 0 (0:disabled, 1: enabled) */
	uint8_t wakeupDuration : 2;		 /* WAKE_DUR[1:0] :This bit defines Wakeup duration. 1 LSB = 1 *1/ODR */
	uint8_t freeFallDurationMSB : 1; /* FF_DUR5 :this bit defines Free-fall duration. Combined with FF_DUR [4:0] bit in FREE_FALL (0x36) register. 1 LSB = 1 * 1/ODR */
} ITDS_wake_up_dur_t;

/**
* FREE_FALL_REG
* Address 0x36
* Type  R/W
* Default value: 0x00

	FF_TH[2:0]  |  Threshold decoding (LSB)
-----------------------------------------
		000     |        5
		001     |        7
		010     |        8
		011     |        10
		100     |        11
		101     |        13
		110     |        15
		111     |        16
*/
typedef struct
{
	uint8_t freeFallThreshold : 3;	 /* FF_TH[2:0] :Threshold decoding (LSB) */
	uint8_t freeFallDurationLSB : 5; /* FF_DUR[4:0]:Defines Free-fall duration. Combined with FF_DUR5 bit in WAKE_UP_DUR (0x35) register. 1 LSB = 1 * 1/ODR */
} ITDS_free_fall_t;

/**
 * STATUS_DETECT_REG
 * Address 0x37
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t drdy : 1;			  /* DRDY Defines Data-ready status (0: not ready, 1: X-, Y- and Z-axis new data available) */
	uint8_t freeFallState : 1;	  /* FF_IA Defines Free-fall event detection status (0: free-fall event not detected,1: free-fall event detected) */
	uint8_t sixDState : 1;		  /* 6D_IA Defines the source of change in position portrait/landscape/face-up/face-down (0: no event detected, 1: a change in position is detected) */
	uint8_t singleTapState : 1;	  /* SINGLE_TAP Enables Single-tap event status (0: Single-tap event not detected; 1: Single-tap event detected) */
	uint8_t doubleTapState : 1;	  /* DOUBLE_TAP Enables Double-tap event status (0: Double-tap event not detected, 1: Double-tap event detected) */
	uint8_t sleepState : 1;		  /* SLEEP_STATE_IA : Defines sleep event status (0: Sleep event not detected, 1: Sleep event detected) */
	uint8_t tempDrdy : 1;		  /* DRDY_T Defines the temperature status (0: data not available, 1: a new set of data is available) */
	uint8_t fifoOverrunState : 1; /* OVR Defines the FIFO overrun status (0: FIFO is not completely filled, 1:FIFO is completely filled and at least one sample has been overwritten) */
} ITDS_status_detect_t;

/**
 * WAKE_UP_EVENT_REG
 * Address 0x38
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t wakeUpZ : 1;	   /* Z_WU Enables Wake-up event detection status on Z-axis (0: Wake-up event on Z not detected, 1: Wakeup event on Z-axis is detected) */
	uint8_t wakeUpY : 1;	   /* Y_WU Enables Wake-up event detection status on Y-axis (0: Wake-up event on Y not detected, 1: Wakeup event on Y-axis is detected) */
	uint8_t wakeUpX : 1;	   /* X_WU Enables Wake-up event detection status on X-axis (0: Wake-up event on X not detected; 1: Wake-up event on X-axis is detected) */
	uint8_t wakeUpState : 1;   /* WU_IA Defines the Wake-up event detection status (0: Wake-up event not detected, 1: Wakeup event is detected) */
	uint8_t sleepState : 1;	   /* SLEEP_STATE_IA Defines the Sleep event status (0: Sleep event not detected, 1: Sleep event detected) */
	uint8_t freeFallState : 1; /* FF_IA Defines the Free-fall event detection status (0: FF event not detected,1: FF event detected) */
	uint8_t not_used_01 : 1;   /* This bit must be set to 0 for proper operation of the device */
	uint8_t not_used_02 : 1;   /* This bit must be set to 0 for proper operation of the device */
} ITDS_wake_up_event_t;

/**
 * TAP_EVENT_REG
 * Address 0x39
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t tapZAxis : 1;	   /* Z_TAP :Defines the Tap event detection status on Z-axis (0: Tap event on Z not detected, 1: Tap event on Z-axis is detected) */
	uint8_t tapYAxis : 1;	   /* Y_TAP :Defines Tap event detection status on Y-axis (0: Tap event on Y not detected, 1: Tap event on Y-axis is detected) */
	uint8_t tapXAxis : 1;	   /* X_TAP :Defines Tap event detection status on X-axis (0: Tap event on X not detected, 1: Defines Tap event on X-axis is detected) */
	uint8_t tapSign : 1;	   /* TAP_SIGN :Defines the sign of acceleration detected by tap event (0: positive sign of acceleration detected, 1: negative sign of acceleration detected) */
	uint8_t doubleState : 1;   /* DOUBLE_TAP :Defines the Double-tap event status (0: double-tap event not detected,1: double-tap event detected) */
	uint8_t singleState : 1;   /* SINGLE_TAP: Defines the single-tap event status (0: single-tap event not detected, 1:single-tap event detected) */
	uint8_t tapEventState : 1; /* TAP_IA :Defines the Tap event status (0: tap event not detected, 1: tap event detected) */
	uint8_t not_used_01 : 1;   /* This bit must be set to 0 for proper operation of the device */
} ITDS_tap_event_t;

/**
 * 6D_EVENT_REG
 * Address 0x3A
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t XLOverThreshold : 1; /* XL :Defines the XL over threshold (0: XL does not exceed the threshold, 1:XL is over the threshold) */
	uint8_t XHOverThreshold : 1; /* XH :Defines the XH over threshold (0: XH does not exceed the threshold,1: XH is over the threshold) */
	uint8_t YLOverThreshold : 1; /* YL :Defines the YL over threshold (0: YL does not exceed the threshold, 1:YL is over the threshold) */
	uint8_t YHOverThreshold : 1; /* YH :Defines the YH over threshold (0: YH does not exceed the threshold,1: YH is over the threshold) */
	uint8_t ZLOverThreshold : 1; /* ZL :Defines the ZL over threshold (0: ZL does not exceed the threshold, 1:ZL is over the threshold) */
	uint8_t ZHOverThreshold : 1; /* ZH :Defines the ZH over threshold (0: ZH does not exceed the threshold, 1:ZH is over the threshold) */
	uint8_t sixDChange : 1;		 /* 6D_IA :Defines the source of change in position portrait/landscape/face-up/face-down (0: no event detected, 1: a change in position is detected) */
	;
	uint8_t not_used_01 : 1; /* This bit must be set to 0 for proper operation of the device */
} ITDS_event_6D_t;

/**
 * ALL_INT_EVENT_REG
 * Address 0x3B
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t freeFallState : 1;	/* FF_IA Defines the Free-fall event detection status (0: free-fall event not detected, 1: free-fall event detected) */
	uint8_t wakeupState : 1;	/* WU_IA Defines the Wakeup event detection status (0: wakeup event not detected, 1: wakeup event detected) */
	uint8_t singleTapState : 1; /* SINGLE _TAP Defines the single-tap event status (0: single-tap event not detected, 1:single-tap event detected) */
	uint8_t doubleTapState : 1; /* DOUBLE_TAP Defines the double-tap event status (0: double-tap event not detected,1: double-tap event detected) */
	uint8_t sixDState : 1;		/* 6D_IA Defines the source of change in position portrait/landscape/face-up/face-down (0: no event detected; 1: a change in position detected) */
	uint8_t sleepState : 1;		/* SLEEP_CHANGE_IA : Defines the sleep change status (0: Sleep change not detected; 1: Sleep change detected) */
	uint8_t not_used_01 : 1;	/* This bit must be set to 0 for proper operation of the device */
	uint8_t not_used_02 : 1;	/* This bit must be set to 0 for proper operation of the device */
} ITDS_all_int_event_t;

/**
 * CTRL_7_REG
 * Address 0x3F
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
	uint8_t lowPassOn6D : 1;		 /* 0: ODR/2 low pass filtered data sent to 6D interrupt function (default), 1: LPF_1 output data sent to 6D interrupt function) */
	uint8_t highPassRefMode : 1;	 /* HP_REF_MODE :Enables high-pass filter reference mode (0: high-pass filter reference mode disabled (default), 1: high-pass filter reference mode enabled) */
	uint8_t userOffset : 1;			 /* USR_OFF_W :Defines the selection of weight of the user offset words specified byX_OFS_USR[7:0], Y_OFS_USR[7:0] and Z_OFS_USR[7:0] bits (0:977 µg /LSB, 1: 15.6 mg /LSB) */
	uint8_t applyWakeUpOffsetXL : 1; /* USR_OFF_ON_WU :Enable application of user offset value on XL data for wakeup function only */
	uint8_t applyOffsetXL : 1;		 /* USR_OFF_ON_OUT :Enable application of user offset value on XL output data registers.FDS :bit in CTRL_6 (0x25) must be set to ’0’-logic (low-pass path selected) */
	uint8_t enInterrupts : 1;		 /* INTERRUPTS_ENABLE :Enables interrupts */
	uint8_t INT1toINT0 : 1;			 /* INT1_ON_INT0 :Defines the signal routing (1: all signals available only on INT_1 :are routed on INT_0) */
	uint8_t drdyPulse : 1;			 /* DRDY_PULSED :Defines the switches between latched and pulsed mode for data ready interrupt (0: latched mode is used, 1: pulsed mode enabled for data-ready) */

} ITDS_ctrl_7_t;

typedef enum
{
	ITDS_disable = 0,
	ITDS_enable = 1
} ITDS_state_t;

typedef enum
{
	negative = 0,
	positive = 1
} ITDS_tapSign_t;

typedef enum
{
	odr0, /*Power down */
	/* High performance     Normal mode   Low power mode */
	odr1, /* 12.5 Hz                12.5 Hz        1.6 Hz      */
	odr2, /* 12.5 Hz                12.5 Hz        12.5 Hz     */
	odr3, /* 25 Hz                  25 Hz          25 Hz       */
	odr4, /* 50 Hz                  50 Hz          50 Hz       */
	odr5, /* 100 Hz                 100 Hz         100 Hz      */
	odr6, /* 200 Hz                 200 Hz         200 Hz      */
	odr7, /* 400 Hz                 200 Hz         200 Hz      */
	odr8, /* 800 Hz                 800 Hz         200 Hz      */
	odr9  /* 1600Hz                 1600Hz         200 Hz      */
} ITDS_output_Data_Rate;

typedef enum
{
	normalOrLowPower,
	highPerformance,
	singleConvertion
} ITDS_operating_mode;

typedef enum
{
	ITDS_lowPower,
	ITDS_normalMode
} ITDS_power_mode;

typedef enum
{
	off = 0,
	positiveAxis = 1,
	negativeAxis = 2
} ITDS_self_Test_Config;

typedef enum
{
	ITDS_pushPull = 0,
	ITDS_openDrain = 1
} ITDS_interrupt_Pin_Config;

typedef enum
{
	ITDS_activeHigh = 0,
	ITDS_activeLow = 1
} ITDS_interrupt_Active_Level;

typedef enum
{
	externalTrigger = 0,
	I2C = 1
} ITDS_singleDataConversion_Input;

typedef enum
{
	outputDataRate_2 = 0,  /* ODR/2 (except for ODR = 1600 Hz, 400 Hz) */
	outputDataRate_4 = 1,  /* ODR/4 (High pass / Low pass filter) */
	outputDataRate_10 = 2, /* ODR/10 (High pass / Low pass filter) */
	outputDataRate_20 = 3  /* ODR/20 (High pass / Low pass filter) */
} ITDS_bandwidth;

typedef enum
{
	twoG = 0,	 /* ±2g */
	fourG = 1,	 /* ±4g */
	eightG = 2,	 /* ±8g */
	sixteenG = 3 /* ±16g */
} ITDS_full_Scale;

typedef enum
{
	lowPass = 0,
	highPass = 1
} ITDS_filter_Type;

typedef enum
{
	ITDS_bypassMode = 0,
	ITDS_fifoModes = 1,
	ITDS_continuousToFifo = 3,
	ITDS_bypassToContinuous = 4,
	ITDS_continuousMode = 6
} ITDS_Fifo_Mode;

typedef enum
{
	eightyDeg = 0,	/* 6 (80 degrees) */
	SeventyDeg = 1, /* 11 (70 degrees) */
	sixtyDeg = 2,	/* 16 (60 degrees) */
	fiftyDeg = 3	/* 21 (50 degrees) */
} ITDS_threshold_Degree;

typedef enum
{
	X_Y_Z = 0,
	Y_X_Z = 1,
	X_Z_Y = 2,
	Z_Y_X = 3,
	Y_Z_X = 5,
	Z_X_Y = 6
} ITDS_tap_Axis_Priority;

typedef enum
{
	five = 0,
	seven = 1,
	eight = 2,
	ten = 3,
	eleven = 4,
	thirteen = 5,
	fifteen = 6,
	sixteen = 7,
} ITDS_Free_Fall_Thr;

typedef enum
{
	latched = 0,
	pulsed = 1
} ITDS_drdy_pulse;

#ifdef __cplusplus
extern "C"
{
#endif

	/* **********************/
	/* Functions definition */
	/* **********************/

	/* DEVICE_ID */
	int8_t ITDS_getDeviceID(uint8_t *device_id);

	/* CTRL-REG 1 */
	int8_t ITDS_setOutputDataRate(ITDS_output_Data_Rate odr);
	int8_t ITDS_getOutputDataRate(uint8_t *odr);
	int8_t ITDS_setOperatingMode(ITDS_operating_mode operatingMode);
	int8_t ITDS_getOperatingMode(uint8_t *opMode);
	int8_t ITDS_setpowerMode(ITDS_power_mode lowOrNormalMode);
	int8_t ITDS_getpowerMode(uint8_t *powerMode);

	/* CTRL-REG 2 */
	int8_t ITDS_reboot(ITDS_state_t rebootmode);
	int8_t ITDS_getBootState(ITDS_state_t *boot);
	int8_t ITDS_softReset(ITDS_state_t Swreset_mode);
	int8_t ITDS_getsoftResetState(ITDS_state_t *swReset);
	int8_t ITDS_ConfigCSPullup(ITDS_state_t CSPullup);
	int8_t ITDS_getCSConfigState(ITDS_state_t *configCS);
	int8_t ITDS_setBlockDataUpdate(ITDS_state_t bdu);
	int8_t ITDS_getBlockDataUpdateState(ITDS_state_t *bdu);
	int8_t ITDS_setAutoIncrement(ITDS_state_t autoInc);
	int8_t ITDS_getAutoIncrementState(ITDS_state_t *autoIncr);
	int8_t ITDS_disableI2CInterface(ITDS_state_t I2CStatus);
	int8_t ITDS_getI2CInterfaceState(ITDS_state_t *i2cState);

	/* CTRL-REG 3 */
	int8_t ITDS_setSelfTestMode(ITDS_self_Test_Config selfTest);
	int8_t ITDS_getSelfTestMode(ITDS_state_t *selfTest);
	int8_t ITDS_setIntPinType(ITDS_interrupt_Pin_Config interruptType);
	int8_t ITDS_getIntPinType(ITDS_state_t *intType);
	int8_t ITDS_enLatchInt(ITDS_state_t lir);
	int8_t ITDS_getLatchIntState(ITDS_state_t *lir);
	int8_t ITDS_setIntActiveLevel(ITDS_interrupt_Active_Level lowHighLevel);
	int8_t ITDS_getIntActiveLevel(ITDS_interrupt_Active_Level *lowHighLevel);
	int8_t ITDS_setSingleConvInput(ITDS_singleDataConversion_Input slpModeSel);
	int8_t ITDS_getSingleConvInput(ITDS_singleDataConversion_Input *slpModeSel);
	int8_t ITDS_enSingleDataConv(ITDS_state_t slpMode1);
	int8_t ITDS_getSingleDataConvState(ITDS_state_t *slpMode1);

	/* CTRL-REG 4 */
	int8_t ITDS_set6DOnINT0(ITDS_state_t int06D);
	int8_t ITDS_get6DOnINT0State(ITDS_state_t *int06D);
	int8_t ITDS_enSingleTapINT0(ITDS_state_t int0SingleTap);
	int8_t ITDS_getSingleTapINT0(ITDS_state_t *int0SingleTap);
	int8_t ITDS_enWakeupOnINT0(ITDS_state_t int0Wakeup);
	int8_t ITDS_getWakeupOnINT0(ITDS_state_t *int0Wakeup);
	int8_t ITDS_enFreeFallINT0(ITDS_state_t int0FreeFall);
	int8_t ITDS_getFreeFallINT0(ITDS_state_t *int0FreeFall);
	int8_t ITDS_enDoubleTapINT0(ITDS_state_t int0DoubleTap);
	int8_t ITDS_getDoubleTapINT0(ITDS_state_t *int0DoubleTap);
	int8_t ITDS_enFifoFullINT0(ITDS_state_t int0FifoFull);
	int8_t ITDS_getFifoFullINT0(ITDS_state_t *int0FifoFull);
	int8_t ITDS_enFifoThresholdINT0(ITDS_state_t int0FifoThresholdInt);
	int8_t ITDS_getFifoThresholdINT0(ITDS_state_t *int0FifoThresholdInt);
	int8_t ITDS_enDataReadyINT0(ITDS_state_t int0DataReadyInt);
	int8_t ITDS_getDataReadyINT0(ITDS_state_t *int0DataReadyInt);

	/* CTRL-REG 5 */
	int8_t ITDS_enInactivityINT1(ITDS_state_t intSleepState);
	int8_t ITDS_getInactivityINT1(ITDS_state_t *intSleepState);
	int8_t ITDS_enActivityINT1(ITDS_state_t int1SleepChange);
	int8_t ITDS_getActivityINT1(ITDS_state_t *int1SleepChange);
	int8_t ITDS_enBootStatusINT1(ITDS_state_t int1Boot);
	int8_t ITDS_getBootStatusINT1(ITDS_state_t *int1Boot);
	int8_t ITDS_enTempDataReadyINT1(ITDS_state_t int1TempDataReady);
	int8_t ITDS_getTempDataReadyINT1(ITDS_state_t *int1TempDataReady);
	int8_t ITDS_enFifoOverrunIntINT1(ITDS_state_t int1FifoOverrun);
	int8_t ITDS_getFifoOverrunIntINT1(ITDS_state_t *int1FifoOverrun);
	int8_t ITDS_enFifoFullINT1(ITDS_state_t int1FifoFull);
	int8_t ITDS_getFifoFullINT1(ITDS_state_t *int1FifoFull);
	int8_t ITDS_enFifoThresholdIntINT1(ITDS_state_t int1FifoThresholdInt);
	int8_t ITDS_getFifoThresholdINT1(ITDS_state_t *int1FifoThresholdInt);
	int8_t ITDS_enDataReadyINT1(ITDS_state_t int1DataReadyInt);
	int8_t ITDS_getDataReadyINT1(ITDS_state_t *int1DataReadyInt);

	/* CTRL-REG 6 */
	int8_t ITDS_setFilteringCutoff(ITDS_bandwidth filteringCutoff);
	int8_t ITDS_getFilteringCutoff(ITDS_bandwidth *filteringCutoff);
	int8_t ITDS_setFullScale(ITDS_full_Scale fullScale_t);
	int8_t ITDS_getFullScale(ITDS_full_Scale *fullScale_t);
	int8_t ITDS_setfilterPath(ITDS_filter_Type filterType);
	int8_t ITDS_getfilterPath(ITDS_filter_Type *filterType);
	int8_t ITDS_enLowNoise(ITDS_state_t lowNoise);
	int8_t ITDS_getLowNoiseState(ITDS_state_t *lowNoise);

	/* Status */
	int8_t ITDS_getdataReadyState(ITDS_state_t *drdy);
	int8_t ITDS_getfreeFallState(ITDS_state_t *freeFall);
	int8_t ITDS_get6DState(ITDS_state_t *sixD);
	int8_t ITDS_getsingleTapState(ITDS_state_t *singleTap);
	int8_t ITDS_getdoubleTapState(ITDS_state_t *doubleTap);
	int8_t ITDS_getsleepState(ITDS_state_t *sleepState);
	int8_t ITDS_getwakeupState(ITDS_state_t *wakeupState);
	int8_t ITDS_getFifoThresholdStatus(ITDS_state_t *fifoTHR);

	/*acceleration Output */
	int8_t ITDS_getRawAccelerationX(int16_t *XRawAcc);
	int8_t ITDS_getRawAccelerationY(int16_t *YRawAcc);
	int8_t ITDS_getRawAccelerationZ(int16_t *ZRawAcc);

	/* Temperature output */
	int8_t ITDS_getTemperature8bit(uint8_t *temp8bit);
	int8_t ITDS_getRawTemp12bit(int16_t *temp12bit);
	int8_t ITDS_getTemperature12bit(float *tempdegC);

	/* FIFO CTRL */
	int8_t ITDS_setFifoMode(ITDS_Fifo_Mode FMode);
	int8_t ITDS_getFifoMode(ITDS_Fifo_Mode *FMode);
	int8_t ITDS_setFifoThreshold(uint8_t fifo_Threshold);
	int8_t ITDS_getFifoThreshold(uint8_t *fifo_Threshold);

	/* FIFO_SAMPLES */
	int8_t ITDS_getFifoThresholdState(ITDS_state_t *fifoTHR);
	int8_t ITDS_getFifoOverrunState(ITDS_state_t *fifoOverrun);
	int8_t ITDS_getFifoFillLevel(uint8_t *fifoFill);

	/* TAP_X_TH */
	int8_t ITDS_en4DDetection(ITDS_state_t detection4D);
	int8_t ITDS_get4DDetectionState(ITDS_state_t *detection4D);
	int8_t ITDS_setTapThresholdX(uint8_t tapThresholdX);
	int8_t ITDS_getTapThresholdX(uint8_t *tapThresholdX);
	int8_t ITDS_set6DThreshold(ITDS_threshold_Degree threshold6D);
	int8_t ITDS_get6DThreshold(ITDS_threshold_Degree *threshold6D);

	/* TAP_Y_TH */
	int8_t ITDS_setTapThresholdY(uint8_t tapThresholdY);
	int8_t ITDS_getTapThresholdY(uint8_t *tapThresholdY);
	int8_t ITDS_setTapAxisPriority(ITDS_tap_Axis_Priority tapPriorityAxis);
	int8_t ITDS_getTapAxisPriority(ITDS_tap_Axis_Priority *tapPriorityAxis);

	/* TAP_Z_TH */
	int8_t ITDS_setTapThresholdZ(uint8_t tapThresholdZ);
	int8_t ITDS_getTapThresholdZ(uint8_t *tapThresholdZ);
	int8_t ITDS_enTapX(ITDS_state_t tapX);
	int8_t ITDS_getTapX(ITDS_state_t *tapX);
	int8_t ITDS_enTapY(ITDS_state_t tapY);
	int8_t ITDS_getTapY(ITDS_state_t *tapY);
	int8_t ITDS_enTapZ(ITDS_state_t tapZ);
	int8_t ITDS_getTapZ(ITDS_state_t *tapZ);

	/* INT_DUR */
	int8_t ITDS_setLatency(uint8_t latency_t);
	int8_t ITDS_getLatency(uint8_t *latency_t);
	int8_t ITDS_setQuiet(uint8_t quiet_t);
	int8_t ITDS_getQuiet(uint8_t *quiet_t);
	int8_t ITDS_setShock(uint8_t shock_t);
	int8_t ITDS_getShock(uint8_t *shock_t);

	/* WAKE_UP_TH */
	int8_t ITDS_enTapEvent(ITDS_state_t singleDoubleTap);	/*Enable single/double-tap event. Default value: 0 (0: enable only single-tap, 1: enable both single and double-tap) */
	int8_t ITDS_getTapEvent(ITDS_state_t *singleDoubleTap); /* only Single Tap is enabled or Single and double Tap are both enabled */
	int8_t ITDS_enInactivity(ITDS_state_t sleep);
	int8_t ITDS_getInactivityState(ITDS_state_t *sleep);
	int8_t ITDS_setWakeupThreshold(uint8_t WakeupTHR);
	int8_t ITDS_getWakeupThreshold(uint8_t *WakeupTHR);

	/* WAKE_UP_DUR */
	int8_t ITDS_setFreeFallDurationMSB(ITDS_state_t FF_DUR5);
	int8_t ITDS_getFreeFallDurationMSB(ITDS_state_t *FF_DUR5);
	int8_t ITDS_setWakeupDuration(uint8_t wakeupDuration);
	int8_t ITDS_getWakeupDuration(uint8_t *wakeupDuration);
	int8_t ITDS_enStationnaryDetection(ITDS_state_t stationary);
	int8_t ITDS_getStationnaryState(ITDS_state_t *stationary);
	int8_t ITDS_setSleepDuration(uint8_t sleepDur);
	int8_t ITDS_getSleepDuration(uint8_t *sleepDur);

	/* FREE_FALL */
	int8_t ITDS_setFreeFallDurationLSB(uint8_t freeFallDuration);
	int8_t ITDS_getFreeFallDurationLSB(uint8_t *freeFallDuration);
	int8_t ITDS_setFreeFallThreshold(ITDS_Free_Fall_Thr FF_TH);
	int8_t ITDS_getFreeFallThreshold(ITDS_Free_Fall_Thr *FF_TH);
	int8_t ITDS_getFreeFallEventState(ITDS_state_t *freefall);

	/* STATUS_DETECT */
	int8_t ITDS_getTempDrdy(ITDS_state_t *tempDrdy);
	int8_t ITDS_getSleepState(ITDS_state_t *sleepState);
	int8_t ITDS_getDoubleTapState(ITDS_state_t *doubleTap);
	int8_t ITDS_getSingleTapState(ITDS_state_t *singleTap);
	int8_t ITDS_getDrdy(ITDS_state_t *drdy);

	/* WAKE_UP_EVENT */
	int8_t ITDS_getWakeupX(ITDS_state_t *wakeUpX);
	int8_t ITDS_getWakeupY(ITDS_state_t *wakeUpY);
	int8_t ITDS_getWakeupZ(ITDS_state_t *wakeUpZ);
	int8_t ITDS_getWakeupState(ITDS_state_t *wakeUpState);

	/* TAP_EVENT */
	int8_t ITDS_getTapEventState(ITDS_state_t *tapEvent);
	int8_t ITDS_getTapSign(ITDS_tapSign_t *tapSign);
	int8_t ITDS_getTapXAxis(ITDS_state_t *tapXAxis);
	int8_t ITDS_getTapYAxis(ITDS_state_t *tapYAxis);
	int8_t ITDS_getTapZAxis(ITDS_state_t *tapZAxis);

	/* 6D_EVENT*/
	int8_t ITDS_getZHOverThresholdState(ITDS_state_t *ZHOverThreshold);
	int8_t ITDS_getZLOverThresholdState(ITDS_state_t *ZLOverThreshold);
	int8_t ITDS_getYHOverThresholdState(ITDS_state_t *YHOverThreshold);
	int8_t ITDS_getYLOverThresholdState(ITDS_state_t *YLOverThreshold);
	int8_t ITDS_getXHOverThresholdState(ITDS_state_t *XHOverThreshold);
	int8_t ITDS_getXLOverThresholdState(ITDS_state_t *XLOverThreshold);

	/* ALL_INT_EVENT */
	int8_t ITDS_getSleepChangeState(ITDS_state_t *sleep);

	/* X_Y_Z_OFS_USR */
	int8_t ITDS_setOffsetValueOnXAxis(uint8_t offsetvalueXAxis);
	int8_t ITDS_getOffsetValueOnXAxis(uint8_t *offsetvalueXAxis);
	int8_t ITDS_setOffsetValueOnYAxis(uint8_t offsetvalueYAxis);
	int8_t ITDS_getOffsetValueOnYAxis(uint8_t *offsetvalueYAxis);
	int8_t ITDS_setOffsetValueOnZAxis(uint8_t offsetvalueZAxis);
	int8_t ITDS_getOffsetValueOnZAxis(uint8_t *offsetvalueZAxis);

	/* CTRL_7 */
	int8_t ITDS_setDrdyPulse(ITDS_drdy_pulse drdyPulsed);		  /* latched or pulsed DRDY */
	int8_t ITDS_getDrdyPulseIntMode(ITDS_drdy_pulse *drdyPulsed); /* latched or pulsed DRDY */
	int8_t ITDS_setInt1OnInt0(ITDS_state_t int1Onint0);
	int8_t ITDS_getInt1OnInt0(ITDS_state_t *int1Onint0);
	int8_t ITDS_enInterrups(ITDS_state_t interrupts);
	int8_t ITDS_getInterruptsState(ITDS_state_t *interrupts);
	int8_t ITDS_enApplyOffsetXL(ITDS_state_t offsetXLOutput);
	int8_t ITDS_getApplyOffsetXL(ITDS_state_t *offsetXLOutput);
	int8_t ITDS_enApplyWakeUpOffsetXL(ITDS_state_t offsetXLWakeup);
	int8_t ITDS_getApplyWakeUpOffsetXL(ITDS_state_t *offsetXLWakeup);

	int8_t ITDS_setOffsetWeight(ITDS_state_t offsetWeight);
	int8_t ITDS_getOffsetWeight(ITDS_state_t *offsetWeight);

	int8_t ITDS_enHighPassRefMode(ITDS_state_t refMode);
	int8_t ITDS_getHighPassRefMode(ITDS_state_t *refMode);

	int8_t ITDS_enLowPassOn6D(ITDS_state_t LPassOn6D);
	int8_t ITDS_getLowPassOn6DState(ITDS_state_t *LPassOn6D);

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_ITDS_H */
	   /**         EOF         */