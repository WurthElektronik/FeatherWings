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
#include "WSEN_ITDS_2533020201601.h"
#include <stdio.h>

/**
 * @brief  Read the device ID
 * @param  Pointer to Device Id .
 * @retval Error code
 */
int8_t ITDS_getDeviceID(uint8_t *device_id)
{
	uint8_t deviceID = 0;
	if (WE_FAIL == ReadReg((uint8_t)ITDS_DEVICE_ID_REG, 1, &deviceID))
		return WE_FAIL;

	*device_id = deviceID;

	return (WE_SUCCESS);
}

/*CTRL_1 */

/**
 * @brief  Set the output data rate
 * @param  output data rate
 * @retval Error code
 */
int8_t ITDS_setOutputDataRate(ITDS_output_Data_Rate odr)
{

	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.outputDataRate = odr;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the output data rate
 * @param  Pointer to output data rate .
 * @retval Error code
 */
int8_t ITDS_getOutputDataRate(uint8_t *odr)
{
	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*odr = ctrl_reg_1.outputDataRate;
	return (WE_SUCCESS);
}

/**
 * @brief  Set the operating mode
 * @param  operating mode
 * @retval Error code
 */
int8_t ITDS_setOperatingMode(ITDS_operating_mode Mode)
{

	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.operatingMode = Mode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the operating mode
 * @param  Pointer to the operating mode.
 * @retval Error code
 */
int8_t ITDS_getOperatingMode(uint8_t *opMode)
{
	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*opMode = ctrl_reg_1.operatingMode;
	return (WE_SUCCESS);
}

/**
 * @brief  Set the power mode
 * @param  operating mode
 * @retval Error code
 */
int8_t ITDS_setpowerMode(ITDS_power_mode lowOrNormalMode)
{

	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	ctrl_reg_1.powerMode = lowOrNormalMode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the power mode
 * @param  Pointer to the power mode  .
 * @retval Error code
 */
int8_t ITDS_getpowerMode(uint8_t *powerMode)
{
	ITDS_ctrl_1_t ctrl_reg_1;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_1_REG, 1, (uint8_t *)&ctrl_reg_1))
		return WE_FAIL;

	*powerMode = ctrl_reg_1.powerMode;
	return WE_SUCCESS;
}

/*CTRL REG 2*/

/**
 * @brief  Set the Boot state [enabled,disabled]
 * @param  boot state
 * @retval Error code
 */
int8_t ITDS_reboot(ITDS_state_t rebootmode)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.boot = rebootmode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Boot state
 * @param  Pointer to the Boot state .
 * @retval Error code
 */
int8_t ITDS_getBootState(ITDS_state_t *boot)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*boot = (ITDS_state_t)(ctrl_reg_2.boot);
	return WE_SUCCESS;
}

/**
 * @brief  Set software reset [enabled,disabled]
 * @param  software reset state
 * @retval Error code
 */
int8_t ITDS_softReset(ITDS_state_t Swreset_mode)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.softReset = Swreset_mode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the software reset state [enabled,disabled]
 * @param  Pointer to the software reset state.
 * @retval Error code
 */
int8_t ITDS_getsoftResetState(ITDS_state_t *swReset)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*swReset = (ITDS_state_t)(ctrl_reg_2.softReset);
	return (WE_SUCCESS);
}

/**
 * @brief  Disconnect CS pull up [pull up connected ,[pull up disconnected]
 * @param  chip pull up state
 * @retval Error code
 */
int8_t ITDS_ConfigCSPullup(ITDS_state_t CSPullup)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.disCSPullup = CSPullup;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the CS pull up [pull up connected ,[pull up disconnected]
 * @param  Pointer to the CS pull up .
 * @retval Error code
 */
int8_t ITDS_getCSConfigState(ITDS_state_t *configCS)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*configCS = (ITDS_state_t)(ctrl_reg_2.disCSPullup);
	return WE_SUCCESS;
};

/**
 * @brief  Set the block data update state
 * @param  BDU state
 * @retval Error code
 */
int8_t ITDS_setBlockDataUpdate(ITDS_state_t bdu)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.blockDataUpdate = bdu;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the block data update state
 * @param  Pointer to the block data update state
 * @retval Error code
 */
int8_t ITDS_getBlockDataUpdateState(ITDS_state_t *bdu)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;
	*bdu = (ITDS_state_t)(ctrl_reg_2.blockDataUpdate);
	return WE_SUCCESS;
}

/**
 * @brief  Set the Auto increment feature
 * @param  Auto increment state
 * @retval Error code
 */
int8_t ITDS_setAutoIncrement(ITDS_state_t autoInc)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.autoAddIncr = autoInc;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the auto increment state
 * @param  Pointer to the auto increment state
 * @retval Error code
 */
int8_t ITDS_getAutoIncrementState(ITDS_state_t *autoIncr)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*autoIncr = (ITDS_state_t)(ctrl_reg_2.autoAddIncr);
	return WE_SUCCESS;
}

/**
 * @brief  Disable the I2C Interface
 * @param  I2C Interface state
 * @retval Error code
 */
int8_t ITDS_disableI2CInterface(ITDS_state_t I2CStatus)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	ctrl_reg_2.i2cDisable = I2CStatus;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the I2C interface state [enabled;disabled]
 * @param  Pointer to the I2C interface state.
 * @retval Error code
 */
int8_t ITDS_getI2CInterfaceState(ITDS_state_t *i2cState)
{
	ITDS_ctrl_2_t ctrl_reg_2;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_2_REG, 1, (uint8_t *)&ctrl_reg_2))
		return WE_FAIL;

	*i2cState = (ITDS_state_t)(ctrl_reg_2.i2cDisable);
	return WE_SUCCESS;
}

/*CTRL REG 3*/

/**
 * @brief  set the self test mode
 * @param  self test mode
 * @retval Error code
 */
int8_t ITDS_setSelfTestMode(ITDS_self_Test_Config selfTest)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.selfTestMode = selfTest;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the self test mode
 * @param  Pointer to the self test mode.
 * @retval Error code
 */
int8_t ITDS_getSelfTestMode(ITDS_state_t *selfTest)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*selfTest = (ITDS_state_t)(ctrl_reg_3.selfTestMode);
	return WE_SUCCESS;
}

/**
 * @brief  set the Interrupt Type [Push-pull/open-drain]
 * @param  interrupt Type
 * @retval Error code
 */
int8_t ITDS_setIntPinType(ITDS_interrupt_Pin_Config interruptType)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.intPinConf = interruptType;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Interrupt Type [Push-pull/open-drain]
 * @param  Pointer to the Interrupt Type .
 * @retval Error code
 */
int8_t ITDS_getIntPinType(ITDS_state_t *intType)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*intType = (ITDS_state_t)(ctrl_reg_3.intPinConf);
	return WE_SUCCESS;
}

/**
 * @brief  set the latched Interrupt
 * @param  latch Interrupt state
 * @retval Error code
 */
int8_t ITDS_enLatchInt(ITDS_state_t lir)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.enLatchedInterrupt = lir;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the latch Interrupt state [enabled,disabled]
 * @param  Pointer to the latch Interrupt state .
 * @retval Error code
 */
int8_t ITDS_getLatchIntState(ITDS_state_t *lir)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*lir = (ITDS_state_t)(ctrl_reg_3.enLatchedInterrupt);
	return WE_SUCCESS;
}

/**
 * @brief  set the Interrupt active level [active high/active low]
 * @param  interrupt active level
 * @retval Error code
 */
int8_t ITDS_setIntActiveLevel(ITDS_interrupt_Active_Level lowHighLevel)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.intActiveLevel = lowHighLevel;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Interrupt active level
 * @param  Pointer to the Interrupt active level.
 * @retval Error code
 */
int8_t ITDS_getIntActiveLevel(ITDS_interrupt_Active_Level *lowHighLevel)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*lowHighLevel = (ITDS_interrupt_Active_Level)(ctrl_reg_3.intActiveLevel);
	return WE_SUCCESS;
}

/**
 * @brief  select the Single data conversion [0:enabled by external trigger signal on INT_1,1:enabled by I2C writing]
 * @param  single data conversion Demand mode
 * @retval Error code
 */
int8_t ITDS_setSingleConvInput(ITDS_singleDataConversion_Input slpModeSel)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.enSingleDataConv = slpModeSel;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the single data conversion Demand mode
 * @param  Pointer to single data conversion Demand mode .
 * @retval Error code
 */
int8_t ITDS_getSingleConvInput(ITDS_singleDataConversion_Input *slpModeSel)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*slpModeSel = (ITDS_singleDataConversion_Input)(ctrl_reg_3.enSingleDataConv);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Single Data Conversion Mode
 * @param  Single Data Conversion Mode
 * @retval Error code
 */
int8_t ITDS_enSingleDataConv(ITDS_state_t slpMode1)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	ctrl_reg_3.singleConvTrigger = slpMode1;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the single data conversion Demand mode
 * @param  Pointer to the single data conversion Demand mode.
 * @retval Error code
 */
int8_t ITDS_getSingleDataConvState(ITDS_state_t *slpMode1)
{
	ITDS_ctrl_3_t ctrl_reg_3;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_3_REG, 1, (uint8_t *)&ctrl_reg_3))
		return WE_FAIL;

	*slpMode1 = (ITDS_state_t)(ctrl_reg_3.singleConvTrigger);
	return WE_SUCCESS;
}

/*CTRL REG 4  */

/**
 * @brief  enable the 6D interrupt on INT_0
 * @param  the 6D interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_set6DOnINT0(ITDS_state_t int06D)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.sixDINT0 = int06D;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the the 6D interrupt state on INT_0
 * @param  Pointer to the 6D interrupt state on INT_0.
 * @retval Error code
 */
int8_t ITDS_get6DOnINT0State(ITDS_state_t *int06D)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int06D = (ITDS_state_t)(ctrl_reg_4.sixDINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Single-tap interrupt on INT_0
 * @param  Single-tap interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_enSingleTapINT0(ITDS_state_t int0SingleTap)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.singleTapINT0 = int0SingleTap;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Single-tap interrupt state on INT_0
 * @param  Pointer to the Single-tap interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_getSingleTapINT0(ITDS_state_t *int0SingleTap)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0SingleTap = (ITDS_state_t)(ctrl_reg_4.singleTapINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Wakeup interrupt  on INT_0
 * @param   Wakeup interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_enWakeupOnINT0(ITDS_state_t int0Wakeup)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.wakeupINT0 = int0Wakeup;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Wakeup interrupt state  on INT_0
 * @param  Pointer to the Wakeup interrupt state  on INT_0 .
 * @retval Error code
 */
int8_t ITDS_getWakeupOnINT0(ITDS_state_t *int0Wakeup)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0Wakeup = (ITDS_state_t)(ctrl_reg_4.wakeupINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable Free-fall interrupt on INT_0
 * @param  Free-fall interrupt state  on INT_0
 * @retval Error code
 */
int8_t ITDS_enFreeFallINT0(ITDS_state_t int0FreeFall)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.freeFallINT0 = int0FreeFall;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Free-fall interrupt State on INT_0
 * @param  Pointer to the Free-fall State on INT_0.
 * @retval Error code
 */
int8_t ITDS_getFreeFallINT0(ITDS_state_t *int0FreeFall)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0FreeFall = (ITDS_state_t)(ctrl_reg_4.freeFallINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Double-tap interrupton INT_0
 * @param  the Double-tap state interrupt on INT_0
 * @retval Error code
 */
int8_t ITDS_enDoubleTapINT0(ITDS_state_t int0DoubleTap)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.doubleTapINT0 = int0DoubleTap;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Double-tap interruptstate  on INT_0
 * @param  Pointer to the Double-tap interrupt state  on INT_0
 * @retval Error code
 */
int8_t ITDS_getDoubleTapINT0(ITDS_state_t *int0DoubleTap)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0DoubleTap = (ITDS_state_t)(ctrl_reg_4.doubleTapINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the FIFO full interrupton INT_0
 * @param  FIFO full interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_enFifoFullINT0(ITDS_state_t int0FifoFull)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.fifoFullINT0 = int0FifoFull;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO full interrupt State on INT_0
 * @param  Pointer to the FIFO full interrupt State on INT_0
 * @retval Error code
 */
int8_t ITDS_getFifoFullINT0(ITDS_state_t *int0FifoFull)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0FifoFull = (ITDS_state_t)(ctrl_reg_4.fifoFullINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the FIFO threshold interrupt  on INT_0
 * @param  FIFO threshold interrupt state on INT_0
 * @retval Error code
 */
int8_t ITDS_enFifoThresholdINT0(ITDS_state_t int0FifoThresholdInt)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.fifoThresholdINT0 = int0FifoThresholdInt;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO threshold interrupt State on INT_0
 * @param  Pointer to the FIFO threshold interrupt State on INT_0
 * @retval Error code
 */
int8_t ITDS_getFifoThresholdINT0(ITDS_state_t *int0FifoThresholdInt)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0FifoThresholdInt = (ITDS_state_t)(ctrl_reg_4.fifoThresholdINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Data-Ready interrupt on INT_0
 * @param  Data-Ready interrupt on INT_0 State
 * @retval Error code
 */
int8_t ITDS_enDataReadyINT0(ITDS_state_t int0DataReadyInt)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	ctrl_reg_4.dataReadyINT0 = int0DataReadyInt;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Data-Ready interrupt on INT_0 State
 * @param  Pointer to Data-Ready interrupt on INT_0 State.
 * @retval Error code
 */
int8_t ITDS_getDataReadyINT0(ITDS_state_t *int0DataReadyInt)
{
	ITDS_ctrl_4_t ctrl_reg_4;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_4_REG, 1, (uint8_t *)&ctrl_reg_4))
		return WE_FAIL;

	*int0DataReadyInt = (ITDS_state_t)(ctrl_reg_4.dataReadyINT0);
	return WE_SUCCESS;
}

/*CTRL REG 5*/

/**
 * @brief  enable Sleep state on INT_1
 * @param  Sleep state on INT_1
 * @retval Error code
 */
int8_t ITDS_enInactivityINT1(ITDS_state_t intSleepState)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.inactiveStatusINT1 = intSleepState;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Sleep  state on INT_1
 * @param  Pointer to the Sleep  state on INT_1.
 * @retval Error code
 */
int8_t ITDS_getInactivityINT1(ITDS_state_t *intSleepState)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*intSleepState = (ITDS_state_t)(ctrl_reg_5.inactiveStatusINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable Sleep change signal on INT_1
 * @param  Sleep change state signal on INT_1
 * @retval Error code
 */
int8_t ITDS_enActivityINT1(ITDS_state_t int1SleepChange)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.activeStatusINT1 = int1SleepChange;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Sleep change state on INT_1
 * @param  Pointer to the Sleep change state on INT_1.
 * @retval Error code
 */
int8_t ITDS_getActivityINT1(ITDS_state_t *int1SleepChange)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1SleepChange = (ITDS_state_t)(ctrl_reg_5.activeStatusINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Boot Interrupt on INT_1
 * @param  Boot Interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_enBootStatusINT1(ITDS_state_t int1Boot)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.bootStatusINT1 = int1Boot;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Boot Interrupt state on INT_1
 * @param  Pointer to Boot Interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_getBootStatusINT1(ITDS_state_t *int1Boot)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1Boot = (ITDS_state_t)(ctrl_reg_5.bootStatusINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Temperature Data-Ready interrupt on INT_1
 * @param  the Temperature Data-Ready interrupt State on INT_1
 * @retval Error code
 */
int8_t ITDS_enTempDataReadyINT1(ITDS_state_t int1TempDataReady)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.tempDataReadyINT1 = int1TempDataReady;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Temperature Data-Ready interrupt State on INT_1
 * @param  Pointer to the Temperature Data-Ready interrupt State on INT_1 .
 * @retval Error code
 */
int8_t ITDS_getTempDataReadyINT1(ITDS_state_t *int1TempDataReady)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1TempDataReady = (ITDS_state_t)(ctrl_reg_5.tempDataReadyINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the FIFO Overrun interrupt  on INT_1
 * @param  FIFO Overrun interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_enFifoOverrunIntINT1(ITDS_state_t int1FifoOverrun)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.fifoOverrunINT1 = int1FifoOverrun;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO Overrun interrupt state on INT_1
 * @param  Pointer to the FIFO Overrun interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_getFifoOverrunIntINT1(ITDS_state_t *int1FifoOverrun)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1FifoOverrun = (ITDS_state_t)(ctrl_reg_5.fifoOverrunINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the FIFO full interrupt on INT_1
 * @param  FIFO full interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_enFifoFullINT1(ITDS_state_t int1FifoFull)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.fifoFullINT1 = int1FifoFull;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO full interrupt state on INT_1
 * @param  Pointer to the FIFO full interrupt state on INT_1
 * @retval Error code
 */
int8_t ITDS_getFifoFullINT1(ITDS_state_t *int1FifoFull)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1FifoFull = (ITDS_state_t)(ctrl_reg_5.fifoFullINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the FIFO threshold interrupt on INT_1
 * @param  FIFO threshold interrupt state  on INT_1
 * @retval Error code
 */
int8_t ITDS_enFifoThresholdIntINT1(ITDS_state_t int1FifoThresholdInt)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.fifoThresholdINT1 = int1FifoThresholdInt;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO threshold interrupt State on INT_1
 * @param  Pointer to the FIFO threshold interrupt State on INT_1 .
 * @retval Error code
 */
int8_t ITDS_getFifoThresholdINT1(ITDS_state_t *int1FifoThresholdInt)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1FifoThresholdInt = (ITDS_state_t)(ctrl_reg_5.fifoThresholdINT1);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Data-Ready interrupt on INT_1
 * @param  Data-Ready interrupt State on INT_1
 * @retval Error code
 */
int8_t ITDS_enDataReadyINT1(ITDS_state_t int1DataReadyInt)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	ctrl_reg_5.dataReadyINT1 = int1DataReadyInt;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Data-Ready interrupt state on INT_1
 * @param  Pointer to the Data-Ready interrupt signal on INT_1
 * @retval Error code
 */
int8_t ITDS_getDataReadyINT1(ITDS_state_t *int1DataReadyInt)
{
	ITDS_ctrl_5_t ctrl_reg_5;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_5_REG, 1, (uint8_t *)&ctrl_reg_5))
		return WE_FAIL;

	*int1DataReadyInt = (ITDS_state_t)(ctrl_reg_5.dataReadyINT1);
	return WE_SUCCESS;
}

/*CTRL REG 6*/

/**
 * @brief  set  the Filtering Cut-off
 * @param  Bandwith
 * @retval Error code
 */
int8_t ITDS_setFilteringCutoff(ITDS_bandwidth filteringCutoff)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	ctrl_reg_6.filterBandwidth = filteringCutoff;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read Filtering Cut-off
 * @param  Pointer to the Filtering Cut-off .
 * @retval Error code
 */
int8_t ITDS_getFilteringCutoff(ITDS_bandwidth *filteringCutoff)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	*filteringCutoff = (ITDS_bandwidth)(ctrl_reg_6.filterBandwidth);
	return WE_SUCCESS;
}

/**
 * @brief  set the Full scale
 * @param  Full scale
 * @retval Error code
 */
int8_t ITDS_setFullScale(ITDS_full_Scale fullScale_t)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	ctrl_reg_6.fullScale = fullScale_t;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Full scale
 * @param  Pointer to the Full scale.
 * @retval Error code
 */
int8_t ITDS_getFullScale(ITDS_full_Scale *fullScale_t)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	*fullScale_t = (ITDS_full_Scale)(ctrl_reg_6.fullScale);
	return WE_SUCCESS;
}

/**
 * @brief  set the Filtered data type [low-pass filter/High-pass filter]
 * @param  pass filter Type
 * @retval Error code
 */
int8_t ITDS_setfilterPath(ITDS_filter_Type filterType)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	ctrl_reg_6.filterPath = filterType;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the pass filter Type
 * @param  Pointer to the pass filter Type  .
 * @retval Error code
 */
int8_t ITDS_getfilterPath(ITDS_filter_Type *filterType)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	*filterType = (ITDS_filter_Type)((ctrl_reg_6.filterPath));
	return WE_SUCCESS;
}

/**
 * @brief  enable low noise configuration
 * @param  low noise state
 * @retval Error code
 */
int8_t ITDS_enLowNoise(ITDS_state_t lowNoise)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	ctrl_reg_6.enLowNoise = lowNoise;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the low noise configuration state
 * @param  Pointer to the low noise configuration state
 * @retval Error code
 */
int8_t ITDS_getLowNoiseState(ITDS_state_t *lowNoise)
{
	ITDS_ctrl_6_t ctrl_reg_6;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_6_REG, 1, (uint8_t *)&ctrl_reg_6))
		return WE_FAIL;

	*lowNoise = (ITDS_state_t)(ctrl_reg_6.enLowNoise);
	return WE_SUCCESS;
}

/*STATUS REG 0*27*/

/**
 * @brief  Read the Data-ready state
 * @param  Pointer to the Data-ready state.
 * @retval Error code
 */
int8_t ITDS_getdataReadyState(ITDS_state_t *drdy)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*drdy = (ITDS_state_t)(status_reg.dataReady);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Free-fall event state   [not detected/detected]
 * @param  Pointer to the Free-fall event state.
 * @retval Error code
 */
int8_t ITDS_getfreeFallState(ITDS_state_t *freeFall)
{
	ITDS_status_t status_reg;
	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*freeFall = (ITDS_state_t)(status_reg.freeFall);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Wake-up Event detection Status
 * @param  Pointer to the Wake-up Event detection State.
 * @retval Error code
 */
int8_t ITDS_getFreeFallEventState(ITDS_state_t *freefall)
{
	ITDS_wake_up_event_t wake_up_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_EVENT_REG, 1, (uint8_t *)&wake_up_event_t))
		return WE_FAIL;

	*freefall = (ITDS_state_t)(wake_up_event_t.freeFallState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the 6D change in position state   [not detected/detected]
 * @param  Pointer to the 6D change in position state.
 * @retval Error code
 */
int8_t ITDS_get6DState(ITDS_state_t *sixD)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*sixD = (ITDS_state_t)(status_reg.sixDDetection);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Single-tap event state   [not detected/detected]
 * @param  Pointer to the single-tap event state.
 * @retval Error code
 */
int8_t ITDS_getsingleTapState(ITDS_state_t *singleTap)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*singleTap = (ITDS_state_t)(status_reg.singleTap);
	return WE_SUCCESS;
}

/**
 * @brief  Read the double-tap event state   [not detected/detected]
 * @param  Pointer to the double-tap event state
 * @retval Error code
 */
int8_t ITDS_getdoubleTapState(ITDS_state_t *doubleTap)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*doubleTap = (ITDS_state_t)(status_reg.doubleTap);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Sleep event state   [not detected/detected]
 * @param  Pointer to the Sleep event state.
 * @retval Error code
 */
int8_t ITDS_getsleepState(ITDS_state_t *sleepState)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*sleepState = (ITDS_state_t)(status_reg.sleep);
	return WE_SUCCESS;
}

/**
 * @brief  Read the wakeup event state   [not detected/detected]
 * @param  Pointer to the wakeup event state.
 * @retval Error code
 */
int8_t ITDS_getwakeupState(ITDS_state_t *wakeupState)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*wakeupState = (ITDS_state_t)(status_reg.wakeup);
	return WE_SUCCESS;
}

/**
 * @brief  Read the fifo Threshold event state   [not detected/detected]
 * @param  Pointer to the fifo Threshold event state.
 * @retval Error code
 */
int8_t ITDS_getFifoThresholdStatus(ITDS_state_t *fifoTHR)
{
	ITDS_status_t status_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_REG, 1, (uint8_t *)&status_reg))
		return WE_FAIL;

	*fifoTHR = (ITDS_state_t)(status_reg.fifoThreshold);
	return WE_SUCCESS;
}

/*X_OUT_L_REG */

/**
 * @brief  Read the Raw X-axis acceleration sensor output
 * @param  Pointer to the Raw X-axis acceleration
 * @retval Error code
 */
int8_t ITDS_getRawAccelerationX(int16_t *XRawAcc)
{

	int16_t XAxisAccelerationRaw = 0;
	uint8_t tmp[2] = {0};

	if (WE_FAIL == ReadReg((uint8_t)ITDS_X_OUT_L_REG, 2, tmp))
		return WE_FAIL;

	XAxisAccelerationRaw = (int16_t)(tmp[1] << 8);
	XAxisAccelerationRaw |= (int16_t)tmp[0];

	*XRawAcc = XAxisAccelerationRaw;
	return WE_SUCCESS;
}

/*Y_OUT_L_REG */

/**
 * @brief  Read the Raw Y-axis acceleration sensor output
 * @param  Pointer to the Raw Y-axis acceleration.
 * @retval Error code
 */
int8_t ITDS_getRawAccelerationY(int16_t *YRawAcc)
{

	int16_t YAxisAcceleration = 0;
	uint8_t tmp[2] = {0};

	if (WE_FAIL == ReadReg((uint8_t)ITDS_Y_OUT_L_REG, 2, tmp))
		return WE_FAIL;

	YAxisAcceleration = (int16_t)(tmp[1] << 8);
	YAxisAcceleration |= (int16_t)tmp[0];

	*YRawAcc = YAxisAcceleration;
	return WE_SUCCESS;
}
/*Z_OUT_L_REG */

/**
 * @brief  Read the Raw Z-axis acceleration sensor output
 * @param  Pointer to the Raw Z-axis acceleration .
 * @retval Error code
 */
int8_t ITDS_getRawAccelerationZ(int16_t *ZRawAcc)
{

	int16_t ZAxisAcceleration = 0;
	uint8_t tmp[2] = {0};

	if (WE_FAIL == ReadReg((uint8_t)ITDS_Z_OUT_L_REG, 2, tmp))
		return WE_FAIL;

	ZAxisAcceleration = (int16_t)(tmp[1] << 8);
	ZAxisAcceleration |= (int16_t)tmp[0];

	*ZRawAcc = ZAxisAcceleration;
	return WE_SUCCESS;
}

/*ITDS_T_OUT_REG*/
/**
 * @brief  Read the 8 bit Temperature
 * @param  Pointer to the Temperature
 * @retval Error code
 */

int8_t ITDS_getTemperature8bit(uint8_t *temp8bit)
{

	uint8_t temperatureValue8bit;
	if (WE_FAIL == ReadReg((uint8_t)ITDS_T_OUT_REG, 1, &temperatureValue8bit))
		return WE_FAIL;

	*temp8bit = temperatureValue8bit;
	return (WE_SUCCESS);
}

/**
 * @brief  Read the 12 bit Temperature
 * @param  Pointer to the Temperature
 * @retval Error code
 */
int8_t ITDS_getRawTemp12bit(int16_t *temp12bit)
{

	uint8_t temp[2] = {0};

	if (WE_FAIL == ReadReg((uint8_t)ITDS_T_OUT_L_REG, 1, temp))
		return WE_FAIL;
	if (WE_FAIL == ReadReg((uint8_t)ITDS_T_OUT_H_REG, 1, temp + 1))
		return WE_FAIL;

	*temp12bit = (int16_t)(temp[1] << 8);
	*temp12bit |= (int16_t)temp[0];

	*temp12bit = (*temp12bit) >> 4;

	return (WE_SUCCESS);
}

/**
 * @brief  Read the 12 bit Temperature in °C
 * @param  Pointer to the Temperature
 * @retval Error code
 */
int8_t ITDS_getTemperature12bit(float *tempdegC)
{
	int16_t rawTemp = 0;
	if (ITDS_getRawTemp12bit(&rawTemp) == WE_SUCCESS)
	{
		*tempdegC = (((float)rawTemp) / 16.0) + 25.0;
	}
	else
	{
		return WE_FAIL;
	}
	return WE_SUCCESS;
}

/*FIFO_CTRL (0x2E)*/
/**
 * @brief  set the Fifo Threshold of the sensor
 * @param  Fifo Threshold
 * @retval Error code
 */
int8_t ITDS_setFifoThreshold(uint8_t fifo_Threshold)
{
	ITDS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	fifo_ctrl_reg.fifoThresholdLevel = fifo_Threshold;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Fifo Threshold
 * @param  Pointer to the Fifo Threshold
 * @retval Error code
 */
int8_t ITDS_getFifoThreshold(uint8_t *fifo_Threshold)
{
	ITDS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	*fifo_Threshold = fifo_ctrl_reg.fifoThresholdLevel;
	return WE_SUCCESS;
}

/**
 * @brief  set the Fifo Mode
 * @param  Fifo Mode
 * @retval Error code
 */
int8_t ITDS_setFifoMode(ITDS_Fifo_Mode FMode)
{
	ITDS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	fifo_ctrl_reg.fifoMode = FMode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Fifo Mode
 * @param  Pointer to the Fifo Mode
 * @retval Error code
 */
int8_t ITDS_getFifoMode(ITDS_Fifo_Mode *FMode)
{
	ITDS_fifo_ctrl_t fifo_ctrl_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_CTRL_REG, 1, (uint8_t *)&fifo_ctrl_reg))
		return WE_FAIL;

	*FMode = (ITDS_Fifo_Mode)(fifo_ctrl_reg.fifoMode);
	return WE_SUCCESS;
}

/*FIFO_SAMPLES (0x2F)*/

/**
 * @brief  Read the FIFO threshold state  [FIFO filling is lower than threshold level/FIFO filling is equal to or higher than the threshold level]
 * @param  Pointer to the FIFO threshold state
 * @retval Error code
 */
int8_t ITDS_getFifoThresholdState(ITDS_state_t *fifoTHR)
{
	ITDS_fifo_samples_t fifo_samples_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_SAMPLES_REG, 1, (uint8_t *)&fifo_samples_reg))
		return WE_FAIL;

	*fifoTHR = (ITDS_state_t)(fifo_samples_reg.fifoThresholdState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO overrun state  [FIFO is not completely filled,/FIFO completely filled and at least one sample has been overwritten]
 * @param  Pointer to the FIFO overrun state .
 * @retval Error code
 */
int8_t ITDS_getFifoOverrunState(ITDS_state_t *fifoOverrun)
{
	ITDS_fifo_samples_t fifo_samples_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_SAMPLES_REG, 1, (uint8_t *)&fifo_samples_reg))
		return WE_FAIL;

	*fifoOverrun = (ITDS_state_t)(fifo_samples_reg.fifoOverrunState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the FIFO Fill Level
 * @param  pointer to the FIFO Fill Level
 * @retval Error code
 */
int8_t ITDS_getFifoFillLevel(uint8_t *fifoFill)
{
	ITDS_fifo_samples_t fifo_samples_reg;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FIFO_SAMPLES_REG, 1, (uint8_t *)&fifo_samples_reg))
		return WE_FAIL;

	*fifoFill = (fifo_samples_reg.fifoFillLevel);
	return WE_SUCCESS;
}

/*TAP_X_TH (0x30)*/

/**
 * @brief  enable the 4D detection position
 * @param  the 4D detection position state
 * @retval Error code
 */
int8_t ITDS_en4DDetection(ITDS_state_t detection4D)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	tap_x_th.fourDPositionState = detection4D;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the 4D detection position state
 * @param  Pointer to the 4D detection position state.
 * @retval Error code
 */
int8_t ITDS_get4DDetectionState(ITDS_state_t *detection4D)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	*detection4D = (ITDS_state_t)(tap_x_th.fourDPositionState);
	return WE_SUCCESS;
}

/**
 * @brief  set the TAP Threshold on X Axis
 * @param  Tap Threshold X axis
 * @retval Error code
 */
int8_t ITDS_setTapThresholdX(uint8_t tapThresholdX)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	tap_x_th.XAxisTapThreshold = tapThresholdX;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Threshold X axis
 * @param  Pointer to the Tap Threshold X axis
 * @retval Error code
 */
int8_t ITDS_getTapThresholdX(uint8_t *tapThresholdX)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	*tapThresholdX = (ITDS_state_t)(tap_x_th.XAxisTapThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  set the 6D Threshold (degrees)
 * @param  6D Threshold (degrees)
 * @retval Error code
 */
int8_t ITDS_set6DThreshold(ITDS_threshold_Degree threshold6D)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	tap_x_th.sixDThreshold = threshold6D;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the 6D Threshold   (degrees)
 * @param  Pointer to the 6D Threshold
 * @retval Error code
 */
int8_t ITDS_get6DThreshold(ITDS_threshold_Degree *threshold6D)
{
	ITDS_tap_x_th_t tap_x_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_X_TH_REG, 1, (uint8_t *)&tap_x_th))
		return WE_FAIL;

	*threshold6D = (ITDS_threshold_Degree)(tap_x_th.sixDThreshold);
	return WE_SUCCESS;
}

/*TAP_Y_TH (0x31)*/

/**
 * @brief  set the TAP Threshold on Y Axis
 * @param  Tap Threshold Y axis
 * @retval Error code
 */
int8_t ITDS_setTapThresholdY(uint8_t tapThresholdY)
{
	ITDS_tap_y_th_t tap_y_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	tap_y_th.YAxisTapThreshold = tapThresholdY;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Threshold Y axis
 * @param  Pointer to the Tap Threshold Y axis.
 * @retval Error code
 */
int8_t ITDS_getTapThresholdY(uint8_t *tapThresholdY)
{
	ITDS_tap_y_th_t tap_y_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	*tapThresholdY = (tap_y_th.YAxisTapThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  set the Axis Tap detection priority
 * @param  Axis Tap detection priority
 * @retval Error code
 */
int8_t ITDS_setTapAxisPriority(ITDS_tap_Axis_Priority tapPriorityAxis)
{
	ITDS_tap_y_th_t tap_y_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	tap_y_th.tapAxisPriority = tapPriorityAxis;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Axis Tap detection priority
 * @param  Pointer to the Axis Tap detection priority
 * @retval Error code
 */
int8_t ITDS_getTapAxisPriority(ITDS_tap_Axis_Priority *tapPriorityAxis)
{
	ITDS_tap_y_th_t tap_y_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Y_TH_REG, 1, (uint8_t *)&tap_y_th))
		return WE_FAIL;

	*tapPriorityAxis = (ITDS_tap_Axis_Priority)(tap_y_th.tapAxisPriority);
	return WE_SUCCESS;
}

/* TAP_Z_TH (0x32) */

/**
 * @brief  set the TAP Threshold on Z Axis
 * @param  Tap Threshold Z axis
 * @retval Error code
 */
int8_t ITDS_setTapThresholdZ(uint8_t tapThresholdZ)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	tap_z_th.ZAxisTapThreshold = tapThresholdZ;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Threshold Z axis
 * @param  Pointer to the Tap Threshold Z axis.
 * @retval Error code
 */
int8_t ITDS_getTapThresholdZ(uint8_t *tapThresholdZ)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	*tapThresholdZ = (tap_z_th.ZAxisTapThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Enables X direction in tap recognition
 * @param  tap X direction  State
 * @retval Error code
 */
int8_t ITDS_enTapX(ITDS_state_t tapX)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	tap_z_th.enTapX = tapX;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the X direction in tap recognition State
 * @param  Pointer to tap X direction  State.
 * @retval Error code
 */
int8_t ITDS_getTapX(ITDS_state_t *tapX)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	*tapX = (ITDS_state_t)(tap_z_th.enTapX);
	return WE_SUCCESS;
}

/**
 * @brief  Enables Y direction in tap recognition
 * @param  tap Y direction  State
 * @retval Error code
 */
int8_t ITDS_enTapY(ITDS_state_t tapY)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	tap_z_th.enTapY = tapY;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Y direction in tap recognition State
 * @param  Pointer to tap Y direction  State.
 * @retval Error code
 */
int8_t ITDS_getTapY(ITDS_state_t *tapY)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	*tapY = (ITDS_state_t)(tap_z_th.enTapY);
	return WE_SUCCESS;
}

/**
 * @brief  Enables the tap Z direction
 * @param  tap Z direction  State
 * @retval Error code
 */
int8_t ITDS_enTapZ(ITDS_state_t tapZ)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	tap_z_th.enTapZ = tapZ;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the tap Z direction state
 * @param  Pointer to the tap Z direction state
 * @retval Error code
 */
int8_t ITDS_getTapZ(ITDS_state_t *tapZ)
{
	ITDS_tap_z_th_t tap_z_th;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_Z_TH_REG, 1, (uint8_t *)&tap_z_th))
		return WE_FAIL;

	*tapZ = (ITDS_state_t)(tap_z_th.enTapZ);
	return WE_SUCCESS;
}

/* INT_DUR (0x33) */

/**
 * @brief  set the maximum duration time gap for double-tap recognition (LATENCY)
 * @param  Latency value
 * @retval Error code
 */
int8_t ITDS_setLatency(uint8_t latency_t)
{
	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	int_dur_t.latency = latency_t;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the maximum duration time gap for double-tap recognition (LATENCY)
 * @param  Pointer to LATENCY .
 * @retval Error code
 */
int8_t ITDS_getLatency(uint8_t *latency_t)
{

	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	*latency_t = (ITDS_state_t)(int_dur_t.latency);
	return WE_SUCCESS;
}

/**
 * @brief  set the expected quiet time after a tap detection (QUIET)
 * @param  quiet time value
 * @retval Error code
 */
int8_t ITDS_setQuiet(uint8_t quiet_t)
{
	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	int_dur_t.quiet = quiet_t;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the expected quiet time after a tap detection (QUIET)
 * @param  Pointer to  QUIET
 * @retval Error code
 */
int8_t ITDS_getQuiet(uint8_t *quiet_t)
{

	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	*quiet_t = (ITDS_state_t)(int_dur_t.quiet);
	return WE_SUCCESS;
}

/**
 * @brief  set the maximum duration of over-threshold event (SHOCK)
 * @param  SHOCK time value
 * @retval Error code
 */
int8_t ITDS_setShock(uint8_t shock_t)
{
	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	int_dur_t.shock = shock_t;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the maximum duration of over-threshold event (SHOCK)
 * @param  Pointer to SHOCK .
 * @retval Error code
 */
int8_t ITDS_getShock(uint8_t *shock_t)
{

	ITDS_int_dur_t int_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_INT_DUR_REG, 1, (uint8_t *)&int_dur_t))
		return WE_FAIL;

	*shock_t = (ITDS_state_t)(int_dur_t.shock);
	return WE_SUCCESS;
}

/* WAKE_UP_TH */

/**
 * @brief  Enable the single und double-tap event OR only Single-tap event [0:only single, 1:single and double-tap]
 * @param  tap event State [0:only single, 1: single and double-tap]
 * @retval Error code
 */
int8_t ITDS_enTapEvent(ITDS_state_t singleDoubleTap)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	wake_up_th_t.enTapEvent = singleDoubleTap;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the tap event State [0:only single, 1: single and double-tap]
 * @param  Pointer to the tap event State
 * @retval Error code
 */
int8_t ITDS_getTapEvent(ITDS_state_t *singleDoubleTap)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	*singleDoubleTap = (ITDS_state_t)(wake_up_th_t.enTapEvent);
	return WE_SUCCESS;
}

/**
 * @brief  Enable inactivity(sleep)
 * @param  sleep state
 * @retval Error code
 */
int8_t ITDS_enInactivity(ITDS_state_t sleep)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	wake_up_th_t.enInactivtyEvent = sleep;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the inactivity(sleep) State
 * @param  Pointer to the inactivity(sleep) State .
 * @retval Error code
 */
int8_t ITDS_getInactivityState(ITDS_state_t *sleep)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	*sleep = (ITDS_state_t)(wake_up_th_t.enInactivtyEvent);
	return WE_SUCCESS;
}

/**
 * @brief  set Wakeup threshold
 * @param  wake up threshold
 * @retval Error code
 */
int8_t ITDS_setWakeupThreshold(uint8_t WakeupTHR)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	wake_up_th_t.wakeupThreshold = WakeupTHR;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Wakeup threshold
 * @param  Pointer to the Wakeup threshold.
 * @retval Error code
 */
int8_t ITDS_getWakeupThreshold(uint8_t *WakeupTHR)
{
	ITDS_wake_up_th_t wake_up_th_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_TH_REG, 1, (uint8_t *)&wake_up_th_t))
		return WE_FAIL;

	*WakeupTHR = (wake_up_th_t.wakeupThreshold);
	return WE_SUCCESS;
}

/*WAKE_UP_DUR*/

/**
 * @brief  set Free-fall duration MSB
 * @param  Free-fall duration MSB
 * @retval Error code
 */
int8_t ITDS_setFreeFallDurationMSB(ITDS_state_t FF_DUR5)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	wake_up_dur_t.freeFallDurationMSB = FF_DUR5;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Free-fall duration MSB
 * @param  Pointer to the Free-fall duration MSB .
 * @retval Error code
 */
int8_t ITDS_getFreeFallDurationMSB(ITDS_state_t *FF_DUR5)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	*FF_DUR5 = (ITDS_state_t)(wake_up_dur_t.freeFallDurationMSB);
	return WE_SUCCESS;
}

/**
 * @brief  Enable stationary detection
 * @param  State
 * @retval Error code
 */
int8_t ITDS_enStationnaryDetection(ITDS_state_t stationary)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	wake_up_dur_t.enStationary = stationary;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the stationary detection state
 * @param  Pointer to the stationary detection state
 * @retval Error code
 */
int8_t ITDS_getStationnaryState(ITDS_state_t *stationary)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	*stationary = (ITDS_state_t)(wake_up_dur_t.enStationary);
	return WE_SUCCESS;
}

/**
 * @brief  set Wakeup duration
 * @param  Wakeup duration
 * @retval Error code
 */
int8_t ITDS_setWakeupDuration(uint8_t wakeDUR)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	wake_up_dur_t.wakeupDuration = wakeDUR;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Wakeup duration
 * @param  Pointer to the Wakeup duration
 * @retval Error code
 */
int8_t ITDS_getWakeupDuration(uint8_t *wakeDUR)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	*wakeDUR = (wake_up_dur_t.wakeupDuration);
	return WE_SUCCESS;
}

/**
 * @brief  set the Sleep Mode Duration
 * @param  Sleep Mode Duration
 * @retval Error code
 */
int8_t ITDS_setSleepDuration(uint8_t sleepDur)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	wake_up_dur_t.sleepDuration = sleepDur;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Sleep Mode Duration
 * @param  Pointer to the Sleep Mode Duration
 * @retval Error code
 */
int8_t ITDS_getSleepDuration(uint8_t *sleepDur)
{
	ITDS_wake_up_dur_t wake_up_dur_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_DUR_REG, 1, (uint8_t *)&wake_up_dur_t))
		return WE_FAIL;

	*sleepDur = (wake_up_dur_t.sleepDuration);
	return WE_SUCCESS;
}

/* FREE_FALL */

/**
 * @brief  set Free-fall duration LSB
 * @param  Free-fall duration LSB
 * @retval Error code
 */
int8_t ITDS_setFreeFallDurationLSB(uint8_t FF_DURLSB)
{
	ITDS_free_fall_t free_fall_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	free_fall_t.freeFallDurationLSB = FF_DURLSB;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Free-fall duration LSB
 * @param  Pointer to the Free-fall duration LSB
 * @retval Error code
 */
int8_t ITDS_getFreeFallDurationLSB(uint8_t *FF_DURLSB)
{
	ITDS_free_fall_t free_fall_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	*FF_DURLSB = (free_fall_t.freeFallDurationLSB);
	return WE_SUCCESS;
}

/**
 * @brief  set Free-fall Threshold
 * @param  Free-fall Threshold Decoding
 * @retval Error code
 */
int8_t ITDS_setFreeFallThreshold(ITDS_Free_Fall_Thr FF_TH)
{
	ITDS_free_fall_t free_fall_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	free_fall_t.freeFallThreshold = FF_TH;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Free-fall Threshold decoding
 * @param  Pointer to the Free-fall Threshold decoding
 * @retval Error code
 */
int8_t ITDS_getFreeFallThreshold(ITDS_Free_Fall_Thr *FF_TH)
{
	ITDS_free_fall_t free_fall_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_FREE_FALL_REG, 1, (uint8_t *)&free_fall_t))
		return WE_FAIL;

	*FF_TH = (ITDS_Free_Fall_Thr)(free_fall_t.freeFallThreshold);
	return WE_SUCCESS;
}

/* STATUS_DETECT */

/**
 * @brief  Read the temperature data state
 * @param  Pointer to the temperature data state
 * @retval Error code
 */
int8_t ITDS_getTempDrdy(ITDS_state_t *tempDrdy)
{
	ITDS_status_detect_t status_detect_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_DETECT_REG, 1, (uint8_t *)&status_detect_t))
		return WE_FAIL;

	*tempDrdy = (ITDS_state_t)(status_detect_t.tempDrdy);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Sleep event data state
 * @param  Pointer to the Sleep event data state
 * @retval Error code
 */
int8_t ITDS_getSleepState(ITDS_state_t *sleepState)
{
	ITDS_status_detect_t status_detect_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_DETECT_REG, 1, (uint8_t *)&status_detect_t))
		return WE_FAIL;

	*sleepState = (ITDS_state_t)(status_detect_t.sleepState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the double Tap Event State
 * @param  Pointer to the double Tap Event State
 * @retval Error code
 */
int8_t ITDS_getDoubleTapState(ITDS_state_t *doubleTap)
{
	ITDS_status_detect_t status_detect_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_DETECT_REG, 1, (uint8_t *)&status_detect_t))
		return WE_FAIL;

	*doubleTap = (ITDS_state_t)(status_detect_t.doubleTapState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the single Tap Event State
 * @param  Pointer to the single Tap Event State
 * @retval Error code
 */
int8_t ITDS_getSingleTapState(ITDS_state_t *singleTap)
{
	ITDS_status_detect_t status_detect_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_DETECT_REG, 1, (uint8_t *)&status_detect_t))
		return WE_FAIL;

	*singleTap = (ITDS_state_t)(status_detect_t.singleTapState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the temperature data state
 * @param  Pointer to the temperature data state
 * @retval Error code
 */
int8_t ITDS_getDrdy(ITDS_state_t *drdy)
{
	ITDS_status_detect_t status_detect_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_STATUS_DETECT_REG, 1, (uint8_t *)&status_detect_t))
		return WE_FAIL;

	*drdy = (ITDS_state_t)(status_detect_t.drdy);
	return WE_SUCCESS;
}

/*WAKE_UP_EVENT*/

/**
 * @brief  Read the Wake-up Event detection Status On X Axis
 * @param  pointer to the Wake-up Event detection Status On X Axis
 * @retval Error code
 */
int8_t ITDS_getWakeupX(ITDS_state_t *wakeUpX)
{
	ITDS_wake_up_event_t wake_up_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_EVENT_REG, 1, (uint8_t *)&wake_up_event_t))
		return WE_FAIL;

	*wakeUpX = (ITDS_state_t)(wake_up_event_t.wakeUpX);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Wake-up Event detection Status On Y Axis
 * @param  pointer to the Wake-up Event detection Status On Y Axis.
 * @retval Error code
 */
int8_t ITDS_getWakeupY(ITDS_state_t *wakeUpY)
{
	ITDS_wake_up_event_t wake_up_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_EVENT_REG, 1, (uint8_t *)&wake_up_event_t))
		return WE_FAIL;

	*wakeUpY = (ITDS_state_t)(wake_up_event_t.wakeUpY);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Wake-up Event detection Status On Z Axis
 * @param  pointer to the Wake-up Event detection Status On Z Axis.
 * @retval Error code
 */
int8_t ITDS_getWakeupZ(ITDS_state_t *wakeUpZ)
{
	ITDS_wake_up_event_t wake_up_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_EVENT_REG, 1, (uint8_t *)&wake_up_event_t))
		return WE_FAIL;

	*wakeUpZ = (ITDS_state_t)(wake_up_event_t.wakeUpZ);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Wake-up Event detection Status
 * @param  Pointer to the Wake-up Event detection State.
 * @retval Error code
 */
int8_t ITDS_getWakeupState(ITDS_state_t *wakeUpState)
{
	ITDS_wake_up_event_t wake_up_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_WAKE_UP_EVENT_REG, 1, (uint8_t *)&wake_up_event_t))
		return WE_FAIL;

	*wakeUpState = (ITDS_state_t)(wake_up_event_t.wakeUpState);
	return WE_SUCCESS;
}

/* TAP EVENT 0x39 */

/**
 * @brief  Read the Tap Event Status
 * @param  Pointer to the Tap Event State
 * @retval Error code
 */
int8_t ITDS_getTapEventState(ITDS_state_t *tapEvent)
{
	ITDS_tap_event_t tap_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_EVENT_REG, 1, (uint8_t *)&tap_event_t))
		return WE_FAIL;

	*tapEvent = (ITDS_state_t)(tap_event_t.tapEventState);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Accelarion Sign
 * @param  Pointer to the Accelarion Sign
 * @retval Error code
 */
int8_t ITDS_getTapSign(ITDS_tapSign_t *tapSign)
{
	ITDS_tap_event_t tap_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_EVENT_REG, 1, (uint8_t *)&tap_event_t))
		return WE_FAIL;

	*tapSign = (ITDS_tapSign_t)(tap_event_t.tapSign);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Event Status on X Axis
 * @param  Pointer to the Tap Event Status on X Axis.
 * @retval Error code
 */
int8_t ITDS_getTapXAxis(ITDS_state_t *tapXAxis)
{
	ITDS_tap_event_t tap_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_EVENT_REG, 1, (uint8_t *)&tap_event_t))
		return WE_FAIL;

	*tapXAxis = (ITDS_state_t)(tap_event_t.tapXAxis);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Event Status on Y Axis
 * @param  Pointer to the Tap Event Status on Y Axis.
 * @retval Error code
 */
int8_t ITDS_getTapYAxis(ITDS_state_t *tapYAxis)
{
	ITDS_tap_event_t tap_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_EVENT_REG, 1, (uint8_t *)&tap_event_t))
		return WE_FAIL;

	*tapYAxis = (ITDS_state_t)(tap_event_t.tapYAxis);
	return WE_SUCCESS;
}

/**
 * @brief  Read the Tap Event Status on Z Axis
 * @param  Pointer to the Tap Event Status on Z Axis.
 * @retval Error code
 */
int8_t ITDS_getTapZAxis(ITDS_state_t *tapZAxis)
{
	ITDS_tap_event_t tap_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_TAP_EVENT_REG, 1, (uint8_t *)&tap_event_t))
		return WE_FAIL;

	*tapZAxis = (ITDS_state_t)(tap_event_t.tapZAxis);
	return WE_SUCCESS;
}

/* 6D_EVENT */

/**
 * @brief  Read the XL Over Threshold State
 * @param  Pointer to the XL Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getXLOverThresholdState(ITDS_state_t *XLOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*XLOverThreshold = (ITDS_state_t)(event_6D_t.XLOverThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Read the XH Over Threshold State
 * @param  Pointer to the XH Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getXHOverThresholdState(ITDS_state_t *XHOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*XHOverThreshold = (ITDS_state_t)(event_6D_t.XHOverThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Read the YL Over Threshold State
 * @param  Pointer to the XL Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getYLOverThresholdState(ITDS_state_t *YLOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*YLOverThreshold = (ITDS_state_t)(event_6D_t.YLOverThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Read the YH Over Threshold State
 * @param  Pointer to the YL Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getYHOverThresholdState(ITDS_state_t *YHOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*YHOverThreshold = (ITDS_state_t)(event_6D_t.YHOverThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Read the ZL Over Threshold State
 * @param  Pointer to the ZL Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getZLOverThresholdState(ITDS_state_t *ZLOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*ZLOverThreshold = (ITDS_state_t)(event_6D_t.ZLOverThreshold);
	return WE_SUCCESS;
}

/**
 * @brief  Read the ZH Over Threshold State
 * @param  Pointer to the ZH Over Threshold State
 * @retval Error code
 */
int8_t ITDS_getZHOverThresholdState(ITDS_state_t *ZHOverThreshold)
{
	ITDS_event_6D_t event_6D_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_6D_EVENT_REG, 1, (uint8_t *)&event_6D_t))
		return WE_FAIL;

	*ZHOverThreshold = (ITDS_state_t)(event_6D_t.ZHOverThreshold);
	return WE_SUCCESS;
}

/* ALL_INT_EVENT */

/**
 * @brief  Read the Sleep Change State
 * @param  Pointer to the Sleep Change State
 * @retval Error code
 */
int8_t ITDS_getSleepChangeState(ITDS_state_t *sleep)
{
	ITDS_all_int_event_t all_int_event_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_ALL_INT_EVENT_REG, 1, (uint8_t *)&all_int_event_t))
		return WE_FAIL;

	*sleep = (ITDS_state_t)(all_int_event_t.sleepState);
	return WE_SUCCESS;
}

/* X_Y_Z_OFS_USR */

/**
 * @brief  set the User offset on X axis for wake-up
 * @param  the User offset on X axis state for wake-up
 * @retval Error code
 */
int8_t ITDS_setOffsetValueOnXAxis(uint8_t offsetValueXAxis)
{
	uint8_t offsetValueOnXAxis = offsetValueXAxis;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_X_OFS_USR_REG, 1, &offsetValueOnXAxis))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the User offset Value on X axis
 * @param  Pointer to the User offset Value on X axis..
 * @retval Error code
 */
int8_t ITDS_getOffsetValueOnXAxis(uint8_t *offsetvalueXAxis)
{
	uint8_t offsetValueOnXAxis;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_X_OFS_USR_REG, 1, &offsetValueOnXAxis))
		return WE_FAIL;

	*offsetvalueXAxis = (offsetValueOnXAxis);
	return WE_SUCCESS;
}

/**
 * @brief  set the User offset on Y axis for wake-up
 * @param  ser offset on Y axis state for wake-up
 * @retval Error code
 */
int8_t ITDS_setOffsetValueOnYAxis(uint8_t offsetValueYAxis)
{
	uint8_t offsetValueOnYAxis = offsetValueYAxis;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_Y_OFS_USR_REG, 1, &offsetValueOnYAxis))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the User offset Value on Y axis
 * @param  Pointer to the User offset Value on Y axis..
 * @retval Error code
 */
int8_t ITDS_getOffsetValueOnYAxis(uint8_t *offsetValueYAxis)
{
	uint8_t offsetValueOnYAxis;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_Y_OFS_USR_REG, 1, &offsetValueOnYAxis))
		return WE_FAIL;

	*offsetValueYAxis = (offsetValueOnYAxis);
	return WE_SUCCESS;
}

/**
 * @brief  set the User offset on Z axis for wake-up
 * @param  the User offset on Z axis state for wake-up
 * @retval Error code
 */
int8_t ITDS_setOffsetValueOnZAxis(uint8_t offsetvalueZAxis)
{
	uint8_t offsetValueOnZAxis = offsetvalueZAxis;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_Z_OFS_USR_REG, 1, &offsetValueOnZAxis))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the User offset Value on Z axis
 * @param  Pointer to the User offset Value on Z axis.
 * @retval Error code
 */
int8_t ITDS_getOffsetValueOnZAxis(uint8_t *offsetValueZAxis)
{
	uint8_t offsetValueOnZAxis;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_Z_OFS_USR_REG, 1, &offsetValueOnZAxis))
		return WE_FAIL;

	*offsetValueZAxis = (offsetValueOnZAxis);
	return WE_SUCCESS;
}

/* CTRL_7 */

/**
 * @brief  select the Data Ready Interrupt Mode [latched mode / pulsed mode ]
 * @param  Data Ready Interrupt Mode
 * @retval Error code
 */
int8_t ITDS_setDrdyPulse(ITDS_drdy_pulse drdyPulsed)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.drdyPulse = drdyPulsed;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Data Ready Interrupt Mode [latched mode / pulsed mode ]
 * @param  Pointer to Data Ready Interrupt Mode
 * @retval Error code
 */
int8_t ITDS_getDrdyPulseIntMode(ITDS_drdy_pulse *drdyPulsed)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*drdyPulsed = (ITDS_drdy_pulse)(ctrl_7_t.drdyPulse);
	return WE_SUCCESS;
}

/**
 * @brief  enable the signal Routing : all signals available only on Int_1 are routed On Int_0
 * @param  signal Routing Int1 On Int0 state
 * @retval Error code
 */
int8_t ITDS_setInt1OnInt0(ITDS_state_t int1Onint0)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.INT1toINT0 = int1Onint0;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the signal Routing INT_1 On INT_0 State
 * @param  Pointer to Routing INT_1 On INT_0 State.
 * @retval Error code
 */
int8_t ITDS_getInt1OnInt0(ITDS_state_t *int1Onint0)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*int1Onint0 = (ITDS_state_t)(ctrl_7_t.INT1toINT0);
	return WE_SUCCESS;
}

/**
 * @brief  enable interrupts
 * @param  interrupts state
 * @retval Error code
 */
int8_t ITDS_enInterrups(ITDS_state_t interrupts)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.enInterrupts = interrupts;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the interrupts State
 * @param  Pointer to interrupts State .
 * @retval Error code
 */
int8_t ITDS_getInterruptsState(ITDS_state_t *interrupts)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*interrupts = (ITDS_state_t)(ctrl_7_t.enInterrupts);
	return WE_SUCCESS;
}

/**
 * @brief  enable the application of User Offset value on XL Output Data
 * @param  state
 * @retval Error code
 */
int8_t ITDS_enApplyOffsetXL(ITDS_state_t offsetXLOutput)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.applyOffsetXL = offsetXLOutput;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Application state of User Offset value on XL Output Data
 * @param  Pointer to offset XL Output.
 * @retval Error code
 */
int8_t ITDS_getApplyOffsetXL(ITDS_state_t *offsetXLOutput)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*offsetXLOutput = (ITDS_state_t)(ctrl_7_t.applyOffsetXL);
	return WE_SUCCESS;
}

/**
 * @brief  enable the application of User Offset value on XL Data only for wakeup functions
 * @param  state
 * @retval Error code
 */
int8_t ITDS_enApplyWakeUpOffsetXL(ITDS_state_t offsetXLWakeup)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.applyWakeUpOffsetXL = offsetXLWakeup;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Application state of User Offset value on XL Data only for wakeup functions
 * @param  Pointer to offset XL Wakeup.
 * @retval Error code
 */
int8_t ITDS_getApplyWakeUpOffsetXL(ITDS_state_t *offsetXLWakeup)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*offsetXLWakeup = (ITDS_state_t)(ctrl_7_t.applyWakeUpOffsetXL);
	return WE_SUCCESS;
}

/**
 * @brief  set the Weight of the user offset words
 * @param  offset Weight
 * @retval Error code
 */
int8_t ITDS_setOffsetWeight(ITDS_state_t offsetWeight)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.userOffset = offsetWeight;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Weight of the user offset words
 * @param  Pointer to offset Weight .
 * @retval Error code
 */
int8_t ITDS_getOffsetWeight(ITDS_state_t *offsetWeight)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*offsetWeight = (ITDS_state_t)(ctrl_7_t.userOffset);
	return WE_SUCCESS;
}

/**
 * @brief  enable High Pass Filter Reference Mode
 * @param  state
 * @retval Error code
 */
int8_t ITDS_enHighPassRefMode(ITDS_state_t refMode)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.highPassRefMode = refMode;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the state of High Pass Filter Reference Mode
 * @param  Pointer to Reference Mode state
 * @retval Error code
 */
int8_t ITDS_getHighPassRefMode(ITDS_state_t *refMode)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*refMode = (ITDS_state_t)(ctrl_7_t.highPassRefMode);
	return WE_SUCCESS;
}

/**
 * @brief  enable the Low Pass On 6D state
 * @param  state
 * @retval Error code
 */
int8_t ITDS_enLowPassOn6D(ITDS_state_t LPassOn6D)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	ctrl_7_t.lowPassOn6D = LPassOn6D;

	if (WE_FAIL == WriteReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	return WE_SUCCESS;
}

/**
 * @brief  Read the Low Pass On 6D state
 * @param  Pointer to Low Pass On 6D state.
 * @retval Error code
 */
int8_t ITDS_getLowPassOn6DState(ITDS_state_t *LPassOn6D)
{
	ITDS_ctrl_7_t ctrl_7_t;

	if (WE_FAIL == ReadReg((uint8_t)ITDS_CTRL_7_REG, 1, (uint8_t *)&ctrl_7_t))
		return WE_FAIL;

	*LPassOn6D = (ITDS_state_t)(ctrl_7_t.lowPassOn6D);
	return WE_SUCCESS;
}
