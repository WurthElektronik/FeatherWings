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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for basic device functionality.
 */
#include <CordeliaI/ATCommands/ATDevice.h>
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* CordeliaI_ATDevice_ATGetIdStrings[CordeliaI_ATDevice_GetId_NumberOfValues] = {"status", "general", "IOT", "UART", "csr", "mqtt", "subtopic0", "subtopic1", "subtopic2", "subtopic3", "pubtopic0", "pubtopic1", "pubtopic2", "pubtopic3", "quarklink", "ota"};

static const char* CordeliaI_ATDevice_ATGetGeneralStrings[CordeliaI_ATDevice_GetGeneral_NumberOfValues] = {"version", "time", "persistent"};

static const char* CordeliaI_ATDevice_ATGetIotStrings[CordeliaI_ATDevice_GetIot_NumberOfValues] = {"UDID", "deviceID"};

static const char* CordeliaI_ATDevice_ATGetCSRStrings[CordeliaI_ATDevice_GetCSR_NumberOfValues] = {"country", "state", "locality", "surname", "organization", "unit", "email"};

static const char* CordeliaI_ATDevice_ATGetPubTopicStrings[CordeliaI_ATDevice_GetPubTopic_NumberOfValues] = {"name", "qos", "retain"};

static const char* CordeliaI_ATDevice_ATGetSubTopicStrings[CordeliaI_ATDevice_GetSubTopic_NumberOfValues] = {"name", "qos"};

static const char* CordeliaI_ATDevice_ATGetMQTTStrings[CordeliaI_ATDevice_GetMQTT_NumberOfValues] = {"iotHubEndpoint", "iotHubPort", "clientCertPath", "rootCAPath", "clientPrivateKey", "clientId", "flags", "TLS1_3", "base64", "willTopic", "willMessage", "willQos", "willRetain", "userName", "password", "keepAliveTimeOut", "cleanConnect"};

static const char* CordeliaI_ATDevice_ATGetQuarklinkStrings[CordeliaI_ATDevice_GetQuarklink_NumberOfValues] = {"hostname", "port", "rootcapath"};

static const char* CordeliaI_ATDevice_ATGetOTAStrings[CordeliaI_ATDevice_GetOTA_NumberOfValues] = {"url", "rootCAPath", "updateVersion"};

static const char* CordeliaI_ATDevice_ATGetUartStrings[CordeliaI_ATDevice_GetUart_NumberOfValues] = {"baudrate", "parity", "flowcontrol", "transparent_trigger", "transparent_timeout", "transparent_etx"};

static const char* CordeliaI_ATDevice_ATGetTransparentModeUartTriggerStrings[CordeliaI_ATDevice_TransparentModeUartTrigger_NumberOfValues] = {"timer", "1etx", "2etx", "transmit_etx"};

static const char* CordeliaI_ATDevice_StatusFlagsStrings[CordeliaI_ATDevice_StatusFlags_NumberOfValues] = {
    "general_error", "wlanasynconnectedresponse", "wlanasyncdisconnectedresponse", "sta_connected", "sta_disconnected", "p2p_dev_found", "connection_failed", "p2p_neg_req_received", "rx_filters", "wlan_sta_connected", "tx_failed", "ipacquired", "ipacquired_v6", "ip_leased", "ip_released", "ipv4_lost", "dhcp_acquire_timeout", "ip_collision", "ipv6_lost"};

static bool CordeliaI_ATDevice_IsInputValidATget(CordeliaI_ATDevice_GetId_t id, uint8_t option);
static bool CordeliaI_ATDevice_IsInputValidATset(CordeliaI_ATDevice_GetId_t id, uint8_t option);
static bool CordeliaI_ATDevice_AddArgumentsATget(char* pAtCommand, uint8_t id, uint8_t option);
static bool CordeliaI_ATDevice_AddArgumentsATset(char* pAtCommand, uint8_t id, uint8_t option, void* pValue);
static bool CordeliaI_ATDevice_ParseResponseATget(uint8_t id, uint8_t option, char* pAtCommand, void* pValue);

/**
 * @brief Tests the connection to the wireless module (using the AT+test command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Test()
{
    if (!CordeliaI_SendRequest("AT+test\r\n"))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * Send AT command - general function
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_SendATCommand(char* atCommand)
{
    if (!CordeliaI_SendRequest(atCommand))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets device ID
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_Get_DeviceID()
{
    if (!CordeliaI_SendRequest("AT+get=iot,deviceid\r\n"))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Starts the network processor (using the AT+start command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Start()
{
    if (!CordeliaI_SendRequest("AT+start\r\n"))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Stops the network processor (using the AT+stop command).
 *
 * @param[in] timeoutMs Timeout in milliseconds.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Stop(uint32_t timeoutMs)
{

    if (timeoutMs > ATDEVICE_STOP_MAX_TIMEOUT)
    {
        return false;
    }

    char cmdToSend[32] = "AT+stop=";

    if (!ATCommand_IntToString(&cmdToSend[strlen(cmdToSend)], timeoutMs, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(cmdToSend, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!CordeliaI_SendRequest(cmdToSend))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Restarts the network processor by sending an AT+stop followed by an AT+start command.
 *
 * @param[in] timeoutMs Timeout for stop command
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Restart(uint32_t timeoutMs)
{
    if (!CordeliaI_ATDevice_Stop(timeoutMs))
    {
        /* Stop command will fail with error -2018 if the network processor is not active at
		 * the time of processing the command. This is not considered to be an error. */
        if (CordeliaI_GetLastError(NULL) != -2018)
        {
            return false;
        }
    }
    return CordeliaI_ATDevice_Start();
}

/**
 * @brief Reboots the device (using the AT+reboot command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Reboot()
{
    if (!CordeliaI_SendRequest("AT+reboot\r\n"))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Performs a factory reset of the device (using the AT+factoryReset command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_FactoryReset()
{
    if (!CordeliaI_SendRequest("AT+factoryreset\r\n"))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_FactoryReset), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Puts the wireless module into the lowest possible power mode for the supplied duration (using the AT+hibernate command).
 *
 * @param[in] timeoutSeconds Timeout until the module wakes up again.
 *                           Must be in range ATDEVICE_SLEEP_MIN_TIMEOUT <= timeoutInSeconds <= ATDEVICE_SLEEP_MAX_TIMEOUT.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Hibernate(uint32_t timeoutSeconds)
{

    if ((timeoutSeconds < ATDEVICE_SLEEP_MIN_TIMEOUT) || (timeoutSeconds > ATDEVICE_SLEEP_MAX_TIMEOUT))
    {
        return false;
    }

    char cmdToSend[32] = "AT+hibernate=";

    if (!ATCommand_IntToString(&cmdToSend[strlen(cmdToSend)], timeoutSeconds, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(cmdToSend, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!CordeliaI_SendRequest(cmdToSend))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Reads device parameters (using the AT+get command).
 *
 * @param[in] id ID of the parameter to get.
 * @param[in] option Option to get. Valid values depend on id.
 * @param[out] pValue Values returned.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Get(CordeliaI_ATDevice_GetId_t id, uint8_t option, void* pValue)
{
    if (!CordeliaI_ATDevice_IsInputValidATget(id, option))
    {
        return false;
    }
    char* pRequestCommand = AT_commandBuffer;
    char* pResponseCommand = AT_commandBuffer;
    uint32_t timeout;
    strcpy(pRequestCommand, "AT+get=");

    if (!CordeliaI_ATDevice_AddArgumentsATget(&pRequestCommand[strlen(pRequestCommand)], id, option))
    {
        return false;
    }

    if (!CordeliaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    switch (id)
    {
        case CordeliaI_ATDevice_GetId_OTA:
        {
            if (option == CordeliaI_ATDevice_GetOTA_FirmwareVersion)
            {
                timeout = CordeliaI_GetTimeout(CordeliaI_Timeout_OTAVersion);
            }
            else
            {
                timeout = CordeliaI_GetTimeout(CordeliaI_Timeout_General);
            }
            break;
        }
        default:
            timeout = CordeliaI_GetTimeout(CordeliaI_Timeout_General);
            break;
    }

    if (!CordeliaI_WaitForConfirm(timeout, CordeliaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!CordeliaI_ATDevice_ParseResponseATget(id, option, pResponseCommand, pValue))
    {
        return false;
    }

    return true;
}

/**
 * @brief Sets device parameters (using the AT+set command).
 *
 * @param[in] id ID of parameter to be set. Valid IDs are CordeliaI_ATDevice_GetId_General and CordeliaI_ATDevice_GetId_UART.
 * @param[in] option Option to set. Valid values depend on ID.
 * @param[out] pValues Values for the specific ID/option
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_t id, uint8_t option, void* pValue)
{

    if (!CordeliaI_ATDevice_IsInputValidATset(id, option))
    {
        return false;
    }
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+set=");

    if (!CordeliaI_ATDevice_AddArgumentsATset(&pRequestCommand[strlen(pRequestCommand)], id, option, pValue))
    {
        return false;
    }
    if (!CordeliaI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief MQTT publish argument (using the AT+wlanConnect command).
 *
 * @param[in] publishArgs Publish parameters
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_MQTT_Publish(CordeliaI_MQTT_PublishArguments_t publishArgs)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+iotpublish=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint8_t)publishArgs.topicNo, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, publishArgs.Message, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!CordeliaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Enrolls the IoT device (using the AT+iotenrol command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_IoT_Enrol()
{
    if (!CordeliaI_SendRequest("AT+iotenrol\r\n"))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Connects the IoT device (using the AT+iotconnect command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_IoT_Connect()
{
    if (!CordeliaI_SendRequest("AT+iotconnect\r\n"))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Disconnects the IoT device (using the AT+iotdisconnect command).
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_IoT_Disconnect()
{
    if (!CordeliaI_SendRequest("AT+iotdisconnect\r\n"))
    {
        return false;
    }

    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Starts provisioning mode.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_StartProvisioning()
{
    if (!CordeliaI_SendRequest("AT+provisioningStart\r\n"))
    {
        return false;
    }
    return CordeliaI_WaitForConfirm(CordeliaI_GetTimeout(CordeliaI_Timeout_General), CordeliaI_CNFStatus_Success, NULL);
}

/**
 * @brief Prints status flags to string (for debugging purposes).
 *
 * @param[in] flags Status flags
 * @param[out] pOutStr Output string buffer
 * @param[in] maxLength Size of output string buffer
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_ATDevice_PrintStatusFlags(uint32_t flags, char* pOutStr, size_t maxLength)
{
    pOutStr[0] = '\0';
    return ATCommand_AppendArgumentBitmask(pOutStr, CordeliaI_ATDevice_StatusFlagsStrings, CordeliaI_ATDevice_StatusFlags_NumberOfValues, flags, ATCOMMAND_STRING_TERMINATE, maxLength);
}

/**
 * @brief Checks if parameters are valid for the AT+get command.
 *
 * @param[in] id The AT+get ID
 * @param[in] option The AT+get option
 *
 * @return true if arguments are valid, false otherwise
 */
bool CordeliaI_ATDevice_IsInputValidATget(CordeliaI_ATDevice_GetId_t id, uint8_t option)
{

    if (id >= CordeliaI_ATDevice_GetId_NumberOfValues)
    {
        return false;
    }

    switch (id)
    {
        case CordeliaI_ATDevice_GetId_MQTT:
        {
            return (option < CordeliaI_ATDevice_GetMQTT_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_General:
        {
            return (option < CordeliaI_ATDevice_GetGeneral_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_IOT:
        {
            return (option < CordeliaI_ATDevice_GetIot_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_CSR:
        {
            return (option < CordeliaI_ATDevice_GetCSR_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_PubTopic0:
        case CordeliaI_ATDevice_GetId_PubTopic1:
        case CordeliaI_ATDevice_GetId_PubTopic2:
        case CordeliaI_ATDevice_GetId_PubTopic3:
        {
            return (option < CordeliaI_ATDevice_GetPubTopic_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_SubTopic0:
        case CordeliaI_ATDevice_GetId_SubTopic1:
        case CordeliaI_ATDevice_GetId_SubTopic2:
        case CordeliaI_ATDevice_GetId_SubTopic3:
        {
            return (option < CordeliaI_ATDevice_GetSubTopic_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_UART:
        {
            return (option < CordeliaI_ATDevice_GetUart_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_Quarklink:
        {
            return (option < CordeliaI_ATDevice_GetQuarklink_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_OTA:
        {
            return (option < CordeliaI_ATDevice_GetOTA_NumberOfValues);
            break;
        }
        default:
        {
            return false;
            break;
        }
    }
}

/**
 * @brief Adds the arguments to the AT+get command string.
 *
 * @param[out] pAtCommand The AT request command string to add the arguments to
 * @param[in] id The ID of the argument to be added
 * @param[in] option The option to add
 * @return true if arguments were added successfully, false otherwise
 */
bool CordeliaI_ATDevice_AddArgumentsATget(char* pAtCommand, uint8_t id, uint8_t option)
{
    if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetIdStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (id)
    {
        case CordeliaI_ATDevice_GetId_General:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetGeneralStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_IOT:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetIotStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_MQTT:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetMQTTStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_CSR:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetCSRStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_Quarklink:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetQuarklinkStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_OTA:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetOTAStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_PubTopic0:
        case CordeliaI_ATDevice_GetId_PubTopic1:
        case CordeliaI_ATDevice_GetId_PubTopic2:
        case CordeliaI_ATDevice_GetId_PubTopic3:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetPubTopicStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_SubTopic0:
        case CordeliaI_ATDevice_GetId_SubTopic1:
        case CordeliaI_ATDevice_GetId_SubTopic2:
        case CordeliaI_ATDevice_GetId_SubTopic3:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetSubTopicStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_UART:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetUartStrings[option], ATCOMMAND_STRING_TERMINATE))
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

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response to the AT+get command.
 *
 * @param[in] id The ID of the parameter to get
 * @param[in] option The option to get
 * @param[in] pAtCommand The response string sent by the module
 * @param[out] pValues Parsed values
 *
 * @return true if response was parsed successfully, false otherwise
 */
bool CordeliaI_ATDevice_ParseResponseATget(uint8_t id, uint8_t option, char* pAtCommand, void* pValue)
{
    const char* cmd = "+get:";
    const size_t cmdLength = strlen(cmd);

    memset(pValue, 0, sizeof(*pValue));

    /* check if response is for get*/
    if (0 != strncmp(pAtCommand, cmd, cmdLength))
    {
        return false;
    }
    pAtCommand += cmdLength;
    switch (id)
    {
        case CordeliaI_ATDevice_GetId_CSR:
        {
            switch (option)
            {

                case CordeliaI_ATDevice_GetCSR_Country:
                {
                    char tempString[3];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_State:
                {
                    char tempString[129];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_Locality:
                {
                    char tempString[129];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_Surname:
                {
                    char tempString[65];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_Organization:
                {
                    char tempString[65];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_Unit:
                {
                    char tempString[65];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                case CordeliaI_ATDevice_GetCSR_Email:
                {
                    char tempString[256];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                default:
                {
                    return false;
                }
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_PubTopic0:
        case CordeliaI_ATDevice_GetId_PubTopic1:
        case CordeliaI_ATDevice_GetId_PubTopic2:
        case CordeliaI_ATDevice_GetId_PubTopic3:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetPubTopic_Name:
                {
                    char tempString[513];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 512);
                    break;
                }
                case CordeliaI_ATDevice_GetPubTopic_Qos:
                {
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, (uint8_t*)pValue, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetPubTopic_Retain:
                {
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, (bool*)pValue, ATCOMMAND_STRING_TERMINATE))
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
        break;
        case CordeliaI_ATDevice_GetId_SubTopic0:
        case CordeliaI_ATDevice_GetId_SubTopic1:
        case CordeliaI_ATDevice_GetId_SubTopic2:
        case CordeliaI_ATDevice_GetId_SubTopic3:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetSubTopic_Name:
                {
                    char tempString[513];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 512);
                    break;
                }
                case CordeliaI_ATDevice_GetSubTopic_Qos:
                {
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, (uint8_t*)pValue, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
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
        break;
        case CordeliaI_ATDevice_GetId_Quarklink:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetQuarklink_Hostname:
                {
                    char tempString[513];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 512);
                    break;
                }
                case CordeliaI_ATDevice_GetQuarklink_Port:
                {
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, (uint32_t*)pValue, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetQuarklink_RootCAPath:
                {
                    char tempString[181];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 180);
                    break;
                }
                default:
                {
                    return false;
                }
            }
        }
        break;
        case CordeliaI_ATDevice_GetId_OTA:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetOTA_URL:
                {
                    char tempString[513];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 512);
                    break;
                }
                case CordeliaI_ATDevice_GetOTA_RootCAPath:
                {
                    char tempString[181];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 180);
                    break;
                }
                case CordeliaI_ATDevice_GetOTA_FirmwareVersion:
                {
                    char tempString[65];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 64);
                    break;
                }

                default:
                {
                    return false;
                }
            }
        }
        break;
        case CordeliaI_ATDevice_GetId_MQTT:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetMQTT_IotHubEndpoint:
                case CordeliaI_ATDevice_GetMQTT_WillTopic:
                case CordeliaI_ATDevice_GetMQTT_WillMessage:
                case CordeliaI_ATDevice_GetMQTT_UserName:
                case CordeliaI_ATDevice_GetMQTT_Password:
                {
                    char tempString[513];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 512);
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_ClientCertPath:
                case CordeliaI_ATDevice_GetMQTT_RootCAPath:
                case CordeliaI_ATDevice_GetMQTT_ClientPrivateKey:
                {
                    char tempString[181];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 180);
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_Flags:
                {
                    char tempString[129];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 128);
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_ClientId:
                {
                    char tempString[257];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    strncpy((char*)pValue, tempString, 256);
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_IotHubPort:
                case CordeliaI_ATDevice_GetMQTT_WillQos:
                case CordeliaI_ATDevice_GetMQTT_KeepAliveTimeOut:
                {
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, (uint32_t*)pValue, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_TLS1_3:
                case CordeliaI_ATDevice_GetMQTT_Base64:
                case CordeliaI_ATDevice_GetMQTT_WillRetain:
                case CordeliaI_ATDevice_GetMQTT_CleanConnect:
                {
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, (bool*)pValue, ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_IOT:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetIot_UDID:
                {
                    char tempString[6];
                    uint8_t udidArr[16];
                    for (uint8_t idx = 0; idx < 15; idx++)
                    {
                        if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                        {
                            return false;
                        }
                        if (!ATCommand_StringToInt(&(udidArr[idx]), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX))
                        {
                            return false;
                        }
                    }
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(udidArr[15]), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX))
                    {
                        return false;
                    }
                    sprintf((char*)pValue, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", udidArr[0], udidArr[1], udidArr[2], udidArr[3], udidArr[4], udidArr[5], udidArr[6], udidArr[7], udidArr[8], udidArr[9], udidArr[10], udidArr[11], udidArr[12], udidArr[13], udidArr[14], udidArr[15]);
                    break;
                }

                case CordeliaI_ATDevice_GetIot_DeviceID:
                {
                    char tempString[66];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    memset((char*)pValue, '\0', sizeof(tempString));
                    sprintf((char*)pValue, "%s", tempString);
                    break;
                }
                default:
                {
                    return false;
                }
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_UART:
        {
            switch (option)
            {
                case CordeliaI_ATDevice_GetUart_Baudrate:
                {
                    char tempString[12];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }

                    if (!ATCommand_StringToInt((uint32_t*)pValue, tempString, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_Parity:
                {
                    char tempString[6];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt((uint8_t*)pValue, tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_FlowControl:
                {
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, (bool*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_TransparentTrigger:
                {
                    uint32_t bitmask;
                    if (!ATCommand_GetNextArgumentBitmask(&pAtCommand, CordeliaI_ATDevice_ATGetTransparentModeUartTriggerStrings, CordeliaI_ATDevice_TransparentModeUartTrigger_NumberOfValues, 32, &bitmask, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    *(uint8_t*)pValue = (uint8_t)bitmask;
                    break;
                }
                case CordeliaI_ATDevice_GetUart_TransparentTimeout:
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, (uint16_t*)pValue, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                case CordeliaI_ATDevice_GetUart_TransparentETX:
                {
                    uint16_t etx;
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, &etx, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    uint8_t* byteArray = (uint8_t*)pValue;
                    byteArray[0] = (etx >> 8) & 0xFF;
                    byteArray[1] = etx & 0xFF;
                    break;
                }
                default:
                {
                    return false;
                }
            }

            break;
        }
        default:
        {
            return false;
        }
    }

    return true;
}

/**
 * @param Checks if parameters are valid for the AT+set command.
 *
 * @param[in] id The ID of the parameter to be added
 * @param[in] option The option to be added
 *
 * @return true if arguments are valid, false otherwise
 */
static bool CordeliaI_ATDevice_IsInputValidATset(CordeliaI_ATDevice_GetId_t id, uint8_t option)
{
    switch (id)
    {

        case CordeliaI_ATDevice_GetId_General:
        {
            /* only option time and persistent can be written*/
            return ((CordeliaI_ATDevice_GetGeneral_Persistent == option) || (CordeliaI_ATDevice_GetGeneral_Time == option));
            break;
        }
        case CordeliaI_ATDevice_GetId_UART:
        {
            return (option < CordeliaI_ATDevice_GetUart_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_MQTT:
        {
            return (option < CordeliaI_ATDevice_GetMQTT_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_PubTopic0:
        case CordeliaI_ATDevice_GetId_PubTopic1:
        case CordeliaI_ATDevice_GetId_PubTopic2:
        case CordeliaI_ATDevice_GetId_PubTopic3:
        {
            return (option < CordeliaI_ATDevice_GetPubTopic_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_SubTopic0:
        case CordeliaI_ATDevice_GetId_SubTopic1:
        case CordeliaI_ATDevice_GetId_SubTopic2:
        case CordeliaI_ATDevice_GetId_SubTopic3:
        {
            return (option < CordeliaI_ATDevice_GetSubTopic_NumberOfValues);
            break;
        }

        case CordeliaI_ATDevice_GetId_Quarklink:
        {
            return (option < CordeliaI_ATDevice_GetQuarklink_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_OTA:
        {
            return (option < CordeliaI_ATDevice_GetOTA_NumberOfValues);
            break;
        }
        case CordeliaI_ATDevice_GetId_CSR:
        {
            return (option < CordeliaI_ATDevice_GetCSR_NumberOfValues);
            break;
        }
        default:
        {
            return false;
        }
    }
    return false;
}

/**
 * @brief Adds the arguments to the AT+set command string.
 *
 * @param[in] id The id of the arguments to add
 * @param[in] option The option to add
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[out] pValues Parsed values
 * @return true if arguments were added successful, false otherwise
 */
static bool CordeliaI_ATDevice_AddArgumentsATset(char* pAtCommand, uint8_t id, uint8_t option, void* pValue)
{

    /* add id */
    if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetIdStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    switch (id)
    {

        case CordeliaI_ATDevice_GetId_CSR:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetCSRStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetCSR_Country:
                case CordeliaI_ATDevice_GetCSR_State:
                case CordeliaI_ATDevice_GetCSR_Locality:
                case CordeliaI_ATDevice_GetCSR_Surname:
                case CordeliaI_ATDevice_GetCSR_Organization:
                case CordeliaI_ATDevice_GetCSR_Unit:
                case CordeliaI_ATDevice_GetCSR_Email:
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                default:
                {
                    return false;
                }
            }
            break;
        }
        case CordeliaI_ATDevice_GetId_PubTopic0:
        case CordeliaI_ATDevice_GetId_PubTopic1:
        case CordeliaI_ATDevice_GetId_PubTopic2:
        case CordeliaI_ATDevice_GetId_PubTopic3:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetPubTopicStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {

                case CordeliaI_ATDevice_GetPubTopic_Name:
                {
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetPubTopic_Qos:
                {
                    if (!ATCommand_AppendArgumentInt(pAtCommand, *((uint8_t*)pValue), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetPubTopic_Retain:
                {
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, *((bool*)pValue), ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_SubTopic0:
        case CordeliaI_ATDevice_GetId_SubTopic1:
        case CordeliaI_ATDevice_GetId_SubTopic2:
        case CordeliaI_ATDevice_GetId_SubTopic3:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetSubTopicStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetSubTopic_Name:
                {
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetSubTopic_Qos:
                {
                    if (!ATCommand_AppendArgumentInt(pAtCommand, *((uint8_t*)pValue), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_Quarklink:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetQuarklinkStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetQuarklink_Hostname:
                case CordeliaI_ATDevice_GetQuarklink_RootCAPath:
                {
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetQuarklink_Port:
                {
                    if (!ATCommand_AppendArgumentInt(pAtCommand, *((uint32_t*)pValue), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_OTA:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetOTAStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetOTA_URL:
                case CordeliaI_ATDevice_GetOTA_RootCAPath:
                case CordeliaI_ATDevice_GetOTA_FirmwareVersion:
                {
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_MQTT:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetMQTTStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetMQTT_IotHubEndpoint:
                case CordeliaI_ATDevice_GetMQTT_ClientCertPath:
                case CordeliaI_ATDevice_GetMQTT_RootCAPath:
                case CordeliaI_ATDevice_GetMQTT_ClientPrivateKey:
                case CordeliaI_ATDevice_GetMQTT_ClientId:
                case CordeliaI_ATDevice_GetMQTT_Flags:
                case CordeliaI_ATDevice_GetMQTT_WillTopic:
                case CordeliaI_ATDevice_GetMQTT_WillMessage:
                case CordeliaI_ATDevice_GetMQTT_UserName:
                case CordeliaI_ATDevice_GetMQTT_Password:
                {
                    if (!ATCommand_AppendArgumentString(pAtCommand, (char*)pValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_IotHubPort:
                case CordeliaI_ATDevice_GetMQTT_KeepAliveTimeOut:
                {
                    if (!ATCommand_AppendArgumentInt(pAtCommand, *((uint32_t*)pValue), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_WillQos:
                {
                    if (!ATCommand_AppendArgumentInt(pAtCommand, *((uint8_t*)pValue), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetMQTT_TLS1_3:
                case CordeliaI_ATDevice_GetMQTT_Base64:
                case CordeliaI_ATDevice_GetMQTT_WillRetain:
                case CordeliaI_ATDevice_GetMQTT_CleanConnect:
                {
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, *((bool*)pValue), ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        case CordeliaI_ATDevice_GetId_UART:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, CordeliaI_ATDevice_ATGetUartStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case CordeliaI_ATDevice_GetUart_Baudrate:
                {
                    uint32_t baudValue = *((uint32_t*)pValue);
                    if (!ATCommand_AppendArgumentInt(pAtCommand, baudValue, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_Parity:
                {
                    CordeliaI_ATDevice_UartParity_t parityValue = *((CordeliaI_ATDevice_UartParity_t*)pValue);
                    if (!ATCommand_AppendArgumentInt(pAtCommand, parityValue, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_FlowControl:
                {
                    bool boolValue = *((bool*)pValue);
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, boolValue, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_TransparentTrigger:
                {
                    uint8_t triggerValue = *((uint8_t*)pValue);
                    if (!ATCommand_AppendArgumentBitmask(pAtCommand, CordeliaI_ATDevice_ATGetTransparentModeUartTriggerStrings, CordeliaI_ATDevice_TransparentModeUartTrigger_NumberOfValues, triggerValue, ATCOMMAND_STRING_TERMINATE, AT_MAX_COMMAND_BUFFER_SIZE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_TransparentTimeout:
                {
                    uint32_t timeoutValue = *((uint32_t*)pValue);
                    if (!ATCommand_AppendArgumentInt(pAtCommand, timeoutValue, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }
                case CordeliaI_ATDevice_GetUart_TransparentETX:
                {
                    uint8_t* ETXByteArray = (uint8_t*)pValue;
                    uint16_t etx = ((uint16_t)ETXByteArray[0] << 8) | ETXByteArray[1];
                    if (!ATCommand_AppendArgumentInt(pAtCommand, etx, (ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_STRING_TERMINATE))
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
            break;
        }
        default:
        {
            return false;
        }
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}
