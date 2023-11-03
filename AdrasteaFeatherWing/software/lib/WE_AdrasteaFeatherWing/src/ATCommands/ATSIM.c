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
 * @brief AT commands for SIM functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATSIM.h"

#include "../Adrastea.h"

static const char *ATSIM_Facility_Strings[ATSIM_Facility_NumberOfValues] = {
		"SC",
		"P2",
		"PN",
		"PU",
		"PS", };

static const char *ATSIM_PIN_Status_Strings[ATSIM_PIN_Status_NumberOfValues] = {
		"READY",
		"SIM PIN",
		"SIM PUK",
		"PH-SIM PIN",
		"PH-FSIM PIN",
		"PH-FSIM PUK",
		"SIM PIN2",
		"SIM PUK2",
		"PH-NET PIN",
		"PH-NET PUK",
		"PH-NETSUB PIN",
		"PH-NETSUB PUK",
		"PH-SP PIN",
		"PH-SP PUK",
		"PH-CORP PIN",
		"PH-CORP PUK", };

/**
 * @brief Read International Mobile Subscriber Identity (using the AT+CIMI command).
 *
 * @param[out] imsiP IMSI is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_RequestInternationalMobileSubscriberIdentity(ATSIM_IMSI_t *imsiP)
{
	if (imsiP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CIMI\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*) *imsiP, ATCOMMAND_STRING_TERMINATE, sizeof(*imsiP)))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set Facility Lock (using the AT+CLCK command).
 *
 * @param[in] facility Facility Lock. See ATSIM_Facility_t.
 *
 * @param[in] mode Lock Mode. See ATSIM_Lock_Mode_t.
 *
 * @param[in] pin PIN (optional pass NULL to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_SetFacilityLock(ATSIM_Facility_t facility, ATSIM_Lock_Mode_t mode, ATSIM_PIN_t pin)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CLCK=");

	if (facility == ATSIM_Facility_P2)
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (pin != NULL)
	{

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read Facility Lock (using the AT+CLCK command).
 *
 * @param[in] facility Facility Lock.
 *
 * @param[out] statusP Lock Status is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_ReadFacilityLock(ATSIM_Facility_t facility, ATSIM_Lock_Status_t *statusP)
{
	if (statusP == NULL)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CLCK=");

	if (facility == ATSIM_Facility_P2)
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, 2, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, statusP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

//TODO check this command
/**
 * @brief Read Subscriber Number (using the AT+CNUM command).
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_ReadSubscriberNumber()
{
	if (!Adrastea_SendRequest("AT+CNUM\r\n"))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Read PIN Status (using the AT+CPIN command).
 *
 * @param[out] statusP PIN Status is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_ReadPinStatus(ATSIM_PIN_Status_t *statusP)
{
	if (statusP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT+CPIN?\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentEnum(&pResponseCommand, (uint8_t*) statusP, ATSIM_PIN_Status_Strings, ATSIM_PIN_Status_NumberOfValues, 30,
	ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	return true;
}

/**
 * @brief Enter PIN (using the AT+CPIN command).
 *
 * @param[in] pin1 PIN1.
 *
 * @param[in] pin2 PIN2 (optional pass NULL to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_EnterPin(ATSIM_PIN_t pin1, ATSIM_PIN_t pin2)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CPIN=");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin1, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (pin2 != NULL)
	{

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin2, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Change Password (using the AT+CPWD command).
 *
 * @param[in] facility Facility Lock. See ATSIM_Facility_t.
 *
 * @param[in] oldpassword Old Password.
 *
 * @param[in] newpassword New Password.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_ChangePassword(ATSIM_Facility_t facility, ATSIM_PIN_t oldpassword, ATSIM_PIN_t newpassword)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CPWD=");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, oldpassword, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, newpassword, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Execute Restricted SIM Access commands (using the AT+CRSM command).
 *
 * @param[in] cmd Restricted SIM Access Command. See ATSIM_Restricted_Access_Command_t.
 *
 * @param[in] fileID Restricted Access File ID.
 *
 * @param[in] p1 P1.
 *
 * @param[in] p2 P2.
 *
 * @param[in] p3 P3.
 *
 * @param[in] dataWritten Data to be written depending on command (optional pass NULL to skip).
 *
 * @param[out] cmdResponse Response of Restricted Access Command.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_RestrictedSIMAccess(ATSIM_Restricted_Access_Command_t cmd, ATSIM_Restricted_Access_File_ID fileID, ATSIM_Restricted_Access_P1 p1, ATSIM_Restricted_Access_P2 p2, ATSIM_Restricted_Access_P3 p3, char *dataWritten, ATSIM_Restricted_Access_Response_t *cmdResponse)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT+CRSM=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, cmd, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, fileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, p1, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, p2, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, p3, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (dataWritten != NULL)
	{
		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, dataWritten, ATCOMMAND_STRING_TERMINATE))
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

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw1, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	switch (Adrastea_CountArgs(pResponseCommand))
	{
	case 1:
	{
		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw2, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}
		break;
	}
	case 2:
	{
		if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw2, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}
		if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, cmdResponse->responseRead, ATCOMMAND_STRING_TERMINATE, cmdResponse->responseReadMaxBufferSize))
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
 * @brief Read Integrated Circuit Card Identifier (using the AT%CCID command).
 *
 * @param[out] iccidP ICCID is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSIM_RequestIntegratedCircuitCardIdentifier(ATSIM_ICCID_t *iccidP)
{
	if (iccidP == NULL)
	{
		return false;
	}

	if (!Adrastea_SendRequest("AT%CCID\r\n"))
	{
		return false;
	}

	char *pResponseCommand = AT_commandBuffer;

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_SIM), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*) *iccidP, ATCOMMAND_STRING_TERMINATE, sizeof(*iccidP)))
	{
		return false;
	}

	return true;
}

/**
 * @brief Parses the value of Subscriber Number event arguments.
 *
 * @param[out] dataP Subscriber Number is returned in this argument. See ATSIM_Subscriber_Number_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSMS_ParseSubscriberNumberEvent(char *pEventArguments, ATSIM_Subscriber_Number_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	argumentsP += 1;

	char alphaN[30];

	if (!ATCommand_GetNextArgumentString(&argumentsP, alphaN, ATCOMMAND_ARGUMENT_DELIM, sizeof(alphaN)))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->number, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataP->number)))
	{
		return false;
	}

	switch (Adrastea_CountArgs(argumentsP))
	{
	case 1:
	{
		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->numberType, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}
		break;
	}
	default:
	{
		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->numberType, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8 ), ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}
		break;
	}
	}

	return true;
}
