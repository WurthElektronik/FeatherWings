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
 * @brief AT commands for basic device functionality.
 */
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <global/global.h>
#include <stdio.h>

static const char* AdrasteaI_ATDevice_Character_Set_Strings[AdrasteaI_ATDevice_Character_Set_NumberOfValues] = {
    "UCS2", "8859-1", "IRA", "HEX", "PCCP437",
};

/**
 * @brief Tests the connection to the wireless module (using the AT command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_Test()
{
    if (!AdrasteaI_SendRequest("AT\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request Manufacturer Identity (using the AT+CGMI command).
 *
 * @param[out] manufacturerIdentityP Manufacturer Identity is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestManufacturerIdentity(AdrasteaI_ATDevice_Manufacturer_Identity_t* manufacturerIdentityP)
{
    if (manufacturerIdentityP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGMI\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*)*manufacturerIdentityP, ATCOMMAND_STRING_TERMINATE, sizeof(*manufacturerIdentityP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request Model Identity (using the AT+CGMM command).
 *
 * @param[out] modelIdentityP Model Identity is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestModelIdentity(AdrasteaI_ATDevice_Model_Identity_t* modelIdentityP)
{
    if (modelIdentityP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGMM\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*)*modelIdentityP, ATCOMMAND_STRING_TERMINATE, sizeof(*modelIdentityP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request Revision Identity (using the AT+CGMR command).
 *
 * @param[out] revisionIdentityP Revision Identity is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestRevisionIdentity(AdrasteaI_ATDevice_Revision_Identity_t* revisionIdentityP)
{
    if (revisionIdentityP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGMR\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, pResponseCommand, '_', sizeof(AT_commandBuffer)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &revisionIdentityP->major, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &revisionIdentityP->minor, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request IMEI (using the AT+CGSN command).
 *
 * @param[out] imeiP IMEI is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestIMEI(AdrasteaI_ATDevice_IMEI_t* imeiP)
{
    if (imeiP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGSN=1\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, (char*)*imeiP, ATCOMMAND_STRING_TERMINATE, sizeof(*imeiP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request IMEISV (using the AT+CGSN command).
 *
 * @param[out] imeisvP IMEISV is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestIMEISV(AdrasteaI_ATDevice_IMEISV_t* imeisvP)
{
    if (imeisvP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGSN=2\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, (char*)*imeisvP, ATCOMMAND_STRING_TERMINATE, sizeof(*imeisvP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request SVN (using the AT+CGSN command).
 *
 * @param[out] svnP SVN is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestSVN(AdrasteaI_ATDevice_SVN_t* svnP)
{
    if (svnP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CGSN=3\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, (char*)*svnP, ATCOMMAND_STRING_TERMINATE, sizeof(*svnP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Request Serial Number (using the AT+GSN command).
 *
 * @param[out] serialNumberP Serial Number is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_RequestSerialNumber(AdrasteaI_ATDevice_Serial_Number_t* serialNumberP)
{
    if (serialNumberP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+GSN\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*)*serialNumberP, ATCOMMAND_STRING_TERMINATE, sizeof(*serialNumberP)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read TE Character Set (using the AT+CSCS command).
 *
 * @param[out] charsetP TE Character Set is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_GetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t* charsetP)
{
    if (charsetP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CSCS?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, (uint8_t*)charsetP, AdrasteaI_ATDevice_Character_Set_Strings, AdrasteaI_ATDevice_Character_Set_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set TE Character Set (using the AT+CSCS command).
 *
 * @param[in] charset TE Character Set. See AdrasteaI_ATDevice_Character_Set_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_SetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t charset)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CSCS=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATDevice_Character_Set_Strings[charset], ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read Capabilities List (using the AT+GCAP command).
 *
 * @param[out] capListP Capabilities List is returned in this argument.
 *
 * @param[in] maxBuffer Maximum size of buffer to hold data.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_GetCapabilitiesList(char* capListP, uint8_t maxBufferSize)
{
    if (capListP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+GCAP\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, capListP, ATCOMMAND_STRING_TERMINATE, maxBufferSize))
    {
        return false;
    }

    return true;
}

/**
 * @brief Read Phone Functionality (using the AT+CFUN command).
 *
 * @param[out] phoneFunP Phone Functionality is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_GetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t* phoneFunP)
{
    if (phoneFunP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CFUN?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand = pResponseCommand + 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, phoneFunP, (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Set Phone Functionality (using the AT+CFUN command).
 *
 * @param[in] phoneFun Phone Functionality. See AdrasteaI_ATDevice_Phone_Functionality_t.
 *
 * @param[in] resetType Reset type when changing phone functionality (optional pass AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid to skip). See AdrasteaI_ATDevice_Phone_Functionality_Reset_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_SetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t phoneFun, AdrasteaI_ATDevice_Phone_Functionality_Reset_t resetType)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CFUN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, phoneFun, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (resetType != AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, resetType, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount = 0;
    }

    pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Resets the device. (using the ATZ command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_Reset()
{
    if (!AdrasteaI_SendRequest("ATZ\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

/**
 * @brief Performs a factory reset of the device (using the AT&F0 command).
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_FactoryReset()
{
    if (!AdrasteaI_SendRequest("AT&F0\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Off)
    {
        WE_Delay(10);
    }
    while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Ready)
    {
        WE_Delay(10);
    }

    return true;
}

/**
 * @brief Set Result Code Format (using the ATV command).
 *
 * @param[in] format Result Code Format. See AdrasteaI_ATDevice_Result_Code_Format_t.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_ATDevice_SetResultCodeFormat(AdrasteaI_ATDevice_Result_Code_Format_t format)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "ATV");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Device), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}
