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
#include <Calypso/ATCommands/ATMQTT.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/Calypso.h>

static const char* Calypso_ATMQTT_SecurityMethodsStrings[Calypso_ATMQTT_SecurityMethod_NumberOfValues] = {"SSLV3", "TLSV1", "TLSV1_1", "TLSV1_2", "SSLV3_TLSV1_2"};

static const char* Calypso_ATMQTT_ProtocolStrings[Calypso_ATMQTT_ProtocolVersion_NumberOfValues] = {"v3_1", "v3_1_1"};

const char* Calypso_ATMQTT_QoSStrings[Calypso_ATMQTT_QoS_NumberOfValues] = {"QOS0", "QOS1", "QOS2"};

static const char* Calypso_ATMQTT_SetOptionStrings[Calypso_ATMQTT_SetOption_NumberOfValues] = {"user", "password", "will", "keepAlive", "clean"};

static const char* Calypso_ATMQTT_CreateFlagsStrings[Calypso_ATMQTT_CreateFlags_NumberOfValues] = {"ip4", "ip6", "url", "sec", "skip_domain_verify", "skip_cert_verify", "skip_date_verify"};

static bool Calypso_ATMQTT_AddArgumentsCreate(char* pAtCommand, char* clientID, uint32_t flags, Calypso_ATMQTT_ServerInfo_t serverInfo, Calypso_ATMQTT_SecurityParams_t securityParams, Calypso_ATMQTT_ConnectionParams_t connectionParams);
static bool Calypso_ATMQTT_AddArgumentsPublish(char* pAtCommand, uint8_t index, char* topic, Calypso_ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char* pMessage);
static bool Calypso_ATMQTT_AddArgumentsSubscribe(char* pAtCommand, uint8_t index, uint8_t numOfTopics, Calypso_ATMQTT_SubscribeTopic_t* pTopics);
static bool Calypso_ATMQTT_AddArgumentsUnsubscribe(char* pAtCommand, uint8_t index, char* topic1, char* topic2, char* topic3, char* topic4);
static bool Calypso_ATMQTT_AddArgumentsSet(char* pAtCommand, uint8_t index, Calypso_ATMQTT_SetOption_t option, Calypso_ATMQTT_SetValues_t* pValues);
static bool Calypso_ATMQTT_ParseResponseCreate(char** pAtCommand, uint8_t* pOutIndex);

/**
 * @brief Creates a new MQTT client (using the AT+MQTTCreate command).
 *
 * @param[in] clientID Client ID
 * @param[in] flags Creation flags (see Calypso_ATMQTT_CreateFlags_t)
 * @param[in] serverInfo Server address and port. See Calypso_ATMQTT_ServerInfo_t.
 * @param[in] securityParams Security parameters. See Calypso_ATMQTT_SecurityParams_t.
 * @param[in] connectionParams Connection parameters. See Calypso_ATMQTT_ConnectionParams_t.
 * @param[out] pIndex Index (handle) of the created MQTT client.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATMQTT_Create(char* clientID, uint32_t flags, Calypso_ATMQTT_ServerInfo_t serverInfo, Calypso_ATMQTT_SecurityParams_t securityParams, Calypso_ATMQTT_ConnectionParams_t connectionParams, uint8_t* pIndex)
{
    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttCreate=");

    if (!Calypso_ATMQTT_AddArgumentsCreate(pRequestCommand, clientID, flags, serverInfo, securityParams, connectionParams))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return Calypso_ATMQTT_ParseResponseCreate(&pRespondCommand, pIndex);
}
/**
 * @brief Deletes an MQTT client (using the AT+MQTTDelete command)
 *
 * @param[in] index Index (handle) of MQTT client to delete
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATMQTT_Delete(uint8_t index)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttDelete=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Connects an MQTT client to an MQTT broker (using the AT+MQTTConnect command).
 *
 * @param[in] index Index (handle) of the MQTT client to connect to server. Server data is set with Calypso_ATMQTT_Create().
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATMQTT_Connect(uint8_t index)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttConnect=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Disconnects from an MQTT broker (using the AT+MQTTDisconnect command).
 *
 * @param[in] index Index (handle) of the MQTT client to disconnect from server.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATMQTT_Disconnect(uint8_t index)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttDisconnect=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
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
bool Calypso_ATMQTT_Publish(uint8_t index, char* topic, Calypso_ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char* pMessage)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttPublish=");

    if (!Calypso_ATMQTT_AddArgumentsPublish(pRequestCommand, index, topic, QoS, retain, messageLength, pMessage))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Subscribes to one or more MQTT topics (using the AT+MQTTsubscribe command).
 *
 * @param[in] index Index (handle) of the MQTT client to use.
 * @param[in] numOfTopics Number of topics to subscribe to (max. MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE)
 * @param[in] pTopics Topics to subscribe to. See Calypso_ATMQTT_SubscribeTopic_t.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATMQTT_Subscribe(uint8_t index, uint8_t numOfTopics, Calypso_ATMQTT_SubscribeTopic_t* pTopics)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttSubscribe=");

    if (!Calypso_ATMQTT_AddArgumentsSubscribe(pRequestCommand, index, numOfTopics, pTopics))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
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
bool Calypso_ATMQTT_Unsubscribe(uint8_t index, char* topic1, char* topic2, char* topic3, char* topic4)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttUnsubscribe=");

    if (!Calypso_ATMQTT_AddArgumentsUnsubscribe(pRequestCommand, index, topic1, topic2, topic3, topic4))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
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
bool Calypso_ATMQTT_Set(uint8_t index, Calypso_ATMQTT_SetOption_t option, Calypso_ATMQTT_SetValues_t* pValues)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+mqttSet=");

    if (!Calypso_ATMQTT_AddArgumentsSet(pRequestCommand, index, option, pValues))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Adds arguments to the AT+MQTTCreate command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] clientID Client ID
 * @param[in] flags Creation flags (see Calypso_ATMQTT_CreateFlags_t)
 * @param[in] serverInfo Server address and port. See Calypso_ATMQTT_ServerInfo_t.
 * @param[in] securityParams Security parameters. See Calypso_ATMQTT_SecurityParams_t.
 * @param[in] connectionParams Connection parameters. See Calypso_ATMQTT_ConnectionParams_t.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATMQTT_AddArgumentsCreate(char* pAtCommand, char* clientID, uint32_t flags, Calypso_ATMQTT_ServerInfo_t serverInfo, Calypso_ATMQTT_SecurityParams_t securityParams, Calypso_ATMQTT_ConnectionParams_t connectionParams)
{

    if (!ATCommand_AppendArgumentString(pAtCommand, clientID, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBitmask(pAtCommand, Calypso_ATMQTT_CreateFlagsStrings, Calypso_ATMQTT_CreateFlags_NumberOfValues, flags, ATCOMMAND_ARGUMENT_DELIM, AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, serverInfo.address, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, serverInfo.port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_SecurityMethodsStrings[securityParams.securityMethod], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_ATSocket_AppendCipherMask(pAtCommand, securityParams.cipher))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, "", ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, securityParams.privateKeyFile, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, securityParams.certificateFile, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, securityParams.CAFile, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, securityParams.DHKey, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_ProtocolStrings[connectionParams.protocolVersion], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, connectionParams.blockingSend, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, connectionParams.format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+MQTTpublish command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client.
 * @param[in] topic Topic to be published
 * @param[in] retain Retain the message (1) or do not retain the message (0)
 * @param[in] QoS Quality of Service
 * @param[in] messageLength Length of the message
 * @param[in] pMessage Message to publish
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATMQTT_AddArgumentsPublish(char* pAtCommand, uint8_t index, char* topic, Calypso_ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char* pMessage)
{

    if (!ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, topic, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_QoSStrings[QoS], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, retain, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, messageLength, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBytes(pAtCommand, pMessage, messageLength, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+MQTTsubscribe command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] index Index (handle) of MQTT client to to subscribe.
 * @param[in] numOfTopics Number of topics to be subscribed to
 * @param[in] pTopics Topics to subscribe to. See Calypso_ATMQTT_SubscribeTopic_t
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATMQTT_AddArgumentsSubscribe(char* pAtCommand, uint8_t index, uint8_t numOfTopics, Calypso_ATMQTT_SubscribeTopic_t* pTopics)
{

    if (numOfTopics > MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, numOfTopics, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    for (uint8_t i = 0; i < numOfTopics; i++)
    {

        if (!ATCommand_AppendArgumentString(pAtCommand, pTopics[i].topic, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_QoSStrings[pTopics[i].QoS], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        /* Reserved argument */
        if (!ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    /* Add empty, unused topics*/
    for (uint8_t i = numOfTopics; i <= MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE; i++)
    {
        if (!ATCommand_AppendArgumentString(pAtCommand, ",,", ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    pAtCommand[strlen(pAtCommand)] = ATCOMMAND_STRING_TERMINATE;

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
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
static bool Calypso_ATMQTT_AddArgumentsUnsubscribe(char* pAtCommand, uint8_t index, char* topic1, char* topic2, char* topic3, char* topic4)
{

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

    if (!ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, numOfTopics, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, (NULL != topic1 ? topic1 : ""), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, (NULL != topic2 ? topic2 : ""), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, (NULL != topic3 ? topic3 : ""), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, (NULL != topic4 ? topic4 : ""), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
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
static bool Calypso_ATMQTT_AddArgumentsSet(char* pAtCommand, uint8_t index, Calypso_ATMQTT_SetOption_t option, Calypso_ATMQTT_SetValues_t* pValues)
{

    if (!ATCommand_AppendArgumentInt(pAtCommand, index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if ((option >= Calypso_ATMQTT_SetOption_NumberOfValues))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_SetOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (option)
    {
        case Calypso_ATMQTT_SetOption_User:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, pValues->username, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }

        case Calypso_ATMQTT_SetOption_Password:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, pValues->password, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }

        case Calypso_ATMQTT_SetOption_Will:
        {
            Calypso_ATMQTT_SetWillParams_t* pWillValues = &pValues->will;

            if (!ATCommand_AppendArgumentString(pAtCommand, pWillValues->topic, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;

                if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATMQTT_QoSStrings[pWillValues->QoS], ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }

                if (!ATCommand_AppendArgumentInt(pAtCommand, pWillValues->retain, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }

                if (!ATCommand_AppendArgumentInt(pAtCommand, pWillValues->messageLength, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }

                if (!ATCommand_AppendArgumentString(pAtCommand, pWillValues->message, ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }

                break;
            }

            case Calypso_ATMQTT_SetOption_KeepAlive:
            {
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->keepAliveSeconds, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;
            }

            case Calypso_ATMQTT_SetOption_Clean:
            {
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->clean, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;
            }

            default:
            {
                return false;
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
static bool Calypso_ATMQTT_ParseResponseCreate(char** pAtCommand, uint8_t* pOutIndex)
{
    const char* cmd = "+mqttcreate:";
    const size_t cmdLength = strlen(cmd);

    /* check if response is for get */
    if (0 != strncmp(*pAtCommand, cmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;
    return ATCommand_GetNextArgumentInt(pAtCommand, pOutIndex, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}
