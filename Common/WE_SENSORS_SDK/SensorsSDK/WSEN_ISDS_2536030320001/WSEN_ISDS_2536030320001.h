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
 * @brief Header file for the WSEN-ISDS-2536030320001 sensor driver.
 */

#ifndef _WSEN_ISDS_2536030320001_H
#define _WSEN_ISDS_2536030320001_H

/*         Includes         */

#include <stdint.h>

#include "../WeSensorsSDK.h"


/*         ISDS 2536030320001 DEVICE_ID         */

#define ISDS_DEVICE_ID_VALUE                  0x6A      /**< This is the expected answer when requesting the ISDS_DEVICE_ID_REG */


/*         Available ISDS I2C slave addresses         */

#define ISDS_ADDRESS_I2C_0                    0x6A      /**< When SAO of ISDS is connected to ground */
#define ISDS_ADDRESS_I2C_1                    0x6B      /**< When SAO of ISDS is connected to positive supply voltage */


/* Register address definitions */

#define ISDS_FIFO_CTRL_1_REG                  0x06      /**< FIFO configuration register 1 */
#define ISDS_FIFO_CTRL_2_REG                  0x07      /**< FIFO configuration register 2 */
#define ISDS_FIFO_CTRL_3_REG                  0x08      /**< FIFO configuration register 3 */
#define ISDS_FIFO_CTRL_4_REG                  0x09      /**< FIFO configuration register 4 */
#define ISDS_FIFO_CTRL_5_REG                  0x0A      /**< FIFO configuration register 5 */
#define ISDS_DRDY_PULSE_CFG_REG               0x0B      /**< Data ready configuration register */
#define ISDS_INT0_CTRL_REG                    0x0D      /**< INT0 pin control */
#define ISDS_INT1_CTRL_REG                    0x0E      /**< INT1 pin control */
#define ISDS_DEVICE_ID_REG                    0x0F      /**< Device ID register */
#define ISDS_CTRL_1_REG                       0x10      /**< Control register 1 (linear acceleration sensor) */
#define ISDS_CTRL_2_REG                       0x11      /**< Control register 2 (angular rate sensor) */
#define ISDS_CTRL_3_REG                       0x12      /**< Control register 3 */
#define ISDS_CTRL_4_REG                       0x13      /**< Control register 4 */
#define ISDS_CTRL_5_REG                       0x14      /**< Control register 5 */
#define ISDS_CTRL_6_REG                       0x15      /**< Control register 6 (angular rate sensor) */
#define ISDS_CTRL_7_REG                       0x16      /**< Control register 7 (angular rate sensor) */
#define ISDS_CTRL_8_REG                       0x17      /**< Control register 8 (linear acceleration sensor) */
#define ISDS_CTRL_9_REG                       0x18      /**< Control register 9 (linear acceleration sensor) */
#define ISDS_CTRL_10_REG                      0x19      /**< Control register 10 */
#define ISDS_WAKE_UP_EVENT_REG                0x1B      /**< Wake-up interrupt source register */
#define ISDS_TAP_EVENT_REG                    0x1C      /**< Tap source register */
#define ISDS_6D_EVENT_REG                     0x1D      /**< 6D orientation source register */
#define ISDS_STATUS_REG                       0x1E      /**< Status data register */
#define ISDS_OUT_TEMP_L_REG                   0x20      /**< Temperature output value LSB */
#define ISDS_OUT_TEMP_H_REG                   0x21      /**< Temperature output value MSB */
#define ISDS_X_OUT_L_GYRO_REG                 0x22      /**< Angular rate output value X (pitch) LSB */
#define ISDS_X_OUT_H_GYRO_REG                 0x23      /**< Angular rate output value X (pitch) MSB */
#define ISDS_Y_OUT_L_GYRO_REG                 0x24      /**< Angular rate output value Y (roll) LSB */
#define ISDS_Y_OUT_H_GYRO_REG                 0x25      /**< Angular rate output value Y (roll) MSB */
#define ISDS_Z_OUT_L_GYRO_REG                 0x26      /**< Angular rate output value Z (yaw) LSB */
#define ISDS_Z_OUT_H_GYRO_REG                 0x27      /**< Angular rate output value Z (yaw) MSB */
#define ISDS_X_OUT_L_ACC_REG                  0x28      /**< Linear acceleration output value X LSB */
#define ISDS_X_OUT_H_ACC_REG                  0x29      /**< Linear acceleration output value X MSB */
#define ISDS_Y_OUT_L_ACC_REG                  0x2A      /**< Linear acceleration output value Y LSB */
#define ISDS_Y_OUT_H_ACC_REG                  0x2B      /**< Linear acceleration output value Y MSB */
#define ISDS_Z_OUT_L_ACC_REG                  0x2C      /**< Linear acceleration output value Z LSB */
#define ISDS_Z_OUT_H_ACC_REG                  0x2D      /**< Linear acceleration output value Z MSB */
#define ISDS_FIFO_STATUS_1_REG                0x3A      /**< FIFO status register 1 */
#define ISDS_FIFO_STATUS_2_REG                0x3B      /**< FIFO status register 2 */
#define ISDS_FIFO_STATUS_3_REG                0x3C      /**< FIFO status register 3 */
#define ISDS_FIFO_STATUS_4_REG                0x3D      /**< FIFO status register 4 */
#define ISDS_FIFO_DATA_OUT_L_REG              0x3E      /**< FIFO data output register LSB */
#define ISDS_FIFO_DATA_OUT_H_REG              0x3F      /**< FIFO data output register MSB */
#define ISDS_FUNC_SRC_1_REG                   0x53      /**< Tilt interrupt source register */
#define ISDS_TAP_CFG_REG                      0x58      /**< Enables interrupt and inactivity functions, configuration of filtering and tap recognition functions */
#define ISDS_TAP_THS_6D_REG                   0x59      /**< Portrait/landscape position and tap function threshold register */
#define ISDS_INT_DUR2_REG                     0x5A      /**< Tap recognition function setting register */
#define ISDS_WAKE_UP_THS_REG                  0x5B      /**< Single and double-tap function threshold register */
#define ISDS_WAKE_UP_DUR_REG                  0x5C      /**< Free-fall, wake-up and sleep mode functions duration setting register */
#define ISDS_FREE_FALL_REG                    0x5D      /**< Free-fall function duration setting register */
#define ISDS_MD1_CFG_REG                      0x5E      /**< Functions routing on INT0 register */
#define ISDS_MD2_CFG_REG                      0x5F      /**< Functions routing on INT1 register */
#define ISDS_X_OFS_USR_REG                    0x73      /**< Accelerometer X-axis user offset correction */
#define ISDS_Y_OFS_USR_REG                    0x74      /**< Accelerometer Y-axis user offset correction */
#define ISDS_Z_OFS_USR_REG                    0x75      /**< Accelerometer Z-axis user offset correction */


/* Register type definitions */

/**
 * @brief ISDS_FIFO_CTRL_1_REG
 *
 * Address 0x06
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoThresholdLsb : 8;         /**< FIFO threshold level setting LSB. Default value: 0. Watermark flag rises when the number of bytes written to FIFO after the next write is greater than or equal to the threshold level. Minimum resolution for the FIFO is 1 LSB = 2 bytes (1 word) in FIFO. */
} ISDS_fifoCtrl1_t;

/**
 * @brief ISDS_FIFO_CTRL_2_REG
 *
 * Address 0x07
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoThresholdMsb : 3;         /**< FIFO threshold level setting MSB. Default value: 0. Watermark flag rises when the number of bytes written to FIFO after the next write is greater than or equal to the threshold level. Minimum resolution for the FIFO is 1LSB = 2 bytes (1 word) in FIFO. */
  uint8_t enFifoTemperature : 1;        /**< Enables the temperature data storage in FIFO. Default: 0. 0: temperature not included in FIFO; 1: temperature included in FIFO. */
  uint8_t notUsed01 : 4;                /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_fifoCtrl2_t;

/**
 * @brief ISDS_FIFO_CTRL_3_REG
 *
 * Address 0x08
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoAccDecimation : 3;        /**< Accelerometer FIFO (second data set) decimation setting. Default: 0. See ISDS_fifoDecimation_t. */
  uint8_t fifoGyroDecimation : 3;       /**< Gyro FIFO (first data set) decimation setting. Default: 0. See ISDS_fifoDecimation_t. */
  uint8_t notUsed01 : 2;                /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_fifoCtrl3_t;

/**
 * @brief ISDS_FIFO_CTRL_4_REG
 *
 * Address 0x09
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoThirdDecimation : 3;      /**< Third FIFO data set decimation setting. Default: 0. See ISDS_fifoDecimation_t. */
  uint8_t fifoFourthDecimation : 3;     /**< Fourth FIFO data set decimation setting. Default: 0. See ISDS_fifoDecimation_t. */
  uint8_t enOnlyHighData : 1;           /**< 8-bit data storage in FIFO. Default: 0. 0: disable MSB only memorization in FIFO for XL and Gyro; 1: enable MSB only memorization in FIFO for XL and Gyro in FIFO. */
  uint8_t enStopOnThreshold : 1;        /**< Enable FIFO threshold level use. Default value: 0. 0: FIFO depth is not limited; 1: FIFO depth is limited to threshold level. */
} ISDS_fifoCtrl4_t;

/**
 * @brief ISDS_FIFO_CTRL_5_REG
 *
 * Address 0x0A
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoMode : 3;                 /**< FIFO mode. See ISDS_fifoMode_t. */
  uint8_t fifoOdr : 4;                  /**< FIFO output data rate. See ISDS_fifoOutputDataRate_t. */
  uint8_t notUsed01 : 1;                /**< This bit must be set to 0 for proper operation of the device. */
} ISDS_fifoCtrl5_t;

/**
 * @brief ISDS_DRDY_PULSE_CFG_REG
 *
 * Address 0x0B
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 7;                /**< These bits must be set to 0 for proper operation of the device. */
  uint8_t enDataReadyPulsed : 1;        /**< Enable pulsed data-ready mode. Default value: 0. 0: data-ready latched mode. Returns to 0 only after output data has been read; 1: data-ready pulsed mode. The data-ready pulses are 75 μs long. */
} ISDS_dataReadyPulseCfg_t;

/**
 * @brief ISDS_INT0_CTRL_REG
 *
 * Address 0x0D
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t int0AccDataReady : 1;         /**< Accelerometer data-ready on INT0 pin. Default value: 0. */
  uint8_t int0GyroDataReady : 1;        /**< Gyroscope data-ready on INT0 pin. Default value: 0. */
  uint8_t int0Boot : 1;                 /**< Boot status on INT0 pin. Default value: 0. */
  uint8_t int0FifoThreshold : 1;        /**< FIFO threshold interrupt on INT0 pin. Default value: 0. */
  uint8_t int0FifoOverrun : 1;          /**< FIFO overrun interrupt on INT0 pin. Default value: 0. */
  uint8_t int0FifoFull : 1;             /**< FIFO full interrupt on INT0 pin. Default value: 0. */
  uint8_t notUsed01 : 2;                /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_int0Ctrl_t;

/**
 * @brief ISDS_INT1_CTRL_REG
 *
 * Address 0x0E
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t int1AccDataReady : 1;         /**< Accelerometer data-ready on INT1 pin. Default value: 0. */
  uint8_t int1GyroDataReady : 1;        /**< Gyroscope data-ready on INT1 pin. Default value: 0. */
  uint8_t int1TempDataReady : 1;        /**< Temperature data-ready on INT1 pin. Default value: 0. */
  uint8_t int1FifoThreshold : 1;        /**< FIFO threshold interrupt on INT1 pin. Default value: 0. */
  uint8_t int1FifoOverrun : 1;          /**< FIFO overrun interrupt on INT1 pin. Default value: 0. */
  uint8_t int1FifoFull : 1;             /**< FIFO full interrupt on INT1 pin. Default value: 0. */
  uint8_t notUsed01 : 2;                /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_int1Ctrl_t;

/**
 * @brief ISDS_CTRL_1_REG
 *
 * Address 0x10
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t accAnalogBandwidth : 1;       /**< Accelerometer analog chain bandwidth selection (only for accelerometer ODR ≥ 1.67 kHz). See ISDS_accAnalogChainBandwidth_t. */
  uint8_t accDigitalBandwidth : 1;      /**< Accelerometer digital LPF (LPF1) bandwidth selection. See ISDS_accDigitalLpfBandwidth_t. */
  uint8_t accFullScale : 2;             /**< Accelerometer full-scale selection. Default value: 0. See ISDS_accFullScale_t. */
  uint8_t accOutputDataRate : 4;        /**< Output data rate and power mode selection. Default value: 0. See ISDS_accOutputDataRate_t. */
} ISDS_ctrl1_t;

/**
 * @brief ISDS_CTRL_2_REG
 *
 * Address 0x11
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 1;                /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t gyroFullScale : 3;            /**< Gyroscope full-scale selection. Default value: 0. See ISDS_gyroFullScale_t. */
  uint8_t gyroOutputDataRate : 4;       /**< Output data rate and power mode selection. Default value: 0. See ISDS_gyroOutputDataRate_t. */
} ISDS_ctrl2_t;

/**
 * @brief ISDS_CTRL_3_REG
 *
 * Address 0x12
 * Type  R/W
 * Default value: 0x04
 */
typedef struct
{
  uint8_t softReset : 1;                /**< Software reset. 0: normal mode; 1: SW reset; Self-clearing upon completion. */
  uint8_t notUsed01 : 1;                /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t autoAddIncr : 1;              /**< Register address automatically incremented during a multiple byte access with I2C/SPI interface. Default: 1. 0: disable; 1: enable. */
  uint8_t spiMode : 1;                  /**< SPI serial interface mode. 0: 4-wire interface; 1: 3-wire interface. See ISDS_spiMode_t.*/
  uint8_t intPinConf : 1;               /**< Push-pull/open-drain selection on INT0 and INT1 pads. Default value: 0. 0: push-pull mode; 1: open-drain mode. See ISDS_interruptPinConfig_t. */
  uint8_t intActiveLevel : 1;           /**< Interrupt activation level. Default value: 0. 0: interrupt output pads active high; 1: interrupt output pads active low. See ISDS_interruptActiveLevel_t. */
  uint8_t blockDataUpdate : 1;          /**< Block data update. 0: continuous update; 1: output registers are not updated until MSB and LSB have been read. */
  uint8_t boot : 1;                     /**< Set this bit to 1 to initiate boot sequence. 0: normal mode; 1: Execute boot sequence. Self-clearing upon completion. */
} ISDS_ctrl3_t;

/**
 * @brief ISDS_CTRL_4_REG
 *
 * Address 0x13
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 1;                  /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t enGyroLPF1 : 1;                 /**< Enable gyroscope digital LPF1. 0: disabled; 1: enabled. */
  uint8_t i2cDisable : 1;                 /**< Disable I2C interface. Default value: 0. 0: both I2C and SPI enabled; 1: I2C disabled, SPI only. */
  uint8_t dataReadyMask : 1;              /**< Enables masking of the accelerometer and gyroscope data-ready signals until the settling of the sensor filters is completed. Default value: 0. 0: Masking disabled; 1: Masking enabled. */
  uint8_t dataEnableDataReadyOnInt0 : 1;  /**< Data enable (DEN) data ready signal on INT0 pad. Default value: 0. 0: disabled; 1: enabled. */
  uint8_t int1OnInt0 : 1;                 /**< All interrupt signals available on INT0 pad enable. Default value: 0. 0: interrupt signals divided between INT0 and INT1 pads; 1: all interrupt signals in logic or on INT0 pad. */
  uint8_t enGyroSleepMode : 1;            /**< Gyroscope sleep mode enable. Default value: 0. 0: disabled; 1: enabled. */
  uint8_t dataEnableExtendToAcc : 1;      /**< Extend data enable (DEN) functionality to accelerometer sensor. Default value: 0. 0: disabled; 1: enabled. */
} ISDS_ctrl4_t;

/**
 * @brief ISDS_CTRL_5_REG
 *
 * Address 0x14
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t accSelfTest : 2;              /**< Accelerometer self-test enable. Default value: 0. See ISDS_accSelfTestMode_t. */
  uint8_t gyroSelfTest : 2;             /**< Gyroscope self-test enable. Default value: 0. See ISDS_gyroSelfTestMode_t. */
  uint8_t dataEnableActiveLevel : 1;    /**< Data enable (DEN) active level configuration. Default value: 0. 0: active low; 1: active high. */
  uint8_t rounding : 3;                 /**< Circular burst-mode (rounding) read from output registers through the primary interface. Default value: 0. See ISDS_roundingPattern_t. */
} ISDS_ctrl5_t;

/**
 * @brief ISDS_CTRL_6_REG
 *
 * Address 0x15
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t gyroLowPassFilterType : 2;            /**< Gyroscope low-pass filter (LPF1) bandwidth selection. See ISDS_gyroLPF_t. */
  uint8_t notUsed01 : 1;                        /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t userOffsetsWeight : 1;                /**< Weight of accelerometer user offset bits in registers ISDS_X_OFS_USR_REG, ISDS_Y_OFS_USR_REG, ISDS_Z_OFS_USR_REG. 0: 2^-10 g/LSB; 1: 2^-6 g/LSB. */
  uint8_t accHighPerformanceModeDisable : 1;    /**< High-performance operating mode disable for accelerometer. Default value: 0. 0: high-performance operating mode enabled; 1: high-performance operating mode disabled. */
  uint8_t dataEnableTriggerMode : 3;            /**< Data enable (DEN) trigger mode. See ISDS_dataEnableTriggerMode_t. */
} ISDS_ctrl6_t;

/**
 * @brief ISDS_CTRL_7_REG
 *
 * Address 0x16
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 2;                        /**< These bits must be set to 0 for proper operation of the device. */
  uint8_t enRounding : 1;                       /**< Source register rounding function on ISDS_WAKE_UP_EVENT_REG, ISDS_TAP_EVENT_REG, ISDS_6D_EVENT_REG, ISDS_STATUS_REG and ISDS_FUNC_SRC_1_REG registers in the primary interface. Default value: 0. 0: rounding disabled; 1: rounding enabled. */
  uint8_t notUsed02 : 1;                        /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t gyroDigitalHighPassCutoff : 2;        /**< Gyroscope digital HP filter cutoff selection. Default: 0. See ISDS_gyroDigitalHighPassCutoff_t. */
  uint8_t gyroDigitalHighPassEnable : 1;        /**< Gyroscope digital high-pass filter enable. The filter is enabled only if the gyro is in HP mode. Default value: 0. 0: HPF disabled; 1: HPF enabled. */
  uint8_t gyroHighPerformanceModeDisable : 1;   /**< High-performance operating mode disable for gyroscope. Default: 0. 0: high-performance operating mode enabled; 1: high-performance operating mode disabled. */
} ISDS_ctrl7_t;

/**
 * @brief ISDS_CTRL_8_REG
 *
 * Address 0x17
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t en6dLowPass: 1;                 /**< LPF2 on 6D function selection. */
  uint8_t notUsed01 : 1;                  /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t enAccHighPassSlopeFilter : 1;   /**< Accelerometer slope filter / high-pass filter selection. 0: the low-pass path of the composite filter block is selected; 1: the high-pass path of the composite filter block is selected. */
  uint8_t inputComposite : 1;             /**< Composite filter input selection. See ISDS_inputCompositeFilter_t. Default: 0. 0: ODR/2 low pass filtered sent to composite filter (default); 1: ODR/4 low pass filtered sent to composite filter. */
  uint8_t highPassFilterRefMode : 1;      /**< Enable HP filter reference mode (when enabled, the first output data has to be discarded). Default value: 0. 0: disabled; 1: enabled. */
  uint8_t accFilterConfig : 2;            /**< Accelerometer LPF2 and high-pass filter configuration and cutoff setting. See ISDS_accFilterConfig_t. */
  uint8_t enAccLowPass : 1;               /**< Accelerometer low-pass filter LPF2 selection. */
} ISDS_ctrl8_t;

/**
 * @brief ISDS_CTRL_9_REG
 *
 * Address 0x18
 * Type  R/W
 * Default value: 0xE0
 */
typedef struct
{
  uint8_t notUsed01 : 4;                  /**< These bits must be set to 0 for proper operation of the device. */
  uint8_t dataEnableStampingSensor : 1;   /**< Data enable (DEN) stamping sensor selection. See ISDS_dataEnableStampingSensor_t. Default value: 0. 0: DEN pin info stamped in the gyroscope axis selected by bits [7:5]; 1: DEN pin info stamped in the accelerometer axis selected by bits [7:5]. */
  uint8_t dataEnableValueZ : 1;           /**< Data enable (DEN) value stored in LSB of Z-axis. Default value: 1. 0: DEN not stored in Z-axis LSB; 1: DEN stored in Z-axis LSB. */
  uint8_t dataEnableValueY : 1;           /**< Data enable (DEN) value stored in LSB of Y-axis. Default value: 1. 0: DEN not stored in Y-axis LSB; 1: DEN stored in Y-axis LSB. */
  uint8_t dataEnableValueX : 1;           /**< Data enable (DEN) value stored in LSB of X-axis. Default value: 1. 0: DEN not stored in X-axis LSB; 1: DEN stored in X-axis LSB. */
} ISDS_ctrl9_t;

/**
 * @brief ISDS_CTRL_10_REG
 *
 * Address 0x19
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 2;                  /**< These bits must be set to 0 for proper operation of the device. */
  uint8_t enEmbeddedFunc : 1;             /**< Enable embedded functionalities (tilt). Default value: 0. 0: disable functionalities of embedded functions and accelerometer filters; 1: enable functionalities of embedded functions and accelerometer filters. */
  uint8_t enTiltCalculation : 1;          /**< Enable tilt calculation. */
  uint8_t notUsed02 : 1;                  /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t notUsed03 : 3;                  /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_ctrl10_t;

/**
 * @brief ISDS_WAKE_UP_EVENT_REG
 *
 * Address 0x1B
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t wakeUpZ : 1;                    /**< Wake-up event on Z-axis status. 0: Wake-up event on Z-axis not detected; 1: Wake-up event on Z-axis detected. */
  uint8_t wakeUpY : 1;                    /**< Wake-up event on Y-axis status. 0: Wake-up event on Y-axis not detected; 1: Wake-up event on Y-axis detected. */
  uint8_t wakeUpX : 1;                    /**< Wake-up event on X-axis status. 0: Wake-up event on X-axis not detected; 1: Wake-up event on X-axis detected. */
  uint8_t wakeUpState : 1;                /**< Wake-up event detection status. 0: Wake-up event not detected; 1: Wake-up event detected. */
  uint8_t sleepState : 1;                 /**< Sleep event status. 0: Sleep event not detected; 1: Sleep event detected. */
  uint8_t freeFallState : 1;              /**< Free-fall event detection status. 0: FF event not detected; 1: FF event detected. */
  uint8_t notUsed01 : 2;                  /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_wakeUpEvent_t;

/**
 * @brief ISDS_TAP_EVENT_REG
 *
 * Address 0x1C
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t tapZAxis : 1;                   /**< Tap event detection on Z-axis status. 0: Tap event on Z-axis not detected; 1: Tap event on Z-axis detected. */
  uint8_t tapYAxis : 1;                   /**< Tap event detection on Y-axis status. 0: Tap event on Y-axis not detected; 1: Tap event on Y-axis detected. */
  uint8_t tapXAxis : 1;                   /**< Tap event detection on X-axis status. 0: Tap event on X-axis not detected; 1: Tap event on X-axis detected. */
  uint8_t tapSign : 1;                    /**< Sign of acceleration detected by tap event. 0: Tap in positive direction; 1: Tap in negative direction. */
  uint8_t doubleState : 1;                /**< Double-tap event status. 0: Double-tap event not detected; 1: Double-tap event detected. */
  uint8_t singleState : 1;                /**< Single-tap event status. 0: Single-tap event not detected; 1: Single-tap event detected. */
  uint8_t tapEventState : 1;              /**< Tap event status. 0: Tap event not detected; 1: Tap event detected. */
  uint8_t notUsed01 : 1;                  /**< This bit must be set to 0 for proper operation of the device. */
} ISDS_tapEvent_t;

/**
 * @brief ISDS_6D_EVENT_REG
 *
 * Address 0x1D
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t xlOverThreshold : 1;            /**< 1: XL threshold exceeded; 0: XL threshold not exceeded */
  uint8_t xhOverThreshold : 1;            /**< 1: XH threshold exceeded; 0: XH threshold not exceeded */
  uint8_t ylOverThreshold : 1;            /**< 1: YL threshold exceeded; 0: YL threshold not exceeded */
  uint8_t yhOverThreshold : 1;            /**< 1: YH threshold exceeded; 0: YH threshold not exceeded */
  uint8_t zlOverThreshold : 1;            /**< 1: ZL threshold exceeded; 0: ZL threshold not exceeded */
  uint8_t zhOverThreshold : 1;            /**< 1: ZH threshold exceeded; 0: ZH threshold not exceeded */
  uint8_t sixDChange : 1;                 /**< Orientation change detection status (0: No event detected; 1: A change in orientation has been detected) */
  uint8_t dataEnableDataReady : 1;        /**< Data enable (DEN) data-ready signal. It is set high when data output is related to the data coming from a DEN active condition. */
} ISDS_6dEvent_t;

/**
 * @brief ISDS_STATUS_REG
 *
 * Address 0x1E
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t accDataReady : 1;               /**< 1: New acceleration data available; 0: No new data available. */
  uint8_t gyroDataReady : 1;              /**< 1: New gyroscope data available; 0: No new data available. */
  uint8_t tempDataReady: 1;               /**< 1: New temperature data available; 0: No new data available. */
  uint8_t notUsed01 : 5;                  /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_status_t;

/**
 * @brief ISDS_FIFO_STATUS_1_REG
 *
 * Address 0x3A
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoFillLevelLsb : 8;           /**< Current fill level of FIFO (0-2047) i.e. the number of unread samples (16-bit values). */
} ISDS_fifoStatus1_t;

/**
 * @brief ISDS_FIFO_STATUS_2_REG
 *
 * Address 0x3B
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoFillLevelMsb : 3;           /**< Current fill level of FIFO (0-2047) i.e. the number of unread samples (16-bit values). */
  uint8_t notUsed01 : 1;                  /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t fifoEmptyState : 1;             /**< FIFO empty bit. Default value: 0. 0: FIFO contains data; 1: FIFO is empty. */
  uint8_t fifoFullSmartState : 1;         /**< Smart FIFO full status. Default value: 0. 0: FIFO is not full; 1: FIFO will be full at the next ODR. */
  uint8_t fifoOverrunState : 1;           /**< FIFO overrun status. Default value: 0. 0: FIFO is not completely filled; 1: FIFO is completely filled. */
  uint8_t fifoThresholdState : 1;         /**< FIFO threshold status bit. 0: FIFO filling is lower than threshold level, 1: FIFO filling is equal to or higher than the threshold level. */
} ISDS_fifoStatus2_t;

/**
 * @brief ISDS_FIFO_STATUS_3_REG
 *
 * Address 0x3C
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoPatternLsb : 8;       /**< Word of recursive pattern read at the next read. */
} ISDS_fifoStatus3_t;

/**
 * @brief ISDS_FIFO_STATUS_4_REG
 *
 * Address 0x3D
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t fifoPatternMsb : 2;       /**< Word of recursive pattern read at the next read. */
  uint8_t notUsed01 : 6;            /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_fifoStatus4_t;

/**
 * @brief ISDS_FUNC_SRC_1_REG
 *
 * Address 0x53
 * Type  R
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 5;            /**< These bits must be set to 0 for proper operation of the device. */
  uint8_t tiltState : 1;            /**< Tilt event detection status. Default value: 0. 0: tilt event not detected; 1: tilt event detected. */
  uint8_t notUsed02 : 2;            /**< These bits must be set to 0 for proper operation of the device. */
} ISDS_funcSrc1_t;

/**
 * @brief ISDS_TAP_CFG_REG
 *
 * Address 0x58
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t latchedInterrupt : 1;     /**< Latched Interrupt. Default value: 0. 0: interrupt request not latched; 1: interrupt request latched. */
  uint8_t enTapZ : 1;               /**< Enable Z direction in tap recognition. Default value: 0. 0: Z direction disabled; 1: Z direction enabled. */
  uint8_t enTapY : 1;               /**< Enable Y direction in tap recognition. Default value: 0. 0: Y direction disabled; 1: Y direction enabled. */
  uint8_t enTapX : 1;               /**< Enable X direction in tap recognition. Default value: 0. 0: X direction disabled; 1: X direction enabled. */
  uint8_t filterSelection : 1;      /**< HPF or SLOPE filter selection on wake-up and activity/inactivity functions. See ISDS_activityFilter_t. Default value: 0. 0: SLOPE filter applied; 1: HPF applied. */
  uint8_t enInactivity : 2;         /**< Enable inactivity function. Default value: 0. See ISDS_inactivityFunction_t. */
  uint8_t enInterrupts : 1;         /**< Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity). Default value: 0. 0: interrupt disabled; 1: interrupt enabled. */
} ISDS_tapCfg_t;

/**
 * @brief ISDS_TAP_THS_6D_REG
 *
 * Address 0x59
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t tapThreshold : 5;           /**< Threshold for tap recognition. Default value: 0. 1 LSB corresponds to FS/32. */
  uint8_t sixDThreshold : 2;          /**< Threshold for 4D/6D function. Default value: 0. See ISDS_sixDThreshold_t. */
  uint8_t fourDDetectionEnabled : 1;  /**< Enable 4D portrait/landscape detection. 0: 4D mode disabled; 1: portrait/landscape detection and face-up/face-down detection enabled. */
} ISDS_tapThs6d_t;

/**
 * @brief ISDS_INT_DUR2_REG
 *
 * Address 0x5A
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t shock : 2;                /**< Defines the maximum duration of over-threshold event when detecting taps. */
  uint8_t quiet : 2;                /**< Defines the expected quiet time after a tap detection. */
  uint8_t latency : 4;              /**< Defines the maximum duration time gap for double-tap recognition. */
} ISDS_intDur2_t;

/**
 * @brief ISDS_WAKE_UP_THS_REG
 *
 * Address 0x5B
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t wakeUpThreshold : 6;      /**< Defines wake-up threshold, 6-bit unsigned 1 LSB = 1/64 of FS. Default value: 0. */
  uint8_t notUsed01 : 1;            /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t enDoubleTapEvent : 1;     /**< Enable double-tap event. Default value: 0. 0: enable only single-tap; 1: enable both (single and double-tap). */
} ISDS_wakeUpThs_t;

/**
 * @brief ISDS_WAKE_UP_DUR_REG
 *
 * Address 0x5C
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t sleepDuration : 4;        /**< Defines the sleep mode duration. Default value is 0 (which is 16 * 1/ODR). 1 LSB = 512 * 1/ODR */
  uint8_t notUsed01 : 1;            /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t wakeUpDuration : 2;       /**< This parameter defines the wake-up duration. 1 LSB = 1 * 1/ODR. */
  uint8_t freeFallDurationMSB : 1;  /**< This bit defines the free-fall duration. Combined with FF_DUR [4:0] bit in ISDS_FREE_FALL_REG register. 1 LSB = 1 * 1/ODR. */
} ISDS_wakeUpDur_t;

/**
 * @brief ISDS_FREE_FALL_REG
 *
 * Address 0x5D
 * Type  R/W
 * Default value: 0x00
 *
 *  freeFallThreshold  |  Decoded threshold
 * ----------------------------------------
 *         000         |        5
 *         001         |        7
 *         010         |        8
 *         011         |        10
 *         100         |        11
 *         101         |        13
 *         110         |        15
 *         111         |        16
 */
typedef struct
{
  uint8_t freeFallThreshold : 3;    /**< Encoded free-fall threshold value. See ISDS_freeFallThreshold_t. The decoded value can be multiplied with 31.25mg to get the used threshold. */
  uint8_t freeFallDurationLSB: 5;   /**< Defines free-fall duration. Is combined with FF_DUR5 bit in ISDS_WAKE_UP_DUR_REG register. 1 LSB = 1 * 1/ODR. */
} ISDS_freeFall_t;

/**
 * @brief ISDS_MD1_CFG_REG
 *
 * Address 0x5E
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 1;            /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t int0Tilt : 1;             /**< Routing of tilt event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int06d : 1;               /**< Routing of 6D event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int0DoubleTap : 1;        /**< Routing of double tap event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int0FreeFall : 1;         /**< Routing of free fall event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int0WakeUp : 1;           /**< Routing of wake-up event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int0SingleTap : 1;        /**< Routing of single tap event on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int0InactivityState : 1;  /**< Routing of inactivity on INT0. Default value: 0. 0: routing disabled; 1: routing enabled. */
} ISDS_mde1Cfg_t;

/**
 * @brief ISDS_MD2_CFG_REG
 *
 * Address 0x5F
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t notUsed01 : 1;            /**< This bit must be set to 0 for proper operation of the device. */
  uint8_t int1Tilt : 1;             /**< Routing of tilt event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int16d : 1;               /**< Routing of 6D event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int1DoubleTap : 1;        /**< Routing of double tap event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int1FreeFall : 1;         /**< Routing of free fall event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int1WakeUp : 1;           /**< Routing of wake-up event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int1SingleTap : 1;        /**< Routing of single tap event on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
  uint8_t int1InactivityState : 1;  /**< Routing of inactivity on INT1. Default value: 0. 0: routing disabled; 1: routing enabled. */
} ISDS_mde2Cfg_t;


/*         Functional type definitions         */

typedef enum
{
  ISDS_disable = 0,
  ISDS_enable = 1
} ISDS_state_t;

typedef enum
{
  ISDS_positive = 0,
  ISDS_negative = 1
} ISDS_tapSign_t;

typedef enum
{
  ISDS_fifoDecimationNoFifo   = 0,
  ISDS_fifoDecimationDisabled = 1,
  ISDS_fifoDecimationFactor2  = 2,
  ISDS_fifoDecimationFactor3  = 3,
  ISDS_fifoDecimationFactor4  = 4,
  ISDS_fifoDecimationFactor8  = 5,
  ISDS_fifoDecimationFactor16 = 6,
  ISDS_fifoDecimationFactor32 = 7
} ISDS_fifoDecimation_t;

typedef enum
{
  ISDS_bypassMode = 0,
  ISDS_fifoEnabled = 1,
  ISDS_continuousToFifo = 3,
  ISDS_bypassToContinuous = 4,
  ISDS_continuousMode = 6
} ISDS_fifoMode_t;

typedef enum
{
  ISDS_fifoOdrOff    =  0,
  ISDS_fifoOdr12Hz5  =  1,
  ISDS_fifoOdr26Hz   =  2,
  ISDS_fifoOdr52Hz   =  3,
  ISDS_fifoOdr104Hz  =  4,
  ISDS_fifoOdr208Hz  =  5,
  ISDS_fifoOdr416Hz  =  6,
  ISDS_fifoOdr833Hz  =  7,
  ISDS_fifoOdr1k66Hz =  8,
  ISDS_fifoOdr3k33Hz =  9,
  ISDS_fifoOdr6k66Hz = 10
} ISDS_fifoOutputDataRate_t;

typedef enum
{
  ISDS_accAnalogChainBandwidth1k5Hz = 0,    /**< 1.5 kHz */
  ISDS_accAnalogChainBandwidth400Hz = 1     /**< 400 Hz */
} ISDS_accAnalogChainBandwidth_t;

typedef enum
{
  ISDS_accDigitalLpfBandwidthOdrDiv2 = 0,   /**< ODR / 2 */
  ISDS_accDigitalLpfBandwidthOdrDiv4 = 1    /**< ODR / 4 */
} ISDS_accDigitalLpfBandwidth_t;

typedef enum
{
  ISDS_inputCompositeFilterOdrDiv2 = 0,     /**< ODR / 2 */
  ISDS_inputCompositeFilterOdrDiv4 = 1      /**< ODR / 4 */
} ISDS_inputCompositeFilter_t;

typedef enum
{
  ISDS_accFullScaleTwoG       = 0,
  ISDS_accFullScaleSixteenG   = 1,
  ISDS_accFullScaleFourG      = 2,
  ISDS_accFullScaleEightG     = 3,
  ISDS_accFullScaleInvalid    = 4
} ISDS_accFullScale_t;

typedef enum
{
  ISDS_accOdrOff      =  0,
  ISDS_accOdr12Hz5    =  1,
  ISDS_accOdr26Hz     =  2,
  ISDS_accOdr52Hz     =  3,
  ISDS_accOdr104Hz    =  4,
  ISDS_accOdr208Hz    =  5,
  ISDS_accOdr416Hz    =  6,
  ISDS_accOdr833Hz    =  7,
  ISDS_accOdr1k66Hz   =  8,
  ISDS_accOdr3k33Hz   =  9,
  ISDS_accOdr6k66Hz   = 10,
  ISDS_accOdr1Hz6     = 11
} ISDS_accOutputDataRate_t;

typedef enum
{
  ISDS_gyroFullScale250dps   = 0,
  ISDS_gyroFullScale125dps   = 1,
  ISDS_gyroFullScale500dps   = 2,
  ISDS_gyroFullScale1000dps  = 4,
  ISDS_gyroFullScale2000dps  = 6
} ISDS_gyroFullScale_t;

typedef enum
{
  ISDS_gyroOdrOff    =  0,
  ISDS_gyroOdr12Hz5  =  1,
  ISDS_gyroOdr26Hz   =  2,
  ISDS_gyroOdr52Hz   =  3,
  ISDS_gyroOdr104Hz  =  4,
  ISDS_gyroOdr208Hz  =  5,
  ISDS_gyroOdr416Hz  =  6,
  ISDS_gyroOdr833Hz  =  7,
  ISDS_gyroOdr1k66Hz =  8,
  ISDS_gyroOdr3k33Hz =  9,
  ISDS_gyroOdr6k66Hz = 10
} ISDS_gyroOutputDataRate_t;

typedef enum
{
  ISDS_spiModeFourWire  = 0,
  ISDS_spiModeThreeWire = 1
} ISDS_spiMode_t;

typedef enum
{
  ISDS_pushPull  = 0,
  ISDS_openDrain = 1
} ISDS_interruptPinConfig_t;

typedef enum
{
  ISDS_activeHigh = 0,
  ISDS_activeLow  = 1
} ISDS_interruptActiveLevel_t;

typedef enum
{
  ISDS_accSelfTestModeOff      = 0,
  ISDS_accSelfTestModePositive = 1,
  ISDS_accSelfTestModeNegative = 2
} ISDS_accSelfTestMode_t;

typedef enum
{
  ISDS_gyroSelfTestModeOff      = 0,
  ISDS_gyroSelfTestModePositive = 1,
  ISDS_gyroSelfTestModeNegative = 3
} ISDS_gyroSelfTestMode_t;

typedef enum
{
  ISDS_roundingPatternNoRounding                  = 0,   /**< No rounding */
  ISDS_roundingPatternAccOnly                     = 1,   /**< Accelerometer only */
  ISDS_roundingPatternGyroOnly                    = 2,   /**< Gyroscope only */
  ISDS_roundingPatternGyroAndAcc                  = 3    /**< Gyroscope and accelerometer */
} ISDS_roundingPattern_t;

typedef enum
{
  ISDS_dataEnableTriggerModeDisabled               = 0,
  ISDS_dataEnableTriggerModeEdgeSensitiveTrigger   = 4,
  ISDS_dataEnableTriggerModeLevelSensitiveTrigger  = 2,
  ISDS_dataEnableTriggerModeLevelSensitiveLatched  = 3,
  ISDS_dataEnableTriggerModeLevelSensitiveFifo     = 6
} ISDS_dataEnableTriggerMode_t;

typedef enum
{
  ISDS_gyroLPF_ODR800Hz_BW245Hz_PhaseDelay14Deg = 0,
  ISDS_gyroLPF_ODR800Hz_BW195Hz_PhaseDelay17Deg = 1,
  ISDS_gyroLPF_ODR800Hz_BW155Hz_PhaseDelay19Deg = 2,
  ISDS_gyroLPF_ODR800Hz_BW293Hz_PhaseDelay13Deg = 3,
  ISDS_gyroLPF_ODR1k6Hz_BW315Hz_PhaseDelay10Deg = 0,
  ISDS_gyroLPF_ODR1k6Hz_BW224Hz_PhaseDelay12Deg = 1,
  ISDS_gyroLPF_ODR1k6Hz_BW168Hz_PhaseDelay15Deg = 2,
  ISDS_gyroLPF_ODR1k6Hz_BW505Hz_PhaseDelay8Deg  = 3,
  ISDS_gyroLPF_ODR3k3Hz_BW343Hz_PhaseDelay8Deg  = 0,
  ISDS_gyroLPF_ODR3k3Hz_BW234Hz_PhaseDelay10Deg = 1,
  ISDS_gyroLPF_ODR3k3Hz_BW172Hz_PhaseDelay12Deg = 2,
  ISDS_gyroLPF_ODR3k3Hz_BW925Hz_PhaseDelay6Deg  = 3,
  ISDS_gyroLPF_ODR6k6Hz_BW351Hz_PhaseDelay7Deg  = 0,
  ISDS_gyroLPF_ODR6k6Hz_BW237Hz_PhaseDelay9Deg  = 1,
  ISDS_gyroLPF_ODR6k6Hz_BW173Hz_PhaseDelay11Deg = 2,
  ISDS_gyroLPF_ODR6k6Hz_BW937Hz_PhaseDelay5Deg  = 3
} ISDS_gyroLPF_t;

typedef enum
{
  ISDS_gyroDigitalHighPassCutoff16mHz  = 0,
  ISDS_gyroDigitalHighPassCutoff65mHz  = 1,
  ISDS_gyroDigitalHighPassCutoff260mHz = 2,
  ISDS_gyroDigitalHighPassCutoff1Hz04  = 3
} ISDS_gyroDigitalHighPassCutoff_t;

typedef enum
{
  ISDS_accFilterConfig_LowPassPath_ODRDiv50   = 0,
  ISDS_accFilterConfig_LowPassPath_ODRDiv100  = 1,
  ISDS_accFilterConfig_LowPassPath_ODRDiv9    = 2,
  ISDS_accFilterConfig_LowPassPath_ODRDiv400  = 3,
  ISDS_accFilterConfig_HighPassPath_ODRDiv4   = 0,
  ISDS_accFilterConfig_HighPassPath_ODRDiv100 = 1,
  ISDS_accFilterConfig_HighPassPath_ODRDiv9   = 2,
  ISDS_accFilterConfig_HighPassPath_ODRDiv400 = 3
} ISDS_accFilterConfig_t;

typedef enum
{
  ISDS_dataEnableStampingSensorGyro = 0,
  ISDS_dataEnableStampingSensorAcc  = 1
} ISDS_dataEnableStampingSensor_t;

typedef enum
{
  ISDS_activityFilterSlope    = 0,
  ISDS_activityFilterHighPass = 1
} ISDS_activityFilter_t;

typedef enum
{
  ISDS_inactivityFunctionDisabled              = 0,
  ISDS_inactivityFunctionAcc12Hz5GyroUnchanged = 1,
  ISDS_inactivityFunctionAcc12Hz5GyroSleep     = 2,
  ISDS_inactivityFunctionAcc12Hz5GyroPowerDown = 3
} ISDS_inactivityFunction_t;

typedef enum
{
  ISDS_sixDThresholdEightyDeg   = 0,
  ISDS_sixDThresholdSeventyDeg  = 1,
  ISDS_sixDThresholdSixtyDeg    = 2,
  ISDS_sixDThresholdFiftyDeg    = 3
} ISDS_sixDThreshold_t;

typedef enum
{
  ISDS_freeFallThreshold156mg = 0,
  ISDS_freeFallThreshold219mg = 1,
  ISDS_freeFallThreshold250mg = 2,
  ISDS_freeFallThreshold312mg = 3,
  ISDS_freeFallThreshold344mg = 4,
  ISDS_freeFallThreshold406mg = 5,
  ISDS_freeFallThreshold469mg = 6,
  ISDS_freeFallThreshold500mg = 7
} ISDS_freeFallThreshold_t;



#ifdef __cplusplus
extern "C"
{
#endif

  /*         Function definitions         */

  int8_t ISDS_getDefaultInterface(WE_sensorInterface_t* sensorInterface);

  int8_t ISDS_getDeviceID(WE_sensorInterface_t* sensorInterface, uint8_t *deviceID);

  /* ISDS_FIFO_CTRL_1_REG */
  /* ISDS_FIFO_CTRL_2_REG */
  int8_t ISDS_setFifoThreshold(WE_sensorInterface_t* sensorInterface, uint16_t threshold);
  int8_t ISDS_getFifoThreshold(WE_sensorInterface_t* sensorInterface, uint16_t *threshold);
  int8_t ISDS_enableFifoTemperature(WE_sensorInterface_t* sensorInterface, ISDS_state_t fifoTemp);
  int8_t ISDS_isFifoTemperatureEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *fifoTemp);

  /* ISDS_FIFO_CTRL_3_REG */
  int8_t ISDS_setFifoAccDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation);
  int8_t ISDS_getFifoAccDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation);
  int8_t ISDS_setFifoGyroDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation);
  int8_t ISDS_getFifoGyroDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation);

  /* ISDS_FIFO_CTRL_4_REG */
  int8_t ISDS_setFifoDataset3Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation);
  int8_t ISDS_getFifoDataset3Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation);
  int8_t ISDS_setFifoDataset4Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation);
  int8_t ISDS_getFifoDataset4Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation);
  int8_t ISDS_enableFifoOnlyHighData(WE_sensorInterface_t* sensorInterface, ISDS_state_t onlyHighData);
  int8_t ISDS_isFifoOnlyHighDataEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *onlyHighData);
  int8_t ISDS_enableFifoStopOnThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t stopOnThreshold);
  int8_t ISDS_isFifoStopOnThresholdEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *stopOnThreshold);

  /* ISDS_FIFO_CTRL_5_REG */
  int8_t ISDS_setFifoMode(WE_sensorInterface_t* sensorInterface, ISDS_fifoMode_t fifoMode);
  int8_t ISDS_getFifoMode(WE_sensorInterface_t* sensorInterface, ISDS_fifoMode_t *fifoMode);
  int8_t ISDS_setFifoOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_fifoOutputDataRate_t fifoOdr);
  int8_t ISDS_getFifoOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_fifoOutputDataRate_t *fifoOdr);

  /* ISDS_DRDY_PULSE_CFG_REG */
  int8_t ISDS_enableDataReadyPulsed(WE_sensorInterface_t* sensorInterface, ISDS_state_t dataReadyPulsed);
  int8_t ISDS_isDataReadyPulsedEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReadyPulsed);

  /* ISDS_INT0_CTRL_REG */
  int8_t ISDS_enableAccDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0AccDataReady);
  int8_t ISDS_isAccDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0AccDataReady);
  int8_t ISDS_enableGyroDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0GyroDataReady);
  int8_t ISDS_isGyroDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0GyroDataReady);
  int8_t ISDS_enableBootStatusINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0BootStatus);
  int8_t ISDS_isBootStatusINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0BootStatus);
  int8_t ISDS_enableFifoThresholdINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoThreshold);
  int8_t ISDS_isFifoThresholdINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoThreshold);
  int8_t ISDS_enableFifoOverrunINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoOverrun);
  int8_t ISDS_isFifoOverrunINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoOverrun);
  int8_t ISDS_enableFifoFullINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoFull);
  int8_t ISDS_isFifoFullINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoFull);

  /* ISDS_INT1_CTRL_REG */
  int8_t ISDS_enableAccDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1AccDataReady);
  int8_t ISDS_isAccDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1AccDataReady);
  int8_t ISDS_enableGyroDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1GyroDataReady);
  int8_t ISDS_isGyroDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1GyroDataReady);
  int8_t ISDS_enableTemperatureDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1TempDataReady);
  int8_t ISDS_isTemperatureDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1TempDataReady);
  int8_t ISDS_enableFifoThresholdINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoThreshold);
  int8_t ISDS_isFifoThresholdINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoThreshold);
  int8_t ISDS_enableFifoOverrunINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoOverrun);
  int8_t ISDS_isFifoOverrunINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoOverrun);
  int8_t ISDS_enableFifoFullINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoFull);
  int8_t ISDS_isFifoFullINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoFull);

  /* ISDS_DEVICE_ID_REG */

  /* ISDS_CTRL_1_REG */
  int8_t ISDS_setAccAnalogChainBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accAnalogChainBandwidth_t bandwidth);
  int8_t ISDS_getAccAnalogChainBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accAnalogChainBandwidth_t *bandwidth);
  int8_t ISDS_setAccDigitalLpfBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accDigitalLpfBandwidth_t bandwidth);
  int8_t ISDS_getAccDigitalLpfBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accDigitalLpfBandwidth_t *bandwidth);
  int8_t ISDS_setAccFullScale(WE_sensorInterface_t* sensorInterface, ISDS_accFullScale_t fullScale);
  int8_t ISDS_getAccFullScale(WE_sensorInterface_t* sensorInterface, ISDS_accFullScale_t *fullScale);
  int8_t ISDS_setAccOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_accOutputDataRate_t odr);
  int8_t ISDS_getAccOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_accOutputDataRate_t *odr);

  /* ISDS_CTRL_2_REG */
  int8_t ISDS_setGyroFullScale(WE_sensorInterface_t* sensorInterface, ISDS_gyroFullScale_t fullScale);
  int8_t ISDS_getGyroFullScale(WE_sensorInterface_t* sensorInterface, ISDS_gyroFullScale_t *fullScale);
  int8_t ISDS_setGyroOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_gyroOutputDataRate_t odr);
  int8_t ISDS_getGyroOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_gyroOutputDataRate_t *odr);

  /* ISDS_CTRL_3_REG */
  int8_t ISDS_softReset(WE_sensorInterface_t* sensorInterface, ISDS_state_t swReset);
  int8_t ISDS_getSoftResetState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *swReset);
  int8_t ISDS_enableAutoIncrement(WE_sensorInterface_t* sensorInterface, ISDS_state_t autoIncr);
  int8_t ISDS_isAutoIncrementEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *autoIncr);
  int8_t ISDS_setSpiMode(WE_sensorInterface_t* sensorInterface, ISDS_spiMode_t spiMode);
  int8_t ISDS_getSpiMode(WE_sensorInterface_t* sensorInterface, ISDS_spiMode_t *spiMode);
  int8_t ISDS_setInterruptPinType(WE_sensorInterface_t* sensorInterface, ISDS_interruptPinConfig_t pinType);
  int8_t ISDS_getInterruptPinType(WE_sensorInterface_t* sensorInterface, ISDS_interruptPinConfig_t *pinType);
  int8_t ISDS_setInterruptActiveLevel(WE_sensorInterface_t* sensorInterface, ISDS_interruptActiveLevel_t level);
  int8_t ISDS_getInterruptActiveLevel(WE_sensorInterface_t* sensorInterface, ISDS_interruptActiveLevel_t *level);
  int8_t ISDS_enableBlockDataUpdate(WE_sensorInterface_t* sensorInterface, ISDS_state_t bdu);
  int8_t ISDS_isBlockDataUpdateEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *bdu);
  int8_t ISDS_reboot(WE_sensorInterface_t* sensorInterface, ISDS_state_t reboot);
  int8_t ISDS_isRebooting(WE_sensorInterface_t* sensorInterface, ISDS_state_t *rebooting);

  /* ISDS_CTRL_4_REG */
  int8_t ISDS_enableGyroDigitalLpf1(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable);
  int8_t ISDS_isGyroDigitalLpf1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable);
  int8_t ISDS_disableI2CInterface(WE_sensorInterface_t* sensorInterface, ISDS_state_t i2cDisable);
  int8_t ISDS_isI2CInterfaceDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *i2cDisabled);
  int8_t ISDS_enableDataReadyMask(WE_sensorInterface_t* sensorInterface, ISDS_state_t dataReadyMask);
  int8_t ISDS_isDataReadyMaskEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReadyMask);
  int8_t ISDS_enableDataEnableDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0DataReady);
  int8_t ISDS_isDataEnableDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0DataReady);
  int8_t ISDS_setInt1OnInt0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1OnInt0);
  int8_t ISDS_getInt1OnInt0(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1OnInt0);
  int8_t ISDS_enableGyroSleepMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t gyroSleepMode);
  int8_t ISDS_isGyroSleepModeEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *gyroSleepMode);
  int8_t ISDS_extendDataEnableToAcc(WE_sensorInterface_t* sensorInterface, ISDS_state_t extendToAcc);
  int8_t ISDS_isDataEnableExtendedToAcc(WE_sensorInterface_t* sensorInterface, ISDS_state_t *extendToAcc);

  /* ISDS_CTRL_5_REG */
  int8_t ISDS_setAccSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_accSelfTestMode_t selfTest);
  int8_t ISDS_getAccSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_accSelfTestMode_t *selfTest);
  int8_t ISDS_setGyroSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_gyroSelfTestMode_t selfTest);
  int8_t ISDS_getGyroSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_gyroSelfTestMode_t *selfTest);
  int8_t ISDS_setDataEnableActiveHigh(WE_sensorInterface_t* sensorInterface, ISDS_state_t activeHigh);
  int8_t ISDS_isDataEnableActiveHigh(WE_sensorInterface_t* sensorInterface, ISDS_state_t *activeHigh);
  int8_t ISDS_setRoundingPattern(WE_sensorInterface_t* sensorInterface, ISDS_roundingPattern_t roundingPattern);
  int8_t ISDS_getRoundingPattern(WE_sensorInterface_t* sensorInterface, ISDS_roundingPattern_t *roundingPattern);

  /* ISDS_CTRL_6_REG */
  int8_t ISDS_setGyroLowPassFilterBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_gyroLPF_t bandwidth);
  int8_t ISDS_getGyroLowPassFilterBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_gyroLPF_t *bandwidth);
  int8_t ISDS_setOffsetWeight(WE_sensorInterface_t* sensorInterface, ISDS_state_t offsetWeight);
  int8_t ISDS_getOffsetWeight(WE_sensorInterface_t* sensorInterface, ISDS_state_t *offsetWeight);
  int8_t ISDS_disableAccHighPerformanceMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t disable);
  int8_t ISDS_isAccHighPerformanceModeDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *disable);
  int8_t ISDS_setDataEnableTriggerMode(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableTriggerMode_t triggerMode);
  int8_t ISDS_getDataEnableTriggerMode(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableTriggerMode_t *triggerMode);

  /* ISDS_CTRL_7_REG */
  int8_t ISDS_enableRounding(WE_sensorInterface_t* sensorInterface, ISDS_state_t rounding);
  int8_t ISDS_isRoundingEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *rounding);
  int8_t ISDS_setGyroDigitalHighPassCutoff(WE_sensorInterface_t* sensorInterface, ISDS_gyroDigitalHighPassCutoff_t cutoff);
  int8_t ISDS_getGyroDigitalHighPassCutoff(WE_sensorInterface_t* sensorInterface, ISDS_gyroDigitalHighPassCutoff_t *cutoff);
  int8_t ISDS_enableGyroDigitalHighPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t highPass);
  int8_t ISDS_isGyroDigitalHighPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *highPass);
  int8_t ISDS_disableGyroHighPerformanceMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t disable);
  int8_t ISDS_isGyroHighPerformanceModeDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *disable);

  /* ISDS_CTRL_8_REG */
  int8_t ISDS_enable6dLowPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t lowPass);
  int8_t ISDS_is6dLowPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lowPass);
  int8_t ISDS_enableAccHighPassSlopeFilter(WE_sensorInterface_t* sensorInterface, ISDS_state_t filterEnable);
  int8_t ISDS_isAccHighPassSlopeFilterEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *filterEnable);
  int8_t ISDS_setInputCompositeFilter(WE_sensorInterface_t* sensorInterface, ISDS_inputCompositeFilter_t inputCompositeFilter);
  int8_t ISDS_getInputCompositeFilter(WE_sensorInterface_t* sensorInterface, ISDS_inputCompositeFilter_t *inputCompositeFilter);
  int8_t ISDS_enableHighPassFilterRefMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t refMode);
  int8_t ISDS_isHighPassFilterRefModeEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *refMode);
  int8_t ISDS_setAccFilterConfig(WE_sensorInterface_t* sensorInterface, ISDS_accFilterConfig_t filterConfig);
  int8_t ISDS_getAccFilterConfig(WE_sensorInterface_t* sensorInterface, ISDS_accFilterConfig_t *filterConfig);
  int8_t ISDS_enableAccLowPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t lowPass);
  int8_t ISDS_isAccLowPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lowPass);

  /* ISDS_CTRL_9_REG */
  int8_t ISDS_setDataEnableStampingSensor(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableStampingSensor_t sensor);
  int8_t ISDS_getDataEnableStampingSensor(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableStampingSensor_t *sensor);
  int8_t ISDS_storeDataEnableValueInZAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable);
  int8_t ISDS_isStoreDataEnableValueInZAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable);
  int8_t ISDS_storeDataEnableValueInYAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable);
  int8_t ISDS_isStoreDataEnableValueInYAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable);
  int8_t ISDS_storeDataEnableValueInXAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable);
  int8_t ISDS_isStoreDataEnableValueInXAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable);

  /* ISDS_CTRL_10_REG */
  int8_t ISDS_enableEmbeddedFunctionalities(WE_sensorInterface_t* sensorInterface, ISDS_state_t embeddedFuncEnable);
  int8_t ISDS_areEmbeddedFunctionalitiesEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *embeddedFuncEnable);
  int8_t ISDS_enableTiltCalculation(WE_sensorInterface_t* sensorInterface, ISDS_state_t tiltCalc);
  int8_t ISDS_isTiltCalculationEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tiltCalc);

  /* ISDS_WAKE_UP_EVENT_REG */
  int8_t ISDS_getWakeUpEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_wakeUpEvent_t *status);
  int8_t ISDS_isWakeUpXEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpX);
  int8_t ISDS_isWakeUpYEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpY);
  int8_t ISDS_isWakeUpZEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpZ);
  int8_t ISDS_isWakeUpEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpState);
  int8_t ISDS_getSleepState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *sleepState);
  int8_t ISDS_isFreeFallEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *freeFall);

  /* ISDS_TAP_EVENT_REG */
  int8_t ISDS_getTapEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_tapEvent_t *status);
  int8_t ISDS_isTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapEventState);
  int8_t ISDS_isTapEventXAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapXAxis);
  int8_t ISDS_isTapEventYAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapYAxis);
  int8_t ISDS_isTapEventZAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapZAxis);
  int8_t ISDS_isDoubleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *doubleTap);
  int8_t ISDS_isSingleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *singleTap);
  int8_t ISDS_getTapSign(WE_sensorInterface_t* sensorInterface, ISDS_tapSign_t *tapSign);

  /* ISDS_6D_EVENT_REG */
  int8_t ISDS_get6dEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_6dEvent_t *status);
  int8_t ISDS_has6dOrientationChanged(WE_sensorInterface_t* sensorInterface, ISDS_state_t *orientationChanged);
  int8_t ISDS_isXLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *xlOverThreshold);
  int8_t ISDS_isXHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *xhOverThreshold);
  int8_t ISDS_isYLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *ylOverThreshold);
  int8_t ISDS_isYHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *yhOverThreshold);
  int8_t ISDS_isZLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *zlOverThreshold);
  int8_t ISDS_isZHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *zhOverThreshold);
  int8_t ISDS_isDataEnableDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady);

  /* ISDS_STATUS_REG */
  int8_t ISDS_getStatusRegister(WE_sensorInterface_t* sensorInterface, ISDS_status_t *status);
  int8_t ISDS_isAccelerationDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady);
  int8_t ISDS_isGyroscopeDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady);
  int8_t ISDS_isTemperatureDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady);
  int8_t ISDS_isDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *temp_state, ISDS_state_t *acc_state, ISDS_state_t *gyro_state);

  /* ISDS_FIFO_STATUS_#_REG */
  int8_t ISDS_getFifoStatus(WE_sensorInterface_t* sensorInterface, ISDS_fifoStatus2_t *status, uint16_t *fillLevel, uint16_t *fifoPattern);
  int8_t ISDS_getFifoStatus2Register(WE_sensorInterface_t* sensorInterface, ISDS_fifoStatus2_t *status);
  int8_t ISDS_getFifoFillLevel(WE_sensorInterface_t* sensorInterface, uint16_t *fillLevel);
  int8_t ISDS_isFifoEmpty(WE_sensorInterface_t* sensorInterface, ISDS_state_t *empty);
  int8_t ISDS_isFifoFull(WE_sensorInterface_t* sensorInterface, ISDS_state_t *full);
  int8_t ISDS_getFifoOverrunState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *overrun);
  int8_t ISDS_isFifoThresholdReached(WE_sensorInterface_t* sensorInterface, ISDS_state_t *threshReached);
  int8_t ISDS_getFifoPattern(WE_sensorInterface_t* sensorInterface, uint16_t *fifoPattern);

  /* ISDS_FUNC_SRC_1_REG */
  int8_t ISDS_isTiltEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tiltEvent);

  /* ISDS_TAP_CFG_REG */
  int8_t ISDS_enableLatchedInterrupt(WE_sensorInterface_t* sensorInterface, ISDS_state_t lir);
  int8_t ISDS_isLatchedInterruptEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lir);
  int8_t ISDS_enableTapX(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapX);
  int8_t ISDS_isTapXEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapX);
  int8_t ISDS_enableTapY(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapY);
  int8_t ISDS_isTapYEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapY);
  int8_t ISDS_enableTapZ(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapZ);
  int8_t ISDS_isTapZEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapZ);
  int8_t ISDS_setActivityFilter(WE_sensorInterface_t* sensorInterface, ISDS_activityFilter_t filter);
  int8_t ISDS_getActivityFilter(WE_sensorInterface_t* sensorInterface, ISDS_activityFilter_t *filter);
  int8_t ISDS_setInactivityFunction(WE_sensorInterface_t* sensorInterface, ISDS_inactivityFunction_t function);
  int8_t ISDS_getInactivityFunction(WE_sensorInterface_t* sensorInterface, ISDS_inactivityFunction_t *function);
  int8_t ISDS_enableInterrupts(WE_sensorInterface_t* sensorInterface, ISDS_state_t interruptsEnable);
  int8_t ISDS_areInterruptsEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *interruptsEnable);

  /* ISDS_TAP_THS_6D_REG */
  int8_t ISDS_setTapThreshold(WE_sensorInterface_t* sensorInterface, uint8_t tapThreshold);
  int8_t ISDS_getTapThreshold(WE_sensorInterface_t* sensorInterface, uint8_t *tapThreshold);
  int8_t ISDS_set6DThreshold(WE_sensorInterface_t* sensorInterface, ISDS_sixDThreshold_t threshold6D);
  int8_t ISDS_get6DThreshold(WE_sensorInterface_t* sensorInterface, ISDS_sixDThreshold_t *threshold6D);
  int8_t ISDS_enable4DDetection(WE_sensorInterface_t* sensorInterface, ISDS_state_t detection4D);
  int8_t ISDS_is4DDetectionEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *detection4D);

  /* ISDS_INT_DUR2_REG */
  int8_t ISDS_setTapLatencyTime(WE_sensorInterface_t* sensorInterface, uint8_t latencyTime);
  int8_t ISDS_getTapLatencyTime(WE_sensorInterface_t* sensorInterface, uint8_t *latencyTime);
  int8_t ISDS_setTapQuietTime(WE_sensorInterface_t* sensorInterface, uint8_t quietTime);
  int8_t ISDS_getTapQuietTime(WE_sensorInterface_t* sensorInterface, uint8_t *quietTime);
  int8_t ISDS_setTapShockTime(WE_sensorInterface_t* sensorInterface, uint8_t shockTime);
  int8_t ISDS_getTapShockTime(WE_sensorInterface_t* sensorInterface, uint8_t *shockTime);

  /* ISDS_WAKE_UP_THS_REG */
  int8_t ISDS_setWakeUpThreshold(WE_sensorInterface_t* sensorInterface, uint8_t thresh);
  int8_t ISDS_getWakeUpThreshold(WE_sensorInterface_t* sensorInterface, uint8_t *thresh);
  int8_t ISDS_enableDoubleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t doubleTapEnable);
  int8_t ISDS_isDoubleTapEventEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *doubleTapEnable);

  /* ISDS_WAKE_UP_DUR_REG */
  int8_t ISDS_setSleepDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration);
  int8_t ISDS_getSleepDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration);
  int8_t ISDS_setWakeUpDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration);
  int8_t ISDS_getWakeUpDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration);

  /* ISDS_FREE_FALL_REG */
  int8_t ISDS_setFreeFallThreshold(WE_sensorInterface_t* sensorInterface, ISDS_freeFallThreshold_t thresh);
  int8_t ISDS_getFreeFallThreshold(WE_sensorInterface_t* sensorInterface, ISDS_freeFallThreshold_t *thresh);
  int8_t ISDS_setFreeFallDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration);
  int8_t ISDS_getFreeFallDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration);

  /* ISDS_MD1_CFG_REG */
  int8_t ISDS_enableTiltINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0Tilt);
  int8_t ISDS_isTiltINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0Tilt);
  int8_t ISDS_enable6dINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int06d);
  int8_t ISDS_is6dINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int06d);
  int8_t ISDS_enableDoubleTapINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0DoubleTap);
  int8_t ISDS_isDoubleTapINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0DoubleTap);
  int8_t ISDS_enableFreeFallINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FreeFall);
  int8_t ISDS_isFreeFallINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FreeFall);
  int8_t ISDS_enableWakeUpINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0WakeUp);
  int8_t ISDS_isWakeUpINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0WakeUp);
  int8_t ISDS_enableSingleTapINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0SingleTap);
  int8_t ISDS_isSingleTapINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0SingleTap);
  int8_t ISDS_enableInactivityStateINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0InactivityState);
  int8_t ISDS_isInactivityStateINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0InactivityState);

  /* ISDS_MD2_CFG_REG */
  int8_t ISDS_enableTiltINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1Tilt);
  int8_t ISDS_isTiltINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1Tilt);
  int8_t ISDS_enable6dINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int16d);
  int8_t ISDS_is6dINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int16d);
  int8_t ISDS_enableDoubleTapINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1DoubleTap);
  int8_t ISDS_isDoubleTapINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1DoubleTap);
  int8_t ISDS_enableFreeFallINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FreeFall);
  int8_t ISDS_isFreeFallINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FreeFall);
  int8_t ISDS_enableWakeUpINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1WakeUp);
  int8_t ISDS_isWakeUpINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1WakeUp);
  int8_t ISDS_enableSingleTapINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1SingleTap);
  int8_t ISDS_isSingleTapINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1SingleTap);
  int8_t ISDS_enableInactivityStateINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1InactivityState);
  int8_t ISDS_isInactivityStateINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1InactivityState);

  /* ISDS_#_OFS_USR_REG */
  int8_t ISDS_setOffsetValueX(WE_sensorInterface_t* sensorInterface, int8_t offsetValueXAxis);
  int8_t ISDS_getOffsetValueX(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueXAxis);
  int8_t ISDS_setOffsetValueY(WE_sensorInterface_t* sensorInterface, int8_t offsetValueYAxis);
  int8_t ISDS_getOffsetValueY(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueYAxis);
  int8_t ISDS_setOffsetValueZ(WE_sensorInterface_t* sensorInterface, int8_t offsetValueZAxis);
  int8_t ISDS_getOffsetValueZ(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueZAxis);

  /* ISDS_FIFO_DATA_OUT_L_REG */
  /* ISDS_FIFO_DATA_OUT_H_REG */
  int8_t ISDS_getFifoData(WE_sensorInterface_t* sensorInterface, uint16_t numSamples, uint16_t *fifoData);

  /* Gyroscope output */
#ifdef WE_USE_FLOAT
  int8_t ISDS_getAngularRateX_float(WE_sensorInterface_t* sensorInterface, float *xRate);
  int8_t ISDS_getAngularRateY_float(WE_sensorInterface_t* sensorInterface, float *yRate);
  int8_t ISDS_getAngularRateZ_float(WE_sensorInterface_t* sensorInterface, float *zRate);
  int8_t ISDS_getAngularRates_float(WE_sensorInterface_t* sensorInterface, float *xRate, float *yRate, float *zRate);
#else
  #warning "WSEN_ISDS sensor driver: Float support is turned off by default. Define WE_USE_FLOAT to enable float support."
#endif /* WE_USE_FLOAT */
  int8_t ISDS_getAngularRateX_int(WE_sensorInterface_t* sensorInterface, int32_t *xRate);
  int8_t ISDS_getAngularRateY_int(WE_sensorInterface_t* sensorInterface, int32_t *yRate);
  int8_t ISDS_getAngularRateZ_int(WE_sensorInterface_t* sensorInterface, int32_t *zRate);
  int8_t ISDS_getAngularRates_int(WE_sensorInterface_t* sensorInterface, int32_t *xRate, int32_t *yRate, int32_t *zRate);
  int8_t ISDS_getRawAngularRateX(WE_sensorInterface_t* sensorInterface, int16_t *xRawRate);
  int8_t ISDS_getRawAngularRateY(WE_sensorInterface_t* sensorInterface, int16_t *yRawRate);
  int8_t ISDS_getRawAngularRateZ(WE_sensorInterface_t* sensorInterface, int16_t *zRawRate);
  int8_t ISDS_getRawAngularRates(WE_sensorInterface_t* sensorInterface, int16_t *xRawRate, int16_t *yRawRate, int16_t *zRawRate);

  /* Accelerometer output */
#ifdef WE_USE_FLOAT
  int8_t ISDS_getAccelerationX_float(WE_sensorInterface_t* sensorInterface, float *xAcc);
  int8_t ISDS_getAccelerationY_float(WE_sensorInterface_t* sensorInterface, float *yAcc);
  int8_t ISDS_getAccelerationZ_float(WE_sensorInterface_t* sensorInterface, float *zAcc);
  int8_t ISDS_getAccelerations_float(WE_sensorInterface_t* sensorInterface, float *xAcc, float *yAcc, float *zAcc);
#endif /* WE_USE_FLOAT */
  int8_t ISDS_getAccelerationX_int(WE_sensorInterface_t* sensorInterface, int16_t *xAcc);
  int8_t ISDS_getAccelerationY_int(WE_sensorInterface_t* sensorInterface, int16_t *yAcc);
  int8_t ISDS_getAccelerationZ_int(WE_sensorInterface_t* sensorInterface, int16_t *zAcc);
  int8_t ISDS_getAccelerations_int(WE_sensorInterface_t* sensorInterface, int16_t *xAcc, int16_t *yAcc, int16_t *zAcc);
  int8_t ISDS_getRawAccelerationX(WE_sensorInterface_t* sensorInterface, int16_t *xRawAcc);
  int8_t ISDS_getRawAccelerationY(WE_sensorInterface_t* sensorInterface, int16_t *yRawAcc);
  int8_t ISDS_getRawAccelerationZ(WE_sensorInterface_t* sensorInterface, int16_t *zRawAcc);
  int8_t ISDS_getRawAccelerations(WE_sensorInterface_t* sensorInterface, int16_t *xRawAcc, int16_t *yRawAcc, int16_t *zRawAcc);

  /* Temperature sensor output */
#ifdef WE_USE_FLOAT
  int8_t ISDS_getTemperature_float(WE_sensorInterface_t* sensorInterface, float *temperature);
#endif /* WE_USE_FLOAT */
  int8_t ISDS_getTemperature_int(WE_sensorInterface_t* sensorInterface, int16_t *temperature);
  int8_t ISDS_getRawTemperature(WE_sensorInterface_t* sensorInterface, int16_t *temperature);


#ifdef WE_USE_FLOAT
  float ISDS_convertAcceleration_float(int16_t acc, ISDS_accFullScale_t fullScale);
  float ISDS_convertAccelerationFs2g_float(int16_t acc);
  float ISDS_convertAccelerationFs4g_float(int16_t acc);
  float ISDS_convertAccelerationFs8g_float(int16_t acc);
  float ISDS_convertAccelerationFs16g_float(int16_t acc);

  float ISDS_convertAngularRate_float(int16_t rate, ISDS_gyroFullScale_t fullScale);
  float ISDS_convertAngularRateFs125dps_float(int16_t rate);
  float ISDS_convertAngularRateFs250dps_float(int16_t rate);
  float ISDS_convertAngularRateFs500dps_float(int16_t rate);
  float ISDS_convertAngularRateFs1000dps_float(int16_t rate);
  float ISDS_convertAngularRateFs2000dps_float(int16_t rate);

  float ISDS_convertTemperature_float(int16_t temperature);
#endif /* WE_USE_FLOAT */

  int16_t ISDS_convertAcceleration_int(int16_t acc, ISDS_accFullScale_t fullScale);
  int16_t ISDS_convertAccelerationFs2g_int(int16_t acc);
  int16_t ISDS_convertAccelerationFs4g_int(int16_t acc);
  int16_t ISDS_convertAccelerationFs8g_int(int16_t acc);
  int16_t ISDS_convertAccelerationFs16g_int(int16_t acc);

  int32_t ISDS_convertAngularRate_int(int16_t rate, ISDS_gyroFullScale_t fullScale);
  int32_t ISDS_convertAngularRateFs125dps_int(int16_t rate);
  int32_t ISDS_convertAngularRateFs250dps_int(int16_t rate);
  int32_t ISDS_convertAngularRateFs500dps_int(int16_t rate);
  int32_t ISDS_convertAngularRateFs1000dps_int(int16_t rate);
  int32_t ISDS_convertAngularRateFs2000dps_int(int16_t rate);

  int16_t ISDS_convertTemperature_int(int16_t temperature);

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_ISDS_H */
