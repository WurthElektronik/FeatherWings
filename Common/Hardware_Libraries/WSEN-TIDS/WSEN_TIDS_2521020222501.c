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

#include "WSEN_TIDS_2521020222501.h"
#include <stdio.h>


/**
* @brief  Read the device ID
* @param  Pointer to device ID.
* @retval Error code
*/
int8_t TIDS_getDeviceID(uint8_t *devID)
{
	if (WE_FAIL == ReadReg((uint8_t)TIDS_DEVICE_ID_REG, 1, devID))
	return WE_FAIL;

	return WE_SUCCESS; /* expected value is TIDS_DEVICE_ID_VALUE */

}

/**
* @brief  Set software reset [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t TIDS_setSwReset(TIDS_state_t mode)
{
	TIDS_soft_reset_t swRstReg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_SOFT_RESET_REG, 1, (uint8_t *)&swRstReg))
	return WE_FAIL;

	swRstReg.reset = mode;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_SOFT_RESET_REG, 1, (uint8_t *)&swRstReg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the software reset state [enabled,disabled]
* @param  Pointer to software reset state
* @retval Error code
*/
int8_t TIDS_getSwReset(TIDS_state_t *mode)
{
	TIDS_soft_reset_t swRstReg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_SOFT_RESET_REG, 1, (uint8_t *)&swRstReg))
	return WE_FAIL;
	
	*mode = (TIDS_state_t)swRstReg.reset;
	return WE_SUCCESS;
}

/**
* @brief  Set the free run mode[enabled,disabled]
* @param  mode
* @retval Error code
*/
int8_t TIDS_setContinuousMode(TIDS_state_t mode)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	ctrl_reg.freeRunBit = mode;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}
/**
* @brief  Read the Single Conversion Mode state [enabled,disabled]
* @param  Pointer to Single Conversion Mode state
* @retval Error code
*/
int8_t TIDS_getContinuousMode(TIDS_state_t *mode)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;
	
	*mode = (TIDS_state_t)ctrl_reg.freeRunBit;
	return WE_SUCCESS;
}

/**
* @brief  Set the block data update feature
* @param  state
* @retval Error code
*/
int8_t TIDS_setBlockDataUpdate(TIDS_state_t bdu)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	ctrl_reg.blockDataUpdate = bdu;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the block data update state
* @param  pointer to auto Increment state
* @retval Error code
*/
int8_t TIDS_getBlockDataUpdate(TIDS_state_t *bdu)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;
	
	*bdu = (TIDS_state_t)ctrl_reg.blockDataUpdate;
	return WE_SUCCESS;
}

/**
* @brief  Set the output data rate of the sensor
* @param  output data rate
* @retval Error code
*/
int8_t TIDS_setOutputDataRate(TIDS_output_data_rate_t odr)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	ctrl_reg.outputDataRate = odr;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the output data rate of the sensor
* @param  Pointer to Output Data Rate
* @retval Error code
*/
int8_t TIDS_getOutputDataRate(TIDS_output_data_rate_t* odr)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	*odr = (TIDS_output_data_rate_t)ctrl_reg.outputDataRate;
	return WE_SUCCESS;
}

/**
* @brief  Set the single conversion mode[enabled,disabled]
* @param  mode
* @retval Error code
*/
int8_t TIDS_setSingleConvMode(TIDS_state_t mode)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	ctrl_reg.oneShotbit = mode;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the Single Conversion Mode state [enabled,disabled]
* @param  Pointer to Single Conversion Mode state
* @retval Error code
*/
int8_t TIDS_getSingleConvMode(TIDS_state_t *mode)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;
	
	*mode = (TIDS_state_t)ctrl_reg.oneShotbit;
	return WE_SUCCESS;
}

/**
* @brief  Set the Auto increment feature
* @param  state
* @retval Error code
*/
int8_t TIDS_setAutoIncrement(TIDS_state_t inc)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	ctrl_reg.autoAddIncr = inc;

	if (WE_FAIL == WriteReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the auto increment state
* @param  pointer to auto Increment state
* @retval Error code
*/
int8_t TIDS_getAutoIncrement(TIDS_state_t *inc)
{
	TIDS_ctrl_t ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_CTRL_REG, 1, (uint8_t *)&ctrl_reg))
	return WE_FAIL;
	
	*inc = (TIDS_state_t)ctrl_reg.autoAddIncr;
	return WE_SUCCESS;
}


/**
* @brief  Check if the sensor is busy
* @param  pointer to busy State
* @retval error code
*/
int8_t TIDS_getBusyStatus(TIDS_state_t *state)
{
	TIDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	
	*state = (TIDS_state_t)status_reg.busy;
	return WE_SUCCESS;
}

/**
* @brief  Check if high limit is reached
* @param  pointer to State
* @retval error code
*/
int8_t TIDS_getOverHighLimStatus(TIDS_state_t *state)
{
	TIDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	
	*state = (TIDS_state_t)status_reg.overLimit;
	return WE_SUCCESS;
}

/**
* @brief  Check if low limit is reached
* @param  pointer to State
* @retval error code
*/
int8_t TIDS_getUnderLowLimStatus(TIDS_state_t *state)
{
	TIDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)TIDS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	
	*state = (TIDS_state_t)status_reg.underlimit;
	return WE_SUCCESS;
}

/**
* @brief  Read the raw measured temperature value
* @param  pointer to Temperature Measurement
* @retval Error code
*/
int8_t TIDS_getRAWTemperature(int16_t *rawTemp)
{
	uint8_t tmp[2] = { 0 };

	if (WE_FAIL == ReadReg((uint8_t)TIDS_DATA_T_L_REG, 1, &tmp[0]))
	return WE_FAIL;
	
	if (WE_FAIL == ReadReg((uint8_t)TIDS_DATA_T_H_REG, 1, &tmp[1]))
	return WE_FAIL;

	*rawTemp = (int16_t)(tmp[1] << 8);
	*rawTemp |= (int16_t)tmp[0];
	return WE_SUCCESS;
}

/**
* @brief  Read the measured temperature value in Â°C
* @param  Pointer to Temperature Measurement
* @retval Error code
*/
int8_t TIDS_getTemperature(float *tempdegC)
{
	int16_t rawTemp = 0;
	if(WE_FAIL == TIDS_getRAWTemperature(&rawTemp))
	return WE_FAIL;

	*tempdegC = (float)rawTemp;
	*tempdegC = *tempdegC / 100;
	return WE_SUCCESS;
}

/**
* @brief  Set temperature high limit
* @param  High limit
* @retval Error code
*/
int8_t TIDS_setTempHighLimit(uint8_t hLimit)
{
	if (WE_FAIL == WriteReg((uint8_t)TIDS_LIMIT_T_H_REG, 1, &hLimit))
	return WE_FAIL;

	return WE_SUCCESS;
}
/**
* @brief  Get temperature high limit
* @param  pointer to temperature high limit
* @retval Error code
*/
int8_t TIDS_getTempHighLimit(uint8_t *hLimit)
{
	if (WE_FAIL == ReadReg((uint8_t)TIDS_LIMIT_T_H_REG, 1, hLimit))
	return WE_FAIL;
	
	return WE_SUCCESS;
}


/**
* @brief  Set temperature low limit
* @param  Low limit
* @retval Error code
*/
int8_t TIDS_setTempLowLimit(uint8_t lLimit)
{
	if (WE_FAIL == WriteReg((uint8_t)TIDS_LIMIT_T_L_REG, 1, &lLimit))
	return WE_FAIL;

	return WE_SUCCESS;
}


/**
* @brief  Get temperature low limit
* @param  pointer to temperature low limit
* @retval Error code
*/
int8_t TIDS_getTempLowLimit(uint8_t *lLimit)
{
	if (WE_FAIL == ReadReg((uint8_t)TIDS_LIMIT_T_L_REG, 1, lLimit))
	return WE_FAIL;
	
	return WE_SUCCESS;
}

/**         EOF         */
