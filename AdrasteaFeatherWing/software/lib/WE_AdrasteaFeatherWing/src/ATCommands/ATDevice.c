/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 *THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 *KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 *INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 *INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 *REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 *PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT
 *RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED.
 *INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS
 *OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE
 *SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for basic device functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATDevice.h"

#include "../Adrastea.h"

static const char
    *ATDevice_Character_Set_Strings[ATDevice_Character_Set_NumberOfValues] = {
        "UCS2", "8859-1", "IRA", "HEX", "PCCP437",
};

/**
 * @brief Tests the connection to the wireless module (using the AT command).
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_Test() {
    if (!Adrastea_SendRequest("AT\r\n")) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Request Manufacturer Identity (using the AT+CGMI command).
 *
 * @param[out] manufacturerIdentityP Manufacturer Identity is returned in this
 * argument.
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_RequestManufacturerIdentity(
    ATDevice_Manufacturer_Identity_t *manufacturerIdentityP) {
    if (manufacturerIdentityP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGMI\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(
            &pResponseCommand, (char *)*manufacturerIdentityP,
            ATCOMMAND_STRING_TERMINATE, sizeof(*manufacturerIdentityP))) {
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
bool ATDevice_RequestModelIdentity(ATDevice_Model_Identity_t *modelIdentityP) {
    if (modelIdentityP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGMM\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(
            &pResponseCommand, (char *)*modelIdentityP,
            ATCOMMAND_STRING_TERMINATE, sizeof(*modelIdentityP))) {
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
bool ATDevice_RequestRevisionIdentity(
    ATDevice_Revision_Identity_t *revisionIdentityP) {
    if (revisionIdentityP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGMR\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, pResponseCommand,
                                         '_', sizeof(AT_commandBuffer))) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &revisionIdentityP->major,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.')) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &revisionIdentityP->minor,
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
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
bool ATDevice_RequestIMEI(ATDevice_IMEI_t *imeiP) {
    if (imeiP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGSN=1\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
            &pResponseCommand, (char *)*imeiP, ATCOMMAND_STRING_TERMINATE,
            sizeof(*imeiP))) {
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
bool ATDevice_RequestIMEISV(ATDevice_IMEISV_t *imeisvP) {
    if (imeisvP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGSN=2\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
            &pResponseCommand, (char *)*imeisvP, ATCOMMAND_STRING_TERMINATE,
            sizeof(*imeisvP))) {
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
bool ATDevice_RequestSVN(ATDevice_SVN_t *svnP) {
    if (svnP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CGSN=3\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
            &pResponseCommand, (char *)*svnP, ATCOMMAND_STRING_TERMINATE,
            sizeof(*svnP))) {
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
bool ATDevice_RequestSerialNumber(ATDevice_Serial_Number_t *serialNumberP) {
    if (serialNumberP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+GSN\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(
            &pResponseCommand, (char *)*serialNumberP,
            ATCOMMAND_STRING_TERMINATE, sizeof(*serialNumberP))) {
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
bool ATDevice_GetTECharacterSet(ATDevice_Character_Set_t *charsetP) {
    if (charsetP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CSCS?\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(
            &pResponseCommand, (uint8_t *)charsetP,
            ATDevice_Character_Set_Strings,
            ATDevice_Character_Set_NumberOfValues, 30,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Set TE Character Set (using the AT+CSCS command).
 *
 * @param[in] charset TE Character Set. See ATDevice_Character_Set_t.
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_SetTECharacterSet(ATDevice_Character_Set_t charset) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CSCS=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(
            pRequestCommand, ATDevice_Character_Set_Strings[charset],
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF,
                                        ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!Adrastea_SendRequest(pRequestCommand)) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
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
bool ATDevice_GetCapabilitiesList(char *capListP, uint8_t maxBufferSize) {
    if (capListP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+GCAP\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, capListP,
                                         ATCOMMAND_STRING_TERMINATE,
                                         maxBufferSize)) {
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
bool ATDevice_GetPhoneFunctionality(ATDevice_Phone_Functionality_t *phoneFunP) {
    if (phoneFunP == NULL) {
        return false;
    }

    if (!Adrastea_SendRequest("AT+CFUN?\r\n")) {
        return false;
    }

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    pResponseCommand = pResponseCommand + 1;

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, phoneFunP,
            (ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED),
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Set Phone Functionality (using the AT+CFUN command).
 *
 * @param[in] phoneFun Phone Functionality. See ATDevice_Phone_Functionality_t.
 *
 * @param[in] resetType Reset type when changing phone functionality (optional
 * pass ATDevice_Phone_Functionality_Reset_Invalid to skip). See
 * ATDevice_Phone_Functionality_Reset_t.
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_SetPhoneFunctionality(
    ATDevice_Phone_Functionality_t phoneFun,
    ATDevice_Phone_Functionality_Reset_t resetType) {
    Adrastea_optionalParamsDelimCount = 1;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CFUN=");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, phoneFun,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (resetType != ATDevice_Phone_Functionality_Reset_Invalid) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, resetType,
                (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
                ATCOMMAND_STRING_TERMINATE)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount = 0;
    }

    pRequestCommand[strlen(pRequestCommand) -
                    Adrastea_optionalParamsDelimCount] =
        ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF,
                                        ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!Adrastea_SendRequest(pRequestCommand)) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Resets the device. (using the ATZ command).
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_Reset() {
    if (!Adrastea_SendRequest("ATZ\r\n")) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Performs a factory reset of the device (using the AT&F0 command).
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_FactoryReset() {
    if (!Adrastea_SendRequest("AT&F0\r\n")) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Set Result Code Format (using the ATV command).
 *
 * @param[in] format Result Code Format. See ATDevice_Result_Code_Format_t.
 *
 * @return true if successful, false otherwise
 */
bool ATDevice_SetResultCodeFormat(ATDevice_Result_Code_Format_t format) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "ATV");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, format,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF,
                                        ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!Adrastea_SendRequest(pRequestCommand)) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Device),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}
