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
 * @brief LoRaWAN commands for DaphnisI source file.
 */
#include <DaphnisI/ATCommands/ATCommon.h>
#include <DaphnisI/ATCommands/LoRaWAN.h>
#include <DaphnisI/DaphnisI.h>
#include <global/ATCommands.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char* DaphnisI_Delay_GetRequestStrings[DaphnisI_Delay_Count] = {"AT+JN1DL=?\r\n", "AT+JN2DL=?\r\n", "AT+RX1DL=?\r\n", "AT+RX2DL=?\r\n"};
static char* DaphnisI_Delay_SetRequestStrings[DaphnisI_Delay_Count] = {"AT+JN1DL=", "AT+JN2DL=", "AT+RX1DL=", "AT+RX2DL="};
static char* DaphnisI_Delay_CmdStrings[DaphnisI_Delay_Count] = {"+JN1DL:", "+JN2DL:", "+RX1DL:", "+RX2DL:"};

static char* DaphnisI_Key_SetRequestStrings[DaphnisI_KeyType_Count] = {"AT+APPKEY=", "AT+NWKSKEY=", "AT+APPSKEY="};

static const char* DaphnisI_RX_Window_Slot_Strings[DaphnisI_SlotWindow_Count] = {"RX_1", "RX_2", "RX_P", "RX_C"};

bool DaphnisI_LoRaWAN_SetAPPEUI(DaphnisI_EUI_t app_eui)
{
    if (NULL == app_eui)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+APPEUI=");

    char app_eui_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_LORAWAN_EUI_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(app_eui, DAPHNISI_LORAWAN_EUI_LENGTH, app_eui_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, app_eui_string, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetAPPEUI(DaphnisI_EUI_t* app_euiP)
{
    if (NULL == app_euiP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+APPEUI=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+APPEUI:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char app_eui_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_LORAWAN_EUI_LENGTH)];

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, app_eui_string, ATCOMMAND_STRING_TERMINATE, sizeof(app_eui_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(app_eui_string, strlen(app_eui_string), *app_euiP);
}

bool DaphnisI_LoRaWAN_SetDeviceEUI(DaphnisI_EUI_t device_eui)
{
    if (NULL == device_eui)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+DEUI=");

    char device_eui_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_LORAWAN_EUI_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(device_eui, DAPHNISI_LORAWAN_EUI_LENGTH, device_eui_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, device_eui_string, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDeviceEUI(DaphnisI_EUI_t* device_euiP)
{
    if (NULL == device_euiP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+DEUI=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+DEUI:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char device_eui_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_LORAWAN_EUI_LENGTH)];

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, device_eui_string, ATCOMMAND_STRING_TERMINATE, sizeof(device_eui_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(device_eui_string, strlen(device_eui_string), *device_euiP);
}

bool DaphnisI_LoRaWAN_SetKey(DaphnisI_KeyType_t keyType, DaphnisI_Key_t key)
{
    if (NULL == key)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, DaphnisI_Key_SetRequestStrings[keyType]);

    char key_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_LORAWAN_KEY_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(key, DAPHNISI_LORAWAN_KEY_LENGTH, key_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, key_string, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_SetDeviceAddress(DaphnisI_Device_Address_t device_address)
{
    if (NULL == device_address)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+DADDR=");

    char device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!DaphnisI_ByteArrayToKeyAddr(device_address, DAPHNISI_ADDRESS_LENGTH, device_address_string))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, device_address_string, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDeviceAddress(DaphnisI_Device_Address_t* device_addressP)
{
    if (NULL == device_addressP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+DADDR=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+DADDR:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char device_address_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, device_address_string, ATCOMMAND_STRING_TERMINATE, sizeof(device_address_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(device_address_string, strlen(device_address_string), *device_addressP);
}

bool DaphnisI_LoRaWAN_SetNetworkID(uint8_t network_id)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+NWKID=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, network_id, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetNetworkID(uint8_t* network_idP)
{
    if (NULL == network_idP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+NWKID=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+NWKID:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, network_idP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_StoreStackContext()
{
    if (!DaphnisI_SendRequest("AT+CS\r\n"))
    {
        return false;
    }
    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

bool DaphnisI_LoRaWAN_EraseStoredStackContext()
{
    if (!DaphnisI_SendRequest("AT+RFS\r\n"))
    {
        return false;
    }
    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

bool DaphnisI_LoRaWAN_SetDeviceClass(DaphnisI_DeviceClass_t device_class)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+CLASS=");

    char device_class_string[2] = "A";
    device_class_string[0] += device_class;

    if (!ATCommand_AppendArgumentString(pRequestCommand, device_class_string, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDeviceClass(DaphnisI_DeviceClass_t* device_classP)
{
    if (NULL == device_classP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+CLASS=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+CLASS:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, device_classP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_Join(DaphnisI_JoinMode_t join_Mode)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+JOIN=");

    ATCommand_AppendArgumentInt(pRequestCommand, join_Mode, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!DaphnisI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand);
}

bool DaphnisI_LoRaWAN_Send(uint8_t port, const uint8_t* payload, uint16_t length, bool ack_Requested)
{
    if (NULL == payload)
    {
        return false;
    }

    if ((port == 0) || (199 < port))
    {
        return false;
    }

    if (DAPHNISI_LORAWAN_MAX_PAYLOAD_SIZE < length)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+SEND=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, port, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_EVENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, ack_Requested ? 1 : 0, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_EVENT_DELIM))
    {
        return false;
    }

    size_t pRequestCommandLength = strlen(pRequestCommand);

    for (uint16_t i = 0; i < length; i++)
    {
        sprintf(&pRequestCommand[pRequestCommandLength], "%02" PRIX32, (uint32_t)payload[i]);
        pRequestCommandLength += 2;
    }

    pRequestCommand[pRequestCommandLength] = '\0';

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

bool DaphnisI_LoRaWAN_SetDelay(DaphnisI_Delay_t delayType, uint16_t delayMS)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, DaphnisI_Delay_SetRequestStrings[delayType]);

    if (!ATCommand_AppendArgumentInt(pRequestCommand, delayMS, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDelay(DaphnisI_Delay_t delayType, uint16_t* delayMSP)
{
    if (NULL == delayMSP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest(DaphnisI_Delay_GetRequestStrings[delayType]))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = DaphnisI_Delay_CmdStrings[delayType];
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, delayMSP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE16, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetRX2Frequency(uint32_t frequency)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+RX2FQ=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, frequency, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetRX2Frequency(uint32_t* frequencyP)
{
    if (NULL == frequencyP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+RX2FQ=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+RX2FQ:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, frequencyP, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetRX2DataRate(uint8_t dataRate)
{
    // range of data rate: 0-7
    if (7 < dataRate)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+RX2DR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, dataRate, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetRX2DataRate(uint8_t* dataRateP)
{
    if (NULL == dataRateP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+RX2DR=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+RX2DR:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, dataRateP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetPingSlotPeriodicityFactor(uint8_t factor)
{
    // range of factor: 0-7
    if (7 < factor)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+PGSLOT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, factor, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetPingSlotPeriodicityFactor(uint8_t* factorP)
{
    if (NULL == factorP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+PGSLOT=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+PGSLOT:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, factorP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetTxPower(uint8_t power)
{
    // range of power: 0-7
    if (7 < power)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+TXP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, power, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetTxPower(uint8_t* powerP)
{
    if (NULL == powerP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+TXP=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+TXP:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, powerP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetAdaptiveDataRate(bool adr)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+ADR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, adr, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetAdaptiveDataRate(bool* adrP)
{
    if (NULL == adrP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+ADR=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+ADR:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, adrP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetDataRate(uint8_t dataRate)
{
    // range of data rate: 0-7
    if (7 < dataRate)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+DR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, dataRate, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDataRate(uint8_t* dataRateP)
{
    if (NULL == dataRateP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+DR=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+DR:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, dataRateP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_SetDutyCycleRestriction(bool dcRestriction)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+DCS=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, dcRestriction, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
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

bool DaphnisI_LoRaWAN_GetDutyCycleRestriction(bool* dcRestrictionP)
{
    if (NULL == dcRestrictionP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+DCS=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+DCS:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, dcRestrictionP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

bool DaphnisI_LoRaWAN_GetLinkLayerVersion(DaphnisI_LoRaWAN_LL_Version_t* ll_VersionP)
{
    if (NULL == ll_VersionP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+LLVER=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+LLVER:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    pRespondCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Minor, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Patch, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_LoRaWAN_GetRegionalParametersrVersion(DaphnisI_LoRaWAN_RP_Version_t* rp_VersionP)
{
    if (NULL == rp_VersionP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+RPVER=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+RPVER:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    pRespondCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &rp_VersionP->Label, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '-'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &rp_VersionP->Major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &rp_VersionP->Minor, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(&pRespondCommand, &rp_VersionP->Patch, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

#endif

bool DaphnisI_LoRaWAN_ParseClassEvent(char** pEventArguments, DaphnisI_DeviceClass_t* pDevice_class)
{
    char temp[2];
    if (ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_STRING_TERMINATE, sizeof(temp)))
    {
        *pDevice_class = temp[0] - 'A';
        return true;
    }

    return false;
}

bool DaphnisI_LoRaWAN_ParseRxInfoEvent(char** pEventArguments, DaphnisI_RxInfo_t* pRxInfo)
{
    int argumentsCount = ATCommand_CountArgs(*pEventArguments);

    if (!ATCommand_GetNextArgumentEnum(pEventArguments, &(pRxInfo->window), DaphnisI_RX_Window_Slot_Strings, DaphnisI_SlotWindow_Count, 5, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->port), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->dataRate), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->RSSI), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    //depending on the argument count, this is the last or 2 more following
    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->SNR), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, (argumentsCount == 5) ? ATCOMMAND_STRING_TERMINATE : ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    // optional parameters present
    if (argumentsCount == 7)
    {
        if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->linkMargin), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->gatewaysReached), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    return true;
}

bool DaphnisI_LoRaWAN_ParseRxDataEvent(char** pEventArguments, DaphnisI_RxData_t* pRxData)
{
    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxData->port), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxData->dataLength), ATCOMMAND_INTFLAGS_NOTATION_HEX | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    uint8_t outLength = (strlen(*pEventArguments) >> 1);
    if (outLength != pRxData->dataLength)
    {
        // not the expected length of data
        return false;
    }

    char byteString[3];
    byteString[2] = '\0';
    // Read to characters of hex representation and convert to bytes
    for (uint8_t i = 0; i < outLength; i++)
    {
        byteString[0] = (*pEventArguments)[2 * i];
        byteString[1] = (*pEventArguments)[2 * i + 1];

        pRxData->data[i] = (uint8_t)strtol(byteString, NULL, 16);
    }

    return true;
}

bool DaphnisI_LoRaWAN_ParseBeaconInfoEvent(char** pEventArguments, DaphnisI_BeaconInfo_t* pBeaconInfo)
{
    char temp[6];

    if (!ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_ARGUMENT_DELIM, 6))
    {
        /* ignore first parameter. Is always the same */
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->dataRate), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->RSSI), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->SNR), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->frequency), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->timestamp), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->infoDescriptor), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, pBeaconInfo->info, ATCOMMAND_ARGUMENT_DELIM, 7))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, &(pBeaconInfo->info[7]), ATCOMMAND_STRING_TERMINATE, 7))
    {
        return false;
    }

    return true;
}
