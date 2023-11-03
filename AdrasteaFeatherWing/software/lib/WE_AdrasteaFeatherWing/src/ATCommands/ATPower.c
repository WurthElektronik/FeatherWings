/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief MCU commands for Power functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATPower.h"
#include "ATDevice.h"
#include "../Adrastea.h"

static const char *ATPower_Mode_Strings[ATPower_Mode_NumberOfValues] = {
		"stop",
		"standby",
		"shutdown", };

/**
 * @brief Set MCU Power Mode (using the pwrMode command).
 *
 * @param[in] mode Power Mode. See ATPower_Mode_t.
 *
 * @param[in] duration Duration for sleep mode.
 *
 * @return true if successful, false otherwise
 */
bool ATPower_SetPowerMode(ATPower_Mode_t mode, ATPower_Mode_Duration_t duration)
{
	Adrastea_Transmit("\x04", 1);

	while (Adrastea_CheckATMode() == true)
	{
	}

	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "pwrMode ");

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATPower_Mode_Strings[mode], ' '))
	{
		return false;
	}

	if (duration != ATPower_Mode_Duration_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, duration, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount = 0;
	}

	pRequestCommand[strlen(pRequestCommand) - Adrastea_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!Adrastea_SendRequest(pRequestCommand))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Power), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	Adrastea_Transmit("map\r\n", 5);

	while (Adrastea_CheckATMode() == false)
	{
	}

	return true;
}

/**
 * @brief Enable MCU Sleep (using the sleepSet command).
 *
 * Note: Make sure to disable the modem before enabling sleep if you don't want the modem to keep waking up the MCU.
 *
 * @return true if successful, false otherwise
 */
bool ATPower_EnableSleep()
{
	Adrastea_Transmit("\x04", 1);

	while (Adrastea_CheckATMode() == true)
	{
	}

	if (!Adrastea_SendRequest("sleepSet enable\r\n"))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Power), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}
