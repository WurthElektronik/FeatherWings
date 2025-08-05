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
 * @brief GPIO Commands for DaphnisI header file.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DAPHNIS_COMMANDS_GPIO_H_
#define DAPHNIS_COMMANDS_GPIO_H_

#include <DaphnisI/DaphnisI.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
    extern "C"
    {
#endif

        typedef enum DaphnisI_GPIO_t
        {
            DaphnisI_GPIO_0 = (uint8_t)0x00,
            DaphnisI_GPIO_1 = (uint8_t)0x01,
            DaphnisI_GPIO_2 = (uint8_t)0x02,
            DaphnisI_GPIO_3 = (uint8_t)0x03,
            DaphnisI_GPIO_4 = (uint8_t)0x04,
            DaphnisI_GPIO_5 = (uint8_t)0x05,
            DaphnisI_GPIO_6 = (uint8_t)0x06,
            DaphnisI_GPIO_7 = (uint8_t)0x07,
            DaphnisI_GPIO_Count
        } DaphnisI_GPIO_t;

        typedef enum DaphnisI_GPIO_IO_t
        {
            DaphnisI_GPIO_IO_Disconnected = (uint8_t)0x00,
            DaphnisI_GPIO_IO_Input = (uint8_t)0x01,
            DaphnisI_GPIO_IO_Output = (uint8_t)0x02,
        } DaphnisI_GPIO_IO_t;

        typedef enum DaphnisI_GPIO_Output_t
        {
            DaphnisI_GPIO_Output_Low = (uint8_t)0x00,
            DaphnisI_GPIO_Output_High = (uint8_t)0x01
        } DaphnisI_GPIO_Output_t;

        typedef enum DaphnisI_GPIO_Input_t
        {
            DaphnisI_GPIO_Input_NoPull = (uint8_t)0x00,
            DaphnisI_GPIO_Input_PullDown = (uint8_t)0x01,
            DaphnisI_GPIO_Input_PullUp = (uint8_t)0x02
        } DaphnisI_GPIO_Input_t;

        typedef enum DaphnisI_GPIO_Block_Status_t
        {
            DaphnisI_GPIO_Block_Status_Fail = (uint8_t)0x00,
            DaphnisI_GPIO_Block_Status_Success = (uint8_t)0x01,
        } DaphnisI_GPIO_Block_Status_t;

        typedef enum DaphnisI_GPIO_Command_Status_t
        {
            DaphnisI_GPIO_Command_Status_Fail = (uint8_t)0x00,
            DaphnisI_GPIO_Command_Status_Success = (uint8_t)0x01,
            DaphnisI_GPIO_Command_Status_Block = (uint8_t)0x02
        } DaphnisI_GPIO_Command_Status_t;

        typedef struct DaphnisI_GPIOConfigBlock_t
        {
            DaphnisI_GPIO_t GPIO_ID;
            DaphnisI_GPIO_IO_t function;
            union
            {
                DaphnisI_GPIO_Input_t input;
                DaphnisI_GPIO_Output_t output;
            } value;
        } DaphnisI_GPIOConfigBlock_t;

        typedef struct DaphnisI_GPIOValueSetBlock_t
        {
            DaphnisI_GPIO_t GPIO_ID;
            DaphnisI_GPIO_Output_t value;
        } DaphnisI_GPIOValueSetBlock_t;

        typedef struct DaphnisI_GPIOValueGetBlock_t
        {
            DaphnisI_GPIO_t GPIO_ID;
            DaphnisI_GPIO_Block_Status_t status;
            DaphnisI_GPIO_Output_t value;
        } DaphnisI_GPIOValueGetBlock_t;

        typedef struct DaphnisI_GPIOStatusBlock_t
        {
            DaphnisI_GPIO_t GPIO_ID;
            DaphnisI_GPIO_Block_Status_t status;
        } DaphnisI_GPIOStatusBlock_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteCfgChangedData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIOConfigBlock_t* configBlocksP;
            uint8_t configBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteCfgChangedData_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteValueChangedData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP;
            uint8_t valueGetBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteValueChangedData_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIO_Command_Status_t command_status;
            DaphnisI_GPIOStatusBlock_t* statusBlocksP;
            uint8_t statusBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIO_Command_Status_t command_status;
            DaphnisI_GPIOConfigBlock_t* configBlocksP;
            uint8_t configBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIO_Command_Status_t command_status;
            DaphnisI_GPIOStatusBlock_t* statusBlocksP;
            uint8_t statusBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t;

        typedef struct DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t
        {
            DaphnisI_Device_Address_t source_address;
            int16_t RSSI;
            DaphnisI_GPIO_Command_Status_t command_status;
            DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP;
            uint8_t valueGetBlocksCount;
        } DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t;

        /**
 * @brief Set the configurations of local GPIO pins (using the AT+GPIOLCFGSET command)
 *
 * @param[in] configBlocksP: Pointer to the configuration blocks. See DaphnisI_GPIOConfigBlock_t.
 *
 * @param[in] configBlocksCount: Number of configuration blocks.
 *
 * @param[out] statusBlocksP: Pointer to the status blocks (The number of status blocks equals the number of configuration blocks). See DaphnisI_GPIOStatusBlock_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_GPIO_Local_Configuration_Set(DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount, DaphnisI_GPIOStatusBlock_t* statusBlocksP);

        /**
 * @brief Get the configurations of local GPIO pins (using the AT+GPIOLCFGGET command)
 *
 * @param[out] configBlocksP: Pointer to the configuration blocks. See DaphnisI_GPIOConfigBlock_t.
 *
 * @param[out] configBlocksCountP: Pointer to number of configuration blocks.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_GPIO_Local_Configuration_Get(DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t* configBlocksCountP);

        /**
 * @brief Set the values of local GPIO pins (using the AT+GPIOLVALUESET command)
 *
 * @param[in] valueSetBlocksP: Pointer to the value set blocks. See DaphnisI_GPIOValueSetBlock_t.
 *
 * @param[in] valueSetBlocksCount: Number of value set blocks.
 *
 * @param[out] statusBlocksP: Pointer to the status blocks (The number of status blocks equals the number of value set blocks). See DaphnisI_GPIOStatusBlock_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_GPIO_Local_Value_Set(DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount, DaphnisI_GPIOStatusBlock_t* statusBlocksP);

        /**
 * @brief Get the values of local GPIO pins (using the AT+GPIOLVALUEGET command)
 *
 * @param[in] gpioToReadP: Pointer to the GPIOs IDs. See DaphnisI_GPIO_t.
 *
 * @param[in] gpioToReadCount: Number of GPIO IDs.
 *
 * @param[out] valueGetBlocksP: Pointer to the value get blocks (The number of value get blocks equals the number of GPIO IDs). See DaphnisI_GPIOValueGetBlock_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_GPIO_Local_Value_Get(DaphnisI_GPIO_t* gpioToReadP, uint8_t gpioToReadCount, DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP);

        /**
 * @brief Set the configurations of remote GPIO pins of the destination address (using the AT+P2PGPIORCFGSET command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @param[in] configBlocksP: Pointer to the configuration blocks. See DaphnisI_GPIOConfigBlock_t.
 *
 * @param[in] configBlocksCount: Number of configuration blocks.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_Configuration_Set(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount);

        /**
 * @brief Get the configurations of remote GPIO pins of the destination address (using the AT+P2PGPIORCFGGET command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_Configuration_Get(DaphnisI_Device_Address_t dest_address);

        /**
 * @brief Set the values of remote GPIO pins of the destination address (using the AT+P2PGPIORVALUESET command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @param[in] valueSetBlocksP: Pointer to the value set blocks. See DaphnisI_GPIOValueSetBlock_t.
 *
 * @param[in] valueSetBlocksCount: Number of value set blocks.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_Value_Set(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount);

        /**
 * @brief Get the values of remote GPIO pins of the destination address (using the AT+P2PGPIORVALUEGET command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @param[in] gpioToReadP: Pointer to the GPIOs IDs. See DaphnisI_GPIO_t.
 *
 * @param[in] gpioToReadCount: Number of GPIO IDs.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_Value_Get(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIO_t* gpioToReadP, uint8_t gpioToReadCount);

        /**
 * @brief Parse the value of P2P remote GPIO configuration changed event (using the +P2PGPIORCFGCHANGED event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] cfgChangedP: Pointer to remote GPIO configuration changed struct. See DaphnisI_P2P_GPIO_RemoteCfgChangedData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseCfgChangedEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgChangedData_t* cfgChangedP);

        /**
 * @brief Parse the value of P2P remote GPIO value changed event (using the +P2PGPIORVALUECHANGED event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] valueChangedP: Pointer to remote GPIO value changed struct. See DaphnisI_P2P_GPIO_RemoteValueChangedData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseValueChangedEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueChangedData_t* valueChangedP);

        /**
 * @brief Parse the value of P2P remote GPIO set configuration response event (using the +P2PGPIORCFGSET event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] cfgSetResponseP: Pointer to remote GPIO configuration set response struct. See DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseCfgSetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t* cfgSetResponseP);

        /**
 * @brief Parse the value of P2P remote GPIO get configuration response event (using the +P2PGPIORCFGSET event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] cfgGetResponseP: Pointer to remote GPIO configuration get response struct. See DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseCfgGetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t* cfgGetResponseP);

        /**
 * @brief Parse the value of P2P remote GPIO set value response event (using the +P2PGPIORVALUESET event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] valueSetResponseP: Pointer to remote GPIO value set response struct. See DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t* valueSetResponseP);

        /**
 * @brief Parse the value of P2P remote GPIO get value response event (using the +P2PGPIORVALUEGET event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] valueGetResponseP: Pointer to remote GPIO value get response struct. See DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t.
 *
 * @return true if successful, false otherwise
 */
        extern bool DaphnisI_P2P_GPIO_Remote_ParseValueGetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t* valueGetResponseP);

#ifdef __cplusplus
    }
#endif

#endif /* DAPHNISI_MIN_FW_VER */

#endif /* DAPHNIS_COMMANDS_GPIO_H_ */
