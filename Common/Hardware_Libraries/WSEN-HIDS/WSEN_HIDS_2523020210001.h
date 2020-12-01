/**
 ***************************************************************************************************
 * This file is part of Sensors SDK:
 * https://www.we-online.com/sensors
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 *THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 *KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 *INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 *INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 *REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 *PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT
 *RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED.
 *INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS
 *OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE
 *SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
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

/**         Available PADS 2511020213301 I2C Slave 7 bit addresses - this sensor
 * has one (1) fixed 7 bit address */

#define HIDS_ADDRESS_I2C_0 (uint8_t)0x5F

/**         HIDS 2523020210001  DEVICE_ID         */

#define HIDS_DEVICE_ID_VALUE \
  (uint8_t)0xBC /* Device ID of HIDS 2523020210001 Sensor */

/**         Register address definitions         */

/* 0x00 .. 0x0E -> RESERVED */

#define HIDS_DEVICE_ID_REG (uint8_t)0x0F /* Device ID register */
#define HIDS_AVG_HT_REG (uint8_t)0x10    /* HIDS AVG_HT register r/w */

/* 0x11 .. 0x1F -> RESERVED */

#define HIDS_CTRL_1_REG (uint8_t)0x20 /* Control 1 register r/w */
#define HIDS_CTRL_2_REG (uint8_t)0x21 /* Control 2 register r/w */
#define HIDS_CTRL_3_REG (uint8_t)0x22 /* Control 2 register r/w */

/* 0x23 .. 0x26 -> RESERVED */

#define HIDS_STATUS_REG (uint8_t)0x27 /* HIDS status register */
#define HIDS_H_OUT_L_REG \
  (uint8_t)0x28 /* HIDS relative humidity our lsb register */
#define HIDS_H_OUT_H_REG \
  (uint8_t)0x29 /* HIDS relative humidity our msb register */
#define HIDS_T_OUT_L_REG \
  (uint8_t)0x2A /* HIDS temperature output lsb register */
#define HIDS_T_OUT_H_REG \
  (uint8_t)0x2B /* HIDS temperature output msb register */

/* 0x2C .. 0x2F -> RESERVED */

/* 0x30 .. 0x3F -> FACTORY CALIBRATION */
#define HIDS_H0_RH_X2 (uint8_t)0x30
#define HIDS_H1_RH_X2 (uint8_t)0x31
#define HIDS_T0_DEGC_X8 (uint8_t)0x32
#define HIDS_T1_DEGC_X8 (uint8_t)0x33
#define HIDS_T0_T1_DEGC_H2 (uint8_t)0x35
#define HIDS_H0_T0_OUT_L (uint8_t)0x36
#define HIDS_H0_T0_OUT_H (uint8_t)0x37
#define HIDS_H1_T0_OUT_L (uint8_t)0x3A
#define HIDS_H1_T0_OUT_H (uint8_t)0x3B
#define HIDS_T0_OUT_L (uint8_t)0x3C
#define HIDS_T0_OUT_H (uint8_t)0x3D
#define HIDS_T1_OUT_L (uint8_t)0x3E
#define HIDS_T1_OUT_H (uint8_t)0x3F

/**
 * The Registers in 30h..3Fh address range contain factory calibration
 * coefficients. Every sensor IC has individual coefficients. Before the first
 * calculation of temperature and/or humidity, the host is required to read out
 * the calibration coefficients.
 */

/**
 * Control Register 1
 * Address 0x20
 * Type  R/W
 * Default value: 0x00
 *  *    ODR1  | ODR0   | Humidity/Temperature output data-rate(Hz)
 *   ---------------- ----------------------------------------------------
 *        0    |  0     |            Single Conversion
 *        0    |  1     |                 1
 *        1    |  0     |                 7
 *        1    |  1     |                 12.5
 */
typedef struct
{
  uint8_t outputDataRate : 2; /* ODR[1:0]: output data rate. */

  uint8_t blockDataUpdate : 1; /* 0: HIDS_H_OUT_X_REG and HIDS_T_OUT_X_REG wil
                                  be updated continuously,
                                  1: when L register is read H register will not
                                  be updated till it is also read */
  uint8_t notUsed : 4;         /* This bits must be set to 0 for proper operation of the
                          device */
  uint8_t powerDown : 1;       /* 0: power down mode, 1: active mode */
} HIDS_ctrl_1_t;

/**
 * Control Register 2
 * Address 0x21
 * Type  R/W
 * Default value: 0x00
 */
typedef struct
{
  uint8_t oneShotbit : 1; /* ONE_SHOT : 0  Normal Operation; 1: start
                             Measurement */

  uint8_t heater : 1; /* integrated heater enabled (de-condensation), enabled
                         will consume additional current. 0 heater disabled, 1
                         heater enabled */

  uint8_t notUsed : 5; /* This bit must be set to 0 for proper operation of
                            the device*/

  uint8_t boot : 1; /* BOOT :Reboot memory content. 0: normal mode; 1: reboot
                       memory content. Self-clearing upon completion (read till
                       '0' again, to wait for boot done) */
} HIDS_ctrl_2_t;

/**
 * Status register
 * Address 0x27
 * read only
 * Default value: Output; 0x00
 */
typedef struct
{
  uint8_t temperatureDataAvailable : 1; /* T_DA : Temperature data available.(0:
                                      temperature data not yet available; 1: a
                                      new temperature data is available) */

  uint8_t humidityDataAvailable : 1; /* H_DA :Humidity data available. (0:
                                      humidity not yet available; 1: a new
                                      humidity data is available) */

  uint8_t notUsed : 6; /* This 6 bits must be set to 0 for proper operation
                                      of the device */
} HIDS_status_t;

typedef enum
{
  disableHIDS = 0,
  enableHIDS = 1
} HIDS_state_t;

typedef enum
{
  outputDataRatePowerDownHIDS = 0, /* single conversion/power down */
  outputDataRate1HzHIDS = 1,       /* 1Hz */
  outputDataRate7HzHIDS = 2,       /* 7Hz */
  outputDataRate12_5HzHIDS = 3,    /* 12.5Hz */
} HIDS_output_data_rate_t;

#ifdef __cplusplus
extern "C"
{
#endif

  int8_t HIDS_getDeviceID(uint8_t *devID);
  int8_t HIDS_activate();

  int8_t HIDS_setOneShot(void);
  int8_t HIDS_setSwReset(void);

  /* Temperature and Humidity data available State */
  int8_t HIDS_getStatus(HIDS_state_t *state);

  /* standard Data Out */

  int8_t HIDS_getRAWValues(int16_t *rawHumidity, int16_t *rawTemp);
  int8_t HIDS_getTemperature(float *tempDegC); // Temperature Value in °C
  int8_t HIDS_getHumidity(float *humidity);    // humidity value in percent

#ifdef __cplusplus
}
#endif

#endif /* _WSEN_HIDS_H */
       /**         EOF         */