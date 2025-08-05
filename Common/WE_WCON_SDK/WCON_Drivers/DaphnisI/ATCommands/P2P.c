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
 * @brief P2P commands for DaphnisI source file.
 */
#include <DaphnisI/ATCommands/P2P.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#include <DaphnisI/ATCommands/ATCommon.h>
#include <global/ATCommands.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool DaphnisI_P2P_SendPayloadString(char* string, uint8_t* payloadP, uint16_t payload_length);

bool DaphnisI_P2P_TransmitBroadcast(uint8_t* payloadP, uint16_t payload_length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+P2PBROADCASTTX=");

    if (!DaphnisI_P2P_SendPayloadString(pRequestCommand, payloadP, payload_length))
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_General), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_TransmitUnicast(uint8_t* payloadP, uint16_t payload_length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+P2PUNICASTTX=");

    if (!DaphnisI_P2P_SendPayloadString(pRequestCommand, payloadP, payload_length))
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespEvents[] = {
        DaphnisI_ATEvent_P2P_TxResp_Success,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Timeout,
        DaphnisI_ATEvent_P2P_TxResp_Fail_NACK_Received,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Internal_Error,
    };

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txRespEvents, sizeof(txRespEvents) / sizeof(DaphnisI_ATEvent_t), false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_TransmitMulticast(uint8_t* payloadP, uint16_t payload_length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+P2PMULTICASTTX=");

    if (!DaphnisI_P2P_SendPayloadString(pRequestCommand, payloadP, payload_length))
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_General), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_TransmitUnicastExtended(DaphnisI_Device_Address_t dest_address, uint8_t* payloadP, uint16_t payload_length)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PUNICASTEXTX=");

    char dest_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(dest_address, DAPHNISI_ADDRESS_LENGTH, dest_device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, dest_device_address_string, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!DaphnisI_P2P_SendPayloadString(pRequestCommand, payloadP, payload_length))
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespEvents[] = {
        DaphnisI_ATEvent_P2P_TxResp_Success,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Timeout,
        DaphnisI_ATEvent_P2P_TxResp_Fail_NACK_Received,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Internal_Error,
    };

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txRespEvents, sizeof(txRespEvents) / sizeof(DaphnisI_ATEvent_t), false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_TransmitMulticastExtended(uint8_t group_id, uint8_t* payloadP, uint16_t payload_length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+P2PMULTICASTEXTX=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, group_id, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!DaphnisI_P2P_SendPayloadString(pRequestCommand, payloadP, payload_length))
    {
        return false;
    }

    DaphnisI_ATEvent_t txRespSuccessEvent = DaphnisI_ATEvent_P2P_TxResp_Success;

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_General), &txRespSuccessEvent, 1, false) == DaphnisI_ATEvent_P2P_TxResp_Success);
}

bool DaphnisI_P2P_SetRXEnabled(bool rx_state)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+P2PRX=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint8_t)rx_state, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_P2P_GetRXEnabled(bool* rx_stateP)
{
    if (rx_stateP == NULL)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+P2PRX=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+P2PRX:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, (uint8_t*)rx_stateP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

static bool DaphnisI_P2P_SendPayloadString(char* string, uint8_t* payloadP, uint16_t payload_length)
{
    if (payloadP == NULL)
    {
        return false;
    }

    if (payload_length > DAPHNISI_P2P_MAX_PAYLOAD_SIZE)
    {
        return false;
    }

    char payload_string[BYTEARRAY_TO_HEXSTRING_LENGTH(DAPHNISI_P2P_MAX_PAYLOAD_SIZE)];

    if (!DaphnisI_ByteArrayToHexString(payloadP, payload_length, payload_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(string, payload_string, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(string, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(string))
    {
        return false;
    }

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t txConfEvents[] = {DaphnisI_ATEvent_P2P_TxConf_Success, DaphnisI_ATEvent_P2P_TxConf_Error, DaphnisI_ATEvent_P2P_TxConf_Busy};

    return (DaphnisI_WaitForEvents(DaphnisI_GetTimeout(DaphnisI_Timeout_P2P_Data), txConfEvents, sizeof(txConfEvents) / sizeof(DaphnisI_ATEvent_t), true) == DaphnisI_ATEvent_P2P_TxConf_Success);
}

bool DaphnisI_P2P_ParseTXTimeEvent(char** pEventArguments, uint32_t* timeonairP) { return ATCommand_GetNextArgumentInt(pEventArguments, timeonairP, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE); }

bool DaphnisI_P2P_ParseRXDataEvent(char** pEventArguments, DaphnisI_P2P_RxData_t* rxDataP)
{
    if ((rxDataP == NULL) || (rxDataP->data == NULL))
    {
        return false;
    }

    char src_device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(pEventArguments, src_device_address_string, ATCOMMAND_ARGUMENT_DELIM, sizeof(src_device_address_string)))
    {
        return false;
    }

    if (!DaphnisI_KeyAddrToByteArray(src_device_address_string, strlen(src_device_address_string), rxDataP->source_address))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rxDataP->RSSI), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rxDataP->data_length), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (rxDataP->data_length != (strlen(*pEventArguments) >> 1))
    {
        return false;
    }

    if (!DaphnisI_HexStringToByteArray(*pEventArguments, (rxDataP->data_length << 1), rxDataP->data))
    {
        return false;
    }

    *pEventArguments += (rxDataP->data_length << 1);

    return true;
}

#endif
