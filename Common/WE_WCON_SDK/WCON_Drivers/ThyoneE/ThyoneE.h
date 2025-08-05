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
 * @brief Thyone-e driver header file.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef THYONEE_H_INCLUDED
#define THYONEE_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>

/* timings */
#define THYONEE_BOOT_DURATION (uint16_t)10

#define THYONEE_DEFAULT_BAUDRATE (uint32_t)115200

#define THYONEE_AMOUNT_GPIO_PINS 2

    typedef struct ThyoneE_Pins_t
    {
        WE_Pin_t ThyoneE_Pin_Reset;
        WE_Pin_t ThyoneE_Pin_Mode;
        WE_Pin_t ThyoneE_Pin_Busy;

    } ThyoneE_Pins_t;

    typedef enum ThyoneE_GPIO_t
    {
        ThyoneE_GPIO_1 = (uint8_t)0x01,
        ThyoneE_GPIO_2 = (uint8_t)0x02
    } ThyoneE_GPIO_t;

    typedef enum ThyoneE_GPIO_IO_t
    {
        ThyoneE_GPIO_IO_Disconnected = (uint8_t)0x00,
        ThyoneE_GPIO_IO_Input = (uint8_t)0x01,
        ThyoneE_GPIO_IO_Output = (uint8_t)0x02
    } ThyoneE_GPIO_IO_t;

    typedef enum ThyoneE_GPIO_Output_t
    {
        ThyoneE_GPIO_Output_Low = (uint8_t)0x00,
        ThyoneE_GPIO_Output_High = (uint8_t)0x01
    } ThyoneE_GPIO_Output_t;

    typedef enum ThyoneE_GPIO_Input_t
    {
        ThyoneE_GPIO_Input_NoPull = (uint8_t)0x00,
        ThyoneE_GPIO_Input_PullDown = (uint8_t)0x01,
        ThyoneE_GPIO_Input_PullUp = (uint8_t)0x02
    } ThyoneE_GPIO_Input_t;

    typedef struct ThyoneE_GPIOConfigBlock_t
    {
        ThyoneE_GPIO_t GPIO_ID;
        ThyoneE_GPIO_IO_t function;
        union /* 3Byte */
        {
            ThyoneE_GPIO_Input_t input;
            ThyoneE_GPIO_Output_t output;
        } value;
    } ThyoneE_GPIOConfigBlock_t;

    typedef struct ThyoneE_GPIOControlBlock_t
    {
        ThyoneE_GPIO_t GPIO_ID;
        union /* 1Byte */
        {
            ThyoneE_GPIO_Output_t output;
        } value;
    } ThyoneE_GPIOControlBlock_t;

    typedef enum ThyoneE_ResetReason_t
    {
        ThyoneE_ResetReason_PowerOn_WakeUp = (uint8_t)0x01,
        ThyoneE_ResetReason_PinReset = (uint8_t)0x02,
        ThyoneE_ResetReason_SoftReset = (uint8_t)0x04,
        ThyoneE_ResetReason_Invalid = (uint8_t)0xFF,
    } ThyoneE_ResetReason_t;

    typedef enum ThyoneE_States_t
    {
        ThyoneE_State_Application = (uint8_t)0x00,
        ThyoneE_State_Test = (uint8_t)0x01,
        ThyoneE_State_Invalid = (uint8_t)0xFF,
    } ThyoneE_States_t;

    typedef enum ThyoneE_ModuleMode_t
    {
        ThyoneE_ModuleMode_Normal = (uint8_t)0,
        ThyoneE_ModuleMode_Sniffer = (uint8_t)1,
        ThyoneE_ModuleMode_Repeater = (uint8_t)2,
        ThyoneE_ModuleMode_Invalid = (uint8_t)0xFF,
    } ThyoneE_ModuleMode_t;

    typedef enum ThyoneE_OperationMode_t
    {
        ThyoneE_OperationMode_CommandMode,
        ThyoneE_OperationMode_TransparentMode
    } ThyoneE_OperationMode_t;

    /* user settings */
    typedef enum ThyoneE_UserSettings_t
    {
        ThyoneE_USERSETTING_INDEX_SERIAL_NUMBER = (uint8_t)0x01,
        ThyoneE_USERSETTING_INDEX_FW_VERSION = (uint8_t)0x02,
        ThyoneE_USERSETTING_INDEX_UART_CONFIG = (uint8_t)0x04,
        ThyoneE_USERSETTING_INDEX_RF_CHANNEL = (uint8_t)0x07,
        ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE = (uint8_t)0x08,
        ThyoneE_USERSETTING_INDEX_RF_PROFILE = (uint8_t)0x09,
        ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES = (uint8_t)0x0A,
        ThyoneE_USERSETTING_INDEX_RF_TX_POWER = (uint8_t)0x0B,
        ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS = (uint8_t)0x0C,
        ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS = (uint8_t)0x10,
        ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS = (uint8_t)0x11,
        ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID = (uint8_t)0x12,
        ThyoneE_USERSETTING_INDEX_MAC_ENCRYPTION_KEY = (uint8_t)0x14,
        ThyoneE_USERSETTING_INDEX_MAC_TLL = (uint8_t)0x15,
        ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG = (uint8_t)0x18,
        ThyoneE_USERSETTING_INDEX_MAC_DOUBLEPACKETCLEARTIME = (uint8_t)0x1F,
        ThyoneE_USERSETTING_INDEX_MODULE_MODE = (uint8_t)0x20,
    } ThyoneE_UserSettings_t;

    /* runtime settings */
    typedef enum ThyoneE_RuntimeSettings_t
    {
        ThyoneE_RUNTIMESETTING_INDEX_CONTEXT = (uint8_t)0xFF,
    } ThyoneE_RuntimeSettings_t;

    typedef enum ThyoneE_TXPower_t
    {
        ThyoneE_TXPower_4 = (int8_t)4,
        ThyoneE_TXPower_0 = (int8_t)0,
        ThyoneE_TXPower_minus4 = (int8_t)-4,
        ThyoneE_TXPower_minus8 = (int8_t)-8,
        ThyoneE_TXPower_minus12 = (int8_t)-12,
        ThyoneE_TXPower_minus16 = (int8_t)-16,
        ThyoneE_TXPower_minus20 = (int8_t)-20,
        ThyoneE_TXPower_minus40 = (int8_t)-40,
    } ThyoneE_TXPower_t;

    typedef enum ThyoneE_BaudRateIndex_t
    {
        ThyoneE_BaudRateIndex_1200 = (uint8_t)0,
        ThyoneE_BaudRateIndex_2400 = (uint8_t)2,
        ThyoneE_BaudRateIndex_4800 = (uint8_t)4,
        ThyoneE_BaudRateIndex_9600 = (uint8_t)6,
        ThyoneE_BaudRateIndex_14400 = (uint8_t)8,
        ThyoneE_BaudRateIndex_19200 = (uint8_t)10,
        ThyoneE_BaudRateIndex_28800 = (uint8_t)12,
        ThyoneE_BaudRateIndex_38400 = (uint8_t)14,
        ThyoneE_BaudRateIndex_56000 = (uint8_t)16,
        ThyoneE_BaudRateIndex_57600 = (uint8_t)18,
        ThyoneE_BaudRateIndex_76800 = (uint8_t)20,
        ThyoneE_BaudRateIndex_115200 = (uint8_t)22,
        ThyoneE_BaudRateIndex_230400 = (uint8_t)24,
        ThyoneE_BaudRateIndex_250000 = (uint8_t)26,
        ThyoneE_BaudRateIndex_460800 = (uint8_t)28,
        ThyoneE_BaudRateIndex_921600 = (uint8_t)30,
        ThyoneE_BaudRateIndex_1000000 = (uint8_t)32
    } ThyoneE_BaudRateIndex_t;

    typedef enum ThyoneE_UartParity_t
    {
        ThyoneE_UartParity_Even,
        ThyoneE_UartParity_None
    } ThyoneE_UartParity_t;

    typedef enum ThyoneE_EncryptionMode_t
    {
        ThyoneE_EncryptionMode_Unencrypted = (uint8_t)0,
        ThyoneE_EncryptionMode_TxEncrypted = (uint8_t)1,   /* Transmit encrypted messages */
        ThyoneE_EncryptionMode_RxEncrypted = (uint8_t)2,   /* Receive only encrypted messages. Discard all other messages*/
        ThyoneE_EncryptionMode_RxTxEncrypted = (uint8_t)3, /* Transmit and receive encrypted messages. Discard all other messages*/
        ThyoneE_EncryptionMode_Invalid = (uint8_t)0xFF,
    } ThyoneE_EncryptionMode_t;

    typedef enum ThyoneE_Profile_t
    {
        ThyoneE_Profile_1000kbit = (uint8_t)2,
        ThyoneE_Profile_2000kbit = (uint8_t)3,
        ThyoneE_Profile_Invalid = (uint8_t)0xFF,
    } ThyoneE_Profile_t;

    typedef enum ThyoneE_AddressMode_t
    {
        ThyoneE_AddressMode_Broadcast = (uint8_t)0,
        ThyoneE_AddressMode_Multicast = (uint8_t)1,
        ThyoneE_AddressMode_Unicast = (uint8_t)2,
    } ThyoneE_AddressMode_t;

    extern bool ThyoneE_Init(WE_UART_t* uartP, ThyoneE_Pins_t* pinoutP, ThyoneE_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t));
    extern bool ThyoneE_Deinit(void);

    extern bool ThyoneE_PinReset(void);
    extern bool ThyoneE_Reset(void);

    extern bool ThyoneE_Sleep();
    extern bool ThyoneE_IsTransparentModeBusy(bool* busyStateP);

    extern bool ThyoneE_TransmitBroadcast(uint8_t* payloadP, uint16_t length);
    extern bool ThyoneE_TransmitMulticast(uint8_t* payloadP, uint16_t length);
    extern bool ThyoneE_TransmitUnicast(uint8_t* payloadP, uint16_t length);
    extern bool ThyoneE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool ThyoneE_TransmitMulticastExtended(uint8_t groupID, uint8_t* payloadP, uint16_t length);
    extern bool ThyoneE_TransmitUnicastExtended(uint32_t address, uint8_t* payloadP, uint16_t length);

    /* Functions to control the GPIO feature */
    extern bool ThyoneE_GPIOLocalSetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t number_of_configs);
    extern bool ThyoneE_GPIOLocalGetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP);
    extern bool ThyoneE_GPIOLocalWrite(ThyoneE_GPIOControlBlock_t* controlP, uint16_t number_of_controls);
    extern bool ThyoneE_GPIOLocalRead(uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP);

    extern bool ThyoneE_GPIORemoteSetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t number_of_configs);
    extern bool ThyoneE_GPIORemoteGetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP);
    extern bool ThyoneE_GPIORemoteWrite(uint32_t destAddress, ThyoneE_GPIOControlBlock_t* controlP, uint16_t number_of_controls);
    extern bool ThyoneE_GPIORemoteRead(uint32_t destAddress, uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP);

    extern bool ThyoneE_SetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* ValueP, uint8_t length);
    extern bool ThyoneE_GetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* ResponseP, uint16_t* Response_LengthP);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool ThyoneE_FactoryReset();
    extern bool ThyoneE_Set(ThyoneE_UserSettings_t userSetting, uint8_t* ValueP, uint8_t length);
    extern bool ThyoneE_CheckNSet(ThyoneE_UserSettings_t userSetting, uint8_t* ValueP, uint8_t length);
    extern bool ThyoneE_SetBaudrateIndex(ThyoneE_BaudRateIndex_t baudrate, ThyoneE_UartParity_t parity, bool flowcontrolEnable);
    extern bool ThyoneE_SetEncryptionMode(ThyoneE_EncryptionMode_t encryptionMode);
    extern bool ThyoneE_SetRfProfile(ThyoneE_Profile_t profile);
    extern bool ThyoneE_SetRFChannel(uint8_t channel);
    extern bool ThyoneE_SetRFChannelRuntime(uint8_t channel);
    extern bool ThyoneE_SetNumRetries(uint8_t numRetries);
    extern bool ThyoneE_SetTXPower(ThyoneE_TXPower_t txpower);
    extern bool ThyoneE_SetRpNumSlots(uint8_t numSlots);
    extern bool ThyoneE_SetSourceAddress(uint32_t sourceAddress);
    extern bool ThyoneE_SetDestinationAddress(uint32_t destinationAddress);
    extern bool ThyoneE_SetGroupID(uint8_t groupId);
    extern bool ThyoneE_SetEncryptionKey(uint8_t* keyP);
    extern bool ThyoneE_SetTimeToLive(uint8_t ttl);
    extern bool ThyoneE_SetGPIOBlockRemoteConfig(uint8_t remoteConfig);
    extern bool ThyoneE_SetModuleMode(ThyoneE_ModuleMode_t moduleMode);

    /* Read the non-volatile settings */
    extern bool ThyoneE_Get(ThyoneE_UserSettings_t userSetting, uint8_t* ResponseP, uint16_t* Response_LengthP);
    extern bool ThyoneE_GetSerialNumber(uint8_t* serialNumberP);
    extern bool ThyoneE_GetFWVersion(uint8_t* versionP);
    extern bool ThyoneE_GetBaudrateIndex(ThyoneE_BaudRateIndex_t* baudrateP, ThyoneE_UartParity_t* parityP, bool* flowcontrolEnableP);
    extern bool ThyoneE_GetEncryptionMode(ThyoneE_EncryptionMode_t* encryptionModeP);
    extern bool ThyoneE_GetRfProfile(ThyoneE_Profile_t* profileP);
    extern bool ThyoneE_GetRFChannel(uint8_t* channelP);
    extern bool ThyoneE_GetNumRetries(uint8_t* numRetriesP);
    extern bool ThyoneE_GetTXPower(ThyoneE_TXPower_t* txpowerP);
    extern bool ThyoneE_GetRpNumSlots(uint8_t* numSlotsP);
    extern bool ThyoneE_GetSourceAddress(uint32_t* sourceAddressP);
    extern bool ThyoneE_GetDestinationAddress(uint32_t* destinationAddressP);
    extern bool ThyoneE_GetGroupID(uint8_t* groupIdP);
    extern bool ThyoneE_GetTimeToLive(uint8_t* ttlP);
    extern bool ThyoneE_GetGPIOBlockRemoteConfig(uint8_t* remoteConfigP);
    extern bool ThyoneE_GetModuleMode(ThyoneE_ModuleMode_t* moduleModeP);

#endif // THYONEE_H_INCLUDED

#ifdef __cplusplus
}
#endif
