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
 * @brief AT commands for MQTT functionality.
 */

#include "ATMQTT.h"

#include "ATSocket.h"

#include "../Calypso.h"

static const char *ATMQTT_SecurityMethodsStrings[ATMQTT_SecurityMethod_NumberOfValues] =
{
    "SSLV3",
    "TLSV1",
    "TLSV1_1",
    "TLSV1_2",
    "SSLV3_TLSV1_2"
};

static const char *ATMQTT_ProtocolStrings[ATMQTT_ProtocolVersion_NumberOfValues] =
{
    "v3_1",
    "v3_1_1"
};

const char *ATMQTT_QoSStrings[ATMQTT_QoS_NumberOfValues] =
{
    "QOS0",
    "QOS1",
    "QOS2"
};

static const char *ATMQTT_SetOptionStrings[ATMQTT_SetOption_NumberOfValues] =
{
    "user",
    "password",
    "will",
    "keepAlive",
    "clean"
};

static const char *ATMQTT_CreateFlagsStrings[ATMQTT_CreateFlags_NumberOfValues] =
{
    "ip4",
    "ip6",
    "url",
    "sec",
    "skip_domain_verify",
    "skip_cert_verify",
    "skip_date_verify"
};

static bool ATMQTT_AddArgumentsCreate(char *pAtCommand,
                                      char *clientID,
                                      uint32_t flags,
                                      ATMQTT_ServerInfo_t serverInfo,
                                      ATMQTT_SecurityParams_t securityParams,
                                      ATMQTT_ConnectionParams_t connectionParams);
static bool ATMQTT_AddArgumentsPublish(char *pAtCommand,
                                       uint8_t index,
                                       char *topic,
                                       ATMQTT_QoS_t QoS,
                                       uint8_t retain,
                                       uint16_t messageLength,
                                       char *pMessage);
static bool ATMQTT_AddArgumentsSubscribe(char *pAtCommand,
                                         uint8_t index,
                                         uint8_t numOfTopics,
                                         ATMQTT_SubscribeTopic_t *pTopics);
static bool ATMQTT_AddArgumentsUnsubscribe(char *pAtCommand,
                                           uint8_t index,
                                           char *topic1,
                                           char *topic2,
                                           char *topic3,
                                           char *topic4);
static bool ATMQTT_AddArgumentsSet(char *pAtCommand,
                                   uint8_t index,
                                   ATMQTT_SetOption_t option,
                                   ATMQTT_SetValues_t *pValues);
static bool ATMQTT_ParseResponseCreate(char **pAtCommand, uint8_t *pOutIndex);


/**
 * @brief Creates a new MQTT client (using the AT+MQTTCreate command).
 *
 * @param[in] clientID Client ID
 * @param[in] flags Creation flags (see ATMQTT_CreateFlags_t)
 * @param[in] serverInfo Server address and port. See ATMQTT_ServerInfo_t.
 * @param[in] securityParams Security parameters. See ATMQTT_SecurityParams_t.
 * @param[in] connectionParams Connection parameters. See ATMQTT_ConnectionParams_t.
 * @param[out] pIndex Index (handle) of the created MQTT client.
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Create(char *clientID,
                   uint32_t flags,
                   ATMQTT_ServerInfo_t serverInfo,
                   ATMQTT_SecurityParams_t securityParams,
                   ATMQTT_ConnectionParams_t connectionParams,
                   uint8_t *pIndex)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttCreate=");

    ret = ATMQTT_AddArgumentsCreate(pRequestCommand, clientID, flags, serverInfo, securityParams, connectionParams);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand);
    }

    if (ret)
    {
        ret = ATMQTT_ParseResponseCreate(&pRespondCommand, pIndex);
    }

    return ret;
}
/**
 * @brief Deletes an MQTT client (using the AT+MQTTDelete command)
 *
 * @param[in] index Index (handle) of MQTT client to delete
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Delete(uint8_t index)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttDelete=");

    ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Connects an MQTT client to an MQTT broker (using the AT+MQTTConnect command).
 *
 * @param[in] index Index (handle) of the MQTT client to connect to server. Server data is set with ATMQTT_Create().
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Connect(uint8_t index)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttConnect=");

    ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Disconnects from an MQTT broker (using the AT+MQTTDisconnect command).
 *
 * @param[in] index Index (handle) of the MQTT client to disconnect from server.
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Disconnect(uint8_t index)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttDisconnect=");

    ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Publishes an MQTT topic (using the AT+MQTTpublish command).
 *
 * @param[in] index Index (handle) of the MQTT client to use.
 * @param[in] topic Topic to be published
 * @param[in] retain Retain the message (1) or do not retain the message (0)
 * @param[in] messageLength Length of the message
 * @param[in] pMessage Message to publish
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Publish(uint8_t index, char *topic, ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char *pMessage)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttPublish=");

    ret = ATMQTT_AddArgumentsPublish(pRequestCommand, index, topic, QoS, retain, messageLength, pMessage);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Subscribes to one or more MQTT topics (using the AT+MQTTsubscribe command).
 *
 * @param[in] index Index (handle) of the MQTT client to use.
 * @param[in] numOfTopics Number of topics to subscribe to (max. MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE)
 * @param[in] pTopics Topics to subscribe to. See ATMQTT_SubscribeTopic_t.
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Subscribe(uint8_t index, uint8_t numOfTopics, ATMQTT_SubscribeTopic_t *pTopics)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttSubscribe=");

    ret = ATMQTT_AddArgumentsSubscribe(pRequestCommand, index, numOfTopics, pTopics);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Unsubscribe from one or more MQTT topics (using the AT+MQTTunsubscribe command).
 *
 * @param[in] index Index (handle) of MQTT client to use.
 * @param[in] topic1 Topic 1 to unsubscribe.
 * @param[in] topic2 Topic 2 to unsubscribe. Set to NULL / empty string to ignore this topic and topics 3,4.
 * @param[in] topic3 Topic 3 to unsubscribe. Set to NULL / empty string to ignore this topic and topic 4.
 * @param[in] topic4 Topic 4 to unsubscribe. Set to NULL / empty string to ignore.
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Unsubscribe(uint8_t index, char *topic1, char *topic2, char *topic3, char *topic4)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttUnsubscribe=");

    ret = ATMQTT_AddArgumentsUnsubscribe(pRequestCommand, index, topic1, topic2, topic3, topic4);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Sets parameters of an MQTT client (using the AT+MQTTset command).
 *
 * CAUTION! Only use this command if firmware version of the Calypso module is 1.1.0 or later.
 *
 * @param[in] index Index (handle) of MQTT client to set options for
 * @param[in] option Option to set
 * @param[in] pValues Values to set
 *
 * @return true if successful, false otherwise
 */
bool ATMQTT_Set(uint8_t index, ATMQTT_SetOption_t option, ATMQTT_SetValues_t *pValues)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttSet=");

    ret = ATMQTT_AddArgumentsSet(pRequestCommand, index, option, pValues);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}


/**
 * @brief Adds arguments to the AT+MQTTCreate command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] clientID Client ID
 * @param[in] flags Creation flags (see ATMQTT_CreateFlags_t)
 * @param[in] serverInfo Server address and port. See ATMQTT_ServerInfo_t.
 * @param[in] securityParams Security parameters. See ATMQTT_SecurityParams_t.
 * @param[in] connectionParams Connection parameters. See ATMQTT_ConnectionParams_t.
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_AddArgumentsCreate(char *pAtCommand,
                                      char *clientID,
                                      uint32_t flags,
                                      ATMQTT_ServerInfo_t serverInfo,
                                      ATMQTT_SecurityParams_t securityParams,
                                      ATMQTT_ConnectionParams_t connectionParams)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentString(pAtCommand, clientID, ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATCommand_AppendArgumentBitmask(pAtCommand,
                                            ATMQTT_CreateFlagsStrings,
                                            ATMQTT_CreateFlags_NumberOfValues,
                                            flags,
                                            ATCOMMAND_ARGUMENT_DELIM,
                                            AT_MAX_COMMAND_BUFFER_SIZE);
    }


    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, serverInfo.address, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, serverInfo.port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATMQTT_SecurityMethodsStrings[securityParams.securityMethod], ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATSocket_AppendCipherMask(pAtCommand, securityParams.cipher);
        if (ret)
        {
            ret = ATCommand_AppendArgumentString(pAtCommand, "", ATCOMMAND_ARGUMENT_DELIM);
        }
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, securityParams.privateKeyFile, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, securityParams.certificateFile, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, securityParams.CAFile, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, securityParams.DHKey, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATMQTT_ProtocolStrings[connectionParams.protocolVersion], ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, connectionParams.blockingSend, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, connectionParams.format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+MQTTpublish command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client.
 * @param[in] topic Topic to be published
 * @param[in] retain Retain the message (1) or do not retain the message (0)
 * @param[in] messageLength Length of the message
 * @param[in] pMessage Message to publish
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_AddArgumentsPublish(char *pAtCommand,
                                       uint8_t index,
                                       char *topic,
                                       ATMQTT_QoS_t QoS,
                                       uint8_t retain,
                                       uint16_t messageLength,
                                       char *pMessage)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, topic, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATMQTT_QoSStrings[QoS], ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, retain, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, messageLength, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentBytes(pAtCommand, pMessage, messageLength, ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+MQTTsubscribe command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client to to subscribe.
 * @param[in] numOfTopics Number of topics to be subscribed to
 * @param[in] pTopics Topics to subscribe to. See ATMQTT_SubscribeTopic_t
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_AddArgumentsSubscribe(char *pAtCommand, uint8_t index, uint8_t numOfTopics, ATMQTT_SubscribeTopic_t *pTopics)
{
    bool ret = false;

    if (numOfTopics > MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE)
    {
        return false;
    }

    ret = ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, numOfTopics, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
    }

    for (int i = 0; i < numOfTopics; i++)
    {
        if (ret)
        {
            ATCommand_AppendArgumentString(pAtCommand, pTopics[i].topic, ATCOMMAND_ARGUMENT_DELIM);
        }

        if (ret)
        {
            ATCommand_AppendArgumentString(pAtCommand, ATMQTT_QoSStrings[pTopics[i].QoS], ATCOMMAND_ARGUMENT_DELIM);
        }

        if (ret)
        {
            /* Reserved argument */
            ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM);
        }
    }

    /* Add empty, unused topics*/
    for (uint8_t i = numOfTopics; i <= MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE;  i++)
    {
        if (ret)
        {
           ATCommand_AppendArgumentString(pAtCommand, ",,", ATCOMMAND_ARGUMENT_DELIM);
        }
    }

    pAtCommand[strlen(pAtCommand)] = ATCOMMAND_STRING_TERMINATE;

    if (ret)
    {
        ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+MQTTunsubscribe command string
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client to unsubscribe.
 * @param[in] topic1 Topic 1 to unsubscribe.
 * @param[in] topic2 Topic 2 to unsubscribe. Set to NULL / empty string to ignore this topic and topics 3,4.
 * @param[in] topic3 Topic 3 to unsubscribe. Set to NULL / empty string to ignore this topic and topic 4.
 * @param[in] topic4 Topic 4 to unsubscribe. Set to NULL / empty string to ignore.
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_AddArgumentsUnsubscribe(char *pAtCommand,
                                           uint8_t index,
                                           char *topic1,
                                           char *topic2,
                                           char *topic3,
                                           char *topic4)
{
    bool ret = false;

    uint32_t numOfTopics = 0;
    if (NULL != topic1 && strlen(topic1) > 0)
    {
        numOfTopics++;
    }
    if (numOfTopics > 0 && NULL != topic2 && strlen(topic2) > 0)
    {
        numOfTopics++;
    }
    if (numOfTopics > 0 && NULL != topic3 && strlen(topic3) > 0)
    {
        numOfTopics++;
    }
    if (numOfTopics > 0 && NULL != topic4 && strlen(topic4) > 0)
    {
        numOfTopics++;
    }

    ret = ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATCommand_AppendArgumentInt(pAtCommand, numOfTopics, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, (NULL != topic1 ? topic1 : "") , ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, (NULL != topic2 ? topic2 : ""), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, (NULL != topic3 ? topic3 : ""), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, (NULL != topic4 ? topic4 : ""), ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }
    return ret;
}

/**
 * @brief Adds arguments to the AT+MQTTset command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client to set options for
 * @param[in] option Option to set
 * @param[in] pValues Values to set
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_AddArgumentsSet(char *pAtCommand, uint8_t index, ATMQTT_SetOption_t option, ATMQTT_SetValues_t *pValues)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

    if (ret && (option < ATMQTT_SetOption_NumberOfValues))
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATMQTT_SetOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM);
    }
    else
    {
        ret = false;
    }

    if (ret)
    {
        switch (option)
        {
        case ATMQTT_SetOption_User:
        {
            ret = ATCommand_AppendArgumentString(pAtCommand, pValues->username, ATCOMMAND_STRING_TERMINATE);
            break;
        }

        case ATMQTT_SetOption_Password:
        {
            ret = ATCommand_AppendArgumentString(pAtCommand, pValues->password, ATCOMMAND_STRING_TERMINATE);
            break;
        }

        case ATMQTT_SetOption_Will:
        {
            ATMQTT_SetWillParams_t *pWillValues = &pValues->will;

            ret = ATCommand_AppendArgumentString(pAtCommand, pWillValues->topic, ATCOMMAND_ARGUMENT_DELIM);

            if (ret)
            {
                ret = ATCommand_AppendArgumentString(pAtCommand, ATMQTT_QoSStrings[pWillValues->QoS], ATCOMMAND_ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pWillValues->retain, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pWillValues->messageLength, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED),ATCOMMAND_ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = ATCommand_AppendArgumentString(pAtCommand, pWillValues->message, ATCOMMAND_STRING_TERMINATE);
            }

            break;
        }

        case ATMQTT_SetOption_KeepAlive:
        {
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->keepAliveSeconds, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;
        }

        case ATMQTT_SetOption_Clean:
        {
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->clean, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;
        }

        default:
        {
            ret = false;
        }
        }
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parse the response to the AT+MQTTCreate command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+MQTTCreate command.
 * @param[out] pOutIndex Index (handle) of the created MQTT client.
 *
 * @return true if successful, false otherwise
*/
static bool ATMQTT_ParseResponseCreate(char **pAtCommand, uint8_t *pOutIndex)
{
    bool ret = false;
    const char *cmd = "+mqttcreate:";
    const size_t cmdLength = strlen(cmd);

    /* check if response is for get*/
    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = ATCommand_GetNextArgumentInt(pAtCommand, pOutIndex, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}
