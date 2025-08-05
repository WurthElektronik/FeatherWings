/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief MCU commands for Power functionality.
 */
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/ATCommands/ATPower.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* AdrasteaI_ATPower_Mode_Strings[AdrasteaI_ATPower_Mode_NumberOfValues] = {
    "stop",
    "standby",
    "shutdown",
};

/**
 * @brief Set MCU Power Mode (using the pwrMode command).
 *
 * @param[in] mode Power Mode. See AdrasteaI_ATPower_Mode_t.
 *
 * @param[in] duration Duration for sleep mode.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATPower_SetPowerMode(AdrasteaI_ATPower_Mode_t mode, AdrasteaI_ATPower_Mode_Duration_t duration)
{
    AdrasteaI_Transparent_Transmit("\x04", 1);

    while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Off)
    {
    }

    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "pwrMode ");

    if (!ATCommand_AppendArgumentString(pRequestCommand, AdrasteaI_ATPower_Mode_Strings[mode], ' '))
    {
        return false;
    }

    if (duration != AdrasteaI_ATPower_Mode_Duration_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, duration, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        AdrasteaI_optionalParamsDelimCount = 0;
    }

    pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Power), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    AdrasteaI_Transparent_Transmit("map\r\n", 5);

    while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Ready)
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
bool AdrasteaI_ATPower_EnableSleep()
{
    AdrasteaI_Transparent_Transmit("\x04", 1);

    while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Off)
    {
    }

    if (!AdrasteaI_SendRequest("sleepSet enable\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Power), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}
