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

#ifndef CALYPSO_AT_WLAN_H_INCLUDED
#define CALYPSO_AT_WLAN_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Calypso_ATWLAN_BSSID_LENGTH 32       /**< BSSID length (MAC address of the wireless access point) */
#define Calypso_ATWLAN_SSID_MAX_LENGTH 32    /**< Max. SSID length (Wireless LAN identifier) */
#define Calypso_ATWLAN_SECURITYKEY_LENGTH 64 /**< Max. security key length */
#define Calypso_ATWLAN_AP_SECURITYKEY_LENGTH 64

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Wireless LAN operation mode
 */
    typedef enum Calypso_ATWLAN_SetMode_t
    {
        Calypso_ATWLAN_SetMode_Station,
        Calypso_ATWLAN_SetMode_AccessPoint,
        Calypso_ATWLAN_SetMode_P2P,
        Calypso_ATWLAN_SetMode_NumberOfValues
    } Calypso_ATWLAN_SetMode_t;

    /**
 * @brief IDs for Calypso_ATWLAN_Get() and Calypso_ATWLAN_Set()
 */
    typedef enum Calypso_ATWLAN_SetID_t
    {
        Calypso_ATWLAN_SetID_General,
        Calypso_ATWLAN_SetID_P2P,
        Calypso_ATWLAN_SetID_Connection,
        Calypso_ATWLAN_SetID_AccessPoint,
        Calypso_ATWLAN_SetID_NumberOfValues
    } Calypso_ATWLAN_SetID_t;

    /**
 * @brief Option parameter for ID Calypso_ATWLAN_SetID_General in Calypso_ATWLAN_Set().
 */
    typedef enum Calypso_ATWLAN_SetGeneral_t
    {
        Calypso_ATWLAN_SetGeneral_CountryCode,
        Calypso_ATWLAN_SetGeneral_StationTxPower,
        Calypso_ATWLAN_SetGeneral_AccessPointTxPower,
        Calypso_ATWLAN_SetGeneral_ScanParams,
        Calypso_ATWLAN_SetGeneral_SuspendProfiles,
        Calypso_ATWLAN_SetGeneral_DisableEntServerAuth,
        Calypso_ATWLAN_SetGeneral_NumberOfValues
    } Calypso_ATWLAN_SetGeneral_t;

    /**
 * @brief Option parameter for ID Calypso_ATWLAN_SetID_P2P in Calypso_ATWLAN_Set().
 */
    typedef enum Calypso_ATWLAN_SetP2P_t
    {
        Calypso_ATWLAN_SetP2P_ChannelNRegs,
        Calypso_ATWLAN_SetP2P_NumberOfValues
    } Calypso_ATWLAN_SetP2P_t;

    /**
 * @brief Option parameter for ID Calypso_ATWLAN_SetID_AccessPoint in Calypso_ATWLAN_Set().
 */
    typedef enum Calypso_ATWLAN_SetAP_t
    {
        Calypso_ATWLAN_SetAP_SSID,
        Calypso_ATWLAN_SetAP_Channel,
        Calypso_ATWLAN_SetAP_HiddenSSID,
        Calypso_ATWLAN_SetAP_Security,
        Calypso_ATWLAN_SetAP_Password,
        Calypso_ATWLAN_SetAP_MaxStations,
        Calypso_ATWLAN_SetAP_MaxStaAging,
        Calypso_ATWLAN_SetAP_NumberOfValues
    } Calypso_ATWLAN_SetAP_t;

    /**
 * @brief WLAN Policy IDs.
 *
 * @see Calypso_ATWLAN_SetConnectionPolicy(), Calypso_ATWLAN_GetConnectionPolicy(),
 * Calypso_ATWLAN_SetScanPolicy(), Calypso_ATWLAN_GetScanPolicy(),
 * Calypso_ATWLAN_SetPMPolicy(), Calypso_ATWLAN_GetPMPolicy(),
 * Calypso_ATWLAN_SetP2PPolicy(), Calypso_ATWLAN_GetP2PPolicy()
 */
    typedef enum Calypso_ATWLAN_PolicyID_t
    {
        Calypso_ATWLAN_PolicyID_Connection,
        Calypso_ATWLAN_PolicyID_Scan,
        Calypso_ATWLAN_PolicyID_PowerManagement,
        Calypso_ATWLAN_PolicyID_P2P,
        Calypso_ATWLAN_PolicyID_NumberOfValues
    } Calypso_ATWLAN_PolicyID_t;

    /**
 * @brief Option parameter (flags) for use with Calypso_ATWLAN_SetConnectionPolicy() and Calypso_ATWLAN_GetConnectionPolicy().
 *
 * Note that setting a connection policy while the device parameter
 * Calypso_ATDevice_GetGeneral_Persistent is set to false (0) will return an error (code -31008).
 */
    typedef enum Calypso_ATWLAN_PolicyConnection_t
    {
        Calypso_ATWLAN_PolicyConnection_None = 0,
        Calypso_ATWLAN_PolicyConnection_Auto = (1 << 0),
        Calypso_ATWLAN_PolicyConnection_Fast = (1 << 1),
        Calypso_ATWLAN_PolicyConnection_P2P = (1 << 2),
        Calypso_ATWLAN_PolicyConnection_NumberOfValues = 3
    } Calypso_ATWLAN_PolicyConnection_t;

    /**
 * @brief Option parameter for use with Calypso_ATWLAN_SetScanPolicy() and Calypso_ATWLAN_GetScanPolicy().
 */
    typedef enum Calypso_ATWLAN_PolicyScan_t
    {
        Calypso_ATWLAN_PolicyScan_HiddenSSID,
        Calypso_ATWLAN_PolicyScan_NoHiddenSSID,
        Calypso_ATWLAN_PolicyScan_DisableScan,
        Calypso_ATWLAN_PolicyScan_NumberOfValues
    } Calypso_ATWLAN_PolicyScan_t;

    /**
 * @brief Option parameter for use with Calypso_ATWLAN_SetPMPolicy() and Calypso_ATWLAN_GetPMPolicy().
 */
    typedef enum Calypso_ATWLAN_PolicyPM_t
    {
        Calypso_ATWLAN_PolicyPM_Normal,
        Calypso_ATWLAN_PolicyPM_LowLatency,
        Calypso_ATWLAN_PolicyPM_LowPower,
        Calypso_ATWLAN_PolicyPM_LongSleep,
        Calypso_ATWLAN_PolicyPM_NumberOfValues
    } Calypso_ATWLAN_PolicyPM_t;

    /**
 * @brief Option parameter for use with Calypso_ATWLAN_SetP2PPolicy() and Calypso_ATWLAN_GetP2PPolicy().
 */
    typedef enum Calypso_ATWLAN_PolicyP2P_t
    {
        Calypso_ATWLAN_PolicyP2P_Client,
        Calypso_ATWLAN_PolicyP2P_GroupOwner,
        Calypso_ATWLAN_PolicyP2P_Negotiate,
        Calypso_ATWLAN_PolicyP2P_NumberOfValues
    } Calypso_ATWLAN_PolicyP2P_t;

    /**
 * @brief Option value parameter for use with Calypso_ATWLAN_SetP2PPolicy() and Calypso_ATWLAN_GetP2PPolicy().
 */
    typedef enum Calypso_ATWLAN_PolicyP2PValue_t
    {
        Calypso_ATWLAN_PolicyP2PValue_Active,
        Calypso_ATWLAN_PolicyP2PValue_Passive,
        Calypso_ATWLAN_PolicyP2PValue_RandBackoff,
        Calypso_ATWLAN_PolicyP2PValue_NumberOfValues
    } Calypso_ATWLAN_PolicyP2PValue_t;

    /**
 * @brief Wireless LAN security types.
 */
    typedef enum Calypso_ATWLAN_SecurityType_t
    {
        Calypso_ATWLAN_SecurityType_Open,
        Calypso_ATWLAN_SecurityType_WEP,
        Calypso_ATWLAN_SecurityType_WEP_SHARED,
        Calypso_ATWLAN_SecurityType_WPA_WPA2,
        Calypso_ATWLAN_SecurityType_WPA2_PLUS,
        Calypso_ATWLAN_SecurityType_WPA3,
        Calypso_ATWLAN_SecurityType_WPA_ENT,
        Calypso_ATWLAN_SecurityType_WPS_PBC,
        Calypso_ATWLAN_SecurityType_WPS_PIN,
        Calypso_ATWLAN_SecurityType_NumberOfValues
    } Calypso_ATWLAN_SecurityType_t;

    /**
 * @brief Access point security types.
 */
    typedef enum Calypso_ATWLAN_APSecurityType_t
    {
        Calypso_ATWLAN_APSecurityType_Open,
        Calypso_ATWLAN_APSecurityType_WEP,
        Calypso_ATWLAN_APSecurityType_WPA_WPA2,
        Calypso_ATWLAN_APSecurityType_NumberOfValues
    } Calypso_ATWLAN_APSecurityType_t;

    /**
 * @brief Scan security types.
 */
    typedef enum Calypso_ATWLAN_ScanSecurityType_t
    {
        Calypso_ATWLAN_ScanSecurityType_Open,
        Calypso_ATWLAN_ScanSecurityType_WEP,
        Calypso_ATWLAN_ScanSecurityType_WPA,
        Calypso_ATWLAN_ScanSecurityType_WPA2,
        Calypso_ATWLAN_ScanSecurityType_WPA_WPA2,
        Calypso_ATWLAN_ScanSecurityType_NumberOfValues
    } Calypso_ATWLAN_ScanSecurityType_t;

    /**
 * @brief Scan cipher types.
 */
    typedef enum Calypso_ATWLAN_ScanCipher_t
    {
        Calypso_ATWLAN_ScanCipherType_None,
        Calypso_ATWLAN_ScanCipherType_WEP40,
        Calypso_ATWLAN_ScanCipherType_WEP104,
        Calypso_ATWLAN_ScanCipherType_TKIP,
        Calypso_ATWLAN_ScanCipherType_CCMP,
        Calypso_ATWLAN_ScanCipherType_NumberOfValues
    } Calypso_ATWLAN_ScanCipher_t;

    /**
 * @brief Scan key management types.
 */
    typedef enum Calypso_ATWLAN_ScanKeyManagement_t
    {
        Calypso_ATWLAN_ScanKeyManagementType_None,
        Calypso_ATWLAN_ScanKeyManagementType_802_1_x,
        Calypso_ATWLAN_ScanKeyManagementType_PSK,
        Calypso_ATWLAN_ScanKeyManagementType_NumberOfValues
    } Calypso_ATWLAN_ScanKeyManagement_t;

    /**
 * @brief Wireless LAN security EAP.
 */
    typedef enum Calypso_ATWLAN_SecurityEAP_t
    {
        Calypso_ATWLAN_SecurityEAP_None,
        Calypso_ATWLAN_SecurityEAP_TLS,
        Calypso_ATWLAN_SecurityEAP_TTLS_TLS,
        Calypso_ATWLAN_SecurityEAP_TTLS_MSCHAPv2,
        Calypso_ATWLAN_SecurityEAP_TTLS_PSK,
        Calypso_ATWLAN_SecurityEAP_PEAP0_TLS,
        Calypso_ATWLAN_SecurityEAP_PEAP0_MSCHAPv2,
        Calypso_ATWLAN_SecurityEAP_PEAP0_PSK,
        Calypso_ATWLAN_SecurityEAP_PEAP1_TLS,
        Calypso_ATWLAN_SecurityEAP_PEAP1_PSK,
        Calypso_ATWLAN_SecurityEAP_NumberOfValues
    } Calypso_ATWLAN_SecurityEAP_t;

    typedef enum Calypso_ATWLAN_Status_t
    {
        Calypso_ATWLAN_Status_Disconnected,
        Calypso_ATWLAN_Status_StationConnected,
        Calypso_ATWLAN_Status_P2PClientConnected,
        Calypso_ATWLAN_Status_P2PGroupOwnerConnected,
        Calypso_ATWLAN_Status_APConnectedStations,
        Calypso_ATWLAN_Status_NumberOfValues
    } Calypso_ATWLAN_Status_t;

    typedef enum Calypso_ATWLAN_SecurityStatus_t
    {
        Calypso_ATWLAN_SecurityStatus_OPEN,
        Calypso_ATWLAN_SecurityStatus_WEP,
        Calypso_ATWLAN_SecurityStatus_WPA_WPA2,
        Calypso_ATWLAN_SecurityStatus_WPS_PBC,
        Calypso_ATWLAN_SecurityStatus_WPS_PIN,
        Calypso_ATWLAN_SecurityStatus_WPA_ENT,
        Calypso_ATWLAN_SecurityStatus_WEP_SHARED,
        Calypso_ATWLAN_SecurityStatus_NumberOfValues
    } Calypso_ATWLAN_SecurityStatus_t;

    /**
 * @brief Wireless LAN security configuration.
 */
    typedef struct Calypso_ATWLAN_SecurityParams_t
    {
        Calypso_ATWLAN_SecurityType_t securityType;
        char securityKey[Calypso_ATWLAN_SECURITYKEY_LENGTH];
    } Calypso_ATWLAN_SecurityParams_t;

    /**
 * @brief Wireless LAN enterprise security configuration.
 */
    typedef struct Calypso_ATWLAN_SecurityEnterpriseParams_t
    {
        char extUser[32];
        char extAnonUser[32];
        Calypso_ATWLAN_SecurityEAP_t eapMethod;
    } Calypso_ATWLAN_SecurityEnterpriseParams_t;

    /**
 * @brief Wireless LAN scan result entry.
 */
    typedef struct Calypso_ATWLAN_ScanEntry_t
    {
        char SSID[Calypso_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[Calypso_ATWLAN_BSSID_LENGTH];
        uint8_t channel;
        uint8_t hiddenSsidEnabled;
        int8_t RSSI;
        Calypso_ATWLAN_ScanCipher_t cipher;
        Calypso_ATWLAN_ScanKeyManagement_t keyManagementMethod;
        Calypso_ATWLAN_ScanSecurityType_t securityType;
    } Calypso_ATWLAN_ScanEntry_t;

    /**
 * @brief Wireless LAN connection arguments.
 */
    typedef struct Calypso_ATWLAN_ConnectionArguments_t
    {
        char SSID[Calypso_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[Calypso_ATWLAN_BSSID_LENGTH];
        Calypso_ATWLAN_SecurityParams_t securityParams;
        Calypso_ATWLAN_SecurityEnterpriseParams_t securityExtParams;
    } Calypso_ATWLAN_ConnectionArguments_t;

    /**
 * @brief Wireless LAN connection profile.
 */
    typedef struct Calypso_ATWLAN_Profile_t
    {
        Calypso_ATWLAN_ConnectionArguments_t connection;
        uint8_t priority;
    } Calypso_ATWLAN_Profile_t;

    /**
 * @brief Wireless LAN scan parameters.
 */
    typedef struct Calypso_ATWLAN_ScanParams_t
    {
        uint16_t channelMask;
        uint32_t rssiTreshold;
    } Calypso_ATWLAN_ScanParams_t;

    /**
 * @brief P2P channel configuration.
 */
    typedef struct Calypso_ATWLAN_P2PChannelNRegs_t
    {
        uint8_t listenChannel;
        uint8_t listenRegulatoryClass;
        uint8_t operatingChannel;
        uint8_t operatingRegulatoryClass;
    } Calypso_ATWLAN_P2PChannelNRegs_t;

    /**
 * @brief General WLAN parameters (used by Calypso_ATWLAN_Settings_t)
 */
    typedef union Calypso_ATWLAN_SettingsGeneral_t
    {
        char countryCode[3];                    /**< US, EU or JP */
        uint8_t staTxPower;                     /**< 0-15 (0 = max. transmit power) */
        uint8_t apTxPower;                      /**< 0-15 (0 = max. transmit power) */
        Calypso_ATWLAN_ScanParams_t scanParams; /**< Scan parameters */
        uint32_t suspendProfiles;               /**< Bitmask containing profiles to be suspended (e.g. set bits 2 and 4 to suspend profiles 2 and 4) */
        uint8_t disableEntServerAuth;           /**< 0 or 1 (1 = disable server auth when manually connecting to an enterprise network) */
    } Calypso_ATWLAN_SettingsGeneral_t;

    /**
 * @brief WLAN P2P parameters (used by Calypso_ATWLAN_Settings_t)
 */
    typedef union Calypso_ATWLAN_SettingsP2P_t
    {
        Calypso_ATWLAN_P2PChannelNRegs_t p2pChannelNRegs; /**< P2P channels */
    } Calypso_ATWLAN_SettingsP2P_t;

    /**
 * @brief WLAN connection parameters (used by Calypso_ATWLAN_Settings_t)
 */
    typedef struct Calypso_ATWLAN_SettingsConnection_t
    {
        Calypso_ATWLAN_SetMode_t role;
        Calypso_ATWLAN_Status_t status;
        Calypso_ATWLAN_SecurityStatus_t security;
        char SSID[Calypso_ATWLAN_SSID_MAX_LENGTH];
        char BSSID[Calypso_ATWLAN_BSSID_LENGTH];
        char p2pDeviceName[18];
    } Calypso_ATWLAN_SettingsConnection_t;

    /**
 * @brief Access point SSID configuration (used by Calypso_ATWLAN_SettingsAP_t)
 */
    typedef struct Calypso_ATWLAN_SettingsAPSSID_t
    {
        char ssid[Calypso_ATWLAN_SSID_MAX_LENGTH]; /**< AP SSID */
        bool appendMac;                            /**< Append MAC address to SSID (setter only, min. required firmware version: 1.9.0) */
    } Calypso_ATWLAN_SettingsAPSSID_t;

    /**
 * @brief Access point parameters (used by Calypso_ATWLAN_Settings_t)
 */
    typedef union Calypso_ATWLAN_SettingsAP_t
    {
        Calypso_ATWLAN_SettingsAPSSID_t ssidConfig;          /**< AP SSID configuration */
        uint8_t channel;                                     /**< AP Channel (1-11) */
        uint8_t hiddenSSID;                                  /**< 0 = disabled, 1 = send empty SSID in beacon and ignore probe request for broadcast SSID */
        Calypso_ATWLAN_APSecurityType_t security;            /**< Security type */
        char password[Calypso_ATWLAN_AP_SECURITYKEY_LENGTH]; /**< AP password */
        uint8_t maxStations;                                 /**< Max. stations count (1-4) */
        uint32_t maxStaAgingSeconds;                         /**< Max. station aging time (seconds) */
    } Calypso_ATWLAN_SettingsAP_t;

    /**
 * @brief Used by Calypso_ATWLAN_Get() and Calypso_ATWLAN_Set() to pass parameter values.
 */
    typedef union Calypso_ATWLAN_Settings_t
    {
        Calypso_ATWLAN_SettingsGeneral_t general;       /**< General settings, used with Calypso_ATWLAN_SetID_General */
        Calypso_ATWLAN_SettingsP2P_t p2p;               /**< P2P settings, used with Calypso_ATWLAN_SetID_P2P */
        Calypso_ATWLAN_SettingsConnection_t connection; /**< Connection settings (read only), used with Calypso_ATWLAN_SetID_Connection */
        Calypso_ATWLAN_SettingsAP_t ap;                 /**< AP settings, used with Calypso_ATWLAN_SetID_AccessPoint */
    } Calypso_ATWLAN_Settings_t;

    extern bool Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_t mode);
    extern bool Calypso_ATWLAN_Scan(uint8_t index, uint8_t deviceCount, Calypso_ATWLAN_ScanEntry_t* pValues, uint8_t* pNumEntries);
    extern bool Calypso_ATWLAN_Connect(Calypso_ATWLAN_ConnectionArguments_t connectionArgs);
    extern bool Calypso_ATWLAN_Disconnect();
    extern bool Calypso_ATWLAN_AddProfile(Calypso_ATWLAN_Profile_t profile, uint8_t* pOutIndex);
    extern bool Calypso_ATWLAN_GetProfile(uint8_t index, Calypso_ATWLAN_Profile_t* pOutProfile);
    extern bool Calypso_ATWLAN_DeleteProfile(uint8_t index);
    extern bool Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues);
    extern bool Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_t id, uint8_t option, Calypso_ATWLAN_Settings_t* pValues);
    extern bool Calypso_ATWLAN_SetConnectionPolicy(uint8_t policy);
    extern bool Calypso_ATWLAN_GetConnectionPolicy(uint8_t* policy);
    extern bool Calypso_ATWLAN_SetScanPolicy(Calypso_ATWLAN_PolicyScan_t policy, uint32_t scanIntervalSeconds);
    extern bool Calypso_ATWLAN_GetScanPolicy(Calypso_ATWLAN_PolicyScan_t* policy, uint32_t* scanIntervalSeconds);
    extern bool Calypso_ATWLAN_SetPMPolicy(Calypso_ATWLAN_PolicyPM_t policy, uint32_t maxSleepTimeMs);
    extern bool Calypso_ATWLAN_GetPMPolicy(Calypso_ATWLAN_PolicyPM_t* policy, uint32_t* maxSleepTimeMs);
    extern bool Calypso_ATWLAN_SetP2PPolicy(Calypso_ATWLAN_PolicyP2P_t policy, Calypso_ATWLAN_PolicyP2PValue_t value);
    extern bool Calypso_ATWLAN_GetP2PPolicy(Calypso_ATWLAN_PolicyP2P_t* policy, Calypso_ATWLAN_PolicyP2PValue_t* value);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_WLAN_H_INCLUDED */
