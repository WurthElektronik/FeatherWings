/**
 ***************************************************************************************************
 * This file is part of WE sensors SDK:
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
 * COPYRIGHT (c) 2019 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

#include "WSEN_HIDS_2523020210001.h"
#include "ConfigPlatform.h"

/**
 * @brief  Read the device ID
 * @param  Pointer to device ID.
 * @retval Error code
 */
int8_t HIDS_getDeviceID(uint8_t *devID)
{
  if (ReadReg((uint8_t)HIDS_DEVICE_ID_REG, 1, devID))
    return WE_FAIL;

  return WE_SUCCESS; /* expected value is PADS_DEVICE_ID_VALUE */
}

/**
 * @brief  Activate the sensor
 * @param  none
 * @retval Error code
 */

int8_t HIDS_activate()
{
  /* Activate */
  HIDS_ctrl_1_t reg;

  reg.notUsed = 0;
  reg.blockDataUpdate = 1;
  reg.outputDataRate = outputDataRatePowerDownHIDS;
  reg.powerDown = 1;

  if (WE_FAIL == WriteReg((uint8_t)HIDS_CTRL_1_REG, 1, (uint8_t *)&reg))
    return WE_FAIL;

  uint8_t avg_reg;
  avg_reg = 0x1B;
  if (WE_FAIL == WriteReg((uint8_t)HIDS_AVG_HT_REG, 1, &avg_reg))
    return WE_FAIL;

  return WE_SUCCESS;
}

/**
 * @brief  Set software reset and wait till the reset is performed successfully.
 * @param  none
 * @retval Error code
 */
int8_t HIDS_setSwReset(void)
{
  HIDS_ctrl_2_t ctrl_reg;
  ctrl_reg.boot = 1;
  ctrl_reg.heater = 0;
  ctrl_reg.notUsed = 0;
  ctrl_reg.oneShotbit = 0;

  if (WE_FAIL == WriteReg((uint8_t)HIDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg))
    return WE_FAIL;

  while (ctrl_reg.boot == 1) /* wait till boot done */
  {
    if (WE_FAIL == ReadReg((uint8_t)HIDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg))
      return WE_FAIL;

    /* todo: skip loop by timeout */
  }

  return WE_SUCCESS;
}
/**
 * @brief  Set the sensor to one shot mode
 * @param  none
 * @retval Error code
 */
int8_t HIDS_setOneShot(void)
{
  HIDS_ctrl_2_t ctrl_reg; /* do not enable heater, do not enable sw reset */

  if (WE_FAIL == ReadReg((uint8_t)HIDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg))
    return WE_FAIL;

  if ((ctrl_reg.boot == 1) || (ctrl_reg.heater == 1))
  {
    return WE_FAIL; /* HIDS is in a state where one shot is not allowed or
                       possible */
  }
  else if (ctrl_reg.oneShotbit == 0)
  {
    ctrl_reg.boot = 0;
    ctrl_reg.heater = 0;
    ctrl_reg.notUsed = 0;
    ctrl_reg.oneShotbit = 1;
    if (WE_FAIL == WriteReg((uint8_t)HIDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg))
      return WE_FAIL;
  }
  else
  {
    return WE_FAIL;
  }

  /* else: one shot is already active. but measurement not yet done */

  /*
   * measurement is available when:
   * HIDS_STATUS_REG shows both temperature and humidity bit == '1'
   * HIDS_CTRL_2_REG.oneShotbit changes back to '0'
   */

  return WE_SUCCESS;
}

/**
 * @brief  Check if the humidity data are available
 * @param  pointer to humidity data State
 * @retval error code
 */
int8_t HIDS_getStatus(HIDS_state_t *state)
{
  HIDS_status_t status_reg;

  if (ReadReg((uint8_t)HIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
    return WE_FAIL;

  if ((status_reg.humidityDataAvailable == 1) &&
      (status_reg.temperatureDataAvailable == 1))
  {
    *state = enableHIDS;
  }
  else
  {
    *state = disableHIDS;
  }

  return WE_SUCCESS;
}

/**
 * @brief  Get raw humidity values
 * @param  pointer to store raw temperature data
 * @param  pointer to store humidity data
 * @retval error code
 */
int8_t HIDS_getRAWValues(int16_t *rawHumidity, int16_t *rawTemp)
{
  uint8_t raw[4] = {0};

  if (ReadReg((uint8_t)HIDS_H_OUT_L_REG, 4, (uint8_t *)&raw))
    return WE_FAIL;

  *rawHumidity = (int16_t)(raw[1] << 8);
  *rawHumidity |= (int16_t)(raw[0]);
  *rawTemp = (int16_t)(raw[3] << 8);
  *rawTemp |= (int16_t)(raw[2]);

  return WE_SUCCESS;
}

/**
 * @brief  Get humidity values in %
 * @param  pointer to store humidity data
 * @retval error code
 */
int8_t HIDS_getHumidity(float *humidity)
{
  uint16_t H0_T0_out, H1_T0_out, H_T_out;
  uint16_t H0_rh, H1_rh;
  uint8_t buffer;
  float h_temp = 0.0;
  float hum = 0.0;

  if (ReadReg(HIDS_H_OUT_H_REG, 1, &buffer))
    return WE_FAIL;
  H_T_out = (uint16_t)buffer << 8;
  if (ReadReg(HIDS_H_OUT_L_REG, 1, &buffer))
    return WE_FAIL;
  H_T_out |= buffer;

  if (ReadReg((uint8_t)HIDS_H0_RH_X2, 1, &buffer))
    return WE_FAIL;
  H0_rh = buffer >> 1;

  if (ReadReg((uint8_t)HIDS_H1_RH_X2, 1, &buffer))
    return WE_FAIL;
  H1_rh = buffer >> 1;

  hum = ((int16_t)(H1_rh) - (int16_t)(H0_rh));

  if (ReadReg(HIDS_H0_T0_OUT_H, 1, &buffer))
    return WE_FAIL;
  H0_T0_out = (((uint16_t)buffer) << 8);

  if (ReadReg(HIDS_H0_T0_OUT_L, 1, &buffer))
    return WE_FAIL;
  H0_T0_out |= buffer;

  if (ReadReg(HIDS_H1_T0_OUT_H, 1, &buffer))
    return WE_FAIL;
  H1_T0_out = (((uint16_t)buffer) << 8);

  if (ReadReg(HIDS_H1_T0_OUT_L, 1, &buffer))
    return WE_FAIL;
  H1_T0_out |= buffer;

  // Decode Humidity
  hum = ((int16_t)(H1_rh) - (int16_t)(H0_rh));

  // Calculate humidity in decimal of degree centigrade i.e. 15.0 = 150.
  h_temp = (float)(((int16_t)H_T_out - (int16_t)H0_T0_out) * hum) /
           (float)((int16_t)H1_T0_out - (int16_t)H0_T0_out);
  hum = (float)H0_rh;
  *humidity = hum + h_temp; // provide signed % measurement unit

  return WE_SUCCESS;
}

/**
 * @brief  Get temperature values in degree centigrade
 * @param  pointer to store temperature data
 * @retval error code
 */
int8_t HIDS_getTemperature(float *tempDegC)
{
  uint16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
  uint16_t T0_degC, T1_degC;
  uint8_t buffer, tmp;
  float t_temp = 0.0;
  float deg = 0.0;

  if (ReadReg(HIDS_T0_T1_DEGC_H2, 1, &tmp))
    return WE_FAIL;

  if (ReadReg(HIDS_T0_DEGC_X8, 1, &buffer))
    return WE_FAIL;
  T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer);
  // remove 8x multiple
  T0_degC = T0_degC_x8_u16 >> 3;

  if (ReadReg(HIDS_T1_DEGC_X8, 1, &buffer))
    return WE_FAIL;
  T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer);
  // remove 8x multiple
  T1_degC = T1_degC_x8_u16 >> 3;

  if (ReadReg(HIDS_T0_OUT_H, 1, &buffer))
    return WE_FAIL;
  T0_out = ((uint16_t)buffer) << 8;

  if (ReadReg(HIDS_T0_OUT_L, 1, &buffer))
    return WE_FAIL;
  T0_out |= buffer;

  if (ReadReg(HIDS_T1_OUT_H, 1, &buffer))
    return WE_FAIL;
  T1_out = ((uint16_t)buffer) << 8;

  if (ReadReg(HIDS_T1_OUT_L, 1, &buffer))
    return WE_FAIL;
  T1_out |= buffer;

  if (ReadReg(HIDS_T_OUT_H_REG, 1, &buffer))
    return WE_FAIL;
  T_out = (uint16_t)buffer << 8;

  if (ReadReg(HIDS_T_OUT_L_REG, 1, &buffer))
    return WE_FAIL;
  T_out |= buffer;

  // Decode Temperature
  deg = (float)((int16_t)(T1_degC) - (int16_t)(T0_degC));

  // T_DegC = (temp_T0_degC + (temp_out - temp0_out) * (temp_T1_degC -
  // temp_T0_degC) / (temp1_out - temp0_out));

  // Calculate Temperature in decimal of grade
  // centigrade i.e. 15.0 = 150.
  t_temp = (float)(((int16_t)T_out - (int16_t)T0_out) * deg) /
           (float)((int16_t)T1_out - (int16_t)T0_out);
  deg = (float)((int16_t)T0_degC); // remove x8 multiple
  // provide signed celsius measurement unit
  *tempDegC = deg + t_temp;

  return WE_SUCCESS;
}
/**         EOF         */