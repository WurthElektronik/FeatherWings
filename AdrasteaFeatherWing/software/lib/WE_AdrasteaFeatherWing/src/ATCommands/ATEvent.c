/**
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
 **/

/**
 * @file
 * @brief AT event definitions.
 */

#include "ATEvent.h"

#include "../Adrastea.h"
#include <ATCommands.h>

static const char *ATEvent_Strings[ATEvent_NumberOfValues] = {
		"invalid",
		"COPN",
		"PINGCMD",
		"DNSRSLV",
		"CEREG",
		"CGDCONT",
		"CGACT",
		"IGNSSINFO",
		"NMEA",
		"SESSIONSTAT",
		"ALLOWSTAT",
		"CONCONF",
		"DISCONF",
		"SUBCONF",
		"UNSCONF",
		"PUBCONF",
		"PUBRCV",
		"CONFAIL",
		"CONCONF",
		"DISCONF",
		"SUBCONF",
		"UNSCONF",
		"PUBCONF",
		"PUBRCV",
		"CONFAIL",
		"PUTCONF",
		"POSTCONF",
		"DELCONF",
		"GETRCV",
		"SESTERM",
		"CMGR",
		"CMGL",
		"CMTI",
		"CMS ERROR",
		"1",
		"2",
		"3",
		"4",
		"SOCKETCMD",
		"CNUM" };

static bool ATEvent_ParseEventSubType(const char *eventSubTypeString, ATEvent_t eventMainType, ATEvent_t *pEventSubType);

/**
 * @brief Parses the received AT command and returns the corresponding ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+' or '%'
 * @param[out] pEvent ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseEventType(char **pAtCommand, ATEvent_t *pEvent)
{
	char cmdName[32], option[64];
	*pEvent = ATEvent_Invalid;
	char delimiters[] = {
			ATCOMMAND_EVENT_DELIM,
			ATCOMMAND_STRING_TERMINATE };
	if (ATCommand_GetCmdName(pAtCommand, cmdName, sizeof(cmdName), delimiters, sizeof(delimiters)))
	{
		if (0 == strcmp(cmdName, "+COPN"))
		{
			*pEvent = ATEvent_NetService_Operator_Read;
		}
		else if (0 == strcmp(cmdName, "%PINGCMD"))
		{
			*pEvent = ATEvent_Proprietary_Ping_Result;
		}
		else if (0 == strcmp(cmdName, "%DNSRSLV"))
		{
			*pEvent = ATEvent_Proprietary_Domain_Name_Resolve;
		}
		else if (0 == strcmp(cmdName, "+CEREG"))
		{
			*pEvent = ATEvent_PacketDomain_Network_Registration_Status;
		}
		else if (0 == strcmp(cmdName, "+CGDCONT"))
		{
			*pEvent = ATEvent_PacketDomain_PDP_Context;
		}
		else if (0 == strcmp(cmdName, "+CGACT"))
		{
			*pEvent = ATEvent_PacketDomain_PDP_Context_State;
		}
		else if (0 == strcmp(cmdName, "%IGNSSINFO"))
		{
			*pEvent = ATEvent_GNSS_Satalite_Query;
		}
		else if (0 == strcmp(cmdName, "%IGNSSEVU"))
		{
			*pAtCommand += 1;
			if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option)))
			{
				return false;
			}

			ATEvent_ParseEventSubType(option, ATEvent_GNSS, pEvent);
		}
		else if (0 == strcmp(cmdName, "%MQTTEVU"))
		{
			if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option)))
			{
				return false;
			}

			ATEvent_ParseEventSubType(option, ATEvent_MQTT, pEvent);
		}
		else if (0 == strcmp(cmdName, "%AWSIOTEVU"))
		{
			if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option)))
			{
				return false;
			}

			ATEvent_ParseEventSubType(option, ATEvent_MQTT_AWSIOT, pEvent);
		}
		else if (0 == strcmp(cmdName, "%HTTPEVU"))
		{
			if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option)))
			{
				return false;
			}

			ATEvent_ParseEventSubType(option, ATEvent_HTTP, pEvent);
		}
		else if (0 == strcmp(cmdName, "+CMGR"))
		{
			*pEvent = ATEvent_SMS_Read_Message;
		}
		else if (0 == strcmp(cmdName, "+CMGL"))
		{
			*pEvent = ATEvent_SMS_List_Messages;
		}
		else if (0 == strcmp(cmdName, "+CMTI"))
		{
			*pEvent = ATEvent_SMS_Message_Received;
		}
		else if (0 == strcmp(cmdName, "+CMS ERROR"))
		{
			*pEvent = ATEvent_SMS_Error;
		}
		else if (0 == strcmp(cmdName, "%SOCKETEV"))
		{
			if (!ATCommand_GetNextArgumentString(pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option)))
			{
				return false;
			}

			ATEvent_ParseEventSubType(option, ATEvent_Socket, pEvent);
		}
		else if (0 == strcmp(cmdName, "%SOCKETCMD"))
		{
			*pEvent = ATEvent_Socket_Sockets_Read;
		}
		else if (0 == strcmp(cmdName, "+CNUM"))
		{
			*pEvent = ATEvent_SIM_Subscriber_Number;
		}
		else
		{
			return false;
		}
		return true;
	}
	return false;
}

/**
 * @brief Parses an event sub type string (first argument of received event string) to ATEvent_t.
 *
 * @param[in] eventSubTypeString String containing the event's ID
 * @param[in] eventMainType Main event type category (ATEvent_General, ATEvent_Wlan, ATEvent_Socket,
 *            ATEvent_Netapp, ATEvent_MQTT or ATEvent_FatalError)
 * @param[out] pEventSubType ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
static bool ATEvent_ParseEventSubType(const char *eventSubTypeString, ATEvent_t eventMainType, ATEvent_t *pEventSubType)
{
	uint8_t typeCount = 0;

	switch (eventMainType)
	{
	case ATEvent_GNSS:
		typeCount = ATEvent_GNSS_NumberOfValues;
		break;
	case ATEvent_MQTT:
		typeCount = ATEvent_MQTT_NumberOfValues;
		break;
	case ATEvent_MQTT_AWSIOT:
		typeCount = ATEvent_MQTT_AWSIOT_NumberOfValues;
		break;
	case ATEvent_HTTP:
		typeCount = ATEvent_HTTP_NumberOfValues;
		break;
	case ATEvent_Socket:
		typeCount = ATEvent_Socket_NumberOfValues;
		break;
	default:
		break;
	}

	*pEventSubType = ATEvent_Invalid;

	for (int i = 0; i < typeCount; i++)
	{
		ATEvent_t event = eventMainType + (ATEvent_t) i;
		if (0 == strcasecmp(eventSubTypeString, ATEvent_Strings[event]))
		{
			*pEventSubType = event;
			return true;
		}
	}

	return false;
}

/**
 * @brief Returns the name of an event.
 *
 * @param[in] event Type of event
 * @param[out] pEventName Event name
 *
 * @return true if successful, false otherwise
 */
bool ATEvent_GetEventName(ATEvent_t event, char *pEventName)
{
	strcpy(pEventName, ATEvent_Strings[event]);
	return true;
}

