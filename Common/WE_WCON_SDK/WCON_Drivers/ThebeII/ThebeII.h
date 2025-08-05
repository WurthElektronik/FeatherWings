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
 * @brief Thebe-II driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef THEBEII_H_INCLUDED
#define THEBEII_H_INCLUDED

/* define product */
#define Thebe_II_2609031181000
    //#define Thebe_II_India_2609031181060

#if (defined Thebe_II_2609031181000)
/* Thebe-II (2609031181000) */
#define THEBEII_MIN_RFCHANNEL 128
#define THEBEII_MAX_RFCHANNEL 133
#elif (defined Thebe_II_India_2609031181060)
/* Thebe-II for India (2609031181060) */
#define THEBEII_MIN_RFCHANNEL 41
#define THEBEII_MAX_RFCHANNEL 79
#else
#error "no product selected"
#endif

#define THEBEII_BROADCASTADDRESS 0xFF
#define THEBEII_MAX_USERSETTING_LENGTH 4

#define THEBEII_DEFAULT_BAUDRATE (uint32_t)115200

    typedef struct ThebeII_Pins_t
    {
        WE_Pin_t ThebeII_Pin_Reset;
        WE_Pin_t ThebeII_Pin_SleepWakeUp;
        WE_Pin_t ThebeII_Pin_Boot;
        WE_Pin_t ThebeII_Pin_Mode;

    } ThebeII_Pins_t;

    typedef enum ThebeII_AddressMode_t
    {
        ThebeII_AddressMode_0 = 0x00,
        ThebeII_AddressMode_1,
        ThebeII_AddressMode_2,
        ThebeII_AddressMode_3,
    } ThebeII_AddressMode_t;

    typedef enum ThebeII_UserSettings_t
    {
        ThebeII_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
        ThebeII_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
        ThebeII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
        ThebeII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
        ThebeII_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
        ThebeII_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
        ThebeII_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
        ThebeII_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
        ThebeII_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
        ThebeII_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
        ThebeII_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
        ThebeII_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
        ThebeII_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
        ThebeII_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD = 0x1A,
        ThebeII_CMD_SETGET_OPTION_LBT_THRESHOLD = 0x1B,
        ThebeII_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
        ThebeII_CMD_SETGET_OPTION_FWVERSION = 0x21,
    } ThebeII_UserSettings_t;

    /**
 * @brief Struct representing a usersetting
 */
    typedef struct ThebeII_Configuration_t
    {
        ThebeII_UserSettings_t usersetting;            /**< user setting */
        uint8_t value[THEBEII_MAX_USERSETTING_LENGTH]; /**< value */
        uint8_t value_length;                          /**< length of the value */
    } ThebeII_Configuration_t;

    extern bool ThebeII_Init(WE_UART_t* uartP, ThebeII_Pins_t* pinoutP, ThebeII_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t));
    extern bool ThebeII_Deinit(void);

    extern bool ThebeII_PinReset(void);
    extern bool ThebeII_Reset(void);

    extern bool ThebeII_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);
    extern bool ThebeII_Transmit(uint8_t* payload, uint8_t length);
    extern bool ThebeII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool ThebeII_Shutdown(void);
    extern bool ThebeII_Standby(void);
    extern bool ThebeII_PinWakeup(bool standby);

    /* Read the non-volatile settings */
    extern bool ThebeII_Get(ThebeII_UserSettings_t us, uint8_t* response, uint8_t* response_length);
    extern bool ThebeII_GetFirmwareVersion(uint8_t* fw);
    extern bool ThebeII_GetSerialNumber(uint8_t* sn);
    extern bool ThebeII_GetDefaultTXPower(uint8_t* txpower);
    extern bool ThebeII_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb);
    extern bool ThebeII_GetSourceNetID(uint8_t* srcnetid);
    extern bool ThebeII_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* srcaddr_msb);
    extern bool ThebeII_GetDefaultDestNetID(uint8_t* destnetid);
    extern bool ThebeII_GetDefaultRFChannel(uint8_t* channel);
    extern bool ThebeII_GetDefaultRFProfile(uint8_t* profile);
    extern bool ThebeII_GetLBTObservationPeriod(uint8_t* period);
    extern bool ThebeII_GetLBTThreshold(int8_t* threshold);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool ThebeII_FactoryReset(void);
    extern bool ThebeII_Set(ThebeII_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool ThebeII_CheckNSet(ThebeII_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool ThebeII_Configure(ThebeII_Configuration_t* config, uint8_t config_length, bool factory_reset);
    extern bool ThebeII_SetDefaultTXPower(uint8_t txpower);
    extern bool ThebeII_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);
    extern bool ThebeII_SetSourceNetID(uint8_t srcnetid);
    extern bool ThebeII_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t srcaddr_msb);
    extern bool ThebeII_SetDefaultDestNetID(uint8_t destnetid);
    extern bool ThebeII_SetDefaultRFChannel(uint8_t channel);
    extern bool ThebeII_SetDefaultRFProfile(uint8_t profile);
    extern bool ThebeII_SetLBTObservationPeriod(uint8_t period);
    extern bool ThebeII_SetLBTThreshold(int8_t threshold);
    extern bool ThebeII_EnableSnifferMode();

    /* Write volatile settings into RAM, these settings are lost after a reset */
    extern bool ThebeII_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);
    extern bool ThebeII_SetVolatile_DestNetID(uint8_t destnetid);
    extern bool ThebeII_SetVolatile_TXPower(uint8_t power);
    extern bool ThebeII_SetVolatile_Channel(uint8_t channel);

    extern bool ThebeII_Ping();

#endif // THEBEII_H_INCLUDED

#ifdef __cplusplus
}
#endif
