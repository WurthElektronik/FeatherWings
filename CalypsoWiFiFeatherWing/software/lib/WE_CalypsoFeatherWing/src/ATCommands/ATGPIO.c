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
 * @brief AT commands for GPIO functionality.
 */

#include <ATCommands.h>
#include "ATGPIO.h"

#include "Calypso.h"

static const char *ATGPIO_GPIOType_Strings[ATGPIO_GPIOType_NumberOfValues] = {
    "unused",
    "input",
    "output",
    "pwm"
};

static const char *ATGPIO_PullType_Strings[ATGPIO_PullType_NumberOfValues] = {
    "nopull",
    "pullup",
    "pulldown"
};

static const char *ATGPIO_GPIOState_Strings[ATGPIO_GPIOState_NumberOfValues] = {
    "low",
    "high"
};

/**
 * @brief Get a GPIO's configuration and/or current value.
 *
 * @param[in] id ID of GPIO to be queried
 * @param[in] defaultSetting If true, the configured default values stored in the module's
 *                           flash are returned. Otherwise the current value is returned.
 * @param[out] gpio The queried data is returned in this argument
 *
 * @return true if successful, false otherwise
 */
bool ATGPIO_Get(ATGPIO_GPIOId_t id, bool defaultSetting, ATGPIO_GPIO_t *gpio)
{
    gpio->type = ATGPIO_GPIOType_Unused;

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+gpioGet=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   id,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      defaultSetting ? "true" : "false",
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_GPIO), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *cmd = "+gpioget:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    uint8_t enumInt;
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &enumInt,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    gpio->id = enumInt;

    if (!ATCommand_GetNextArgumentEnum(&pRespondCommand,
                                     &enumInt,
                                     ATGPIO_GPIOType_Strings,
                                     ATGPIO_GPIOType_NumberOfValues,
                                     8,
                                     ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    gpio->type = enumInt;

    switch (gpio->type)
    {
    case ATGPIO_GPIOType_Unused:
        return true;

    case ATGPIO_GPIOType_Input:
        if (!ATCommand_GetNextArgumentEnum(&pRespondCommand,
                                         &enumInt,
                                         ATGPIO_GPIOState_Strings,
                                         ATGPIO_GPIOState_NumberOfValues,
                                         5,
                                         ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        gpio->parameters.input.state = enumInt;

        if (!ATCommand_GetNextArgumentEnum(&pRespondCommand,
                                         &enumInt,
                                         ATGPIO_PullType_Strings,
                                         ATGPIO_PullType_NumberOfValues,
                                         9,
                                         ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        gpio->parameters.input.pullType = enumInt;
        return true;

    case ATGPIO_GPIOType_Output:
        if (!ATCommand_GetNextArgumentEnum(&pRespondCommand,
                                         &enumInt,
                                         ATGPIO_GPIOState_Strings,
                                         ATGPIO_GPIOState_NumberOfValues,
                                         5,
                                         ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        gpio->parameters.output.state = enumInt;
        return true;

    case ATGPIO_GPIOType_PWM:
        if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                        &gpio->parameters.pwm.period,
                                        ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                        ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                        &gpio->parameters.pwm.ratio,
                                        ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                        ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        return true;

    default:
        break;
    }

    return false;
}

/**
 * @brief Set a GPIO's configuration and/or current value.
 *
 * @param[in] gpio GPIO configuration containing values to be set. Also includes the ID of the GPIO to be modified.
 * @param[in] save If true, the values are saved to the flash (used to set GPIO type and
 *                 default value). Otherwise, the values are not saved (used to set current value).
 *
 * @return true if successful, false otherwise
 */
bool ATGPIO_Set(ATGPIO_GPIO_t *gpio, bool save)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+gpioSet=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   gpio->id,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      save ? "true" : "false",
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATGPIO_GPIOType_Strings[gpio->type],
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    switch (gpio->type)
    {
    case ATGPIO_GPIOType_Unused:
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                          "",
                                          ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        break;

    case ATGPIO_GPIOType_Input:
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                          ATGPIO_PullType_Strings[gpio->parameters.input.pullType],
                                          ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        break;

    case ATGPIO_GPIOType_Output:
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                          ATGPIO_GPIOState_Strings[gpio->parameters.output.state],
                                          ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        break;

    case ATGPIO_GPIOType_PWM:
        if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                       gpio->parameters.pwm.period,
                                       ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                       ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                       gpio->parameters.pwm.ratio,
                                       ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                       ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    default:
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_GPIO), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Checks if the GPIO with the supplied ID supports the supplied GPIO function.
 *
 * @param[in] id GPIO ID
 * @param[in] type GPIO function type
 *
 * @return true if the GPIO supports the supplied GPIO function, false otherwise
 */
bool ATGPIO_IsFunctionSupported(ATGPIO_GPIOId_t id, ATGPIO_GPIOType_t type)
{
    if (id >= ATGPIO_GPIOId_NumberOfValues)
    {
        return false;
    }
    if (type == ATGPIO_GPIOType_PWM)
    {
        return id == ATGPIO_GPIOId_2 || id == ATGPIO_GPIOId_3;
    }
    return true;
}
