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
 * @brief AT commands for WLAN functionality.
 */

#include <ATCommands.h>
#include "ATWLAN.h"

#include "Calypso.h"

static const char *ATWLAN_SetModeStrings[ATWLAN_SetMode_NumberOfValues] =
{
    "STA",
    "AP",
    "P2P",
};

static const char *ATWLAN_SetIDStrings[ATWLAN_SetID_NumberOfValues] =
{
    "general",
    "P2P",
    "Connection",
    "AP"
};

static const char *ATWLAN_SetGetGeneralStrings[ATWLAN_SetGeneral_NumberOfValues] =
{
    "COUNTRY_CODE",
    "STA_TX_POWER",
    "AP_TX_POWER",
    "SCAN_PARAMS",
    "SUSPEND_PROFILES",
    "DISABLE_ENT_SERVER_AUTH",
};

static const char *ATWLAN_SetGetP2PStrings[ATWLAN_SetP2P_NumberOfValues] =
{
    "CHANNEL_N_REGS"
};

static const char *ATWLAN_SetGetAPStrings[ATWLAN_SetAP_NumberOfValues] =
{
    "SSID",
    "CHANNEL",
    "HIDDEN_SSID",
    "SECURITY",
    "PASSWORD",
    "MAX_STATIONS",
    "MAX_STA_AGING"
};

static const char *ATWLAN_PolicyStrings[ATWLAN_PolicyID_NumberOfValues] =
{
    "connection",
    "scan",
    "PM",
    "P2P"
};

static const char *ATWLAN_PolicyConnectionStrings[ATWLAN_PolicyConnection_NumberOfValues] =
{
    "auto",
    "fast",
    "P2P"
};

static const char *ATWLAN_PolicyScanStrings[ATWLAN_PolicyScan_NumberOfValues] =
{
    "Hidden_SSID",
    "No_Hidden_SSID",
    "Disable_Scan"
};

static const char *ATWLAN_PolicyPMStrings[ATWLAN_PolicyPM_NumberOfValues] =
{
    "normal",
    "low_latency",
    "low_power",
    "long_sleep"
};

static const char *ATWLAN_PolicyP2PStrings[ATWLAN_PolicyP2P_NumberOfValues] =
{
    "CLIENT",
    "GROUP_OWNER",
    "NEGOTIATE"
};

static const char *ATWLAN_PolicyP2PValue_Strings[ATWLAN_PolicyP2PValue_NumberOfValues] =
{
    "ACTIVE",
    "PASSIVE",
    "RAND_BACKOFF"
};

static const char *ATWLAN_SecurityTypeStrings[ATWLAN_SecurityType_NumberOfValues] =
{
    "OPEN",
    "WEP",
    "WEP_SHARED",
    "WPA_WPA2",
    "WPA2_PLUS",
    "WPA3",
    "WPA_ENT",
    "WPS_PBC",
    "WPS_PIN"
};

static const char *ATWLAN_APSecurityTypeStrings[ATWLAN_APSecurityType_NumberOfValues] =
{
    "OPEN",
    "WEP",
    "WPA_WPA2"
};

static const char *ATWLAN_ScanSecurityTypeStrings[ATWLAN_ScanSecurityType_NumberOfValues] =
{
    "OPEN",
    "WEP",
    "WPA",
    "WPA2",
    "WPA_WPA2"
};

static const char *ATWLAN_ScanCipherStrings[ATWLAN_ScanCipherType_NumberOfValues] =
{
    "NONE",
    "WEP40",
    "WEP104",
    "TKIP",
    "CCMP"
};

static const char *ATWLAN_ScanKeyMgmntStrings[ATWLAN_ScanKeyManagementType_NumberOfValues] =
{
    "NONE",
    "802_1_x",
    "PSK"
};

static const char *ATWLAN_SecurityEAPStrings[ATWLAN_SecurityEAP_NumberOfValues] =
{
    "",
    "TLS",
    "TTLS_TLS",
    "TTLS_MSCHAPv2",
    "TTLS_PSK",
    "PEAP0_TLS",
    "PEAP0_MSCHAPv2",
    "PEAP0_PSK",
    "PEAP1_TLS",
    "PEAP1_PSK"
};

static const char *ATWLAN_StatusStrings[ATWLAN_Status_NumberOfValues] =
{
    "disconnected",
    "station_connected",
    "p2pcl_connected",
    "p2pgo_connected",
    "ap_connected_stations",
};

static const char *ATWLAN_SecurityStatusStrings[ATWLAN_SecurityStatus_NumberOfValues] =
{
    "open",
    "wep",
    "wpa_wpa2",
    "wps_pbc",
    "wps_pin",
    "wpa_ent",
    "wep_shared"
};

static bool ATWLAN_IsInputValidWlanGet(ATWLAN_SetID_t id, uint8_t option);
static bool ATWLAN_IsInputValidWlanSet(ATWLAN_SetID_t id, uint8_t option);

static bool ATWLAN_AddConnectionArguments(char *pOutString, ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim);
static bool ATWLAN_AddArgumentsWlanGet(char *pAtCommand, ATWLAN_SetID_t id, uint8_t option);
static bool ATWLAN_AddArgumentsWlanSet(char *pAtCommand, ATWLAN_SetID_t id, uint8_t option, ATWLAN_Settings_t *pValues);


static bool ATWLAN_ParseResponseWlanScanEntry(char **pAtCommand, ATWLAN_ScanEntry_t *pOutScanEntry);
static bool ATWLAN_ParseResponseWlanAddProfile(char **pAtCommand, uint8_t *pOutIndex);
static bool ATWLAN_ParseResponseWlanGetProfile(char **pAtCommand, ATWLAN_Profile_t *pOutProfile);
static bool ATWLAN_ParseResponseWlanGet(ATWLAN_SetID_t id, uint8_t option, char **pAtCommand, ATWLAN_Settings_t *pValues);

static bool ATWLAN_SendPolicyGet(ATWLAN_PolicyID_t id, char **pRespondCommand);


/**
 * @brief Sets the wireless LAN mode (using the AT+wlanSetMode command).
 *
 * @param[in] mode WLAN operating mode to be set.
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_SetMode(ATWLAN_SetMode_t mode)
{
    bool ret = false;

    if (mode < ATWLAN_SetMode_NumberOfValues)
    {
        char *pRequestCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+wlanSetMode=");

        ret = ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_SetModeStrings[mode], ATCOMMAND_STRING_TERMINATE);

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
    }

    return ret;
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
bool ATWLAN_Scan(uint8_t index, uint8_t deviceCount, ATWLAN_ScanEntry_t *pOutValues, uint8_t *pOutNumEntries)
{
    bool ret = false;

    if ((index < 30) && (deviceCount < 30))
    {
        char *pRequestCommand = AT_commandBuffer;
        char *pRespondCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+wlanScan=");

        ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM);

        if (ret)
        {
            ret = ATCommand_AppendArgumentInt(pRequestCommand, deviceCount, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);
        }

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
            ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_WlanScan), Calypso_CNFStatus_Success, pRespondCommand);
        }

        if (ret)
        {
            uint8_t numEntries = 0;
            for (int i = 0; i < deviceCount; i++)
            {
                if (ATWLAN_ParseResponseWlanScanEntry(&pRespondCommand, &pOutValues[numEntries]))
                {
                    numEntries++;
                }
            }
            *pOutNumEntries = numEntries;
        }
    }

    return ret;
}

/**
 * @brief Connects to a wireless network (using the AT+wlanConnect command).
 *
 * @param[in] connectArgs Connection parameters
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_Connect(ATWLAN_ConnectionArguments_t connectArgs)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanConnect=");

    ret = ATWLAN_AddConnectionArguments(pRequestCommand, connectArgs, ATCOMMAND_STRING_TERMINATE);

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
 * @brief Disconnects from a wireless network (using the AT+disconnect command).
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_Disconnect()
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
 * @param[in] connectArgs Connection parameters
 * @param[in] priority Priority of the profile to be added (0-15)
 * @param[out] pOutIndex The index of the added profile. Can be used to access the profile.
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_AddProfile(ATWLAN_Profile_t profile, uint8_t *pOutIndex)
{
    bool ret = false;

    if ((pOutIndex != NULL) && (profile.priority < 16))
    {
        char *pRequestCommand = AT_commandBuffer;
        char *pRespondCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+wlanProfileAdd=");

        ret = ATWLAN_AddConnectionArguments(pRequestCommand, profile.connection, ATCOMMAND_ARGUMENT_DELIM);

        if (ret)
        {
            ret = ATCommand_AppendArgumentInt(pRequestCommand, profile.priority, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
        }

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
            ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_WlanAddProfile), Calypso_CNFStatus_Success, pRespondCommand);
        }

        if (ret)
        {
            ret = ATWLAN_ParseResponseWlanAddProfile(&pRespondCommand, pOutIndex);
        }
    }

    return ret;
}

/**
 * @brief Gets a wireless LAN profile (using the AT+wlanProfileGet command).
 *
 * @param[in] index Index of the profile as returned by ATWLAN_AddProfile().
 * @param[out] pOutProfile The returned WLAN profile.
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_GetProfile(uint8_t index, ATWLAN_Profile_t *pOutProfile)
{
    bool ret = false;

    if (NULL == pOutProfile)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanProfileGet=");


    ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);

    if (ret)
    {
        ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

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
        ret = ATWLAN_ParseResponseWlanGetProfile(&pRespondCommand, pOutProfile);
    }

    return ret;
}

/**
 * @brief Deletes a wireless LAN profile (using the AT+wlanProfileDel command).
 *
 * @param[in] index Index of the profile to be deleted (as returned by ATWLAN_AddProfile()).
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_DeleteProfile(uint8_t index)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanProfileDel=");

    ret = ATCommand_AppendArgumentInt(pRequestCommand, index, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);

    if (ret)
    {
        ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
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
 * @briefs Reads wireless LAN settings (using the AT+wlanGet command).
 *
 * @param[in] id ID of the value to get
 * @param[in] option Option of the value to get
 * @param[out] pValues Values returned by the module
 * @param[out] pValuesSize Length (in bytes) of the returned values
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_Get(ATWLAN_SetID_t id,
                uint8_t option,
                ATWLAN_Settings_t *pValues)
{
    bool ret = false;

    memset(pValues, 0, sizeof(*pValues));

    if (ATWLAN_IsInputValidWlanGet(id, option))
    {
        char *pRequestCommand = AT_commandBuffer;
        char *pRespondCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+wlanGet=");

        ret = ATWLAN_AddArgumentsWlanGet(pRequestCommand, id, option);

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
            ret = ATWLAN_ParseResponseWlanGet(id, option, &pRespondCommand, pValues);
        }
    }

    return ret;
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
bool ATWLAN_Set(ATWLAN_SetID_t id, uint8_t option, ATWLAN_Settings_t *pValues)
{
    bool ret = false;

    if (ATWLAN_IsInputValidWlanSet(id, option))
    {
        char *pRequestCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+wlanSet=");

        ret = ATWLAN_AddArgumentsWlanSet(pRequestCommand, id, option, pValues);

        if (ret)
        {
            if (!Calypso_SendRequest(pRequestCommand))
            {
                return false;
            }
            ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
        }
    }

    return ret;
}

/**
 * @brief Set WLAN connection policy.
 *
 * Note that setting a connection policy while the device parameter
 * ATDevice_GetGeneral_Persistent is set to false (0) will return an error (code -31008).
 *
 * @param[in] policy Connection policy flags. See ATWLAN_PolicyConnection_t.
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_SetConnectionPolicy(uint8_t policy)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyStrings[ATWLAN_PolicyID_Connection], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBitmask(pRequestCommand,
                                       ATWLAN_PolicyConnectionStrings,
                                       ATWLAN_PolicyConnection_NumberOfValues,
                                       policy,
                                       ATCOMMAND_ARGUMENT_DELIM,
                                       AT_MAX_COMMAND_BUFFER_SIZE))
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
 * @param[out] policy Connection policy flags. See ATWLAN_PolicyConnection_t.
 *
 * @return true if successful, false otherwise
 */
bool ATWLAN_GetConnectionPolicy(uint8_t *policy)
{
    *policy = 0;

    char *pRespondCommand = NULL;
    if (!ATWLAN_SendPolicyGet(ATWLAN_PolicyID_Connection, &pRespondCommand))
    {
        return false;
    }

    uint32_t bitmask;
    if (ATCommand_GetNextArgumentBitmask(&pRespondCommand,
                                       ATWLAN_PolicyConnectionStrings,
                                       ATWLAN_PolicyConnection_NumberOfValues,
                                       5,
                                       &bitmask,
                                       ATCOMMAND_STRING_TERMINATE))
    {
        *policy = (uint8_t) bitmask;
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
bool ATWLAN_SetScanPolicy(ATWLAN_PolicyScan_t policy, uint32_t scanIntervalSeconds)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyStrings[ATWLAN_PolicyID_Scan], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyScanStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
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
bool ATWLAN_GetScanPolicy(ATWLAN_PolicyScan_t *policy, uint32_t *scanIntervalSeconds)
{
    *policy = ATWLAN_PolicyScan_DisableScan;
    *scanIntervalSeconds = 0;

    char *pRespondCommand = NULL;
    if (!ATWLAN_SendPolicyGet(ATWLAN_PolicyID_Scan, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(ATWLAN_PolicyScanStrings, ATWLAN_PolicyScan_NumberOfValues, temp, ATWLAN_PolicyScan_DisableScan, &ok);
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
bool ATWLAN_SetPMPolicy(ATWLAN_PolicyPM_t policy, uint32_t maxSleepTimeMs)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyStrings[ATWLAN_PolicyID_PowerManagement], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyPMStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
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
bool ATWLAN_GetPMPolicy(ATWLAN_PolicyPM_t *policy, uint32_t *maxSleepTimeMs)
{
    *policy = ATWLAN_PolicyPM_Normal;
    *maxSleepTimeMs = 0;

    char *pRespondCommand = NULL;
    if (!ATWLAN_SendPolicyGet(ATWLAN_PolicyID_PowerManagement, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(ATWLAN_PolicyPMStrings, ATWLAN_PolicyPM_NumberOfValues, temp, ATWLAN_PolicyPM_Normal, &ok);
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
bool ATWLAN_SetP2PPolicy(ATWLAN_PolicyP2P_t policy, ATWLAN_PolicyP2PValue_t value)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicySet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyStrings[ATWLAN_PolicyID_P2P], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyP2PStrings[policy], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyP2PValue_Strings[value], ATCOMMAND_STRING_TERMINATE))
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
bool ATWLAN_GetP2PPolicy(ATWLAN_PolicyP2P_t *policy, ATWLAN_PolicyP2PValue_t *value)
{
    *policy = ATWLAN_PolicyP2P_Negotiate;
    *value = ATWLAN_PolicyP2PValue_Active;

    char *pRespondCommand = NULL;
    if (!ATWLAN_SendPolicyGet(ATWLAN_PolicyID_P2P, &pRespondCommand))
    {
        return false;
    }

    char temp[15];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    bool ok;
    *policy = ATCommand_FindString(ATWLAN_PolicyP2PStrings, ATWLAN_PolicyP2P_NumberOfValues, temp, ATWLAN_PolicyP2P_Negotiate, &ok);
    if (!ok)
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_STRING_TERMINATE, sizeof(temp)))
    {
        return false;
    }

    *value = ATCommand_FindString(ATWLAN_PolicyP2PValue_Strings, ATWLAN_PolicyP2PValue_NumberOfValues, temp, ATWLAN_PolicyP2PValue_Active, &ok);
    return ok;
}

/**
 * @brief Sends the AT+wlanPolicyGet command and waits for the response.
 *
 * Used by ATWLAN_GetConnectionPolicy(), ATWLAN_GetScanPolicy(),
 * ATWLAN_GetPMPolicy(), ATWLAN_GetP2PPolicy().
 *
 * @param[in] id ID of policy to get
 * @param[out] pRespondCommand Pointer to response arguments
 *
 * @return true if ID and option are valid, false otherwise
 */
bool ATWLAN_SendPolicyGet(ATWLAN_PolicyID_t id, char **pRespondCommand)
{
    if (id >= ATWLAN_PolicyID_NumberOfValues)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+wlanPolicyGet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATWLAN_PolicyStrings[id], ATCOMMAND_STRING_TERMINATE))
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

    const char *expectedCmd = "+wlanpolicyget:";
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
static bool ATWLAN_IsInputValidWlanGet(ATWLAN_SetID_t id, uint8_t option)
{
    if (id >= ATWLAN_SetID_NumberOfValues)
    {
        return false;
    }

    switch (id)
    {
    case ATWLAN_SetID_General:
        /* Only countrycode, sta_tx_power, ap_tx_power and scan_params are valid */
        return (option == ATWLAN_SetGeneral_CountryCode) ||
                (option == ATWLAN_SetGeneral_StationTxPower) ||
                (option == ATWLAN_SetGeneral_AccessPointTxPower) ||
                (option == ATWLAN_SetGeneral_ScanParams);

    case ATWLAN_SetID_P2P:
        /* All options valid */
        return (option < ATWLAN_SetP2P_NumberOfValues);

    case ATWLAN_SetID_Connection:
        /* No options (parameter is ignored) */
        return true;

    case ATWLAN_SetID_AccessPoint:
        /* All options valid */
        return (option < ATWLAN_SetAP_NumberOfValues);

    default:
        break;
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
static bool ATWLAN_IsInputValidWlanSet(ATWLAN_SetID_t id, uint8_t option)
{
    if (id < ATWLAN_SetID_NumberOfValues)
    {
        switch (id)
        {
        case ATWLAN_SetID_General:
            /* All options valid */
            return (option < ATWLAN_SetGeneral_NumberOfValues);

        case ATWLAN_SetID_P2P:
            /* All options valid */
            return (option < ATWLAN_SetP2P_NumberOfValues);

        case ATWLAN_SetID_AccessPoint:
            /* All options except MAX_STA_AGING are valid */
            return (option < ATWLAN_SetAP_MaxStaAging);

        default:
            break;
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
static bool ATWLAN_AddConnectionArguments(char *pOutString, ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentString(pOutString, connectionArgs.SSID, ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, connectionArgs.BSSID, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, ATWLAN_SecurityTypeStrings[connectionArgs.securityParams.securityType], ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, connectionArgs.securityParams.securityKey, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, connectionArgs.securityExtParams.extUser, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, connectionArgs.securityExtParams.extAnonUser, ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = ATCommand_AppendArgumentString(pOutString, ATWLAN_SecurityEAPStrings[connectionArgs.securityExtParams.eapMethod], lastDelim);
    }

    return ret;
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
static bool ATWLAN_AddArgumentsWlanGet(char *pAtCommand, ATWLAN_SetID_t id, uint8_t option)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetIDStrings[id], ATCOMMAND_ARGUMENT_DELIM);

    if (ret)
    {
        switch (id)
        {
        case ATWLAN_SetID_General:
            ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetGeneralStrings[option], ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetID_P2P:
            ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetP2PStrings[option], ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetID_Connection:
            ret = ATCommand_AppendArgumentString(pAtCommand, "", ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetID_AccessPoint:
            ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetAPStrings[option], ATCOMMAND_STRING_TERMINATE);
            break;

        default:
            break;
        }
    }

    if (ret)
    {
        ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
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
static bool ATWLAN_AddArgumentsWlanSet(char *pAtCommand, ATWLAN_SetID_t id, uint8_t option, ATWLAN_Settings_t *pValues)
{
    bool ret = false;

    ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetIDStrings[id], ATCOMMAND_ARGUMENT_DELIM);

    if (!ret)
    {
        return false;
    }

    switch (id)
    {
    case ATWLAN_SetID_General:
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetGeneralStrings[option], ATCOMMAND_ARGUMENT_DELIM);

        switch (option)
        {
        case ATWLAN_SetGeneral_CountryCode:
            ret = ATCommand_AppendArgumentString(pAtCommand, pValues->general.countryCode, ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetGeneral_StationTxPower:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.staTxPower, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetGeneral_AccessPointTxPower:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.apTxPower, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetGeneral_ScanParams:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.scanParams.channelMask, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_ARGUMENT_DELIM);
            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.scanParams.rssiTreshold, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_STRING_TERMINATE);
            }
            break;

        case ATWLAN_SetGeneral_SuspendProfiles:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.suspendProfiles, (ATCOMMAND_INTFLAGS_NOTATION_HEX | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetGeneral_DisableEntServerAuth:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->general.disableEntServerAuth, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE);
            break;

        default:
        {
            ret = false;
            break;
        }
        }

        break;
    }

    case ATWLAN_SetID_P2P:
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetP2PStrings[option], ATCOMMAND_ARGUMENT_DELIM);

        if (ATWLAN_SetP2P_ChannelNRegs == option)
        {
            ATWLAN_P2PChannelNRegs_t *pChannelNRegs = &pValues->p2p.p2pChannelNRegs;

            ret = ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->listenChannel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->listenRegulatoryClass, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->operatingChannel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = ATCommand_AppendArgumentInt(pAtCommand, pChannelNRegs->operatingRegulatoryClass, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            }
        }
        else
        {
            ret = false;
        }
        break;
    }

    case ATWLAN_SetID_AccessPoint:
    {
        ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_SetGetAPStrings[option], ATCOMMAND_ARGUMENT_DELIM);

        switch (option)
        {
        case ATWLAN_SetAP_SSID:
            if (Calypso_firmwareVersionMajor > 1 || (Calypso_firmwareVersionMajor == 1 && Calypso_firmwareVersionMinor >= 9))
            {
                /* Starting with firmware version 1.9.0, there is an appendMac argument */
                if (!ATCommand_AppendArgumentString(pAtCommand, pValues->ap.ssidConfig.appendMac ? "true" : "false", ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
            }
            ret = ATCommand_AppendArgumentString(pAtCommand, pValues->ap.ssidConfig.ssid, ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetAP_Channel:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetAP_HiddenSSID:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.hiddenSSID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetAP_Security:
            ret = ATCommand_AppendArgumentString(pAtCommand, ATWLAN_APSecurityTypeStrings[pValues->ap.security], ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetAP_Password:
            ret = ATCommand_AppendArgumentString(pAtCommand, pValues->ap.password, ATCOMMAND_STRING_TERMINATE);
            break;

        case ATWLAN_SetAP_MaxStations:
            ret = ATCommand_AppendArgumentInt(pAtCommand, pValues->ap.maxStations, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
            break;

        default:
        {
            ret = false;
            break;
        }
        }

        break;
    }

    default:
    {
        break;
    }
    }

    if (ret)
    {
        ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF,ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the response of a AT+wlanScan command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+wlanScan command.
 * @param[out] pOutScanEntry The parsed response. See ATWLAN_ScanEntry_t
 *
 * @return true if successful, false otherwise
 */
static bool ATWLAN_ParseResponseWlanScanEntry(char **pAtCommand, ATWLAN_ScanEntry_t *pOutScanEntry)
{
    bool ret = false;

    if ((NULL == pAtCommand) || (NULL == pOutScanEntry))
    {
        return false;
    }

    const char *expectedCmd = "+wlanscan:";
    const size_t cmdLength = strlen(expectedCmd);
    char tempString[32];

    /* Check if response is for wlanscan */
    ret = (0 == strncmp(*pAtCommand, expectedCmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = ATCommand_GetNextArgumentString(pAtCommand, pOutScanEntry->SSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutScanEntry->SSID));

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, pOutScanEntry->BSSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutScanEntry->BSSID));
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->RSSI), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString));
            if (ret)
            {
                pOutScanEntry->securityType = ATCommand_FindString(ATWLAN_ScanSecurityTypeStrings,
                                                                 ATWLAN_ScanSecurityType_NumberOfValues,
                                                                 tempString,
                                                                 ATWLAN_ScanSecurityType_Open,
                                                                 &ret);
            }
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentInt(pAtCommand, &(pOutScanEntry->hiddenSsidEnabled), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString));
            if (ret)
            {
                pOutScanEntry->cipher = ATCommand_FindString(ATWLAN_ScanCipherStrings,
                                                           ATWLAN_ScanCipherType_NumberOfValues,
                                                           tempString,
                                                           ATWLAN_ScanCipherType_None,
                                                           &ret);
            }
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString));
            if (ret)
            {
                pOutScanEntry->keyManagementMethod = ATCommand_FindString(ATWLAN_ScanKeyMgmntStrings,
                                                                        ATWLAN_ScanKeyManagementType_NumberOfValues,
                                                                        tempString,
                                                                        ATWLAN_ScanKeyManagementType_None,
                                                                        &ret);
            }
        }
    }

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
static bool ATWLAN_ParseResponseWlanAddProfile(char **pAtCommand, uint8_t *pOutIndex)
{
    bool ret = false;
    const char *cmd = "+wlanprofileadd:";
    const size_t cmdLength = strlen(cmd);

    /* Check if response is for add profile */
    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = ATCommand_GetNextArgumentInt(pAtCommand, pOutIndex, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the response of a AT+wlanProfileGet command
 *
 * @param[in,out] pAtCommand The string received in response to the AT+wlanProfileGet command.
 * @param[out] pOutProfile The returned profile.
 *
 * @return true if successful, false otherwise
 */
static bool ATWLAN_ParseResponseWlanGetProfile(char **pAtCommand, ATWLAN_Profile_t *pOutProfile)
{
    bool ret = false;

    const char *expectedCmd = "+wlanprofileget:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[32];

    /* Check if response is for get profile */
    ret = (0 == strncmp(*pAtCommand, expectedCmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.SSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.SSID));

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.BSSID, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.BSSID));
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString));
            if (ret)
            {
                if (tempString[0] == '\0')
                {
                    pOutProfile->connection.securityParams.securityType = ATWLAN_SecurityType_Open;
                }
                else
                {
                    pOutProfile->connection.securityParams.securityType = ATCommand_FindString(ATWLAN_SecurityTypeStrings,
                                                                                             ATWLAN_SecurityType_NumberOfValues,
                                                                                             tempString,
                                                                                             ATWLAN_SecurityType_Open,
                                                                                             &ret);
                }
            }
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityParams.securityKey, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityParams.securityKey));
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityExtParams.extUser, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityExtParams.extUser));
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, pOutProfile->connection.securityExtParams.extAnonUser, ATCOMMAND_ARGUMENT_DELIM, sizeof(pOutProfile->connection.securityExtParams.extAnonUser));
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_ARGUMENT_DELIM, sizeof(tempString));
            if (ret)
            {
                if (tempString[0] == '\0')
                {
                    pOutProfile->connection.securityExtParams.eapMethod = ATWLAN_SecurityEAP_None;
                }
                else
                {
                    pOutProfile->connection.securityExtParams.eapMethod = ATCommand_FindString(ATWLAN_SecurityEAPStrings,
                                                                                             ATWLAN_SecurityEAP_NumberOfValues,
                                                                                             tempString,
                                                                                             ATWLAN_SecurityEAP_None,
                                                                                             &ret);
                }
            }
        }

        if (ret)
        {
            ret = ATCommand_GetNextArgumentInt(pAtCommand, &(pOutProfile->priority), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
        }
    }

    return ret;
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
static bool ATWLAN_ParseResponseWlanGet(ATWLAN_SetID_t id,
                                        uint8_t option,
                                        char **pAtCommand,
                                        ATWLAN_Settings_t *pValues)
{
    bool ret = false;

    const char *expectedCmd = "+wlanget:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[32];

    /* Check if response is for wlanget */
    ret = (0 == strncmp(*pAtCommand, expectedCmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        switch (id)
        {
        case ATWLAN_SetID_General:
        {
            ATWLAN_SettingsGeneral_t *general = &pValues->general;

            switch (option)
            {
            case ATWLAN_SetGeneral_CountryCode:
                ret = ATCommand_GetNextArgumentString(pAtCommand, general->countryCode, ATCOMMAND_STRING_TERMINATE, sizeof(general->countryCode));
                break;

            case ATWLAN_SetGeneral_StationTxPower:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &general->staTxPower, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
                break;

            case ATWLAN_SetGeneral_AccessPointTxPower:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &general->apTxPower, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
                break;

            case ATWLAN_SetGeneral_ScanParams:
            {
                ATWLAN_ScanParams_t *scanParams = &general->scanParams;
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &(scanParams->channelMask), (ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_ARGUMENT_DELIM);

                if (ret)
                {
                    ret = ATCommand_GetNextArgumentInt(pAtCommand, &(scanParams->rssiTreshold), (ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX), ATCOMMAND_STRING_TERMINATE);
                }
                break;
            }

            default:
                ret = false;
            }

            break;
        }

        case ATWLAN_SetID_P2P:
        {
            if (ATWLAN_SetP2P_ChannelNRegs == option)
            {
                ATWLAN_P2PChannelNRegs_t *channelNRegs = &pValues->p2p.p2pChannelNRegs;

                ret = ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->listenChannel), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);

                if (ret)
                {
                    ret = ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->listenRegulatoryClass), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
                }

                if (ret)
                {
                    ret = ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->operatingChannel), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM);
                }

                if (ret)
                {
                    ret = ATCommand_GetNextArgumentInt(pAtCommand, &(channelNRegs->operatingRegulatoryClass), (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
                }

            }
            else
            {
                ret = false;
            }
            break;
        }

        case ATWLAN_SetID_Connection:
        {
            char temp[32];
            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.role = ATCommand_FindString(ATWLAN_SetModeStrings,
                                                          ATWLAN_SetMode_NumberOfValues,
                                                          temp,
                                                          ATWLAN_SetMode_Station,
                                                          &ret);
            if (!ret)
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.status = ATCommand_FindString(ATWLAN_StatusStrings,
                                                            ATWLAN_Status_NumberOfValues,
                                                            temp,
                                                            ATWLAN_Status_Disconnected,
                                                            &ret);
            if (!ret)
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentString(pAtCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
            {
                return false;
            }
            pValues->connection.security = ATCommand_FindString(ATWLAN_SecurityStatusStrings,
                                                              ATWLAN_SecurityStatus_NumberOfValues,
                                                              temp,
                                                              ATWLAN_SecurityStatus_OPEN,
                                                              &ret);
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

        case ATWLAN_SetID_AccessPoint:
        {
            switch (option)
            {
            case ATWLAN_SetAP_SSID:
                ret = ATCommand_GetNextArgumentString(pAtCommand, pValues->ap.ssidConfig.ssid, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->ap.ssidConfig.ssid));
                /* Getter doesn't return the appendMac parameter */
                pValues->ap.ssidConfig.appendMac = false;
                break;

            case ATWLAN_SetAP_Channel:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.channel, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
                break;

            case ATWLAN_SetAP_HiddenSSID:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.hiddenSSID, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
                break;

            case ATWLAN_SetAP_Security:
            {
                ret = ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString));
                if (ret)
                {
                    pValues->ap.security = ATCommand_FindString(ATWLAN_APSecurityTypeStrings,
                                                              ATWLAN_APSecurityType_NumberOfValues,
                                                              tempString,
                                                              ATWLAN_APSecurityType_Open,
                                                              &ret);
                }

                break;
            }

            case ATWLAN_SetAP_Password:
                ret = ATCommand_GetNextArgumentString(pAtCommand, pValues->ap.password, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->ap.password));
                break;

            case ATWLAN_SetAP_MaxStations:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.maxStations, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
                break;

            case ATWLAN_SetAP_MaxStaAging:
                ret = ATCommand_GetNextArgumentInt(pAtCommand, &pValues->ap.maxStaAgingSeconds, (ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE);
                break;

            default:
                ret = false;
                break;
            }

            break;
        }

        default:
            break;
        }
    }
    return ret;
}
