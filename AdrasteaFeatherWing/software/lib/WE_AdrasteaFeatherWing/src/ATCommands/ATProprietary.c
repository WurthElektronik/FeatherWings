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
 * @brief AT commands for Proprietary functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATProprietary.h"

#include "../Adrastea.h"

static const char *ATProprietary_RAT_Strings[ATProprietary_RAT_NumberOfValues] = {
		"DEFAULT",
		"CATM",
		"NBIOT", };

static const char *ATProprietary_IP_Addr_Format_Strings[ATProprietary_IP_Addr_Format_NumberOfValues] = {
		"IP",
		"IPV6",
		"IPV4V6", };

/**
 * @brief Read Network Attachment State (using the AT%CMATT command).
 *
 * @param[out] stateP Network Attachment State is returned in this argument. See ATProprietary_Network_Attachment_State_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ReadNetworkAttachmentState(ATProprietary_Network_Attachment_State_t *stateP)
{

	if (stateP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%CMATT?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, stateP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set Network Attachment State (using the AT%CMATT command).
 *
 * @param[in] state Network Attachment State. See ATProprietary_Network_Attachment_State_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_SetNetworkAttachmentState(ATProprietary_Network_Attachment_State_t state)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CMATT=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read Remaining PIN and PUK Attempts left (using the AT%CPININFO command).
 *
 * @param[out] attemptsP Remaining Attempts are returned in this argument. See ATProprietary_PIN_PUK_Attempts_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ReadRemainingPINPUKAttempts(ATProprietary_PIN_PUK_Attempts_t *attemptsP)
{
	if (attemptsP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%CPININFO\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &attemptsP->pinAttempts, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &attemptsP->pukAttempts, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &attemptsP->pin2Attempts, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &attemptsP->puk2Attempts, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Switch to RAT without Full Reboot (using the AT%RATACT command).
 *
 * @param[in] rat RAT. See ATProprietary_RAT_t.
 *
 * @param[in] storage RAT Storage (optional pass ATProprietary_RAT_Storage_Invalid to skip). See ATProprietary_RAT_Storage_t.
 *
 * @param[in] source RAT Source (optional pass ATProprietary_RAT_Source_Invalid to skip). See ATProprietary_RAT_Source_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_SwitchToRATWithoutFullReboot(ATProprietary_RAT_t rat, ATProprietary_RAT_Storage_t storage, ATProprietary_RAT_Source_t source)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%RATACT=");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATProprietary_RAT_Strings[rat], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (storage != ATProprietary_RAT_Storage_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, storage, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (source != ATProprietary_RAT_Source_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, source, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read RAT Status (using the AT%RATACT command).
 *
 * @param[out] ratstatusP RAT Status is returned in this argument. See ATProprietary_RAT_Status_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ReadRATStatus(ATProprietary_RAT_Status_t *ratstatusP)
{
	if (ratstatusP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%RATACT?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, (uint8_t*) &ratstatusP->rat, ATProprietary_RAT_Strings, ATProprietary_RAT_NumberOfValues, 30,
	ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &ratstatusP->mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &ratstatusP->source, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set Boot Delay (using the AT%SETBDELAY command).
 *
 * @param[in] delay Boot Delay in seconds.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_SetBootDelay(ATProprietary_Boot_Delay_t delay)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%SETBDELAY=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, delay, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @briefResolve Domain Name (using the AT%DNSRSLV command).
 *
 * @param[in] sessionid Session ID.
 *
 * @param[in] domain Domain Name (URL).
 *
 * @param[in] format IP Address Format (optional pass ATProprietary_IP_Addr_Format_Invalid to skip). See ATProprietary_IP_Addr_Format_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ResolveDomainName(ATCommon_Session_ID_t sessionid, ATProprietary_Domain_Name_t domain, ATProprietary_IP_Addr_Format_t format)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%DNSRSLV=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, sessionid, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, domain, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (format != ATProprietary_IP_Addr_Format_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of Resolve Domain Name event arguments.
 *
 * @param[out] dataP Domain Name Resolve Result is returned in this argument. See ATPacketDomain_PDP_Context_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ParseResolveDomainNameEvent(char *pEventArguments, ATProprietary_Domain_Name_Resolve_Result_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->addr, ATCOMMAND_STRING_TERMINATE, sizeof(dataP->addr)))
	{
		return false;
	}

	return true;
}

/**
 * @brief Ping an address (using the AT%PINGCMD command).
 *
 * @param[in] format IP Address Format. See ATProprietary_IP_Addr_Format_t.
 *
 * @param[in] destaddr Destination IP Address.
 *
 * @param[in] packetcount Packet Count (optional pass ATProprietary_Ping_Packet_Count_Invalid to skip).
 *
 * @param[in] packetsize Packet Size in Bytes (optional pass ATProprietary_Ping_Packet_Size_Invalid to skip).
 *
 * @param[in] timeout Ping Timeout (optional pass ATProprietary_Ping_Timeout_Invalid to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_Ping(ATProprietary_IP_Addr_Format_t format, ATCommon_IP_Addr_t destaddr, ATProprietary_Ping_Packet_Count_t packetcount, ATProprietary_Ping_Packet_Size_t packetsize, ATProprietary_Ping_Timeout_t timeout)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%PINGCMD=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, destaddr, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (packetcount != ATProprietary_Ping_Packet_Count_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, packetcount, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (packetsize != ATProprietary_Ping_Packet_Size_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, packetsize, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (timeout != ATProprietary_Ping_Timeout_Invalid)
	{

		if (!ATCommand_AppendArgumentInt(pRequestCommand, timeout, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of Ping Result event arguments.
 *
 * @param[out] dataP Ping Result is returned in this argument. See ATPacketDomain_PDP_Context_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ParsePingResultEvent(char *pEventArguments, ATProprietary_Ping_Result_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->id, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->addr, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataP->addr)))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->rtt, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE16 ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->ttl, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE16 ), ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename Name of file to read.
 *
 * @param[out] dataP Contents of file is read to this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ReadCredential(ATProprietary_File_Name_t filename, char *dataP, uint16_t dataMaxBufferSize)
{
	if (dataP == NULL)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CERTCMD=\"READ\",");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_STRING_TERMINATE))
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

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, dataP, ATCOMMAND_STRING_TERMINATE, dataMaxBufferSize))
	{
		return false;
	}

	return true;
}

/**
 * @brief Write a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename Name of file to write.
 *
 * @param[in] format Format of credential. See ATProprietary_Credential_Format_t.
 *
 * @param[out] data Data to write to file.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_WriteCredential(ATProprietary_File_Name_t filename, ATProprietary_Credential_Format_t format, char *data)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CERTCMD=\"WRITE\",");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, ATCOMMAND_STRING_TERMINATE))
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

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	return true;
}

/**
 * @brief List filenames of credentials stored (using the AT%CERTCMD command).
 *
 * @param[out] filenamesList List of credentials Stored. (Free memory from filenamesList->filenames after usage)
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ListCredentials(ATProprietary_File_Names_List_t *filenamesList)
{
	if (filenamesList == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%CERTCMD=\"DIR\"\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	filenamesList->filenames = NULL;

	filenamesList->count = Adrastea_CountArgs(pResponseCommand);

	if (filenamesList->count != 0)
	{

		filenamesList->filenames = malloc(filenamesList->count * sizeof(ATProprietary_File_Name_t));

		if (filenamesList->filenames == NULL)
		{
			return false;
		}

		for (uint8_t i = 0; i < filenamesList->count - 1; i++)
		{
			if (!ATCommand_GetNextArgumentString(&pResponseCommand, filenamesList->filenames[i], ATCOMMAND_ARGUMENT_DELIM, sizeof(ATProprietary_File_Name_t)))
			{
				return false;
			}
		}

		if (!ATCommand_GetNextArgumentString(&pResponseCommand, filenamesList->filenames[filenamesList->count - 1], ATCOMMAND_STRING_TERMINATE, sizeof(ATProprietary_File_Name_t)))
		{
			return false;
		}

	}

	return true;
}

/**
 * @brief Delete a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename Name of file to delete.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_DeleteCredential(ATProprietary_File_Name_t filename)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CERTCMD=\"DELETE\",");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief List Configured TLS Profile IDS (using the AT%CERTCFG command).
 *
 * @param[out] profileIDsList List of TLS profile IDs. (Free memory from profileIDsList->profileIDs after usage)
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ListTLSProfiles(ATProprietary_TLS_Profile_ID_List_t *profileIDsList)
{
	if (profileIDsList == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%CERTCFG?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	pResponseCommand[0] = '\0';

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	profileIDsList->profileIDs = NULL;

	if (Adrastea_CountArgs(pResponseCommand) != 0)
	{

		pResponseCommand += 1;

		profileIDsList->count = Adrastea_CountArgs(pResponseCommand);

		profileIDsList->profileIDs = malloc(profileIDsList->count * sizeof(ATCommon_TLS_Profile_ID_t));

		if (profileIDsList->profileIDs == NULL)
		{
			return false;
		}

		for (uint8_t i = 0; i < profileIDsList->count - 1; i++)
		{
			if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &profileIDsList->profileIDs[i], (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED ), ATCOMMAND_ARGUMENT_DELIM))
			{
				return false;
			}
		}

		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &profileIDsList->profileIDs[profileIDsList->count - 1], (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

	}
	else
	{
		profileIDsList->count = 0;
	}

	return true;
}

/**
 * @brief Add TLS Profile (using the AT%CERTCFG command).
 *
 * @param[in] profileID Profile ID of TLS Profile.
 *
 * @param[in] CA Name of CA (optional pass NULL to skip).
 *
 * @param[in] CAPath Path of CA (optional pass NULL to skip).
 *
 * @param[in] deviceCert Name of device Certificate (optional pass NULL to skip).
 *
 * @param[in] deviceKey Name of device Key (optional pass NULL to skip).
 *
 * @param[in] pskID Name of PSK ID (optional pass NULL to skip).
 *
 * @param[in] pskKey Name of PSK Key (optional pass NULL to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_AddTLSProfile(ATCommon_TLS_Profile_ID_t profileID, ATProprietary_File_Name_t CA, ATProprietary_File_Path_t CAPath, ATProprietary_File_Name_t deviceCert, ATProprietary_File_Name_t deviceKey, ATProprietary_File_Name_t pskID, ATProprietary_File_Name_t pskKey)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CERTCFG=\"ADD\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (CA != NULL)
	{

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, CA, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount = 1;

		if (CAPath != NULL)
		{
			if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, CAPath, ATCOMMAND_ARGUMENT_DELIM))
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

	}
	else
	{

		if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount++;

		if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount++;
	}

	if (deviceCert != NULL && deviceKey != NULL)
	{

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, deviceCert, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, deviceKey, ATCOMMAND_ARGUMENT_DELIM))
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

		if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount++;
	}

	if (pskID != NULL && pskKey != NULL)
	{

		if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Delete TLS Profile (using the AT%CERTCFG command).
 *
 * @param[in] profileID Profile ID of TLS Profile.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_DeleteTLSProfile(ATCommon_TLS_Profile_ID_t profileID)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%CERTCFG=\"DELETE\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set PDN Parameters (using the AT%PDNSET command).
 *
 * @param[in] parameters PDN Parameters. See ATProprietary_PDN_Parameters_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_SetPDNParameters(ATProprietary_PDN_Parameters_t parameters)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%PDNSET=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters.sessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (parameters.apnName != NULL)
	{
		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, parameters.apnName, ATCOMMAND_ARGUMENT_DELIM))
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

	if (parameters.ipFormat != ATProprietary_IP_Addr_Format_Invalid)
	{
		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATProprietary_IP_Addr_Format_Strings[parameters.ipFormat], ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read PDN Parameters (using the AT%PDNSET command).
 *
 * @param[out] parameters PDN Parameters are returned in this argument. See ATProprietary_PDN_Parameters_t.
 *
 * @return true if successful, false otherwise
 */
bool ATProprietary_ReadPDNParameters(ATProprietary_PDN_Parameters_t *parameters)
{
	if (parameters == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%PDNSET?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Proprietary), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &parameters->sessionID, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentString(&pResponseCommand, parameters->apnName, ATCOMMAND_ARGUMENT_DELIM, sizeof(ATCommon_APN_Name_t)))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentEnum(&pResponseCommand, (uint8_t*) &parameters->ipFormat, ATProprietary_IP_Addr_Format_Strings, ATProprietary_IP_Addr_Format_NumberOfValues, 30,
	ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	return true;
}
