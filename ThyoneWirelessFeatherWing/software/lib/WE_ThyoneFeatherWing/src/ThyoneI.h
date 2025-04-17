/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 *THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 *KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 *INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 *INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 *REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 *PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT
 *RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED.
 *INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS
 *OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE
 *SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
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
 * @brief Thyone-I driver header file.
 */

#include <stdbool.h>
#include <stdint.h>
#include "../global/global.h"
#include "atecc608a.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THYONEI_H_INCLUDED
#define THYONEI_H_INCLUDED

/* timings */
#define THYONEI_BOOT_DURATION (uint16_t)75

#define THYONEI_DEFAULT_BAUDRATE (uint32_t)115200

#define THYONEI_AMOUNT_GPIO_PINS 6

typedef struct ThyoneI_Pins_t {
    WE_Pin_t ThyoneI_Pin_SleepWakeUp;
    WE_Pin_t ThyoneI_Pin_Mode;
} ThyoneI_Pins_t;

typedef enum ThyoneI_GPIO_t {
    ThyoneI_GPIO_1 = (uint8_t)0x01,
    ThyoneI_GPIO_2 = (uint8_t)0x02,
    ThyoneI_GPIO_3 = (uint8_t)0x03,
    ThyoneI_GPIO_4 = (uint8_t)0x04,
    ThyoneI_GPIO_5 = (uint8_t)0x05,
    ThyoneI_GPIO_6 = (uint8_t)0x06
} ThyoneI_GPIO_t;

typedef enum ThyoneI_GPIO_IO_t {
    ThyoneI_GPIO_IO_Disconnected = (uint8_t)0x00,
    ThyoneI_GPIO_IO_Input = (uint8_t)0x01,
    ThyoneI_GPIO_IO_Output = (uint8_t)0x02,
    ThyoneI_GPIO_IO_PWM = (uint8_t)0x03
} ThyoneI_GPIO_IO_t;

typedef enum ThyoneI_GPIO_Output_t {
    ThyoneI_GPIO_Output_Low = (uint8_t)0x00,
    ThyoneI_GPIO_Output_High = (uint8_t)0x01
} ThyoneI_GPIO_Output_t;

typedef enum ThyoneI_GPIO_Input_t {
    ThyoneI_GPIO_Input_NoPull = (uint8_t)0x00,
    ThyoneI_GPIO_Input_PullDown = (uint8_t)0x01,
    ThyoneI_GPIO_Input_PullUp = (uint8_t)0x02
} ThyoneI_GPIO_Input_t;

typedef struct ThyoneI_GPIO_PwmValue_t {
    uint16_t period; /* in ms */
    uint8_t ratio;   /* 0-255 (0%-100%)*/
} ThyoneI_GPIO_PwmValue_t;

typedef struct ThyoneI_GPIOConfigBlock_t {
    ThyoneI_GPIO_t GPIO_ID;
    ThyoneI_GPIO_IO_t function;
    union /* 3Byte */
    {
        ThyoneI_GPIO_PwmValue_t pwm;
        ThyoneI_GPIO_Input_t input;
        ThyoneI_GPIO_Output_t output;
    } value;
} ThyoneI_GPIOConfigBlock_t;

typedef struct ThyoneI_GPIOControlBlock_t {
    ThyoneI_GPIO_t GPIO_ID;
    union /* 1Byte */
    {
        ThyoneI_GPIO_Output_t output;
        uint8_t ratio; /* 0-255 (0%-100%)*/
    } value;
} ThyoneI_GPIOControlBlock_t;

typedef enum ThyoneI_ResetReason_t {
    ThyoneI_ResetReason_PowerOn_WakeUp = (uint8_t)0x01,
    ThyoneI_ResetReason_PinReset = (uint8_t)0x02,
    ThyoneI_ResetReason_SoftReset = (uint8_t)0x04,
    ThyoneI_ResetReason_Invalid = (uint8_t)0xFF,
} ThyoneI_ResetReason_t;

typedef enum ThyoneI_States_t {
    ThyoneI_State_Application = (uint8_t)0x00,
    ThyoneI_State_Test = (uint8_t)0x01,
    ThyoneI_State_Invalid = (uint8_t)0xFF,
} ThyoneI_States_t;

typedef enum ThyoneI_ModuleMode_t {
    ThyoneI_ModuleMode_Normal = (uint8_t)0,
    ThyoneI_ModuleMode_Sniffer = (uint8_t)1,
    ThyoneI_ModuleMode_Repeater = (uint8_t)2,
    ThyoneI_ModuleMode_Invalid = (uint8_t)0xFF,
} ThyoneI_ModuleMode_t;

typedef enum ThyoneI_OperationMode_t {
    ThyoneI_OperationMode_CommandMode,
    ThyoneI_OperationMode_TransparentMode
} ThyoneI_OperationMode_t;

/* user settings */
typedef enum ThyoneI_UserSettings_t {
    ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER = (uint8_t)0x01,
    ThyoneI_USERSETTING_INDEX_FW_VERSION = (uint8_t)0x02,
    ThyoneI_USERSETTING_INDEX_UART_CONFIG = (uint8_t)0x04,
    ThyoneI_USERSETTING_INDEX_RF_CHANNEL = (uint8_t)0x07,
    ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE = (uint8_t)0x08,
    ThyoneI_USERSETTING_INDEX_RF_PROFILE = (uint8_t)0x09,
    ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES = (uint8_t)0x0A,
    ThyoneI_USERSETTING_INDEX_RF_TX_POWER = (uint8_t)0x0B,
    ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS = (uint8_t)0x0C,
    ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS = (uint8_t)0x10,
    ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS = (uint8_t)0x11,
    ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID = (uint8_t)0x12,
    ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY = (uint8_t)0x14,
    ThyoneI_USERSETTING_INDEX_MAC_TLL = (uint8_t)0x15,
    ThyoneI_USERSETTING_INDEX_CCA_MODE = (uint8_t)0x16,
    ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD = (uint8_t)0x17,
    ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG = (uint8_t)0x18,
    ThyoneI_USERSETTING_INDEX_MODULE_MODE = (uint8_t)0x20,
} ThyoneI_UserSettings_t;

typedef enum ThyoneI_TXPower_t {
    ThyoneI_TXPower_8 = (int8_t)8,
    ThyoneI_TXPower_4 = (int8_t)4,
    ThyoneI_TXPower_0 = (int8_t)0,
    ThyoneI_TXPower_minus4 = (int8_t)-4,
    ThyoneI_TXPower_minus8 = (int8_t)-8,
    ThyoneI_TXPower_minus12 = (int8_t)-12,
    ThyoneI_TXPower_minus16 = (int8_t)-16,
    ThyoneI_TXPower_minus20 = (int8_t)-20,
    ThyoneI_TXPower_minus40 = (int8_t)-40,
} ThyoneI_TXPower_t;

typedef enum ThyoneI_BaudRateIndex_t {
    ThyoneI_BaudRateIndex_1200 = (uint8_t)0,
    ThyoneI_BaudRateIndex_2400 = (uint8_t)2,
    ThyoneI_BaudRateIndex_4800 = (uint8_t)4,
    ThyoneI_BaudRateIndex_9600 = (uint8_t)6,
    ThyoneI_BaudRateIndex_14400 = (uint8_t)8,
    ThyoneI_BaudRateIndex_19200 = (uint8_t)10,
    ThyoneI_BaudRateIndex_28800 = (uint8_t)12,
    ThyoneI_BaudRateIndex_38400 = (uint8_t)14,
    ThyoneI_BaudRateIndex_56000 = (uint8_t)16,
    ThyoneI_BaudRateIndex_57600 = (uint8_t)18,
    ThyoneI_BaudRateIndex_76800 = (uint8_t)20,
    ThyoneI_BaudRateIndex_115200 = (uint8_t)22,
    ThyoneI_BaudRateIndex_230400 = (uint8_t)24,
    ThyoneI_BaudRateIndex_250000 = (uint8_t)26,
    ThyoneI_BaudRateIndex_460800 = (uint8_t)28,
    ThyoneI_BaudRateIndex_921600 = (uint8_t)30,
    ThyoneI_BaudRateIndex_1000000 = (uint8_t)32
} ThyoneI_BaudRateIndex_t;

typedef enum ThyoneI_UartParity_t {
    ThyoneI_UartParity_Even,
    ThyoneI_UartParity_None
} ThyoneI_UartParity_t;

typedef enum ThyoneI_EncryptionMode_t {
    ThyoneI_EncryptionMode_Unencrypted = (uint8_t)0,
    ThyoneI_EncryptionMode_TxEncrypted =
        (uint8_t)1, /* Transmit encrypted messages */
    ThyoneI_EncryptionMode_RxEncrypted =
        (uint8_t)2, /* Receive only encrypted messages. Discard all other
                       messages*/
    ThyoneI_EncryptionMode_RxTxEncrypted =
        (uint8_t)3, /* Transmit and receive encrypted messages. Discard all
                       other messages*/
    ThyoneI_EncryptionMode_Invalid = (uint8_t)0xFF,
} ThyoneI_EncryptionMode_t;

typedef enum ThyoneI_Profile_t {
    ThyoneI_Profile_125kbit = (uint8_t)0,
    ThyoneI_Profile_500kbit = (uint8_t)1,
    ThyoneI_Profile_1000kbit = (uint8_t)2,
    ThyoneI_Profile_2000kbit = (uint8_t)3,
    ThyoneI_Profile_Invalid = (uint8_t)0xFF,
} ThyoneI_Profile_t;

typedef enum ThyoneI_AddressMode_t {
    ThyoneI_AddressMode_Broadcast = (uint8_t)0,
    ThyoneI_AddressMode_Multicast = (uint8_t)1,
    ThyoneI_AddressMode_Unicast = (uint8_t)2,
} ThyoneI_AddressMode_t;

extern bool ThyoneI_Init(ThyoneI_Pins_t *pinoutP, uint32_t baudrate,
                         WE_FlowControl_t flow_control,
                         ThyoneI_OperationMode_t opMode,
                         void (*RXcb)(uint8_t *, uint16_t, uint32_t, int8_t));
extern bool ThyoneI_Deinit(void);

extern bool ThyoneI_Reset(void);

extern bool ThyoneI_Sleep();
extern bool ThyoneI_PinWakeup();

extern bool ThyoneI_TransmitBroadcast(uint8_t *payloadP, uint16_t length);
extern bool ThyoneI_TransmitMulticast(uint8_t *payloadP, uint16_t length);
extern bool ThyoneI_TransmitUnicast(uint8_t *payloadP, uint16_t length);

extern bool ThyoneI_TransmitMulticastExtended(uint8_t groupID,
                                              uint8_t *payloadP,
                                              uint16_t length);
extern bool ThyoneI_TransmitUnicastExtended(uint32_t address, uint8_t *payloadP,
                                            uint16_t length);

/* Functions to control the GPIO feature */
extern bool ThyoneI_GPIOLocalSetConfig(ThyoneI_GPIOConfigBlock_t *configP,
                                       uint16_t number_of_configs);
extern bool ThyoneI_GPIOLocalGetConfig(ThyoneI_GPIOConfigBlock_t *configP,
                                       uint16_t *number_of_configsP);
extern bool ThyoneI_GPIOLocalWrite(ThyoneI_GPIOControlBlock_t *controlP,
                                   uint16_t number_of_controls);
extern bool ThyoneI_GPIOLocalRead(uint8_t *GPIOToReadP,
                                  uint8_t amountGPIOToRead,
                                  ThyoneI_GPIOControlBlock_t *controlP,
                                  uint16_t *number_of_controlsP);

extern bool ThyoneI_GPIORemoteSetConfig(uint32_t destAddress,
                                        ThyoneI_GPIOConfigBlock_t *configP,
                                        uint16_t number_of_configs);
extern bool ThyoneI_GPIORemoteGetConfig(uint32_t destAddress,
                                        ThyoneI_GPIOConfigBlock_t *configP,
                                        uint16_t *number_of_configsP);
extern bool ThyoneI_GPIORemoteWrite(uint32_t destAddress,
                                    ThyoneI_GPIOControlBlock_t *controlP,
                                    uint16_t number_of_controls);
extern bool ThyoneI_GPIORemoteRead(uint32_t destAddress, uint8_t *GPIOToReadP,
                                   uint8_t amountGPIOToRead,
                                   ThyoneI_GPIOControlBlock_t *controlP,
                                   uint16_t *number_of_controlsP);

/* Functions that write the non-volatile settings in the flash: After
 * modification of any non-volatile setting, the module must be reset such that
 * the update takes effect. IMPORTANT: Use only in rare cases, since flash can
 * be written to only a limited number of times.
 */
extern bool ThyoneI_FactoryReset();
extern bool ThyoneI_Set(ThyoneI_UserSettings_t userSetting, uint8_t *ValueP,
                        uint8_t length);
extern bool ThyoneI_SetBaudrateIndex(ThyoneI_BaudRateIndex_t baudrate,
                                     ThyoneI_UartParity_t parity,
                                     bool flowcontrolEnable);
extern bool ThyoneI_SetEncryptionMode(ThyoneI_EncryptionMode_t encryptionMode);
extern bool ThyoneI_SetRfProfile(ThyoneI_Profile_t profile);
extern bool ThyoneI_SetRFChannel(uint8_t channel);
extern bool ThyoneI_SetRFChannelRuntime(uint8_t channel);
extern bool ThyoneI_SetNumRetries(uint8_t numRetries);
extern bool ThyoneI_SetTXPower(ThyoneI_TXPower_t txpower);
extern bool ThyoneI_SetRpNumSlots(uint8_t numSlots);
extern bool ThyoneI_SetSourceAddress(uint32_t sourceAddress);
extern bool ThyoneI_SetDestinationAddress(uint32_t destinationAddress);
extern bool ThyoneI_SetGroupID(uint8_t groupId);
extern bool ThyoneI_SetEncryptionKey(uint8_t *keyP);
extern bool ThyoneI_SetTimeToLive(uint8_t ttl);
extern bool ThyoneI_SetCCAMode(uint8_t ccaMode);
extern bool ThyoneI_SetCCAThreshold(uint8_t ccaThreshold);
extern bool ThyoneI_SetGPIOBlockRemoteConfig(uint8_t remoteConfig);
extern bool ThyoneI_SetModuleMode(ThyoneI_ModuleMode_t moduleMode);

/* Read the non-volatile settings */
extern bool ThyoneI_Get(ThyoneI_UserSettings_t userSetting, uint8_t *ResponseP,
                        uint16_t *Response_LengthP);
extern bool ThyoneI_GetSerialNumber(uint8_t *serialNumberP);
extern bool ThyoneI_GetFWVersion(uint8_t *versionP);
extern bool ThyoneI_GetBaudrateIndex(ThyoneI_BaudRateIndex_t *baudrateP,
                                     ThyoneI_UartParity_t *parityP,
                                     bool *flowcontrolEnableP);
extern bool ThyoneI_GetEncryptionMode(
    ThyoneI_EncryptionMode_t *encryptionModeP);
extern bool ThyoneI_GetRfProfile(ThyoneI_Profile_t *profileP);
extern bool ThyoneI_GetRFChannel(uint8_t *channelP);
extern bool ThyoneI_GetNumRetries(uint8_t *numRetriesP);
extern bool ThyoneI_GetTXPower(ThyoneI_TXPower_t *txpowerP);
extern bool ThyoneI_GetRpNumSlots(uint8_t *numSlotsP);
extern bool ThyoneI_GetSourceAddress(uint32_t *sourceAddressP);
extern bool ThyoneI_GetDestinationAddress(uint32_t *destinationAddressP);
extern bool ThyoneI_GetGroupID(uint8_t *groupIdP);
extern bool ThyoneI_GetTimeToLive(uint8_t *ttlP);
extern bool ThyoneI_GetCCAMode(uint8_t *ccaModeP);
extern bool ThyoneI_GetCCAThreshold(uint8_t *ccaThresholdP);
extern bool ThyoneI_GetGPIOBlockRemoteConfig(uint8_t *remoteConfigP);
extern bool ThyoneI_GetModuleMode(ThyoneI_ModuleMode_t *moduleModeP);
extern bool ThyoneI_GetState(ThyoneI_States_t *state);

#endif  // THYONEI_H_INCLUDED

#ifdef __cplusplus
}
#endif
