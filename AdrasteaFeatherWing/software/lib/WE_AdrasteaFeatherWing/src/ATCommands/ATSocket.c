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
 * @brief AT commands for Socket functionality.
 */

#include <stdio.h>
#include <ATCommands.h>
#include "ATSocket.h"
#include "../Adrastea.h"

static const char *ATSocket_State_Strings[ATSocket_State_NumberOfValues] = {
    "DEACTIVATED", "ACTIVATED", "LISTENING"};

static const char *ATSocket_Type_Strings[ATSocket_Type_NumberOfValues] = {
    "TCP", "UDP"};

static const char
    *ATSocket_Behaviour_Strings[ATSocket_Behaviour_NumberOfValues] = {
        "OPEN", "LISTEN", "LISTENP"};

/**
 * @brief Read Created Sockets States (using the AT%SOCKETCMD command).
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReadCreatedSocketsStates() {
    if (!Adrastea_SendRequest("AT%SOCKETCMD?\r\n")) {
        return false;
    }
    return Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                   Adrastea_CNFStatus_Success, NULL);
}

/**
 * @brief Allocate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] sessionID Session ID.
 *
 * @param[in] socketType Socket Type. See ATSocket_Type_t.
 *
 * @param[in] socketBehaviour Socket Behaviour. See ATSocket_Behaviour_t.
 *
 * @param[in] destinationIPAddress Destination IP Address.
 *
 * @param[in] destinationPortNumber Destination Port Number.
 *
 * @param[in] sourcePortNumber Source Port Number (optional pass
 * ATCommon_Port_Number_Invalid to skip).
 *
 * @param[in] packetSize Packet Size (optional pass
 * ATSocket_Data_Length_Automatic to skip).
 *
 * @param[in] socketTimeout Socket Timeout in seconds (optional pass
 * ATSocket_Timeout_Invalid to skip).
 *
 * @param[in] addressFormat IP Address Format (optional pass
 * ATSocket_IP_Addr_Format_Invalid to skip). See ATSocket_IP_Addr_Format_t.
 *
 * @param[out] socketIDP Socket ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_AllocateSocket(ATCommon_Session_ID_t sessionID,
                             ATSocket_Type_t socketType,
                             ATSocket_Behaviour_t socketBehaviour,
                             ATCommon_IP_Addr_t destinationIPAddress,
                             ATCommon_Port_Number_t destinationPortNumber,
                             ATCommon_Port_Number_t sourcePortNumber,
                             ATSocket_Data_Length_t packetSize,
                             ATSocket_Timeout_t socketTimeout,
                             ATSocket_IP_Addr_Format_t addressFormat,
                             ATSocket_ID_t *socketIDP) {
    if (socketIDP == NULL) {
        return false;
    }

    Adrastea_optionalParamsDelimCount = 1;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"ALLOCATE\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, sessionID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(
            pRequestCommand, ATSocket_Type_Strings[socketType],
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(
            pRequestCommand, ATSocket_Behaviour_Strings[socketBehaviour],
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(
            pRequestCommand, destinationIPAddress, ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, destinationPortNumber,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (sourcePortNumber != ATCommon_Port_Number_Invalid) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, sourcePortNumber,
                (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
                ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount = 1;
    } else {
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                            ATCOMMAND_STRING_EMPTY,
                                            ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount++;
    }

    if (packetSize != ATSocket_Data_Length_Automatic) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, packetSize,
                (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
                ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount = 1;
    } else {
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                            ATCOMMAND_STRING_EMPTY,
                                            ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount++;
    }

    if (socketTimeout != ATSocket_Timeout_Invalid) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, socketTimeout,
                (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
                ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount = 1;
    } else {
        if (!ATCommand_AppendArgumentString(pRequestCommand,
                                            ATCOMMAND_STRING_EMPTY,
                                            ATCOMMAND_ARGUMENT_DELIM)) {
            return false;
        }
        Adrastea_optionalParamsDelimCount++;
    }

    if (addressFormat != ATSocket_IP_Addr_Format_Invalid) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, addressFormat,
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

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, socketIDP,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Activate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[in] SSLSessionID SSL Session ID (optional pass
 * ATCommon_Session_ID_Invalid to skip).
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ActivateSocket(ATSocket_ID_t socketID,
                             ATCommon_Session_ID_t SSLSessionID) {
    Adrastea_optionalParamsDelimCount = 1;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"ACTIVATE\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (SSLSessionID != ATCommon_Session_ID_Invalid) {
        if (!ATCommand_AppendArgumentInt(
                pRequestCommand, SSLSessionID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Read Socket Info (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[out] infoP Socket Info is returned in this argument. See
 * ATSocket_Info_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReadSocketInfo(ATSocket_ID_t socketID, ATSocket_Info_t *infoP) {
    if (infoP == NULL) {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"INFO\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(
            &pResponseCommand, (uint8_t *)&infoP->socketState,
            ATSocket_State_Strings, ATSocket_State_NumberOfValues, 30,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(
            &pResponseCommand, (uint8_t *)&infoP->socketType,
            ATSocket_Type_Strings, ATSocket_Type_NumberOfValues, 30,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
            &pResponseCommand, infoP->sourceIPAddress, ATCOMMAND_ARGUMENT_DELIM,
            sizeof(infoP->sourceIPAddress))) {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
            &pResponseCommand, infoP->destinationIPAddress,
            ATCOMMAND_ARGUMENT_DELIM, sizeof(infoP->sourceIPAddress))) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &infoP->sourcePortNumber,
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    switch (Adrastea_CountArgs(pResponseCommand)) {
        case 1: {
            if (!ATCommand_GetNextArgumentInt(
                    &pResponseCommand, &infoP->destinationPortNumber,
                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
                    ATCOMMAND_STRING_TERMINATE)) {
                return false;
            }

            infoP->tcpSocketDirection = ATSocket_Direction_Invalid;

            infoP->socketTimeout = ATSocket_Timeout_Invalid;

            break;
        }
        case 3: {
            if (!ATCommand_GetNextArgumentInt(
                    &pResponseCommand, &infoP->destinationPortNumber,
                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
                    ATCOMMAND_ARGUMENT_DELIM)) {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(
                    &pResponseCommand, &infoP->tcpSocketDirection,
                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
                    ATCOMMAND_ARGUMENT_DELIM)) {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(
                    &pResponseCommand, &infoP->socketTimeout,
                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
                    ATCOMMAND_STRING_TERMINATE)) {
                return false;
            }

            break;
        }
        default:
            return false;
    }

    return true;
}

/**
 * @brief Deactivate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_DeactivateSocket(ATSocket_ID_t socketID) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"DEACTIVATE\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Set Socket Options (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[in] aggregationTime Aggregation Time in milliseconds.
 *
 * @param[in] aggregationBufferSize Aggregation Buffer Size in bytes.
 *
 * @param[in] idleTime Idle Time in seconds.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_SetSocketOptions(
    ATSocket_ID_t socketID, ATSocket_Aggregation_Time_t aggregationTime,
    ATSocket_Aggregation_Buffer_Size_t aggregationBufferSize,
    ATSocket_TCP_Idle_Time_t idleTime) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SETOPT\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, aggregationTime,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, aggregationBufferSize,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, idleTime,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Delete Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_DeleteSocket(ATSocket_ID_t socketID) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"DELETE\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Add SSL to Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[in] authMode SSL Authentication Mode.
 *
 * @param[in] profileID SSL Profile ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_AddSSLtoSocket(ATSocket_ID_t socketID,
                             ATCommon_SSL_Auth_Mode_t authMode,
                             ATCommon_SSL_Profile_ID_t profileID) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLALLOC\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, authMode,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, profileID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Read Socket Last Error (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[out] errorCodeP Socket Error Code is returned in this argument. See
 * ATSocket_Error_Code_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReadSocketLastError(ATSocket_ID_t socketID,
                                  ATSocket_Error_Code_t *errorCodeP) {
    if (errorCodeP == NULL) {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"LASTERROR\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, errorCodeP,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Read Socket SSL Info (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[out] infoP SSL Info is returned in this argument. See
 * ATSocket_SSL_Info_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReadSocketSSLInfo(ATSocket_ID_t socketID,
                                ATSocket_SSL_Info_t *infoP) {
    if (infoP == NULL) {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLINFO\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &infoP->SSLMode,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &infoP->profileID,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Keep SSL Session for Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] sessionID Session ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_KeepSocketSSLSession(ATSocket_ID_t socketID) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLKEEP\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Delete SSL Session for Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID Socket ID.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_DeleteSocketSSLSession(ATSocket_ID_t socketID) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLDEL\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Receive from Socket (using the AT%SOCKETDATA command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[in] maxBufferLength Maximum data length to read.
 *
 * @param[out] dataReadP Data read is returned in this argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReceiveFromSocket(ATSocket_ID_t socketID,
                                ATSocket_Data_Read_t *dataReadP,
                                uint16_t maxBufferLength) {
    if (dataReadP == NULL) {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETDATA=\"RECEIVE\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, maxBufferLength,
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

    char *pResponseCommand = AT_commandBuffer;

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success,
                                 pResponseCommand)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &dataReadP->socketID,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &dataReadP->dataLength,
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &pResponseCommand, &dataReadP->dataLeftLength,
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    switch (Adrastea_CountArgs(pResponseCommand)) {
        case 1: {
            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
                    &pResponseCommand, dataReadP->data,
                    ATCOMMAND_STRING_TERMINATE, maxBufferLength)) {
                return false;
            }

            strcpy(dataReadP->sourceIPAddress, "");

            dataReadP->sourcePortNumber = ATCommon_Port_Number_Invalid;

            break;
        }
        case 3: {
            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
                    &pResponseCommand, dataReadP->data,
                    ATCOMMAND_ARGUMENT_DELIM, maxBufferLength)) {
                return false;
            }

            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(
                    &pResponseCommand, dataReadP->sourceIPAddress,
                    ATCOMMAND_ARGUMENT_DELIM,
                    sizeof(dataReadP->sourceIPAddress))) {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(
                    &pResponseCommand, &dataReadP->sourcePortNumber,
                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
                    ATCOMMAND_STRING_TERMINATE)) {
                return false;
            }

            break;
        }
        default:
            return false;
    }

    return true;
}

/**
 * @brief Send to Socket (using the AT%SOCKETDATA command).
 *
 * @param[in] socketID Socket ID.
 *
 * @param[in] data Data to send.
 *
 * @param[in] dataLength Length of data to send.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_SendToSocket(ATSocket_ID_t socketID, char *data,
                           ATSocket_Data_Length_t dataLength) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETDATA=\"SEND\",");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, socketID,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, dataLength,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(
            pRequestCommand, data, ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF,
                                        ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    if (!Adrastea_SendRequest(pRequestCommand)) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Set Socket Notification Events (using the AT%SOCKETEV command).
 *
 * @param[in] event Socket event type. See ATSocket_Event_t.
 *
 * @param[in] state Event State
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_SetSocketUnsolicitedNotificationEvents(
    ATSocket_Event_t event, ATCommon_Event_State_t state) {
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETEV=");

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, event,
            (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC),
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(
            pRequestCommand, state,
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

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Socket),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Data Received event arguments.
 *
 * @param[out] dataP ID of Socket where data was received is returned in this
 * argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ParseDataReceivedEvent(char *pEventArguments,
                                     ATSocket_ID_t *dataP) {
    if (dataP == NULL || pEventArguments == NULL) {
        return false;
    }

    char *argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(
            &argumentsP, dataP,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Socket Terminated event arguments.
 *
 * @param[out] dataP ID of Socket that was terminated is returned in this
 * argument.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ParseSocketTerminatedEvent(char *pEventArguments,
                                         ATSocket_ID_t *dataP) {
    if (dataP == NULL || pEventArguments == NULL) {
        return false;
    }

    char *argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(
            &argumentsP, dataP,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}

/**
 * @brief Parses the value of Socket Read event arguments.
 *
 * @param[out] dataP Socket Read Result si returned in this argument. See
 * ATSocket_Read_Result_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ParseSocketsReadEvent(char *pEventArguments,
                                    ATSocket_Read_Result_t *dataP) {
    if (dataP == NULL || pEventArguments == NULL) {
        return false;
    }

    char *argumentsP = pEventArguments;

    if (Adrastea_CountArgs(argumentsP) != 2) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            &argumentsP, &dataP->socketID,
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(
            &argumentsP, (uint8_t *)&dataP->socketState, ATSocket_State_Strings,
            ATSocket_State_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    return true;
}
