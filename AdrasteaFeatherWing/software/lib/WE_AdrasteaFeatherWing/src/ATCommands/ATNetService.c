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
 * @brief AT commands for Net Service functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATNetService.h"
#include "Math.h"
#include "../Adrastea.h"

/**
 * @brief Read Operators from memory (using the AT+COPN command).
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadOperators()
{
	if (!Adrastea_SendRequest("AT+COPN\r\n"))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of Operator Read event arguments.
 *
 * @param[out] dataP the operator is returned in this argument. See ATNetService_Operator_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ParseOperatorReadEvent(char *pEventArguments, ATNetService_Operator_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	argumentsP += 1;

	if (!ATCommand_GetNextArgumentIntWithoutQuotationMarks(&argumentsP, &dataP->operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32 ), ATCOMMAND_ARGUMENT_DELIM))
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
 * @param[in] format PLMN format. See ATNetService_PLMN_Format_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_SetPLMNReadFormat(ATNetService_PLMN_Format_t format)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+COPS=3,");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!Adrastea_SendRequest(pRequestCommand))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set Public Land Mobile Network (using the AT+COPS command).
 *
 * @param[in] plmn either choose a specific plmn or automatic. See ATNetService_PLMN_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_SetPLMN(ATNetService_PLMN_t plmn)
{
	switch (plmn.selectionMode)
	{
	case ATNetService_PLMN_Selection_Mode_Automatic:
	{

		if (!Adrastea_SendRequest("AT+COPS=0\r\n"))
		{
			return false;
		}

		break;
	}
	case ATNetService_PLMN_Selection_Mode_Manual:
	{
		Adrastea_optionalParamsDelimCount = 1;

		char *pRequestCommand = AT_commandBuffer;

		strcpy(pRequestCommand, "AT+COPS=1,");

		if (!ATCommand_AppendArgumentInt(pRequestCommand, plmn.format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		switch (plmn.format)
		{
		case ATNetService_PLMN_Format_Short_AlphaNumeric:
		case ATNetService_PLMN_Format_Long_AlphaNumeric:
		{

			if (!ATCommand_AppendArgumentString(pRequestCommand, plmn.operator.operatorString, ATCOMMAND_STRING_TERMINATE))
			{
				return false;
			}

			break;
		}
		case ATNetService_PLMN_Format_Numeric:
		{

			if (!ATCommand_AppendArgumentIntQuotationMarks(pRequestCommand, plmn.operator.operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
			{
				return false;
			}

			break;
		}
		default:
			return false;
			break;
		}

		if (plmn.accessTechnology != ATCommon_AcT_Invalid)
		{

			if (!ATCommand_AppendArgumentInt(pRequestCommand, plmn.accessTechnology, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

		break;
	}
	case ATNetService_PLMN_Selection_Mode_Deregister:
	{

		if (!Adrastea_SendRequest("AT+COPS=2\r\n"))
		{
			return false;
		}

		break;
	}
	default:
		return false;
		break;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read currently selected plmn (using the AT+COPS command).
 *
 * @param[in] plmnP Currently selected plmn is returned in this argument. See ATNetService_PLMN_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadPLMN(ATNetService_PLMN_t *plmnP)
{
	if (plmnP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+COPS?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	plmnP->operator.operatorNumeric = -1;
	plmnP->operator.operatorString[0] = '\0';
	plmnP->format = ATNetService_PLMN_Format_Invalid;
	plmnP->accessTechnology = ATCommon_AcT_Invalid;

	switch (Adrastea_CountArgs(pResponseCommand))
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
		case ATNetService_PLMN_Format_Short_AlphaNumeric:
		case ATNetService_PLMN_Format_Long_AlphaNumeric:
		{

			if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, plmnP->operator.operatorString, ATCOMMAND_ARGUMENT_DELIM, sizeof(plmnP->operator.operatorString)))
			{
				return false;
			}

			break;
		}
		case ATNetService_PLMN_Format_Numeric:
		{

			if (!ATCommand_GetNextArgumentIntWithoutQuotationMarks(&pResponseCommand, &plmnP->operator.operatorNumeric, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32 ), ATCOMMAND_ARGUMENT_DELIM))
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
 * @param[out] sq Signal Quality is returned in this argument. See ATNetService_Signal_Quality_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadSignalQuality(ATNetService_Signal_Quality_t *sq)
{
	if (sq == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CSQ\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
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
 * @param[out] esq Extended Signal Quality is returned in this argument. See ATNetService_Extended_Signal_Quality_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadExtendedSignalQuality(ATNetService_Extended_Signal_Quality_t *esq)
{
	if (esq == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CESQ\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
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
 * @param[out] psmP Power Saving Mode state and timers are returned in this argument. See ATNetService_Power_Saving_Mode_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadPowerSavingMode(ATNetService_Power_Saving_Mode_t *psmP)
{
	if (psmP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CPSMS?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
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
 * @param[in] psm Power Saving Mode state and timers. See ATNetService_Power_Saving_Mode_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_SetPowerSavingMode(ATNetService_Power_Saving_Mode_t psm)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CPSMS=");

	switch (psm.state)
	{
	case ATNetService_Power_Saving_Mode_State_Disable:
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, ATNetService_Power_Saving_Mode_State_Disable, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		break;
	}
	case ATNetService_Power_Saving_Mode_State_Enable:
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, ATNetService_Power_Saving_Mode_State_Enable, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (!Adrastea_SendRequest(pRequestCommand))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set eDRX Settings (using the AT+CEDRXS command).
 *
 * @param[in] mode eDRX State. See ATNetService_eDRX_Mode_t.
 *
 * @param[in] eDRX Configuration. See ATNetService_eDRX_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_SeteDRXDynamicParameters(ATNetService_eDRX_Mode_t mode, ATNetService_eDRX_t edrx)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CEDRXS=");

	switch (mode)
	{
	case ATNetService_eDRX_Mode_Disable:
	case ATNetService_eDRX_Mode_Disable_Reset:
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		break;
	}

	case ATNetService_eDRX_Mode_Enable:
	{

		Adrastea_optionalParamsDelimCount = 1;

		if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (edrx.AcT > ATNetService_eDRX_AcT_NotUsingeDRX)
		{
			if (!ATCommand_AppendArgumentInt(pRequestCommand, edrx.AcT, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
			{
				return false;
			}
			Adrastea_optionalParamsDelimCount = 1;
		}
		else
		{
			if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
			{
				return false;
			}
			Adrastea_optionalParamsDelimCount++;
		}

		if (edrx.requestedValue != ATNetService_eDRX_Value_Invalid)
		{
			if (!ATCommand_AppendArgumentBitsQuotationMarks(pRequestCommand, edrx.requestedValue, (ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_STRING_TERMINATE))
			{
				return false;
			}
			Adrastea_optionalParamsDelimCount = 0;
		}

		pRequestCommand[strlen(pRequestCommand) - Adrastea_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

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

	if (!Adrastea_SendRequest(pRequestCommand))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set eDRX Settings (using the AT+CEDRXRDP command).
 *
 * @param[out] edrxP eDRX Configuration is returned in this argument. See ATNetService_eDRX_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadeDRXDynamicParameters(ATNetService_eDRX_t *edrxP)
{
	if (edrxP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CEDRXRDP\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	edrxP->requestedValue = ATNetService_eDRX_Value_Invalid;
	edrxP->networkProvidedValue = ATNetService_eDRX_Value_Invalid;
	edrxP->pagingTime = 0;

	switch (Adrastea_CountArgs(pResponseCommand))
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

	}
	default:
		return false;
	}

	return true;
}

/**
 * @brief Read Coverage Enhancement Status (using the AT+CRCES command).
 *
 * @param[out] cesP Coverage Enhancement Status is returned in this argument. See ATNetService_CES_t.
 *
 * @return true if successful, false otherwise
 */
bool ATNetService_ReadCoverageEnhancementStatus(ATNetService_CES_t *cesP)
{
	if (cesP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CRCES\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_NetService), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	switch (Adrastea_CountArgs(pResponseCommand))
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

