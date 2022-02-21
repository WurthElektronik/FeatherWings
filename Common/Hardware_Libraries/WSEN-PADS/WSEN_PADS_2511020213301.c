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
* COPYRIGHT (c) 2019 Würth Elektronik eiSos GmbH & Co. KG
*
***************************************************************************************************
**/

#include "WSEN_PADS_2511020213301.h"
#include <stdio.h>


/**
* @brief  Read the device ID
* @param  Pointer to device ID.
* @retval Error code
*/
int8_t PADS_getDeviceID(uint8_t *devID)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_DEVICE_ID_REG, 1, devID))
	return WE_FAIL;

	return WE_SUCCESS; /* expected value is PADS_DEVICE_ID_VALUE */

}

/**
* @brief  Set the output data rate of the sensor
* @param  output data rate
* @retval Error code
*/
int8_t PADS_setOutputDataRate(PADS_output_data_rate_t odr)
{

	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	ctrl_reg_1.outputDataRate = odr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the output data rate of the sensor
* @param  Pointer to Output Data Rate
* @retval Error code
*/
int8_t PADS_getOutputDataRate(PADS_output_data_rate_t* odr)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	*odr = (PADS_output_data_rate_t)ctrl_reg_1.outputDataRate;
	return WE_SUCCESS;
}

/**
* @brief  Set the low pass filter state
* @param  filter state
* @retval Error code
*/
int8_t PADS_setLowPassFilter(PADS_state_t filterStatus)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	ctrl_reg_1.enLowPassFilter = filterStatus;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Get the status of the low pass filter
* @param  Pointer to Low pass filter state
* @retval Error code
*/
int8_t PADS_getLowPassFilterStatus(PADS_state_t *filterStatus)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;
	*filterStatus = (PADS_state_t)ctrl_reg_1.enLowPassFilter;

	return WE_SUCCESS;
}

/**
* @brief  Set the low pass filter configuration
* @param  configuration parameter
* @retval Error code
*/
int8_t PADS_setLowPassFilterConf(PADS_filter_conf_t conf)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	ctrl_reg_1.lowPassFilterConfig = conf;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	return WE_SUCCESS;
}


/**
* @brief  Read the low pass filter configuration
* @param  Pointer to Low pass filter configuration
* @retval Error code
*/
int8_t PADS_getLowPassFilterConf(PADS_filter_conf_t *conf )
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;
	*conf = (PADS_filter_conf_t)ctrl_reg_1.lowPassFilterConfig;

	return WE_SUCCESS;
}

/**
* @brief  Set the block data update state
* @param  BDU state
* @retval Error code
*/
int8_t PADS_setBlockDataUpdate(PADS_state_t bdu)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	ctrl_reg_1.blockDataUpdate = bdu;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the block data update state
* @param  Pointer to Block data update state
* @retval Error code
*/
int8_t PADS_getBlockDataUpdateState(PADS_state_t *bdu)
{
	PADS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
	return WE_FAIL;

	*bdu = (PADS_state_t)ctrl_reg_1.blockDataUpdate;
	return WE_SUCCESS;
}

/**
* @brief  Set the power mode of the sensor [Low noise , low current]
* @param  Power mode
* @retval Error code
*/
int8_t PADS_setPowerMode(PADS_pwr_mode_t mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.lowNoiseMode = mode;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the power mode [Low noise , low current]
* @param  pointer to Power mode
* @retval Error code
*/
int8_t PADS_getPowerMode(PADS_pwr_mode_t *mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*mode = (PADS_pwr_mode_t)ctrl_reg_2.lowNoiseMode;

	return WE_SUCCESS;
}

/**
* @brief  Set the Auto increment feature
* @param  state
* @retval Error code
*/
int8_t PADS_setAutoIncrement(PADS_state_t autoInc)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.autoAddIncr = autoInc;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the auto increment state
* @param  pointer to auto Increment state
* @retval Error code
*/
int8_t PADS_getAutoIncrementState(PADS_state_t *inc)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*inc = (PADS_state_t)ctrl_reg_2.autoAddIncr;

	return WE_SUCCESS;
}

/**
* @brief  Set memory reboot state
* @param  reboot state
* @retval Error code
*/
int8_t PADS_rebootMemory(PADS_state_t reboot)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.boot = reboot;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the memory reboot state
* @param  Pointer to reboot memory state
* @retval Error code
*/
int8_t PADS_getRebootMemoryState(PADS_state_t *reboot)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*reboot = (PADS_state_t)ctrl_reg_2.boot;

	return WE_SUCCESS;
}

/**
* @brief  Read the boot state
* @param  Pointer to Boot state
* @retval Error code
*/
int8_t PADS_getBootStatus(PADS_state_t *boot)
{
	PADS_intSource_t int_source_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_SOURCE_REG, 1, (uint8_t *)&int_source_reg))
	return WE_FAIL;
	*boot = (PADS_state_t)int_source_reg.bootOn;

	return WE_SUCCESS;
}

/**
* @brief  Set the interrupt active level
* @param  active level
* @retval Error code
*/
int8_t PADS_setIntActiveLevel(PADS_interrupt_level_t intLvl)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.intActiveLevel = intLvl;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the interrupt active level [High,Low]
* @param  Pointer to Interrupt active level
* @retval Error code
*/
int8_t PADS_getIntActiveLevel(PADS_interrupt_level_t *intLvl)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*intLvl = (PADS_interrupt_level_t)ctrl_reg_2.intActiveLevel;

	return WE_SUCCESS;
}

/**
* @brief  Select the interrupt pin type [Push-pull,open_drain]
* @param  pin type
* @retval Error code
*/
int8_t PADS_setIntPinType(PADS_interrupt_pin_t intr)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.openDrainOnINTpin = intr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the interrupt type [Push-pull,open_drain]
* @param  pointer to interrupt pin type
* @retval Error code
*/
int8_t PADS_getIntPinType(PADS_interrupt_pin_t *intr)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*intr = (PADS_interrupt_pin_t)ctrl_reg_2.openDrainOnINTpin;

	return WE_SUCCESS;
}

/**
* @brief  Set the single conversion mode[enabled,disabled]
* @param  mode
* @retval Error code
*/
int8_t PADS_setSingleConvMode(PADS_state_t mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.oneShotbit = mode;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the Single Conversion Mode state [enabled,disabled]
* @param  Pointer to Single Conversion Mode state
* @retval Error code
*/
int8_t PADS_getSingleConvModeState(PADS_state_t *mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*mode = (PADS_state_t)ctrl_reg_2.oneShotbit;

	return WE_SUCCESS;
}

/**
* @brief  Set software reset [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setSwreset(PADS_state_t mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	ctrl_reg_2.softwareReset = mode;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the software reset state [enabled,disabled]
* @param  Pointer to software reset state
* @retval Error code
*/
int8_t PADS_getSwresetState(PADS_state_t *mode)
{
	PADS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
	return WE_FAIL;
	*mode = (PADS_state_t)ctrl_reg_2.softwareReset;

	return WE_SUCCESS;
}

/**
* @brief Set the fifo full interrupt  [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setFifoFullInt(PADS_state_t fullState)
{
	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	ctrl_reg_3.fifoFullInt = fullState;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Set the fifo threshold level interrupt  [enabled,disabled]
* @param  state
* @retval Error code [Error,NoError]
*/
int8_t PADS_setFifoWtmInt(PADS_state_t WtmState)
{
	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	ctrl_reg_3.fifoThresholdInt = WtmState;


	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Enable the fifo overrun interrupt   [enabled,disabled]
* @param  a variable , its Type is state
* @retval Error code [Error,NoError]
*/
int8_t PADS_setFifoOvrInterrupt(PADS_state_t OvrState)
{
	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	ctrl_reg_3.fifoOverrunInt = OvrState;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the fifo full state [enabled,disabled]
* @param  Point to fifo full state
* @retval Error code
*/
int8_t PADS_getFifoFullState(PADS_state_t *fifoFull)
{
	PADS_fifo_status2_t fifo_status2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_STATUS2_REG, 1, (uint8_t *)&fifo_status2))
	return WE_FAIL;
	*fifoFull = (PADS_state_t)fifo_status2.fifoFull;

	return WE_SUCCESS;
}

/**
* @brief  Read if the fifo level has exceeded the user defined watermark[enabled,disabled]
* @param  Pointer to Fifo threshold level state
* @retval Error code
*/
int8_t PADS_getFifoWtmState(PADS_state_t *fifoWtm)
{
	PADS_fifo_status2_t fifo_status2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_STATUS2_REG, 1, (uint8_t *)&fifo_status2))
	return WE_FAIL;
	*fifoWtm = (PADS_state_t)fifo_status2.fifoWtm;

	return WE_SUCCESS;
}

/**
* @brief  Read the fifo overrun  state [enabled,disabled]
* @param  Pointer to fifo overrun state
* @retval Error code
*/
int8_t PADS_getFifoOvrState(PADS_state_t *fifoOvr)
{
	PADS_fifo_status2_t fifo_status2;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_STATUS2_REG, 1, (uint8_t *)&fifo_status2))
	return WE_FAIL;

	*fifoOvr = (PADS_state_t)fifo_status2.fifoOverrun;
	return WE_SUCCESS;
}


/**
* @brief  Set the data ready signal interrupt    [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setDrdyInt(PADS_state_t DRDYStatus)
{

	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	ctrl_reg_3.dataReadyInt = DRDYStatus;


	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the data-ready signal interrupt state [enabled,disabled]
* @param  Pointer to Data-ready signal interrupt state
* @retval Error code
*/
int8_t PADS_getDrdyIntState(PADS_state_t *drdy)
{
	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;
	*drdy = (PADS_state_t)ctrl_reg_3.dataReadyInt;

	return WE_SUCCESS;
}

/**
* @brief  Set the interrupt control event
* @param  event
* @retval Error code
*/
int8_t PADS_setIntEventCtrl(PADS_interrupt_event_t ctr)
{

	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	ctrl_reg_3.intEventCtrl = ctr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the interrupt control event
* @param  Pointer to Interrupt control event
* @retval Error code
*/
int8_t PADS_getIntEventCtrlState(PADS_interrupt_event_t *intEvent)
{
	PADS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)PADS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
	return WE_FAIL;
	*intEvent = (PADS_interrupt_event_t)ctrl_reg_3.intEventCtrl;

	return WE_SUCCESS;
}


/**
* @brief  Check if the temperature data are available
* @param  pointer to temperature data State
* @retval error code
*/
int8_t PADS_getTempStatus(PADS_state_t *state)
{
	PADS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	*state = (PADS_state_t)status_reg.tempDataAvailable;

	return WE_SUCCESS;
}

/**
* @brief  Check if the pressure data are available
* @param  pointer to pressure data State
* @retval error code
*/
int8_t PADS_getPresStatus(PADS_state_t *state)
{
	PADS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	*state = (PADS_state_t)status_reg.presDataAvailable;

	return WE_SUCCESS;
}

/**
* @brief  Check if the temperature data register has been overwritten
* @param  Pointer to temperature overwritten State
* @retval Error code
*/
int8_t PADS_getTemperOverrunStatus(PADS_state_t *state)
{
	PADS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	*state = (PADS_state_t)status_reg.tempDataOverrun;

	return WE_SUCCESS;
}

/**
* @brief  Check if the pressure data register has been overwritten
* @param  Pointer to pressure overwritten State
* @retval Error code
*/
int8_t PADS_getPressureOverrunStatus(PADS_state_t *state)
{
	PADS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_STATUS_REG, 1, (uint8_t *)&status_reg))
	return WE_FAIL;
	*state = (PADS_state_t)status_reg.presDataOverrun;

	return WE_SUCCESS;
}

/**
* @brief Set autorefp function   [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setAutoRefp(PADS_state_t autorep)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.autoRefp = autorep;


	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the autorefp function state [enabled,disabled]
* @param  pointer to autorefp function state
* @retval Error code
*/
int8_t PADS_getAutoRefpState(PADS_state_t *autoRefp)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*autoRefp = (PADS_state_t)interrupt_Configuration_reg.autoRefp;
	return WE_SUCCESS;
}


/**
* @brief  Reset the autorefp function
* @param  state
* @retval Error code
*/
int8_t PADS_resetAutoRefp(PADS_state_t autorep)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.resetAutoRefp = autorep;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}


/**
* @brief Set the autozero function [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setAutoZeroFunction(PADS_state_t autozero)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.autoZeroFunc = autozero;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the autozero function state [enabled,disabled]
* @param  pointer to autozero function state
* @retval Error code
*/
int8_t PADS_getAutoZeroState(PADS_state_t *autoZero)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*autoZero = (PADS_state_t)interrupt_Configuration_reg.autoZeroFunc;
	return WE_SUCCESS;
}

/**
* @brief  Reset autozero function
* @param  state
* @retval Error code
*/
int8_t PADS_resetAutoZeroFunction(PADS_state_t autozeroRst)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.resetAutoZeroFunc = autozeroRst;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Set the differential pressure interrupt [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setDiffPresInt(PADS_state_t diffEn)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.diffInt = diffEn;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;

}

/**
* @brief  Read the differential interrupt state [enabled,disabled]
* @param  pointer to differential interrupt state
* @retval Error code
*/
int8_t PADS_getDiffPresIntState(PADS_state_t *diffIntState)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*diffIntState = (PADS_state_t)interrupt_Configuration_reg.diffInt;
	return WE_SUCCESS;
}

/**
* @brief  Set latched Interrupt  [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setLatchInt(PADS_state_t state)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.latchedInt = state;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the latch Interrupt [enabled,disabled]
* @param  Pointer to latch Interrupt state
* @retval Error code
*/
int8_t PADS_getLatchIntState(PADS_state_t *latchInt)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*latchInt = (PADS_state_t)interrupt_Configuration_reg.latchedInt;
	return WE_SUCCESS;
}

/**
* @brief  Set the interrupt on pressure low event [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setLowPresInt(PADS_state_t state)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.lowPresInt = state;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the interrupt on pressure low event [enabled,disabled]
* @param  Pointer to state of the interrupt on pressure low event
* @retval Error code
*/
int8_t PADS_getLowPresIntState(PADS_state_t *lpint)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*lpint = (PADS_state_t)interrupt_Configuration_reg.lowPresInt;
	return WE_SUCCESS;
}

/**
* @brief  Set the interrupt on pressure High event [enabled,disabled]
* @param  state
* @retval Error code
*/
int8_t PADS_setHighPresInt(PADS_state_t state)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	interrupt_Configuration_reg.highPresInt = state;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the interrupt on pressure High event [enabled,disabled]
* @param  Pointer to state of the interrupt on pressure High event
* @retval Error code
*/
int8_t PADS_getHighPresIntState(PADS_state_t *hpint)
{
	PADS_interrupt_configuration_t interrupt_Configuration_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_CFG_REG, 1, (uint8_t *)&interrupt_Configuration_reg))
	return WE_FAIL;
	*hpint = (PADS_state_t)interrupt_Configuration_reg.highPresInt;
	return WE_SUCCESS;
}


/**
* @brief  Set the LSB pressure threshold value
* @param  value
* @retval Error code
*/

int8_t PADS_setPresThrLSB(uint8_t thr)
{
	uint8_t thresholdLSB = thr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_THR_P_L_REG, 1, &thresholdLSB))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Set the MSB pressure threshold value
* @param  value
* @retval Error code
*/
int8_t PADS_setPresThrMSB(uint8_t thr)
{
	uint8_t thresholdMSB = thr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_THR_P_H_REG, 1, &thresholdMSB))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read LSB threshold pressure value
* @param  Pointer to store threshold pressure LSB value
* @retval Error code
*/
int8_t PADS_getPresThrLSB(uint8_t *thrLSB)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_THR_P_L_REG, 1, thrLSB))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read MSB threshold pressure value
* @param  Pointer to threshold pressure MSB value
* @retval Error code
*/
int8_t PADS_getPresThrMSB(uint8_t *thrMSB)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_THR_P_H_REG, 1, thrMSB))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Disable/enable the I2C Interface
* @param  state
* @retval Error code
*/
int8_t PADS_disI2C(PADS_state_t I2CStatus)
{
	PADS_interface_ctrl_t interface_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	interface_ctrl.disableI2C = I2CStatus;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the I2C interface state [enabled;disabled]
* @param  Pointer to I2C interface state
* @retval Error code
*/
int8_t PADS_getI2CState(PADS_state_t *i2cState)
{
	PADS_interface_ctrl_t interface_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;
	*i2cState = (PADS_state_t)interface_ctrl.disableI2C;
	return WE_SUCCESS;
}


/**
* @brief  Disable/enable the internal pull-down on Interrupt pin
* @param  state
* @retval Error code
*/
int8_t PADS_disPullDownIntPin(PADS_state_t pullDownState)
{
	PADS_interface_ctrl_t interface_ctrl;
	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	interface_ctrl.disPullDownOnIntPin = pullDownState;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the pull down on the interrupt pin [enabled;disabled]
* @param  Pointer to state of the pull down on the interrupt pin
* @retval Error code
*/
int8_t PADS_getPullDownIntPinState(PADS_state_t *pinState)
{
	PADS_interface_ctrl_t interface_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;
	*pinState = (PADS_state_t)interface_ctrl.disPullDownOnIntPin;

	return WE_SUCCESS;
}


/**
* @brief  Set internal pull-up on the SAO pin
* @param  state
* @retval Error code
*/
int8_t PADS_setSAOPullUp(PADS_state_t SaoStatus)
{
	PADS_interface_ctrl_t interface_ctrl;
	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	interface_ctrl.pullUpOnSAOpin = SaoStatus;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the pull up on the SAO pin [enabled;disabled]
* @param  pointer to state of the pull up on the SAO pin
* @retval Error code
*/
int8_t PADS_getSAOPullUpState(PADS_state_t *saoPinState)
{
	PADS_interface_ctrl_t interface_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;
	*saoPinState = (PADS_state_t)interface_ctrl.pullUpOnSAOpin;

	return WE_SUCCESS;

}
/**
* @brief  Set internal pull-up on the SDA pin
* @param  state
* @retval Error code
*/
int8_t PADS_setSDAPullUp(PADS_state_t SdaStatus)
{
	PADS_interface_ctrl_t interface_ctrl;
	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	interface_ctrl.pullUpOnSDApin = SdaStatus;

	if (WE_FAIL == WriteReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the pull up on the SDA pin [enabled;disabled]
* @param  state of the pull up on the SDA pin
* @retval Error code
*/
int8_t PADS_getSDAPullUpState(PADS_state_t *sdaPinState)
{
	PADS_interface_ctrl_t interface_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INTERFACE_CTRL_REG, 1, (uint8_t *)&interface_ctrl))
	return WE_FAIL;
	*sdaPinState = (PADS_state_t)interface_ctrl.pullUpOnSDApin;

	return WE_SUCCESS;
}

/**
* @brief  Set the Fifo mode
* @param  config
* @retval Error code
*/
int8_t PADS_setFifoMode(PADS_fifo_mode_t fifoMode)
{
	PADS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
	return WE_FAIL;

	fifo_ctrl_reg.fifoMode = fifoMode;

	if (WE_FAIL == WriteReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the Fifo mode
* @param  Pointer to Fifo mode Configuration
* @retval Error code
*/
int8_t PADS_getFifoMode(PADS_fifo_mode_t *fifoMode)
{
	PADS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
	return WE_FAIL;
	*fifoMode = (PADS_fifo_mode_t)fifo_ctrl_reg.fifoMode;

	return WE_SUCCESS;
}


/**
* @brief  Set stop on user defined fifo threshold level
* @param  state
* @retval Error code
*/
int8_t PADS_setStopOnThr(PADS_state_t state)
{
	PADS_fifo_ctrl_t fifo_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl))
	return WE_FAIL;

	fifo_ctrl.stopOnThreshold = state;

	if (WE_FAIL == WriteReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the StopOnThresholLevel State
* @param  Pointer to state of the StopOnThresholLevel
* @retval Error code
*/
int8_t PADS_getStopOnThrStatus(PADS_state_t *state)
{
	PADS_fifo_ctrl_t fifo_ctrl;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl))
	return WE_FAIL;
	*state = (PADS_state_t)fifo_ctrl.stopOnThreshold;

	return WE_SUCCESS;
}


/**
* @brief  Set the fifo threshold level
* @param  level
* @retval Error code
*/
int8_t PADS_setFifoThr(uint8_t fifoThr)
{
	PADS_fifo_wtm_t fifo_wtm;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_WTM_REG, 1, (uint8_t *)&fifo_wtm))
	return WE_FAIL;

	fifo_wtm.fifoThreshold = fifoThr;

	if (WE_FAIL == WriteReg((uint8_t)PADS_FIFO_WTM_REG, 1, (uint8_t *)&fifo_wtm))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the fifo threshold level
* @param  pointer to fifo threshold level
* @retval Error code
*/
int8_t PADS_getFifoThr(uint8_t *fifoThr)
{
	PADS_fifo_wtm_t fifo_wtm;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_WTM_REG, 1, (uint8_t *)&fifo_wtm))
	return WE_FAIL;
	*fifoThr = (uint8_t)fifo_wtm.fifoThreshold;

	return WE_SUCCESS;
}

/**
* @brief  Read the fifo fill level
* @param  Pointer to fifo filling level
* @retval Error code
*/
int8_t PADS_getFifoFillLevel(uint8_t *fifoLevel)
{
	;

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_STATUS1_REG, 1, fifoLevel))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the LSB of pressure reference value
* @param  Pointer to LSB pressure reference value
* @retval Error code
*/
int8_t PADS_getPresRefLSB(uint8_t *lowReferenceValue)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_REF_P_H_REG, 1, lowReferenceValue))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the MSB of pressure reference value
* @param  Pointer to MSB pressure reference value
* @retval Error code
*/
int8_t PADS_getPresRefMSB(uint8_t *highReferenceValue)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_REF_P_H_REG, 1, highReferenceValue))
	return WE_FAIL;

	return WE_SUCCESS;
}


/**
* @brief  Set LSB part of the pressure offset value
* @param  a variable,its Type is uint8_t
* @retval Error code
*/
int8_t PADS_setPresOffsetLSB(uint8_t offSet)
{
	uint8_t pressureOffsetLsb = offSet;

	if (WE_FAIL == WriteReg((uint8_t)PADS_OPC_P_L_REG, 1, &pressureOffsetLsb))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the LSB part of the pressure offset value
* @param  Pointer to LSB part of the pressure offset value
* @retval Error code
*/
int8_t PADS_getPresOffsetLSB(uint8_t *offSet)
{

	if (WE_FAIL == ReadReg((uint8_t)PADS_OPC_P_L_REG, 1, offSet))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Set MSB part of the pressure offset value
* @param  a variable,its Type is uint8_t
* @retval Error code [Error,NoError]
*/
int8_t PADS_setPresOffsetMSB(uint8_t highPresureOffsetValue)
{
	uint8_t pressureOffsetMsb = highPresureOffsetValue;

	if (WE_FAIL == WriteReg((uint8_t)PADS_OPC_P_H_REG, 1, &pressureOffsetMsb))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief  Read the MSB part of the pressure offset value
* @param  Pointer to MSB part of the pressure offset value
* @retval Error code
*/
int8_t PADS_getPresOffsetMSB(uint8_t *offSet)
{
	if (WE_FAIL == ReadReg((uint8_t)PADS_OPC_P_H_REG, 1, offSet))
	return WE_FAIL;

	return WE_SUCCESS;
}

/**
* @brief Read the state of the interrupts [Not active , active]
* @param  pointer to state of the interrupt generation
* @retval Error code
*/
int8_t PADS_getIntStatus(PADS_state_t *intState)
{
	PADS_intSource_t int_source;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_SOURCE_REG, 1, (uint8_t *)&int_source))
	return WE_FAIL;
	*intState = (PADS_state_t)int_source.intStatus;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the differential low pressure interrupt [Not active , active]
* @param  Pointer to state of the differential low pressure interrupt
* @retval Error code
*/
int8_t PADS_getLowPresIntStatus(PADS_state_t *lpState)
{
	PADS_intSource_t int_source;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_SOURCE_REG, 1, (uint8_t *)&int_source))
	return WE_FAIL;
	*lpState = (PADS_state_t)int_source.diffPresLowEvent;

	return WE_SUCCESS;
}

/**
* @brief  Read the state of the differential  high pressure interrupt [Not active , active]
* @param  Pointer to state of the differential high pressure interrupt
* @retval No error
*/
int8_t PADS_getHighPresIntStatus(PADS_state_t *hpState)
{
	PADS_intSource_t int_source;

	if (WE_FAIL == ReadReg((uint8_t)PADS_INT_SOURCE_REG, 1, (uint8_t *)&int_source))
	return WE_FAIL;
	*hpState = int_source.diffPresHighEvent;

	return WE_SUCCESS;
}

/**
* @brief  Read the raw measured pressure value
* @param  Pointer to Pressure Measurement
* @retval Error code
*/
int8_t PADS_getRAWPressure(int32_t *rawPres)
{
	uint8_t tmp[3] = { 0 };

	if (WE_FAIL == ReadReg((uint8_t)PADS_DATA_P_XL_REG, 3, tmp))
	return WE_FAIL;

	*rawPres = (int32_t)(tmp[2] << 16);
	*rawPres |= (int32_t)(tmp[1] << 8);
	*rawPres |= (int32_t)(tmp[0]);

	return WE_SUCCESS;
}



/**
* @brief  Read the measured pressure value in kPa
* @param  pointer to Pressure Measurement
* @retval Error code
*/
int8_t PADS_getPressure(float *presskPa)
{
	int32_t rawPressure = 0;
	if(PADS_getRAWPressure(&rawPressure) == WE_SUCCESS)
	{
		*presskPa = (float)rawPressure;
		*presskPa = *presskPa/40960;
	}
	else
	{
		return WE_FAIL;
	}
	return WE_SUCCESS;
}


/**
* @brief  Read the raw measured temperature value
* @param  pointer to Temperature Measurement
* @retval Error code
*/
int8_t PADS_getRAWTemperature(int16_t *rawTemp)
{
	uint8_t tmp[2] = { 0 };

	if (WE_FAIL == ReadReg((uint8_t)PADS_DATA_T_L_REG, 2, tmp))
	return WE_FAIL;

	*rawTemp = (int16_t)(tmp[1] << 8);
	*rawTemp |= (int16_t)tmp[0];

	return WE_SUCCESS;
}

/**
* @brief  Read the measured temperature value in °C
* @param  Pointer to Temperature Measurement
* @retval Error code
*/
int8_t PADS_getTemperature(float *tempdegC)
{
	int16_t rawTemp = 0;
	if(PADS_getRAWTemperature(&rawTemp) == WE_SUCCESS)
	{
		*tempdegC = (float)rawTemp;
		*tempdegC = *tempdegC / 100;
	}
	else
	{
		return WE_FAIL;
	}

	return WE_SUCCESS;
}

/**
* @brief  Read the raw pressure value from Fifo
* @param  Pointer to Fifo Pressure Measurement
* @retval Error code
*/
int8_t PADS_getFifoRAWPressure(int32_t *rawPres)
{
	uint8_t tmp[3] = { 0 };

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_DATA_P_XL_REG, 3, tmp))
	return WE_FAIL;

	*rawPres = (int32_t)(tmp[2] << 16);
	*rawPres |= (int32_t)(tmp[1] << 8);
	*rawPres |= (int32_t)(tmp[0]);

	return WE_SUCCESS;
}
/**
* @brief  Read the pressure value from Fifo in kPa
* @param  Pointer to Fifo Pressure Measurement
* @retval Error code
*/
int8_t PADS_getFifoPressure(float *presskPa)
{
	int32_t rawPressure = 0;
	if(PADS_getFifoRAWPressure(&rawPressure) == WE_SUCCESS)
	{
		*presskPa = (float)rawPressure;
		*presskPa = *presskPa/40960;
	}
	else
	{
		return WE_FAIL;
	}
	return WE_SUCCESS;
}


/**
* @brief  Read the raw temperature value from fifo
* @param  Pointer to Fifo Temperature Measurement
* @retval Error code
*/
int8_t PADS_getFifoRAWTemperature(int16_t *rawTemp)
{
	uint8_t tmp[2] = { 0 };

	if (WE_FAIL == ReadReg((uint8_t)PADS_FIFO_DATA_T_L_REG, 2, tmp))
	return WE_FAIL;

	*rawTemp = (int16_t)(tmp[1] << 8);
	*rawTemp |= (int16_t)tmp[0];

	return WE_SUCCESS;
}

/**
* @brief  Read the temperature value from fifo in °C
* @param  Pointer to Fifo Temperature Measurement
* @retval error code
*/
int8_t PADS_getFifoTemperature(float *tempdegC)
{

	int16_t rawTemp = 0;
	if(PADS_getFifoRAWTemperature(&rawTemp) == WE_SUCCESS)
	{
		*tempdegC = (float)rawTemp;
		*tempdegC = *tempdegC / 100;
	}
	else
	{
		return WE_FAIL;
	}

	return WE_SUCCESS;
}

/**         EOF         */
