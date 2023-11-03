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
 * @brief AT commands for Packet Domain functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATPacketDomain.h"
#include "../Adrastea.h"

static const char *ATPacketDomain_PDP_Type_Strings[ATPacketDomain_PDP_Type_NumberOfValues] = {
		"IP",
		"IPV6",
		"IPV4V6",
		"Non−IP", };

/**
 * @brief Set Network Registration Result Code (using the AT+CEREG command).
 *
 * @param[in] resultcode result code. See ATPacketDomain_Network_Registration_Result_Code_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_t resultcode)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CEREG=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, resultcode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read Network Registration Status (using the AT+CEREG command).
 *
 * @param[out] statusP Registration Status is returned in this argument. See ATPacketDomain_Network_Registration_Status_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ReadNetworkRegistrationStatus(ATPacketDomain_Network_Registration_Status_t *statusP)
{
	if (statusP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CEREG?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	statusP->TAC[0] = '\0';

	statusP->ECI[0] = '\0';

	statusP->AcT = ATCommon_AcT_Invalid;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &statusP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	switch (statusP->resultCode)
	{
	case ATPacketDomain_Network_Registration_Result_Code_Disable:
	case ATPacketDomain_Network_Registration_Result_Code_Enable:
	{

		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &statusP->state, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		break;
	}
	case ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info:
	{

		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &statusP->state, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, statusP->TAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(statusP->TAC)))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, statusP->ECI, ATCOMMAND_ARGUMENT_DELIM, sizeof(statusP->ECI)))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &statusP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Parses the value of Network Registration Status event arguments.
 *
 * @param[out] statusP Registration Status is returned in this argument. See ATPacketDomain_Network_Registration_Status_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ParseNetworkRegistrationStatusEvent(char *pEventArguments, ATPacketDomain_Network_Registration_Status_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	argumentsP += 1;

	dataP->resultCode = ATPacketDomain_Network_Registration_Result_Code_Invalid;

	dataP->TAC[0] = '\0';

	dataP->ECI[0] = '\0';

	dataP->AcT = ATCommon_AcT_Invalid;

	switch (Adrastea_CountArgs(argumentsP))
	{
	case 1:
	{
		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		break;
	}
	case 4:
	{

		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->TAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataP->TAC)))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->ECI, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataP->ECI)))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->AcT, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		break;
	}
	default:
		break;
	}

	return true;
}

/**
 * @brief Set Packet Domain Event Reporting (using the AT+CGEREP command).
 *
 * @param[in] reporting Event reporting. See ATPacketDomain_Event_Reporting_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_SetPacketDomainEventReporting(ATPacketDomain_Event_Reporting_t reporting)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CGEREP=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, reporting.mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (reporting.mode != ATPacketDomain_Event_Reporting_Mode_Buffer_Unsolicited_Result_Codes_if_Full_Discard && reporting.buffer != ATPacketDomain_Event_Reporting_Buffer_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, reporting.buffer, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read Packet Domain Event Reporting (using the AT+CGEREP command).
 *
 * @param[out] reportingP Event Reporting is returned in this argument. See ATPacketDomain_Event_Reporting_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ReadPacketDomainEventReporting(ATPacketDomain_Event_Reporting_t *reportingP)
{
	if (reportingP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CGEREP?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &reportingP->mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &reportingP->buffer, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Define PDP Context (using the AT+CGDCONT command).
 *
 * @param[in] context PDP context. See ATPacketDomain_PDP_Context_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_DefinePDPContext(ATPacketDomain_PDP_Context_t context)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CGDCONT=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, context.cid, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATPacketDomain_PDP_Type_Strings[context.pdpType], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (context.apnName != NULL)
	{
		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, context.apnName, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read Defined PDP Contexts (using the AT+CGDCONT command).
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ReadPDPContexts()
{
	if (!Adrastea_SendRequest("AT+CGDCONT?\r\n"))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of PDP Context event arguments.
 *
 * @param[out] dataP PDP Context is returned in this argument. See ATPacketDomain_PDP_Context_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ParsePDPContextEvent(char *pEventArguments, ATPacketDomain_PDP_Context_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->cid, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&argumentsP, (uint8_t*) &dataP->pdpType, ATPacketDomain_PDP_Type_Strings, ATPacketDomain_PDP_Type_NumberOfValues, 30,
	ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->apnName, ATCOMMAND_ARGUMENT_DELIM, sizeof(ATCommon_APN_Name_t)))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set PDP Context State (using the AT+CGACT command).
 *
 * @param[in] cidstate PDP Context CID State. See ATPacketDomain_PDP_Context_CID_State_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_SetPDPContextState(ATPacketDomain_PDP_Context_CID_State_t cidstate)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CGACT=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, cidstate.state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, cidstate.cid, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read State of PDP Contexts (using the AT+CGACT command).
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ReadPDPContextsState()
{
	if (!Adrastea_SendRequest("AT+CGACT?\r\n"))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_PacketDomain), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of PDP Context State event arguments.
 *
 * @param[out] dataP PDP Context State is returned in this argument. See ATPacketDomain_PDP_Context_CID_State_t.
 *
 * @return true if successful, false otherwise
 */
bool ATPacketDomain_ParsePDPContextStateEvent(char *pEventArguments, ATPacketDomain_PDP_Context_CID_State_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->cid, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}
