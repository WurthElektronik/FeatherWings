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
 * @brief AT commands for basic device functionality.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* Calypso_ATDevice_ATGetIdStrings[Calypso_ATDevice_GetId_NumberOfValues] = {"status", "general", "IOT", "UART", "transparent_mode", "GPIO"};

static const char* Calypso_ATDevice_ATGetStatusStrings[Calypso_ATDevice_GetStatus_NumberOfValues] = {"device", "WLAN", "BSD", "netapp"};

static const char* Calypso_ATDevice_ATGetGeneralStrings[Calypso_ATDevice_GetGeneral_NumberOfValues] = {"version", "time", "persistent"};

static const char* Calypso_ATDevice_ATGetIotStrings[Calypso_ATDevice_GetIot_NumberOfValues] = {"UDID"};

static const char* Calypso_ATDevice_ATGetUartStrings[Calypso_ATDevice_GetUart_NumberOfValues] = {"baudrate", "parity", "flowcontrol", "transparent_trigger", "transparent_timeout", "transparent_etx"};

static const char* Calypso_ATDevice_ATGetTransparentModeStrings[Calypso_ATDevice_GetTransparentMode_NumberOfValues] = {"remote_address", "remote_port", "local_port", "socket_type", "secure_method", "power_save", "skip_date_verify", "disable_cert_store"};

static const char* Calypso_ATDevice_ATGetGPIOStrings[Calypso_ATDevice_GetGPIO_NumberOfValues] = {"remote_lock"};

static const char* Calypso_ATDevice_ATGetTransparentModeSocketTypeStrings[Calypso_ATDevice_TransparentModeSocketType_NumberOfValues] = {"udp", "tcp_server", "tcp_client"};

static const char* Calypso_ATDevice_ATGetTransparentModeSecureMethodStrings[Calypso_ATDevice_TransparentModeSecureMethod_NumberOfValues] = {"none", "sslv3", "tlsv1", "tlsv1_1", "tlsv1_2", "sslv3_tlsv1_2"};

static const char* Calypso_ATDevice_ATGetTransparentModeUartTriggerStrings[Calypso_ATDevice_TransparentModeUartTrigger_NumberOfValues] = {"timer", "1etx", "2etx", "transmit_etx"};

static const char* Calypso_ATDevice_StatusFlagsStrings[Calypso_ATDevice_StatusFlags_NumberOfValues] = {
    "general_error", "wlanasynconnectedresponse", "wlanasyncdisconnectedresponse", "sta_connected", "sta_disconnected", "p2p_dev_found", "connection_failed", "p2p_neg_req_received", "rx_filters", "wlan_sta_connected", "tx_failed", "ipacquired", "ipacquired_v6", "ip_leased", "ip_released", "ipv4_lost", "dhcp_acquire_timeout", "ip_collision", "ipv6_lost"};

static bool Calypso_ATDevice_IsInputValidATget(Calypso_ATDevice_GetId_t id, uint8_t option);
static bool Calypso_ATDevice_IsInputValidATset(Calypso_ATDevice_GetId_t id, uint8_t option);
static bool Calypso_ATDevice_AddArgumentsATget(char* pAtCommand, uint8_t id, uint8_t option);
static bool Calypso_ATDevice_AddArgumentsATset(char* pAtCommand, uint8_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue);
static bool Calypso_ATDevice_ParseResponseATget(uint8_t id, uint8_t option, char* pAtCommand, Calypso_ATDevice_Value_t* pValue);

/**
 * @brief Tests the connection to the wireless module (using the AT+test command).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Test()
{
    if (!Calypso_SendRequest("AT+test\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Starts the network processor (using the AT+start command).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Start()
{
    if (!Calypso_SendRequest("AT+start\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Stops the network processor (using the AT+stop command).
 *
 * @param[in] timeoutMs Timeout in milliseconds.
 *
 * @return true if successful, false otherwise
 */

bool Calypso_ATDevice_Stop(uint32_t timeoutMs)
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

    if (!Calypso_SendRequest(cmdToSend))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Restarts the network processor by sending an AT+stop followed by an AT+start command.
 *
 * @param[in] timeoutMs Timeout for stop command
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Restart(uint32_t timeoutMs)
{
    if (!Calypso_ATDevice_Stop(timeoutMs))
    {
        /* Stop command will fail with error -2018 if the network processor is not active at
		 * the time of processing the command. This is not considered to be an error. */
        if (Calypso_GetLastError(NULL) != -2018)
        {
            return false;
        }
    }
    return Calypso_ATDevice_Start();
}

/**
 * @brief Reboots the device (using the AT+reboot command).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Reboot()
{
    if (!Calypso_SendRequest("AT+reboot\r\n"))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Performs a factory reset of the device (using the AT+factoryReset command).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_FactoryReset()
{
    if (!Calypso_SendRequest("AT+factoryreset\r\n"))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_FactoryReset), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Puts the wireless module into the lowest possible power mode for the supplied duration (using the AT+sleep command).
 *
 * @param[in] timeoutSeconds Timeout until the module wakes up again.
 *                           Must be in range ATDEVICE_SLEEP_MIN_TIMEOUT <= timeoutInSeconds <= ATDEVICE_SLEEP_MAX_TIMEOUT.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Sleep(uint32_t timeoutSeconds)
{

    if ((timeoutSeconds < ATDEVICE_SLEEP_MIN_TIMEOUT) || (timeoutSeconds > ATDEVICE_SLEEP_MAX_TIMEOUT))
    {
        return false;
    }

    char cmdToSend[32] = "AT+sleep=";

    if (!ATCommand_IntToString(&cmdToSend[strlen(cmdToSend)], timeoutSeconds, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(cmdToSend, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(cmdToSend))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Puts the module in power save mode.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_PowerSave()
{
    if (!Calypso_SendRequest("AT+powersave\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
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
bool Calypso_ATDevice_Get(Calypso_ATDevice_GetId_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue)
{
    if (!Calypso_ATDevice_IsInputValidATget(id, option))
    {
        return false;
    }
    char* pRequestCommand = AT_commandBuffer;
    char* pResponseCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+get=");

    if (!Calypso_ATDevice_AddArgumentsATget(&pRequestCommand[strlen(pRequestCommand)], id, option))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!Calypso_ATDevice_ParseResponseATget(id, option, pResponseCommand, pValue))
    {
        return false;
    }

    /* If getting version info: Store firmware version info for later use (in Calypso_firmwareVersionMajor/Minor/Patch).
	 * Note: Firmware versions below 1.9.0 don't return Calypso firmware version info - in that case the version can't be determined. */
    if (id == Calypso_ATDevice_GetId_General && option == Calypso_ATDevice_GetGeneral_Version && pValue->general.version.calypsoFirmwareVersion[0] != '\0')
    {
        char* fw = pValue->general.version.calypsoFirmwareVersion;
        uint8_t major, minor, patch;
        if (!ATCommand_GetNextArgumentInt(&fw, &major, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
        {
            return false;
        }
        if (!ATCommand_GetNextArgumentInt(&fw, &minor, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
        {
            return false;
        }
        if (!ATCommand_GetNextArgumentInt(&fw, &patch, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
        {
            return false;
        }
        Calypso_firmwareVersionMajor = major;
        Calypso_firmwareVersionMinor = minor;
        Calypso_firmwareVersionPatch = patch;
    }

    return true;
}

/**
 * @brief Sets device parameters (using the AT+set command).
 *
 * @param[in] id ID of parameter to be set. Valid IDs are Calypso_ATDevice_GetId_General and Calypso_ATDevice_GetId_UART.
 * @param[in] option Option to set. Valid values depend on ID.
 * @param[out] pValues Values for the specific ID/option
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_Set(Calypso_ATDevice_GetId_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue)
{

    if (!Calypso_ATDevice_IsInputValidATset(id, option))
    {
        return false;
    }
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+set=");

    if (!Calypso_ATDevice_AddArgumentsATset(&pRequestCommand[strlen(pRequestCommand)], id, option, pValue))
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
 * @brief Starts provisioning mode.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATDevice_StartProvisioning()
{
    if (!Calypso_SendRequest("AT+provisioningStart\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
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
bool Calypso_ATDevice_PrintStatusFlags(uint32_t flags, char* pOutStr, size_t maxLength)
{
    pOutStr[0] = '\0';
    return ATCommand_AppendArgumentBitmask(pOutStr, Calypso_ATDevice_StatusFlagsStrings, Calypso_ATDevice_StatusFlags_NumberOfValues, flags, ATCOMMAND_STRING_TERMINATE, maxLength);
}

/**
 * @brief Checks if parameters are valid for the AT+get command.
 *
 * @param[in] id The AT+get ID
 * @param[in] option The AT+get option
 *
 * @return true if arguments are valid, false otherwise
 */
bool Calypso_ATDevice_IsInputValidATget(Calypso_ATDevice_GetId_t id, uint8_t option)
{

    if (id >= Calypso_ATDevice_GetId_NumberOfValues)
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATDevice_GetId_Status:
        {
            return (option < Calypso_ATDevice_GetStatus_NumberOfValues);
        }
        case Calypso_ATDevice_GetId_General:
        {
            return (option < Calypso_ATDevice_GetGeneral_NumberOfValues);
        }
        case Calypso_ATDevice_GetId_IOT:
        {
            return (option < Calypso_ATDevice_GetIot_NumberOfValues);
        }
        case Calypso_ATDevice_GetId_UART:
        {
            return (option < Calypso_ATDevice_GetUart_NumberOfValues);
        }
        case Calypso_ATDevice_GetId_TransparentMode:
        {
            return (option < Calypso_ATDevice_GetTransparentMode_NumberOfValues);
        }
        case Calypso_ATDevice_GetId_GPIO:
        {
            return (option < Calypso_ATDevice_GetGPIO_NumberOfValues);
        }
        default:
        {
            return false;
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
bool Calypso_ATDevice_AddArgumentsATget(char* pAtCommand, uint8_t id, uint8_t option)
{
    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetIdStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATDevice_GetId_Status:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetStatusStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case Calypso_ATDevice_GetId_General:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetGeneralStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case Calypso_ATDevice_GetId_IOT:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetIotStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case Calypso_ATDevice_GetId_UART:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetUartStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case Calypso_ATDevice_GetId_TransparentMode:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetTransparentModeStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case Calypso_ATDevice_GetId_GPIO:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetGPIOStrings[option], ATCOMMAND_STRING_TERMINATE))
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
bool Calypso_ATDevice_ParseResponseATget(uint8_t id, uint8_t option, char* pAtCommand, Calypso_ATDevice_Value_t* pValue)
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
        case Calypso_ATDevice_GetId_Status:
        {
            /* All options return a bitmask */
            return ATCommand_GetNextArgumentBitmask(&pAtCommand, Calypso_ATDevice_StatusFlagsStrings, Calypso_ATDevice_StatusFlags_NumberOfValues, 32, &pValue->status, ATCOMMAND_STRING_TERMINATE);
        }

        case Calypso_ATDevice_GetId_General:
        {
            switch (option)
            {
                case Calypso_ATDevice_GetGeneral_Version:
                {
                    Calypso_ATDevice_Version_t* version = &pValue->general.version;
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, version->chipId, ATCOMMAND_ARGUMENT_DELIM, sizeof(version->chipId)))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, version->MACVersion, ATCOMMAND_ARGUMENT_DELIM, sizeof(version->MACVersion)))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, version->PHYVersion, ATCOMMAND_ARGUMENT_DELIM, sizeof(version->PHYVersion)))
                    {
                        return false;
                    }
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, version->NWPVersion, ATCOMMAND_ARGUMENT_DELIM, sizeof(version->NWPVersion)))
                    {
                        return false;
                    }
                    /* Calypso firmware version is only returned by firmware versions >= 1.9.0 - in that case, there is another argument after the ROM version.
			 * Otherwise, the ROM version is the last argument (followed by ATCOMMAND_STRING_TERMINATE). */

                    if (ATCommand_GetNextArgumentString(&pAtCommand, version->ROMVersion, ATCOMMAND_ARGUMENT_DELIM, sizeof(version->ROMVersion)))
                    {
                        return ATCommand_GetNextArgumentString(&pAtCommand, version->calypsoFirmwareVersion, ATCOMMAND_STRING_TERMINATE, sizeof(version->calypsoFirmwareVersion));
                    }
                    else
                    {
                        version->calypsoFirmwareVersion[0] = '\0';
                        return ATCommand_GetNextArgumentString(&pAtCommand, version->ROMVersion, ATCOMMAND_STRING_TERMINATE, sizeof(version->ROMVersion));
                    }
                    break;
                }

                case Calypso_ATDevice_GetGeneral_Time:
                {
                    Calypso_ATDevice_Time_t* time = &pValue->general.time;

                    char tempString[6];

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->hour), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->minute), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->second), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->day), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->month), tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&(time->year), tempString, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATDevice_GetGeneral_Persistent:
                {
                    char tempString[6];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&pValue->general.persistent, tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
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

        case Calypso_ATDevice_GetId_IOT:
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
            sprintf((char*)&pValue->iot.udid, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", udidArr[0], udidArr[1], udidArr[2], udidArr[3], udidArr[4], udidArr[5], udidArr[6], udidArr[7], udidArr[8], udidArr[9], udidArr[10], udidArr[11], udidArr[12], udidArr[13], udidArr[14], udidArr[15]);
            break;
        }

        case Calypso_ATDevice_GetId_UART:
        {
            switch (option)
            {
                case Calypso_ATDevice_GetUart_Baudrate:
                {
                    char tempString[12];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    if (!ATCommand_StringToInt(&pValue->uart.baudrate, tempString, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATDevice_GetUart_Parity:
                {
                    char tempString[6];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    uint8_t parity;
                    if (!ATCommand_StringToInt(&parity, tempString, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC))
                    {
                        return false;
                    }
                    pValue->uart.parity = parity;
                    break;
                }

                case Calypso_ATDevice_GetUart_FlowControl:
                {
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, &pValue->uart.flowControl, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATDevice_GetUart_TransparentTrigger:
                {
                    uint32_t bitmask;
                    if (!ATCommand_GetNextArgumentBitmask(&pAtCommand, Calypso_ATDevice_ATGetTransparentModeUartTriggerStrings, Calypso_ATDevice_TransparentModeUartTrigger_NumberOfValues, 32, &bitmask, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    pValue->uart.transparentTrigger = (uint8_t)bitmask;
                    break;
                }

                case Calypso_ATDevice_GetUart_TransparentTimeout:
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, &pValue->uart.transparentTimeoutMs, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_TransparentETX:
                {
                    uint16_t etx;
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, &etx, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    pValue->uart.transparentETX[0] = (etx >> 8) & 0xFF;
                    pValue->uart.transparentETX[1] = etx & 0xFF;
                    break;
                }
                default:
                {
                    return false;
                }
            }

            break;
        }

        case Calypso_ATDevice_GetId_TransparentMode:
        {
            switch (option)
            {
                case Calypso_ATDevice_GetTransparentMode_RemoteAddress:
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, pValue->transparentMode.remoteAddress, ATCOMMAND_STRING_TERMINATE, sizeof(pValue->transparentMode.remoteAddress)))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_RemotePort:
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, &pValue->transparentMode.remotePort, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_LocalPort:
                    if (!ATCommand_GetNextArgumentInt(&pAtCommand, &pValue->transparentMode.localPort, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_SocketType:
                {
                    char tempString[32];
                    if (!ATCommand_GetNextArgumentString(&pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    bool ret;
                    pValue->transparentMode.socketType = ATCommand_FindString(Calypso_ATDevice_ATGetTransparentModeSocketTypeStrings, Calypso_ATDevice_TransparentModeSocketType_NumberOfValues, tempString, Calypso_ATDevice_TransparentModeSocketType_UDP, &ret);
                    if (!ret)
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATDevice_GetTransparentMode_SecureMethod:
                {
                    uint8_t enumValue;
                    if (!ATCommand_GetNextArgumentEnum(&pAtCommand, &enumValue, Calypso_ATDevice_ATGetTransparentModeSecureMethodStrings, Calypso_ATDevice_TransparentModeSecureMethod_NumberOfValues, 25, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    pValue->transparentMode.secureMethod = enumValue;
                    break;
                }

                case Calypso_ATDevice_GetTransparentMode_PowerSave:
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, &pValue->transparentMode.powerSave, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_SkipDateVerify:
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, &pValue->transparentMode.skipDateVerify, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_DisableCertificateStore:
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, &pValue->transparentMode.disableCertificateStore, ATCOMMAND_STRING_TERMINATE))
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

        case Calypso_ATDevice_GetId_GPIO:
        {
            switch (option)
            {
                case Calypso_ATDevice_GetGPIO_RemoteLock:
                    if (!ATCommand_GetNextArgumentBoolean(&pAtCommand, &pValue->gpio.remoteLock, ATCOMMAND_STRING_TERMINATE))
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
static bool Calypso_ATDevice_IsInputValidATset(Calypso_ATDevice_GetId_t id, uint8_t option)
{
    switch (id)
    {
        case Calypso_ATDevice_GetId_General:
        {
            /* only option time and persistent can be written*/
            return ((Calypso_ATDevice_GetGeneral_Persistent == option) || (Calypso_ATDevice_GetGeneral_Time == option));
            break;
        }

        case Calypso_ATDevice_GetId_UART:
        {
            return (option < Calypso_ATDevice_GetUart_NumberOfValues);
            break;
        }

        case Calypso_ATDevice_GetId_TransparentMode:
        {
            return (option < Calypso_ATDevice_GetTransparentMode_NumberOfValues);
            break;
        }

        case Calypso_ATDevice_GetId_GPIO:
        {
            return (option < Calypso_ATDevice_GetGPIO_NumberOfValues);
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
static bool Calypso_ATDevice_AddArgumentsATset(char* pAtCommand, uint8_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue)
{

    /* add id */
    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetIdStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATDevice_GetId_General:
        {
            /* add option */
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetGeneralStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case Calypso_ATDevice_GetGeneral_Persistent:
                {
                    char tempString[4];
                    if (!ATCommand_IntToString(tempString, pValue->general.persistent, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }
                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATDevice_GetGeneral_Time:
                {
                    char tempString[4];

                    if (!ATCommand_IntToString(tempString, pValue->general.time.hour, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_IntToString(tempString, pValue->general.time.minute, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_IntToString(tempString, pValue->general.time.second, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_IntToString(tempString, pValue->general.time.day, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_IntToString(tempString, pValue->general.time.month, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_IntToString(tempString, pValue->general.time.year, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC)))
                    {
                        return false;
                    }

                    if (!ATCommand_AppendArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE))
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

        case Calypso_ATDevice_GetId_UART:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetUartStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case Calypso_ATDevice_GetUart_Baudrate:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValue->uart.baudrate, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_Parity:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValue->uart.parity, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_FlowControl:
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, pValue->uart.flowControl, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_TransparentTrigger:
                    if (!ATCommand_AppendArgumentBitmask(pAtCommand, Calypso_ATDevice_ATGetTransparentModeUartTriggerStrings, Calypso_ATDevice_TransparentModeUartTrigger_NumberOfValues, pValue->uart.transparentTrigger, ATCOMMAND_STRING_TERMINATE, AT_MAX_COMMAND_BUFFER_SIZE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_TransparentTimeout:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValue->uart.transparentTimeoutMs, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetUart_TransparentETX:
                {
                    uint16_t etx = ((uint16_t)pValue->uart.transparentETX[0] << 8) | pValue->uart.transparentETX[1];
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

        case Calypso_ATDevice_GetId_TransparentMode:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetTransparentModeStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case Calypso_ATDevice_GetTransparentMode_RemoteAddress:
                    if (!ATCommand_AppendArgumentString(pAtCommand, pValue->transparentMode.remoteAddress, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_RemotePort:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValue->transparentMode.remotePort, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_LocalPort:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValue->transparentMode.localPort, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_SocketType:
                    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetTransparentModeSocketTypeStrings[pValue->transparentMode.socketType], ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_SecureMethod:
                    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetTransparentModeSecureMethodStrings[pValue->transparentMode.secureMethod], ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_PowerSave:
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, pValue->transparentMode.powerSave, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_SkipDateVerify:
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, pValue->transparentMode.skipDateVerify, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATDevice_GetTransparentMode_DisableCertificateStore:
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, pValue->transparentMode.disableCertificateStore, ATCOMMAND_STRING_TERMINATE))
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

        case Calypso_ATDevice_GetId_GPIO:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATDevice_ATGetGPIOStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            switch (option)
            {
                case Calypso_ATDevice_GetGPIO_RemoteLock:
                    if (!ATCommand_AppendArgumentBoolean(pAtCommand, pValue->gpio.remoteLock, ATCOMMAND_STRING_TERMINATE))
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

        default:
        {
            return false;
        }
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}
