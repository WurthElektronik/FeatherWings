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
 * @brief AT commands for Wifi functionality.
 */

#include <StephanoI/ATCommands/ATWifi.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

/**
 * @brief Enable/disable the Wifi
 *
 * @param[in] enable Enable or disable
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Init(bool enable)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWINIT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Read the Wifi driver state
 *
 * @param[in] enableP Enable or disable
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetInit(bool* enableP)
{
    char responsebuffer[5];
    if (!StephanoI_SendRequest("AT+CWINIT?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseInit(responsebuffer, enableP);
}

/**
 * @brief Set the Wifi mode
 *
 * @param[in] mode Mode of the Wifi device
 * @param[in] autoconnect Autoconnect
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_t mode, bool autoconnect)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWMODE=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, autoconnect ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the Wifi mode
 *
 * @param[out] modeP Pointer to the wifi mode
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetMode(StephanoI_ATWifiMode_t* modeP)
{
    char responsebuffer[2];
    if (!StephanoI_SendRequest("AT+CWMODE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseMode(responsebuffer, modeP);
}

/**
 * @brief Get the Wifi state
 *
 * @param[out] stateP Pointer to the wifi state
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetState(StephanoI_ATWiFi_State_t* stateP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_State_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSTATE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseState(responsebuffer, stateP);
}

/**
 * @brief Set the country code
 *
 * @param[in] t Country code
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_SetCountryCode(StephanoI_ATWifiCountryCode_t t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWCOUNTRY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.policy, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, t.country_code, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.start_channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.total_channel_count, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the country code
 *
 * @param[out] t Pointer to country code type
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetCountryCode(StephanoI_ATWifiCountryCode_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWifiCountryCode_t) + 1];
    if (!StephanoI_SendRequest("AT+CWCOUNTRY?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseCountryCode(responsebuffer, t);
}

/**
 * @brief Set the host name
 *
 * @param[in] name Host name
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_SetHostname(StephanoI_ATWiFi_Hostname_t name)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWHOSTNAME=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, name, ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the host name
 *
 * @param[in] nameP Pointer to the host name
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetHostname(StephanoI_ATWiFi_Hostname_t* nameP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_Hostname_t) + 1];
    if (!StephanoI_SendRequest("AT+CWHOSTNAME?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseHostname(responsebuffer, nameP);
}

/**
 * @brief Scan options
 *
 * @param[in] ssid SSID to scan for
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_ScanAPOptions(uint16_t printmask, int8_t rssi, uint16_t authmode_mask)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWLAPOPT=1,");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, printmask, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, rssi, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, authmode_mask, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiScan), StephanoI_CNFStatus_Success);
}

/**
 * @brief Scan APs
 *
 * @param[in] ssid SSID to scan for
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_ScanAPs(char* ssid)
{
    char* pRequestCommand = AT_commandBuffer;

    if (ssid == NULL)
    {
        strcpy(pRequestCommand, "AT+CWLAP\r\n");
    }
    else
    {
        strcpy(pRequestCommand, "AT+CWLAP=");
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiScan), StephanoI_CNFStatus_Success);
}

/**
 * @brief Connect to AP
 *
 * @param[in] ssid SSID to connect to
 * @param[in] password Pass word
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_Connect(char* ssid, char* password)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWJAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_ARGUMENT_DELIM))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiConnect), StephanoI_CNFStatus_Success);
}

/**
 * @brief Configure the reconnection settings
 *
 * @param[in] interval_s   Reconnect interval in s
 * @param[in] repeat_count Repeat count
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_ReConnectConfiguration(uint16_t interval_s, uint16_t repeat_count)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWRECONNCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, interval_s, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, repeat_count, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Connect to WPA2 Enterprise AP
 *
 * @param[in] ssid SSID to connect to
 * @param[in] method Method
 * @param[in] identity Identity
 * @param[in] username User name
 * @param[in] password Pass word
 * @param[in] security Security
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_ConnectWPA2Enterprise(char* ssid, StephanoI_ATWifiWPA2Method_t method, char* identity, char* username, char* password, StephanoI_ATWPA2SecurityFlags_t security)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWJEAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)method, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, identity, ATCOMMAND_ARGUMENT_DELIM))
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
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)security, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiConnect), StephanoI_CNFStatus_Success);
}

/**
 * @brief Disconnect from AP
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_Disconnect()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWQAP\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Enable the autoconnect mode
 *
 * @param[in] autoconnect Autoconnect
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_AutoConnect(bool autoconnect)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWAUTOCONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, autoconnect ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the IP of a station
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_GetIP()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTA?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Set the MAC of a station
 *
 * @param[in] mac MAC
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_SetMAC(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTAMAC=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the MAC of the station
 *
 * @param[out] t Pointer to the MAC
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_GetMAC(StephanoI_ATWiFi_MAC_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_MAC_t) + 1];
    if (!StephanoI_SendRequest("AT+CIPSTAMAC?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetMAC(responsebuffer, t);
}

/**
 * @brief Set the protocol standard of the station
 *
 * @param[in] prot Protocol standard
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWSTAPROTO=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)prot, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the protocol standard of the station
 *
 * @param[in] protP Pointer to the protocol standard
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_Station_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP)
{
    char responsebuffer[sizeof(StephanoI_ATWifiProtocolFlags_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSTAPROTO?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseProtocol(responsebuffer, protP);
}

/**
 * @brief Set the MAC of a AP
 *
 * @param[in] mac MAC
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_SetMAC(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAPMAC=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the MAC of a AP
 *
 * @param[out] t Pointer to the MAC
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetMAC(StephanoI_ATWiFi_MAC_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_MAC_t) + 1];
    if (!StephanoI_SendRequest("AT+CIPAPMAC?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetMAC(responsebuffer, t);
}

/**
 * @brief Configure the AP
 *
 * @param[in] config Configuration
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_SetConfiguration(StephanoI_ATWiFi_APConfiguration_t config)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWSAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, config.ssid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, config.pwd, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.ecn, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.max_connections, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.ssid_hidden ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_FactoryReset), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the AP configuration
 *
 * @param[in] configP Pointer to the configuration
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetConfiguration(StephanoI_ATWiFi_APConfiguration_t* configP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_APConfiguration_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSAP?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseAPConfiguration(responsebuffer, configP);
}

/**
 * @brief Get the IP of the connected stations
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetConnectedStations()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWLIF\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Disconnect stations from AP
 *
 * @param[in] mac MAC to disconnect
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_DisconnectStation(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    if (mac == NULL)
    {
        strcpy(pRequestCommand, "AT+CWQIF\r\n");
    }
    else
    {
        strcpy(pRequestCommand, "AT+CWQIF=");

        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Set the protocol standard of the AP
 *
 * @param[in] prot Protocol standard
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWAPPROTO=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)prot, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the protocol standard of the AP
 *
 * @param[in] protP Pointer to the protocol standard
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP)
{
    char responsebuffer[sizeof(StephanoI_ATWifiProtocolFlags_t) + 1];
    if (!StephanoI_SendRequest("AT+CWAPPROTO?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseProtocol(responsebuffer, protP);
}

/**
 * @brief Enable the DHCP
 *
 * @param[in] enable Enable or disable
 * @param[in] mode   Mode (bit0 Station DHCP, bit1 AP DHCP)
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_EnableDHCP(bool enable, StephanoI_ATWifiDHCPFlags_t mode)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWDHCP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the DHCP state
 *
 * @param[in] stateP Pointer to the protocol standard
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_GetDHCPState(uint8_t* stateP)
{
    char responsebuffer[5];
    if (!StephanoI_SendRequest("AT+CWDHCP?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetDHCPState(responsebuffer, stateP);
}

/**
 * @brief Configure IP address range of AP
 *
 * @param[in] enable Enable or disable
 * @param[in] range IP range
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_SetIPrangeConfiguration(bool enable, StephanoI_ATWiFi_IPRange_t range)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWDHCPS=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, range.lease_time, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, range.start_ip, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, range.end_ip, ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Get the DHCP state
 *
 * @param[in] rangeP Pointer to the IP range
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetIPrangeConfiguration(StephanoI_ATWiFi_IPRange_t* rangeP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_IPRange_t) + 5];
    if (!StephanoI_SendRequest("AT+CWDHCPS?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetIPrangeConfiguration(responsebuffer, rangeP);
}

/**
 * @brief Get the IP of a AP
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_GetIP()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAP?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Set the IP of a AP
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_AP_SetIP(char* ip, char* gateway, char* netmask)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ip, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, gateway, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, netmask, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

/**
 * @brief Enable/disable the MDNS
 *
 * @param[in] enable       Enable or disable the MDNS
 * @param[in] host_name    Host name
 * @param[in] service_name Service name
 * @param[in] port         Port
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATWifi_EnableMDNS(bool enable, char* host_name, char* service_name, uint16_t port)
{
    char* pRequestCommand = AT_commandBuffer;

    if (enable)
    {
        strcpy(pRequestCommand, "AT+MDNS=");

        if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, host_name, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, service_name, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        strcpy(pRequestCommand, "AT+MDNS=0\r\n");
    }
    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

/**
 * @brief Parses the values of the Wifi mode arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseMode(char* EventArgumentsP, StephanoI_ATWifiMode_t* t)
{
    char* argumentsP = EventArgumentsP;

    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the Wifi Scan arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseScan(char* EventArgumentsP, StephanoI_ATWiFi_Scan_t* t)
{
    /*ignore first '(' */
    char* argumentsP = EventArgumentsP + 1;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->enc), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ssid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->ssid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->rssi), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->MAC)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->freq_offset), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->freqcal_val), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->pairwise_cipher), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->group_cipher), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->bgn), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->wps_flags), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ')'))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the Wifi state arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseState(char* EventArgumentsP, StephanoI_ATWiFi_State_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->state), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ssid, ATCOMMAND_STRING_TERMINATE, sizeof(t->ssid)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the Wifi GetconnectedIPs arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseGetConnectedIPs(char* EventArgumentsP, StephanoI_ATWiFi_GetConnectedIPs_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->IP, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->IP)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_STRING_TERMINATE, sizeof(t->MAC)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the Wifi GetIP arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseGetIP(char* EventArgumentsP, StephanoI_ATWiFi_GetIP_t* t)
{
    char* argumentsP = EventArgumentsP;
    char type[16];
    if (!ATCommand_GetNextArgumentString(&argumentsP, type, ':', sizeof(type)))
    {
        return false;
    }
    if (0 == strcmp(type, "ip"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->IP, ATCOMMAND_STRING_TERMINATE, sizeof(t->IP)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "gateway"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->gateway, ATCOMMAND_STRING_TERMINATE, sizeof(t->gateway)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "netmask"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->netmask, ATCOMMAND_STRING_TERMINATE, sizeof(t->netmask)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "ip6ll"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip6ll, ATCOMMAND_STRING_TERMINATE, sizeof(t->ip6ll)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "ip6gl"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip6gl, ATCOMMAND_STRING_TERMINATE, sizeof(t->ip6gl)))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the Wifi GetMAC arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseGetMAC(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t)
{
    char* argumentsP = EventArgumentsP;

    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATWiFi_MAC_t));
}

/**
 * @brief Parses the values of the Wifi StationhasConnected arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseStationhasConnected(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t) { return StephanoI_ATWiFi_ParseGetMAC(EventArgumentsP, t); }

/**
 * @brief Parses the values of the Wifi AP assign IP arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseAPAssignIP(char* EventArgumentsP, StephanoI_ATWiFi_APAssignIP_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->MAC)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip, ATCOMMAND_STRING_TERMINATE, sizeof(t->MAC)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the Wifi country code.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseCountryCode(char* EventArgumentsP, StephanoI_ATWifiCountryCode_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->policy), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->country_code, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->country_code)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->start_channel), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->total_channel_count), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return true;
}

/**
 * @brief Parses the values of the Wifi host name.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseHostname(char* EventArgumentsP, StephanoI_ATWiFi_Hostname_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATWiFi_Hostname_t));
}

/**
 * @brief Parses the values of the protocol
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseProtocol(char* EventArgumentsP, StephanoI_ATWifiProtocolFlags_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the Wifi driver state arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseInit(char* EventArgumentsP, bool* t)
{
    char* argumentsP = EventArgumentsP;
    uint8_t val;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &val, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *t = (val == (uint8_t)0x01);
    return true;
}

/**
 * @brief Parses the values of the AP configuration.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseAPConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_APConfiguration_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->ssid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->ssid)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->pwd, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->pwd)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->ecn), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->max_connections), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    uint8_t dummy;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dummy, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    t->ssid_hidden = (dummy == 0x01);
    return true;
}

/**
 * @brief Parses the values of the DHCP state
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseGetDHCPState(char* EventArgumentsP, uint8_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return true;
}

/**
 * @brief Parses the values of the IP range
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATWiFi_ParseGetIPrangeConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_IPRange_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->lease_time), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->start_ip, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->start_ip)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->end_ip, ATCOMMAND_STRING_TERMINATE, sizeof(t->end_ip)))
    {
        return false;
    }
    return true;
}
