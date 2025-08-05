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
 * @brief Device commands for DaphnisI source file.
 */
#include <DaphnisI/ATCommands/ATCommon.h>
#include <DaphnisI/ATCommands/ATDevice.h>
#include <global/ATCommands.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool DaphnisI_Test()
{
    if (!DaphnisI_SendRequest("AT\r\n"))
    {
        return false;
    }

    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

bool DaphnisI_Reset()
{
    if (!DaphnisI_SendRequest("ATZ\r\n"))
    {
        return false;
    }

    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL))
    {
        return false;
    }

    DaphnisI_ATEvent_t startUpEvent = DaphnisI_ATEvent_StartUp;

    return (DaphnisI_WaitForEvents(DAPHNISI_STARTUP_TIME, &startUpEvent, 1, true) == DaphnisI_ATEvent_StartUp);
}

bool DaphnisI_Sleep()
{
    if (!DaphnisI_SendRequest("AT+SLEEP\r\n"))
    {
        return false;
    }
    return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

bool DaphnisI_GetBatteryLevel(uint16_t* batteryLevelP)
{
    if (NULL == batteryLevelP)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+BAT=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+BAT:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, batteryLevelP, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool DaphnisI_GetGenericVersion(DaphnisI_FW_Version_t* firmware_VersionP, DaphnisI_LoRaWAN_LL_Version_t* ll_VersionP, DaphnisI_LoRaWAN_RP_Version_t* rp_VersionP)
{
    if ((firmware_VersionP == NULL) || (ll_VersionP == NULL) || (rp_VersionP == NULL))
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+VER=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+VER:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    pRespondCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Minor, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Patch, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    pRespondCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Minor, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &ll_VersionP->Patch, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

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

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

bool DaphnisI_GetSerialNumber(DaphnisI_SerialNumber_t* serialNumberP)
{
    if (serialNumberP == NULL)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+SN=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+SN:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char serial_number_string[BYTEARRAY_TO_KEYADDR_LENGTH(DAPHNISI_ADDRESS_LENGTH)];

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, serial_number_string, ATCOMMAND_STRING_TERMINATE, sizeof(serial_number_string)))
    {
        return false;
    }

    return DaphnisI_KeyAddrToByteArray(serial_number_string, strlen(serial_number_string), *serialNumberP);
}

bool DaphnisI_GetFirmwareVersion(DaphnisI_FW_Version_t* firmware_VersionP)
{
    if (firmware_VersionP == NULL)
    {
        return false;
    }

    if (!DaphnisI_SendRequest("AT+FWVER=?\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+FWVER:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    pRespondCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Minor, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, '.'))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(&pRespondCommand, &firmware_VersionP->Patch, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}
#endif
