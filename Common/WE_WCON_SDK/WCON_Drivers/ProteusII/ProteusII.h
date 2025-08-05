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
 * @brief Proteus-II driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROTEUSII_H_INCLUDED
#define PROTEUSII_H_INCLUDED

/* Max. radio payload length for transmission. Note that this is the max. length in high
 * throughput mode (see ProteusII_CfgFlags_HighThroughputMode) - in normal mode,
 * the max. radio payload length is 243 bytes. */
#define PROTEUSII_MAX_RADIO_PAYLOAD_LENGTH (uint16_t)964
#define PROTEUSII_MAX_CMD_PAYLOAD_LENGTH (uint16_t)(PROTEUSII_MAX_RADIO_PAYLOAD_LENGTH + 7)

/* Max. length of beacon data (custom data in scan response packet,
 * see ProteusII_SetBeacon()) */
#define PROTEUSII_MAX_BEACON_LENGTH (uint16_t)19

/* Time (ms) required for booting */
#define PROTEUSII_BOOT_DURATION (uint16_t)75

/* Channel is considered to be connected if status pin (LED_2) is on for this duration */
#define PROTEUSII_STATUS_LED_CONNECTED_TIMEOUT_MS (uint16_t)5

/* Default UART baudrate of Proteus-II module */
#define PROTEUSII_DEFAULT_BAUDRATE (uint32_t)115200

/* Note that this is the max. number of devices that may be queried
 * using ProteusII_GetBonds() - the module itself might in fact
 * support more devices. */
#define PROTEUSII_MAX_BOND_DEVICES (uint8_t)32

    typedef enum ProteusII_OperationMode_t
    {
        ProteusII_OperationMode_CommandMode,
        ProteusII_OperationMode_PeripheralOnlyMode
    } ProteusII_OperationMode_t;

    typedef struct ProteusII_Device_t
    {
        uint8_t btmac[6];
        int8_t rssi;
        int8_t txPower;
        uint8_t deviceNameLength;
        uint8_t deviceName[32];
    } ProteusII_Device_t;

/* Max. number of devices that may be queried using ProteusII_GetDevices() */
#define PROTEUSII_MAX_NUMBER_OF_DEVICES (uint8_t)10

    typedef struct ProteusII_GetDevices_t
    {
        uint8_t numberOfDevices;
        ProteusII_Device_t devices[PROTEUSII_MAX_NUMBER_OF_DEVICES];
    } ProteusII_GetDevices_t;

    typedef struct ProteusII_Pins_t
    {
        WE_Pin_t ProteusII_Pin_Reset;
        WE_Pin_t ProteusII_Pin_SleepWakeUp;
        WE_Pin_t ProteusII_Pin_Boot;
        WE_Pin_t ProteusII_Pin_Mode;
        WE_Pin_t ProteusII_Pin_Busy;
        WE_Pin_t ProteusII_Pin_StatusLed2;

    } ProteusII_Pins_t;

    typedef enum ProteusII_DisplayPasskeyAction_t
    {
        ProteusII_DisplayPasskeyAction_NoAction = (uint8_t)0x00,
        ProteusII_DisplayPasskeyAction_PleaseConfirm = (uint8_t)0x01
    } ProteusII_DisplayPasskeyAction_t;

    typedef enum ProteusII_DriverState_t
    {
        ProteusII_DriverState_BLE_Idle = (uint8_t)0x00,
        ProteusII_DriverState_BLE_Connected = (uint8_t)0x01,
        ProteusII_DriverState_BLE_ChannelOpen = (uint8_t)0x02
    } ProteusII_DriverState_t;

    typedef enum ProteusII_SecurityState_t
    {
        ProteusII_SecurityState_BLE_ReBonded = (uint8_t)0x00,
        ProteusII_SecurityState_BLE_Bonded = (uint8_t)0x01,
        ProteusII_SecurityState_BLE_Paired = (uint8_t)0x02
    } ProteusII_SecurityState_t;

    typedef enum ProteusII_DisconnectReason_t
    {
        ProteusII_DisconnectReason_Unknown,
        ProteusII_DisconnectReason_ConnectionTimeout,
        ProteusII_DisconnectReason_UserTerminatedConnection,
        ProteusII_DisconnectReason_HostTerminatedConnection,
        ProteusII_DisconnectReason_ConnectionIntervalUnacceptable,
        ProteusII_DisconnectReason_MicFailure,
        ProteusII_DisconnectReason_ConnectionSetupFailed
    } ProteusII_DisconnectReason_t;

    typedef enum ProteusII_BLE_Role_t
    {
        ProteusII_BLE_Role_None = (uint8_t)0x00,
        ProteusII_BLE_Role_Peripheral = (uint8_t)0x01,
        ProteusII_BLE_Role_Central = (uint8_t)0x02,
        ProteusII_BLE_Role_DTM = (uint8_t)0x10
    } ProteusII_BLE_Role_t;

    typedef enum ProteusII_BLE_Action_t
    {
        ProteusII_BLE_Action_None = (uint8_t)0x00,
        ProteusII_BLE_Action_Idle = (uint8_t)0x01,
        ProteusII_BLE_Action_Scanning = (uint8_t)0x02,
        ProteusII_BLE_Action_Connected = (uint8_t)0x03,
        ProteusII_BLE_Action_Sleep = (uint8_t)0x04,
        ProteusII_BLE_Action_DTM = (uint8_t)0x05
    } ProteusII_BLE_Action_t;

    /**
 * @brief User settings
 */
    typedef enum ProteusII_UserSettings_t
    {
        ProteusII_USERSETTING_FS_FWVersion = (uint8_t)0x01,
        ProteusII_USERSETTING_RF_DEVICE_NAME = (uint8_t)0x02,
        ProteusII_USERSETTING_FS_MAC = (uint8_t)0x03,
        ProteusII_USERSETTING_FS_BTMAC = (uint8_t)0x04,
        ProteusII_USERSETTING_RF_ADVERTISING_TIMEOUT = (uint8_t)0x07,
        ProteusII_USERSETTING_RF_CONNECTION_TIMING = (uint8_t)0x08,
        ProteusII_USERSETTING_RF_SCAN_TIMING = (uint8_t)0x09,
        ProteusII_USERSETTING_RF_SCAN_FACTOR = (uint8_t)0x0A,
        ProteusII_USERSETTING_UART_BAUDRATEINDEX = (uint8_t)0x0B,
        ProteusII_USERSETTING_RF_SEC_FLAGS = (uint8_t)0x0C,
        ProteusII_USERSETTING_RF_SCAN_FLAGS = (uint8_t)0x0D,
        ProteusII_USERSETTING_RF_BEACON_FLAGS = (uint8_t)0x0E,
        ProteusII_USERSETTING_FS_DEVICE_INFO = (uint8_t)0x0F,
        ProteusII_USERSETTING_FS_SERIAL_NUMBER = (uint8_t)0x10,
        ProteusII_USERSETTING_RF_TX_POWER = (uint8_t)0x11,
        ProteusII_USERSETTING_RF_STATIC_PASSKEY = (uint8_t)0x12,
        ProteusII_USERSETTING_DIS_FLAGS = (uint8_t)0x13,
        ProteusII_USERSETTING_DIS_MANUFACTURERNAME = (uint8_t)0x14,
        ProteusII_USERSETTING_DIS_MODELNUMBER = (uint8_t)0x15,
        ProteusII_USERSETTING_DIS_SERIALNUMBER = (uint8_t)0x16,
        ProteusII_USERSETTING_DIS_HWVERSION = (uint8_t)0x17,
        ProteusII_USERSETTING_DIS_SWVERSION = (uint8_t)0x18,
        ProteusII_USERSETTING_RF_APPEARANCE = (uint8_t)0x19,
        ProteusII_USERSETTING_RF_SPPBASEUUID = (uint8_t)0x1A,
        ProteusII_USERSETTING_UART_FLAGS = (uint8_t)0x1B,
        ProteusII_USERSETTING_RF_CFGFLAGS = (uint8_t)0x1C,
        ProteusII_USERSETTING_RF_ADVERTISING_FLAGS = (uint8_t)0x1D,
        ProteusII_USERSETTING_RF_SECFLAGSPERONLY = (uint8_t)0x2C
    } ProteusII_UserSettings_t;

    /**
 * @brief Configuration flags to be used with ProteusII_SetCFGFlags() and ProteusII_GetCFGFlags()
 */
    typedef enum ProteusII_CfgFlags_t
    {
        ProteusII_CfgFlags_HighThroughputMode = (1 << 0)
    } ProteusII_CfgFlags_t;

    /**
 * @brief Flags to be used with ProteusII_SetScanFlags() and ProteusII_GetScanFlags()
 */
    typedef enum ProteusII_ScanFlags_t
    {
        ProteusII_ScanFlags_None = 0,
        ProteusII_ScanFlags_ActiveScanning = 1
    } ProteusII_ScanFlags_t;

    /**
 * @brief Flags to be used with ProteusII_SetBeaconFlags() and ProteusII_GetBeaconFlags()
 */
    typedef enum ProteusII_BeaconFlags_t
    {
        ProteusII_BeaconFlags_ReceiveDisabled = 0,
        ProteusII_BeaconFlags_ReceiveInterpreted = 1,
        ProteusII_BeaconFlags_ReceiveInterpretedDropDuplicates = 3,
        ProteusII_BeaconFlags_ReceiveRssiIndications = 4
    } ProteusII_BeaconFlags_t;

    /**
 * @brief Flags to be used with ProteusII_SetAdvertisingFlags() and ProteusII_GetAdvertisingFlags()
 */
    typedef enum ProteusII_AdvertisingFlags_t
    {
        ProteusII_AdvertisingFlags_Default = 0,
        ProteusII_AdvertisingFlags_DeviceNameAndUuid = 1,
        ProteusII_AdvertisingFlags_DeviceNameAndTxPower = 2
    } ProteusII_AdvertisingFlags_t;

#define PROTEUSII_SEC_MODE_BONDING_ENABLE_MASK (uint8_t)0x08

    typedef enum ProteusII_SecFlags_t
    {
        ProteusII_SecFlags_NONE = (uint8_t)0x00,
        ProteusII_SecFlags_LescJustWorks = (uint8_t)0x01,
        ProteusII_SecFlags_JustWorks = (uint8_t)0x02,
        ProteusII_SecFlags_StaticPassKey = (uint8_t)0x03,
        ProteusII_SecFlags_LescJustWorks_Bonding = (uint8_t)(ProteusII_SecFlags_LescJustWorks | PROTEUSII_SEC_MODE_BONDING_ENABLE_MASK),
        ProteusII_SecFlags_JustWorks_Bonding = (uint8_t)(ProteusII_SecFlags_JustWorks | PROTEUSII_SEC_MODE_BONDING_ENABLE_MASK),
        ProteusII_SecFlags_StaticPassKey_Bonding = (uint8_t)(ProteusII_SecFlags_StaticPassKey | PROTEUSII_SEC_MODE_BONDING_ENABLE_MASK)
    } ProteusII_SecFlags_t;

    typedef enum ProteusII_ConnectionTiming_t
    {
        ProteusII_ConnectionTiming_0 = (uint8_t)0x00,
        ProteusII_ConnectionTiming_1 = (uint8_t)0x01,
        ProteusII_ConnectionTiming_2 = (uint8_t)0x02,
        ProteusII_ConnectionTiming_3 = (uint8_t)0x03,
        ProteusII_ConnectionTiming_4 = (uint8_t)0x04,
        ProteusII_ConnectionTiming_5 = (uint8_t)0x05,
        ProteusII_ConnectionTiming_6 = (uint8_t)0x06,
        ProteusII_ConnectionTiming_7 = (uint8_t)0x07,
        ProteusII_ConnectionTiming_8 = (uint8_t)0x08,
        ProteusII_ConnectionTiming_9 = (uint8_t)0x09,
        ProteusII_ConnectionTiming_10 = (uint8_t)0x0A
    } ProteusII_ConnectionTiming_t;

    typedef enum ProteusII_ScanTiming_t
    {
        ProteusII_ScanTiming_0 = (uint8_t)0x00,
        ProteusII_ScanTiming_1 = (uint8_t)0x01,
        ProteusII_ScanTiming_2 = (uint8_t)0x02,
        ProteusII_ScanTiming_3 = (uint8_t)0x03,
        ProteusII_ScanTiming_4 = (uint8_t)0x04,
        ProteusII_ScanTiming_5 = (uint8_t)0x05
    } ProteusII_ScanTiming_t;

    typedef enum ProteusII_TXPower_t
    {
        ProteusII_TXPower_4 = (int8_t)4,
        ProteusII_TXPower_3 = (int8_t)3,
        ProteusII_TXPower_2 = (int8_t)2,
        ProteusII_TXPower_0 = (int8_t)0,
        ProteusII_TXPower_minus4 = (int8_t)-4,
        ProteusII_TXPower_minus8 = (int8_t)-8,
        ProteusII_TXPower_minus12 = (int8_t)-12,
        ProteusII_TXPower_minus16 = (int8_t)-16,
        ProteusII_TXPower_minus20 = (int8_t)-20,
        ProteusII_TXPower_minus40 = (int8_t)-40
    } ProteusII_TXPower_t;

    typedef enum ProteusII_BaudRate_t
    {
        ProteusII_BaudRateIndex_9600 = (uint8_t)0,
        ProteusII_BaudRateIndex_19200 = (uint8_t)1,
        ProteusII_BaudRateIndex_38400 = (uint8_t)2,
        ProteusII_BaudRateIndex_115200 = (uint8_t)3,
        ProteusII_BaudRateIndex_230400 = (uint8_t)4,
        ProteusII_BaudRateIndex_460800 = (uint8_t)5,
        ProteusII_BaudRateIndex_921600 = (uint8_t)6
    } ProteusII_BaudRate_t;

    typedef enum ProteusII_UartParity_t
    {
        ProteusII_UartParity_Even,
        ProteusII_UartParity_None
    } ProteusII_UartParity_t;

    typedef enum ProteusII_Phy_t
    {
        ProteusII_Phy_1MBit = 0x01,
        ProteusII_Phy_2MBit = 0x02
    } ProteusII_Phy_t;

    /**
 * @brief Device info structure.
 * @see ProteusII_GetDeviceInfo()
 */
    typedef struct ProteusII_DeviceInfo_t
    {
        uint16_t osVersion;
        uint32_t buildCode;
        uint16_t packageVariant;
        uint32_t chipId;
    } ProteusII_DeviceInfo_t;

    /**
 * @brief Module state structure.
 * @see ProteusII_GetState()
 */
    typedef struct ProteusII_ModuleState_t
    {
        ProteusII_BLE_Role_t role;
        ProteusII_BLE_Action_t action;
        uint8_t connectedDeviceBtMac[6];
    } ProteusII_ModuleState_t;

    /**
 * @brief Entry in ProteusII_BondDatabase_t.
 * @see ProteusII_GetBonds()
 */
    typedef struct ProteusII_BondDatabaseEntry_t
    {
        uint16_t id;
        uint8_t btMac[6];
    } ProteusII_BondDatabaseEntry_t;

    /**
 * @brief List of bonded devices.
 * @see ProteusII_GetBonds()
 */
    typedef struct ProteusII_BondDatabase_t
    {
        uint8_t nrOfDevices;
        ProteusII_BondDatabaseEntry_t devices[PROTEUSII_MAX_BOND_DEVICES];
    } ProteusII_BondDatabase_t;

    /**
 * @brief DTM command type
 */
    typedef enum ProteusII_DTMCommand_t
    {
        ProteusII_DTMCommand_Setup = 0x00,
        ProteusII_DTMCommand_StartRX = 0x01,
        ProteusII_DTMCommand_StartTX = 0x02,
        ProteusII_DTMCommand_Stop = 0x03,
    } ProteusII_DTMCommand_t;

    /**
 * @brief DTM TX pattern
 */
    typedef enum ProteusII_DTMTXPattern_t
    {
        ProteusII_DTMTXPattern_PRBS9 = 0x00,
        ProteusII_DTMTXPattern_0x0F = 0x01,
        ProteusII_DTMTXPattern_0x55 = 0x02,
    } ProteusII_DTMTXPattern_t;

    /* Callback definition */

    typedef void (*ProteusII_RxCallback_t)(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
    /* Note that btMac is not provided if success == false (is set to all zeros) */
    typedef void (*ProteusII_ConnectCallback_t)(bool success, uint8_t* btMac);
    typedef void (*ProteusII_SecurityCallback_t)(uint8_t* btMac, ProteusII_SecurityState_t securityState);
    typedef void (*ProteusII_PasskeyCallback_t)(uint8_t* btMac);
    typedef void (*ProteusII_DisplayPasskeyCallback_t)(ProteusII_DisplayPasskeyAction_t action, uint8_t* btMac, uint8_t* passkey);
    typedef void (*ProteusII_DisconnectCallback_t)(ProteusII_DisconnectReason_t reason);
    typedef void (*ProteusII_ChannelOpenCallback_t)(uint8_t* btMac, uint16_t maxPayload);
    /* Note that btMac is not provided if success == false (is set to all zeros) */
    typedef void (*ProteusII_PhyUpdateCallback_t)(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
    typedef void (*ProteusII_SleepCallback_t)();
    typedef void (*ProteusII_RssiCallback_t)(uint8_t* btMac, int8_t rssi, int8_t txPower);
    typedef void (*ProteusII_ErrorCallback_t)(uint8_t errorCode);

    /**
 * @brief Callback configuration structure. Used as argument for ProteusII_Init().
 *
 * Please note that code in callbacks should be kept simple, as the callback
 * functions are called from ISRs. For this reason, it is also not possible
 * to send requests to the Proteus-II directly from inside a callback.
 */
    typedef struct ProteusII_CallbackConfig_t
    {
        ProteusII_RxCallback_t rxCb;                         /**< Callback for CMD_DATA_IND */
        ProteusII_RxCallback_t beaconRxCb;                   /**< Callback for CMD_BEACON_IND and CMD_BEACON_RSP */
        ProteusII_ConnectCallback_t connectCb;               /**< Callback for CMD_CONNECT_IND */
        ProteusII_SecurityCallback_t securityCb;             /**< Callback for CMD_SECURITY_IND */
        ProteusII_PasskeyCallback_t passkeyCb;               /**< Callback for CMD_PASSKEY_IND */
        ProteusII_DisplayPasskeyCallback_t displayPasskeyCb; /**< Callback for CMD_DISPLAY_PASSKEY_IND */
        ProteusII_DisconnectCallback_t disconnectCb;         /**< Callback for CMD_DISCONNECT_IND */
        ProteusII_ChannelOpenCallback_t channelOpenCb;       /**< Callback for CMD_CHANNELOPEN_RSP */
        ProteusII_PhyUpdateCallback_t phyUpdateCb;           /**< Callback for CMD_PHYUPDATE_IND */
        ProteusII_SleepCallback_t sleepCb;                   /**< Callback for CMD_SLEEP_IND */
        ProteusII_RssiCallback_t rssiCb;                     /**< Callback for CMD_RSSI_IND */
        ProteusII_ErrorCallback_t errorCb;                   /**< Callback for CMD_ERROR_IND */
    } ProteusII_CallbackConfig_t;

    extern bool ProteusII_Init(WE_UART_t* uartP, ProteusII_Pins_t* pinoutP, ProteusII_OperationMode_t opMode, ProteusII_CallbackConfig_t callbackConfig);
    extern bool ProteusII_Deinit(void);

    extern bool ProteusII_GetState(ProteusII_ModuleState_t* moduleStateP);

    extern bool ProteusII_PinReset(void);
    extern bool ProteusII_Reset(void);

    extern bool ProteusII_Sleep();
    extern bool ProteusII_PinWakeup();

    extern bool ProteusII_UartDisable(void);
    extern bool ProteusII_PinUartEnable(void);

    extern bool ProteusII_Connect(uint8_t* btMacP);
    extern bool ProteusII_Disconnect();

    extern bool ProteusII_ScanStart();
    extern bool ProteusII_ScanStop();
    extern bool ProteusII_GetDevices(ProteusII_GetDevices_t* devicesP);

    extern bool ProteusII_Transmit(uint8_t* payloadP, uint16_t length);
    extern bool ProteusII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool ProteusII_SetBeacon(uint8_t* beaconDataP, uint16_t length);

    extern bool ProteusII_Passkey(uint8_t* passkeyP);
    extern bool ProteusII_NumericCompareConfirm(bool keyIsOk);

    extern bool ProteusII_PhyUpdate(ProteusII_Phy_t phy);

    extern ProteusII_DriverState_t ProteusII_GetDriverState();

    extern bool ProteusII_GetStatusLed2PinLevel(WE_Pin_Level_t* statusLed2LevelP);
    extern bool ProteusII_IsPeripheralOnlyModeBusy(bool* busyStateP);
    extern void ProteusII_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

    extern bool ProteusII_GetBonds(ProteusII_BondDatabase_t* bondDatabaseP);
    extern bool ProteusII_DeleteBonds();
    extern bool ProteusII_DeleteBond(uint8_t bondId);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool ProteusII_FactoryReset();
    extern bool ProteusII_Set(ProteusII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool ProteusII_CheckNSet(ProteusII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool ProteusII_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength);
    extern bool ProteusII_SetAdvertisingTimeout(uint16_t advTimeout);
    extern bool ProteusII_SetAdvertisingFlags(ProteusII_AdvertisingFlags_t advFlags);
    extern bool ProteusII_SetScanFlags(uint8_t scanFlags);
    extern bool ProteusII_SetBeaconFlags(ProteusII_BeaconFlags_t beaconFlags);
    extern bool ProteusII_SetCFGFlags(uint16_t cfgflags);
    extern bool ProteusII_SetConnectionTiming(ProteusII_ConnectionTiming_t connectionTiming);
    extern bool ProteusII_SetScanTiming(ProteusII_ScanTiming_t scanTiming);
    extern bool ProteusII_SetScanFactor(uint8_t scanFactor);
    extern bool ProteusII_SetTXPower(ProteusII_TXPower_t txPower);
    extern bool ProteusII_SetSecFlags(ProteusII_SecFlags_t secFlags);
    extern bool ProteusII_SetSecFlagsPeripheralOnly(ProteusII_SecFlags_t secFlags);
    extern bool ProteusII_SetBaudrateIndex(ProteusII_BaudRate_t baudrate);
    extern bool ProteusII_SetStaticPasskey(uint8_t* staticPasskeyP);
    extern bool ProteusII_SetAppearance(uint16_t appearance);
    extern bool ProteusII_SetSppBaseUuid(uint8_t* uuidP);

    /* Read the non-volatile settings */
    extern bool ProteusII_Get(ProteusII_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);
    extern bool ProteusII_GetFWVersion(uint8_t* versionP);
    extern bool ProteusII_GetDeviceInfo(ProteusII_DeviceInfo_t* deviceInfoP);
    extern bool ProteusII_GetSerialNumber(uint8_t* serialNumberP);
    extern bool ProteusII_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP);
    extern bool ProteusII_GetMAC(uint8_t* macP);
    extern bool ProteusII_GetBTMAC(uint8_t* btMacP);
    extern bool ProteusII_GetAdvertisingTimeout(uint16_t* advTimeoutP);
    extern bool ProteusII_GetAdvertisingFlags(ProteusII_AdvertisingFlags_t* advFlagsP);
    extern bool ProteusII_GetScanFlags(uint8_t* scanFlagsP);
    extern bool ProteusII_GetBeaconFlags(ProteusII_BeaconFlags_t* beaconFlagsP);
    extern bool ProteusII_GetCFGFlags(uint16_t* cfgFlagsP);
    extern bool ProteusII_GetConnectionTiming(ProteusII_ConnectionTiming_t* connectionTimingP);
    extern bool ProteusII_GetScanTiming(ProteusII_ScanTiming_t* scanTimingP);
    extern bool ProteusII_GetScanFactor(uint8_t* scanFactorP);
    extern bool ProteusII_GetTXPower(ProteusII_TXPower_t* txPowerP);
    extern bool ProteusII_GetSecFlags(ProteusII_SecFlags_t* secFlagsP);
    extern bool ProteusII_GetSecFlagsPeripheralOnly(ProteusII_SecFlags_t* secFlagsP);
    extern bool ProteusII_GetBaudrateIndex(ProteusII_BaudRate_t* baudrateP);
    extern bool ProteusII_GetStaticPasskey(uint8_t* staticPasskeyP);
    extern bool ProteusII_GetAppearance(uint16_t* appearanceP);
    extern bool ProteusII_GetSppBaseUuid(uint8_t* uuidP);

    extern bool ProteusII_DTMEnable();
    extern bool ProteusII_DTMRun(ProteusII_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload);
    extern bool ProteusII_DTMStartTX(uint8_t channel, uint8_t length, ProteusII_DTMTXPattern_t pattern);
    extern bool ProteusII_DTMStartTXCarrier(uint8_t channel);
    extern bool ProteusII_DTMStop();
    extern bool ProteusII_DTMSetPhy(ProteusII_Phy_t phy);
    extern bool ProteusII_DTMSetTXPower(ProteusII_TXPower_t power);
#endif // PROTEUSII_H_INCLUDED

#ifdef __cplusplus
}
#endif
