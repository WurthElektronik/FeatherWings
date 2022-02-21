/**
***************************************************************************************************
* This file is part of WE sensors SDK:
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
#include <stdio.h>
#include "WSEN_HIDS_2523020210001.h"

/**
* @brief  Set the Humidity average configuration
* @param  Humidity average parameter
* @retval Error code
*/
uint8_t HIDS_setHumAvg(HIDS_average_conf_t avgH)
{
	HIDS_average_config_t average_reg;

	if (ReadReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;

	average_reg.avgHum = avgH;

	if (WriteReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the Humidity average configuration
* @param  Pointer to the Humidity average configuration
* @retval Error code
*/
uint8_t HIDS_getHumAvg(HIDS_average_conf_t *avgH)
{
	HIDS_average_config_t average_reg;

	if (ReadReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;
	*avgH = (HIDS_average_conf_t)average_reg.avgHum;

	return WE_SUCCESS;
}

/**
* @brief  Set the Temperature average configuration
* @param  Temperature average parameter
* @retval Error code
*/
uint8_t HIDS_setTempAvg(HIDS_average_conf_t avgT)
{
	HIDS_average_config_t average_reg;

	if (ReadReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;

	average_reg.avgTemp = avgT;

	if (WriteReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the Temperature average configuration
* @param  Pointer to the Temperature average configuration
* @retval Error code
*/
uint8_t HIDS_getTempAvg(HIDS_average_conf_t *avgT)
{
	HIDS_average_config_t average_reg;

	if (ReadReg((uint8_t)HIDS_Average_REG, 1, (uint8_t *)&average_reg))
		return WE_FAIL;
	*avgT = (HIDS_average_conf_t)average_reg.avgTemp;

	return WE_SUCCESS;
}

/**
* @brief  Set the output data rate of the sensor
* @param  output data rate
* @retval Error code
*/
int8_t HIDS_setOdr(HIDS_output_data_rate_t odr)
{

	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.odr = odr;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the output data rate of the sensor
* @param  Pointer to Output Data Rate
* @retval Error code
*/
int8_t HIDS_getOdr(HIDS_output_data_rate_t *Odr)
{
	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*Odr = (HIDS_output_data_rate_t)ctrl_reg_1.odr;
	return WE_SUCCESS;
}

/**
* @brief  Set the block data update state
* @param  BDU state
* @retval Error code
*/
int8_t HIDS_setBdu(HIDS_state_t bdu)
{
	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.bdu = bdu;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the block data update state
* @param  Pointer to Block data update state
* @retval Error code
*/
int8_t HIDS_getBduState(HIDS_state_t *bdu)
{
	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*bdu = (HIDS_state_t)ctrl_reg_1.bdu;
	return WE_SUCCESS;
}

/**
* @brief  Set the Power Control Mode
* @param  Power Control Mode
* @retval Error code
*/
int8_t HIDS_setPowerMode(HIDS_power_mode_t pd)
{
	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.powerControlMode = pd;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the Power Control Mode
* @param  Pointer to Power Control Mode
* @retval Error code
*/
int8_t HIDS_getPowerMode(HIDS_power_mode_t *pd)
{
	HIDS_ctrl_1_t ctrl_reg_1;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_1, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*pd = (HIDS_power_mode_t)ctrl_reg_1.powerControlMode;
	return WE_SUCCESS;
}

/**
* @brief  enable the One Shot Mode
* @param  One shot bit state
* @retval Error code
*/
int8_t HIDS_enOneShot(HIDS_state_t oneShot)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.oneShotBit = oneShot;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the Power Control Mode
* @param  Pointer to One shot bit state
* @retval Error code
*/
int8_t HIDS_getOneShotState(HIDS_state_t *oneShot)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*oneShot = (HIDS_state_t)ctrl_reg_2.oneShotBit;
	return WE_SUCCESS;
}

/**
* @brief  enable the Heater
* @param  Heater state
* @retval Error code
*/
int8_t HIDS_enHeater(HIDS_state_t heater)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.heater = heater;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the Heater state
* @param  Pointer to Heater state
* @retval Error code
*/
int8_t HIDS_getHeaterState(HIDS_state_t *heater)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*heater = (HIDS_state_t)ctrl_reg_2.heater;
	return WE_SUCCESS;
}

/**
* @brief  enable the Memory reboot
* @param  Reboot state
* @retval Error code
*/
int8_t HIDS_enRebootMemory(HIDS_state_t reboot)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.rebootMemory = reboot;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the reboot state
* @param  Pointer to reboot state
* @retval Error code
*/
int8_t HIDS_getRebootMemoryState(HIDS_state_t *reboot)
{
	HIDS_ctrl_2_t ctrl_reg_2;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_2, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*reboot = (HIDS_state_t)ctrl_reg_2.rebootMemory;
	return WE_SUCCESS;
}

/**
* @brief  enable the data Ready
* @param  data Ready
* @retval Error code
*/
int8_t HIDS_enInt(HIDS_state_t Drdy)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.enDataReady = Drdy;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the data Ready state
* @param  Pointer to data Ready state
* @retval Error code
*/
int8_t HIDS_getIntState(HIDS_state_t *Drdy)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*Drdy = (HIDS_state_t)ctrl_reg_3.enDataReady;
	return WE_SUCCESS;
}

/**
* @brief  set the Interrupt pin Type [push-pull / open drain]
* @param  data Ready
* @retval Error code
*/
int8_t HIDS_setIntPinType(HIDS_PP_OD_t PP_OD)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.interruptPinConfig = PP_OD;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the Interrupt pin Type
* @param  Pointer to the Interrupt pin Type
* @retval Error code
*/
int8_t HIDS_getIntPinType(HIDS_PP_OD_t *PP_OD)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*PP_OD = (HIDS_PP_OD_t)ctrl_reg_3.interruptPinConfig;
	return WE_SUCCESS;
}

/**
* @brief  set the data ready output level
* @param  data ready output level
* @retval Error code
*/
int8_t HIDS_setIntLevel(HIDS_active_level_t Drdy_H_L)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.drdyOutputLevel = Drdy_H_L;

	if (WriteReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the data ready output level
* @param  Pointer to the data ready output level
* @retval Error code
*/
int8_t HIDS_getIntLevel(HIDS_active_level_t *Drdy_H_L)
{
	HIDS_ctrl_3_t ctrl_reg_3;

	if (ReadReg((uint8_t)HIDS_CTRL_REG_3, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*Drdy_H_L = (HIDS_active_level_t)ctrl_reg_3.drdyOutputLevel;
	return WE_SUCCESS;
}

/**
* @brief  Read the device ID
* @param  Pointer to device ID.
* @retval Error code
*/
int8_t HIDS_getDeviceID(uint8_t *devID)
{
	if (ReadReg((uint8_t)HIDS_DEVICE_ID_REG, 1, devID))
		return WE_FAIL;

	return WE_SUCCESS; /* expected value is HIDS_DEVICE_ID_VALUE */
}

/**
* @brief Read the humidity Data availability
* @param  Pointer to the humidity Data availability state
* @retval Error code
*/
int8_t HIDS_getHumStatus(HIDS_state_t *humidity_state)
{
	HIDS_status_t status_reg;

	if (ReadReg((uint8_t)HIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*humidity_state = (HIDS_state_t)status_reg.humDataAvailable;
	return WE_SUCCESS;
}

/**
* @brief Read the Temperature Data availability
* @param  Pointer to the temperature Data availability state
* @retval Error code
*/
int8_t HIDS_getTempStatus(HIDS_state_t *temp_state)
{
	HIDS_status_t status_reg;

	if (ReadReg((uint8_t)HIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*temp_state = (HIDS_state_t)status_reg.tempDataAvailable;
	return WE_SUCCESS;
}

/**
* @brief  Read the temperature and humidity raw values
* @param  Pointer to rawHumidity and  rawTemp
* @retval Error code
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
* @brief  Read the raw humidity output
* @param  no parameter.
* @retval Humidity Output
*/

uint16_t HIDS_get_H_T_out()
{
	uint8_t buffer;
	uint16_t H_T_out = 0;
	if (ReadReg(HIDS_H_OUT_H_REG, 1, &buffer))
		return WE_FAIL;
	H_T_out = (uint16_t)buffer << 8;

	if (ReadReg(HIDS_H_OUT_L_REG, 1, &buffer))
		return WE_FAIL;
	H_T_out |= buffer;

	return H_T_out;
}

/**
* @brief  Read the H0_T0_out calibration
* @param  no parameter.
* @retval H0_T0_out calibration
*/
uint16_t HIDS_get_H0_T0_out()
{
	uint8_t buffer;
	uint16_t H0_T0_out = 0;
	if (ReadReg(HIDS_H0_T0_OUT_H, 1, &buffer))
		return WE_FAIL;
	H0_T0_out = (((uint16_t)buffer) << 8);

	if (ReadReg(HIDS_H0_T0_OUT_L, 1, &buffer))
		return WE_FAIL;
	H0_T0_out |= buffer;

	return H0_T0_out;
}

/**
* @brief  Read the H1_T0_out calibration
* @param  no parameter.
* @retval H1_T0_out calibration
*/
uint16_t HIDS_get_H1_T0_out()
{
	uint8_t buffer;
	uint16_t H1_T0_out = 0;
	if (ReadReg(HIDS_H1_T0_OUT_H, 1, &buffer))
		return WE_FAIL;
	H1_T0_out = (((uint16_t)buffer) << 8);

	if (ReadReg(HIDS_H1_T0_OUT_L, 1, &buffer))
		return WE_FAIL;
	H1_T0_out |= buffer;

	return H1_T0_out;
}

/**
* @brief  Read the H0_rh calibration
* @param  no parameter.
* @retval H0_rh calibration
*/

uint16_t HIDS_get_H0_rh()
{
	uint8_t H0_rh = 0;

	if (ReadReg(HIDS_H0_RH_X2, 1, &H0_rh))
		return WE_FAIL;

	return H0_rh;
}

/**
* @brief  Read the H1_rh calibration
* @param  no parameter.
* @retval H1_rh calibration
*/
uint16_t HIDS_get_H1_rh()
{
	uint8_t H1_rh = 0;

	if (ReadReg((uint8_t)HIDS_H1_RH_X2, 1, &H1_rh))
		return WE_FAIL;

	return H1_rh;
}

/**
* @brief  Read the raw temperature output
* @param  no parameter.
* @retval Temperature Output
*/
uint16_t HIDS_get_T_OUT()
{
	uint8_t buffer;
	uint16_t T_OUT = 0;
	if (ReadReg(HIDS_T_OUT_H_REG, 1, &buffer))
		return WE_FAIL;
	T_OUT = (uint16_t)buffer << 8;

	if (ReadReg(HIDS_T_OUT_L_REG, 1, &buffer))
		return WE_FAIL;
	T_OUT |= buffer;

	return T_OUT;
}

/**
* @brief  Read the T1_OUT calibration
* @param  no parameter.
* @retval T1_OUT calibration
*/

uint16_t HIDS_get_T1_OUT()
{
	uint8_t buffer;
	uint16_t T1_OUT = 0;
	if (ReadReg(HIDS_T1_OUT_H, 1, &buffer))
		return WE_FAIL;
	T1_OUT = ((uint16_t)buffer) << 8;

	if (ReadReg(HIDS_T1_OUT_L, 1, &buffer))
		return WE_FAIL;
	T1_OUT |= buffer;

	return T1_OUT;
}

/**
* @brief  Read the T0_OUT calibration
* @param  no parameter.
* @retval T0_OUT calibration
*/
uint16_t HIDS_get_T0_OUT()
{
	uint8_t buffer;
	uint16_t T0_OUT = 0;
	if (ReadReg(HIDS_T0_OUT_H, 1, &buffer))
		return WE_FAIL;
	T0_OUT = ((uint16_t)buffer) << 8;

	if (ReadReg(HIDS_T0_OUT_L, 1, &buffer))
		return WE_FAIL;
	T0_OUT |= buffer;

	return T0_OUT;
}

/**
* @brief  Read the Humidity data
* @param  Pointer to the Humidity data in %
* @retval error code
*/

int8_t HIDS_getHumidity(float *humidity)
{

	uint16_t H0_T0_out, H1_T0_out, H_T_out;
	uint16_t H0_rh, H1_rh;
	float h_temp = 0.0;
	float hum = 0.0;

	H_T_out = HIDS_get_H_T_out();

	H0_rh = HIDS_get_H0_rh();
	H0_rh = H0_rh >> 1;

	H1_rh = HIDS_get_H1_rh();
	H1_rh = H1_rh >> 1;

	hum = H1_rh - H0_rh;

	H0_T0_out = HIDS_get_H0_T0_out();

	H1_T0_out = HIDS_get_H1_T0_out();

	// Decode Humidity

	// Calculate humidity in decimal i.e. 15.0 = 150.

	h_temp = (float)(((int16_t)H_T_out - (int16_t)H0_T0_out) * hum) /

			 (float)((int16_t)H1_T0_out - (int16_t)H0_T0_out);

	hum = (float)H0_rh;

	*humidity = hum + h_temp; // provide signed % measurement unit

	return WE_SUCCESS;
}
/**
* @brief  Read the Temperature data
* @param  Pointer to the Temperature data on °C
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

	T0_out = HIDS_get_T0_OUT();
	T1_out = HIDS_get_T1_OUT();
	T_out = HIDS_get_T_OUT();

	// Decode Temperature

	deg = (float)((int16_t)(T1_degC) - (int16_t)(T0_degC));

	// Calculate Temperature in decimal of degree
	// centigrade i.e. 15.0 = 150.

	t_temp = (float)(((int16_t)T_out - (int16_t)T0_out) * deg) /

			 (float)((int16_t)T1_out - (int16_t)T0_out);

	deg = (float)((int16_t)T0_degC);

	// provide signed celsius measurement unit

	*tempDegC = deg + t_temp;

	return WE_SUCCESS;
}

/**         EOF         */
