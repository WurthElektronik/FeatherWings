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
 * @brief Proteus-e driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROTEUSE_H_INCLUDED
#define PROTEUSE_H_INCLUDED

/* Max. payload length for transmission. */
#define PROTEUSE_MAX_RADIO_PAYLOAD_LENGTH (uint16_t)243
#define PROTEUSE_MAX_CMD_PAYLOAD_LENGTH (uint16_t)(PROTEUSE_MAX_RADIO_PAYLOAD_LENGTH + 7)

/* Time (ms) required for booting */
#define PROTEUSE_BOOT_DURATION (uint16_t)35

/* Channel is considered to be connected if status pin (LED_1) is on for this duration */
#define PROTEUSE_STATUS_LED_CONNECTED_TIMEOUT_MS (uint16_t)260

/* Default UART baudrate of Proteus-e module */
#define PROTEUSE_DEFAULT_BAUDRATE (uint32_t)115200

/* Max number of bonded devices supported by the Proteus-e module. */
#define PROTEUSE_MAX_BOND_DEVICES (uint8_t)12

    typedef struct ProteusE_Pins_t
    {
        WE_Pin_t ProteusE_Pin_Reset;
        WE_Pin_t ProteusE_Pin_BusyUartEnable;
        WE_Pin_t ProteusE_Pin_Mode;
        WE_Pin_t ProteusE_Pin_StatusLed1;
    } ProteusE_Pins_t;

    typedef enum ProteusE_OperationMode_t
    {
        ProteusE_OperationMode_CommandMode,
        ProteusE_OperationMode_TransparentMode
    } ProteusE_OperationMode_t;

/* Number of free GPIOs available on the module */
#define PROTEUSE_AMOUNT_GPIO_PINS 2

    typedef enum ProteusE_GPIO_t
    {
        ProteusE_GPIO_1 = (uint8_t)0x01,
        ProteusE_GPIO_2 = (uint8_t)0x02
    } ProteusE_GPIO_t;

    typedef enum ProteusE_GPIO_IO_t
    {
        ProteusE_GPIO_IO_Disconnected = (uint8_t)0x00,
        ProteusE_GPIO_IO_Input = (uint8_t)0x01,
        ProteusE_GPIO_IO_Output = (uint8_t)0x02
    } ProteusE_GPIO_IO_t;

    typedef enum ProteusE_GPIO_Output_t
    {
        ProteusE_GPIO_Output_Low = (uint8_t)0x00,
        ProteusE_GPIO_Output_High = (uint8_t)0x01
    } ProteusE_GPIO_Output_t;

    typedef enum ProteusE_GPIO_Input_t
    {
        ProteusE_GPIO_Input_NoPull = (uint8_t)0x00,
        ProteusE_GPIO_Input_PullDown = (uint8_t)0x01,
        ProteusE_GPIO_Input_PullUp = (uint8_t)0x02
    } ProteusE_GPIO_Input_t;

    typedef struct ProteusE_GPIOConfigBlock_t
    {
        ProteusE_GPIO_t gpioId;
        ProteusE_GPIO_IO_t function;
        union
        {
            ProteusE_GPIO_Input_t input;
            ProteusE_GPIO_Output_t output;
        } value;
    } ProteusE_GPIOConfigBlock_t;

    typedef struct ProteusE_GPIOControlBlock_t
    {
        ProteusE_GPIO_t gpioId;
        union
        {
            ProteusE_GPIO_Output_t output;
        } value;
    } ProteusE_GPIOControlBlock_t;

    typedef enum ProteusE_DriverState_t
    {
        ProteusE_DriverState_BLE_Idle = (uint8_t)0x00,
        ProteusE_DriverState_BLE_Connected = (uint8_t)0x01,
        ProteusE_DriverState_BLE_ChannelOpen = (uint8_t)0x02
    } ProteusE_DriverState_t;

    typedef enum ProteusE_SecurityState_t
    {
        ProteusE_SecurityState_BLE_ReBonded = (uint8_t)0x00,
        ProteusE_SecurityState_BLE_Bonded = (uint8_t)0x01,
        ProteusE_SecurityState_BLE_Paired = (uint8_t)0x02
    } ProteusE_SecurityState_t;

    typedef enum ProteusE_DisconnectReason_t
    {
        ProteusE_DisconnectReason_Unknown,
        ProteusE_DisconnectReason_ConnectionTimeout,
        ProteusE_DisconnectReason_UserTerminatedConnection,
        ProteusE_DisconnectReason_HostTerminatedConnection,
        ProteusE_DisconnectReason_ConnectionIntervalUnacceptable,
        ProteusE_DisconnectReason_MicFailure,
        ProteusE_DisconnectReason_ConnectionSetupFailed,
    } ProteusE_DisconnectReason_t;

    typedef enum ProteusE_BLE_Role_t
    {
        ProteusE_BLE_Role_None = (uint8_t)0x00,
        ProteusE_BLE_Role_Peripheral = (uint8_t)0x01,
        ProteusE_BLE_Role_DTM = (uint8_t)0x10
    } ProteusE_BLE_Role_t;

    typedef enum ProteusE_BLE_Action_t
    {
        ProteusE_BLE_Action_None = (uint8_t)0x00,
        ProteusE_BLE_Action_Idle = (uint8_t)0x01,
        ProteusE_BLE_Action_Connected = (uint8_t)0x03,
        ProteusE_BLE_Action_Sleep = (uint8_t)0x04,
        ProteusE_BLE_Action_DTM = (uint8_t)0x05
    } ProteusE_BLE_Action_t;

    /**
 * @brief User settings
 */
    typedef enum ProteusE_UserSettings_t
    {
        ProteusE_USERSETTING_FS_FWVersion = (uint8_t)0x01,
        ProteusE_USERSETTING_RF_DEVICE_NAME = (uint8_t)0x02,
        ProteusE_USERSETTING_FS_MAC = (uint8_t)0x03,
        ProteusE_USERSETTING_FS_BTMAC = (uint8_t)0x04,
        ProteusE_USERSETTING_UART_TRANSP_ETX = (uint8_t)0x05,
        ProteusE_USERSETTING_UART_TRANSP_ETX_CFG = (uint8_t)0x06,
        ProteusE_USERSETTING_RF_ADVERTISING_TIMEOUT = (uint8_t)0x07,
        ProteusE_USERSETTING_RF_CONNECTION_INTERVAL = (uint8_t)0x08,
        ProteusE_USERSETTING_RF_ADVERTISING_INTERVAL = (uint8_t)0x09,
        ProteusE_USERSETTING_UART_CONFIG_INDEX = (uint8_t)0x0B,
        ProteusE_USERSETTING_RF_SEC_FLAGS = (uint8_t)0x0C,
        ProteusE_USERSETTING_RF_ADVERTISING_DATA = (uint8_t)0x0D,
        ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA = (uint8_t)0x0E,
        ProteusE_USERSETTING_FS_DEVICE_INFO = (uint8_t)0x0F,
        ProteusE_USERSETTING_FS_SERIAL_NUMBER = (uint8_t)0x10,
        ProteusE_USERSETTING_RF_TX_POWER = (uint8_t)0x11,
        ProteusE_USERSETTING_RF_STATIC_PASSKEY = (uint8_t)0x12,
        ProteusE_USERSETTING_RF_APPEARANCE = (uint8_t)0x19,
        ProteusE_USERSETTING_RF_SPPBASEUUID = (uint8_t)0x1A,
        ProteusE_USERSETTING_RF_CFGFLAGS = (uint8_t)0x1C,
        ProteusE_USERSETTING_RF_SPPServiceUUID = (uint8_t)0x20,
        ProteusE_USERSETTING_RF_SPPRXUUID = (uint8_t)0x21,
        ProteusE_USERSETTING_RF_SPPTXUUID = (uint8_t)0x22,
        ProteusE_USERSETTING_UART_TRANSPARENT_TIMEOUT = (uint8_t)0x24,
        ProteusE_USERSETTING_UART_TRANSPARENT_MAXPAYLOAD = (uint8_t)0x25,
    } ProteusE_UserSettings_t;

    /**
 * @brief Configuration flags to be used with ProteusE_SetCFGFlags() and ProteusE_GetCFGFlags()
 */
    typedef enum ProteusE_CfgFlags_t
    {
        ProteusE_CfgFlags_GPIORemoteConfig = (1 << 2),
        ProteusE_CfgFlags_DCDCEnable = (1 << 3),
        ProteusE_CfgFlags_DisconnectEnable = (1 << 4)
    } ProteusE_CfgFlags_t;

#define PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK (uint8_t)0x08
#define PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE (uint8_t)0x10

    typedef enum ProteusE_SecFlags_t
    {
        ProteusE_SecFlags_NONE = (uint8_t)0x00,
        ProteusE_SecFlags_JustWorks = (uint8_t)0x02,
        ProteusE_SecFlags_StaticPassKey = (uint8_t)0x03,
        ProteusE_SecFlags_JustWorks_Bonding = (uint8_t)(ProteusE_SecFlags_JustWorks | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK),
        ProteusE_SecFlags_StaticPassKey_Bonding = (uint8_t)(ProteusE_SecFlags_StaticPassKey | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK),
        ProteusE_SecFlags_JustWorks_BondingOnly = (uint8_t)(ProteusE_SecFlags_JustWorks | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
        ProteusE_SecFlags_StaticPassKey_BondingOnly = (uint8_t)(ProteusE_SecFlags_StaticPassKey | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE)
    } ProteusE_SecFlags_t;

    typedef enum ProteusE_TXPower_t
    {
        ProteusE_TXPower_4 = (int8_t)4,
        ProteusE_TXPower_3 = (int8_t)3,
        ProteusE_TXPower_0 = (int8_t)0,
        ProteusE_TXPower_minus4 = (int8_t)-4,
        ProteusE_TXPower_minus8 = (int8_t)-8,
        ProteusE_TXPower_minus12 = (int8_t)-12,
        ProteusE_TXPower_minus16 = (int8_t)-16,
        ProteusE_TXPower_minus20 = (int8_t)-20,
        ProteusE_TXPower_minus40 = (int8_t)-40
    } ProteusE_TXPower_t;

    typedef enum ProteusE_BaudRate_t
    {
        ProteusE_BaudRateIndex_1200 = (uint8_t)0,
        ProteusE_BaudRateIndex_2400 = (uint8_t)2,
        ProteusE_BaudRateIndex_4800 = (uint8_t)4,
        ProteusE_BaudRateIndex_9600 = (uint8_t)6,
        ProteusE_BaudRateIndex_14400 = (uint8_t)8,
        ProteusE_BaudRateIndex_19200 = (uint8_t)10,
        ProteusE_BaudRateIndex_28800 = (uint8_t)12,
        ProteusE_BaudRateIndex_38400 = (uint8_t)14,
        ProteusE_BaudRateIndex_56000 = (uint8_t)16,
        ProteusE_BaudRateIndex_57600 = (uint8_t)18,
        ProteusE_BaudRateIndex_76800 = (uint8_t)20,
        ProteusE_BaudRateIndex_115200 = (uint8_t)22,
        ProteusE_BaudRateIndex_230400 = (uint8_t)24,
        ProteusE_BaudRateIndex_250000 = (uint8_t)26,
        ProteusE_BaudRateIndex_460800 = (uint8_t)28,
        ProteusE_BaudRateIndex_921600 = (uint8_t)30,
        ProteusE_BaudRateIndex_1000000 = (uint8_t)32
    } ProteusE_BaudRate_t;

    typedef enum ProteusE_UartParity_t
    {
        ProteusE_UartParity_Even,
        ProteusE_UartParity_None
    } ProteusE_UartParity_t;

    typedef enum ProteusE_Phy_t
    {
        ProteusE_Phy_1MBit = 0x01,
        ProteusE_Phy_2MBit = 0x02
    } ProteusE_Phy_t;

    /**
 * @brief Device info structure.
 * @see ProteusE_GetDeviceInfo()
 */
    typedef struct ProteusE_DeviceInfo_t
    {
        uint16_t osVersion;
        uint32_t buildCode;
        uint16_t packageVariant;
        uint32_t chipId;
    } ProteusE_DeviceInfo_t;

    /**
 * @brief Module state structure.
 * @see ProteusE_GetState()
 */
    typedef struct ProteusE_ModuleState_t
    {
        ProteusE_BLE_Role_t role;
        ProteusE_BLE_Action_t action;
        uint8_t connectedDeviceBtMac[6];
        uint16_t connectedDeviceMaxPayloadSize;
    } ProteusE_ModuleState_t;

    /**
 * @brief Entry in ProteusE_BondDatabase_t.
 * @see ProteusE_GetBonds()
 */
    typedef struct ProteusE_BondDatabaseEntry_t
    {
        uint16_t id;
        uint8_t btMac[6];
    } ProteusE_BondDatabaseEntry_t;

    /**
 * @brief List of bonded devices.
 * @see ProteusE_GetBonds()
 */
    typedef struct ProteusE_BondDatabase_t
    {
        uint8_t nrOfDevices;
        ProteusE_BondDatabaseEntry_t devices[PROTEUSE_MAX_BOND_DEVICES];
    } ProteusE_BondDatabase_t;

    /**
 * @brief DTM command type
 */
    typedef enum ProteusE_DTMCommand_t
    {
        ProteusE_DTMCommand_Setup = 0x00,
        ProteusE_DTMCommand_StartRX = 0x01,
        ProteusE_DTMCommand_StartTX = 0x02,
        ProteusE_DTMCommand_Stop = 0x03,
    } ProteusE_DTMCommand_t;

    /**
 * @brief DTM TX pattern
 */
    typedef enum ProteusE_DTMTXPattern_t
    {
        ProteusE_DTMTXPattern_PRBS9 = 0x00,
        ProteusE_DTMTXPattern_0x0F = 0x01,
        ProteusE_DTMTXPattern_0x55 = 0x02,
    } ProteusE_DTMTXPattern_t;

    /* Callback definition */

    typedef void (*ProteusE_RxCallback_t)(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
    /* Note that btMac is not provided if success == false (is set to all zeros) */
    typedef void (*ProteusE_ConnectCallback_t)(bool success, uint8_t* btMac);
    typedef void (*ProteusE_SecurityCallback_t)(uint8_t* btMac, ProteusE_SecurityState_t securityState);
    typedef void (*ProteusE_DisconnectCallback_t)(ProteusE_DisconnectReason_t reason);
    typedef void (*ProteusE_ChannelOpenCallback_t)(uint8_t* btMac, uint16_t maxPayload);
    /* Note that btMac is not provided if success == false (is set to all zeros) */
    typedef void (*ProteusE_PhyUpdateCallback_t)(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
    typedef void (*ProteusE_SleepCallback_t)();
    /* Note that the gpioId parameter is of type uint8_t instead of ProteusE_GPIO_t, as the
 * remote device may support other GPIOs than this device. */
    typedef void (*ProteusE_GpioWriteCallback_t)(bool remote, uint8_t gpioId, uint8_t value);
    typedef void (*ProteusE_GpioRemoteConfigCallback_t)(ProteusE_GPIOConfigBlock_t* gpioConfig);
    typedef void (*ProteusE_ErrorCallback_t)(uint8_t errorCode);

    /**
 * @brief Callback configuration structure. Used as argument for ProteusE_Init().
 *
 * Please note that code in callbacks should be kept simple, as the callback
 * functions are called from ISRs. For this reason, it is also not possible
 * to send requests to the Proteus-e directly from inside a callback.
 */
    typedef struct ProteusE_CallbackConfig_t
    {
        ProteusE_RxCallback_t rxCb;                             /**< Callback for CMD_DATA_IND */
        ProteusE_ConnectCallback_t connectCb;                   /**< Callback for CMD_CONNECT_IND */
        ProteusE_SecurityCallback_t securityCb;                 /**< Callback for CMD_SECURITY_IND */
        ProteusE_DisconnectCallback_t disconnectCb;             /**< Callback for CMD_DISCONNECT_IND */
        ProteusE_ChannelOpenCallback_t channelOpenCb;           /**< Callback for CMD_CHANNELOPEN_RSP */
        ProteusE_PhyUpdateCallback_t phyUpdateCb;               /**< Callback for CMD_PHYUPDATE_IND */
        ProteusE_SleepCallback_t sleepCb;                       /**< Callback for CMD_SLEEP_IND */
        ProteusE_GpioWriteCallback_t gpioWriteCb;               /**< Callback for CMD_GPIO_LOCAL_WRITE_IND and CMD_GPIO_REMOTE_WRITE_IND */
        ProteusE_GpioRemoteConfigCallback_t gpioRemoteConfigCb; /**< Callback for CMD_GPIO_REMOTE_WRITECONFIG_IND */
        ProteusE_ErrorCallback_t errorCb;                       /**< Callback for CMD_ERROR_IND */
    } ProteusE_CallbackConfig_t;

    extern bool ProteusE_Init(WE_UART_t* uartP, ProteusE_Pins_t* pinoutP, ProteusE_OperationMode_t opMode, ProteusE_CallbackConfig_t callbackConfig);
    extern bool ProteusE_Deinit(void);

    extern bool ProteusE_GetState(ProteusE_ModuleState_t* moduleStateP);

    extern bool ProteusE_PinReset(void);
    extern bool ProteusE_Reset(void);

    extern bool ProteusE_UartDisable(void);
    extern bool ProteusE_PinUartEnable(void);

    extern bool ProteusE_Sleep();

    extern bool ProteusE_Disconnect();

    extern bool ProteusE_Transmit(uint8_t* payloadP, uint16_t length);
    extern bool ProteusE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool ProteusE_PhyUpdate(ProteusE_Phy_t phy);

    extern ProteusE_DriverState_t ProteusE_GetDriverState();

    extern bool ProteusE_GetStatusPinLed1Level(WE_Pin_Level_t* statusPinLed1LevelP);
    extern bool ProteusE_IsTransparentModeBusy(bool* busyStateP);
    extern void ProteusE_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

    /* functions to control the GPIO feature */
    extern bool ProteusE_GPIOLocalWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);
    extern bool ProteusE_GPIOLocalReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);
    extern bool ProteusE_GPIOLocalWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);
    extern bool ProteusE_GPIOLocalRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

    extern bool ProteusE_GPIORemoteWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);
    extern bool ProteusE_GPIORemoteReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);
    extern bool ProteusE_GPIORemoteWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);
    extern bool ProteusE_GPIORemoteRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

    extern bool ProteusE_GetBonds(ProteusE_BondDatabase_t* bondDatabaseP);
    extern bool ProteusE_DeleteBonds();
    extern bool ProteusE_DeleteBond(uint8_t bondId);
    extern bool ProteusE_AllowUnbondedConnections();

    /* Functions writing/reading volatile settings */
    extern bool ProteusE_SetRAM(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool ProteusE_GetRAM(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);

    extern bool ProteusE_SetAdvertisingDataRAM(uint8_t* dataP, uint8_t length);
    extern bool ProteusE_SetScanResponseDataRAM(uint8_t* dataP, uint8_t length);

    extern bool ProteusE_GetAdvertisingDataRAM(uint8_t* dataP, uint16_t* lengthP);
    extern bool ProteusE_GetScanResponseDataRAM(uint8_t* dataP, uint16_t* lengthP);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool ProteusE_FactoryReset();
    extern bool ProteusE_Set(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool ProteusE_CheckNSet(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool ProteusE_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength);
    extern bool ProteusE_SetAdvertisingTimeout(uint16_t advTimeout);
    extern bool ProteusE_SetConnectionInterval(uint16_t minIntervalMs, uint16_t maxIntervalMs);
    extern bool ProteusE_SetAdvertisingInterval(uint16_t intervalMs);
    extern bool ProteusE_SetCFGFlags(uint16_t cfgflags);
    extern bool ProteusE_SetBTMAC(uint8_t* btMacP);
    extern bool ProteusE_SetTXPower(ProteusE_TXPower_t txPower);
    extern bool ProteusE_SetSecFlags(ProteusE_SecFlags_t secFlags);
    extern bool ProteusE_SetAdvertisingData(uint8_t* dataP, uint16_t length);
    extern bool ProteusE_SetScanResponseData(uint8_t* dataP, uint16_t length);
    extern bool ProteusE_SetBaudrateIndex(ProteusE_BaudRate_t baudrate, ProteusE_UartParity_t parity, bool flowControlEnable);
    extern bool ProteusE_SetStaticPasskey(uint8_t* staticPasskeyP);
    extern bool ProteusE_SetAppearance(uint16_t appearance);
    extern bool ProteusE_SetSppBaseUuid(uint8_t* uuidP);
    extern bool ProteusE_SetSppServiceUuid(uint8_t* uuidP);
    extern bool ProteusE_SetSppRxUuid(uint8_t* uuidP);
    extern bool ProteusE_SetSppTxUuid(uint8_t* uuidP);

    /* Read the non-volatile settings */
    extern bool ProteusE_Get(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);
    extern bool ProteusE_GetFWVersion(uint8_t* versionP);
    extern bool ProteusE_GetDeviceInfo(ProteusE_DeviceInfo_t* deviceInfoP);
    extern bool ProteusE_GetSerialNumber(uint8_t* serialNumberP);
    extern bool ProteusE_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP);
    extern bool ProteusE_GetMAC(uint8_t* macP);
    extern bool ProteusE_GetBTMAC(uint8_t* btMacP);
    extern bool ProteusE_GetAdvertisingTimeout(uint16_t* advTimeoutP);
    extern bool ProteusE_GetConnectionInterval(uint16_t* minIntervalMsP, uint16_t* maxIntervalMsP);
    extern bool ProteusE_GetAdvertisingInterval(uint16_t* intervalMsP);
    extern bool ProteusE_GetCFGFlags(uint16_t* cfgFlagsP);
    extern bool ProteusE_GetTXPower(ProteusE_TXPower_t* txPowerP);
    extern bool ProteusE_GetSecFlags(ProteusE_SecFlags_t* secFlagsP);
    extern bool ProteusE_GetAdvertisingData(uint8_t* dataP, uint16_t* lengthP);
    extern bool ProteusE_GetScanResponseData(uint8_t* dataP, uint16_t* lengthP);
    extern bool ProteusE_GetBaudrateIndex(ProteusE_BaudRate_t* baudrateP, ProteusE_UartParity_t* parityP, bool* flowControlEnableP);
    extern bool ProteusE_GetStaticPasskey(uint8_t* staticPasskeyP);
    extern bool ProteusE_GetAppearance(uint16_t* appearanceP);
    extern bool ProteusE_GetSppBaseUuid(uint8_t* uuidP);
    extern bool ProteusE_GetSppServiceUuid(uint8_t* uuidP);
    extern bool ProteusE_GetSppRxUuid(uint8_t* uuidP);
    extern bool ProteusE_GetSppTxUuid(uint8_t* uuidP);

    extern bool ProteusE_DTMEnable();
    extern bool ProteusE_DTMRun(ProteusE_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload);
    extern bool ProteusE_DTMStartTX(uint8_t channel, uint8_t length, ProteusE_DTMTXPattern_t pattern);
    extern bool ProteusE_DTMStartTXCarrier(uint8_t channel);
    extern bool ProteusE_DTMStop();
    extern bool ProteusE_DTMSetPhy(ProteusE_Phy_t phy);
    extern bool ProteusE_DTMSetTXPower(ProteusE_TXPower_t power);

#endif // PROTEUSE_H_INCLUDED

#ifdef __cplusplus
}
#endif
