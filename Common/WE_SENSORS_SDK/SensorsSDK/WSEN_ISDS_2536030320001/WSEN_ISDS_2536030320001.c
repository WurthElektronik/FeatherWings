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
 * @brief Driver file for the WSEN-ISDS-2536030320001 sensor.
 */

#include "WSEN_ISDS_2536030320001.h"

#include <stdio.h>

#include <platform.h>

/**
 * @brief Default sensor interface configuration.
 */
static WE_sensorInterface_t isdsDefaultSensorInterface = {
    .sensorType = WE_ISDS,
    .interfaceType = WE_i2c,
    .options = {.i2c = {.address = ISDS_ADDRESS_I2C_0, .burstMode = 0, .protocol = WE_i2cProtocol_RegisterBased, .useRegAddrMsbForMultiBytesRead = 0, .reserved = 0},
                .spi = {.chipSelectPort = 0, .chipSelectPin = 0, .burstMode = 0, .reserved = 0},
                .readTimeout = 1000,
                .writeTimeout = 1000},
    .handle = 0};


/**
 * @brief Stores the current value of the accelerometer full scale parameter.

 * The value is updated when calling ISDS_setAccFullScale() or
 * ISDS_getAccFullScale().
 *
 */
static ISDS_accFullScale_t currentAccFullScale = ISDS_accFullScaleTwoG;

/**
 * @brief Stores the current value of the gyroscope full scale parameter.

 * The value is updated when calling ISDS_setGyroFullScale() or
 * ISDS_getGyroFullScale().
 *
 */
static ISDS_gyroFullScale_t currentGyroFullScale = ISDS_gyroFullScale250dps;


/**
 * @brief Read data from sensor.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] regAdr Address of register to read from
 * @param[in] numBytesToRead Number of bytes to be read
 * @param[out] data Target buffer
 * @return Error Code
 */
static inline int8_t ISDS_ReadReg(WE_sensorInterface_t* sensorInterface,
                                  uint8_t regAdr,
                                  uint16_t numBytesToRead,
                                  uint8_t *data)
{
  return WE_ReadReg(sensorInterface, regAdr, numBytesToRead, data);
}

/**
 * @brief Write data to sensor.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] regAdr Address of register to write to
 * @param[in] numBytesToWrite Number of bytes to be written
 * @param[in] data Source buffer
 * @return Error Code
 */
static inline int8_t ISDS_WriteReg(WE_sensorInterface_t* sensorInterface,
                                   uint8_t regAdr,
                                   uint16_t numBytesToWrite,
                                   uint8_t *data)
{
  return WE_WriteReg(sensorInterface, regAdr, numBytesToWrite, data);
}

/**
 * @brief Returns the default sensor interface configuration.
 * @param[out] sensorInterface Sensor interface configuration (output parameter)
 * @return Error code
 */
int8_t ISDS_getDefaultInterface(WE_sensorInterface_t* sensorInterface)
{
  *sensorInterface = isdsDefaultSensorInterface;
  return WE_SUCCESS;
}

/**
 * @brief Read the device ID
 *
 * Expected value is ISDS_DEVICE_ID_VALUE.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] deviceID The returned device ID.
 * @retval Error code
 */
int8_t ISDS_getDeviceID(WE_sensorInterface_t* sensorInterface, uint8_t *deviceID)
{
  return ISDS_ReadReg(sensorInterface, ISDS_DEVICE_ID_REG, 1, deviceID);
}


/* ISDS_FIFO_CTRL_1_REG */
/* ISDS_FIFO_CTRL_2_REG */

/**
 * @brief Set the FIFO threshold of the sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] threshold FIFO threshold (value between 0 and 127)
 * @retval Error code
 */
int8_t ISDS_setFifoThreshold(WE_sensorInterface_t* sensorInterface, uint16_t threshold)
{
  ISDS_fifoCtrl1_t fifoCtrl1;
  ISDS_fifoCtrl2_t fifoCtrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2))
  {
    return WE_FAIL;
  }

  fifoCtrl1.fifoThresholdLsb = (uint8_t) (threshold & 0xFF);
  fifoCtrl2.fifoThresholdMsb = (uint8_t) ((threshold >> 8) & 0x07);

  if (WE_FAIL == ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_1_REG, 1, (uint8_t *) &fifoCtrl1))
  {
    return WE_FAIL;
  }
  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2);
}

/**
 * @brief Read the FIFO threshold of the sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] threshold The returned FIFO threshold
 * @retval Error code
 */
int8_t ISDS_getFifoThreshold(WE_sensorInterface_t* sensorInterface, uint16_t *threshold)
{
  ISDS_fifoCtrl1_t fifoCtrl1;
  ISDS_fifoCtrl2_t fifoCtrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_1_REG, 1, (uint8_t *) &fifoCtrl1))
  {
    return WE_FAIL;
  }
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2))
  {
    return WE_FAIL;
  }

  *threshold = ((uint16_t) fifoCtrl1.fifoThresholdLsb) |
               (((uint16_t) (fifoCtrl2.fifoThresholdMsb & 0x07)) << 8);

  return WE_SUCCESS;
}

/**
 * @brief Enable storage of temperature data in FIFO.
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] fifoTemp The storage of temperature data in FIFO enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoTemperature(WE_sensorInterface_t* sensorInterface, ISDS_state_t fifoTemp)
{
  ISDS_fifoCtrl2_t fifoCtrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2))
  {
    return WE_FAIL;
  }

  fifoCtrl2.enFifoTemperature = fifoTemp;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2);
}

/**
 * @brief Check if storage of temperature data in FIFO is enabled.
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fifoTemp The returned storage of temperature data in FIFO enable state
 * @retval Error code
 */
int8_t ISDS_isFifoTemperatureEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *fifoTemp)
{
  ISDS_fifoCtrl2_t fifoCtrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_2_REG, 1, (uint8_t *) &fifoCtrl2))
  {
    return WE_FAIL;
  }

  *fifoTemp = (ISDS_state_t) fifoCtrl2.enFifoTemperature;

  return WE_SUCCESS;
}

/* ISDS_FIFO_CTRL_3_REG */

/**
 * @brief Set decimation of acceleration data in FIFO (second data set in FIFO)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] decimation FIFO acceleration data decimation setting
 * @retval Error code
 */
int8_t ISDS_setFifoAccDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation)
{
  ISDS_fifoCtrl3_t fifoCtrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3))
  {
    return WE_FAIL;
  }

  fifoCtrl3.fifoAccDecimation = (uint8_t) decimation;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3);
}

/**
 * @brief Read decimation of acceleration data in FIFO (second data set in FIFO) setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] decimation The returned FIFO acceleration data decimation setting
 * @retval Error code
 */
int8_t ISDS_getFifoAccDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation)
{
  ISDS_fifoCtrl3_t fifoCtrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3))
  {
    return WE_FAIL;
  }

  *decimation = (ISDS_fifoDecimation_t) fifoCtrl3.fifoAccDecimation;

  return WE_SUCCESS;
}

/**
 * @brief Set decimation of gyroscope data in FIFO (first data set in FIFO)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] decimation FIFO gyroscope data decimation setting
 * @retval Error code
 */
int8_t ISDS_setFifoGyroDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation)
{
  ISDS_fifoCtrl3_t fifoCtrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3))
  {
    return WE_FAIL;
  }

  fifoCtrl3.fifoGyroDecimation = (uint8_t) decimation;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3);
}

/**
 * @brief Read decimation of gyroscope data in FIFO (first data set in FIFO) setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] decimation The returned FIFO gyroscope data decimation setting
 * @retval Error code
 */
int8_t ISDS_getFifoGyroDecimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation)
{
  ISDS_fifoCtrl3_t fifoCtrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_3_REG, 1, (uint8_t *) &fifoCtrl3))
  {
    return WE_FAIL;
  }

  *decimation = (ISDS_fifoDecimation_t) fifoCtrl3.fifoGyroDecimation;

  return WE_SUCCESS;
}


/* ISDS_FIFO_CTRL_4_REG */

/**
 * @brief Set decimation of third data set in FIFO
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] decimation FIFO third data set decimation setting
 * @retval Error code
 */
int8_t ISDS_setFifoDataset3Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  fifoCtrl4.fifoThirdDecimation = (uint8_t) decimation;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4);
}

/**
 * @brief Read decimation of third data set in FIFO setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] decimation The returned FIFO third data set decimation setting
 * @retval Error code
 */
int8_t ISDS_getFifoDataset3Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  *decimation = (ISDS_fifoDecimation_t) fifoCtrl4.fifoThirdDecimation;

  return WE_SUCCESS;
}

/**
 * @brief Set decimation of fourth data set in FIFO
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] decimation FIFO fourth data set decimation setting
 * @retval Error code
 */
int8_t ISDS_setFifoDataset4Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t decimation)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  fifoCtrl4.fifoFourthDecimation = (uint8_t) decimation;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4);
}

/**
 * @brief Read decimation of fourth data set in FIFO setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] decimation The returned FIFO fourth data set decimation setting
 * @retval Error code
 */
int8_t ISDS_getFifoDataset4Decimation(WE_sensorInterface_t* sensorInterface, ISDS_fifoDecimation_t *decimation)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  *decimation = (ISDS_fifoDecimation_t) fifoCtrl4.fifoFourthDecimation;

  return WE_SUCCESS;
}

/**
 * @brief Enable storage of MSB only (8-bit) in FIFO
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] onlyHighData MSB only enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoOnlyHighData(WE_sensorInterface_t* sensorInterface, ISDS_state_t onlyHighData)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  fifoCtrl4.enOnlyHighData = onlyHighData;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4);
}

/**
 * @brief Check if storage of MSB only (8-bit) in FIFO is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] onlyHighData The returned MSB only enable state
 * @retval Error code
 */
int8_t ISDS_isFifoOnlyHighDataEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *onlyHighData)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  *onlyHighData = (ISDS_state_t) fifoCtrl4.enOnlyHighData;

  return WE_SUCCESS;
}

/**
 * @brief Enable stop when FIFO threshold is reached
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] stopOnThreshold FIFO stop on threshold enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoStopOnThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t stopOnThreshold)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  fifoCtrl4.enStopOnThreshold = stopOnThreshold;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4);
}

/**
 * @brief Check if stop when FIFO threshold is reached is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] stopOnThreshold The returned FIFO stop on threshold enable state
 * @retval Error code
 */
int8_t ISDS_isFifoStopOnThresholdEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *stopOnThreshold)
{
  ISDS_fifoCtrl4_t fifoCtrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_4_REG, 1, (uint8_t *) &fifoCtrl4))
  {
    return WE_FAIL;
  }

  *stopOnThreshold = (ISDS_state_t) fifoCtrl4.enStopOnThreshold;

  return WE_SUCCESS;
}


/* ISDS_FIFO_CTRL_5_REG */

/**
 * @brief Set the FIFO mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] fifoMode FIFO mode
 * @retval Error code
 */
int8_t ISDS_setFifoMode(WE_sensorInterface_t* sensorInterface, ISDS_fifoMode_t fifoMode)
{
  ISDS_fifoCtrl5_t fifoCtrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5))
  {
    return WE_FAIL;
  }

  fifoCtrl5.fifoMode = (uint8_t) fifoMode;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5);
}

/**
 * @brief Read the FIFO mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fifoMode The returned FIFO mode
 * @retval Error code
 */
int8_t ISDS_getFifoMode(WE_sensorInterface_t* sensorInterface, ISDS_fifoMode_t *fifoMode)
{
  ISDS_fifoCtrl5_t fifoCtrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5))
  {
    return WE_FAIL;
  }

  *fifoMode = (ISDS_fifoMode_t) fifoCtrl5.fifoMode;

  return WE_SUCCESS;
}

/**
 * @brief Set the FIFO output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] fifoOdr FIFO output data rate
 * @retval Error code
 */
int8_t ISDS_setFifoOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_fifoOutputDataRate_t fifoOdr)
{
  ISDS_fifoCtrl5_t fifoCtrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5))
  {
    return WE_FAIL;
  }

  fifoCtrl5.fifoOdr = (uint8_t) fifoOdr;

  return ISDS_WriteReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5);
}

/**
 * @brief Read the FIFO output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fifoOdr The returned FIFO output data rate
 * @retval Error code
 */
int8_t ISDS_getFifoOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_fifoOutputDataRate_t *fifoOdr)
{
  ISDS_fifoCtrl5_t fifoCtrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_CTRL_5_REG, 1, (uint8_t *) &fifoCtrl5))
  {
    return WE_FAIL;
  }

  *fifoOdr = (ISDS_fifoOutputDataRate_t) fifoCtrl5.fifoOdr;

  return WE_SUCCESS;
}


/* ISDS_DRDY_PULSE_CFG_REG */

/**
 * @brief Enable pulsed data ready mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] dataReadyPulsed Data ready pulsed mode enable state
 * @retval Error code
 */
int8_t ISDS_enableDataReadyPulsed(WE_sensorInterface_t* sensorInterface, ISDS_state_t dataReadyPulsed)
{
  ISDS_dataReadyPulseCfg_t dataReadyPulseCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_DRDY_PULSE_CFG_REG, 1, (uint8_t *) &dataReadyPulseCfg))
  {
    return WE_FAIL;
  }

  dataReadyPulseCfg.enDataReadyPulsed = (uint8_t) dataReadyPulsed;

  return ISDS_WriteReg(sensorInterface, ISDS_DRDY_PULSE_CFG_REG, 1, (uint8_t *) &dataReadyPulseCfg);
}

/**
 * @brief Check if pulsed data ready mode is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReadyPulsed The returned data ready pulsed mode enable state
 * @retval Error code
 */
int8_t ISDS_isDataReadyPulsedEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReadyPulsed)
{
  ISDS_dataReadyPulseCfg_t dataReadyPulseCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_DRDY_PULSE_CFG_REG, 1, (uint8_t *) &dataReadyPulseCfg))
  {
    return WE_FAIL;
  }

  *dataReadyPulsed = (ISDS_state_t) dataReadyPulseCfg.enDataReadyPulsed;

  return WE_SUCCESS;
}


/* ISDS_INT0_CTRL_REG */

/**
 * @brief Enable/disable the acceleration data ready interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0AccDataReady Acceleration data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableAccDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0AccDataReady)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0AccDataReady = (uint8_t) int0AccDataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the acceleration data ready interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0AccDataReady The returned acceleration data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isAccDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0AccDataReady)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0AccDataReady = (ISDS_state_t) int0Ctrl.int0AccDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the gyroscope data ready interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0GyroDataReady Gyroscope data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableGyroDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0GyroDataReady)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0GyroDataReady = (uint8_t) int0GyroDataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the gyroscope data ready interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0GyroDataReady The returned gyroscope data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isGyroDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0GyroDataReady)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0GyroDataReady = (ISDS_state_t) int0Ctrl.int0GyroDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the boot status interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0BootStatus Boot status interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableBootStatusINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0BootStatus)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0Boot = (uint8_t) int0BootStatus;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the boot status interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0BootStatus The returned boot status interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isBootStatusINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0BootStatus)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0BootStatus = (ISDS_state_t) int0Ctrl.int0Boot;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO threshold reached interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0FifoThreshold FIFO threshold reached interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoThresholdINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoThreshold)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0FifoThreshold = (uint8_t) int0FifoThreshold;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the FIFO threshold reached interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0FifoThreshold The returned FIFO threshold reached interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoThresholdINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoThreshold)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0FifoThreshold = (ISDS_state_t) int0Ctrl.int0FifoThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO overrun interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0FifoOverrun FIFO overrun interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoOverrunINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoOverrun)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0FifoOverrun = (uint8_t) int0FifoOverrun;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the FIFO overrun interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0FifoOverrun The returned FIFO overrun interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoOverrunINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoOverrun)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0FifoOverrun = (ISDS_state_t) int0Ctrl.int0FifoOverrun;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO full interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0FifoFull FIFO full interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoFullINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FifoFull)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  int0Ctrl.int0FifoFull = (uint8_t) int0FifoFull;

  return ISDS_WriteReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl);
}

/**
 * @brief Check if the FIFO full interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0FifoFull The returned FIFO full interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoFullINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FifoFull)
{
  ISDS_int0Ctrl_t int0Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT0_CTRL_REG, 1, (uint8_t *) &int0Ctrl))
  {
    return WE_FAIL;
  }

  *int0FifoFull = (ISDS_state_t) int0Ctrl.int0FifoFull;

  return WE_SUCCESS;
}


/* ISDS_INT1_CTRL_REG */

/**
 * @brief Enable/disable the acceleration data ready interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1AccDataReady Acceleration data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableAccDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1AccDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1AccDataReady = (uint8_t) int1AccDataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the acceleration data ready interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1AccDataReady The returned acceleration data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isAccDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1AccDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1AccDataReady = (ISDS_state_t) int1Ctrl.int1AccDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the gyroscope data ready interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1GyroDataReady Gyroscope data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableGyroDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1GyroDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1GyroDataReady = (uint8_t) int1GyroDataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the gyroscope data ready interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1GyroDataReady The returned gyroscope data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isGyroDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1GyroDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1GyroDataReady = (ISDS_state_t) int1Ctrl.int1GyroDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the temperature data ready interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1TempDataReady Temperature data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableTemperatureDataReadyINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1TempDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1TempDataReady = (uint8_t) int1TempDataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the temperature data ready interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1TempDataReady The returned temperature data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isTemperatureDataReadyINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1TempDataReady)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1TempDataReady = (ISDS_state_t) int1Ctrl.int1TempDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO threshold reached interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1FifoThreshold FIFO threshold reached interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoThresholdINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoThreshold)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1FifoThreshold = (uint8_t) int1FifoThreshold;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the FIFO threshold reached interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1FifoThreshold The returned FIFO threshold reached interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoThresholdINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoThreshold)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1FifoThreshold = (ISDS_state_t) int1Ctrl.int1FifoThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO overrun interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1FifoOverrun FIFO overrun interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoOverrunINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoOverrun)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1FifoOverrun = (uint8_t) int1FifoOverrun;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the FIFO overrun interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1FifoOverrun The returned FIFO overrun interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoOverrunINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoOverrun)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1FifoOverrun = (ISDS_state_t) int1Ctrl.int1FifoOverrun;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the FIFO full interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1FifoFull FIFO full interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFifoFullINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FifoFull)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  int1Ctrl.int1FifoFull = (uint8_t) int1FifoFull;

  return ISDS_WriteReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl);
}

/**
 * @brief Check if the FIFO full interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1FifoFull The returned FIFO full interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isFifoFullINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FifoFull)
{
  ISDS_int1Ctrl_t int1Ctrl;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT1_CTRL_REG, 1, (uint8_t *) &int1Ctrl))
  {
    return WE_FAIL;
  }

  *int1FifoFull = (ISDS_state_t) int1Ctrl.int1FifoFull;

  return WE_SUCCESS;
}


/* ISDS_DEVICE_ID_REG */


/* ISDS_CTRL_1_REG */

/**
 * @brief Set the accelerometer analog chain bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] bandwidth Accelerometer analog chain bandwidth
 * @retval Error code
 */
int8_t ISDS_setAccAnalogChainBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accAnalogChainBandwidth_t bandwidth)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  ctrl1.accAnalogBandwidth = bandwidth;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1);
}

/**
 * @brief Read the accelerometer analog chain bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] bandwidth The returned accelerometer analog chain bandwidth
 * @retval Error code
 */
int8_t ISDS_getAccAnalogChainBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accAnalogChainBandwidth_t *bandwidth)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  *bandwidth = (ISDS_accAnalogChainBandwidth_t) ctrl1.accAnalogBandwidth;

  return WE_SUCCESS;
}

/**
 * @brief Set the accelerometer digital LPF (LPF1) bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] bandwidth Accelerometer digital LPF (LPF1) bandwidth
 * @retval Error code
 */
int8_t ISDS_setAccDigitalLpfBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accDigitalLpfBandwidth_t bandwidth)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  ctrl1.accDigitalBandwidth = bandwidth;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1);
}

/**
 * @brief Read the accelerometer digital LPF (LPF1) bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] bandwidth The returned accelerometer digital LPF (LPF1) bandwidth
 * @retval Error code
 */
int8_t ISDS_getAccDigitalLpfBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_accDigitalLpfBandwidth_t *bandwidth)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  *bandwidth = (ISDS_accDigitalLpfBandwidth_t) ctrl1.accDigitalBandwidth;

  return WE_SUCCESS;
}

/**
 * @brief Set the accelerometer full scale
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] fullScale Accelerometer full scale
 * @retval Error code
 */
int8_t ISDS_setAccFullScale(WE_sensorInterface_t* sensorInterface, ISDS_accFullScale_t fullScale)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  ctrl1.accFullScale = fullScale;

  int8_t errCode = ISDS_WriteReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1);

  /* Store current full scale value to allow convenient conversion of sensor readings */
  if (WE_SUCCESS == errCode)
  {
    currentAccFullScale = fullScale;
  }

  return errCode;
}

/**
 * @brief Read the accelerometer full scale
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fullScale The returned accelerometer full scale
 * @retval Error code
 */
int8_t ISDS_getAccFullScale(WE_sensorInterface_t* sensorInterface, ISDS_accFullScale_t *fullScale)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  *fullScale = (ISDS_accFullScale_t) ctrl1.accFullScale;

  /* Store current full scale value to allow convenient conversion of sensor readings */
  currentAccFullScale = *fullScale;

  return WE_SUCCESS;
}

/**
 * @brief Set the accelerometer output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] odr Output data rate
 * @retval Error code
 */
int8_t ISDS_setAccOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_accOutputDataRate_t odr)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  ctrl1.accOutputDataRate = odr;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1);
}

/**
 * @brief Read the accelerometer output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] odr The returned output data rate
 * @retval Error code
 */
int8_t ISDS_getAccOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_accOutputDataRate_t *odr)
{
  ISDS_ctrl1_t ctrl1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_1_REG, 1, (uint8_t *) &ctrl1))
  {
    return WE_FAIL;
  }

  *odr = (ISDS_accOutputDataRate_t) ctrl1.accOutputDataRate;

  return WE_SUCCESS;
}


/* ISDS_CTRL_2_REG */

/**
 * @brief Set the gyroscope full scale
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] fullScale gyroscope full scale
 * @retval Error code
 */
int8_t ISDS_setGyroFullScale(WE_sensorInterface_t* sensorInterface, ISDS_gyroFullScale_t fullScale)
{
  ISDS_ctrl2_t ctrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2))
  {
    return WE_FAIL;
  }

  ctrl2.gyroFullScale = fullScale;

  int8_t errCode = ISDS_WriteReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2);

  /* Store current full scale value to allow convenient conversion of sensor readings */
  if (WE_SUCCESS == errCode)
  {
    currentGyroFullScale = fullScale;
  }

  return errCode;
}

/**
 * @brief Read the gyroscope full scale
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fullScale The returned gyroscope full scale
 * @retval Error code
 */
int8_t ISDS_getGyroFullScale(WE_sensorInterface_t* sensorInterface, ISDS_gyroFullScale_t *fullScale)
{
  ISDS_ctrl2_t ctrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2))
  {
    return WE_FAIL;
  }

  *fullScale = (ISDS_gyroFullScale_t) ctrl2.gyroFullScale;

  /* Store current full scale value to allow convenient conversion of sensor readings */
  currentGyroFullScale = *fullScale;

  return WE_SUCCESS;
}

/**
 * @brief Set the gyroscope output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] odr Output data rate
 * @retval Error code
 */
int8_t ISDS_setGyroOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_gyroOutputDataRate_t odr)
{
  ISDS_ctrl2_t ctrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2))
  {
    return WE_FAIL;
  }

  ctrl2.gyroOutputDataRate = odr;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2);
}

/**
 * @brief Read the gyroscope output data rate
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] odr The returned output data rate.
 * @retval Error code
 */
int8_t ISDS_getGyroOutputDataRate(WE_sensorInterface_t* sensorInterface, ISDS_gyroOutputDataRate_t *odr)
{
  ISDS_ctrl2_t ctrl2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_2_REG, 1, (uint8_t *) &ctrl2))
  {
    return WE_FAIL;
  }

  *odr = (ISDS_gyroOutputDataRate_t) ctrl2.gyroOutputDataRate;

  return WE_SUCCESS;
}


/* ISDS_CTRL_3_REG */

/**
 * @brief Set software reset [enabled, disabled]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] swReset Software reset state
 * @retval Error code
 */
int8_t ISDS_softReset(WE_sensorInterface_t* sensorInterface, ISDS_state_t swReset)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.softReset = swReset;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the software reset state [enabled, disabled]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] swReset The returned software reset state
 * @retval Error code
 */
int8_t ISDS_getSoftResetState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *swReset)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *swReset = (ISDS_state_t) ctrl3.softReset;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable auto increment mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] autoIncr Auto increment mode enable state
 * @retval Error code
 */
int8_t ISDS_enableAutoIncrement(WE_sensorInterface_t* sensorInterface, ISDS_state_t autoIncr)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.autoAddIncr = autoIncr;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the auto increment mode state
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] autoIncr The returned auto increment mode enable state
 * @retval Error code
 */
int8_t ISDS_isAutoIncrementEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *autoIncr)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *autoIncr = (ISDS_state_t) ctrl3.autoAddIncr;

  return WE_SUCCESS;
}

/**
 * @brief Set the SPI serial interface mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] spiMode SPI serial interface mode
 * @retval Error code
 */
int8_t ISDS_setSpiMode(WE_sensorInterface_t* sensorInterface, ISDS_spiMode_t spiMode)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.spiMode = spiMode;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the SPI serial interface mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] spiMode The returned SPI serial interface mode
 * @retval Error code
 */
int8_t ISDS_getSpiMode(WE_sensorInterface_t* sensorInterface, ISDS_spiMode_t *spiMode)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *spiMode = (ISDS_spiMode_t) ctrl3.spiMode;

  return WE_SUCCESS;
}

/**
 * @brief Set the interrupt pin type [push-pull/open-drain]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] pinType Interrupt pin type
 * @retval Error code
 */
int8_t ISDS_setInterruptPinType(WE_sensorInterface_t* sensorInterface, ISDS_interruptPinConfig_t pinType)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.intPinConf = pinType;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the interrupt pin type [push-pull/open-drain]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] pinType The returned interrupt pin type
 * @retval Error code
 */
int8_t ISDS_getInterruptPinType(WE_sensorInterface_t* sensorInterface, ISDS_interruptPinConfig_t *pinType)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *pinType = (ISDS_interruptPinConfig_t) ctrl3.intPinConf;

  return WE_SUCCESS;
}

/**
 * @brief Set the interrupt active level [active high/active low]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] level Interrupt active level
 * @retval Error code
 */
int8_t ISDS_setInterruptActiveLevel(WE_sensorInterface_t* sensorInterface, ISDS_interruptActiveLevel_t level)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.intActiveLevel = level;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the interrupt active level
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] level The returned interrupt active level
 * @retval Error code
 */
int8_t ISDS_getInterruptActiveLevel(WE_sensorInterface_t* sensorInterface, ISDS_interruptActiveLevel_t *level)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *level = (ISDS_interruptActiveLevel_t) ctrl3.intActiveLevel;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable block data update mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] bdu Block data update enable state
 * @retval Error code
 */
int8_t ISDS_enableBlockDataUpdate(WE_sensorInterface_t* sensorInterface, ISDS_state_t bdu)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.blockDataUpdate = bdu;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the block data update state
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] bdu The returned block data update enable state
 * @retval Error code
 */
int8_t ISDS_isBlockDataUpdateEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *bdu)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *bdu = (ISDS_state_t) ctrl3.blockDataUpdate;

  return WE_SUCCESS;
}

/**
 * @brief (Re)boot the device [enabled, disabled]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] reboot Reboot state
 * @retval Error code
 */
int8_t ISDS_reboot(WE_sensorInterface_t* sensorInterface, ISDS_state_t reboot)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  ctrl3.boot = reboot;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3);
}

/**
 * @brief Read the reboot state
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] rebooting The returned reboot state
 * @retval Error code
 */
int8_t ISDS_isRebooting(WE_sensorInterface_t* sensorInterface, ISDS_state_t *rebooting)
{
  ISDS_ctrl3_t ctrl3;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_3_REG, 1, (uint8_t *) &ctrl3))
  {
    return WE_FAIL;
  }

  *rebooting = (ISDS_state_t) ctrl3.boot;

  return WE_SUCCESS;
}


/* ISDS_CTRL_4_REG */

/**
 * @brief Enable gyroscope digital LPF1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] enable Gyroscope digital LPF1 enable state
 * @retval Error code
 */
int8_t ISDS_enableGyroDigitalLpf1(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.enGyroLPF1 = enable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if gyroscope digital LPF1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] enable The returned gyroscope digital LPF1 enable state
 * @retval Error code
 */
int8_t ISDS_isGyroDigitalLpf1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *enable = (ISDS_state_t) ctrl4.enGyroLPF1;

  return WE_SUCCESS;
}

/**
 * @brief Disable the I2C interface
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] i2cDisable I2C interface disable state (0: I2C enabled, 1: I2C disabled)
 * @retval Error code
 */
int8_t ISDS_disableI2CInterface(WE_sensorInterface_t* sensorInterface, ISDS_state_t i2cDisable)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.i2cDisable = i2cDisable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Read the I2C interface disable state [enabled, disabled]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] i2cDisabled The returned I2C interface disable state (0: I2C enabled, 1: I2C disabled)
 * @retval Error code
 */
int8_t ISDS_isI2CInterfaceDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *i2cDisabled)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *i2cDisabled = (ISDS_state_t) ctrl4.i2cDisable;

  return WE_SUCCESS;
}

/**
 * @brief Enable masking of the accelerometer and gyroscope data-ready signals
 * until the settling of the sensor filters is completed
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] dataReadyMask Masking enable state
 * @retval Error code
 */
int8_t ISDS_enableDataReadyMask(WE_sensorInterface_t* sensorInterface, ISDS_state_t dataReadyMask)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.dataReadyMask = dataReadyMask;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if masking of the accelerometer and gyroscope data-ready signals
 * until the settling of the sensor filters is completed is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReadyMask The returned masking enable state
 * @retval Error code
 */
int8_t ISDS_isDataReadyMaskEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReadyMask)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *dataReadyMask = (ISDS_state_t) ctrl4.dataReadyMask;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the data enable (DEN) data ready interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0DataReady Data enable data (DEN) ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableDataEnableDataReadyINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0DataReady)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.dataEnableDataReadyOnInt0 = int0DataReady;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if the data enable (DEN) data ready interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0DataReady The returned data enable (DEN) data ready interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isDataEnableDataReadyINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0DataReady)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *int0DataReady = (ISDS_state_t) ctrl4.dataEnableDataReadyOnInt0;

  return WE_SUCCESS;
}

/**
 * @brief Enable signal routing from INT_1 to INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1OnInt0 Signal routing INT_1 to INT_0 state
 * @retval Error code
 */
int8_t ISDS_setInt1OnInt0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1OnInt0)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.int1OnInt0 = int1OnInt0;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if signal routing from INT_1 to INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1OnInt0 The returned routing enable state.
 * @retval Error code
 */
int8_t ISDS_getInt1OnInt0(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1OnInt0)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *int1OnInt0 = (ISDS_state_t) ctrl4.int1OnInt0;

  return WE_SUCCESS;
}

/**
 * @brief Enable gyroscope sleep mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] gyroSleepMode Gyroscope sleep mode enable state
 * @retval Error code
 */
int8_t ISDS_enableGyroSleepMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t gyroSleepMode)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.enGyroSleepMode = gyroSleepMode;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if gyroscope sleep mode is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] gyroSleepMode The returned gyroscope sleep mode enable state
 * @retval Error code
 */
int8_t ISDS_isGyroSleepModeEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *gyroSleepMode)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *gyroSleepMode = (ISDS_state_t) ctrl4.enGyroSleepMode;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable extension of the data enable (DEN) functionality to accelerometer sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] extendToAcc Extension of data enable (DEN) functionality enable state
 * @retval Error code
 */
int8_t ISDS_extendDataEnableToAcc(WE_sensorInterface_t* sensorInterface, ISDS_state_t extendToAcc)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  ctrl4.dataEnableExtendToAcc = extendToAcc;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4);
}

/**
 * @brief Check if extension of the data enable (DEN) functionality to accelerometer sensor is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] extendToAcc The returned extension of data enable (DEN) functionality enable state
 * @retval Error code
 */
int8_t ISDS_isDataEnableExtendedToAcc(WE_sensorInterface_t* sensorInterface, ISDS_state_t *extendToAcc)
{
  ISDS_ctrl4_t ctrl4;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_4_REG, 1, (uint8_t *) &ctrl4))
  {
    return WE_FAIL;
  }

  *extendToAcc = (ISDS_state_t) ctrl4.dataEnableExtendToAcc;

  return WE_SUCCESS;
}


/* ISDS_CTRL_5_REG */

/**
 * @brief Set the accelerometer self test mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] selfTest Accelerometer self test mode
 * @retval Error code
 */
int8_t ISDS_setAccSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_accSelfTestMode_t selfTest)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  ctrl5.accSelfTest = selfTest;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5);
}

/**
 * @brief Read the accelerometer self test mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] selfTest The returned accelerometer self test mode
 * @retval Error code
 */
int8_t ISDS_getAccSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_accSelfTestMode_t *selfTest)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  *selfTest = (ISDS_accSelfTestMode_t) ctrl5.accSelfTest;

  return WE_SUCCESS;
}

/**
 * @brief Set the gyroscope self test mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] selfTest Gyroscope self test mode
 * @retval Error code
 */
int8_t ISDS_setGyroSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_gyroSelfTestMode_t selfTest)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  ctrl5.gyroSelfTest = selfTest;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5);
}

/**
 * @brief Read the gyroscope self test mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] selfTest The returned gyroscope self test mode
 * @retval Error code
 */
int8_t ISDS_getGyroSelfTestMode(WE_sensorInterface_t* sensorInterface, ISDS_gyroSelfTestMode_t *selfTest)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  *selfTest = (ISDS_gyroSelfTestMode_t) ctrl5.gyroSelfTest;

  return WE_SUCCESS;
}

/**
 * @brief Set the data enable (DEN) active level
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] activeHigh Data enable (DEN) active level (0: active low, 1: active high)
 * @retval Error code
 */
int8_t ISDS_setDataEnableActiveHigh(WE_sensorInterface_t* sensorInterface, ISDS_state_t activeHigh)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  ctrl5.dataEnableActiveLevel = activeHigh;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5);
}

/**
 * @brief Get the data enable (DEN) active level
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] activeHigh The returned data enable (DEN) active level (0: active low, 1: active high)
 * @retval Error code
 */
int8_t ISDS_isDataEnableActiveHigh(WE_sensorInterface_t* sensorInterface, ISDS_state_t *activeHigh)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  *activeHigh = (ISDS_state_t) ctrl5.dataEnableActiveLevel;

  return WE_SUCCESS;
}

/**
 * @brief Set the circular burst-mode (rounding) pattern
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] roundingPattern Rounding pattern
 * @retval Error code
 */
int8_t ISDS_setRoundingPattern(WE_sensorInterface_t* sensorInterface, ISDS_roundingPattern_t roundingPattern)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  ctrl5.rounding = roundingPattern;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5);
}

/**
 * @brief Read the circular burst-mode (rounding) pattern
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] roundingPattern The returned rounding pattern
 * @retval Error code
 */
int8_t ISDS_getRoundingPattern(WE_sensorInterface_t* sensorInterface, ISDS_roundingPattern_t *roundingPattern)
{
  ISDS_ctrl5_t ctrl5;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_5_REG, 1, (uint8_t *) &ctrl5))
  {
    return WE_FAIL;
  }

  *roundingPattern = (ISDS_roundingPattern_t) ctrl5.rounding;

  return WE_SUCCESS;
}


/* ISDS_CTRL_6_REG */

/**
 * @brief Set the gyroscope low-pass filter (LPF1) bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] bandwidth Low-pass filter bandwidth
 * @retval Error code
 */
int8_t ISDS_setGyroLowPassFilterBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_gyroLPF_t bandwidth)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  ctrl6.gyroLowPassFilterType = bandwidth;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6);
}

/**
 * @brief Read the gyroscope low-pass filter (LPF1) bandwidth
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] bandwidth The returned low-pass filter bandwidth
 * @retval Error code
 */
int8_t ISDS_getGyroLowPassFilterBandwidth(WE_sensorInterface_t* sensorInterface, ISDS_gyroLPF_t *bandwidth)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  *bandwidth = (ISDS_gyroLPF_t) ctrl6.gyroLowPassFilterType;

  return WE_SUCCESS;
}

/**
 * @brief Set the weight of the user offset words
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] offsetWeight Offset weight
 * @retval Error code
 */
int8_t ISDS_setOffsetWeight(WE_sensorInterface_t* sensorInterface, ISDS_state_t offsetWeight)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  ctrl6.userOffsetsWeight = offsetWeight;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6);
}

/**
 * @brief Read the weight of the user offset words
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] offsetWeight The returned offset weight
 * @retval Error code
 */
int8_t ISDS_getOffsetWeight(WE_sensorInterface_t* sensorInterface, ISDS_state_t *offsetWeight)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  *offsetWeight = (ISDS_state_t) ctrl6.userOffsetsWeight;

  return WE_SUCCESS;
}

/**
 * @brief Disable the accelerometer high performance mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] disable Accelerometer high performance mode disable state
 * @retval Error code
 */
int8_t ISDS_disableAccHighPerformanceMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t disable)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  ctrl6.accHighPerformanceModeDisable = disable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6);
}

/**
 * @brief Check if the accelerometer high performance mode is disabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] disable The returned accelerometer high performance mode disable state
 * @retval Error code
 */
int8_t ISDS_isAccHighPerformanceModeDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *disable)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  *disable = (ISDS_state_t) ctrl6.accHighPerformanceModeDisable;

  return WE_SUCCESS;
}

/**
 * @brief Set the data enable (DEN) trigger mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] triggerMode Data enable (DEN) trigger mode
 * @retval Error code
 */
int8_t ISDS_setDataEnableTriggerMode(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableTriggerMode_t triggerMode)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  ctrl6.dataEnableTriggerMode = triggerMode;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6);
}

/**
 * @brief Read the data enable (DEN) trigger mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] triggerMode The returned data enable (DEN) trigger mode
 * @retval Error code
 */
int8_t ISDS_getDataEnableTriggerMode(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableTriggerMode_t *triggerMode)
{
  ISDS_ctrl6_t ctrl6;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_6_REG, 1, (uint8_t *) &ctrl6))
  {
    return WE_FAIL;
  }

  *triggerMode = (ISDS_dataEnableTriggerMode_t) ctrl6.dataEnableTriggerMode;

  return WE_SUCCESS;
}


/* ISDS_CTRL_7_REG */

/**
 * @brief Enable/disable the source register rounding function
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] rounding Rounding enable state
 * @retval Error code
 */
int8_t ISDS_enableRounding(WE_sensorInterface_t* sensorInterface, ISDS_state_t rounding)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  ctrl7.enRounding = rounding;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7);
}

/**
 * @brief Check if the source register rounding function is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] rounding The returned rounding enable state
 * @retval Error code
 */
int8_t ISDS_isRoundingEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *rounding)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  *rounding = (ISDS_state_t) ctrl7.enRounding;

  return WE_SUCCESS;
}

/**
 * @brief Set the gyroscope digital high pass filter cutoff
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] cutoff Gyroscope digital high pass filter cutoff
 * @retval Error code
 */
int8_t ISDS_setGyroDigitalHighPassCutoff(WE_sensorInterface_t* sensorInterface, ISDS_gyroDigitalHighPassCutoff_t cutoff)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  ctrl7.gyroDigitalHighPassCutoff = cutoff;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7);
}

/**
 * @brief Read the gyroscope digital high pass filter cutoff
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] cutoff The returned gyroscope digital high pass filter cutoff
 * @retval Error code
 */
int8_t ISDS_getGyroDigitalHighPassCutoff(WE_sensorInterface_t* sensorInterface, ISDS_gyroDigitalHighPassCutoff_t *cutoff)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  *cutoff = (ISDS_gyroDigitalHighPassCutoff_t) ctrl7.gyroDigitalHighPassCutoff;

  return WE_SUCCESS;
}

/**
 * @brief Enable the gyroscope digital high pass filter
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] highPass Gyroscope digital high pass filter enable state
 * @retval Error code
 */
int8_t ISDS_enableGyroDigitalHighPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t highPass)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  ctrl7.gyroDigitalHighPassEnable = highPass;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7);
}

/**
 * @brief Check if the gyroscope digital high pass filter is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] highPass The returned gyroscope digital high pass filter enable state
 * @retval Error code
 */
int8_t ISDS_isGyroDigitalHighPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *highPass)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  *highPass = (ISDS_state_t) ctrl7.gyroDigitalHighPassEnable;

  return WE_SUCCESS;
}

/**
 * @brief Disable the gyroscope high performance mode
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] disable Gyroscope high performance mode disable state
 * @retval Error code
 */
int8_t ISDS_disableGyroHighPerformanceMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t disable)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  ctrl7.gyroHighPerformanceModeDisable = disable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7);
}

/**
 * @brief Check if the gyroscope high performance mode is disabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] disable The returned gyroscope high performance mode disable state
 * @retval Error code
 */
int8_t ISDS_isGyroHighPerformanceModeDisabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *disable)
{
  ISDS_ctrl7_t ctrl7;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_7_REG, 1, (uint8_t *) &ctrl7))
  {
    return WE_FAIL;
  }

  *disable = (ISDS_state_t) ctrl7.gyroHighPerformanceModeDisable;

  return WE_SUCCESS;
}


/* ISDS_CTRL_8_REG */

/**
 * @brief Enable/disable the low pass filter for 6D orientation detection
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] lowPass Low pass filter enable state
 * @retval Error code
 */
int8_t ISDS_enable6dLowPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t lowPass)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.en6dLowPass = lowPass;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Check if the low pass filter for 6D orientation detection is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] lowPass The returned low pass filter enable state
 * @retval Error code
 */
int8_t ISDS_is6dLowPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lowPass)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *lowPass = (ISDS_state_t) ctrl8.en6dLowPass;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the accelerometer high pass / slope filter
 * (i.e. the high-pass path of the composite filter block).
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] filterEnable HP / slope filter enable state (0: select low-pass path; 1: select high-pass path)
 * @retval Error code
 */
int8_t ISDS_enableAccHighPassSlopeFilter(WE_sensorInterface_t* sensorInterface, ISDS_state_t filterEnable)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.enAccHighPassSlopeFilter = filterEnable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Check if the accelerometer slope filter / high pass filter is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] filterEnable The returned filter enable state
 * @retval Error code
 */
int8_t ISDS_isAccHighPassSlopeFilterEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *filterEnable)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *filterEnable = (ISDS_state_t) ctrl8.enAccHighPassSlopeFilter;

  return WE_SUCCESS;
}

/**
 * @brief Set composite filter input
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] inputCompositeFilter Composite filter input
 * @retval Error code
 */
int8_t ISDS_setInputCompositeFilter(WE_sensorInterface_t* sensorInterface, ISDS_inputCompositeFilter_t inputCompositeFilter)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.inputComposite = inputCompositeFilter;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Read composite filter input
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] inputCompositeFilter The returned composite filter input
 * @retval Error code
 */
int8_t ISDS_getInputCompositeFilter(WE_sensorInterface_t* sensorInterface, ISDS_inputCompositeFilter_t *inputCompositeFilter)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *inputCompositeFilter = (ISDS_inputCompositeFilter_t) ctrl8.inputComposite;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable high pass filter reference mode
 *
 * Note that for reference mode to be enabled, it is also required to call
 * ISDS_enableAccHighPassSlopeFilter(ISDS_enable) and call
 * ISDS_setAccFilterConfig() with a non-zero argument.
 *
 * The first accelerometer output sample after enabling reference mode has to be discarded.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] refMode High pass filter reference mode enable state
 * @retval Error code
 */
int8_t ISDS_enableHighPassFilterRefMode(WE_sensorInterface_t* sensorInterface, ISDS_state_t refMode)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.highPassFilterRefMode = refMode;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Check if the high pass filter reference mode is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] refMode The returned high pass filter reference mode enable state
 * @retval Error code
 */
int8_t ISDS_isHighPassFilterRefModeEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *refMode)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *refMode = (ISDS_state_t) ctrl8.highPassFilterRefMode;

  return WE_SUCCESS;
}

/**
 * @brief Set accelerometer LPF2 and high pass filter configuration and cutoff setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] filterConfig Filter configuration
 * @retval Error code
 */
int8_t ISDS_setAccFilterConfig(WE_sensorInterface_t* sensorInterface, ISDS_accFilterConfig_t filterConfig)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.accFilterConfig = filterConfig;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Read the accelerometer LPF2 and high pass filter configuration and cutoff setting
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] filterConfig The returned filter configuration
 * @retval Error code
 */
int8_t ISDS_getAccFilterConfig(WE_sensorInterface_t* sensorInterface, ISDS_accFilterConfig_t *filterConfig)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *filterConfig = (ISDS_accFilterConfig_t) ctrl8.accFilterConfig;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the accelerometer low pass filter (LPF2)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] lowPass Filter enable state
 * @retval Error code
 */
int8_t ISDS_enableAccLowPass(WE_sensorInterface_t* sensorInterface, ISDS_state_t lowPass)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  ctrl8.enAccLowPass = lowPass;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8);
}

/**
 * @brief Check if the accelerometer low pass filter (LPF2) is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] lowPass The returned filter enable state
 * @retval Error code
 */
int8_t ISDS_isAccLowPassEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lowPass)
{
  ISDS_ctrl8_t ctrl8;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_8_REG, 1, (uint8_t *) &ctrl8))
  {
    return WE_FAIL;
  }

  *lowPass = (ISDS_state_t) ctrl8.enAccLowPass;

  return WE_SUCCESS;
}


/* ISDS_CTRL_9_REG */

/**
 * @brief Set the data enable (DEN) stamping sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] sensor Data enable (DEN) stamping sensor
 * @retval Error code
 */
int8_t ISDS_setDataEnableStampingSensor(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableStampingSensor_t sensor)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  ctrl9.dataEnableStampingSensor = sensor;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9);
}

/**
 * @brief Get the data enable (DEN) stamping sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] sensor The returned data enable (DEN) stamping sensor
 * @retval Error code
 */
int8_t ISDS_getDataEnableStampingSensor(WE_sensorInterface_t* sensorInterface, ISDS_dataEnableStampingSensor_t *sensor)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  *sensor = (ISDS_dataEnableStampingSensor_t) ctrl9.dataEnableStampingSensor;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable storage of data enable (DEN) value in LSB of Z-axis
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] enable Storage of DEN value in LSB of Z-axis enable state
 * @retval Error code
 */
int8_t ISDS_storeDataEnableValueInZAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  ctrl9.dataEnableValueZ = enable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9);
}

/**
 * @brief Check if storage of data enable (DEN) value in LSB of Z-axis is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] enable The returned storage of DEN value in LSB of Z-axis enable state
 * @retval Error code
 */
int8_t ISDS_isStoreDataEnableValueInZAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  *enable = (ISDS_state_t) ctrl9.dataEnableValueZ;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable storage of data enable (DEN) value in LSB of Y-axis
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] enable Storage of DEN value in LSB of Y-axis enable state
 * @retval Error code
 */
int8_t ISDS_storeDataEnableValueInYAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  ctrl9.dataEnableValueY = enable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9);
}

/**
 * @brief Check if storage of data enable (DEN) value in LSB of Y-axis is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] enable The returned storage of DEN value in LSB of Y-axis enable state
 * @retval Error code
 */
int8_t ISDS_isStoreDataEnableValueInYAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  *enable = (ISDS_state_t) ctrl9.dataEnableValueY;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable storage of data enable (DEN) value in LSB of X-axis
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] enable Storage of DEN value in LSB of X-axis enable state
 * @retval Error code
 */
int8_t ISDS_storeDataEnableValueInXAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  ctrl9.dataEnableValueX = enable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9);
}

/**
 * @brief Check if storage of data enable (DEN) value in LSB of X-axis is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] enable The returned storage of DEN value in LSB of X-axis enable state
 * @retval Error code
 */
int8_t ISDS_isStoreDataEnableValueInXAxisLSB(WE_sensorInterface_t* sensorInterface, ISDS_state_t *enable)
{
  ISDS_ctrl9_t ctrl9;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_9_REG, 1, (uint8_t *) &ctrl9))
  {
    return WE_FAIL;
  }

  *enable = (ISDS_state_t) ctrl9.dataEnableValueX;

  return WE_SUCCESS;
}


/* ISDS_CTRL_10_REG */

/**
 * @brief Enable/disable embedded functionalities (tilt)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] embeddedFuncEnable Embedded functionalities enable state
 * @retval Error code
 */
int8_t ISDS_enableEmbeddedFunctionalities(WE_sensorInterface_t* sensorInterface, ISDS_state_t embeddedFuncEnable)
{
  ISDS_ctrl10_t ctrl10;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10))
  {
    return WE_FAIL;
  }

  ctrl10.enEmbeddedFunc = embeddedFuncEnable;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10);
}

/**
 * @brief Check if embedded functionalities (tilt) are enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] embeddedFuncEnable The returned embedded functionalities enable state
 * @retval Error code
 */
int8_t ISDS_areEmbeddedFunctionalitiesEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *embeddedFuncEnable)
{
  ISDS_ctrl10_t ctrl10;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10))
  {
    return WE_FAIL;
  }

  *embeddedFuncEnable = (ISDS_state_t) ctrl10.enEmbeddedFunc;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable tilt calculation
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] tiltCalc Tilt calculation enable state
 * @retval Error code
 */
int8_t ISDS_enableTiltCalculation(WE_sensorInterface_t* sensorInterface, ISDS_state_t tiltCalc)
{
  ISDS_ctrl10_t ctrl10;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10))
  {
    return WE_FAIL;
  }

  ctrl10.enTiltCalculation = tiltCalc;

  return ISDS_WriteReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10);
}

/**
 * @brief Check if tilt calculation is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tiltCalc The returned tilt calculation enable state
 * @retval Error code
 */
int8_t ISDS_isTiltCalculationEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tiltCalc)
{
  ISDS_ctrl10_t ctrl10;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_CTRL_10_REG, 1, (uint8_t *) &ctrl10))
  {
    return WE_FAIL;
  }

  *tiltCalc = (ISDS_state_t) ctrl10.enTiltCalculation;

  return WE_SUCCESS;
}

/* ISDS_WAKE_UP_EVENT_REG */

/**
 * @brief Read the overall wake-up event status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned wake-up event status
 * @retval Error code
 */
int8_t ISDS_getWakeUpEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_wakeUpEvent_t *status)
{
  return ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) status);
}

/**
 * @brief Read the wake-up event detection status on axis X
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] wakeUpX The returned wake-up event detection status on axis X
 * @retval Error code
 */
int8_t ISDS_isWakeUpXEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpX)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *wakeUpX = (ISDS_state_t) status.wakeUpX;

  return WE_SUCCESS;
}

/**
 * @brief Read the wake-up event detection status on axis Y
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] wakeUpY The returned wake-up event detection status on axis Y
 * @retval Error code
 */
int8_t ISDS_isWakeUpYEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpY)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *wakeUpY = (ISDS_state_t) status.wakeUpY;

  return WE_SUCCESS;
}

/**
 * @brief Read the wake-up event detection status on axis Z
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] wakeUpZ The returned wake-up event detection status on axis Z
 * @retval Error code
 */
int8_t ISDS_isWakeUpZEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpZ)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *wakeUpZ = (ISDS_state_t) status.wakeUpZ;

  return WE_SUCCESS;
}

/**
 * @brief Read the wake-up event detection status (wake-up event on any axis)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] wakeUpState The returned wake-up event detection state
 * @retval Error code
 */
int8_t ISDS_isWakeUpEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *wakeUpState)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *wakeUpState = (ISDS_state_t) status.wakeUpState;

  return WE_SUCCESS;
}

/**
 * @brief Read the sleep state [not sleeping/sleeping]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] sleepState The returned sleep state.
 * @retval Error code
 */
int8_t ISDS_getSleepState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *sleepState)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *sleepState = (ISDS_state_t) status.sleepState;

  return WE_SUCCESS;
}

/**
 * @brief Read the free-fall event detection status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] freeFall The returned free-fall event detection state
 * @retval Error code
 */
int8_t ISDS_isFreeFallEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *freeFall)
{
  ISDS_wakeUpEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *freeFall = (ISDS_state_t) status.freeFallState;

  return WE_SUCCESS;
}


/* ISDS_TAP_EVENT_REG */

/**
 * @brief Read the overall tap event status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned tap event status
 * @retval Error code
 */
int8_t ISDS_getTapEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_tapEvent_t *status)
{
  return ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) status);
}

/**
 * @brief Read the tap event status (tap event on any axis)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapEventState The returned tap event state
 * @retval Error code
 */
int8_t ISDS_isTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapEventState)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *tapEventState = (ISDS_state_t) status.tapEventState;

  return WE_SUCCESS;
}

/**
 * @brief Read the tap event status on axis X
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapXAxis The returned tap event status on axis X
 * @retval Error code
 */
int8_t ISDS_isTapEventXAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapXAxis)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *tapXAxis = (ISDS_state_t) status.tapXAxis;

  return WE_SUCCESS;
}

/**
 * @brief Read the tap event status on axis Y
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapYAxis The returned tap event status on axis Y
 * @retval Error code
 */
int8_t ISDS_isTapEventYAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapYAxis)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *tapYAxis = (ISDS_state_t) status.tapYAxis;

  return WE_SUCCESS;
}

/**
 * @brief Read the tap event status on axis Z
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapZAxis The returned tap event status on axis Z
 * @retval Error code
 */
int8_t ISDS_isTapEventZAxis(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapZAxis)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *tapZAxis = (ISDS_state_t) status.tapZAxis;

  return WE_SUCCESS;
}

/**
 * @brief Read the double-tap event status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] doubleTap The returned double-tap event status
 * @retval Error code
 */
int8_t ISDS_isDoubleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *doubleTap)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *doubleTap = (ISDS_state_t) status.doubleState;

  return WE_SUCCESS;
}

/**
 * @brief Read the single-tap event status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] singleTap The returned single-tap event status
 * @retval Error code
 */
int8_t ISDS_isSingleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *singleTap)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *singleTap = (ISDS_state_t) status.singleState;

  return WE_SUCCESS;
}

/**
 * @brief Read the tap event acceleration sign (direction of tap event)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapSign The returned tap event acceleration sign
 * @retval Error code
 */
int8_t ISDS_getTapSign(WE_sensorInterface_t* sensorInterface, ISDS_tapSign_t *tapSign)
{
  ISDS_tapEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *tapSign = (ISDS_tapSign_t) status.tapSign;

  return WE_SUCCESS;
}


/* ISDS_6D_EVENT_REG */

/**
 * @brief Read register containing info on 6D orientation change event
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned 6D event status
 * @retval Error code
 */
int8_t ISDS_get6dEventRegister(WE_sensorInterface_t* sensorInterface, ISDS_6dEvent_t *status)
{
  return ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) status);
}

/**
 * @brief Check if 6D orientation change event has occurred
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] orientationChanged The returned 6D orientation change event status
 * @retval Error code
 */
int8_t ISDS_has6dOrientationChanged(WE_sensorInterface_t* sensorInterface, ISDS_state_t *orientationChanged)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *orientationChanged = (ISDS_state_t) status.sixDChange;

  return WE_SUCCESS;
}

/**
 * @brief Read the XL over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xlOverThreshold The returned XL over threshold state
 * @retval Error code
 */
int8_t ISDS_isXLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *xlOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *xlOverThreshold = (ISDS_state_t) status.xlOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the XH over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xhOverThreshold The returned XH over threshold state
 * @retval Error code
 */
int8_t ISDS_isXHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *xhOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *xhOverThreshold = (ISDS_state_t) status.xhOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the YL over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] ylOverThreshold The returned YL over threshold state
 * @retval Error code
 */
int8_t ISDS_isYLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *ylOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *ylOverThreshold = (ISDS_state_t) status.ylOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the YH over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yhOverThreshold The returned YH over threshold state
 * @retval Error code
 */
int8_t ISDS_isYHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *yhOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *yhOverThreshold = (ISDS_state_t) status.yhOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the ZL over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zlOverThreshold The returned ZL over threshold state
 * @retval Error code
 */
int8_t ISDS_isZLOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *zlOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *zlOverThreshold = (ISDS_state_t) status.zlOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the ZH over threshold state (6D orientation)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zhOverThreshold The returned ZH over threshold state
 * @retval Error code
 */
int8_t ISDS_isZHOverThreshold(WE_sensorInterface_t* sensorInterface, ISDS_state_t *zhOverThreshold)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *zhOverThreshold = (ISDS_state_t) status.zhOverThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Read the data enable (DEN) data ready signal.
 * It is set high when data output is related to the data coming from a DEN active condition.
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReady The returned DEN data ready signal state
 * @retval Error code
 */
int8_t ISDS_isDataEnableDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady)
{
  ISDS_6dEvent_t status;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_6D_EVENT_REG, 1, (uint8_t *) &status))
  {
    return WE_FAIL;
  }

  *dataReady = (ISDS_state_t) status.dataEnableDataReady;

  return WE_SUCCESS;
}


/* ISDS_STATUS_REG */

/**
 * @brief Get overall sensor status
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned sensor event data
 * @retval Error code
 */
int8_t ISDS_getStatusRegister(WE_sensorInterface_t* sensorInterface, ISDS_status_t *status)
{
  return ISDS_ReadReg(sensorInterface, ISDS_STATUS_REG, 1, (uint8_t *) status);
}

/**
 * @brief Check if new acceleration samples are available
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReady The returned data-ready state
 * @retval Error code
 */
int8_t ISDS_isAccelerationDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady)
{
  ISDS_status_t statusRegister;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_STATUS_REG, 1, (uint8_t *) &statusRegister))
  {
    return WE_FAIL;
  }

  *dataReady = (ISDS_state_t) statusRegister.accDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Check if new gyroscope samples are available
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReady The returned data-ready state
 * @retval Error code
 */
int8_t ISDS_isGyroscopeDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady)
{
  ISDS_status_t statusRegister;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_STATUS_REG, 1, (uint8_t *) &statusRegister))
  {
    return WE_FAIL;
  }

  *dataReady = (ISDS_state_t) statusRegister.gyroDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Check if new temperature samples are available
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] dataReady The returned data-ready state
 * @retval Error code
 */
int8_t ISDS_isTemperatureDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *dataReady)
{
  ISDS_status_t statusRegister;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_STATUS_REG, 1, (uint8_t *) &statusRegister))
  {
    return WE_FAIL;
  }

  *dataReady = (ISDS_state_t) statusRegister.tempDataReady;

  return WE_SUCCESS;
}

/**
 * @brief Check if new temperature, gyroscope and acceleration samples are available
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] temp_state The returned data-ready state for the temperature
 * @param[out] acc_state The returned data-ready state for the acceleration
 * @param[out] gyro_state The returned data-ready state for the gyroscope
 * @retval Error code
 */
int8_t ISDS_isDataReady(WE_sensorInterface_t* sensorInterface, ISDS_state_t *temp_state, ISDS_state_t *acc_state, ISDS_state_t *gyro_state)
{
  ISDS_status_t statusRegister;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_STATUS_REG, 1, (uint8_t *) &statusRegister))
  {
    return WE_FAIL;
  }

  if(temp_state != NULL)
  {
	  *temp_state = (ISDS_state_t) statusRegister.tempDataReady;
  }
  if(acc_state != NULL)
  {
	  *acc_state = (ISDS_state_t) statusRegister.accDataReady;
  }
  if(gyro_state != NULL)
  {
	  *gyro_state = (ISDS_state_t) statusRegister.gyroDataReady;
  }

  return WE_SUCCESS;
}


/* ISDS_FIFO_STATUS_1_REG */
/* ISDS_FIFO_STATUS_2_REG */
/* ISDS_FIFO_STATUS_3_REG */
/* ISDS_FIFO_STATUS_4_REG */

/**
 * @brief Get overall FIFO status
 *
 * This is a convenience function querying all FIFO status registers in one read operation.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned FIFO status flags register
 * @param[out] fillLevel The returned FIFO fill level (0-2047)
 * @param[out] fifoPattern Word of recursive pattern read at the next read
 * @retval Error code
 */
int8_t ISDS_getFifoStatus(WE_sensorInterface_t* sensorInterface, ISDS_fifoStatus2_t *status, uint16_t *fillLevel, uint16_t *fifoPattern)
{
  uint8_t tmp[4];
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_1_REG, 4, tmp))
  {
    return WE_FAIL;
  }

  ISDS_fifoStatus1_t *fifoStatus1 = (ISDS_fifoStatus1_t *) tmp;
  ISDS_fifoStatus2_t *fifoStatus2 = (ISDS_fifoStatus2_t *) tmp + 1;
  ISDS_fifoStatus3_t *fifoStatus3 = (ISDS_fifoStatus3_t *) tmp + 2;
  ISDS_fifoStatus4_t *fifoStatus4 = (ISDS_fifoStatus4_t *) tmp + 3;

  *status = *(ISDS_fifoStatus2_t*) fifoStatus2;

  *fillLevel = ((uint16_t) fifoStatus1->fifoFillLevelLsb) |
               (((uint16_t) (fifoStatus2->fifoFillLevelMsb & 0x07)) << 8);

  *fifoPattern = ((uint16_t) fifoStatus3->fifoPatternLsb) |
                 (((uint16_t) (fifoStatus4->fifoPatternMsb & 0x03)) << 8);

  return WE_SUCCESS;
}

/**
 * @brief Get FIFO status flags register
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] status The returned FIFO status flags register
 * @retval Error code
 */
int8_t ISDS_getFifoStatus2Register(WE_sensorInterface_t* sensorInterface, ISDS_fifoStatus2_t *status)
{
  return ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_2_REG, 1, (uint8_t *) status);
}

/**
 * @brief Read the FIFO fill level
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fillLevel The returned FIFO fill level (0-2047)
 * @retval Error code
 */
int8_t ISDS_getFifoFillLevel(WE_sensorInterface_t* sensorInterface, uint16_t *fillLevel)
{
  uint8_t tmp[2];
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_1_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  ISDS_fifoStatus1_t *fifoStatus1 = (ISDS_fifoStatus1_t *) tmp;
  ISDS_fifoStatus2_t *fifoStatus2 = (ISDS_fifoStatus2_t *) tmp + 1;

  *fillLevel = ((uint16_t) fifoStatus1->fifoFillLevelLsb) |
               (((uint16_t) (fifoStatus2->fifoFillLevelMsb & 0x07)) << 8);

  return WE_SUCCESS;
}

/**
 * @brief Check if the FIFO is empty
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] empty FIFO empty state
 * @retval Error code
 */
int8_t ISDS_isFifoEmpty(WE_sensorInterface_t* sensorInterface, ISDS_state_t *empty)
{
  ISDS_fifoStatus2_t fifoStatus2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_2_REG, 1, (uint8_t *) &fifoStatus2))
  {
    return WE_FAIL;
  }

  *empty = (ISDS_state_t) fifoStatus2.fifoEmptyState;

  return WE_SUCCESS;
}

/**
 * @brief Check if the FIFO is full
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] full FIFO full state
 * @retval Error code
 */
int8_t ISDS_isFifoFull(WE_sensorInterface_t* sensorInterface, ISDS_state_t *full)
{
  ISDS_fifoStatus2_t fifoStatus2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_2_REG, 1, (uint8_t *) &fifoStatus2))
  {
    return WE_FAIL;
  }

  *full = (ISDS_state_t) fifoStatus2.fifoFullSmartState;

  return WE_SUCCESS;
}

/**
 * @brief Check if a FIFO overrun has occurred
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] overrun FIFO overrun state
 * @retval Error code
 */
int8_t ISDS_getFifoOverrunState(WE_sensorInterface_t* sensorInterface, ISDS_state_t *overrun)
{
  ISDS_fifoStatus2_t fifoStatus2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_2_REG, 1, (uint8_t *) &fifoStatus2))
  {
    return WE_FAIL;
  }

  *overrun = (ISDS_state_t) fifoStatus2.fifoOverrunState;

  return WE_SUCCESS;
}

/**
 * @brief Check if the FIFO fill threshold has been reached
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] threshReached FIFO threshold status bit
 * @retval Error code
 */
int8_t ISDS_isFifoThresholdReached(WE_sensorInterface_t* sensorInterface, ISDS_state_t *threshReached)
{
  ISDS_fifoStatus2_t fifoStatus2;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_2_REG, 1, (uint8_t *) &fifoStatus2))
  {
    return WE_FAIL;
  }

  *threshReached = (ISDS_state_t) fifoStatus2.fifoThresholdState;

  return WE_SUCCESS;
}

/**
 * @brief Get the word of recursive pattern read at the next read
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] fifoPattern Word of recursive pattern read at the next read
 * @retval Error code
 */
int8_t ISDS_getFifoPattern(WE_sensorInterface_t* sensorInterface, uint16_t *fifoPattern)
{
  uint8_t tmp[2];
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FIFO_STATUS_3_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  ISDS_fifoStatus3_t *fifoStatus3 = (ISDS_fifoStatus3_t *) tmp;
  ISDS_fifoStatus4_t *fifoStatus4 = (ISDS_fifoStatus4_t *) tmp + 1;

  *fifoPattern = ((uint16_t) fifoStatus3->fifoPatternLsb) |
                 (((uint16_t) (fifoStatus4->fifoPatternMsb & 0x03)) << 8);

  return WE_SUCCESS;
}


/* ISDS_FUNC_SRC_1_REG */

/**
 * @brief Check if a tilt event has occurred
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tiltEvent Tilt event state
 * @retval Error code
 */
int8_t ISDS_isTiltEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tiltEvent)
{
  ISDS_funcSrc1_t funcSrc1;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FUNC_SRC_1_REG, 1, (uint8_t *) &funcSrc1))
  {
    return WE_FAIL;
  }

  *tiltEvent = (ISDS_state_t) funcSrc1.tiltState;

  return WE_SUCCESS;
}


/* ISDS_TAP_CFG_REG */

/**
 * @brief Enable/disable latched interrupts
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] lir Latched interrupts state
 * @retval Error code
 */
int8_t ISDS_enableLatchedInterrupt(WE_sensorInterface_t* sensorInterface, ISDS_state_t lir)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.latchedInterrupt = lir;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Read the latched interrupts state [enabled, disabled]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] lir The returned latched interrupts state
 * @retval Error code
 */
int8_t ISDS_isLatchedInterruptEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *lir)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *lir = (ISDS_state_t) tapCfg.latchedInterrupt;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable tap recognition in X direction
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] tapX Tap X direction state
 * @retval Error code
 */
int8_t ISDS_enableTapX(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapX)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.enTapX = tapX;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Check if detection of tap events in X direction is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapX The returned tap X direction state
 * @retval Error code
 */
int8_t ISDS_isTapXEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapX)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *tapX = (ISDS_state_t) tapCfg.enTapX;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable tap recognition in Y direction
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] tapY Tap Y direction state
 * @retval Error code
 */
int8_t ISDS_enableTapY(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapY)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.enTapY = tapY;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Check if detection of tap events in Y direction is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapY The returned tap Y direction state
 * @retval Error code
 */
int8_t ISDS_isTapYEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapY)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *tapY = (ISDS_state_t) tapCfg.enTapY;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable tap recognition in Z direction
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] tapZ Tap Z direction state
 * @retval Error code
 */
int8_t ISDS_enableTapZ(WE_sensorInterface_t* sensorInterface, ISDS_state_t tapZ)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.enTapZ = tapZ;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Check if detection of tap events in Z direction is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapZ The returned tap Z direction state
 * @retval Error code
 */
int8_t ISDS_isTapZEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *tapZ)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *tapZ = (ISDS_state_t) tapCfg.enTapZ;

  return WE_SUCCESS;
}

/**
 * @brief Set activity filter (HPF or SLOPE filter selection on wake-up and activity/inactivity functions)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] filter Activity filter
 * @retval Error code
 */
int8_t ISDS_setActivityFilter(WE_sensorInterface_t* sensorInterface, ISDS_activityFilter_t filter)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.filterSelection = filter;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Read activity filter (HPF or SLOPE filter selection on wake-up and activity/inactivity functions)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] filter The returned activity filter
 * @retval Error code
 */
int8_t ISDS_getActivityFilter(WE_sensorInterface_t* sensorInterface, ISDS_activityFilter_t *filter)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *filter = (ISDS_activityFilter_t) tapCfg.filterSelection;

  return WE_SUCCESS;
}

/**
 * @brief Set inactivity function
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] function Inactivity function
 * @retval Error code
 */
int8_t ISDS_setInactivityFunction(WE_sensorInterface_t* sensorInterface, ISDS_inactivityFunction_t function)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.enInactivity = function;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Read inactivity function
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] function The returned inactivity function
 * @retval Error code
 */
int8_t ISDS_getInactivityFunction(WE_sensorInterface_t* sensorInterface, ISDS_inactivityFunction_t *function)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *function = (ISDS_inactivityFunction_t) tapCfg.enInactivity;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable interrupts
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] interruptsEnable Interrupts enable state
 * @retval Error code
 */
int8_t ISDS_enableInterrupts(WE_sensorInterface_t* sensorInterface, ISDS_state_t interruptsEnable)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  tapCfg.enInterrupts = interruptsEnable;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg);
}

/**
 * @brief Check if interrupts are enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] interruptsEnable The returned interrupts enable state.
 * @retval Error code
 */
int8_t ISDS_areInterruptsEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *interruptsEnable)
{
  ISDS_tapCfg_t tapCfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_CFG_REG, 1, (uint8_t *) &tapCfg))
  {
    return WE_FAIL;
  }

  *interruptsEnable = (ISDS_state_t) tapCfg.enInterrupts;

  return WE_SUCCESS;
}


/* ISDS_TAP_THS_6D_REG */

/**
 * @brief Set the tap threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] tapThreshold Tap threshold (5 bits)
 * @retval Error code
 */
int8_t ISDS_setTapThreshold(WE_sensorInterface_t* sensorInterface, uint8_t tapThreshold)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  tapThs6d.tapThreshold = (tapThreshold & 0x1F);

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d);
}

/**
 * @brief Read the tap threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] tapThreshold The returned tap threshold
 * @retval Error code
 */
int8_t ISDS_getTapThreshold(WE_sensorInterface_t* sensorInterface, uint8_t *tapThreshold)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  *tapThreshold = tapThs6d.tapThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Set the 6D orientation detection threshold (degrees)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] threshold6D 6D orientation detection threshold
 * @retval Error code
 */
int8_t ISDS_set6DThreshold(WE_sensorInterface_t* sensorInterface, ISDS_sixDThreshold_t threshold6D)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  tapThs6d.sixDThreshold = threshold6D;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d);
}

/**
 * @brief Read the 6D orientation detection threshold (degrees)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] threshold6D The returned 6D orientation detection threshold
 * @retval Error code
 */
int8_t ISDS_get6DThreshold(WE_sensorInterface_t* sensorInterface, ISDS_sixDThreshold_t *threshold6D)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  *threshold6D = (ISDS_sixDThreshold_t) tapThs6d.sixDThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable 4D orientation detection
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] detection4D The 4D orientation detection enable state
 * @retval Error code
 */
int8_t ISDS_enable4DDetection(WE_sensorInterface_t* sensorInterface, ISDS_state_t detection4D)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  tapThs6d.fourDDetectionEnabled = detection4D;

  return ISDS_WriteReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d);
}

/**
 * @brief Check if 4D orientation detection is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] detection4D The returned 4D orientation detection enable state
 * @retval Error code
 */
int8_t ISDS_is4DDetectionEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *detection4D)
{
  ISDS_tapThs6d_t tapThs6d;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_TAP_THS_6D_REG, 1, (uint8_t *) &tapThs6d))
  {
    return WE_FAIL;
  }

  *detection4D = (ISDS_state_t) tapThs6d.fourDDetectionEnabled;

  return WE_SUCCESS;
}


/* ISDS_INT_DUR2_REG */

/**
 * @brief Set the maximum duration time gap for double-tap recognition (LATENCY)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] latencyTime Latency value (4 bits)
 * @retval Error code
 */
int8_t ISDS_setTapLatencyTime(WE_sensorInterface_t* sensorInterface, uint8_t latencyTime)
{
  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  intDuration.latency = (latencyTime & 0x0F);

  return ISDS_WriteReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration);
}

/**
 * @brief Read the maximum duration time gap for double-tap recognition (LATENCY)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] latencyTime The returned latency time
 * @retval Error code
 */
int8_t ISDS_getTapLatencyTime(WE_sensorInterface_t* sensorInterface, uint8_t *latencyTime)
{
  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  *latencyTime = intDuration.latency;

  return WE_SUCCESS;
}

/**
 * @brief Set the expected quiet time after a tap detection (QUIET)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] quietTime Quiet time value (2 bits)
 * @retval Error code
 */
int8_t ISDS_setTapQuietTime(WE_sensorInterface_t* sensorInterface, uint8_t quietTime)
{
  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  intDuration.quiet = (quietTime & 0x03);

  return ISDS_WriteReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration);
}

/**
 * @brief Read the expected quiet time after a tap detection (QUIET)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] quietTime The returned quiet time
 * @retval Error code
 */
int8_t ISDS_getTapQuietTime(WE_sensorInterface_t* sensorInterface, uint8_t *quietTime)
{

  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  *quietTime = intDuration.quiet;

  return WE_SUCCESS;
}

/**
 * @brief Set the maximum duration of over-threshold events (SHOCK)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] shockTime Shock time value (2 bits)
 * @retval Error code
 */
int8_t ISDS_setTapShockTime(WE_sensorInterface_t* sensorInterface, uint8_t shockTime)
{
  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  intDuration.shock = (shockTime & 0x03);

  return ISDS_WriteReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration);
}

/**
 * @brief Read the maximum duration of over-threshold events (SHOCK)
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] shockTime The returned shock time.
 * @retval Error code
 */
int8_t ISDS_getTapShockTime(WE_sensorInterface_t* sensorInterface, uint8_t *shockTime)
{
  ISDS_intDur2_t intDuration;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_INT_DUR2_REG, 1, (uint8_t *) &intDuration))
  {
    return WE_FAIL;
  }

  *shockTime = intDuration.shock;

  return WE_SUCCESS;
}


/* ISDS_WAKE_UP_THS_REG */

/**
 * @brief Set wake-up threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] thresh Wake-up threshold (six bits)
 * @retval Error code
 */
int8_t ISDS_setWakeUpThreshold(WE_sensorInterface_t* sensorInterface, uint8_t thresh)
{
  ISDS_wakeUpThs_t wakeUpThs;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs))
  {
    return WE_FAIL;
  }

  wakeUpThs.wakeUpThreshold = (thresh & 0x3F);

  return ISDS_WriteReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs);
}

/**
 * @brief Read the wake-up threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] thresh The returned wake-up threshold.
 * @retval Error code
 */
int8_t ISDS_getWakeUpThreshold(WE_sensorInterface_t* sensorInterface, uint8_t *thresh)
{
  ISDS_wakeUpThs_t wakeUpThs;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs))
  {
    return WE_FAIL;
  }

  *thresh = wakeUpThs.wakeUpThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the single and double-tap event OR only single-tap event
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] doubleTapEnable Tap event state [0: only single, 1: single and double-tap]
 * @retval Error code
 */
int8_t ISDS_enableDoubleTapEvent(WE_sensorInterface_t* sensorInterface, ISDS_state_t doubleTapEnable)
{
  ISDS_wakeUpThs_t wakeUpThs;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs))
  {
    return WE_FAIL;
  }

  wakeUpThs.enDoubleTapEvent = doubleTapEnable;

  return ISDS_WriteReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs);
}

/**
 * @brief Check if double-tap events are enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] doubleTap The returned tap event state [0: only single, 1: single and double-tap]
 * @retval Error code
 */
int8_t ISDS_isDoubleTapEventEnabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *doubleTapEnable)
{
  ISDS_wakeUpThs_t wakeUpThs;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_THS_REG, 1, (uint8_t *) &wakeUpThs))
  {
    return WE_FAIL;
  }

  *doubleTapEnable = (ISDS_state_t) wakeUpThs.enDoubleTapEvent;

  return WE_SUCCESS;
}


/* ISDS_WAKE_UP_DUR_REG */

/**
 * @brief Set the sleep mode duration
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] duration Sleep mode duration (4 bits)
 * @retval Error code
 */
int8_t ISDS_setSleepDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration)
{
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  wakeUpDur.sleepDuration = (duration & 0x0F);

  return ISDS_WriteReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur);
}

/**
 * @brief Read the sleep mode duration
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] duration The returned sleep mode duration
 * @retval Error code
 */
int8_t ISDS_getSleepDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration)
{
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  *duration = wakeUpDur.sleepDuration;

  return WE_SUCCESS;
}

/**
 * @brief Set wake-up duration
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] duration Wake-up duration (two bits)
 * @retval Error code
 */
int8_t ISDS_setWakeUpDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration)
{
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  wakeUpDur.wakeUpDuration = (duration & 0x03);

  return ISDS_WriteReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur);
}

/**
 * @brief Read the wake-up duration
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] duration The returned wake-up duration (two bits)
 * @retval Error code
 */
int8_t ISDS_getWakeUpDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration)
{
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  *duration = wakeUpDur.wakeUpDuration;

  return WE_SUCCESS;
}


/* ISDS_FREE_FALL_REG */

/**
 * @brief Set free-fall threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] thresh Free-fall threshold value
 * @retval Error code
 */
int8_t ISDS_setFreeFallThreshold(WE_sensorInterface_t* sensorInterface, ISDS_freeFallThreshold_t thresh)
{
  ISDS_freeFall_t freeFall;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall))
  {
    return WE_FAIL;
  }

  freeFall.freeFallThreshold = thresh;

  return ISDS_WriteReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall);
}

/**
 * @brief Read the free-fall threshold
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] thresh The returned free-fall threshold value
 * @retval Error code
 */
int8_t ISDS_getFreeFallThreshold(WE_sensorInterface_t* sensorInterface, ISDS_freeFallThreshold_t *thresh)
{
  ISDS_freeFall_t freeFall;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall))
  {
    return WE_FAIL;
  }

  *thresh = (ISDS_freeFallThreshold_t) freeFall.freeFallThreshold;

  return WE_SUCCESS;
}

/**
 * @brief Set the free-fall duration (both LSB and MSB).
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] duration Free-fall duration (6 bits)
 * @retval Error code
 */
int8_t ISDS_setFreeFallDuration(WE_sensorInterface_t* sensorInterface, uint8_t duration)
{
  ISDS_freeFall_t freeFall;
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall))
  {
    return WE_FAIL;
  }
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  freeFall.freeFallDurationLSB = (uint8_t) (duration & 0x1F);
  wakeUpDur.freeFallDurationMSB = (uint8_t) ((duration >> 5) & 0x01);

  if (WE_FAIL == ISDS_WriteReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall))
  {
    return WE_FAIL;
  }
  return ISDS_WriteReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur);
}

/**
 * @brief Read the free-fall duration (both LSB and MSB).
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] duration The returned free-fall duration (6 bits)
 * @retval Error code
 */
int8_t ISDS_getFreeFallDuration(WE_sensorInterface_t* sensorInterface, uint8_t *duration)
{
  ISDS_freeFall_t freeFall;
  ISDS_wakeUpDur_t wakeUpDur;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_FREE_FALL_REG, 1, (uint8_t *) &freeFall))
  {
    return WE_FAIL;
  }
  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_WAKE_UP_DUR_REG, 1, (uint8_t *) &wakeUpDur))
  {
    return WE_FAIL;
  }

  *duration = ((uint8_t) freeFall.freeFallDurationLSB) |
              (((uint8_t) (wakeUpDur.freeFallDurationMSB & 0x01)) << 5);

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the tilt event interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0Tilt Tilt event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableTiltINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0Tilt)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0Tilt = int0Tilt;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the tilt event interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0Tilt The returned tilt event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isTiltINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0Tilt)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0Tilt = (ISDS_state_t) mde1Cfg.int0Tilt;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the 6D orientation change event interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int06d 6D orientation change event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enable6dINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int06d)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int06d = int06d;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the 6D orientation change event interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int06d The returned 6D orientation change event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_is6dINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int06d)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int06d = (ISDS_state_t) mde1Cfg.int06d;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the double-tap interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0DoubleTap The double-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableDoubleTapINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0DoubleTap)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0DoubleTap = int0DoubleTap;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the double-tap interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0DoubleTap The returned double-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isDoubleTapINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0DoubleTap)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0DoubleTap = (ISDS_state_t) mde1Cfg.int0DoubleTap;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the free-fall interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0FreeFall Free-fall interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFreeFallINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0FreeFall)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0FreeFall = int0FreeFall;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the free-fall interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0FreeFall The returned free-fall enable state
 * @retval Error code
 */
int8_t ISDS_isFreeFallINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0FreeFall)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0FreeFall = (ISDS_state_t) mde1Cfg.int0FreeFall;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the wake-up interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0WakeUp Wake-up interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableWakeUpINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0WakeUp)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0WakeUp = int0WakeUp;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the wake-up interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0WakeUp The returned wake-up interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isWakeUpINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0WakeUp)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0WakeUp = (ISDS_state_t) mde1Cfg.int0WakeUp;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the single-tap interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0SingleTap Single-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableSingleTapINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0SingleTap)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0SingleTap = int0SingleTap;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the single-tap interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0SingleTap The returned single-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isSingleTapINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0SingleTap)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0SingleTap = (ISDS_state_t) mde1Cfg.int0SingleTap;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the inactivity state interrupt on INT_0
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int0InactivityState Inactivity state interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableInactivityStateINT0(WE_sensorInterface_t* sensorInterface, ISDS_state_t int0InactivityState)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  mde1Cfg.int0InactivityState = int0InactivityState;

  return ISDS_WriteReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg);
}

/**
 * @brief Check if the inactivity state interrupt on INT_0 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int0InactivityState The returned inactivity state interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isInactivityStateINT0Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int0InactivityState)
{
  ISDS_mde1Cfg_t mde1Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD1_CFG_REG, 1, (uint8_t *) &mde1Cfg))
  {
    return WE_FAIL;
  }

  *int0InactivityState = (ISDS_state_t) mde1Cfg.int0InactivityState;

  return WE_SUCCESS;
}


/* ISDS_MD2_CFG_REG */

/**
 * @brief Enable/disable the tilt event interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1Tilt Tilt event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableTiltINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1Tilt)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1Tilt = int1Tilt;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the tilt event interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1Tilt The returned tilt event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isTiltINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1Tilt)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1Tilt = (ISDS_state_t) mde2Cfg.int1Tilt;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the 6D orientation change event interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int16d 6D orientation change event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enable6dINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int16d)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int16d = int16d;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the 6D orientation change event interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int16d The returned 6D orientation change event interrupt enable state
 * @retval Error code
 */
int8_t ISDS_is6dINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int16d)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int16d = (ISDS_state_t) mde2Cfg.int16d;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the double-tap interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1DoubleTap The double-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableDoubleTapINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1DoubleTap)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1DoubleTap = int1DoubleTap;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the double-tap interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1DoubleTap The returned double-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isDoubleTapINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1DoubleTap)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1DoubleTap = (ISDS_state_t) mde2Cfg.int1DoubleTap;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the free-fall interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1FreeFall Free-fall interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableFreeFallINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1FreeFall)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1FreeFall = int1FreeFall;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the free-fall interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1FreeFall The returned free-fall enable state
 * @retval Error code
 */
int8_t ISDS_isFreeFallINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1FreeFall)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1FreeFall = (ISDS_state_t) mde2Cfg.int1FreeFall;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the wake-up interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1WakeUp Wake-up interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableWakeUpINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1WakeUp)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1WakeUp = int1WakeUp;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the wake-up interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1WakeUp The returned wake-up interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isWakeUpINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1WakeUp)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1WakeUp = (ISDS_state_t) mde2Cfg.int1WakeUp;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the single-tap interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1SingleTap Single-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableSingleTapINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1SingleTap)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1SingleTap = int1SingleTap;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the single-tap interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1SingleTap The returned single-tap interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isSingleTapINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1SingleTap)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1SingleTap = (ISDS_state_t) mde2Cfg.int1SingleTap;

  return WE_SUCCESS;
}

/**
 * @brief Enable/disable the inactivity state interrupt on INT_1
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] int1InactivityState Inactivity state interrupt enable state
 * @retval Error code
 */
int8_t ISDS_enableInactivityStateINT1(WE_sensorInterface_t* sensorInterface, ISDS_state_t int1InactivityState)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  mde2Cfg.int1InactivityState = int1InactivityState;

  return ISDS_WriteReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg);
}

/**
 * @brief Check if the inactivity state interrupt on INT_1 is enabled
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] int1InactivityState The returned inactivity state interrupt enable state
 * @retval Error code
 */
int8_t ISDS_isInactivityStateINT1Enabled(WE_sensorInterface_t* sensorInterface, ISDS_state_t *int1InactivityState)
{
  ISDS_mde2Cfg_t mde2Cfg;

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_MD2_CFG_REG, 1, (uint8_t *) &mde2Cfg))
  {
    return WE_FAIL;
  }

  *int1InactivityState = (ISDS_state_t) mde2Cfg.int1InactivityState;

  return WE_SUCCESS;
}

/* ISDS_X_OFS_USR_REG */
/* ISDS_Y_OFS_USR_REG */
/* ISDS_Z_OFS_USR_REG */

/**
 * @brief Set the user offset for axis X
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] offsetValueXAxis User offset for axis X
 * @retval Error code
 */
int8_t ISDS_setOffsetValueX(WE_sensorInterface_t* sensorInterface, int8_t offsetValueXAxis)
{
  return ISDS_WriteReg(sensorInterface, ISDS_X_OFS_USR_REG, 1, (uint8_t *) &offsetValueXAxis);
}

/**
 * @brief Read the user offset for axis X
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] offsetValueXAxis The returned user offset for axis X
 * @retval Error code
 */
int8_t ISDS_getOffsetValueX(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueXAxis)
{
  return ISDS_ReadReg(sensorInterface, ISDS_X_OFS_USR_REG, 1, (uint8_t *) offsetValueXAxis);
}

/**
 * @brief Set the user offset for axis Y
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] offsetValueYAxis User offset for axis Y
 * @retval Error code
 */
int8_t ISDS_setOffsetValueY(WE_sensorInterface_t* sensorInterface, int8_t offsetValueYAxis)
{
  return ISDS_WriteReg(sensorInterface, ISDS_Y_OFS_USR_REG, 1, (uint8_t *) &offsetValueYAxis);
}

/**
 * @brief Read the user offset for axis Y
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] offsetValueYAxis The returned user offset for axis Y
 * @retval Error code
 */
int8_t ISDS_getOffsetValueY(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueYAxis)
{
  return ISDS_ReadReg(sensorInterface, ISDS_Y_OFS_USR_REG, 1, (uint8_t *) offsetValueYAxis);
}

/**
 * @brief Set the user offset for axis Z
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] offsetValueZAxis The user offset for axis Z
 * @retval Error code
 */
int8_t ISDS_setOffsetValueZ(WE_sensorInterface_t* sensorInterface, int8_t offsetValueZAxis)
{
  return ISDS_WriteReg(sensorInterface, ISDS_Z_OFS_USR_REG, 1, (uint8_t *) &offsetValueZAxis);
}

/**
 * @brief Read the user offset for axis Z
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] offsetValueZAxis The returned user offset for axis Z
 * @retval Error code
 */
int8_t ISDS_getOffsetValueZ(WE_sensorInterface_t* sensorInterface, int8_t *offsetValueZAxis)
{
  return ISDS_ReadReg(sensorInterface, ISDS_Z_OFS_USR_REG, 1, (uint8_t *) offsetValueZAxis);
}


/* ISDS_FIFO_DATA_OUT_L_REG */
/* ISDS_FIFO_DATA_OUT_H_REG */

/**
 * @brief Reads the specified number of 16 bit values from the FIFO buffer
 *
 * The type of values read depends on the FIFO configuration and the current
 * read position. Get the current FIFO pattern value (e.g. via
 * ISDS_getFifoPattern() or ISDS_getFifoStatus()). Before reading FIFO data to
 * determine the type of the next value read.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] numSamples The number of values to be read
 * @param[out] fifoData The returned values (must provide pointer to buffer of length numSamples)
 * @retval Error code
 */
int8_t ISDS_getFifoData(WE_sensorInterface_t* sensorInterface, uint16_t numSamples, uint16_t *fifoData)
{
  return ISDS_ReadReg(sensorInterface, ISDS_FIFO_DATA_OUT_L_REG, numSamples * 2, (uint8_t *) fifoData);
}


#ifdef WE_USE_FLOAT
/**
 * @brief Reads the X-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRate X-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateX_float(WE_sensorInterface_t* sensorInterface, float *xRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateX(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *xRate = ISDS_convertAngularRate_float(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Reads the Y-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yRate Y-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateY_float(WE_sensorInterface_t* sensorInterface, float *yRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateY(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *yRate = ISDS_convertAngularRate_float(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Reads the Z-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zRate Z-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateZ_float(WE_sensorInterface_t* sensorInterface, float *zRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateZ(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *zRate = ISDS_convertAngularRate_float(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the gyroscope sensor output in [mdps] for all three axes
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRate The returned X-axis angular rate in [mdps]
 * @param[out] yRate The returned Y-axis angular rate in [mdps]
 * @param[out] zRate The returned Z-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRates_float(WE_sensorInterface_t* sensorInterface, float *xRate, float *yRate, float *zRate)
{
  int16_t xRawRate, yRawRate, zRawRate;
  if (WE_FAIL == ISDS_getRawAngularRates(sensorInterface, &xRawRate, &yRawRate, &zRawRate))
  {
    return WE_FAIL;
  }
  *xRate = ISDS_convertAngularRate_float(xRawRate, currentGyroFullScale);
  *yRate = ISDS_convertAngularRate_float(yRawRate, currentGyroFullScale);
  *zRate = ISDS_convertAngularRate_float(zRawRate, currentGyroFullScale);
  return WE_SUCCESS;
}
#endif /* WE_USE_FLOAT */

/**
 * @brief Reads the X-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRate X-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateX_int(WE_sensorInterface_t* sensorInterface, int32_t *xRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateX(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *xRate = ISDS_convertAngularRate_int(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Reads the Y-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yRate Y-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateY_int(WE_sensorInterface_t* sensorInterface, int32_t *yRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateY(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *yRate = ISDS_convertAngularRate_int(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Reads the Z-axis angular rate in [mdps]
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zRate Z-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRateZ_int(WE_sensorInterface_t* sensorInterface, int32_t *zRate)
{
  int16_t rawRate;
  if (WE_FAIL == ISDS_getRawAngularRateZ(sensorInterface, &rawRate))
  {
    return WE_FAIL;
  }
  *zRate = ISDS_convertAngularRate_int(rawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the gyroscope sensor output in [mdps] for all three axes
 *
 * Note that this functions relies on the current gyroscope full scale value.
 * Make sure that the current gyroscope full scale value is known by calling
 * ISDS_setGyroFullScale() or ISDS_getGyroFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRate The returned X-axis angular rate in [mdps]
 * @param[out] yRate The returned Y-axis angular rate in [mdps]
 * @param[out] zRate The returned Z-axis angular rate in [mdps]
 * @retval Error code
 */
int8_t ISDS_getAngularRates_int(WE_sensorInterface_t* sensorInterface, int32_t *xRate, int32_t *yRate, int32_t *zRate)
{
  int16_t xRawRate, yRawRate, zRawRate;
  if (WE_FAIL == ISDS_getRawAngularRates(sensorInterface, &xRawRate, &yRawRate, &zRawRate))
  {
    return WE_FAIL;
  }
  *xRate = ISDS_convertAngularRate_int(xRawRate, currentGyroFullScale);
  *yRate = ISDS_convertAngularRate_int(yRawRate, currentGyroFullScale);
  *zRate = ISDS_convertAngularRate_int(zRawRate, currentGyroFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the raw X-axis gyroscope sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRawAcc The returned raw X-axis angular rate
 * @retval Error code
 */
int8_t ISDS_getRawAngularRateX(WE_sensorInterface_t* sensorInterface, int16_t *xRawRate)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_X_OUT_L_GYRO_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *xRawRate = (int16_t) (tmp[1] << 8);
  *xRawRate |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw Y-axis gyroscope sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yRawRate The returned raw Y-axis angular rate
 * @retval Error code
 */
int8_t ISDS_getRawAngularRateY(WE_sensorInterface_t* sensorInterface, int16_t *yRawRate)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_Y_OUT_L_GYRO_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *yRawRate = (int16_t) (tmp[1] << 8);
  *yRawRate |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw Z-axis gyroscope sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zRawAcc The returned raw Z-axis angular rate
 * @retval Error code
 */
int8_t ISDS_getRawAngularRateZ(WE_sensorInterface_t* sensorInterface, int16_t *zRawRate)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_Z_OUT_L_GYRO_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *zRawRate = (int16_t) (tmp[1] << 8);
  *zRawRate |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw gyroscope sensor output for all three axes
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRawRate The returned raw X-axis angular rate
 * @param[out] yRawRate The returned raw Y-axis angular rate
 * @param[out] zRawRate The returned raw Z-axis angular rate
 * @retval Error code
 */
int8_t ISDS_getRawAngularRates(WE_sensorInterface_t* sensorInterface, int16_t *xRawRate, int16_t *yRawRate, int16_t *zRawRate)
{
  uint8_t tmp[6] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_X_OUT_L_GYRO_REG, 6, tmp))
  {
    return WE_FAIL;
  }

  *xRawRate = (int16_t) (tmp[1] << 8);
  *xRawRate |= (int16_t) tmp[0];

  *yRawRate = (int16_t) (tmp[3] << 8);
  *yRawRate |= (int16_t) tmp[2];

  *zRawRate = (int16_t) (tmp[5] << 8);
  *zRawRate |= (int16_t) tmp[4];

  return WE_SUCCESS;
}

#ifdef WE_USE_FLOAT
/**
 * @brief Read the X-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xAcc X-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationX_float(WE_sensorInterface_t* sensorInterface, float *xAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationX(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *xAcc = ISDS_convertAcceleration_float(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the Y-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yAcc Y-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationY_float(WE_sensorInterface_t* sensorInterface, float *yAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationY(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *yAcc = ISDS_convertAcceleration_float(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the Z-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zAcc Z-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationZ_float(WE_sensorInterface_t* sensorInterface, float *zAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationZ(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *zAcc = ISDS_convertAcceleration_float(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the accelerometer sensor output in [mg] for all three axes
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xAcc The returned X-axis acceleration in [mg]
 * @param[out] yAcc The returned Y-axis acceleration in [mg]
 * @param[out] zAcc The returned Z-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerations_float(WE_sensorInterface_t* sensorInterface, float *xAcc, float *yAcc, float *zAcc)
{
  int16_t xRawAcc, yRawAcc, zRawAcc;
  if (WE_FAIL == ISDS_getRawAccelerations(sensorInterface, &xRawAcc, &yRawAcc, &zRawAcc))
  {
    return WE_FAIL;
  }
  *xAcc = ISDS_convertAcceleration_float(xRawAcc, currentAccFullScale);
  *yAcc = ISDS_convertAcceleration_float(yRawAcc, currentAccFullScale);
  *zAcc = ISDS_convertAcceleration_float(zRawAcc, currentAccFullScale);
  return WE_SUCCESS;
}
#endif /* WE_USE_FLOAT */

/**
 * @brief Read the X-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xAcc X-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationX_int(WE_sensorInterface_t* sensorInterface, int16_t *xAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationX(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *xAcc = ISDS_convertAcceleration_int(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the Y-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yAcc Y-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationY_int(WE_sensorInterface_t* sensorInterface, int16_t *yAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationY(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *yAcc = ISDS_convertAcceleration_int(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the Z-axis acceleration in [mg]
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zAcc Z-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerationZ_int(WE_sensorInterface_t* sensorInterface, int16_t *zAcc)
{
  int16_t rawAcc;
  if (WE_FAIL == ISDS_getRawAccelerationZ(sensorInterface, &rawAcc))
  {
    return WE_FAIL;
  }
  *zAcc = ISDS_convertAcceleration_int(rawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the accelerometer sensor output in [mg] for all three axes
 *
 * Note that this functions relies on the current accelerometer full scale value.
 * Make sure that the current accelerometer full scale value is known by calling
 * ISDS_setAccFullScale() or ISDS_getAccFullScale() at least once prior to
 * calling this function.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xAcc The returned X-axis acceleration in [mg]
 * @param[out] yAcc The returned Y-axis acceleration in [mg]
 * @param[out] zAcc The returned Z-axis acceleration in [mg]
 * @retval Error code
 */
int8_t ISDS_getAccelerations_int(WE_sensorInterface_t* sensorInterface, int16_t *xAcc, int16_t *yAcc, int16_t *zAcc)
{
  int16_t xRawAcc, yRawAcc, zRawAcc;
  if (WE_FAIL == ISDS_getRawAccelerations(sensorInterface, &xRawAcc, &yRawAcc, &zRawAcc))
  {
    return WE_FAIL;
  }
  *xAcc = ISDS_convertAcceleration_int(xRawAcc, currentAccFullScale);
  *yAcc = ISDS_convertAcceleration_int(yRawAcc, currentAccFullScale);
  *zAcc = ISDS_convertAcceleration_int(zRawAcc, currentAccFullScale);
  return WE_SUCCESS;
}

/**
 * @brief Read the raw X-axis acceleration sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRawAcc The returned raw X-axis acceleration
 * @retval Error code
 */
int8_t ISDS_getRawAccelerationX(WE_sensorInterface_t* sensorInterface, int16_t *xRawAcc)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_X_OUT_L_ACC_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *xRawAcc = (int16_t) (tmp[1] << 8);
  *xRawAcc |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw Y-axis acceleration sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] yRawAcc The returned raw Y-axis acceleration
 * @retval Error code
 */
int8_t ISDS_getRawAccelerationY(WE_sensorInterface_t* sensorInterface, int16_t *yRawAcc)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_Y_OUT_L_ACC_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *yRawAcc = (int16_t) (tmp[1] << 8);
  *yRawAcc |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw Z-axis acceleration sensor output
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] zRawAcc The returned raw Z-axis acceleration
 * @retval Error code
 */
int8_t ISDS_getRawAccelerationZ(WE_sensorInterface_t* sensorInterface, int16_t *zRawAcc)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_Z_OUT_L_ACC_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *zRawAcc = (int16_t) (tmp[1] << 8);
  *zRawAcc |= (int16_t) tmp[0];

  return WE_SUCCESS;
}

/**
 * @brief Read the raw accelerometer sensor output for all three axes
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] xRawAcc The returned raw X-axis acceleration
 * @param[out] yRawAcc The returned raw Y-axis acceleration
 * @param[out] zRawAcc The returned raw Z-axis acceleration
 * @retval Error code
 */
int8_t ISDS_getRawAccelerations(WE_sensorInterface_t* sensorInterface, int16_t *xRawAcc, int16_t *yRawAcc, int16_t *zRawAcc)
{
  uint8_t tmp[6] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_X_OUT_L_ACC_REG, 6, tmp))
  {
    return WE_FAIL;
  }

  *xRawAcc = (int16_t) (tmp[1] << 8);
  *xRawAcc |= (int16_t) tmp[0];

  *yRawAcc = (int16_t) (tmp[3] << 8);
  *yRawAcc |= (int16_t) tmp[2];

  *zRawAcc = (int16_t) (tmp[5] << 8);
  *zRawAcc |= (int16_t) tmp[4];

  return WE_SUCCESS;
}

#ifdef WE_USE_FLOAT

/**
 * @brief Read the temperature in [°C]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] temperature Temperature in [°C]
 * @retval Error code
 */
int8_t ISDS_getTemperature_float(WE_sensorInterface_t* sensorInterface, float *temperature)
{
  int16_t tempRaw;
  if (WE_FAIL == ISDS_getRawTemperature(sensorInterface, &tempRaw))
  {
    return WE_FAIL;
  }
  *temperature = ISDS_convertTemperature_float(tempRaw);
  return WE_SUCCESS;
}

/**
 * @brief Read the temperature in [°C x 100]
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] temperature Temperature in [°C x 100]
 * @retval Error code
 */
int8_t ISDS_getTemperature_int(WE_sensorInterface_t* sensorInterface, int16_t *temperature)
{
  int16_t tempRaw;
  if (WE_FAIL == ISDS_getRawTemperature(sensorInterface, &tempRaw))
  {
    return WE_FAIL;
  }
  *temperature = ISDS_convertTemperature_int(tempRaw);
  return WE_SUCCESS;
}

#endif /* WE_USE_FLOAT */

/**
 * @brief Read the raw temperature
 *
 * Can be converted to [°C] using ISDS_convertTemperature_float()
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] temperature Raw temperature value (temperature sensor output)
 * @retval Error code
 */
int8_t ISDS_getRawTemperature(WE_sensorInterface_t* sensorInterface, int16_t *temperature)
{
  uint8_t tmp[2] = {0};

  if (WE_FAIL == ISDS_ReadReg(sensorInterface, ISDS_OUT_TEMP_L_REG, 2, tmp))
  {
    return WE_FAIL;
  }

  *temperature = (int16_t) (tmp[1] << 8);
  *temperature |= (int16_t) tmp[0];

  return WE_SUCCESS;
}


#ifdef WE_USE_FLOAT
/**
 * @brief Converts the supplied raw acceleration into [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @param[in] fullScale Accelerometer full scale
 * @retval The converted acceleration in [mg]
 */
float ISDS_convertAcceleration_float(int16_t acc, ISDS_accFullScale_t fullScale)
{
  switch (fullScale)
  {
  case ISDS_accFullScaleTwoG:
    return ISDS_convertAccelerationFs2g_float(acc);

  case ISDS_accFullScaleSixteenG:
    return ISDS_convertAccelerationFs16g_float(acc);

  case ISDS_accFullScaleFourG:
    return ISDS_convertAccelerationFs4g_float(acc);

  case ISDS_accFullScaleEightG:
    return ISDS_convertAccelerationFs8g_float(acc);

  case ISDS_accFullScaleInvalid:
  default:
    return 0;
  }
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleTwoG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
float ISDS_convertAccelerationFs2g_float(int16_t acc)
{
  return ((float) acc * 0.061f);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleFourG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
float ISDS_convertAccelerationFs4g_float(int16_t acc)
{
  return ((float) acc * 0.122f);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleEightG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
float ISDS_convertAccelerationFs8g_float(int16_t acc)
{
  return ((float) acc * 0.244f);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleSixteenG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
float ISDS_convertAccelerationFs16g_float(int16_t acc)
{
  return ((float) acc * 0.488f);
}

/**
 * @brief Converts the supplied raw angular rate into [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @param[in] fullScale Gyroscope full scale
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRate_float(int16_t rate, ISDS_gyroFullScale_t fullScale)
{
  switch (fullScale)
  {
  case ISDS_gyroFullScale250dps:
    return ISDS_convertAngularRateFs250dps_float(rate);

  case ISDS_gyroFullScale125dps:
    return ISDS_convertAngularRateFs125dps_float(rate);

  case ISDS_gyroFullScale500dps:
    return ISDS_convertAngularRateFs500dps_float(rate);

  case ISDS_gyroFullScale1000dps:
    return ISDS_convertAngularRateFs1000dps_float(rate);

  case ISDS_gyroFullScale2000dps:
    return ISDS_convertAngularRateFs2000dps_float(rate);

  default:
    return 0;
  }
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale125dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRateFs125dps_float(int16_t rate)
{
  return ((float) rate * 4.375f);
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale250dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRateFs250dps_float(int16_t rate)
{
  return ((float) rate * 8.75f);
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale500dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRateFs500dps_float(int16_t rate)
{
  return ((float) rate * 17.5f);
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale1000dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRateFs1000dps_float(int16_t rate)
{
  return ((float) rate * 35);
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale2000dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
float ISDS_convertAngularRateFs2000dps_float(int16_t rate)
{
  return ((float) rate * 70);
}

/**
 * @brief Converts the supplied raw temperature to [°C].
 * @param[in] temperature Raw temperature (temperature sensor output)
 * @retval The converted temperature in [°C]
 */
float ISDS_convertTemperature_float(int16_t temperature)
{
  return ((((float) temperature) / 256.0f) + 25.0f);
}
#endif /* WE_USE_FLOAT */


/**
 * @brief Converts the supplied raw acceleration into [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @param[in] fullScale Accelerometer full scale
 * @retval The converted acceleration in [mg]
 */
int16_t ISDS_convertAcceleration_int(int16_t acc, ISDS_accFullScale_t fullScale)
{
  switch (fullScale)
  {
  case ISDS_accFullScaleTwoG:
    return ISDS_convertAccelerationFs2g_int(acc);

  case ISDS_accFullScaleSixteenG:
    return ISDS_convertAccelerationFs16g_int(acc);

  case ISDS_accFullScaleFourG:
    return ISDS_convertAccelerationFs4g_int(acc);

  case ISDS_accFullScaleEightG:
    return ISDS_convertAccelerationFs8g_int(acc);

  case ISDS_accFullScaleInvalid:
  default:
    return 0;
  }
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleTwoG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
int16_t ISDS_convertAccelerationFs2g_int(int16_t acc)
{
  return (int16_t) ((((int32_t) acc) * 61) / 1000);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleFourG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
int16_t ISDS_convertAccelerationFs4g_int(int16_t acc)
{
  return (int16_t) ((((int32_t) acc) * 122) / 1000);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleEightG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
int16_t ISDS_convertAccelerationFs8g_int(int16_t acc)
{
  return (int16_t) ((((int32_t) acc) * 244) / 1000);
}

/**
 * @brief Converts the supplied raw acceleration sampled using
 * ISDS_accFullScaleSixteenG to [mg]
 * @param[in] acc Raw acceleration value (accelerometer output)
 * @retval The converted acceleration in [mg]
 */
int16_t ISDS_convertAccelerationFs16g_int(int16_t acc)
{
  return (int16_t) ((((int32_t) acc) * 488) / 1000);
}

/**
 * @brief Converts the supplied raw angular rate into [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @param[in] fullScale Gyroscope full scale
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRate_int(int16_t rate, ISDS_gyroFullScale_t fullScale)
{
  switch (fullScale)
  {
  case ISDS_gyroFullScale250dps:
    return ISDS_convertAngularRateFs250dps_int(rate);

  case ISDS_gyroFullScale125dps:
    return ISDS_convertAngularRateFs125dps_int(rate);

  case ISDS_gyroFullScale500dps:
    return ISDS_convertAngularRateFs500dps_int(rate);

  case ISDS_gyroFullScale1000dps:
    return ISDS_convertAngularRateFs1000dps_int(rate);

  case ISDS_gyroFullScale2000dps:
    return ISDS_convertAngularRateFs2000dps_int(rate);

  default:
    return 0;
  }
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale125dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRateFs125dps_int(int16_t rate)
{
  return (((int32_t) rate) * 4375) / 1000;
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale250dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRateFs250dps_int(int16_t rate)
{
  return (((int32_t) rate) * 875) / 100;
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale500dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRateFs500dps_int(int16_t rate)
{
  return (((int32_t) rate) * 175) / 10;
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale1000dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRateFs1000dps_int(int16_t rate)
{
  return ((int32_t) rate) * 35;
}

/**
 * @brief Converts the supplied raw angular rate sampled using
 * ISDS_gyroFullScale2000dps to [mdps] (millidegrees per second).
 * @param[in] rate Raw angular rate (gyroscope output)
 * @retval The converted angular rate in [mdps] (millidegrees per second)
 */
int32_t ISDS_convertAngularRateFs2000dps_int(int16_t rate)
{
  return ((int32_t) rate) * 70;
}

/**
 * @brief Converts the supplied raw temperature to [°C x 100].
 * @param[in] temperature Raw temperature (temperature sensor output)
 * @retval The converted temperature in [°C x 100]
 */
int16_t ISDS_convertTemperature_int(int16_t temperature)
{
  return (((int32_t) temperature) * 100) / 256 + 2500;
}
