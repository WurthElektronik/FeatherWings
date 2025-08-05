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
 * @brief Tarvos-E driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TARVOS_E_H_INCLUDED
#define TARVOS_E_H_INCLUDED

#define TARVOSE_BROADCASTADDRESS 0xFF
#define TARVOSE_MAX_USERSETTING_LENGTH 4

#define TARVOSE_DEFAULT_BAUDRATE (uint32_t)115200

    typedef struct TarvosE_Pins_t
    {
        WE_Pin_t TarvosE_Pin_Reset;
        WE_Pin_t TarvosE_Pin_SleepWakeUp;
        WE_Pin_t TarvosE_Pin_Boot;
        WE_Pin_t TarvosE_Pin_Mode;

    } TarvosE_Pins_t;

    typedef enum TarvosE_AddressMode_t
    {
        TarvosE_AddressMode_0 = 0x00,
        TarvosE_AddressMode_1,
        TarvosE_AddressMode_2,
        TarvosE_AddressMode_3,
    } TarvosE_AddressMode_t;

    typedef enum TarvosE_UserSettings_t
    {
        TarvosE_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
        TarvosE_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
        TarvosE_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
        TarvosE_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
        TarvosE_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
        TarvosE_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
        TarvosE_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
        TarvosE_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
        TarvosE_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
        TarvosE_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
        TarvosE_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
        TarvosE_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
        TarvosE_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
        TarvosE_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD = 0x1A,
        TarvosE_CMD_SETGET_OPTION_LBT_THRESHOLD = 0x1B,
        TarvosE_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
        TarvosE_CMD_SETGET_OPTION_FWVERSION = 0x21,
    } TarvosE_UserSettings_t;

    /**
 * @brief Struct representing a user setting.
 */
    typedef struct TarvosE_Configuration_t
    {
        TarvosE_UserSettings_t usersetting;            /**< user setting */
        uint8_t value[TARVOSE_MAX_USERSETTING_LENGTH]; /**< value */
        uint8_t value_length;                          /**< length of the value */
    } TarvosE_Configuration_t;

    extern bool TarvosE_Init(WE_UART_t* uartP, TarvosE_Pins_t* pinoutP, TarvosE_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t));
    extern bool TarvosE_Deinit(void);

    extern bool TarvosE_PinReset(void);
    extern bool TarvosE_Reset(void);

    extern bool TarvosE_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);
    extern bool TarvosE_Transmit(uint8_t* payload, uint8_t length);
    extern bool TarvosE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool TarvosE_Shutdown(void);
    extern bool TarvosE_Standby(void);
    extern bool TarvosE_PinWakeup(bool standby);

    /* Read the non-volatile settings */
    extern bool TarvosE_Get(TarvosE_UserSettings_t us, uint8_t* response, uint8_t* response_length);
    extern bool TarvosE_GetFirmwareVersion(uint8_t* fw);
    extern bool TarvosE_GetSerialNumber(uint8_t* sn);
    extern bool TarvosE_GetDefaultTXPower(uint8_t* txpower);
    extern bool TarvosE_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb);
    extern bool TarvosE_GetSourceNetID(uint8_t* srcnetid);
    extern bool TarvosE_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* srcaddr_msb);
    extern bool TarvosE_GetDefaultDestNetID(uint8_t* destnetid);
    extern bool TarvosE_GetDefaultRFChannel(uint8_t* channel);
    extern bool TarvosE_GetDefaultRFProfile(uint8_t* profile);
    extern bool TarvosE_GetLBTObservationPeriod(uint8_t* period);
    extern bool TarvosE_GetLBTThreshold(int8_t* threshold);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool TarvosE_FactoryReset(void);
    extern bool TarvosE_Set(TarvosE_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool TarvosE_CheckNSet(TarvosE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);
    extern bool TarvosE_Configure(TarvosE_Configuration_t* config, uint8_t config_length, bool factory_reset);
    extern bool TarvosE_SetDefaultTXPower(uint8_t txpower);
    extern bool TarvosE_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);
    extern bool TarvosE_SetSourceNetID(uint8_t srcnetid);
    extern bool TarvosE_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t srcaddr_msb);
    extern bool TarvosE_SetDefaultDestNetID(uint8_t destnetid);
    extern bool TarvosE_SetDefaultRFChannel(uint8_t channel);
    extern bool TarvosE_SetDefaultRFProfile(uint8_t profile);
    extern bool TarvosE_SetLBTObservationPeriod(uint8_t period);
    extern bool TarvosE_SetLBTThreshold(int8_t threshold);
    extern bool TarvosE_EnableSnifferMode();

    /* write volatile settings into RAM, these settings are lost after a reset */
    extern bool TarvosE_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);
    extern bool TarvosE_SetVolatile_DestNetID(uint8_t destnetid);
    extern bool TarvosE_SetVolatile_TXPower(uint8_t power);
    extern bool TarvosE_SetVolatile_Channel(uint8_t channel);

    extern bool TarvosE_Ping();

#endif // TARVOS_E_H_INCLUDED

#ifdef __cplusplus
}
#endif
