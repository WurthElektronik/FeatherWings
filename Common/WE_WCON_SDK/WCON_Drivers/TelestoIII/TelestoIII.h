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
 * @brief Telesto-III driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TELESTOIII_H_INCLUDED
#define TELESTOIII_H_INCLUDED

#define TELESTOIII_BROADCASTADDRESS 0xFF
#define TELESTOIII_MAX_USERSETTING_LENGTH 4

#define TELESTOIII_DEFAULT_BAUDRATE (uint32_t)115200

    typedef struct TelestoIII_Pins_t
    {
        WE_Pin_t TelestoIII_Pin_Reset;
        WE_Pin_t TelestoIII_Pin_SleepWakeUp;
        WE_Pin_t TelestoIII_Pin_Boot;
        WE_Pin_t TelestoIII_Pin_Mode;

    } TelestoIII_Pins_t;

    typedef enum TelestoIII_AddressMode_t
    {
        TelestoIII_AddressMode_0 = 0x00,
        TelestoIII_AddressMode_1,
        TelestoIII_AddressMode_2,
        TelestoIII_AddressMode_3,
    } TelestoIII_AddressMode_t;

    typedef enum TelestoIII_UserSettings_t
    {
        TelestoIII_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
        TelestoIII_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
        TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
        TelestoIII_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
        TelestoIII_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
        TelestoIII_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
        TelestoIII_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
        TelestoIII_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
        TelestoIII_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD = 0x1A,
        TelestoIII_CMD_SETGET_OPTION_LBT_THRESHOLD = 0x1B,
        TelestoIII_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
        TelestoIII_CMD_SETGET_OPTION_FWVERSION = 0x21,
    } TelestoIII_UserSettings_t;

    /**
 * @brief Struct representing a usersetting
 */
    typedef struct TelestoIII_Configuration_t
    {
        TelestoIII_UserSettings_t usersetting;            /**< user setting */
        uint8_t value[TELESTOIII_MAX_USERSETTING_LENGTH]; /**< value */
        uint8_t value_length;                             /**< length of the value */
    } TelestoIII_Configuration_t;

    extern bool TelestoIII_Init(WE_UART_t* uartP, TelestoIII_Pins_t* pinoutP, TelestoIII_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t));
    extern bool TelestoIII_Deinit(void);

    extern bool TelestoIII_PinReset(void);
    extern bool TelestoIII_Reset(void);

    extern bool TelestoIII_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);
    extern bool TelestoIII_Transmit(uint8_t* payload, uint8_t length);
    extern bool TelestoIII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

    extern bool TelestoIII_Shutdown(void);
    extern bool TelestoIII_Standby(void);
    extern bool TelestoIII_PinWakeup(bool standby);

    /* Read the non-volatile settings */
    extern bool TelestoIII_Get(TelestoIII_UserSettings_t us, uint8_t* response, uint8_t* response_length);
    extern bool TelestoIII_GetFirmwareVersion(uint8_t* fw);
    extern bool TelestoIII_GetSerialNumber(uint8_t* sn);
    extern bool TelestoIII_GetDefaultTXPower(uint8_t* txpower);
    extern bool TelestoIII_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb);
    extern bool TelestoIII_GetSourceNetID(uint8_t* srcnetid);
    extern bool TelestoIII_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* srcaddr_msb);
    extern bool TelestoIII_GetDefaultDestNetID(uint8_t* destnetid);
    extern bool TelestoIII_GetDefaultRFChannel(uint8_t* channel);
    extern bool TelestoIII_GetDefaultRFProfile(uint8_t* profile);
    extern bool TelestoIII_GetLBTObservationPeriod(uint8_t* period);
    extern bool TelestoIII_GetLBTThreshold(int8_t* threshold);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool TelestoIII_FactoryReset(void);
    extern bool TelestoIII_Set(TelestoIII_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool TelestoIII_CheckNSet(TelestoIII_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool TelestoIII_Configure(TelestoIII_Configuration_t* config, uint8_t config_length, bool factory_reset);
    extern bool TelestoIII_SetDefaultTXPower(uint8_t txpower);
    extern bool TelestoIII_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);
    extern bool TelestoIII_SetSourceNetID(uint8_t srcnetid);
    extern bool TelestoIII_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t srcaddr_msb);
    extern bool TelestoIII_SetDefaultDestNetID(uint8_t destnetid);
    extern bool TelestoIII_SetDefaultRFChannel(uint8_t channel);
    extern bool TelestoIII_SetDefaultRFProfile(uint8_t profile);
    extern bool TelestoIII_SetLBTObservationPeriod(uint8_t period);
    extern bool TelestoIII_SetLBTThreshold(int8_t threshold);
    extern bool TelestoIII_EnableSnifferMode();

    /* Write volatile settings into RAM, these settings are lost after a reset */
    extern bool TelestoIII_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);
    extern bool TelestoIII_SetVolatile_DestNetID(uint8_t destnetid);
    extern bool TelestoIII_SetVolatile_TXPower(uint8_t power);
    extern bool TelestoIII_SetVolatile_Channel(uint8_t channel);

#endif // TELESTOIII_H_INCLUDED

#ifdef __cplusplus
}
#endif
