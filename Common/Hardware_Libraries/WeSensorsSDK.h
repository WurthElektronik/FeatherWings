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
 * @brief This is the main header file of the WE Sensors SDK.
 */

#ifndef WE_SENSORS_SDK_H_INCLUDED
#define WE_SENSORS_SDK_H_INCLUDED

#include <stdint.h>
/**
 * @brief SDK major version number.
 */
#define WE_SENSOR_SDK_MAJOR_VERSION 2

/**
 * @brief SDK minor version number.
 */
#define WE_SENSOR_SDK_MINOR_VERSION 6

/**
 * @brief Types of sensors supported by the WE Sensors SDK.
 */
typedef enum
{
  WE_HIDS, /* Humidity sensor */
  WE_ITDS, /* Acceleration sensor */
  WE_TIDS, /* Temperature sensor */
  WE_PADS, /* Absolute pressure sensor */
  WE_PDUS, /* Differential pressure sensor */
  WE_ISDS, /* 3D accelerometer and 3D gyroscope */
  WE_SENSOR_TYPE_MAX
} WE_sensorType_t;

/**
 * @brief Supported digital interfaces of the sensors.
 */
typedef enum
{
  WE_i2c,
  WE_spi
} WE_sensorInterfaceType_t;

/**
 * @brief Protocols for I2C interface.
 */
typedef enum
{
  /**
   * @brief I2C protocol where you need to specify which register you are writing to or reading from.
   */
  WE_i2cProtocol_RegisterBased = 0x0,

  /**
   * @brief I2C protocol to deal with raw data.
   */
  WE_i2cProtocol_Raw = 0x1
} WE_i2cProtocol_t;

/**
 * @brief Options for I2C interface.
 */
typedef struct
{
  /**
   * @brief The sensor's I2C address.
   */
  uint8_t address;

  /**
   * @brief Enables receiving of multiple bytes in a single read operation.
   */
  uint8_t burstMode : 1;

  /**
   * @brief Selects the I2C protocol used for reading and writing.
   */
  WE_i2cProtocol_t protocol : 1;

  /**
   * @brief Enables usage of most significant bit of I2C register address to enable
   * multi byte read (required e.g. by HIDS humidity sensor).
   */
  uint8_t useRegAddrMsbForMultiBytesRead : 1;

  /**
   * @brief Currently unused.
   */
  uint8_t reserved : 5;
} WE_i2cOptions_t;

/**
 * @brief Options for SPI interface.
 */
typedef struct
{
  /**
   * @brief HAL port of chip select pin. The type of the port depends on the platform.
   */
  void *chipSelectPort;

  /**
   * @brief Pin to use for chip select.
   */
  uint16_t chipSelectPin;

  /**
   * @brief Enables receiving of multiple bytes in a single read operation.
   */
  uint8_t burstMode : 1;

  /**
   * @brief Currently unused.
   */
  uint8_t reserved : 7;
} WE_spiOptions_t;

/**
 * @brief Interface options.
 */
typedef struct
{
  /**
   * @brief I2C interface options.
   */
  WE_i2cOptions_t i2c;

  /**
   * @brief SPI interface options.
   */
  WE_spiOptions_t spi;

  /**
   * @brief Timeout (ms) for read operations.
   */
  uint16_t readTimeout;

  /**
   * @brief Timeout (ms) for write operations.
   */
  uint16_t writeTimeout;
} WE_sensorInterfaceOptions_t;

/**
 * @brief Sensor interface configuration structure.
 */
typedef struct
{
  /**
   * @brief Sensor type specifier.
   */
  WE_sensorType_t sensorType;

  /**
   * @brief Specifies the interface to be used to communicate with the sensor.
   */
  WE_sensorInterfaceType_t interfaceType;

  /**
   * @brief Options of sensor interface.
   */
  WE_sensorInterfaceOptions_t options;

  /**
   * @brief HAL interface handle. The type of the handle depends on the interface used.
   */
  void *handle;
} WE_sensorInterface_t;

#endif /* WE_SENSORS_SDK_H_INCLUDED */
