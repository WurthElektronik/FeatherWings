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
 * @brief AT commands for MQTT functionality.
 */
#include <AdrasteaI/ATCommands/ATMQTT.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* AdrasteaI_ATMQTT_Event_Strings[AdrasteaI_ATMQTT_Event_NumberOfValues] = {"CONCONF", "DISCONF", "SUBCONF", "UNSCONF", "PUBCONF", "PUBRCV", "CONFAIL", "ALL"};

/**
 * @brief Configure Nodes (using the AT%MQTTCFG command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] clientID Unique client ID to connect to broker.
 *
 * @param[in] addr Broker ip address or URL.
 *
 * @param[in] username Username if authentication is required (optional pass empty string to skip).
 *
 * @param[in] password Password if authentication is required (optional pass empty string to skip).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ConfigureNodes(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Client_ID_t clientID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATCommon_Auth_Username_t username, AdrasteaI_ATCommon_Auth_Password_t password)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCFG=\"NODES\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, clientID, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, addr, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if ((strlen(username) != 0) && (strlen(password) != 0))
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        AdrasteaI_optionalParamsDelimCount = 0;
    }

    if (username != NULL && password == NULL)
    {

        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure TLS (using the AT%MQTTCFG command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] authMode Authentication mode of TLS.
 *
 * @param[in] tlsProfileID Profile ID of TLS configuration.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ConfigureTLS(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATCommon_TLS_Auth_Mode_t authMode, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCFG=\"TLS\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, authMode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure IP (using the AT%MQTTCFG command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] sessionID Session ID (optional pass AdrasteaI_ATMQTT_IP_Session_ID_Invalid to skip).
 *
 * @param[in] ipFormat IP Address format (optional pass AdrasteaI_ATMQTT_IP_Addr_Format_Invalid to skip).
 *
 * @param[in] port Destination Port (optional pass AdrasteaI_ATCommon_Port_Number_Invalid to skip).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ConfigureIP(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat, AdrasteaI_ATCommon_Port_Number_t port)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCFG=\"IP\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (sessionID != AdrasteaI_ATMQTT_IP_Session_ID_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, sessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
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

    if (ipFormat != AdrasteaI_ATMQTT_IP_Addr_Format_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, ipFormat, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
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

    if (port != AdrasteaI_ATCommon_Port_Number_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, port, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure Will Message (using the AT%MQTTCFG command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] presence Configure if will message is enabled.
 *
 * @param[in] qos Will Quality of Service.
 *
 * @param[in] retain Whether or not the Will Message will be retained across disconnects.
 *
 * @param[in] topic Topic Name.
 *
 * @param[in] message Will Message.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ConfigureWillMessage(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_WILL_Presence_t presence, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topic, char* message)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCFG=\"WILLMSG\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, presence, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, qos, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, retain, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topic, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, message, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure Protocol (using the AT%MQTTCFG command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] keepAlive Keep Alive time in seconds.
 *
 * @param[in] cleanSession determine if server should discard subscriptions or not after disconnect.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ConfigureProtocol(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_Clean_Session_t cleanSession)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCFG=\"PROTOCOL\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, 0, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, keepAlive, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, cleanSession, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set MQTT Notification Events (using the AT%MQTTEV command).
 *
 * @param[in] event MQTT event type. See AdrasteaI_ATMQTT_Event_t.
 *
 * @param[in] state Event State
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_SetMQTTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTEV=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATMQTT_Event_Strings[event], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Connection Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the connection result is returned in this argument. See AdrasteaI_ATMQTT_Connection_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->connID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (ATCommand_CountArgs(argumentsP))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        case 2:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->errorCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Parses the value of Disconnection Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the disconnection result is returned in this argument. See AdrasteaI_ATMQTT_Connection_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP) { return AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Subscription Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the subscription result is returned in this argument. See AdrasteaI_ATMQTT_Subscription_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->connID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->msgID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (ATCommand_CountArgs(argumentsP))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        case 2:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->errorCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Parses the value of Unsubscription Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the unsubscription result is returned in this argument. See AdrasteaI_ATMQTT_Subscription_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP) { return AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Publication Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the publication result is returned in this argument. See AdrasteaI_ATMQTT_Publication_Confirmation_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParsePublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Confirmation_Result_t* dataP) { return AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Publication Received event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the received publication is returned in this argument. See AdrasteaI_ATMQTT_Publication_Confirmation_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParsePublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Received_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->connID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->msgID, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->topicName, ATCOMMAND_ARGUMENT_DELIM, sizeof(AdrasteaI_ATMQTT_Topic_Name_t)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->payloadSize, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, dataP->payload, ATCOMMAND_STRING_TERMINATE, dataP->payloadMaxBufferSize))
    {
        return false;
    }

    return true;
}

/**
 * @brief Connect to Broker (using the AT%MQTTCMD command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_Connect(AdrasteaI_ATMQTT_Conn_ID_t connID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCMD=\"CONNECT\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Disconnect from Broker (using the AT%MQTTCMD command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_Disconnect(AdrasteaI_ATMQTT_Conn_ID_t connID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCMD=\"DISCONNECT\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Subscribe to Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] qos Quality of Service.
 *
 * @param[in] topicName MQTT Topic Name.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_Subscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t QoS, AdrasteaI_ATMQTT_Topic_Name_t topicName)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCMD=\"SUBSCRIBE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, QoS, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Unsubscribe from Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] topicName MQTT Topic Name.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_Unsubscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Topic_Name_t topicName)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCMD=\"UNSUBSCRIBE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Publish to Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] qos Quality of Service.
 *
 * @param[in] retain Whether or not the Will Message will be retained across disconnects.
 *
 * @param[in] topicName MQTT Topic Name.
 *
 * @param[in] payload Payload to be published to topic.
 *
 * @param[in] payloadSize Size of the payload in bytes.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_Publish(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload, AdrasteaI_ATMQTT_Payload_Size_t payloadSize)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%MQTTCMD=\"PUBLISH\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, connID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, retain, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, qos, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, payloadSize + 1, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, payload, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure AWS connection (using the AT%AWSIOTCFG command).
 * *
 * @param[in] url URL of endpoint.
 *
 * @param[in] profileID Profile ID of TLS configuration.
 *
 * @param[in] clientID Unique Client ID.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTConfigureConnection(AdrasteaI_ATCommon_IP_Addr_t url, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID, AdrasteaI_ATMQTT_Client_ID_t clientID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCFG=\"CONN\",");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, url, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, clientID, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure AWS IP (using the AT%AWSIOTCFG command).
 * *
 * @param[in] sessionID Session ID (optional pass AdrasteaI_ATMQTT_IP_Session_ID_Invalid to skip).
 *
 * @param[in] ipFormat ip address format (optional pass AdrasteaI_ATMQTT_IP_Addr_Format_Invalid to skip).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTConfigureIP(AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCFG=\"IP\",");

    if (sessionID != AdrasteaI_ATMQTT_IP_Session_ID_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, sessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
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

    if (ipFormat != AdrasteaI_ATMQTT_IP_Addr_Format_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, ipFormat, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Configure AWS Protocol (using the AT%AWSIOTCFG command).
 * *
 * @param[in] keepAlive Keep Alive.
 *
 * @param[in] qos Quality of Service.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTConfigureProtocol(AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_AWSIOT_QoS_t qos)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCFG=\"PROTOCOL\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, keepAlive, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, qos, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set AWS IoT Notification Events (using the AT%AWSIOTEV command).
 *
 * @param[in] event MQTT event type. See AdrasteaI_ATMQTT_Event_t.
 *
 * @param[in] state Event State
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_SetAWSIOTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTEV=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATMQTT_Event_Strings[event], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Connection Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the event result code is returned in this argument. See AdrasteaI_ATMQTT_Event_Result_Code_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, dataP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Disconnection Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the event result code is returned in this argument. See AdrasteaI_ATMQTT_Event_Result_Code_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP) { return AdrasteaI_ATMQTT_ParseAWSIOTConnectionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Subscription Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the subscription result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->msgID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->resultCode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Unsubscription Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the unsubscription result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP) { return AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Publication Confirmation event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the publication confirmation result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t* dataP) { return AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(pEventArguments, dataP); }

/**
 * @brief Parses the value of Publication Received event arguments.
 *
 * @param[in]  pEventArguments String containing arguments of the AT command
 * @param[out] dataP the publication received result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->topicName, ATCOMMAND_ARGUMENT_DELIM, sizeof(AdrasteaI_ATMQTT_Topic_Name_t)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->payload, ATCOMMAND_STRING_TERMINATE, dataP->payloadMaxBufferSize))
    {
        return false;
    }

    return true;
}

/**
 * @brief Connect to AWS (using the AT%AWSIOTCMD command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTConnect()
{
    if (!AdrasteaI_SendRequest("AT%AWSIOTCMD=\"CONNECT\"\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Disconnect from AWS (using the AT%AWSIOTCMD command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTDisconnect()
{
    if (!AdrasteaI_SendRequest("AT%AWSIOTCMD=\"DISCONNECT\"\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Subscribe to Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTSubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCMD=\"SUBSCRIBE\",");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Unsubscribe from Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTUnsubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCMD=\"UNSUBSCRIBE\",");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Publish to Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @param[in] payload Payload to be published. See AdrasteaI_ATMQTT_Payload_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATMQTT_AWSIOTPublish(AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%AWSIOTCMD=\"PUBLISH\",");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topicName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, payload, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_MQTT), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}
