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
 * @brief AT commands for Net Service functionality.
 */
#include <AdrasteaI/ATCommands/ATNetService.h>
#include <AdrasteaI/AdrasteaI.h>
#include <Math.h>
#include <global/ATCommands.h>
#include <stdio.h>

/**
 * @brief Read Operators from memory (using the AT+COPN command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadOperators()
{
    if (!AdrasteaI_SendRequest("AT+COPN\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Operator Read event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the operator is returned in this argument. See AdrasteaI_ATNetService_Operator_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ParseOperatorReadEvent(char* pEventArguments, AdrasteaI_ATNetService_Operator_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    argumentsP += 1;

    if (!ATCommand_GetNextArgumentIntWithoutQuotationMarks(&argumentsP, &dataP->operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    argumentsP += 1;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->operatorString, ATCOMMAND_STRING_TERMINATE, sizeof(dataP->operatorString)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set Public Land Mobile Network read format (using the AT+COPS command).
 *
 * @param[in] format PLMN format. See AdrasteaI_ATNetService_PLMN_Format_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_SetPLMNReadFormat(AdrasteaI_ATNetService_PLMN_Format_t format)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+COPS=3,");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set Public Land Mobile Network (using the AT+COPS command).
 *
 * @param[in] plmn either choose a specific plmn or automatic. See AdrasteaI_ATNetService_PLMN_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_SetPLMN(AdrasteaI_ATNetService_PLMN_t plmn)
{
    switch (plmn.selectionMode)
    {
        case AdrasteaI_ATNetService_PLMN_Selection_Mode_Automatic:
        {
            if (!AdrasteaI_SendRequest("AT+COPS=0\r\n"))
            {
                return false;
            }

            break;
        }
        case AdrasteaI_ATNetService_PLMN_Selection_Mode_Manual:
        {
            AdrasteaI_optionalParamsDelimCount = 1;

            char* pRequestCommand = AT_commandBuffer;

            strcpy(pRequestCommand, "AT+COPS=1,");

            if (!ATCommand_AppendArgumentInt(pRequestCommand, plmn.format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (plmn.format)
            {
                case AdrasteaI_ATNetService_PLMN_Format_Short_AlphaNumeric:
                case AdrasteaI_ATNetService_PLMN_Format_Long_AlphaNumeric:
                {
                    if (!ATCommand_AppendArgumentString(pRequestCommand, plmn.operator.operatorString, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }

                    break;
                }
                case AdrasteaI_ATNetService_PLMN_Format_Numeric:
                {
                    if (!ATCommand_AppendArgumentIntQuotationMarks(pRequestCommand, plmn.operator.operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }

                    break;
                }
                default:
                    return false;
                    break;
            }

            if (plmn.accessTechnology != AdrasteaI_ATCommon_AcT_Invalid)
            {
                if (!ATCommand_AppendArgumentInt(pRequestCommand, plmn.accessTechnology, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

            break;
        }
        case AdrasteaI_ATNetService_PLMN_Selection_Mode_Deregister:
        {
            if (!AdrasteaI_SendRequest("AT+COPS=2\r\n"))
            {
                return false;
            }

            break;
        }
        default:
            return false;
            break;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read currently selected plmn (using the AT+COPS command).
 *
 * @param[in] plmnP Currently selected plmn is returned in this argument. See AdrasteaI_ATNetService_PLMN_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadPLMN(AdrasteaI_ATNetService_PLMN_t* plmnP)
{
    if (plmnP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+COPS?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    plmnP->operator.operatorNumeric = - 1;
    plmnP->operator.operatorString[0] = '\0';
    plmnP->format = AdrasteaI_ATNetService_PLMN_Format_Invalid;
    plmnP->accessTechnology = AdrasteaI_ATCommon_AcT_Invalid;

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &plmnP->selectionMode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case 4:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &plmnP->selectionMode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &plmnP->format, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (plmnP->format)
            {
                case AdrasteaI_ATNetService_PLMN_Format_Short_AlphaNumeric:
                case AdrasteaI_ATNetService_PLMN_Format_Long_AlphaNumeric:
                {
                    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, plmnP->operator.operatorString, ATCOMMAND_ARGUMENT_DELIM, sizeof(plmnP->operator.operatorString)))
                    {
                        return false;
                    }

                    break;
                }
                case AdrasteaI_ATNetService_PLMN_Format_Numeric:
                {
                    if (!ATCommand_GetNextArgumentIntWithoutQuotationMarks(&pResponseCommand, &plmnP->operator.operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32), ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    break;
                }
                default:
                    return false;
                    break;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &plmnP->accessTechnology, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
            break;
    }

    return true;
}

/**
 * @brief Read Signal Quality (using the AT+CSQ command).
 *
 * @param[out] sq Signal Quality is returned in this argument. See AdrasteaI_ATNetService_Signal_Quality_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadSignalQuality(AdrasteaI_ATNetService_Signal_Quality_t* sq)
{
    if (sq == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CSQ\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &sq->rssi, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &sq->ber, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read Extended Signal Quality (using the AT+CESQ command).
 *
 * @param[out] esq Extended Signal Quality is returned in this argument. See AdrasteaI_ATNetService_Extended_Signal_Quality_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadExtendedSignalQuality(AdrasteaI_ATNetService_Extended_Signal_Quality_t* esq)
{
    if (esq == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CESQ\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->rxlev, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->ber, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->rscp, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->ecno, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->rsrq, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &esq->rsrp, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read Power Saving Mode (using the AT+CPSMS command).
 *
 * @param[out] psmP Power Saving Mode state and timers are returned in this argument. See AdrasteaI_ATNetService_Power_Saving_Mode_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadPowerSavingMode(AdrasteaI_ATNetService_Power_Saving_Mode_t* psmP)
{
    if (psmP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CPSMS?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    char temp[40];

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &psmP->state, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentBitsWithoutQuotationMarks(&pResponseCommand, &psmP->periodicTAU.periodicTAU, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentBitsWithoutQuotationMarks(&pResponseCommand, &psmP->activeTime.activeTime, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set Power Saving Mode (using the AT+CPSMS command).
 *
 * @param[in] psm Power Saving Mode state and timers. See AdrasteaI_ATNetService_Power_Saving_Mode_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_SetPowerSavingMode(AdrasteaI_ATNetService_Power_Saving_Mode_t psm)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CPSMS=");

    switch (psm.state)
    {
        case AdrasteaI_ATNetService_Power_Saving_Mode_State_Disable:
        {
            if (!ATCommand_AppendArgumentInt(pRequestCommand, AdrasteaI_ATNetService_Power_Saving_Mode_State_Disable, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        case AdrasteaI_ATNetService_Power_Saving_Mode_State_Enable:
        {
            if (!ATCommand_AppendArgumentInt(pRequestCommand, AdrasteaI_ATNetService_Power_Saving_Mode_State_Enable, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentBitsQuotationMarks(pRequestCommand, psm.periodicTAU.periodicTAU, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentBitsQuotationMarks(pRequestCommand, psm.activeTime.activeTime, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
            break;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set eDRX Settings (using the AT+CEDRXS command).
 *
 * @param[in] mode eDRX State. See AdrasteaI_ATNetService_eDRX_Mode_t.
 *
 * @param[in] eDRX Configuration. See AdrasteaI_ATNetService_eDRX_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_SeteDRXDynamicParameters(AdrasteaI_ATNetService_eDRX_Mode_t mode, AdrasteaI_ATNetService_eDRX_t edrx)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CEDRXS=");

    switch (mode)
    {
        case AdrasteaI_ATNetService_eDRX_Mode_Disable:
        case AdrasteaI_ATNetService_eDRX_Mode_Disable_Reset:
        {
            if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }

        case AdrasteaI_ATNetService_eDRX_Mode_Enable:
        {
            AdrasteaI_optionalParamsDelimCount = 1;

            if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (edrx.AcT > AdrasteaI_ATNetService_eDRX_AcT_NotUsingeDRX)
            {
                if (!ATCommand_AppendArgumentInt(pRequestCommand, edrx.AcT, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
                AdrasteaI_optionalParamsDelimCount = 1;
            }
            else
            {
                if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
                AdrasteaI_optionalParamsDelimCount++;
            }

            if (edrx.requestedValue != AdrasteaI_ATNetService_eDRX_Value_Invalid)
            {
                if (!ATCommand_AppendArgumentBitsQuotationMarks(pRequestCommand, edrx.requestedValue, (ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                AdrasteaI_optionalParamsDelimCount = 0;
            }

            pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

            break;
        }

        default:
            return false;
            break;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set eDRX Settings (using the AT+CEDRXRDP command).
 *
 * @param[out] edrxP eDRX Configuration is returned in this argument. See AdrasteaI_ATNetService_eDRX_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadeDRXDynamicParameters(AdrasteaI_ATNetService_eDRX_t* edrxP)
{
    if (edrxP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CEDRXRDP\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    edrxP->requestedValue = AdrasteaI_ATNetService_eDRX_Value_Invalid;
    edrxP->networkProvidedValue = AdrasteaI_ATNetService_eDRX_Value_Invalid;
    edrxP->pagingTime = 0;

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &edrxP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        case 4:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &edrxP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentBitsWithoutQuotationMarks(&pResponseCommand, &edrxP->requestedValue, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentBitsWithoutQuotationMarks(&pResponseCommand, &edrxP->networkProvidedValue, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentBitsWithoutQuotationMarks(&pResponseCommand, &edrxP->pagingTime, ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
    }

    return true;
}

/**
 * @brief Read Coverage Enhancement Status (using the AT+CRCES command).
 *
 * @param[out] cesP Coverage Enhancement Status is returned in this argument. See AdrasteaI_ATNetService_CES_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATNetService_ReadCoverageEnhancementStatus(AdrasteaI_ATNetService_CES_t* cesP)
{
    if (cesP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CRCES\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_NetService), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cesP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case 3:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cesP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            pResponseCommand += 1;

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cesP->coverageEnhacementLevel, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            pResponseCommand += 1;

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cesP->coverageClass, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
    }

    return true;
}
