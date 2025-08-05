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
 * @brief UserSettings commands for DaphnisI header file.
 */

#ifndef DAPHNIS_COMMANDS_USERSETTINGS_H_
#define DAPHNIS_COMMANDS_USERSETTINGS_H_

#include <DaphnisI/DaphnisI.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum DaphnisI_UARTBaudRate_t
    {
        DaphnisI_UARTBaudRate_9600 = 0x0,
        DaphnisI_UARTBaudRate_57600 = 0x1,
        DaphnisI_UARTBaudRate_115200 = 0x2,
        DaphnisI_UARTBaudRate_Count
    } DaphnisI_UARTBaudRate_t;

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

    typedef enum DaphnisI_Mode_t
    {
        DaphnisI_Mode_Lorawan = 0x0,
        DaphnisI_Mode_P2P = 0x1
    } DaphnisI_Mode_t;

    typedef enum DaphnisI_P2P_Role_t
    {
        DaphnisI_P2P_Role_Transceiver = 0x0,
        DaphnisI_P2P_Role_Repeater = 0x1
    } DaphnisI_P2P_Role_t;

    typedef enum DaphnisI_P2P_Encryption_Mode_t
    {
        DaphnisI_P2P_Encryption_Mode_None = 0x0,
        DaphnisI_P2P_Encryption_Mode_TX = 0x1,
        DaphnisI_P2P_Encryption_Mode_RX = 0x2,
        DaphnisI_P2P_Encryption_Mode = 0x3,
    } DaphnisI_P2P_Encryption_Mode_t;

#define DAPHNISI_P2P_ENCRYPTION_KEY_LENGTH 16

    typedef uint8_t DaphnisI_P2P_Encryption_Key_t[DAPHNISI_P2P_ENCRYPTION_KEY_LENGTH];

    /**
 * @brief Set the UART baudrate UserSetting (using the AT+UARTBAUDRATEUS command)
 *
 * @param[in] baudRate: Baudrate enum. See DaphnisI_UARTBaudRate_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetUARTBaudRateUS(DaphnisI_UARTBaudRate_t baudRate);

    /**
 * @brief Get the UART baudrate UserSetting (using the AT+UARTBAUDRATEUS command)
 *
 * @param[out] baudRateP: Pointer to the Baudrate enum. See DaphnisI_UARTBaudRate_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetUARTBaudRateUS(DaphnisI_UARTBaudRate_t* baudRateP);

    /**
 * @brief Get the UART baudrate RunTimeSetting (using the AT+UARTBAUDRATERS command)
 *
 * @param[out] baudRateP: Pointer to the Baudrate enum. See DaphnisI_UARTBaudRate_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetUARTBaudRateRS(DaphnisI_UARTBaudRate_t* baudRateP);

    /**
 * @brief Set the operation mode UserSetting (using the AT+MODEUS command)
 *
 * @param[in] mode: Operation mode enum. See DaphnisI_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetModeUS(DaphnisI_Mode_t mode);

    /**
 * @brief Get the operation mode UserSetting (using the AT+MODEUS command)
 *
 * @param[out] modeP: Pointer to operation mode enum. See DaphnisI_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetModeUS(DaphnisI_Mode_t* modeP);

    /**
 * @brief Get the operation mode RunTimeSetting (using the AT+MODERS command)
 *
 * @param[out] modeP: Pointer to operation mode enum. See DaphnisI_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetModeRS(DaphnisI_Mode_t* modeP);

    /**
 * @brief Set the P2P role UserSetting (using the AT+P2PROLEUS command)
 *
 * @param[in] role: P2P role enum. See DaphnisI_P2P_Role_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRoleUS(DaphnisI_P2P_Role_t role);

    /**
 * @brief Get the P2P role UserSetting (using the AT+P2PROLEUS command)
 *
 * @param[out] roleP: Pointer to P2P role enum. See DaphnisI_P2P_Role_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRoleUS(DaphnisI_P2P_Role_t* roleP);

    /**
 * @brief Get the P2P role RunTimeSetting (using the AT+P2PROLERS command)
 *
 * @param[out] roleP: Pointer to P2P role enum. See DaphnisI_P2P_Role_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRoleRS(DaphnisI_P2P_Role_t* roleP);

    /**
 * @brief Set the P2P duty cycle tracker state UserSetting (using the AT+P2PDCTRACKERUS command)
 *
 * @param[in] dct_state: Duty cycle tracker state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PDutyCycleTrackerUS(bool dct_state);

    /**
 * @brief Get the P2P duty cycle tracker state UserSetting (using the AT+P2PDCTRACKERUS command)
 *
 * @param[out] dct_stateP: Pointer to duty cycle tracker state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PDutyCycleTrackerUS(bool* dct_stateP);

    /**
 * @brief Get the P2P duty cycle tracker state RunTimeSetting (using the AT+P2PDCTRACKERRS command)
 *
 * @param[out] dct_stateP: Pointer to duty cycle tracker state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PDutyCycleTrackerRS(bool* dct_stateP);

    /**
 * @brief Set the P2P duty cycle enforce state UserSetting (using the AT+P2PDCENFORCEUS command)
 *
 * @param[in] dce_state: Duty cycle enforce state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PDutyCycleEnforceUS(bool dce_state);

    /**
 * @brief Get the P2P duty cycle enforce state UserSetting (using the AT+P2PDCENFORCEUS command)
 *
 * @param[out] dce_stateP: Pointer to duty cycle enforce state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PDutyCycleEnforceUS(bool* dce_stateP);

    /**
 * @brief Get the P2P duty cycle enforce state RunTimeSetting (using the AT+P2PDCENFORCERS command)
 *
 * @param[out] dce_stateP: Pointer to duty cycle enforce state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PDutyCycleEnforceRS(bool* dce_stateP);

    /**
 * @brief Set the P2P listen before talk state UserSetting (using the AT+P2PLBTUS command)
 *
 * @param[in] lbt_state: Listen before talk state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PLBTUS(bool lbt_state);

    /**
 * @brief Get the P2P listen before talk state UserSetting (using the AT+P2PLBTUS command)
 *
 * @param[out] lbt_stateP: Pointer to listen before talk state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PLBTUS(bool* lbt_stateP);

    /**
 * @brief Get the P2P listen before talk state RunTimeSetting (using the AT+P2PLBTRS command)
 *
 * @param[out] lbt_stateP: Pointer to listen before talk state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PLBTRS(bool* lbt_stateP);

    /**
 * @brief Set the P2P GPIO remote cfg block state UserSetting (using the AT+P2PGPIORBLOCKCFGUS command)
 *
 * @param[in] gpio_remote_cfg_block_state: GPIO remote cfg block.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PGPIORemoteCfgBlockUS(bool gpio_remote_cfg_block_state);

    /**
 * @brief Get the P2P GPIO remote cfg lock state UserSetting (using the AT+P2PGPIORBLOCKCFGUS command)
 *
 * @param[out] ack_stateP: Pointer to GPIO remote cfg block state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PGPIORemoteCfgBlockUS(bool* gpio_remote_cfg_block_stateP);

    /**
 * @brief Get the P2P GPIO remote cfg lock state RunTimeSetting (using the AT+P2PGPIORBLOCKCFGRS command)
 *
 * @param[out] ack_stateP: Pointer to GPIO remote cfg block state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PGPIORemoteCfgBlockRS(bool* gpio_remote_cfg_block_stateP);

    /**
 * @brief Set the P2P RF profile UserSetting (using the AT+P2PRFPROFILEUS command)
 *
 * @param[in] profile: Profile.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRFProfileUS(uint8_t profile);

    /**
 * @brief Get the P2P rf profile UserSetting (using the AT+P2PRFPROFILEUS command)
 *
 * @param[out] profileP: Pointer to profile.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRFProfileUS(uint8_t* profileP);

    /**
 * @brief Get the P2P rf profile RunTimeSetting (using the AT+P2PRFPROFILERS command)
 *
 * @param[out] profileP: Pointer to profile.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRFProfileRS(uint8_t* profileP);

    /**
 * @brief Set the P2P tx power UserSetting (using the AT+P2PTXPOWERUS command)
 *
 * @param[in] power: Power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PTXPowerUS(uint8_t power);

    /**
 * @brief Get the P2P tx power UserSetting (using the AT+P2PTXPOWERUS command)
 *
 * @param[out] powerP: Pointer to power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PTXPowerUS(uint8_t* powerP);

    /**
 * @brief Set the P2P tx power RunTimeSetting (using the AT+P2PTXPOWERRS command)
 *
 * @param[in] power: Power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PTXPowerRS(uint8_t power);

    /**
 * @brief Get the P2P tx power RunTimeSetting (using the AT+P2PTXPOWERRS command)
 *
 * @param[out] powerP: Pointer to power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PTXPowerRS(uint8_t* powerP);

    /**
 * @brief Set the P2P rf channel UserSetting (using the AT+P2PRFCHANNELUS command)
 *
 * @param[in] channel: Channel.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRFChannelUS(uint8_t channel);

    /**
 * @brief Get the P2P rf channel UserSetting (using the AT+P2PRFCHANNELUS command)
 *
 * @param[out] channelP: Pointer to Channel.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRFChannelUS(uint8_t* channelP);

    /**
 * @brief Set the P2P rf channel RunTimeSetting (using the AT+P2PRFCHANNELRS command)
 *
 * @param[in] channel: Channel.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRFChannelRS(uint8_t channel);

    /**
 * @brief Get the P2P rf channel RunTimeSetting (using the AT+P2PRFCHANNELRS command)
 *
 * @param[out] channelP: Pointer to Channel.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRFChannelRS(uint8_t* channelP);

    /**
 * @brief Set the P2P MAC source address UserSetting (using the AT+P2PMACSRCADDRUS command)
 *
 * @param[in] src_address: Source address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACSourceAddressUS(DaphnisI_Device_Address_t src_address);

    /**
 * @brief Get the P2P MAC source address UserSetting (using the AT+P2PMACSRCADDRUS command)
 *
 * @param[out] src_addressP: Pointer to source address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACSourceAddressUS(DaphnisI_Device_Address_t* src_addressP);

    /**
 * @brief Get the P2P MAC source address RunTimeSetting (using the AT+P2PMACSRCADDRRS command)
 *
 * @param[out] src_addressP: Pointer to source address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACSourceAddressRS(DaphnisI_Device_Address_t* src_addressP);

    /**
 * @brief Set the P2P MAC destination address UserSetting (using the AT+P2PMACDESTADDRUS command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACDestinationAddressUS(DaphnisI_Device_Address_t dest_address);

    /**
 * @brief Get the P2P MAC destination address UserSetting (using the AT+P2PMACDESTADDRUS command)
 *
 * @param[out] dest_addressP: Pointer to destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACDestinationAddressUS(DaphnisI_Device_Address_t* dest_addressP);

    /**
 * @brief Get the P2P MAC destination address RunTimeSetting (using the AT+P2PMACDESTADDRRS command)
 *
 * @param[out] dest_addressP: Pointer to destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACDestinationAddressRS(DaphnisI_Device_Address_t* dest_addressP);

    /**
 * @brief Set the P2P MAC group id UserSetting (using the AT+P2PMACGRPIDUS command)
 *
 * @param[in] group_id: Group ID.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACGroupIDUS(uint8_t group_id);

    /**
 * @brief Get the P2P MAC group id UserSetting (using the AT+P2PMACGRPIDUS command)
 *
 * @param[out] group_idP: Pointer to group ID.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACGroupIDUS(uint8_t* group_idP);

    /**
 * @brief Get the P2P MAC group id RunTimeSetting (using the AT+P2PMACGRPIDRS command)
 *
 * @param[out] group_idP: Pointer to group ID.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACGroupIDRS(uint8_t* group_idP);

    /**
 * @brief Set the MAC encryption mode UserSetting (using the AT+P2PMACENCMODEUS command)
 *
 * @param[in] enc_mode: Encryption mode enum. See DaphnisI_P2P_Encryption_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACEncryptionModeUS(DaphnisI_P2P_Encryption_Mode_t enc_mode);

    /**
 * @brief Get the MAC encryption mode UserSetting (using the AT+P2PMACENCMODEUS command)
 *
 * @param[out] enc_modeP: Pointer to encryption mode enum. See DaphnisI_P2P_Encryption_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACEncryptionModeUS(DaphnisI_P2P_Encryption_Mode_t* enc_modeP);

    /**
 * @brief Get the MAC encryption mode RunTimeSetting (using the AT+P2PMACENCMODERS command)
 *
 * @param[out] enc_modeP: Pointer to encryption mode enum. See DaphnisI_P2P_Encryption_Mode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACEncryptionModeRS(DaphnisI_P2P_Encryption_Mode_t* enc_modeP);

    /**
 * @brief Set the P2P MAC encryption key UserSetting (using the AT+P2PMACENCKEYUS command)
 *
 * @param[in] enc_key: Encryption key (LSB First). See DaphnisI_P2P_Encryption_Key_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACEncryptionKeyUS(DaphnisI_P2P_Encryption_Key_t enc_key);

    /**
 * @brief Get the P2P MAC encryption key state UserSetting (using the AT+P2PMACENCKEYUS command)
 *
 * @param[out] key_configuredP: Pointer to key configuration state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_isP2PMACEncryptionKeyConfiguredUS(bool* key_configuredP);

    /**
 * @brief Get the P2P MAC encryption key state RunTimeSetting (using the AT+P2PMACENCKEYRS command)
 *
 * @param[out] key_configuredP: Pointer to key configuration state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_isP2PMACEncryptionKeyConfiguredRS(bool* key_configuredP);

    /**
 * @brief Set the P2P MAC time to live UserSetting (using the AT+P2PMACTTLUS command)
 *
 * @param[in] ttl: Time to live.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACTTLUS(uint8_t ttl);

    /**
 * @brief Get the P2P MAC time to live UserSetting (using the AT+P2PMACTTLUS command)
 *
 * @param[out] ttlP: Pointer to time to live.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACTTLUS(uint8_t* ttlP);

    /**
 * @brief Get the P2P MAC time to live RunTimeSetting (using the AT+P2PMACTTLRS command)
 *
 * @param[out] ttlP: Pointer to time to live.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACTTLRS(uint8_t* ttlP);

    /**
 * @brief Set the P2P MAC Acknowledgments state UserSetting (using the AT+P2PMACACKUS command)
 *
 * @param[in] ack_state: MAC ACK state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PMACAckUS(bool ack_state);

    /**
 * @brief Get the P2P MAC Acknowledgments state UserSetting (using the AT+P2PMACACKUS command)
 *
 * @param[out] ack_stateP: Pointer to ack state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACAckUS(bool* ack_stateP);

    /**
 * @brief Get the P2P MAC Acknowledgments state RunTimeSetting (using the AT+P2PMACACKRS command)
 *
 * @param[out] ack_stateP: Pointer to ack state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PMACAckRS(bool* ack_stateP);

    /**
 * @brief Set the P2P repeater threshold UserSetting (using the AT+P2PRPTHRESHUS command)
 *
 * @param[in] threshold: Threshold value in dBm.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRPThresholdUS(int16_t threshold);

    /**
 * @brief Get the P2P repeater threshold UserSetting (using the AT+P2PRPTHRESHUS command)
 *
 * @param[out] thresholdP: Pointer to threshold value in dBm.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRPThresholdUS(int16_t* thresholdP);

    /**
 * @brief Get the P2P repeater threshold RunTimeSetting (using the AT+P2PRPTHRESHRS command)
 *
 * @param[out] thresholdP: Pointer to threshold value in dBm.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRPThresholdRS(int16_t* thresholdP);

    /**
 * @brief Set the P2P repeater number of slots UserSetting (using the AT+P2PRPNUMSLOTSUS command)
 *
 * @param[in] num_slots: Number of repeater slots.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PRPNumSlotsUS(uint8_t num_slots);

    /**
 * @brief Get the P2P repeater number of slots UserSetting (using the AT+P2PRPNUMSLOTSUS command)
 *
 * @param[out] num_slotsP: Pointer to number of repeater slots.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRPNumSlotsUS(uint8_t* num_slotsP);

    /**
 * @brief Get the P2P repeater number of slots UserSetting (using the AT+P2PRPNUMSLOTSRS command)
 *
 * @param[out] num_slotsP: Pointer to number of repeater slots.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PRPNumSlotsRS(uint8_t* num_slotsP);

    /**
 * @brief Set the P2P CMD response timeout UserSetting (using the AT+P2PCMDRESPTIMEOUTUS command)
 *
 * @param[in] timeout: timeout value in ms.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetP2PCMDResponseTimeoutUS(uint16_t timeout);

    /**
 * @brief Get the P2P CMD response timeout UserSetting (using the AT+P2PCMDRESPTIMEOUTUS command)
 *
 * @param[out] timeoutP: Pointer to timeout value in ms.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PCMDResponseTimeoutUS(uint16_t* timeoutP);

    /**
 * @brief Get the P2P CMD response timeout RunTimeSetting (using the AT+P2PCMDRESPTIMEOUTRS command)
 *
 * @param[out] timeoutP: Pointer to timeout value in ms.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetP2PCMDResponseTimeoutRS(uint16_t* timeoutP);

#else /* DAPHNISI_MIN_FW_VER */

/**
 * @brief Sets the UART baud rate (using the AT+UARTBAUDRATE command)
 *
 * @param[in] baudRate: baud rate. (Refer to DaphnisI_UARTBaudRate_t)
 *
 * @return true if successful, false otherwise
 */
extern bool DaphnisI_SetUARTBaudRate(DaphnisI_UARTBaudRate_t baudRate);

/**
 * @brief gets the UART baud rate (using the AT+UARTBAUDRATE command)
 *
 * @param[out] baudRateP: Pointer to the baud rate. (Refer to DaphnisI_UARTBaudRate_t)
 *
 * @return true if successful, false otherwise
 */
extern bool DaphnisI_GetUARTBaudRate(DaphnisI_UARTBaudRate_t* baudRateP);

#endif /* DAPHNISI_MIN_FW_VER */

    /**
 * @brief Sleep device (using the AT+RUS command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_ResetUserSettings();

#ifdef __cplusplus
}
#endif

#endif /* DAPHNIS_COMMANDS_USERSETTINGS_H_ */
