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

#ifndef AT_WLAN_H_INCLUDED
#define AT_WLAN_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ATWLAN_BSSID_LENGTH 32       /**< BSSID length (MAC address of the wireless access point) */
#define ATWLAN_SSID_MAX_LENGTH 32    /**< Max. SSID length (Wireless LAN identifier) */
#define ATWLAN_SECURITYKEY_LENGTH 64 /**< Max. security key length */
#define ATWLAN_AP_SECURITYKEY_LENGTH 64

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Wireless LAN operation mode
     */
    typedef enum ATWLAN_SetMode_t
    {
        ATWLAN_SetMode_Station,
        ATWLAN_SetMode_AccessPoint,
        ATWLAN_SetMode_P2P,
        ATWLAN_SetMode_NumberOfValues
    } ATWLAN_SetMode_t;

    /**
     * @brief IDs for ATWLAN_Get() and ATWLAN_Set()
     */
    typedef enum ATWLAN_SetID_t
    {
        ATWLAN_SetID_General,
        ATWLAN_SetID_P2P,
        ATWLAN_SetID_Connection,
        ATWLAN_SetID_AccessPoint,
        ATWLAN_SetID_NumberOfValues
    } ATWLAN_SetID_t;

    /**
     * @brief Option parameter for ID ATWLAN_SetID_General in ATWLAN_Set().
     */
    typedef enum ATWLAN_SetGeneral_t
    {
        ATWLAN_SetGeneral_CountryCode,
        ATWLAN_SetGeneral_StationTxPower,
        ATWLAN_SetGeneral_AccessPointTxPower,
        ATWLAN_SetGeneral_ScanParams,
        ATWLAN_SetGeneral_SuspendProfiles,
        ATWLAN_SetGeneral_DisableEntServerAuth,
        ATWLAN_SetGeneral_NumberOfValues
    } ATWLAN_SetGeneral_t;

    /**
     * @brief Option parameter for ID ATWLAN_SetID_P2P in ATWLAN_Set().
     */
    typedef enum ATWLAN_SetP2P_t
    {
        ATWLAN_SetP2P_ChannelNRegs,
        ATWLAN_SetP2P_NumberOfValues
    } ATWLAN_SetP2P_t;

    /**
     * @brief Option parameter for ID ATWLAN_SetID_AccessPoint in ATWLAN_Set().
     */
    typedef enum ATWLAN_SetAP_t
    {
        ATWLAN_SetAP_SSID,
        ATWLAN_SetAP_Channel,
        ATWLAN_SetAP_HiddenSSID,
        ATWLAN_SetAP_Security,
        ATWLAN_SetAP_Password,
        ATWLAN_SetAP_MaxStations,
        ATWLAN_SetAP_MaxStaAging,
        ATWLAN_SetAP_NumberOfValues
    } ATWLAN_SetAP_t;

    /**
     * @brief WLAN Policy IDs.
     *
     * @see ATWLAN_SetConnectionPolicy(), ATWLAN_GetConnectionPolicy(),
     * ATWLAN_SetScanPolicy(), ATWLAN_GetScanPolicy(),
     * ATWLAN_SetPMPolicy(), ATWLAN_GetPMPolicy(),
     * ATWLAN_SetP2PPolicy(), ATWLAN_GetP2PPolicy()
     */
    typedef enum ATWLAN_PolicyID_t
    {
        ATWLAN_PolicyID_Connection,
        ATWLAN_PolicyID_Scan,
        ATWLAN_PolicyID_PowerManagement,
        ATWLAN_PolicyID_P2P,
        ATWLAN_PolicyID_NumberOfValues
    } ATWLAN_PolicyID_t;

    /**
     * @brief Option parameter (flags) for use with ATWLAN_SetConnectionPolicy() and ATWLAN_GetConnectionPolicy().
     *
     * Note that setting a connection policy while the device parameter
     * ATDevice_GetGeneral_Persistent is set to false (0) will return an error (code -31008).
     */
    typedef enum ATWLAN_PolicyConnection_t
    {
        ATWLAN_PolicyConnection_None = 0,
        ATWLAN_PolicyConnection_Auto = (1 << 0),
        ATWLAN_PolicyConnection_Fast = (1 << 1),
        ATWLAN_PolicyConnection_P2P = (1 << 2),
        ATWLAN_PolicyConnection_NumberOfValues = 3
    } ATWLAN_PolicyConnection_t;

    /**
     * @brief Option parameter for use with ATWLAN_SetScanPolicy() and ATWLAN_GetScanPolicy().
     */
    typedef enum ATWLAN_PolicyScan_t
    {
        ATWLAN_PolicyScan_HiddenSSID,
        ATWLAN_PolicyScan_NoHiddenSSID,
        ATWLAN_PolicyScan_DisableScan,
        ATWLAN_PolicyScan_NumberOfValues
    } ATWLAN_PolicyScan_t;

    /**
     * @brief Option parameter for use with ATWLAN_SetPMPolicy() and ATWLAN_GetPMPolicy().
     */
    typedef enum ATWLAN_PolicyPM_t
    {
        ATWLAN_PolicyPM_Normal,
        ATWLAN_PolicyPM_LowLatency,
        ATWLAN_PolicyPM_LowPower,
        ATWLAN_PolicyPM_LongSleep,
        ATWLAN_PolicyPM_NumberOfValues
    } ATWLAN_PolicyPM_t;

    /**
     * @brief Option parameter for use with ATWLAN_SetP2PPolicy() and ATWLAN_GetP2PPolicy().
     */
    typedef enum ATWLAN_PolicyP2P_t
    {
        ATWLAN_PolicyP2P_Client,
        ATWLAN_PolicyP2P_GroupOwner,
        ATWLAN_PolicyP2P_Negotiate,
        ATWLAN_PolicyP2P_NumberOfValues
    } ATWLAN_PolicyP2P_t;

    /**
     * @brief Option value parameter for use with ATWLAN_SetP2PPolicy() and ATWLAN_GetP2PPolicy().
     */
    typedef enum ATWLAN_PolicyP2PValue_t
    {
        ATWLAN_PolicyP2PValue_Active,
        ATWLAN_PolicyP2PValue_Passive,
        ATWLAN_PolicyP2PValue_RandBackoff,
        ATWLAN_PolicyP2PValue_NumberOfValues
    } ATWLAN_PolicyP2PValue_t;

    /**
     * @brief Wireless LAN security types.
     */
    typedef enum ATWLAN_SecurityType_t
    {
        ATWLAN_SecurityType_Open,
        ATWLAN_SecurityType_WEP,
        ATWLAN_SecurityType_WEP_SHARED,
        ATWLAN_SecurityType_WPA_WPA2,
        ATWLAN_SecurityType_WPA2_PLUS,
        ATWLAN_SecurityType_WPA3,
        ATWLAN_SecurityType_WPA_ENT,
        ATWLAN_SecurityType_WPS_PBC,
        ATWLAN_SecurityType_WPS_PIN,
        ATWLAN_SecurityType_NumberOfValues
    } ATWLAN_SecurityType_t;

    /**
     * @brief Access point security types.
     */
    typedef enum ATWLAN_APSecurityType_t
    {
        ATWLAN_APSecurityType_Open,
        ATWLAN_APSecurityType_WEP,
        ATWLAN_APSecurityType_WPA_WPA2,
        ATWLAN_APSecurityType_NumberOfValues
    } ATWLAN_APSecurityType_t;

    /**
     * @brief Scan security types.
     */
    typedef enum ATWLAN_ScanSecurityType_t
    {
        ATWLAN_ScanSecurityType_Open,
        ATWLAN_ScanSecurityType_WEP,
        ATWLAN_ScanSecurityType_WPA,
        ATWLAN_ScanSecurityType_WPA2,
        ATWLAN_ScanSecurityType_WPA_WPA2,
        ATWLAN_ScanSecurityType_NumberOfValues
    } ATWLAN_ScanSecurityType_t;

    /**
     * @brief Scan cipher types.
     */
    typedef enum ATWLAN_ScanCipher_t
    {
        ATWLAN_ScanCipherType_None,
        ATWLAN_ScanCipherType_WEP40,
        ATWLAN_ScanCipherType_WEP104,
        ATWLAN_ScanCipherType_TKIP,
        ATWLAN_ScanCipherType_CCMP,
        ATWLAN_ScanCipherType_NumberOfValues
    } ATWLAN_ScanCipher_t;

    /**
     * @brief Scan key management types.
     */
    typedef enum ATWLAN_ScanKeyManagement_t
    {
        ATWLAN_ScanKeyManagementType_None,
        ATWLAN_ScanKeyManagementType_802_1_x,
        ATWLAN_ScanKeyManagementType_PSK,
        ATWLAN_ScanKeyManagementType_NumberOfValues
    } ATWLAN_ScanKeyManagement_t;

    /**
     * @brief Wireless LAN security EAP.
     */
    typedef enum ATWLAN_SecurityEAP_t
    {
        ATWLAN_SecurityEAP_None,
        ATWLAN_SecurityEAP_TLS,
        ATWLAN_SecurityEAP_TTLS_TLS,
        ATWLAN_SecurityEAP_TTLS_MSCHAPv2,
        ATWLAN_SecurityEAP_TTLS_PSK,
        ATWLAN_SecurityEAP_PEAP0_TLS,
        ATWLAN_SecurityEAP_PEAP0_MSCHAPv2,
        ATWLAN_SecurityEAP_PEAP0_PSK,
        ATWLAN_SecurityEAP_PEAP1_TLS,
        ATWLAN_SecurityEAP_PEAP1_PSK,
        ATWLAN_SecurityEAP_NumberOfValues
    } ATWLAN_SecurityEAP_t;

    typedef enum ATWLAN_Status_t
    {
        ATWLAN_Status_Disconnected,
        ATWLAN_Status_StationConnected,
        ATWLAN_Status_P2PClientConnected,
        ATWLAN_Status_P2PGroupOwnerConnected,
        ATWLAN_Status_APConnectedStations,
        ATWLAN_Status_NumberOfValues
    } ATWLAN_Status_t;

    typedef enum ATWLAN_SecurityStatus_t
    {
        ATWLAN_SecurityStatus_OPEN,
        ATWLAN_SecurityStatus_WEP,
        ATWLAN_SecurityStatus_WPA_WPA2,
        ATWLAN_SecurityStatus_WPS_PBC,
        ATWLAN_SecurityStatus_WPS_PIN,
        ATWLAN_SecurityStatus_WPA_ENT,
        ATWLAN_SecurityStatus_WEP_SHARED,
        ATWLAN_SecurityStatus_NumberOfValues
    } ATWLAN_SecurityStatus_t;

    /**
     * @brief Wireless LAN security configuration.
     */
    typedef struct ATWLAN_SecurityParams_t
    {
        ATWLAN_SecurityType_t securityType;
        char securityKey[ATWLAN_SECURITYKEY_LENGTH];
    } ATWLAN_SecurityParams_t;

    /**
     * @brief Wireless LAN enterprise security configuration.
     */
    typedef struct ATWLAN_SecurityEnterpriseParams_t
    {
        char extUser[32];
        char extAnonUser[32];
        ATWLAN_SecurityEAP_t eapMethod;
    } ATWLAN_SecurityEnterpriseParams_t;

    /**
     * @brief Wireless LAN scan result entry.
     */
    typedef struct ATWLAN_ScanEntry_t
    {
        char SSID[ATWLAN_SSID_MAX_LENGTH];
        char BSSID[ATWLAN_BSSID_LENGTH];
        uint8_t channel;
        uint8_t hiddenSsidEnabled;
        int8_t RSSI;
        ATWLAN_ScanCipher_t cipher;
        ATWLAN_ScanKeyManagement_t keyManagementMethod;
        ATWLAN_ScanSecurityType_t securityType;
    } ATWLAN_ScanEntry_t;

    /**
     * @brief Wireless LAN connection arguments.
     */
    typedef struct ATWLAN_ConnectionArguments_t
    {
        char SSID[ATWLAN_SSID_MAX_LENGTH];
        char BSSID[ATWLAN_BSSID_LENGTH];
        ATWLAN_SecurityParams_t securityParams;
        ATWLAN_SecurityEnterpriseParams_t securityExtParams;
    } ATWLAN_ConnectionArguments_t;

    /**
     * @brief Wireless LAN connection profile.
     */
    typedef struct ATWLAN_Profile_t
    {
        ATWLAN_ConnectionArguments_t connection;
        uint8_t priority;
    } ATWLAN_Profile_t;

    /**
     * @brief Wireless LAN scan parameters.
     */
    typedef struct ATWLAN_ScanParams_t
    {
        uint16_t channelMask;
        uint32_t rssiTreshold;
    } ATWLAN_ScanParams_t;

    /**
     * @brief P2P channel configuration.
     */
    typedef struct ATWLAN_P2PChannelNRegs_t
    {
        uint8_t listenChannel;
        uint8_t listenRegulatoryClass;
        uint8_t operatingChannel;
        uint8_t operatingRegulatoryClass;
    } ATWLAN_P2PChannelNRegs_t;

    /**
     * @brief General WLAN parameters (used by ATWLAN_Settings_t)
     */
    typedef union ATWLAN_SettingsGeneral_t
    {
        char countryCode[3];            /**< US, EU or JP */
        uint8_t staTxPower;             /**< 0-15 (0 = max. transmit power) */
        uint8_t apTxPower;              /**< 0-15 (0 = max. transmit power) */
        ATWLAN_ScanParams_t scanParams; /**< Scan parameters */
        uint32_t suspendProfiles;       /**< Bitmask containing profiles to be suspended (e.g. set bits 2 and 4 to suspend profiles 2 and 4) */
        uint8_t disableEntServerAuth;   /**< 0 or 1 (1 = disable server auth when manually connecting to an enterprise network) */
    } ATWLAN_SettingsGeneral_t;

    /**
     * @brief WLAN P2P parameters (used by ATWLAN_Settings_t)
     */
    typedef union ATWLAN_SettingsP2P_t
    {
        ATWLAN_P2PChannelNRegs_t p2pChannelNRegs; /**< P2P channels */
    } ATWLAN_SettingsP2P_t;

    /**
     * @brief WLAN connection parameters (used by ATWLAN_Settings_t)
     */
    typedef struct ATWLAN_SettingsConnection_t
    {
        ATWLAN_SetMode_t role;
        ATWLAN_Status_t status;
        ATWLAN_SecurityStatus_t security;
        char SSID[ATWLAN_SSID_MAX_LENGTH];
        char BSSID[ATWLAN_BSSID_LENGTH];
        char p2pDeviceName[18];
    } ATWLAN_SettingsConnection_t;

    /**
     * @brief Access point SSID configuration (used by ATWLAN_SettingsAP_t)
     */
    typedef struct ATWLAN_SettingsAPSSID_t
    {
        char ssid[ATWLAN_SSID_MAX_LENGTH]; /**< AP SSID */
        bool appendMac;                    /**< Append MAC address to SSID (setter only, min. required firmware version: 1.9.0) */
    } ATWLAN_SettingsAPSSID_t;

    /**
     * @brief Access point parameters (used by ATWLAN_Settings_t)
     */
    typedef union ATWLAN_SettingsAP_t
    {
        ATWLAN_SettingsAPSSID_t ssidConfig;          /**< AP SSID configuration */
        uint8_t channel;                             /**< AP Channel (1-11) */
        uint8_t hiddenSSID;                          /**< 0 = disabled, 1 = send empty SSID in beacon and ignore probe request for broadcast SSID */
        ATWLAN_APSecurityType_t security;            /**< Security type */
        char password[ATWLAN_AP_SECURITYKEY_LENGTH]; /**< AP password */
        uint8_t maxStations;                         /**< Max. stations count (1-4) */
        uint32_t maxStaAgingSeconds;                 /**< Max. station aging time (seconds) */
    } ATWLAN_SettingsAP_t;

    /**
     * @brief Used by ATWLAN_Get() and ATWLAN_Set() to pass parameter values.
     */
    typedef union ATWLAN_Settings_t
    {
        ATWLAN_SettingsGeneral_t general;       /**< General settings, used with ATWLAN_SetID_General */
        ATWLAN_SettingsP2P_t p2p;               /**< P2P settings, used with ATWLAN_SetID_P2P */
        ATWLAN_SettingsConnection_t connection; /**< Connection settings (read only), used with ATWLAN_SetID_Connection */
        ATWLAN_SettingsAP_t ap;                 /**< AP settings, used with ATWLAN_SetID_AccessPoint */
    } ATWLAN_Settings_t;

    extern bool ATWLAN_SetMode(ATWLAN_SetMode_t mode);
    extern bool ATWLAN_Scan(uint8_t index,
                            uint8_t deviceCount,
                            ATWLAN_ScanEntry_t *pValues,
                            uint8_t *pNumEntries);
    extern bool ATWLAN_Connect(ATWLAN_ConnectionArguments_t connectionArgs);
    extern bool ATWLAN_Disconnect();
    extern bool ATWLAN_AddProfile(ATWLAN_Profile_t profile,
                                  uint8_t *pOutIndex);
    extern bool ATWLAN_GetProfile(uint8_t index, ATWLAN_Profile_t *pOutProfile);
    extern bool ATWLAN_DeleteProfile(uint8_t index);
    extern bool ATWLAN_Get(ATWLAN_SetID_t id,
                           uint8_t option,
                           ATWLAN_Settings_t *pValues);
    extern bool ATWLAN_Set(ATWLAN_SetID_t id,
                           uint8_t option,
                           ATWLAN_Settings_t *pValues);
    extern bool ATWLAN_SetConnectionPolicy(uint8_t policy);
    extern bool ATWLAN_GetConnectionPolicy(uint8_t *policy);
    extern bool ATWLAN_SetScanPolicy(ATWLAN_PolicyScan_t policy, uint32_t scanIntervalSeconds);
    extern bool ATWLAN_GetScanPolicy(ATWLAN_PolicyScan_t *policy, uint32_t *scanIntervalSeconds);
    extern bool ATWLAN_SetPMPolicy(ATWLAN_PolicyPM_t policy, uint32_t maxSleepTimeMs);
    extern bool ATWLAN_GetPMPolicy(ATWLAN_PolicyPM_t *policy, uint32_t *maxSleepTimeMs);
    extern bool ATWLAN_SetP2PPolicy(ATWLAN_PolicyP2P_t policy, ATWLAN_PolicyP2PValue_t value);
    extern bool ATWLAN_GetP2PPolicy(ATWLAN_PolicyP2P_t *policy, ATWLAN_PolicyP2PValue_t *value);

#ifdef __cplusplus
}
#endif

#endif /* AT_WLAN_H_INCLUDED */
