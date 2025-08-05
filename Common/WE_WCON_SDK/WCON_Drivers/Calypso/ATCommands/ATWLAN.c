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
 * @brief AT commands for WLAN functionality.
 */
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>

static const char* Calypso_ATWLAN_SetModeStrings[Calypso_ATWLAN_SetMode_NumberOfValues] = {
    "STA",
    "AP",
    "P2P",
};

static const char* Calypso_ATWLAN_SetIDStrings[Calypso_ATWLAN_SetID_NumberOfValues] = {"general", "P2P", "Connection", "AP"};

static const char* Calypso_ATWLAN_SetGetGeneralStrings[Calypso_ATWLAN_SetGeneral_NumberOfValues] = {
    "COUNTRY_CODE", "STA_TX_POWER", "AP_TX_POWER", "SCAN_PARAMS", "SUSPEND_PROFILES", "DISABLE_ENT_SERVER_AUTH",
};

static const char* Calypso_ATWLAN_SetGetP2PStrings[Calypso_ATWLAN_SetP2P_NumberOfValues] = {"CHANNEL_N_REGS"};

static const char* Calypso_ATWLAN_SetGetAPStrings[Calypso_ATWLAN_SetAP_NumberOfValues] = {"SSID", "CHANNEL", "HIDDEN_SSID", "SECURITY", "PASSWORD", "MAX_STATIONS", "MAX_STA_AGING"};

static const char* Calypso_ATWLAN_PolicyStrings[Calypso_ATWLAN_PolicyID_NumberOfValues] = {"connection", "scan", "PM", "P2P"};

static const char* Calypso_ATWLAN_PolicyConnectionStrings[Calypso_ATWLAN_PolicyConnection_NumberOfValues] = {"auto", "fast", "P2P"};

static const char* Calypso_ATWLAN_PolicyScanStrings[Calypso_ATWLAN_PolicyScan_NumberOfValues] = {"Hidden_SSID", "No_Hidden_SSID", "Disable_Scan"};

static const char* Calypso_ATWLAN_PolicyPMStrings[Calypso_ATWLAN_PolicyPM_NumberOfValues] = {"normal", "low_latency", "low_power", "long_sleep"};

static const char* Calypso_ATWLAN_PolicyP2PStrings[Calypso_ATWLAN_PolicyP2P_NumberOfValues] = {"CLIENT", "GROUP_OWNER", "NEGOTIATE"};

static const char* Calypso_ATWLAN_PolicyP2PValue_Strings[Calypso_ATWLAN_PolicyP2PValue_NumberOfValues] = {"ACTIVE", "PASSIVE", "RAND_BACKOFF"};

static const char* Calypso_ATWLAN_SecurityTypeStrings[Calypso_ATWLAN_SecurityType_NumberOfValues] = {"OPEN", "WEP", "WEP_SHARED", "WPA_WPA2", "WPA2_PLUS", "WPA3", "WPA_ENT", "WPS_PBC", "WPS_PIN"};

static const char* Calypso_ATWLAN_APSecurityTypeStrings[Calypso_ATWLAN_APSecurityType_NumberOfValues] = {"OPEN", "WEP", "WPA_WPA2"};

static const char* Calypso_ATWLAN_ScanSecurityTypeStrings[Calypso_ATWLAN_ScanSecurityType_NumberOfValues] = {"OPEN", "WEP", "WPA", "WPA2", "WPA_WPA2"};

static const char* Calypso_ATWLAN_ScanCipherStrings[Calypso_ATWLAN_ScanCipherType_NumberOfValues] = {"NONE", "WEP40", "WEP104", "TKIP", "CCMP"};

static const char* Calypso_ATWLAN_ScanKeyMgmntStrings[Calypso_ATWLAN_ScanKeyManagementType_NumberOfValues] = {"NONE", "802_1_x", "PSK"};

static const char* Calypso_ATWLAN_SecurityEAPStrings[Calypso_ATWLAN_SecurityEAP_NumberOfValues] = {"", "TLS", "TTLS_TLS", "TTLS_MSCHAPv2", "TTLS_PSK", "PEAP0_TLS", "PEAP0_MSCHAPv2", "PEAP0_PSK", "PEAP1_TLS", "PEAP1_PSK"};

static const char* Calypso_ATWLAN_StatusStrings[Calypso_ATWLAN_Status_NumberOfValues] = {
    "disconnected", "station_connected", "p2pcl_connected", "p2pgo_connected", "ap_connected_stations",
};

static const char* Calypso_ATWLAN_SecurityStatusStrings[Calypso_ATWLAN_SecurityStatus_NumberOfValues] = {"open", "wep", "wpa_wpa2", "wps_pbc", "wps_pin", "wpa_ent", "wep_shared"};

static bool Calypso_ATWLAN_IsInputValidWlanGet(Calypso_ATWLAN_SetID_t id, uint8_t option);
static bool Calypso_ATWLAN_IsInputValidWlanSet(Calypso_ATWLAN_SetID_t id, uint8_t option);

static bool Calypso_ATWLAN_AddConnectionArguments(char* pOutString, Calypso_ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim);
static bool Calypso_ATWLAN_AddArgumentsWlanGet(char* pAtCommand, Calypso_ATWLAN_SetID_t id, uint8_t option);
static bool Calypso_ATWLAN_AddArgumentsWlanSet(char* pAtCommand, Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues);

static bool Calypso_ATWLAN_ParseResponseWlanScanEntry(char** pAtCommand, Calypso_ATWLAN_ScanEntry_t* pOutScanEntry);
static bool Calypso_ATWLAN_ParseResponseWlanAddProfile(char** pAtCommand, uint8_t* pOutIndex);
static bool Calypso_ATWLAN_ParseResponseWlanGetProfile(char** pAtCommand, Calypso_ATWLAN_Profile_t* pOutProfile);
static bool Calypso_ATWLAN_ParseResponseWlanGet(Calypso_ATWLAN_SetID_t id, uint8_t option, char** pAtCommand, Calypso_ATWLAN_Settings_t* pValues);

static bool Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_t id, char** pRespondCommand);

/**
 * @brief Sets the wireless LAN mode (using the AT+wlanSetMode command).
 *
 * @param[in] mode WLAN operating mode to be set.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_t mode)
{

    if (mode >= Calypso_ATWLAN_SetMode_NumberOfValues)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanSetMode=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_SetModeStrings[mode], ATCOMMAND_STRING_TERMINATE))
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
 * @brief Initiates a WLAN scan (using the AT+wlanScan command).
 *
 * Note that when calling this function for the first time, an error is returned, as the module responds
 * with SL_ERROR_WLAN_GET_NETWORK_LIST_EAGAIN (-2073).
 *
 * @param[in] index Starting index (0-29)
 * @param[in] deviceCount Max. number of entries to get (max. 30)
 * @param[out] pOutValues The scan entries which the module has returned
 * @param[out] pOutNumEntries Number of entries the module has returned
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_Scan(uint8_t index, uint8_t deviceCount, Calypso_ATWLAN_ScanEntry_t* pOutValues, uint8_t* pOutNumEntries)
{

    if ((index >= 30) || (deviceCount >= 30))
    {
        return false;
    }
    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanScan=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, deviceCount, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_WlanScan), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    uint8_t numEntries = 0;
    for (uint8_t i = 0; i < deviceCount; i++)
    {
        if (Calypso_ATWLAN_ParseResponseWlanScanEntry(&pRespondCommand, &pOutValues[numEntries]))
        {
            numEntries++;
        }
    }
    *pOutNumEntries = numEntries;

    return true;
}

/**
 * @brief Connects to a wireless network (using the AT+wlanConnect command).
 *
 * @param[in] connectArgs Connection parameters
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_Connect(Calypso_ATWLAN_ConnectionArguments_t connectArgs)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanConnect=");

    if (!Calypso_ATWLAN_AddConnectionArguments(pRequestCommand, connectArgs, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Disconnects from a wireless network (using the AT+disconnect command).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_Disconnect()
{
    if (!Calypso_SendRequest("AT+wlanDisconnect\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Adds a wireless LAN profile.
 *
 * Internally sends the AT+wlanProfileAdd command.
 *
 * @param[in] profile WLAN profile to be added.
 * @param[out] pOutIndex The index of the added profile. Can be used to access the profile.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_AddProfile(Calypso_ATWLAN_Profile_t profile, uint8_t* pOutIndex)
{

    if ((pOutIndex == NULL) || (profile.priority >= 16))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanProfileAdd=");

    if (!Calypso_ATWLAN_AddConnectionArguments(pRequestCommand, profile.connection, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, profile.priority, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_WlanAddProfile), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return Calypso_ATWLAN_ParseResponseWlanAddProfile(&pRespondCommand, pOutIndex);
}

/**
 * @brief Gets a wireless LAN profile (using the AT+wlanProfileGet command).
 *
 * @param[in] index Index of the profile as returned by Calypso_ATWLAN_AddProfile().
 * @param[out] pOutProfile The returned WLAN profile.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_GetProfile(uint8_t index, Calypso_ATWLAN_Profile_t* pOutProfile)
{

    if (NULL == pOutProfile)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanProfileGet=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return Calypso_ATWLAN_ParseResponseWlanGetProfile(&pRespondCommand, pOutProfile);
}

/**
 * @brief Deletes a wireless LAN profile (using the AT+wlanProfileDel command).
 *
 * @param[in] index Index of the profile to be deleted (as returned by Calypso_ATWLAN_AddProfile()).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_DeleteProfile(uint8_t index)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanProfileDel=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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
 * @briefs Reads wireless LAN settings (using the AT+wlanGet command).
 *
 * @param[in] id ID of the value to get
 * @param[in] option Option of the value to get
 * @param[out] pValues Values returned by the module
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues)
{

    memset(pValues, 0, sizeof(*pValues));

    if (!Calypso_ATWLAN_IsInputValidWlanGet(id, option))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanGet=");

    if (!Calypso_ATWLAN_AddArgumentsWlanGet(pRequestCommand, id, option))
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

    return Calypso_ATWLAN_ParseResponseWlanGet(id, option, &pRespondCommand, pValues);
}

/**
 * @brief Writes wireless LAN settings (using the AT+wlanSet command).
 *
 * @param[in] id ID of the value to set
 * @param[in] option Option of the value to set
 * @param[in] pValues Values to set
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues)
{

    if (!Calypso_ATWLAN_IsInputValidWlanSet(id, option))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanSet=");

    if (!Calypso_ATWLAN_AddArgumentsWlanSet(pRequestCommand, id, option, pValues))
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
 * @brief Set WLAN connection policy.
 *
 * Note that setting a connection policy while the device parameter
 * Calypso_ATDevice_GetGeneral_Persistent is set to false (0) will return an error (code -31008).
 *
 * @param[in] policy Connection policy flags. See Calypso_ATWLAN_PolicyConnection_t.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_SetConnectionPolicy(uint8_t policy)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyStrings[Calypso_ATWLAN_PolicyID_Connection], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBitmask(pRequestCommand, Calypso_ATWLAN_PolicyConnectionStrings, Calypso_ATWLAN_PolicyConnection_NumberOfValues, policy, ATCOMMAND_ARGUMENT_DELIM, AT_MAX_COMMAND_BUFFER_SIZE))
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
 * @brief Get WLAN connection policy.
 *
 * @param[out] policy Connection policy flags. See Calypso_ATWLAN_PolicyConnection_t.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_GetConnectionPolicy(uint8_t* policy)
{
    *policy = 0;

    char* pRespondCommand = NULL;
    if (!Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_Connection, &pRespondCommand))
    {
        return false;
    }

    uint32_t bitmask;
    if (ATCommand_GetNextArgumentBitmask(&pRespondCommand, Calypso_ATWLAN_PolicyConnectionStrings, Calypso_ATWLAN_PolicyConnection_NumberOfValues, 5, &bitmask, ATCOMMAND_STRING_TERMINATE))
    {
        *policy = (uint8_t)bitmask;
        return true;
    }

    return false;
}

/**
 * @brief Set WLAN scan policy.
 *
 * @param[in] policy Scan policy to set
 * @param[in] scanIntervalSeconds Scan interval in seconds
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_SetScanPolicy(Calypso_ATWLAN_PolicyScan_t policy, uint32_t scanIntervalSeconds)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyStrings[Calypso_ATWLAN_PolicyID_Scan], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyScanStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, scanIntervalSeconds, ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Get WLAN scan policy.
 *
 * @param[out] policy Scan policy
 * @param[out] scanIntervalSeconds Scan interval in seconds
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_GetScanPolicy(Calypso_ATWLAN_PolicyScan_t* policy, uint32_t* scanIntervalSeconds)
{
    *policy = Calypso_ATWLAN_PolicyScan_DisableScan;
    *scanIntervalSeconds = 0;

    char* pRespondCommand = NULL;
    if (!Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_Scan, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(Calypso_ATWLAN_PolicyScanStrings, Calypso_ATWLAN_PolicyScan_NumberOfValues, temp, Calypso_ATWLAN_PolicyScan_DisableScan, &ok);
    if (!ok)
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(&pRespondCommand, scanIntervalSeconds, ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Set WLAN power management policy.
 *
 * @param[in] policy Power management policy to set
 * @param[in] maxSleepTimeMs Max. sleep time in milliseconds
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_SetPMPolicy(Calypso_ATWLAN_PolicyPM_t policy, uint32_t maxSleepTimeMs)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyStrings[Calypso_ATWLAN_PolicyID_PowerManagement], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyPMStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, maxSleepTimeMs, ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Get WLAN power management policy.
 *
 * @param[out] policy Power management policy
 * @param[out] maxSleepTimeMs Max. sleep time in milliseconds
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_GetPMPolicy(Calypso_ATWLAN_PolicyPM_t* policy, uint32_t* maxSleepTimeMs)
{
    *policy = Calypso_ATWLAN_PolicyPM_Normal;
    *maxSleepTimeMs = 0;

    char* pRespondCommand = NULL;
    if (!Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_PowerManagement, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(Calypso_ATWLAN_PolicyPMStrings, Calypso_ATWLAN_PolicyPM_NumberOfValues, temp, Calypso_ATWLAN_PolicyPM_Normal, &ok);
    if (!ok)
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(&pRespondCommand, maxSleepTimeMs, ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Set WLAN P2P policy.
 *
 * @param[in] policy P2P policy to set
 * @param[in] value Option value
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_SetP2PPolicy(Calypso_ATWLAN_PolicyP2P_t policy, Calypso_ATWLAN_PolicyP2PValue_t value)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyStrings[Calypso_ATWLAN_PolicyID_P2P], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyP2PStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyP2PValue_Strings[value], ATCOMMAND_STRING_TERMINATE))
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
 * @brief Get WLAN P2P policy.
 *
 * @param[out] policy P2P policy
 * @param[out] value Option value
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATWLAN_GetP2PPolicy(Calypso_ATWLAN_PolicyP2P_t* policy, Calypso_ATWLAN_PolicyP2PValue_t* value)
{
    *policy = Calypso_ATWLAN_PolicyP2P_Negotiate;
    *value = Calypso_ATWLAN_PolicyP2PValue_Active;

    char* pRespondCommand = NULL;
    if (!Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_P2P, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(Calypso_ATWLAN_PolicyP2PStrings, Calypso_ATWLAN_PolicyP2P_NumberOfValues, temp, Calypso_ATWLAN_PolicyP2P_Negotiate, &ok);
    if (!ok)
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_STRING_TERMINATE, sizeof(temp)))
    {
        return false;
    }

    *value = ATCommand_FindString(Calypso_ATWLAN_PolicyP2PValue_Strings, Calypso_ATWLAN_PolicyP2PValue_NumberOfValues, temp, Calypso_ATWLAN_PolicyP2PValue_Active, &ok);
    return ok;
}

/**
 * @brief Sends the AT+wlanPolicyGet command and waits for the response.
 *
 * Used by Calypso_ATWLAN_GetConnectionPolicy(), Calypso_ATWLAN_GetScanPolicy(),
 * Calypso_ATWLAN_GetPMPolicy(), Calypso_ATWLAN_GetP2PPolicy().
 *
 * @param[in] id ID of policy to get
 * @param[out] pRespondCommand Pointer to response arguments
 *
 * @return true if ID and option are valid, false otherwise
 */
bool Calypso_ATWLAN_SendPolicyGet(Calypso_ATWLAN_PolicyID_t id, char** pRespondCommand)
{
    if (id >= Calypso_ATWLAN_PolicyID_NumberOfValues)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicyGet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATWLAN_PolicyStrings[id], ATCOMMAND_STRING_TERMINATE))
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, *pRespondCommand))
    {
        return false;
    }

    const char* expectedCmd = "+wlanpolicyget:";
    const size_t cmdLength = strlen(expectedCmd);

    /* Check if response is for wlanPolicyGet */
    if (0 != strncmp(*pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pRespondCommand += cmdLength;

    return true;
}

/**
 * @brief Checks if the ID and option are valid for the AT+wlanGet command.
 *
 * @param[in] id ID of the command
 * @param[in] option Option of the command
 *
 * @return true if ID and option are valid, false otherwise
 */
static bool Calypso_ATWLAN_IsInputValidWlanGet(Calypso_ATWLAN_SetID_t id, uint8_t option)
{
    if (id >= Calypso_ATWLAN_SetID_NumberOfValues)
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATWLAN_SetID_General:
            /* Only countrycode, sta_tx_power, ap_tx_power and scan_params are valid */
            return (option == Calypso_ATWLAN_SetGeneral_CountryCode) || (option == Calypso_ATWLAN_SetGeneral_StationTxPower) || (option == Calypso_ATWLAN_SetGeneral_AccessPointTxPower) || (option == Calypso_ATWLAN_SetGeneral_ScanParams);

        case Calypso_ATWLAN_SetID_P2P:
            /* All options valid */
            return (option < Calypso_ATWLAN_SetP2P_NumberOfValues);

        case Calypso_ATWLAN_SetID_Connection:
            /* No options (parameter is ignored) */
            return true;

        case Calypso_ATWLAN_SetID_AccessPoint:
            /* All options valid */
            return (option < Calypso_ATWLAN_SetAP_NumberOfValues);

        default:
            return false;
    }

    return false;
}

/**
 * @brief Checks if the ID and option are valid for the AT+wlanSet command.
 *
 * @param[in] id ID of the command
 * @param[in] option Option of the command
 *
 * @return true if ID and option are valid, false otherwise
 */
static bool Calypso_ATWLAN_IsInputValidWlanSet(Calypso_ATWLAN_SetID_t id, uint8_t option)
{
    if (id < Calypso_ATWLAN_SetID_NumberOfValues)
    {
        switch (id)
        {
            case Calypso_ATWLAN_SetID_General:
                /* All options valid */
                return (option < Calypso_ATWLAN_SetGeneral_NumberOfValues);

            case Calypso_ATWLAN_SetID_P2P:
                /* All options valid */
                return (option < Calypso_ATWLAN_SetP2P_NumberOfValues);

            case Calypso_ATWLAN_SetID_AccessPoint:
                /* All options except MAX_STA_AGING are valid */
                return (option < Calypso_ATWLAN_SetAP_MaxStaAging);

            default:
                return false;
        }
    }
    return false;
}

/**
 * @brief Adds arguments to the connection command string.
 *
 * @param[out] pOutString The AT command string to add the arguments to
 * @param[in] connectionArgs The connection parameters
 * @param[in] lastDelim The delimiter to add after the last argument.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_AddConnectionArguments(char* pOutString, Calypso_ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim)
{

    if (!ATCommand_AppendArgumentString(pOutString, connectionArgs.SSID, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pOutString, connectionArgs.BSSID, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pOutString, Calypso_ATWLAN_SecurityTypeStrings[connectionArgs.securityParams.securityType], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pOutString, connectionArgs.securityParams.securityKey, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pOutString, connectionArgs.securityExtParams.extUser, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pOutString, connectionArgs.securityExtParams.extAnonUser, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pOutString, Calypso_ATWLAN_SecurityEAPStrings[connectionArgs.securityExtParams.eapMethod], lastDelim);
}

/**
 * @brief Adds arguments to the AT+wlanGet command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] id ID of the value to get
 * @param[in] option Option of the value to get
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_AddArgumentsWlanGet(char* pAtCommand, Calypso_ATWLAN_SetID_t id, uint8_t option)
{

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetIDStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATWLAN_SetID_General:
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetGeneralStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case Calypso_ATWLAN_SetID_P2P:
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetP2PStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case Calypso_ATWLAN_SetID_Connection:
            if (!ATCommand_AppendArgumentString(pAtCommand, "", ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case Calypso_ATWLAN_SetID_AccessPoint:
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetAPStrings[option], ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+wlanSet command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] id ID of the value to set
 * @param[in] option Option of the value to set
 * @param[in] pValues Values to set
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_AddArgumentsWlanSet(char* pAtCommand, Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues)
{

    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetIDStrings[id], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (id)
    {
        case Calypso_ATWLAN_SetID_General:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetGeneralStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case Calypso_ATWLAN_SetGeneral_CountryCode:
                    if (!ATCommand_AppendArgumentString(pAtCommand, pValues->general.countryCode, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_StationTxPower:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.staTxPower, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_AccessPointTxPower:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.apTxPower, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_ScanParams:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.scanParams.channelMask, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.scanParams.rssiTreshold, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_SuspendProfiles:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.suspendProfiles, (ATCOMMAND_INTFLAGS_NOTATION_HEX | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_DisableEntServerAuth:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->general.disableEntServerAuth, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

        case Calypso_ATWLAN_SetID_P2P:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetP2PStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (Calypso_ATWLAN_SetP2P_ChannelNRegs != option)
            {
                return false;
            }

            Calypso_ATWLAN_P2PChannelNRegs_t* pChannelNRegs = &pValues->p2p.p2pChannelNRegs;

            if (!ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->listenChannel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->listenRegulatoryClass, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->operatingChannel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->operatingRegulatoryClass, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }

        case Calypso_ATWLAN_SetID_AccessPoint:
        {
            if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_SetGetAPStrings[option], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
                case Calypso_ATWLAN_SetAP_SSID:
                    if (Calypso_firmwareVersionMajor > 1 || (Calypso_firmwareVersionMajor == 1 && Calypso_firmwareVersionMinor >= 9))
                    {
                        /* Starting with firmware version 1.9.0, there is an appendMac argument */
                        if (!ATCommand_AppendArgumentString(pAtCommand, pValues->ap.ssidConfig.appendMac ? "true" : "false", ATCOMMAND_ARGUMENT_DELIM))
                        {
                            return false;
                        }
                    }
                    if (!ATCommand_AppendArgumentString(pAtCommand, pValues->ap.ssidConfig.ssid, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_Channel:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_HiddenSSID:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.hiddenSSID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_Security:
                    if (!ATCommand_AppendArgumentString(pAtCommand, Calypso_ATWLAN_APSecurityTypeStrings[pValues->ap.security], ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_Password:
                    if (!ATCommand_AppendArgumentString(pAtCommand, pValues->ap.password, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_MaxStations:
                    if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.maxStations, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

/**
 * @brief Parses the response of a AT+wlanScan command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+wlanScan command.
 * @param[out] pOutScanEntry The parsed response. See Calypso_ATWLAN_ScanEntry_t
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_ParseResponseWlanScanEntry(char** pAtCommand, Calypso_ATWLAN_ScanEntry_t* pOutScanEntry)
{
    bool ret;
    if ((NULL == pAtCommand) || (NULL == pOutScanEntry))
    {
        return false;
    }

    const char* expectedCmd = "+wlanscan:";
    const size_t cmdLength = strlen(expectedCmd);
    char tempString[32];

    /* Check if response is for wlanscan */
    if (0 != strncmp(*pAtCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutScanEntry->SSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutScanEntry->SSID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutScanEntry->BSSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutScanEntry->BSSID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->RSSI), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
    {
        return false;
    }

    pOutScanEntry->securityType = ATCommand_FindString(Calypso_ATWLAN_ScanSecurityTypeStrings, Calypso_ATWLAN_ScanSecurityType_NumberOfValues, tempString, Calypso_ATWLAN_ScanSecurityType_Open, &ret);
    if (!ret)
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->hiddenSsidEnabled), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
    {
        return false;
    }

    pOutScanEntry->cipher = ATCommand_FindString(Calypso_ATWLAN_ScanCipherStrings, Calypso_ATWLAN_ScanCipherType_NumberOfValues, tempString, Calypso_ATWLAN_ScanCipherType_None, &ret);
    if (!ret)
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
    {
        return false;
    }
    pOutScanEntry->keyManagementMethod = ATCommand_FindString(Calypso_ATWLAN_ScanKeyMgmntStrings, Calypso_ATWLAN_ScanKeyManagementType_NumberOfValues, tempString, Calypso_ATWLAN_ScanKeyManagementType_None, &ret);
    return ret;
}

/**
 * @brief Parses the response of a AT+wlanProfileAdd command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+wlanProfileAdd command.
 * @param[out] pOutIndex The index of the added profile.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_ParseResponseWlanAddProfile(char** pAtCommand, uint8_t* pOutIndex)
{
    const char* cmd = "+wlanprofileadd:";
    const size_t cmdLength = strlen(cmd);

    /* Check if response is for add profile */
    if (0 != strncmp(*pAtCommand, cmd, cmdLength))
    {
        return false;
    }
    *pAtCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(pAtCommand, pOutIndex, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response of a AT+wlanProfileGet command
 *
 * @param[in,out] pAtCommand The string received in response to the AT+wlanProfileGet command.
 * @param[out] pOutProfile The returned profile.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_ParseResponseWlanGetProfile(char** pAtCommand, Calypso_ATWLAN_Profile_t* pOutProfile)
{

    const char* expectedCmd = "+wlanprofileget:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[32];

    /* Check if response is for get profile */
    if (0 != strncmp(*pAtCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;
    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.SSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.SSID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.BSSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.BSSID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
    {
        return false;
    }

    if (tempString[0] == '\0')
    {
        pOutProfile->connection.securityParams.securityType = Calypso_ATWLAN_SecurityType_Open;
    }
    else
    {
        bool ret;
        pOutProfile->connection.securityParams.securityType = ATCommand_FindString(Calypso_ATWLAN_SecurityTypeStrings, Calypso_ATWLAN_SecurityType_NumberOfValues, tempString, Calypso_ATWLAN_SecurityType_Open, &ret);
        if (!ret)
        {
            return false;
        }
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityParams.securityKey, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityParams.securityKey)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityExtParams.extUser, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityExtParams.extUser)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityExtParams.extAnonUser, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityExtParams.extAnonUser)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString)))
    {
        return false;
    }
    if (tempString[0] == '\0')
    {
        pOutProfile->connection.securityExtParams.eapMethod = Calypso_ATWLAN_SecurityEAP_None;
    }
    else
    {
        bool ret;
        pOutProfile->connection.securityExtParams.eapMethod = ATCommand_FindString(Calypso_ATWLAN_SecurityEAPStrings, Calypso_ATWLAN_SecurityEAP_NumberOfValues, tempString, Calypso_ATWLAN_SecurityEAP_None, &ret);
        if (!ret)
        {
            return false;
        }
    }

    return ATCommand_GetNextArgumentInt(pAtCommand, &(pOutProfile->priority), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response of a AT+wlanGet command.
 *
 * @param[in] id ID of the value to get
 * @param[in] option Option of the value to get
 * @param[in,out] pAtCommand The string received in response to the AT+wlanGet command.
 * @param[out] pValues The parsed response. Depends on the id/option.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATWLAN_ParseResponseWlanGet(Calypso_ATWLAN_SetID_t id, uint8_t option, char** pAtCommand, Calypso_ATWLAN_Settings_t* pValues)
{

    const char* expectedCmd = "+wlanget:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[32];

    /* Check if response is for wlanget */
    if (0 != strncmp(*pAtCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;
    switch (id)
    {
        case Calypso_ATWLAN_SetID_General:
        {
            Calypso_ATWLAN_SettingsGeneral_t* general = &pValues->general;

            switch (option)
            {
                case Calypso_ATWLAN_SetGeneral_CountryCode:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, general->countryCode, ATCOMMAND_STRING_TERMINATE, sizeof(general->countryCode)))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_StationTxPower:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &general->staTxPower, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_AccessPointTxPower:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &general->apTxPower, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetGeneral_ScanParams:
                {
                    Calypso_ATWLAN_ScanParams_t* scanParams = &general->scanParams;
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &(scanParams->channelMask), (ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }

                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &(scanParams->rssiTreshold), (ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;
                }

                default:
                    return false;
            }

            break;
        }

        case Calypso_ATWLAN_SetID_P2P:
        {
            if (Calypso_ATWLAN_SetP2P_ChannelNRegs != option)
            {
                return false;
            }
            Calypso_ATWLAN_P2PChannelNRegs_t* channelNRegs = &pValues->p2p.p2pChannelNRegs;

            if (!ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->listenChannel), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->listenRegulatoryClass), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->operatingChannel), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->operatingRegulatoryClass), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }

        case Calypso_ATWLAN_SetID_Connection:
        {
            bool ret;
            char temp[32];
            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.role = ATCommand_FindString(Calypso_ATWLAN_SetModeStrings, Calypso_ATWLAN_SetMode_NumberOfValues, temp, Calypso_ATWLAN_SetMode_Station, &ret);
            if (!ret)
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.status = ATCommand_FindString(Calypso_ATWLAN_StatusStrings, Calypso_ATWLAN_Status_NumberOfValues, temp, Calypso_ATWLAN_Status_Disconnected, &ret);
            if (!ret)
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.security = ATCommand_FindString(Calypso_ATWLAN_SecurityStatusStrings, Calypso_ATWLAN_SecurityStatus_NumberOfValues, temp, Calypso_ATWLAN_SecurityStatus_OPEN, &ret);
            if (!ret)
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->connection.SSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pValues->connection.SSID)))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->connection.BSSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pValues->connection.BSSID)))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->connection.p2pDeviceName, ATCOMMAND_ARGUMENT_DELIM, sizeof(pValues->connection.p2pDeviceName)))
            {
                return false;
            }
            break;
        }

        case Calypso_ATWLAN_SetID_AccessPoint:
        {
            switch (option)
            {
                case Calypso_ATWLAN_SetAP_SSID:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->ap.ssidConfig.ssid, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->ap.ssidConfig.ssid)))
                    {
                        return false;
                    }
                    /* Getter doesn't return the appendMac parameter */
                    pValues->ap.ssidConfig.appendMac = false;
                    break;

                case Calypso_ATWLAN_SetAP_Channel:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.channel, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_HiddenSSID:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.hiddenSSID, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_Security:
                {
                    bool ret;
                    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }

                    pValues->ap.security = ATCommand_FindString(Calypso_ATWLAN_APSecurityTypeStrings, Calypso_ATWLAN_APSecurityType_NumberOfValues, tempString, Calypso_ATWLAN_APSecurityType_Open, &ret);
                    if (!ret)
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATWLAN_SetAP_Password:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->ap.password, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->ap.password)))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_MaxStations:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.maxStations, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATWLAN_SetAP_MaxStaAging:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.maxStaAgingSeconds, (ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                default:
                    return false;
            }

            break;
        }

        default:
            return false;
    }
    return true;
}
