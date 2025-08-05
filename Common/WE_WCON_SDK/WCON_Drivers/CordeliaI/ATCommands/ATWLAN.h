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
 * @brief AT commands for WLAN functionality.
 */

#ifndef CORDELIAI_AT_WLAN_H_INCLUDED
#define CORDELIAI_AT_WLAN_H_INCLUDED
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CordeliaI_ATWLAN_BSSID_LENGTH 32       /**< BSSID length (MAC address of the wireless access point) */
#define CordeliaI_ATWLAN_SSID_MAX_LENGTH 32    /**< Max. SSID length (Wireless LAN identifier) */
#define CordeliaI_ATWLAN_SECURITYKEY_LENGTH 64 /**< Max. security key length */
#define CordeliaI_ATWLAN_AP_SECURITYKEY_LENGTH 64

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief IDs for CordeliaI_ATWLAN_Get() and CordeliaI_ATWLAN_Set()
 */
    typedef enum CordeliaI_ATWLAN_SetID_t
    {
        CordeliaI_ATWLAN_SetID_General,
        CordeliaI_ATWLAN_SetID_NumberOfValues
    } CordeliaI_ATWLAN_SetID_t;

    /**
 * @brief Option parameter for ID CordeliaI_ATWLAN_SetID_General in CordeliaI_ATWLAN_Set().
 */
    typedef enum CordeliaI_ATWLAN_SetGeneral_t
    {
        CordeliaI_ATWLAN_SetGeneral_CountryCode,
        CordeliaI_ATWLAN_SetGeneral_StationTxPower,
        CordeliaI_ATWLAN_SetGeneral_ScanParams,
        CordeliaI_ATWLAN_SetGeneral_SuspendProfiles,
        CordeliaI_ATWLAN_SetGeneral_DisableEntServerAuth,
        CordeliaI_ATWLAN_SetGeneral_NumberOfValues
    } CordeliaI_ATWLAN_SetGeneral_t;

    /**
 * @brief WLAN Policy IDs.
 *
 * @see CordeliaI_ATWLAN_SetConnectionPolicy(), CordeliaI_ATWLAN_GetConnectionPolicy(),
 * CordeliaI_ATWLAN_SetScanPolicy(), CordeliaI_ATWLAN_GetScanPolicy(),
 * CordeliaI_ATWLAN_SetPMPolicy(), CordeliaI_ATWLAN_GetPMPolicy(),
 * CordeliaI_ATWLAN_SetP2PPolicy(), CordeliaI_ATWLAN_GetP2PPolicy()
 */
    typedef enum CordeliaI_ATWLAN_PolicyID_t
    {
        CordeliaI_ATWLAN_PolicyID_Connection,
        CordeliaI_ATWLAN_PolicyID_Scan,
        CordeliaI_ATWLAN_PolicyID_PowerManagement,
        CordeliaI_ATWLAN_PolicyID_P2P,
        CordeliaI_ATWLAN_PolicyID_NumberOfValues
    } CordeliaI_ATWLAN_PolicyID_t;

    /**
 * @brief Option parameter (flags) for use with CordeliaI_ATWLAN_SetConnectionPolicy() and CordeliaI_ATWLAN_GetConnectionPolicy().
 *
 * Note that setting a connection policy while the device parameter
 * CordeliaI_ATDevice_GetGeneral_Persistent is set to false (0) will return an error (code -31008).
 */
    typedef enum CordeliaI_ATWLAN_PolicyConnection_t
    {
        CordeliaI_ATWLAN_PolicyConnection_None = 0,
        CordeliaI_ATWLAN_PolicyConnection_Auto = (1 << 0),
        CordeliaI_ATWLAN_PolicyConnection_Fast = (1 << 1),
        CordeliaI_ATWLAN_PolicyConnection_P2P = (1 << 2),
        CordeliaI_ATWLAN_PolicyConnection_NumberOfValues = 3
    } CordeliaI_ATWLAN_PolicyConnection_t;

    /**
 * @brief Option parameter for use with CordeliaI_ATWLAN_SetScanPolicy() and CordeliaI_ATWLAN_GetScanPolicy().
 */
    typedef enum CordeliaI_ATWLAN_PolicyScan_t
    {
        CordeliaI_ATWLAN_PolicyScan_HiddenSSID,
        CordeliaI_ATWLAN_PolicyScan_NoHiddenSSID,
        CordeliaI_ATWLAN_PolicyScan_DisableScan,
        CordeliaI_ATWLAN_PolicyScan_NumberOfValues
    } CordeliaI_ATWLAN_PolicyScan_t;

    /**
 * @brief Option parameter for use with CordeliaI_ATWLAN_SetPMPolicy() and CordeliaI_ATWLAN_GetPMPolicy().
 */
    typedef enum CordeliaI_ATWLAN_PolicyPM_t
    {
        CordeliaI_ATWLAN_PolicyPM_Normal,
        CordeliaI_ATWLAN_PolicyPM_LowLatency,
        CordeliaI_ATWLAN_PolicyPM_LowPower,
        CordeliaI_ATWLAN_PolicyPM_LongSleep,
        CordeliaI_ATWLAN_PolicyPM_NumberOfValues
    } CordeliaI_ATWLAN_PolicyPM_t;

    /**
 * @brief Option parameter for use with CordeliaI_ATWLAN_SetP2PPolicy() and CordeliaI_ATWLAN_GetP2PPolicy().
 */
    typedef enum CordeliaI_ATWLAN_PolicyP2P_t
    {
        CordeliaI_ATWLAN_PolicyP2P_Client,
        CordeliaI_ATWLAN_PolicyP2P_GroupOwner,
        CordeliaI_ATWLAN_PolicyP2P_Negotiate,
        CordeliaI_ATWLAN_PolicyP2P_NumberOfValues
    } CordeliaI_ATWLAN_PolicyP2P_t;

    /**
 * @brief Option value parameter for use with CordeliaI_ATWLAN_SetP2PPolicy() and CordeliaI_ATWLAN_GetP2PPolicy().
 */
    typedef enum CordeliaI_ATWLAN_PolicyP2PValue_t
    {
        CordeliaI_ATWLAN_PolicyP2PValue_Active,
        CordeliaI_ATWLAN_PolicyP2PValue_Passive,
        CordeliaI_ATWLAN_PolicyP2PValue_RandBackoff,
        CordeliaI_ATWLAN_PolicyP2PValue_NumberOfValues
    } CordeliaI_ATWLAN_PolicyP2PValue_t;

    /**
 * @brief Wireless LAN security types.
 */
    typedef enum CordeliaI_ATWLAN_SecurityType_t
    {
        CordeliaI_ATWLAN_SecurityType_Open,
        CordeliaI_ATWLAN_SecurityType_WEP,
        CordeliaI_ATWLAN_SecurityType_WEP_SHARED,
        CordeliaI_ATWLAN_SecurityType_WPA_WPA2,
        CordeliaI_ATWLAN_SecurityType_WPA2_PLUS,
        CordeliaI_ATWLAN_SecurityType_WPA3,
        CordeliaI_ATWLAN_SecurityType_WPA_ENT,
        CordeliaI_ATWLAN_SecurityType_WPS_PBC,
        CordeliaI_ATWLAN_SecurityType_WPS_PIN,
        CordeliaI_ATWLAN_SecurityType_NumberOfValues
    } CordeliaI_ATWLAN_SecurityType_t;

    /**
 * @brief Scan security types.
 */
    typedef enum CordeliaI_ATWLAN_ScanSecurityType_t
    {
        CordeliaI_ATWLAN_ScanSecurityType_Open,
        CordeliaI_ATWLAN_ScanSecurityType_WEP,
        CordeliaI_ATWLAN_ScanSecurityType_WPA,
        CordeliaI_ATWLAN_ScanSecurityType_WPA2,
        CordeliaI_ATWLAN_ScanSecurityType_WPA_WPA2,
        CordeliaI_ATWLAN_ScanSecurityType_NumberOfValues
    } CordeliaI_ATWLAN_ScanSecurityType_t;

    /**
 * @brief Scan cipher types.
 */
    typedef enum CordeliaI_ATWLAN_ScanCipher_t
    {
        CordeliaI_ATWLAN_ScanCipherType_None,
        CordeliaI_ATWLAN_ScanCipherType_WEP40,
        CordeliaI_ATWLAN_ScanCipherType_WEP104,
        CordeliaI_ATWLAN_ScanCipherType_TKIP,
        CordeliaI_ATWLAN_ScanCipherType_CCMP,
        CordeliaI_ATWLAN_ScanCipherType_NumberOfValues
    } CordeliaI_ATWLAN_ScanCipher_t;

    /**
 * @brief Scan key management types.
 */
    typedef enum CordeliaI_ATWLAN_ScanKeyManagement_t
    {
        CordeliaI_ATWLAN_ScanKeyManagementType_None,
        CordeliaI_ATWLAN_ScanKeyManagementType_802_1_x,
        CordeliaI_ATWLAN_ScanKeyManagementType_PSK,
        CordeliaI_ATWLAN_ScanKeyManagementType_NumberOfValues
    } CordeliaI_ATWLAN_ScanKeyManagement_t;

    /**
 * @brief Wireless LAN security EAP.
 */
    typedef enum CordeliaI_ATWLAN_SecurityEAP_t
    {
        CordeliaI_ATWLAN_SecurityEAP_None,
        CordeliaI_ATWLAN_SecurityEAP_TLS,
        CordeliaI_ATWLAN_SecurityEAP_TTLS_TLS,
        CordeliaI_ATWLAN_SecurityEAP_TTLS_MSCHAPv2,
        CordeliaI_ATWLAN_SecurityEAP_TTLS_PSK,
        CordeliaI_ATWLAN_SecurityEAP_PEAP0_TLS,
        CordeliaI_ATWLAN_SecurityEAP_PEAP0_MSCHAPv2,
        CordeliaI_ATWLAN_SecurityEAP_PEAP0_PSK,
        CordeliaI_ATWLAN_SecurityEAP_PEAP1_TLS,
        CordeliaI_ATWLAN_SecurityEAP_PEAP1_PSK,
        CordeliaI_ATWLAN_SecurityEAP_NumberOfValues
    } CordeliaI_ATWLAN_SecurityEAP_t;

    /**
 * @brief Wireless Lan Status.
 */
    typedef enum CordeliaI_ATWLAN_Status_t
    {
        CordeliaI_ATWLAN_Status_Disconnected,
        CordeliaI_ATWLAN_Status_StationConnected,
        CordeliaI_ATWLAN_Status_P2PClientConnected,
        CordeliaI_ATWLAN_Status_P2PGroupOwnerConnected,
        CordeliaI_ATWLAN_Status_APConnectedStations,
        CordeliaI_ATWLAN_Status_NumberOfValues
    } CordeliaI_ATWLAN_Status_t;

    /**
 * @brief Wireless Security Status.
 */
    typedef enum CordeliaI_ATWLAN_SecurityStatus_t
    {
        CordeliaI_ATWLAN_SecurityStatus_OPEN,
        CordeliaI_ATWLAN_SecurityStatus_WEP,
        CordeliaI_ATWLAN_SecurityStatus_WPA_WPA2,
        CordeliaI_ATWLAN_SecurityStatus_WPS_PBC,
        CordeliaI_ATWLAN_SecurityStatus_WPS_PIN,
        CordeliaI_ATWLAN_SecurityStatus_WPA_ENT,
        CordeliaI_ATWLAN_SecurityStatus_WEP_SHARED,
        CordeliaI_ATWLAN_SecurityStatus_NumberOfValues
    } CordeliaI_ATWLAN_SecurityStatus_t;

    /**
 * @brief Wireless LAN security configuration.
 */
    typedef struct CordeliaI_ATWLAN_SecurityParams_t
    {
        CordeliaI_ATWLAN_SecurityType_t securityType;
        char securityKey[CordeliaI_ATWLAN_SECURITYKEY_LENGTH];
    } CordeliaI_ATWLAN_SecurityParams_t;

    /**
 * @brief Wireless LAN enterprise security configuration.
 */
    typedef struct CordeliaI_ATWLAN_SecurityEnterpriseParams_t
    {
        char extUser[32];
        char extAnonUser[32];
        CordeliaI_ATWLAN_SecurityEAP_t eapMethod;
    } CordeliaI_ATWLAN_SecurityEnterpriseParams_t;

    /**
 * @brief Wireless LAN scan result entry.
 */
    typedef struct CordeliaI_ATWLAN_ScanEntry_t
    {
        char SSID[CordeliaI_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[CordeliaI_ATWLAN_BSSID_LENGTH];
        uint8_t channel;
        uint8_t hiddenSsidEnabled;
        int8_t RSSI;
        CordeliaI_ATWLAN_ScanCipher_t cipher;
        CordeliaI_ATWLAN_ScanKeyManagement_t keyManagementMethod;
        CordeliaI_ATWLAN_ScanSecurityType_t securityType;
    } CordeliaI_ATWLAN_ScanEntry_t;

    /**
 * @brief Wireless LAN connection arguments.
 */
    typedef struct CordeliaI_ATWLAN_ConnectionArguments_t
    {
        char SSID[CordeliaI_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[CordeliaI_ATWLAN_BSSID_LENGTH];
        CordeliaI_ATWLAN_SecurityParams_t securityParams;
        CordeliaI_ATWLAN_SecurityEnterpriseParams_t securityExtParams;
    } CordeliaI_ATWLAN_ConnectionArguments_t;

    /**
 * @brief Wireless LAN connection profile.
 */
    typedef struct CordeliaI_ATWLAN_Profile_t
    {
        CordeliaI_ATWLAN_ConnectionArguments_t connection;
        uint8_t priority;
    } CordeliaI_ATWLAN_Profile_t;

    /**
 * @brief Wireless LAN scan parameters.
 */
    typedef struct CordeliaI_ATWLAN_ScanParams_t
    {
        uint16_t channelMask;
        uint32_t rssiTreshold;
    } CordeliaI_ATWLAN_ScanParams_t;

    /**
 * @brief General WLAN parameters (used by CordeliaI_ATWLAN_Settings_t)
 */
    typedef union CordeliaI_ATWLAN_SettingsGeneral_t
    {
        char countryCode[3];                      /**< US, EU or JP */
        uint8_t staTxPower;                       /**< 0-15 (0 = max. transmit power) */
        uint8_t apTxPower;                        /**< 0-15 (0 = max. transmit power) */
        CordeliaI_ATWLAN_ScanParams_t scanParams; /**< Scan parameters */
        uint32_t suspendProfiles;                 /**< Bitmask containing profiles to be suspended (e.g. set bits 2 and 4 to suspend profiles 2 and 4) */
        uint8_t disableEntServerAuth;             /**< 0 or 1 (1 = disable server auth when manually connecting to an enterprise network) */
    } CordeliaI_ATWLAN_SettingsGeneral_t;

    /**
 * @brief WLAN connection parameters (used by CordeliaI_ATWLAN_Settings_t)
 */
    typedef struct CordeliaI_ATWLAN_SettingsConnection_t
    {
        CordeliaI_ATWLAN_Status_t status;
        CordeliaI_ATWLAN_SecurityStatus_t security;
        char SSID[CordeliaI_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[CordeliaI_ATWLAN_BSSID_LENGTH];
        char p2pDeviceName[18];
    } CordeliaI_ATWLAN_SettingsConnection_t;

    /**
 * @brief Used by CordeliaI_ATWLAN_Get() and CordeliaI_ATWLAN_Set() to pass parameter values.
 */
    typedef union CordeliaI_ATWLAN_Settings_t
    {
        CordeliaI_ATWLAN_SettingsGeneral_t general; /**< General settings, used with CordeliaI_ATWLAN_SetID_General */
    } CordeliaI_ATWLAN_Settings_t;

    extern bool CordeliaI_ATWLAN_Scan(uint8_t index, uint8_t deviceCount, CordeliaI_ATWLAN_ScanEntry_t* pValues, uint8_t* pNumEntries);
    extern bool CordeliaI_ATWLAN_Connect(CordeliaI_ATWLAN_ConnectionArguments_t connectionArgs);
    extern bool CordeliaI_ATWLAN_Disconnect();
    extern bool CordeliaI_ATWLAN_AddProfile(CordeliaI_ATWLAN_Profile_t profile, uint8_t* pOutIndex);
    extern bool CordeliaI_ATWLAN_GetProfile(uint8_t index, CordeliaI_ATWLAN_Profile_t* pOutProfile);
    extern bool CordeliaI_ATWLAN_DeleteProfile(uint8_t index);
    extern bool CordeliaI_ATWLAN_Get(CordeliaI_ATWLAN_SetID_t id, uint8_t option, CordeliaI_ATWLAN_Settings_t* pValues);
    extern bool CordeliaI_ATWLAN_Set(CordeliaI_ATWLAN_SetID_t id, uint8_t option, CordeliaI_ATWLAN_Settings_t* pValues);
    extern bool CordeliaI_ATWLAN_SetConnectionPolicy(uint8_t policy);
    extern bool CordeliaI_ATWLAN_GetConnectionPolicy(uint8_t* policy);
    extern bool CordeliaI_ATWLAN_SetScanPolicy(CordeliaI_ATWLAN_PolicyScan_t policy, uint32_t scanIntervalSeconds);
    extern bool CordeliaI_ATWLAN_GetScanPolicy(CordeliaI_ATWLAN_PolicyScan_t* policy, uint32_t* scanIntervalSeconds);
    extern bool CordeliaI_ATWLAN_SetPMPolicy(CordeliaI_ATWLAN_PolicyPM_t policy, uint32_t maxSleepTimeMs);
    extern bool CordeliaI_ATWLAN_GetPMPolicy(CordeliaI_ATWLAN_PolicyPM_t* policy, uint32_t* maxSleepTimeMs);
    extern bool CordeliaI_ATWLAN_SetP2PPolicy(CordeliaI_ATWLAN_PolicyP2P_t policy, CordeliaI_ATWLAN_PolicyP2PValue_t value);
    extern bool CordeliaI_ATWLAN_GetP2PPolicy(CordeliaI_ATWLAN_PolicyP2P_t* policy, CordeliaI_ATWLAN_PolicyP2PValue_t* value);

#ifdef __cplusplus
}
#endif

#endif /* CORDELIAI_AT_WLAN_H_INCLUDED */
