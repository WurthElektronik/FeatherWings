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

#include <StephanoI/ATCommands/ATMQTT.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

/**
 * @brief MQTT User config
 *
 * @param[in] link_ID     Link ID
 * @param[in] scheme      Connection scheme
 * @param[in] client_ID   Client ID
 * @param[in] username    User name
 * @param[in] password    Password
 * @param[in] cert_key_ID ID of the certificate
 * @param[in] CA_ID       ID of the CA
 * @param[in] path 	      Path to the resource
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Userconfig(uint8_t link_ID, StephanoI_ATMQTT_Scheme_t scheme, char* client_ID, char* username, char* password, uint8_t cert_key_ID, uint8_t CA_ID, char* path)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTUSERCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)scheme, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, client_ID, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, cert_key_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, CA_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, path, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Set client ID
 *
 * @param[in] link_ID     Link ID
 * @param[in] client_ID   Client ID
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_SetClientID(uint8_t link_ID, char* client_ID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTCLIENTID=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, client_ID, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Set user name
 *
 * @param[in] link_ID     Link ID
 * @param[in] username    User name
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_SetUsername(uint8_t link_ID, char* username)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTUSERNAME=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Set pass word
 *
 * @param[in] link_ID     Link ID
 * @param[in] password    Pass word
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_SetPassword(uint8_t link_ID, char* password)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTPASSWORD=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT connection config
 *
 * @param[in] link_ID      Link ID
 * @param[in] keep_alive   Keep alive time
 * @param[in] disable_clean_session   Clean session
 * @param[in] lwt_topics    LWT topic
 * @param[in] lwt_msg    	LWT message
 * @param[in] qos           QOS
 * @param[in] lwt_retain   LWT retain
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_ConnectionConfig(uint8_t link_ID, uint16_t keep_alive, bool disable_clean_session, char* lwt_topics, char* lwt_msg, StephanoI_ATMQTT_QOS_t qos, bool lwt_retain)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTUSERCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, keep_alive, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)(disable_clean_session ? 1 : 0), (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, lwt_topics, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, lwt_msg, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, qos, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)(lwt_retain ? 1 : 0), (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Connection setup
 *
 * @param[in] link_ID   Link ID
 * @param[in] host      URL of the host
 * @param[in] port      Port
 * @param[in] reconnect Reconnect
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Connect(uint8_t link_ID, char* host, uint16_t port, bool reconnect)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTCONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, host, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, reconnect ? (uint8_t)1 : (uint8_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTConnect), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Get connection Info
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_GetConnectionInfo()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTCONN?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Publish
 *
 * @param[in] link_ID   Link ID
 * @param[in] topic     Topic
 * @param[in] data      Data
 * @param[in] qos       Quality of service
 * @param[in] retain    Retain
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Publish(uint8_t link_ID, char* topic, char* data, StephanoI_ATMQTT_QOS_t qos, uint8_t retain)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTPUB=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topic, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)qos, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, retain, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Subscribe
 *
 * @param[in] link_ID   Link ID
 * @param[in] topic     Topic
 * @param[in] qos       Quality of service
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Subscribe(uint8_t link_ID, char* topic, StephanoI_ATMQTT_QOS_t qos)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTSUB=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topic, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)qos, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Get subscriptions
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_GetSubscriptions()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTSUB?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Unsubscribe
 *
 * @param[in] link_ID   Link ID
 * @param[in] topic     Topic
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Unsubscribe(uint8_t link_ID, char* topic)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTUNSUB=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, topic, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief MQTT Clean
 *
 * @param[in] link_ID   Link ID
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATMQTT_Clean(uint8_t link_ID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+MQTTCLEAN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_MQTTGeneral), StephanoI_CNFStatus_Success);
}

/**
 * @brief Parses the values of the MQTT connection info event arguments
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATMQTT_ParseConnectionInfo(char* EventArgumentsP, StephanoI_ATMQTT_ConnectionInfo_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->state), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->scheme), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->host, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->host)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->port), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->path, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->path)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->reconnect), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the MQTT get subscriptions event arguments
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATMQTT_ParseGetSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_GetSubscriptions_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->state), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->topic, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->topic)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->qos), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the MQTT receive subscriptions event arguments
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATMQTT_ParseReceiveSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_ReceiveSubscriptions_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->topic, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->topic)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    t->data = malloc(t->length);
    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, t->length))
    {
        return false;
    }

    return true;
}
