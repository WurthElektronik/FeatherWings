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
 * @brief UserSettings commands for DaphnisI source file.
 */
#include <DaphnisI/ATCommands/ATCommon.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <DaphnisI/DaphnisI.h>
#include <global/ATCommands.h>
#include <stdio.h>
#include <string.h>

static bool DaphnisI_SetIntSetting(const char* setting, uint32_t integer, uint16_t int_flags);
static bool DaphnisI_GetIntSetting(const char* setting, void* integerP, uint16_t int_flags);

#define AT_UARTBAUDRATE "+UARTBAUDRATE"

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#define AT_MODE "+MODE"
#define AT_P2PROLE "+P2PROLE"
#define AT_P2PDCTRACKER "+P2PDCTRACKER"
#define AT_P2PDCENFORCE "+P2PDCENFORCE"
#define AT_P2PLBT "+P2PLBT"
#define AT_P2PGPIORBLOCKCFG "+P2PGPIORBLOCKCFG"
#define AT_P2PRFPROFILE "+P2PRFPROFILE"
#define AT_P2PTXPOWER "+P2PTXPOWER"
#define AT_P2PRFCHANNEL "+P2PRFCHANNEL"
#define AT_P2PMACSRCADDR "+P2PMACSRCADDR"
#define AT_P2PMACDESTADDR "+P2PMACDESTADDR"
#define AT_P2PMACGRPID "+P2PMACGRPID"
#define AT_P2PMACENCMODE "+P2PMACENCMODE"
#define AT_P2PMACENCNKEY "+P2PMACENCKEY"
#define AT_P2PMACTTL "+P2PMACTTL"
#define AT_P2PMACACK "+P2PMACACK"
#define AT_P2PRPTHRESH "+P2PRPTHRESH"
#define AT_P2PRPNUMSLOTS "+P2PRPNUMSLOTS"
#define AT_P2PCMDRESPTIMEOUT "+P2PCMDRESPTIMEOUT"

#define AT_US(USERSETTING) USERSETTING "US"
#define AT_RS(RUNTIMESETTING) RUNTIMESETTING "RS"

static bool DaphnisI_SetStringSetting(const char* setting, char* string);
static bool DaphnisI_GetStringSetting(const char* setting, char* string, uint16_t max_length);

bool DaphnisI_SetUARTBaudRateUS(DaphnisI_UARTBaudRate_t baudRate) { return DaphnisI_SetIntSetting(AT_US(AT_UARTBAUDRATE), (uint8_t)baudRate, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetUARTBaudRateUS(DaphnisI_UARTBaudRate_t* baudRateP) { return DaphnisI_GetIntSetting(AT_US(AT_UARTBAUDRATE), (uint8_t*)baudRateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetUARTBaudRateRS(DaphnisI_UARTBaudRate_t* baudRateP) { return DaphnisI_GetIntSetting(AT_RS(AT_UARTBAUDRATE), (uint8_t*)baudRateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetModeUS(DaphnisI_Mode_t mode) { return DaphnisI_SetIntSetting(AT_US(AT_MODE), (uint8_t)mode, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetModeUS(DaphnisI_Mode_t* modeP) { return DaphnisI_GetIntSetting(AT_US(AT_MODE), (uint8_t*)modeP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetModeRS(DaphnisI_Mode_t* modeP) { return DaphnisI_GetIntSetting(AT_RS(AT_MODE), (uint8_t*)modeP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PRoleUS(DaphnisI_P2P_Role_t role) { return DaphnisI_SetIntSetting(AT_US(AT_P2PROLE), (uint8_t)role, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRoleUS(DaphnisI_P2P_Role_t* roleP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PROLE), (uint8_t*)roleP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRoleRS(DaphnisI_P2P_Role_t* roleP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PROLE), (uint8_t*)roleP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PDutyCycleTrackerUS(bool dct_state) { return DaphnisI_SetIntSetting(AT_US(AT_P2PDCTRACKER), (uint8_t)dct_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PDutyCycleTrackerUS(bool* dct_stateP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PDCTRACKER), (uint8_t*)dct_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PDutyCycleTrackerRS(bool* dct_stateP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PDCTRACKER), (uint8_t*)dct_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PDutyCycleEnforceUS(bool dce_state) { return DaphnisI_SetIntSetting(AT_US(AT_P2PDCENFORCE), (uint8_t)dce_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PDutyCycleEnforceUS(bool* dce_stateP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PDCENFORCE), (uint8_t*)dce_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PDutyCycleEnforceRS(bool* dce_stateP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PDCENFORCE), (uint8_t*)dce_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PLBTUS(bool lbt_state) { return DaphnisI_SetIntSetting(AT_US(AT_P2PLBT), (uint8_t)lbt_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PLBTUS(bool* lbt_stateP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PLBT), (uint8_t*)lbt_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PLBTRS(bool* lbt_stateP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PLBT), (uint8_t*)lbt_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PGPIORemoteCfgBlockUS(bool gpio_remote_cfg_block_state) { return DaphnisI_SetIntSetting(AT_US(AT_P2PGPIORBLOCKCFG), (uint8_t)gpio_remote_cfg_block_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PGPIORemoteCfgBlockUS(bool* gpio_remote_cfg_block_stateP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PGPIORBLOCKCFG), (uint8_t*)gpio_remote_cfg_block_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PGPIORemoteCfgBlockRS(bool* gpio_remote_cfg_block_stateP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PGPIORBLOCKCFG), (uint8_t*)gpio_remote_cfg_block_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PRFProfileUS(uint8_t profile) { return DaphnisI_SetIntSetting(AT_US(AT_P2PRFPROFILE), (uint8_t)profile, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRFProfileUS(uint8_t* profileP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PRFPROFILE), (uint8_t*)profileP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRFProfileRS(uint8_t* profileP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PRFPROFILE), (uint8_t*)profileP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PTXPowerUS(uint8_t power) { return DaphnisI_SetIntSetting(AT_US(AT_P2PTXPOWER), (uint8_t)power, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PTXPowerUS(uint8_t* powerP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PTXPOWER), (uint8_t*)powerP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PTXPowerRS(uint8_t power) { return DaphnisI_SetIntSetting(AT_RS(AT_P2PTXPOWER), (uint8_t)power, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PTXPowerRS(uint8_t* powerP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PTXPOWER), (uint8_t*)powerP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PRFChannelUS(uint8_t channel) { return DaphnisI_SetIntSetting(AT_US(AT_P2PRFCHANNEL), (uint8_t)channel, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRFChannelUS(uint8_t* channelP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PRFCHANNEL), (uint8_t*)channelP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PRFChannelRS(uint8_t channel) { return DaphnisI_SetIntSetting(AT_RS(AT_P2PRFCHANNEL), (uint8_t)channel, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRFChannelRS(uint8_t* channelP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PRFCHANNEL), (uint8_t*)channelP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PMACSourceAddressUS(DaphnisI_Device_Address_t src_address)
{
    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(src_address, DAPHNISI_ADDRESS_LENGTH, src_device_address_string))
    {
        return false;
    }

    return DaphnisI_SetStringSetting(AT_US(AT_P2PMACSRCADDR), src_device_address_string);
}

bool DaphnisI_GetP2PMACSourceAddressUS(DaphnisI_Device_Address_t* src_addressP)
{
    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_GetStringSetting(AT_US(AT_P2PMACSRCADDR), src_device_address_string, sizeof(src_device_address_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), *src_addressP);
}

bool DaphnisI_GetP2PMACSourceAddressRS(DaphnisI_Device_Address_t* src_addressP)
{
    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_GetStringSetting(AT_RS(AT_P2PMACSRCADDR), src_device_address_string, sizeof(src_device_address_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), *src_addressP);
}

bool DaphnisI_SetP2PMACDestinationAddressUS(DaphnisI_Device_Address_t dest_address)
{
    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    return DaphnisI_SetStringSetting(AT_US(AT_P2PMACDESTADDR), dest_device_address_string);
}

bool DaphnisI_GetP2PMACDestinationAddressUS(DaphnisI_Device_Address_t* dest_addressP)
{
    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_GetStringSetting(AT_US(AT_P2PMACDESTADDR), dest_device_address_string, sizeof(dest_device_address_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(dest_device_address_string, strlen(dest_device_address_string), *dest_addressP);
}

bool DaphnisI_GetP2PMACDestinationAddressRS(DaphnisI_Device_Address_t* dest_addressP)
{
    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_GetStringSetting(AT_RS(AT_P2PMACDESTADDR), dest_device_address_string, sizeof(dest_device_address_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(dest_device_address_string, strlen(dest_device_address_string), *dest_addressP);
}

bool DaphnisI_SetP2PMACGroupIDUS(uint8_t group_id) { return DaphnisI_SetIntSetting(AT_US(AT_P2PMACGRPID), (uint8_t)group_id, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACGroupIDUS(uint8_t* group_idP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PMACGRPID), (uint8_t*)group_idP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACGroupIDRS(uint8_t* group_idP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PMACGRPID), (uint8_t*)group_idP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PMACEncryptionModeUS(DaphnisI_P2P_Encryption_Mode_t enc_mode) { return DaphnisI_SetIntSetting(AT_US(AT_P2PMACENCMODE), (uint8_t)enc_mode, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACEncryptionModeUS(DaphnisI_P2P_Encryption_Mode_t* enc_modeP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PMACENCMODE), (uint8_t*)enc_modeP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACEncryptionModeRS(DaphnisI_P2P_Encryption_Mode_t* enc_modeP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PMACENCMODE), (uint8_t*)enc_modeP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PMACEncryptionKeyUS(DaphnisI_P2P_Encryption_Key_t enc_key)
{
    char enc_key_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_P2P_ENCRYPTION_KEY_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(enc_key, DAPHNISI_P2P_ENCRYPTION_KEY_LENGTH, enc_key_string))
    {
        return false;
    }

    return DaphnisI_SetStringSetting(AT_US(AT_P2PMACENCNKEY), enc_key_string);
}

bool DaphnisI_isP2PMACEncryptionKeyConfiguredUS(bool* key_configuredP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PMACENCNKEY), (uint8_t*)key_configuredP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_isP2PMACEncryptionKeyConfiguredRS(bool* key_configuredP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PMACENCNKEY), (uint8_t*)key_configuredP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PMACTTLUS(uint8_t ttl) { return DaphnisI_SetIntSetting(AT_US(AT_P2PMACTTL), (uint8_t)ttl, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACTTLUS(uint8_t* ttlP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PMACTTL), (uint8_t*)ttlP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACTTLRS(uint8_t* ttlP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PMACTTL), (uint8_t*)ttlP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PMACAckUS(bool ack_state) { return DaphnisI_SetIntSetting(AT_US(AT_P2PMACACK), (uint8_t)ack_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACAckUS(bool* ack_stateP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PMACACK), (uint8_t*)ack_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PMACAckRS(bool* ack_stateP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PMACACK), (uint8_t*)ack_stateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PRPThresholdUS(int16_t threshold) { return DaphnisI_SetIntSetting(AT_US(AT_P2PRPTHRESH), (int16_t)threshold, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED); }

bool DaphnisI_GetP2PRPThresholdUS(int16_t* thresholdP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PRPTHRESH), (int16_t*)thresholdP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED); }

bool DaphnisI_GetP2PRPThresholdRS(int16_t* thresholdP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PRPTHRESH), (int16_t*)thresholdP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED); }

bool DaphnisI_SetP2PRPNumSlotsUS(uint8_t num_slots) { return DaphnisI_SetIntSetting(AT_US(AT_P2PRPNUMSLOTS), (uint8_t)num_slots, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRPNumSlotsUS(uint8_t* num_slotsP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PRPNUMSLOTS), (uint8_t*)num_slotsP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PRPNumSlotsRS(uint8_t* num_slotsP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PRPNUMSLOTS), (uint8_t*)num_slotsP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_SetP2PCMDResponseTimeoutUS(uint16_t timeout) { return DaphnisI_SetIntSetting(AT_US(AT_P2PCMDRESPTIMEOUT), (uint16_t)timeout, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PCMDResponseTimeoutUS(uint16_t* timeoutP) { return DaphnisI_GetIntSetting(AT_US(AT_P2PCMDRESPTIMEOUT), (uint16_t*)timeoutP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetP2PCMDResponseTimeoutRS(uint16_t* timeoutP) { return DaphnisI_GetIntSetting(AT_RS(AT_P2PCMDRESPTIMEOUT), (uint16_t*)timeoutP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED); }

static bool DaphnisI_SetStringSetting(const char* setting, char* string)
{
    if (string == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT");

    strcat(pRequestCommand, setting);

    strcat(pRequestCommand, "=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, string, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

static bool DaphnisI_GetStringSetting(const char* setting, char* string, uint16_t max_length)
{
    if (string == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT");

    strcat(pRequestCommand, setting);

    strcat(pRequestCommand, "=?\r\n");

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    size_t setting_length = strlen(setting);

    if (0 != strncmp(pRespondCommand, setting, setting_length))
    {
        return false;
    }

    pRespondCommand += setting_length;

    if (*pRespondCommand != ':')
    {
        return false;
    }

    pRespondCommand += 1;

    return ATCommand_GetNextArgumentString(&pRespondCommand, string, ATCOMMAND_STRING_TERMINATE, max_length);
}

#else

bool DaphnisI_SetUARTBaudRate(DaphnisI_UARTBaudRate_t baudRate) { return DaphnisI_SetIntSetting(AT_UARTBAUDRATE, (uint8_t)baudRate, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

bool DaphnisI_GetUARTBaudRate(DaphnisI_UARTBaudRate_t* baudRateP) { return DaphnisI_GetIntSetting(AT_UARTBAUDRATE, (uint8_t*)baudRateP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED); }

#endif

bool DaphnisI_ResetUserSettings()
{
    if (!DaphnisI_SendRequest("AT+RUS\r\n"))
    {
        return false;
    }
    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

static bool DaphnisI_SetIntSetting(const char* setting, uint32_t integer, uint16_t int_flags)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT");

    strcat(pRequestCommand, setting);

    strcat(pRequestCommand, "=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, integer, int_flags, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

static bool DaphnisI_GetIntSetting(const char* setting, void* integerP, uint16_t int_flags)
{
    if (integerP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT");

    strcat(pRequestCommand, setting);

    strcat(pRequestCommand, "=?\r\n");

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    size_t setting_length = strlen(setting);

    if (0 != strncmp(pRespondCommand, setting, setting_length))
    {
        return false;
    }

    pRespondCommand += setting_length;

    if (*pRespondCommand != ':')
    {
        return false;
    }

    pRespondCommand += 1;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, integerP, int_flags, ATCOMMAND_STRING_TERMINATE);
}
