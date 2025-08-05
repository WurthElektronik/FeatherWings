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
 * @brief AT commands for Bluetooth LE functionality.
 */

#ifndef STEPHANOI_BLUETOOTHLE_H_INCLUDED
#define STEPHANOI_BLUETOOTHLE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BLE_WE_SPP_PRIMARYSERVICE_UUID "6E400001C35211E5953D0002A5D5C51B"
#define BLE_WE_SPP_TXCHAR_UUID "6E400002C35211E5953D0002A5D5C51B"
#define BLE_WE_SPP_RXCHAR_UUID "6E400003C35211E5953D0002A5D5C51B"

#define BLE_DEFAULT_MTU 23

#define BLE_CONNECTION_INDEX_INVALID -1

#define BLE_MAC_STRINGLEN (2 + 5 + 2 * 6 + 1)
#define BLE_ADVDATA_STRINGLEN (2 + 2 * 31 + 1)
#define BLE_DATA_STRINGLEN (2 + 2 * 512)
#define BLE_UUID_STRINGLEN (2 + 16 * 2 + 1)
#define BLE_NAME_STRINGLEN (32 + 1)
#define BLE_KEY_STRINGLEN (6 + 1)

    /**
 * @brief Bluetooth LE device ID
 */
    typedef char StephanoI_ATBluetoothLE_DeviceID_t[BLE_MAC_STRINGLEN];

    /**
 * @brief Bluetooth LE device name
 */
    typedef char StephanoI_ATBluetoothLE_DeviceName_t[BLE_NAME_STRINGLEN];

    /**
 * @brief Bluetooth LE UUID
 */
    typedef char StephanoI_ATBluetoothLE_UUID_t[BLE_UUID_STRINGLEN];

    /**
 * @brief Bluetooth LE manufacturer data
 */
    typedef char StephanoI_ATBluetoothLE_ManufacturerData_t[32];

    /**
 * @brief Bluetooth LE init type
 */
    typedef enum StephanoI_ATBluetoothLE_InitType_t
    {
        StephanoI_ATBluetoothLE_InitType_None = 0,
        StephanoI_ATBluetoothLE_InitType_Client = 1,
        StephanoI_ATBluetoothLE_InitType_Server = 2,
    } StephanoI_ATBluetoothLE_InitType_t;

    /**
 * @brief Bluetooth LE scan filter
 */
    typedef enum StephanoI_ATBluetoothLE_ScanFilter_t
    {
        StephanoI_ATBluetoothLE_ScanFilter_Invalid = 0,
        StephanoI_ATBluetoothLE_ScanFilter_MAC = 1,
        StephanoI_ATBluetoothLE_ScanFilter_NAME = 2,
    } StephanoI_ATBluetoothLE_ScanFilter_t;

    /**
 * @brief Bluetooth LE Phy
 */
    typedef enum StephanoI_ATBluetoothLE_Phy_t
    {
        StephanoI_ATBluetoothLE_Phy_Invalid = 0,
        StephanoI_ATBluetoothLE_Phy_1Mbit = 1,
        StephanoI_ATBluetoothLE_Phy_2Mbit = 2,
        StephanoI_ATBluetoothLE_Phy_Coded = 3
    } StephanoI_ATBluetoothLE_Phy_t;

    /**
 * @brief Bluetooth LE read phy
 */
    typedef struct StephanoI_ATBluetoothLE_ReadPhy_t
    {
        StephanoI_ATBluetoothLE_DeviceID_t address;
        StephanoI_ATBluetoothLE_Phy_t tx;
        StephanoI_ATBluetoothLE_Phy_t rx;
    } StephanoI_ATBluetoothLE_ReadPhy_t;

    /**
 * @brief Bluetooth LE address
 */
    typedef enum StephanoI_ATBluetoothLE_Address_t
    {
        StephanoI_ATBluetoothLE_Address_Public = 0,
        StephanoI_ATBluetoothLE_Address_Random = 1,
    } StephanoI_ATBluetoothLE_Address_t;

    /**
 * @brief Bluetooth LE advertising data
 */
    typedef struct StephanoI_ATBluetoothLE_AdvertisingData_t
    {
        StephanoI_ATBluetoothLE_DeviceName_t devname;
        StephanoI_ATBluetoothLE_UUID_t uuid;
        StephanoI_ATBluetoothLE_ManufacturerData_t manufacturerdata;
        bool include_power;
    } StephanoI_ATBluetoothLE_AdvertisingData_t;

    /**
 * @brief Bluetooth LE advertising type
 */
    typedef enum StephanoI_ATBluetoothLE_AdvertisingType_t
    {
        StephanoI_ATBluetoothLE_AdvertisingType_IND = 0,
        StephanoI_ATBluetoothLE_AdvertisingType_DIRECT_IND_HIGH = 1,
        StephanoI_ATBluetoothLE_AdvertisingType_SCAN_IND = 2,
        StephanoI_ATBluetoothLE_AdvertisingType_NONCONN_IND = 3,
        StephanoI_ATBluetoothLE_AdvertisingType_DIRECT_IND_LOW = 4,
        StephanoI_ATBluetoothLE_AdvertisingType_EXT_NOSCANNABLE_IND = 5,
        StephanoI_ATBluetoothLE_AdvertisingType_EXT_CONNECTABLE = 6,
        StephanoI_ATBluetoothLE_AdvertisingType_EXT_SCANNABLE_IND = 7,
    } StephanoI_ATBluetoothLE_AdvertisingType_t;

    /**
 * @brief Bluetooth LE advertising channel map
 */
    typedef enum StephanoI_ATBluetoothLE_ChannelMap_t
    {
        StephanoI_ATBluetoothLE_ChannelMap_37 = 1,
        StephanoI_ATBluetoothLE_ChannelMap_38 = 2,
        StephanoI_ATBluetoothLE_ChannelMap_39 = 4,
        StephanoI_ATBluetoothLE_ChannelMap_ALL = 7
    } StephanoI_ATBluetoothLE_ChannelMap_t;

    /**
 * @brief Bluetooth LE advertising filter policy
 */
    typedef enum StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_t
    {
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_SCAN_ANY_CON_ANY = 0,
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_SCAN_WLST_CON_ANY = 1,
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_SCAN_ANY_CON_WLST = 2,
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_SCAN_WLST_CON_WLST = 3
    } StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_t;

    /**
 * @brief Bluetooth LE advertising parameters
 */
    typedef struct StephanoI_ATBluetoothLE_AdvertisingParameters_t
    {
        uint16_t min_interval;
        uint16_t max_interval;
        StephanoI_ATBluetoothLE_AdvertisingType_t type;
        StephanoI_ATBluetoothLE_Address_t addr_type;
        StephanoI_ATBluetoothLE_ChannelMap_t channel;
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_t filter;
    } StephanoI_ATBluetoothLE_AdvertisingParameters_t;

    /**
 * @brief Bluetooth LE scan filter policy
 */
    typedef enum StephanoI_ATBluetoothLE_ScanFilterPolicy_t
    {
        StephanoI_ATBluetoothLE_ScanFilterPolicy_ALL = 0,
        StephanoI_ATBluetoothLE_ScanFilterPolicy_ONLY_WLST = 1,
        StephanoI_ATBluetoothLE_ScanFilterPolicy_UND_RPA_DIR = 2,
        StephanoI_ATBluetoothLE_ScanFilterPolicy_WLIST_PRA_DIR = 3
    } StephanoI_ATBluetoothLE_ScanFilterPolicy_t;

    /**
 * @brief Bluetooth LE scan address
 */
    typedef enum StephanoI_ATBluetoothLE_ScanAddress_t
    {
        StephanoI_ATBluetoothLE_ScanAddress_Public = 0,
        StephanoI_ATBluetoothLE_ScanAddress_Random = 1,
        StephanoI_ATBluetoothLE_ScanAddress_RPA_Public = 2,
        StephanoI_ATBluetoothLE_ScanAddress_RAP_Random = 3
    } StephanoI_ATBluetoothLE_ScanAddress_t;

    /**
 * @brief Bluetooth LE scan type
 */
    typedef enum StephanoI_ATBluetoothLE_ScanType_t
    {
        StephanoI_ATBluetoothLE_ScanType_Passive = 0,
        StephanoI_ATBluetoothLE_ScanType_Active = 1
    } StephanoI_ATBluetoothLE_ScanType_t;

    /**
 * @brief Bluetooth LE scan parameters
 */
    typedef struct StephanoI_ATBluetoothLE_ScanParameters_t
    {
        uint16_t scan_interval;
        uint16_t scan_window;
        StephanoI_ATBluetoothLE_ScanType_t type;
        StephanoI_ATBluetoothLE_ScanAddress_t addr_type;
        StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_t filter;
    } StephanoI_ATBluetoothLE_ScanParameters_t;

    /**
 * @brief Bluetooth LE raw advertising data
 */
    typedef char StephanoI_ATBluetoothLE_AdvertisingRawData_t[31 + 1];

    /**
 * @brief Bluetooth LE peripheral discover service event
 */
    typedef struct StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t
    {
        uint8_t srv_index;
        uint8_t start;
        StephanoI_ATBluetoothLE_UUID_t srv_uuid;
        uint8_t srv_type;
    } StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t;

    /**
 * @brief Bluetooth LE discover characteristics event
 */
    typedef struct StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics_t
    {
        char characteristics_type[4 + 1];
        uint8_t srv_index;
        uint8_t char_index;
        uint8_t char_prop;
        uint8_t desc_index;
        char char_uuid[BLE_UUID_STRINGLEN];
        char desc_uuid[BLE_UUID_STRINGLEN];
    } StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics_t;

    /**
 * @brief Bluetooth LE connection event
 */
    typedef struct StephanoI_ATBluetoothLE_Connection_t
    {
        int8_t conn_index;
        char remote_address[BLE_MAC_STRINGLEN];
        bool channelopen;
        bool secure;
    } StephanoI_ATBluetoothLE_Connection_t;

    /**
 * @brief Bluetooth LE encryption request event
 */
    typedef struct StephanoI_ATBluetoothLE_EncryptionRequest_t
    {
        int8_t conn_index;
    } StephanoI_ATBluetoothLE_EncryptionRequest_t;

    /**
 * @brief Bluetooth LE security key request event
 */
    typedef StephanoI_ATBluetoothLE_EncryptionRequest_t StephanoI_ATBluetoothLE_SecurityKeyRequest_t;

    /**
 * @brief Bluetooth LE MTU event
 */
    typedef struct StephanoI_ATBluetoothLE_MTU_t
    {
        int8_t conn_index;
        uint16_t MTU;
    } StephanoI_ATBluetoothLE_MTU_t;

    /**
 * @brief Bluetooth LE write event
 */
    typedef struct StephanoI_ATBluetoothLE_Write_t
    {
        int8_t conn_index;
        uint8_t srv_index;
        uint8_t char_index;
        uint8_t desc;
        uint16_t length;
        uint8_t data[BLE_DATA_STRINGLEN];
    } StephanoI_ATBluetoothLE_Write_t;

    /**
 * @brief Bluetooth LE read event
 */
    typedef struct StephanoI_ATBluetoothLE_Read_t
    {
        int8_t conn_index;
        uint16_t length;
        uint8_t data[BLE_DATA_STRINGLEN];
    } StephanoI_ATBluetoothLE_Read_t;

    /**
 * @brief Bluetooth LE scan event
 */
    typedef struct StephanoI_ATBluetoothLE_Central_Scan_t
    {
        char remote_address[BLE_MAC_STRINGLEN];
        int8_t rssi;
        char adv_data[BLE_ADVDATA_STRINGLEN];
        char scanrsp_data[BLE_ADVDATA_STRINGLEN];
        uint8_t address_type;
    } StephanoI_ATBluetoothLE_Central_Scan_t;

    /**
 * @brief Bluetooth LE discover primary service event
 */
    typedef struct StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t
    {
        int8_t conn_index;
        uint8_t srv_index;
        StephanoI_ATBluetoothLE_UUID_t srv_uuid;
        uint8_t srv_type;
    } StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t;

    /**
 * @brief Bluetooth LE discover included services event
 */
    typedef struct StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t
    {
        int8_t conn_index;
        uint8_t srv_index;
        StephanoI_ATBluetoothLE_UUID_t srv_uuid;
        uint8_t srv_type;
        StephanoI_ATBluetoothLE_UUID_t included_srv_uuid;
        uint8_t included_srv_type;
    } StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t;

    /**
 * @brief Bluetooth LE discover characteristics event
 */
    typedef struct StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t
    {
        char characteristics_type[4 + 1];
        int8_t conn_index;
        uint8_t srv_index;
        uint8_t char_index;
        StephanoI_ATBluetoothLE_UUID_t char_uuid;
        uint8_t char_prop;
        uint8_t desc_index;
        StephanoI_ATBluetoothLE_UUID_t desc_uuid;
    } StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t;

    /**
 * @brief Bluetooth LE connection parameters
 */
    typedef struct StephanoI_ATBluetoothLE_Central_ConnectionParameters_t
    {
        int8_t conn_index;
        uint16_t min_interval;
        uint16_t max_interval;
        uint16_t latency;
        uint16_t timeout;

    } StephanoI_ATBluetoothLE_Central_ConnectionParameters_t;

    /**
 * @brief Bluetooth LE authentication complete event
 */
    typedef struct StephanoI_ATBluetoothLE_AuthenticationComplete_t
    {
        int8_t conn_index;
        bool success;
    } StephanoI_ATBluetoothLE_AuthenticationComplete_t;

    /**
 * @brief Bluetooth LE authentication options
 */
    typedef enum StephanoI_ATBluetoothLE_AuthenticationOptions_t
    {
        StephanoI_ATBluetoothLE_AuthenticationOptions_Open = 0,
        StephanoI_ATBluetoothLE_AuthenticationOptions_Bond = 1,
        StephanoI_ATBluetoothLE_AuthenticationOptions_MITM = 4,
        StephanoI_ATBluetoothLE_AuthenticationOptions_SC_Only = 8,
        StephanoI_ATBluetoothLE_AuthenticationOptions_SC_Bond = 9,
        StephanoI_ATBluetoothLE_AuthenticationOptions_SC_MITM = 12,
        StephanoI_ATBluetoothLE_AuthenticationOptions_SC_MITM_BOND = 13
    } StephanoI_ATBluetoothLE_AuthenticationOptions_t;

    /**
 * @brief Bluetooth LE authentication input output capabilities
 */
    typedef enum StephanoI_ATBluetoothLE_IOCapabilities_t
    {
        StephanoI_ATBluetoothLE_IOCapabilities_DisplayOnly = 0,
        StephanoI_ATBluetoothLE_IOCapabilities_DisplayYesNo = 1,
        StephanoI_ATBluetoothLE_IOCapabilities_KeyboardOnly = 2,
        StephanoI_ATBluetoothLE_IOCapabilities_NoInputOutput = 3,
        StephanoI_ATBluetoothLE_IOCapabilities_KeyboardNDisplay = 4
    } StephanoI_ATBluetoothLE_IOCapabilities_t;

    /**
 * @brief Bluetooth LE security parameters
 */
    typedef struct StephanoI_ATBluetoothLE_SecurityParameters_t
    {
        StephanoI_ATBluetoothLE_AuthenticationOptions_t auth;
        StephanoI_ATBluetoothLE_IOCapabilities_t io;
        uint8_t key_size;
        uint8_t init_key;
        uint8_t rsp_key;
        bool auth_option;

    } StephanoI_ATBluetoothLE_SecurityParameters_t;

    /**
 * @brief Bluetooth LE key
 */
    typedef char StephanoI_ATBluetoothLE_Key_t[BLE_KEY_STRINGLEN];

    /**
 * @brief Bluetooth LE notify key parameters
 */
    typedef struct StephanoI_ATBluetoothLE_NotifyKey_t
    {
        int8_t conn_index;
        StephanoI_ATBluetoothLE_Key_t key;

    } StephanoI_ATBluetoothLE_NotifyKey_t;

    /**
 * @brief Bluetooth LE initiate encryption
 */
    typedef enum StephanoI_ATBluetoothLE_InitiateEncryption_t
    {
        StephanoI_ATBluetoothLE_InitiateEncryption_None = 0,
        StephanoI_ATBluetoothLE_InitiateEncryption_Encrypt = 1,
        StephanoI_ATBluetoothLE_InitiateEncryption_Encrypt_no_MITM = 2,
        StephanoI_ATBluetoothLE_InitiateEncryption_Encrypt_MITM = 3
    } StephanoI_ATBluetoothLE_InitiateEncryption_t;

    /**
 * @brief Bluetooth LE bonding event parameters
 */
    typedef struct StephanoI_ATBluetoothLE_Bonding_t
    {
        int8_t enc_dev_index;
        char mac[BLE_MAC_STRINGLEN];

    } StephanoI_ATBluetoothLE_Bonding_t;

    extern bool StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_t type);
    extern bool StephanoI_ATBluetoothLE_SetDeviceID(StephanoI_ATBluetoothLE_Address_t type, StephanoI_ATBluetoothLE_DeviceID_t ID);
    extern bool StephanoI_ATBluetoothLE_GetDeviceID(StephanoI_ATBluetoothLE_DeviceID_t* ID);
    extern bool StephanoI_ATBluetoothLE_SetDeviceName(StephanoI_ATBluetoothLE_DeviceName_t dev_name);
    extern bool StephanoI_ATBluetoothLE_GetDeviceName(StephanoI_ATBluetoothLE_DeviceName_t* dev_name);
    extern bool StephanoI_ATBluetoothLE_SetScanResponseRawData(StephanoI_ATBluetoothLE_AdvertisingRawData_t data);
    extern bool StephanoI_ATBluetoothLE_SetAdvertisingRawData(StephanoI_ATBluetoothLE_AdvertisingRawData_t data);
    extern bool StephanoI_ATBluetoothLE_SetAdvertisingData(StephanoI_ATBluetoothLE_AdvertisingData_t data);
    extern bool StephanoI_ATBluetoothLE_GetAdvertisingData(StephanoI_ATBluetoothLE_AdvertisingData_t* data);
    extern bool StephanoI_ATBluetoothLE_SetAdvertisingParameters(StephanoI_ATBluetoothLE_AdvertisingParameters_t params);
    extern bool StephanoI_ATBluetoothLE_GetAdvertisingParameters(StephanoI_ATBluetoothLE_AdvertisingParameters_t* params);
    extern bool StephanoI_ATBluetoothLE_StartAdvertising(void);
    extern bool StephanoI_ATBluetoothLE_StopAdvertising(void);
    extern bool StephanoI_ATBluetoothLE_SetScanParameters(StephanoI_ATBluetoothLE_ScanParameters_t params);
    extern bool StephanoI_ATBluetoothLE_GetScanParameters(StephanoI_ATBluetoothLE_ScanParameters_t* params);

    extern bool StephanoI_ATBluetoothLE_SetSecurityParameters(StephanoI_ATBluetoothLE_SecurityParameters_t params);
    extern bool StephanoI_ATBluetoothLE_GetSecurityParameters(StephanoI_ATBluetoothLE_SecurityParameters_t* paramsP);

    extern bool StephanoI_ATBluetoothLE_RespondPairingRequest(int8_t conn_index, bool accept);
    extern bool StephanoI_ATBluetoothLE_InitiateEncryption(int8_t conn_index, StephanoI_ATBluetoothLE_InitiateEncryption_t sec_act);
    extern bool StephanoI_ATBluetoothLE_ReplyKey(int8_t conn_index, char* key);
    extern bool StephanoI_ATBluetoothLE_ConfirmValue(int8_t conn_index, bool confirm);
    extern bool StephanoI_ATBluetoothLE_SetStaticKey(StephanoI_ATBluetoothLE_Key_t key);
    extern bool StephanoI_ATBluetoothLE_GetStaticKey(StephanoI_ATBluetoothLE_Key_t* keyP);
    extern bool StephanoI_ATBluetoothLE_BondsDelete(int8_t bonding_index);
    extern bool StephanoI_ATBluetoothLE_BondsGet();

    extern bool StephanoI_ATBluetoothLE_Peripheral_ServiceCreate(void);
    extern bool StephanoI_ATBluetoothLE_Peripheral_ServiceStart(int8_t srv_index);
    extern bool StephanoI_ATBluetoothLE_Peripheral_ServiceStop(int8_t srv_index);
    extern bool StephanoI_ATBluetoothLE_Peripheral_DiscoverServices(void);
    extern bool StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics(void);
    extern bool StephanoI_ATBluetoothLE_SetDatalen(int8_t conn_index, uint16_t length);
    extern bool StephanoI_ATBluetoothLE_PeripheralNotifyClient(int8_t conn_index, uint8_t srv_index, uint8_t char_index, uint8_t* data, uint32_t length);
    extern bool StephanoI_ATBluetoothLE_PeripheralIndicateClient(int8_t conn_index, uint8_t srv_index, uint8_t char_index, uint8_t* data, uint32_t length);
    extern bool StephanoI_ATBluetoothLE_CentralScan(bool enable, int8_t interval, StephanoI_ATBluetoothLE_ScanFilter_t filter_type, char* filter_param);
    extern bool StephanoI_ATBluetoothLE_CentralConnect(int8_t conn_index, char* remote_address, bool public_address, uint8_t timeout);
    extern bool StephanoI_ATBluetoothLE_CentralSetMTU(StephanoI_ATBluetoothLE_MTU_t data);
    extern bool StephanoI_ATBluetoothLE_CentralGetMTU(StephanoI_ATBluetoothLE_MTU_t* data);
    extern bool StephanoI_ATBluetoothLE_Disconnect(int8_t conn_index);
    extern bool StephanoI_ATBluetoothLE_CentralWrite(int8_t conn_index, uint8_t srv_index, uint8_t char_index, int8_t desc_index, uint8_t* data, uint32_t length);
    extern bool StephanoI_ATBluetoothLE_CentralRead(int8_t conn_index, uint8_t srv_index, uint8_t char_index, int8_t desc_index, StephanoI_ATBluetoothLE_Read_t* t);
    extern bool StephanoI_ATBluetoothLE_CentralDiscoverPrimaryService(int8_t conn_index);
    extern bool StephanoI_ATBluetoothLE_CentralDiscoverIncludedServices(int8_t conn_index, uint8_t srv_index);
    extern bool StephanoI_ATBluetoothLE_CentralDiscoverCharacteristics(int8_t conn_index, uint8_t srv_index);
    extern bool StephanoI_ATBluetoothLE_CentralSetConnectionParameters(StephanoI_ATBluetoothLE_Central_ConnectionParameters_t param);
    extern bool StephanoI_ATBluetoothLE_CentralGetConnectionParameters(StephanoI_ATBluetoothLE_Central_ConnectionParameters_t* paramP, uint16_t* current_intervalP);
    extern bool StephanoI_ATBluetoothLE_SetConnectionPhy(int8_t conn_index, StephanoI_ATBluetoothLE_Phy_t tx_rx_phy);
    extern bool StephanoI_ATBluetoothLE_GetConnectionPhy(int8_t conn_index, StephanoI_ATBluetoothLE_ReadPhy_t* tx_rx_phy);

    extern bool StephanoI_ATBluetoothLE_ParsePeripheralDiscoverService(char* EventArgumentsP, StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t* t);
    extern bool StephanoI_ATBluetoothLE_ParsePeripheralDiscoverCharacteristics(char* EventArgumentsP, StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseConnection(char* EventArgumentsP, StephanoI_ATBluetoothLE_Connection_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseMTU(char* EventArgumentsP, StephanoI_ATBluetoothLE_MTU_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseWrite(char* EventArgumentsP, StephanoI_ATBluetoothLE_Write_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseRead(char* EventArgumentsP, StephanoI_ATBluetoothLE_Read_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseScan(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_Scan_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseCentralDiscoverPrimaryService(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseCentralDiscoverIncludedServices(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseCentralDiscoverCharacteristics(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseAuthenticationComplete(char* EventArgumentsP, StephanoI_ATBluetoothLE_AuthenticationComplete_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseEncryptionRequest(char* EventArgumentsP, StephanoI_ATBluetoothLE_EncryptionRequest_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseSecurityKeyRequest(char* EventArgumentsP, StephanoI_ATBluetoothLE_SecurityKeyRequest_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseNotifyKey(char* EventArgumentsP, StephanoI_ATBluetoothLE_NotifyKey_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseDeviceID(char* EventArgumentsP, StephanoI_ATBluetoothLE_DeviceID_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseDeviceName(char* EventArgumentsP, StephanoI_ATBluetoothLE_DeviceName_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseAdvertisingData(char* EventArgumentsP, StephanoI_ATBluetoothLE_AdvertisingData_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseAdvertisingParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_AdvertisingParameters_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseScanParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_ScanParameters_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseReadPhy(char* EventArgumentsP, StephanoI_ATBluetoothLE_ReadPhy_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseConnectionParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_ConnectionParameters_t* t, uint16_t* current_intervalP);
    extern bool StephanoI_ATBluetoothLE_ParseSecurityParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_SecurityParameters_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseBondingInformations(char* EventArgumentsP, StephanoI_ATBluetoothLE_Bonding_t* t);
    extern bool StephanoI_ATBluetoothLE_ParseStaticKey(char* EventArgumentsP, StephanoI_ATBluetoothLE_Key_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_BLUETOOTHLE_H_INCLUDED */
