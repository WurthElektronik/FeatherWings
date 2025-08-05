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
 * @brief GPIO commands for DaphnisI source file.
 */

#include <DaphnisI/ATCommands/ATGPIO.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#include <DaphnisI/ATCommands/ATCommon.h>
#include <global/ATCommands.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool DaphnisI_GPIO_ParseConfigBlocks(char* string, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t* configBlocksCountP);
static bool DaphnisI_GPIO_ParseValueGetBlocks(char* string, DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP, uint8_t* valueGetBlocksCountP);
static bool DaphnisI_GPIO_ParseStatusBlocks(char* string, DaphnisI_GPIOStatusBlock_t* statusBlocksP, uint8_t* statusBlocksCountP);
static bool DaphnisI_GPIO_CreateConfigBlocksString(char* string, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount);
static bool DaphnisI_GPIO_CreateValueSetBlocksString(char* string, DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount);

#define GPIO_PIN_DELIMITER ':'

bool DaphnisI_GPIO_Local_Configuration_Set(DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount, DaphnisI_GPIOStatusBlock_t* statusBlocksP)
{
    if ((configBlocksP == NULL) || (configBlocksCount == 0))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+GPIOLCFGSET=");

    if (!DaphnisI_GPIO_CreateConfigBlocksString(pRequestCommand, configBlocksP, configBlocksCount))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    if (statusBlocksP == NULL)
    {
        return true;
    }

    const char* cmd = "+GPIOLCFGSET:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    uint8_t statusBlocksCount;

    return DaphnisI_GPIO_ParseStatusBlocks(pRespondCommand, statusBlocksP, &statusBlocksCount);
}

bool DaphnisI_GPIO_Local_Configuration_Get(DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t* configBlocksCountP)
{
    if ((configBlocksP == NULL) || (configBlocksCountP == NULL))
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+GPIOLCFGGET=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+GPIOLCFGGET:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return DaphnisI_GPIO_ParseConfigBlocks(pRespondCommand, configBlocksP, configBlocksCountP);
}

bool DaphnisI_GPIO_Local_Value_Set(DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount, DaphnisI_GPIOStatusBlock_t* statusBlocksP)
{
    if ((valueSetBlocksP == NULL) || (valueSetBlocksCount == 0))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+GPIOLVALUESET=");

    if (!DaphnisI_GPIO_CreateValueSetBlocksString(pRequestCommand, valueSetBlocksP, valueSetBlocksCount))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    if (statusBlocksP == NULL)
    {
        return true;
    }

    const char* cmd = "+GPIOLVALUESET:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    uint8_t statusBlocksCount;

    return DaphnisI_GPIO_ParseStatusBlocks(pRespondCommand, statusBlocksP, &statusBlocksCount);
}

bool DaphnisI_GPIO_Local_Value_Get(DaphnisI_GPIO_t* gpioToReadP, uint8_t gpioToReadCount, DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP)
{
    if ((gpioToReadP == NULL) || (valueGetBlocksP == NULL))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+GPIOLVALUEGET=");

    for (uint8_t i = 0; i < gpioToReadCount; i++)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, gpioToReadP[i], (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        if (i != (gpioToReadCount - 1))
        {
            if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+GPIOLVALUEGET:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    uint8_t valueGetBlocksCount;

    return DaphnisI_GPIO_ParseValueGetBlocks(pRespondCommand, valueGetBlocksP, &valueGetBlocksCount);
}

bool DaphnisI_P2P_GPIO_Remote_Configuration_Set(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount)
{
    if ((configBlocksP == NULL) || (configBlocksCount == 0))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PGPIORCFGSET=");

    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, dest_device_address_string, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!DaphnisI_GPIO_CreateConfigBlocksString(pRequestCommand, configBlocksP, configBlocksCount))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t txConfEvents[] = {DaphnisI_ATEvent_P2P_TxConf_Success, DaphnisI_ATEvent_P2P_TxConf_Error, DaphnisI_ATEvent_P2P_TxConf_Busy};

    if (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txConfEvents, sizeof(txConfEvents) / sizeof(DaphnisI_ATEvent_t), true) != DaphnisI_ATEvent_P2P_TxConf_Success)
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Remote_GPIO), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_GPIO_Remote_Configuration_Get(DaphnisI_Device_Address_t dest_address)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PGPIORCFGGET=");

    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, dest_device_address_string, ATCOMMAND_STRING_TERMINATE))
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

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t txConfEvents[] = {DaphnisI_ATEvent_P2P_TxConf_Success, DaphnisI_ATEvent_P2P_TxConf_Error, DaphnisI_ATEvent_P2P_TxConf_Busy};

    if (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txConfEvents, sizeof(txConfEvents) / sizeof(DaphnisI_ATEvent_t), true) != DaphnisI_ATEvent_P2P_TxConf_Success)
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Remote_GPIO), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_GPIO_Remote_Value_Set(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount)
{
    if ((valueSetBlocksP == NULL) || (valueSetBlocksCount == 0))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PGPIORVALUESET=");

    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, dest_device_address_string, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!DaphnisI_GPIO_CreateValueSetBlocksString(pRequestCommand, valueSetBlocksP, valueSetBlocksCount))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t txConfEvents[] = {DaphnisI_ATEvent_P2P_TxConf_Success, DaphnisI_ATEvent_P2P_TxConf_Error, DaphnisI_ATEvent_P2P_TxConf_Busy};

    if (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txConfEvents, sizeof(txConfEvents) / sizeof(DaphnisI_ATEvent_t), true) != DaphnisI_ATEvent_P2P_TxConf_Success)
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Remote_GPIO), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_GPIO_Remote_Value_Get(DaphnisI_Device_Address_t dest_address, DaphnisI_GPIO_t* gpioToReadP, uint8_t gpioToReadCount)
{
    if ((gpioToReadP == NULL) || (gpioToReadCount == 0))
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PGPIORVALUEGET=");

    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, dest_device_address_string, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    for (uint8_t i = 0; i < gpioToReadCount; i++)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, gpioToReadP[i], (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        if (i != (gpioToReadCount - 1))
        {
            if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t txConfEvents[] = {DaphnisI_ATEvent_P2P_TxConf_Success, DaphnisI_ATEvent_P2P_TxConf_Error, DaphnisI_ATEvent_P2P_TxConf_Busy};

    if (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txConfEvents, sizeof(txConfEvents) / sizeof(DaphnisI_ATEvent_t), true) != DaphnisI_ATEvent_P2P_TxConf_Success)
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Remote_GPIO), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_GPIO_Remote_ParseCfgChangedEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgChangedData_t* cfgChangedP)
{
    if ((cfgChangedP == NULL) || (cfgChangedP->configBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), cfgChangedP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(cfgChangedP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return DaphnisI_GPIO_ParseConfigBlocks(*pEventArguments, cfgChangedP->configBlocksP, &(cfgChangedP->configBlocksCount));
}

bool DaphnisI_P2P_GPIO_Remote_ParseValueChangedEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueChangedData_t* valueChangedP)
{
    if ((valueChangedP == NULL) || (valueChangedP->valueGetBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), valueChangedP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(valueChangedP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return DaphnisI_GPIO_ParseValueGetBlocks(*pEventArguments, valueChangedP->valueGetBlocksP, &(valueChangedP->valueGetBlocksCount));
}

bool DaphnisI_P2P_GPIO_Remote_ParseCfgSetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t* cfgSetResponseP)
{
    if ((cfgSetResponseP == NULL) || (cfgSetResponseP->statusBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), cfgSetResponseP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(cfgSetResponseP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    cfgSetResponseP->command_status = (**pEventArguments - '0');

    *pEventArguments += 2;

    if (cfgSetResponseP->command_status != DaphnisI_GPIO_Command_Status_Success)
    {
        // No more parameters to parse after this
        return true;
    }

    return DaphnisI_GPIO_ParseStatusBlocks(*pEventArguments, cfgSetResponseP->statusBlocksP, &(cfgSetResponseP->statusBlocksCount));
}

bool DaphnisI_P2P_GPIO_Remote_ParseCfgGetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t* cfgGetResponseP)
{
    if ((cfgGetResponseP == NULL) || (cfgGetResponseP->configBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), cfgGetResponseP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(cfgGetResponseP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(cfgGetResponseP->command_status), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return DaphnisI_GPIO_ParseConfigBlocks(*pEventArguments, cfgGetResponseP->configBlocksP, &(cfgGetResponseP->configBlocksCount));
}

bool DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t* valueSetResponseP)
{
    if ((valueSetResponseP == NULL) || (valueSetResponseP->statusBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), valueSetResponseP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(valueSetResponseP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(valueSetResponseP->command_status), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return DaphnisI_GPIO_ParseStatusBlocks(*pEventArguments, valueSetResponseP->statusBlocksP, &(valueSetResponseP->statusBlocksCount));
}

bool DaphnisI_P2P_GPIO_Remote_ParseValueGetResponseEvent(char** pEventArguments, DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t* valueGetResponseP)
{
    if ((valueGetResponseP == NULL) || (valueGetResponseP->valueGetBlocksP == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), valueGetResponseP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(valueGetResponseP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(valueGetResponseP->command_status), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return DaphnisI_GPIO_ParseValueGetBlocks(*pEventArguments, valueGetResponseP->valueGetBlocksP, &(valueGetResponseP->valueGetBlocksCount));
}

static bool DaphnisI_GPIO_ParseConfigBlocks(char* string, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t* configBlocksCountP)
{
    if ((configBlocksP == NULL) || (configBlocksCountP == NULL))
    {
        return false;
    }

    *configBlocksCountP = ATCommand_CountArgs(string);

    for (uint8_t i = 0; i < *configBlocksCountP; i++)
    {
        if (!ATCommand_GetNextArgumentInt(&string, &configBlocksP[i].GPIO_ID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, GPIO_PIN_DELIMITER))
        {
            return false;
        }

        configBlocksP[i].function = (*string - '0');

        string += 2;

        char value_delimiter = (i == (*configBlocksCountP - 1)) ? ATCOMMAND_STRING_TERMINATE : ATCOMMAND_ARGUMENT_DELIM;

        switch (configBlocksP[i].function)
        {
            case DaphnisI_GPIO_IO_Disconnected:
            {
                break;
            }
            case DaphnisI_GPIO_IO_Input:
            {
                if (!ATCommand_GetNextArgumentInt(&string, &(configBlocksP[i].value.input), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, value_delimiter))
                {
                    return false;
                }

                break;
            }
            case DaphnisI_GPIO_IO_Output:
            {
                if (!ATCommand_GetNextArgumentInt(&string, &(configBlocksP[i].value.output), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, value_delimiter))
                {
                    return false;
                }

                break;
            }
            default:
            {
                return false;
            }
        }
    }

    return true;
}

static bool DaphnisI_GPIO_ParseValueGetBlocks(char* string, DaphnisI_GPIOValueGetBlock_t* valueGetBlocksP, uint8_t* valueGetBlocksCountP)
{
    if ((valueGetBlocksP == NULL) || (valueGetBlocksCountP == NULL))
    {
        return false;
    }

    *valueGetBlocksCountP = ATCommand_CountArgs(string);

    for (uint8_t i = 0; i < *valueGetBlocksCountP; i++)
    {
        if (!ATCommand_GetNextArgumentInt(&string, &valueGetBlocksP[i].GPIO_ID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, GPIO_PIN_DELIMITER))
        {
            return false;
        }

        valueGetBlocksP[i].status = (*string - '0');

        string += 2;

        if (!ATCommand_GetNextArgumentInt(&string, &(valueGetBlocksP[i].value), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, (i == (*valueGetBlocksCountP - 1)) ? ATCOMMAND_STRING_TERMINATE : ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    return true;
}

static bool DaphnisI_GPIO_ParseStatusBlocks(char* string, DaphnisI_GPIOStatusBlock_t* statusBlocksP, uint8_t* statusBlocksCountP)
{
    if ((statusBlocksP == NULL) || (statusBlocksCountP == NULL))
    {
        return false;
    }

    *statusBlocksCountP = ATCommand_CountArgs(string);

    for (uint8_t i = 0; i < *statusBlocksCountP; i++)
    {

        if (!ATCommand_GetNextArgumentInt(&string, &(statusBlocksP[i].GPIO_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, GPIO_PIN_DELIMITER))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentInt(&string, &(statusBlocksP[i].status), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, (i == (*statusBlocksCountP - 1)) ? ATCOMMAND_STRING_TERMINATE : ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    return true;
}

static bool DaphnisI_GPIO_CreateConfigBlocksString(char* string, DaphnisI_GPIOConfigBlock_t* configBlocksP, uint8_t configBlocksCount)
{
    if ((string == NULL) || (configBlocksP == NULL))
    {
        return false;
    }

    for (uint8_t i = 0; i < configBlocksCount; i++)
    {
        if (!ATCommand_AppendArgumentInt(string, configBlocksP[i].GPIO_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), GPIO_PIN_DELIMITER))
        {
            return false;
        }

        if (!ATCommand_AppendArgumentInt(string, configBlocksP[i].function, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        switch (configBlocksP[i].function)
        {
            case DaphnisI_GPIO_IO_Disconnected:
            {
                break;
            }
            case DaphnisI_GPIO_IO_Input:
            {
                if (!ATCommand_AppendArgumentString(string, ATCOMMAND_STRING_EMPTY, GPIO_PIN_DELIMITER))
                {
                    return false;
                }

                if (!ATCommand_AppendArgumentInt(string, configBlocksP[i].value.input, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }

                break;
            }
            case DaphnisI_GPIO_IO_Output:
            {
                if (!ATCommand_AppendArgumentString(string, ATCOMMAND_STRING_EMPTY, GPIO_PIN_DELIMITER))
                {
                    return false;
                }

                if (!ATCommand_AppendArgumentInt(string, configBlocksP[i].value.output, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }

                break;
            }
            default:
            {
                return false;
            }
        }

        if (i != (configBlocksCount - 1))
        {
            if (!ATCommand_AppendArgumentString(string, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    if (!ATCommand_AppendArgumentString(string, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

static bool DaphnisI_GPIO_CreateValueSetBlocksString(char* string, DaphnisI_GPIOValueSetBlock_t* valueSetBlocksP, uint8_t valueSetBlocksCount)
{
    for (uint8_t i = 0; i < valueSetBlocksCount; i++)
    {
        if (!ATCommand_AppendArgumentInt(string, valueSetBlocksP[i].GPIO_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), GPIO_PIN_DELIMITER))
        {
            return false;
        }

        if (!ATCommand_AppendArgumentInt(string, valueSetBlocksP[i].value, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        if (i != (valueSetBlocksCount - 1))
        {
            if (!ATCommand_AppendArgumentString(string, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    if (!ATCommand_AppendArgumentString(string, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

#endif
