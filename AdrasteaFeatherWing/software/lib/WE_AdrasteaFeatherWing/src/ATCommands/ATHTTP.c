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
 * @brief AT commands for HTTP functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATHTTP.h"
#include "../Adrastea.h"

static const char *ATHTTP_Event_Strings[ATHTTP_Event_NumberOfValues] = {
		"PUTCONF",
		"POSTCONF",
		"DELCONF",
		"GETRCV",
		"SESTERM",
		"ALL" };

/**
 * @brief Configure Nodes (using the AT%HTTPCFG command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] addr Ip address or URL for this profile.
 *
 * @param[in] username Username if authentication is required (optional pass NULL to skip).
 *
 * @param[in] password Password if authentication is required (optional pass NULL to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ConfigureNodes(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATCommon_Auth_Username_t username, ATCommon_Auth_Password_t password)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCFG=\"NODES\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, addr, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (username != NULL && password != NULL)
	{

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure TLS (using the AT%HTTPCFG command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] authMode Authentication mode of TLS.
 *
 * @param[in] tlsProfileID Profile ID of TLS configuration.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ConfigureTLS(ATHTTP_Profile_ID_t profileID, ATCommon_TLS_Auth_Mode_t authMode, ATCommon_TLS_Profile_ID_t tlsProfileID)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCFG=\"TLS\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, authMode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, tlsProfileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure IP (using the AT%HTTPCFG command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] sessionID Session ID (optional pass ATHTTP_IP_Session_ID_Invalid to skip).
 *
 * @param[in] ipFormat IP Address format (optional pass ATHTTP_IP_Addr_Format_Invalid to skip).
 *
 * @param[in] destPort Destination Port (optional pass ATCommon_Port_Number_Invalid to skip).
 *
 * @param[in] sourcePort Source Port (optional pass ATCommon_Port_Number_Invalid to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ConfigureIP(ATHTTP_Profile_ID_t profileID, ATHTTP_IP_Session_ID_t sessionID, ATHTTP_IP_Addr_Format_t ipFormat, ATCommon_Port_Number_t destPort, ATCommon_Port_Number_t sourcePort)
{
	Adrastea_optionalParamsDelimCount = 1;

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCFG=\"IP\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (sessionID != ATHTTP_IP_Session_ID_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, sessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (ipFormat != ATHTTP_IP_Addr_Format_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, ipFormat, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (destPort != ATCommon_Port_Number_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, destPort, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (sourcePort != ATCommon_Port_Number_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, sourcePort, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure Format (using the AT%HTTPCFG command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] responseHeader Configure if headers should be present in the response.
 *
 * @param[in] requestHeader Configure if headers should be present in the request.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ConfigureFormat(ATHTTP_Profile_ID_t profileID, ATHTTP_Header_Presence_t responseHeader, ATHTTP_Header_Presence_t requestHeader)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCFG=\"FORMAT\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, 0, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, responseHeader, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, requestHeader, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure Timeout (using the AT%HTTPCFG command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] timeout Configure Server response timeout in seconds.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ConfigureTimeout(ATHTTP_Profile_ID_t profileID, ATHTTP_Timeout_t timeout)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCFG=\"TIMEOUT\",");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, timeout, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set HTTP Notification Events (using the AT%HTTPEV command).
 *
 * @param[in] event HTTP event type. See ATHTTP_Event_t.
 *
 * @param[in] state Event State
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_SetHTTPUnsolicitedNotificationEvents(ATHTTP_Event_t event, ATCommon_Event_State_t state)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPEV=");

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ATHTTP_Event_Strings[event], ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

bool GETDELETE_Common(char *pRequestCommand, ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATHTTP_Header_Presence_t responseHeader, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	Adrastea_optionalParamsDelimCount = 1;

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, addr, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, 0, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (responseHeader != ATHTTP_Header_Presence_Invalid)
	{
		if (!ATCommand_AppendArgumentInt(pRequestCommand, responseHeader, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
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

	if (headers != NULL && headersCount != 0)
	{
		for (int i = 0; i < headersCount - 1; i++)
		{
			if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, headers[i], ATCOMMAND_ARGUMENT_DELIM))
			{
				return false;
			}
		}

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, headers[headersCount - 1], ATCOMMAND_STRING_TERMINATE))
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

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure GET request (using the AT%HTTPCMD command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] addr Ip address or URL for this request.
 *
 * @param[in] responseHeader Configure if headers should be present in the response of this request.
 *
 * @param[in] headers Headers array to be passed in this request (optional pass NULL to skip).
 *
 * @param[in] headersCount Headers count in headers array to be passed in this request (optional pass 0 to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_GET(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATHTTP_Header_Presence_t responseHeader, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCMD=\"GET\",");

	return GETDELETE_Common(pRequestCommand, profileID, addr, responseHeader, headers, headersCount);
}

/**
 * @brief Configure DELETE request (using the AT%HTTPCMD command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] addr Ip address or URL for this request.
 *
 * @param[in] responseHeader Configure if headers should be present in the response of this request.
 *
 * @param[in] headers Headers array to be passed in this request (optional pass NULL to skip).
 *
 * @param[in] headersCount Headers count in headers array to be passed in this request (optional pass 0 to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_DELETE(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, ATHTTP_Header_Presence_t responseHeader, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPCMD=\"DELETE\",");

	return GETDELETE_Common(pRequestCommand, profileID, addr, responseHeader, headers, headersCount);
}

bool POSTPUT_Common(char *pRequestCommand, ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, char *body, ATHTTP_Body_Size_t bodySize, char *contentType, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	Adrastea_optionalParamsDelimCount = 1;

	char crchar[] = {
			'\r',
			ATCOMMAND_STRING_TERMINATE };

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, bodySize, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, addr, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (contentType != NULL)
	{
		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, contentType, ATCOMMAND_ARGUMENT_DELIM))
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

	if (headers != NULL && headersCount != 0)
	{
		for (int i = 0; i < headersCount - 1; i++)
		{
			if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, headers[i], ATCOMMAND_ARGUMENT_DELIM))
			{
				return false;
			}
		}

		if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, headers[headersCount - 1], ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}

		Adrastea_optionalParamsDelimCount = 0;
	}

	pRequestCommand[strlen(pRequestCommand) - Adrastea_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

	if (!ATCommand_AppendArgumentString(pRequestCommand, crchar, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, body, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!Adrastea_SendRequest(pRequestCommand))
	{
		return false;
	}

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, NULL))
	{
		return false;
	}

	return true;
}

/**
 * @brief Configure POST request (using the AT%HTTPSEND command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] addr Ip address or URL for this request.
 *
 * @param[in] body Body of the http request.
 *
 * @param[in] bodySize Size of the body in the http request.
 *
 * @param[in] contentType Content type of the body (optional pass NULL to skip).
 *
 * @param[in] headers Headers array to be passed in this request (optional pass NULL to skip).
 *
 * @param[in] headersCount Headers count in headers array to be passed in this request (optional pass 0 to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_POST(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, char *body, ATHTTP_Body_Size_t bodySize, char *contentType, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPSEND=\"POST\",");

	return POSTPUT_Common(pRequestCommand, profileID, addr, body, bodySize, contentType, headers, headersCount);
}

/**
 * @brief Configure PUT request (using the AT%HTTPSEND command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] addr Ip address or URL for this request.
 *
 * @param[in] body Body of the http request.
 *
 * @param[in] bodySize Size of the body in the http request.
 *
 * @param[in] contentType Content type of the body (optional pass NULL to skip).
 *
 * @param[in] headers Headers array to be passed in this request (optional pass NULL to skip).
 *
 * @param[in] headersCount Headers count in headers array to be passed in this request (optional pass 0 to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_PUT(ATHTTP_Profile_ID_t profileID, ATCommon_IP_Addr_t addr, char *body, ATHTTP_Body_Size_t bodySize, char *contentType, char *headers[], ATHTTP_Header_Count_t headersCount)
{
	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPSEND=\"PUT\",");

	return POSTPUT_Common(pRequestCommand, profileID, addr, body, bodySize, contentType, headers, headersCount);
}

/**
 * @brief Parses the value of GET event arguments.
 *
 * @param[out] dataP the result of the HTTP event is returned in this argument. See ATHTTP_Event_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ParseGETEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP)
{
	if (dataP == NULL || pEventArguments == NULL)
	{
		return false;
	}

	char *argumentsP = pEventArguments;

	if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->profileID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	switch (Adrastea_CountArgs(argumentsP))
	{
	case 1:
	{
		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->state, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}
		break;
	}
	default:
		if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->state, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}
		break;
	}

	return true;
}

/**
 * @brief Read the response (using the AT%HTTPREAD command).
 *
 * @param[in] profileID HTTP Profile. See ATHTTP_Profile_ID_t.
 *
 * @param[in] maxLength max length of bytes to be read.
 *
 * @param[out] response Response is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ReadResponse(ATHTTP_Profile_ID_t profileID, ATHTTP_Data_Length_t maxLength, ATHTTP_Response_t *response)
{
	if (response == NULL)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;

	strcpy(pRequestCommand, "AT%HTTPREAD=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, maxLength, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC ), ATCOMMAND_STRING_TERMINATE))
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

	memset(pResponseCommand, 0, sizeof(AT_commandBuffer));

	if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_HTTP), Adrastea_CNFStatus_Success, pResponseCommand))
	{
		return false;
	}

	pResponseCommand += 1;

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &response->dataLength, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &response->receivedLength, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	size_t responseByteCount = 0;

	while (1)
	{

		if (!ATCommand_GetNextArgumentString(&pResponseCommand, &response->responseBody[responseByteCount], ATCOMMAND_STRING_TERMINATE, maxLength))
		{
			return false;
		}

		if ((strlen(response->responseBody) - responseByteCount) == 0)
		{
			break;
		}

		responseByteCount = strlen(response->responseBody);

		memcpy(&response->responseBody[responseByteCount], ATCOMMAND_CRLF, 2);
		responseByteCount += 2;

		response->responseBody[responseByteCount] = ATCOMMAND_STRING_TERMINATE;

	}

	return true;
}

/**
 * @brief Parses the value of DELETE event arguments.
 *
 * @param[out] dataP the result of the HTTP event is returned in this argument. See ATHTTP_Event_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ParseDELETEEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP)
{
	return ATHTTP_ParseGETEvent(pEventArguments, dataP);
}

/**
 * @brief Parses the value of POST event arguments.
 *
 * @param[out] dataP the result of the HTTP event is returned in this argument. See ATHTTP_Event_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ParsePOSTEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP)
{
	return ATHTTP_ParseGETEvent(pEventArguments, dataP);
}

/**
 * @brief Parses the value of PUT event arguments.
 *
 * @param[out] dataP the result of the HTTP event is returned in this argument. See ATHTTP_Event_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ParsePUTEvent(char *pEventArguments, ATHTTP_Event_Result_t *dataP)
{
	return ATHTTP_ParseGETEvent(pEventArguments, dataP);
}
