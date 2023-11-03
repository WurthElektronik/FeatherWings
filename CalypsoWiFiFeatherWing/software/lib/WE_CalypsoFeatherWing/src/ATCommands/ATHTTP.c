/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for HTTP client functionality.
 */

#include <ATCommands.h>
#include "ATHTTP.h"

#include "Calypso.h"

static const char *ATHTTP_ConnectFlags_Strings[ATHTTP_ConnectFlags_NumberOfValues] = {
    "ignore_proxy",
    "host_exist"
};

static const char *ATHTTP_Method_Strings[ATHTTP_Method_NumberOfValues] = {
    "get",
    "post",
    "head",
    "options",
    "put",
    "del",
    "connect"
};

static const char *ATHTTP_RequestFlags_Strings[ATHTTP_RequestFlags_NumberOfValues] = {
    "chunk_start",
    "chunk_end",
    "drop_body"
};

static const char *ATHTTP_HeaderField_Strings[ATHTTP_HeaderField_NumberOfValues] = {
    "res_age",
    "res_allow",
    "res_cache_control",
    "res_connection",
    "res_content_encoding",
    "res_content_language",
    "res_content_length",
    "res_content_location",
    "res_content_range",
    "res_content_type",
    "res_date",
    "res_etag",
    "res_expires",
    "res_last_modified",
    "res_location",
    "res_proxy_auth",
    "res_retry_after",
    "res_server",
    "res_set_cookie",
    "res_trailer",
    "res_tx_encoding",
    "res_upgrade",
    "res_vary",
    "res_via",
    "res_www_auth",
    "res_warning",
    "req_accept",
    "req_accept_charset",
    "req_accept_encoding",
    "req_accept_language",
    "req_allow",
    "req_auth",
    "req_cache_control",
    "req_connection",
    "req_content_encoding",
    "req_content_language",
    "req_content_location",
    "req_content_type",
    "req_cookie",
    "req_date",
    "req_expect",
    "req_forwarded",
    "req_from",
    "req_host",
    "req_if_match",
    "req_if_modified_since",
    "req_if_none_match",
    "req_if_range",
    "req_if_unmodified_since",
    "req_origin",
    "req_proxy_auth",
    "req_range",
    "req_te",
    "req_tx_encoding",
    "req_upgrade",
    "req_user_agent",
    "req_via",
    "req_warning"
};

static const char *ATHTTP_HeaderPersistency_Strings[ATHTTP_HeaderPersistency_NumberOfValues] = {
    "not_persistent",
    "persistent"
};

/**
 * @brief Creates an HTTP client.
 *
 * @param[out] clientHandle Handle (index) of the created HTTP client
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_Create(uint8_t *clientHandle)
{
    if (!Calypso_SendRequest("AT+httpCreate\r\n"))
    {
        return false;
    }

    char *pRespondCommand = AT_commandBuffer;
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *cmd = "+httpcreate:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand,
                                      clientHandle,
                                      ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                      ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Deletes an HTTP client.
 *
 * @param[in] clientHandle Handle of the HTTP client to be deleted
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_Destroy(uint8_t clientHandle)
{
    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpDestroy=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Establishes an HTTP connection.
 *
 * @param[in] clientHandle Handle of the HTTP client to connect
 * @param[in] host Name of host to connect to
 * @param[in] flags Connection flags. See ATHTTP_ConnectFlag_t.
 * @param[in] privateKey Private key file name (optional, set to NULL if not used).
 * @param[in] certificate Client certificate file name (optional, set to NULL if not used).
 * @param[in] rootCaCertificate Root CA certificate file name (optional, set to NULL if not used).
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_Connect(uint8_t clientHandle,
                    const char *host,
                    uint8_t flags,
                    const char *privateKey,
                    const char *certificate,
                    const char *rootCaCertificate)
{
    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpConnect=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      host,
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBitmask(pRequestCommand,
                                       ATHTTP_ConnectFlags_Strings,
                                       ATHTTP_ConnectFlags_NumberOfValues,
                                       flags,
                                       ATCOMMAND_ARGUMENT_DELIM,
                                       AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      privateKey,
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      certificate,
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      rootCaCertificate,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpConnect), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Closes an HTTP connection.
 *
 * @param[in] clientHandle Handle of the HTTP client to disconnect
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_Disconnect(uint8_t clientHandle)
{
    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpDisconnect=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Sets a proxy server for HTTP communication.
 *
 * @param[in] proxy Proxy server info
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_SetProxy(ATSocket_Descriptor_t proxy)
{
    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSetProxy=");
    if (!ATSocket_AppendSocketDescriptor(pRequestCommand, proxy, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Sends an HTTP request.
 *
 * @param[in] clientHandle Handle of the HTTP client to be used for sending the request
 * @param[in] method HTTP method to be used
 * @param[in] uri HTTP server address or URL
 * @param[in] flags HTTP Request flags (see ATHTTP_RequestFlags_t)
 * @param[in] format Format in which the request's data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes to send as payload of the request (see argument data)
 * @param[in] data Payload of the request
 * @param[out] status HTTP status code (usually 200 in case of success, else failure)
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_SendRequest(uint8_t clientHandle,
                        ATHTTP_Method_t method,
                        const char *uri,
                        uint8_t flags,
                        Calypso_DataFormat_t format,
                        bool encodeAsBase64,
                        uint16_t length,
                        const char *data,
                        uint32_t *status)
{
    *status = 0;

    if (ATHTTP_Method_Head == method)
    {
        /* HEAD request is always without body. Must use this flag, otherwise the
         * next call to ATHTTP_ReadResponseBody will cause the module to hang. */
        flags |= ATHTTP_RequestFlags_DropBody;
    }

    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded = Calypso_GetBase64EncBufSize(length);
        char base64Buffer[lengthEncoded];
        Calypso_EncodeBase64((uint8_t *) data,
                             length,
                             (uint8_t *) base64Buffer,
                             &lengthEncoded);

        /* Recursively call ATHTTP_SendRequest() with the encoded binary data (excluding '\0') */
        return ATHTTP_SendRequest(clientHandle,
                                  method,
                                  uri,
                                  flags,
                                  format,
                                  false,
                                  lengthEncoded - 1,
                                  base64Buffer,
                                  status);
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSendReq=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATHTTP_Method_Strings[method],
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      uri,
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBitmask(pRequestCommand,
                                       ATHTTP_RequestFlags_Strings,
                                       ATHTTP_RequestFlags_NumberOfValues,
                                       flags,
                                       ATCOMMAND_ARGUMENT_DELIM,
                                       AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }


    const char *cmd = "+httpsendreq:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand,
                                      status,
                                      ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                      ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Fetches the response to the last HTTP request from the Calypso module.
 *
 * @param[in] clientHandle HTTP client handle
 * @param[in] format Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[in] length Number of bytes to fetch
 * @param[out] responseBody The returned HTTP response data. The payload of the response is contained in the body attribute
 *                          of responseBody. Note that, depending on the length argument, the response might be split into
 *                          chunks. The hasMoreData attribute of responseBody is set to true, if the body data is fragmented
 *                          and there is more data to be fetched.
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_ReadResponseBody(uint8_t clientHandle,
                             Calypso_DataFormat_t format,
                             bool decodeBase64,
                             uint16_t length,
                             ATHTTP_ResponseBody_t *responseBody)
{
    responseBody->clientHandle = 0;
    responseBody->format = Calypso_DataFormat_Binary;
    responseBody->hasMoreData = false;
    responseBody->length = 0;

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpReadResBody=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }


    const char *cmd = "+httpreadresbody:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &responseBody->clientHandle,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &responseBody->hasMoreData,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &responseBody->format,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &responseBody->length,
                                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (responseBody->length > 0)
    {
        if (decodeBase64)
        {
            uint32_t decodedSize = Calypso_GetBase64DecBufSize((uint8_t*) pRespondCommand, responseBody->length);

            if (decodedSize - 1 > length)
            {
                return false;
            }

            uint32_t encodedSize = responseBody->length;
            responseBody->length = decodedSize - 1;

            return Calypso_DecodeBase64((uint8_t*) pRespondCommand,
                                        encodedSize,
                                        (uint8_t*) responseBody->body,
                                        &decodedSize);
        }
        else
        {
            if (responseBody->length > length)
            {
                return false;
            }

            return ATCommand_GetNextArgumentString(&pRespondCommand,
                                                 responseBody->body,
                                                 ATCOMMAND_STRING_TERMINATE,
                                                 sizeof(responseBody->body));
        }
    }

    responseBody->body[0] = '\0';
    return true;
}

/**
 * @brief Sets HTTP header data for the next HTTP request(s).
 *
 * When used with a request field (prefix "ATHTTP_HeaderField_Request"), the field's value is set.
 * When used with a response field (prefix "ATHTTP_HeaderField_Response"), a response filter is set for the field.
 *
 * @param[in] clientHandle HTTP client handle
 * @param[in] field Header field to be set
 * @param[in] persistency Specifies if the supplied header field value is persistent or not persistent
 * @param[in] format Format in which the header field value is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes in header field value. Set to zero to remove a request header.
 * @param[in] data Header field value
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_SetHeader(uint8_t clientHandle,
                      ATHTTP_HeaderField_t field,
                      ATHTTP_HeaderPersistency_t persistency,
                      Calypso_DataFormat_t format,
                      bool encodeAsBase64,
                      uint16_t length,
                      const char *data)
{
    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded = Calypso_GetBase64EncBufSize(length);
        char base64Buffer[lengthEncoded];
        Calypso_EncodeBase64((uint8_t *) data,
                             length,
                             (uint8_t *) base64Buffer,
                             &lengthEncoded);

        /* Recursively call ATHTTP_SendRequest() with the encoded binary data (excluding '\0') */
        return ATHTTP_SetHeader(clientHandle,
                                field,
                                persistency,
                                format,
                                false,
                                lengthEncoded - 1,
                                base64Buffer);
    }

    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSetHeader=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATHTTP_HeaderField_Strings[field],
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATHTTP_HeaderPersistency_Strings[persistency],
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Gets HTTP header data.
 *
 * @param[in] clientHandle HTTP client handle
 * @param[in] field Header field to be fetched
 * @param[in] format Format in which the header data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[in] length Number of bytes to fetch
 * @param[out] header The returned header data
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_GetHeader(uint8_t clientHandle,
                      ATHTTP_HeaderField_t field,
                      Calypso_DataFormat_t format,
                      bool decodeBase64,
                      uint16_t length,
                      ATHTTP_HeaderData_t *header)
{
    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpGetHeader=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                   clientHandle,
                                   ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                   ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATHTTP_HeaderField_Strings[field],
                                      ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand,
                                      ATCOMMAND_CRLF,
                                      ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }


    const char *cmd = "+httpgetheader:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &header->clientHandle,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &header->format,
                                    ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                    &header->length,
                                    ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                    ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (header->length > 0)
    {
        if (decodeBase64)
        {
            uint32_t decodedSize = Calypso_GetBase64DecBufSize((uint8_t*) pRespondCommand, header->length);

            if (decodedSize - 1 > length)
            {
                return false;
            }

            uint32_t encodedSize = header->length;
            header->length = decodedSize - 1;

            return Calypso_DecodeBase64((uint8_t*) pRespondCommand,
                                        encodedSize,
                                        (uint8_t*) header->data,
                                        &decodedSize);
        }
        else
        {
            if (header->length > length)
            {
                return false;
            }

            return ATCommand_GetNextArgumentString(&pRespondCommand,
                                                 header->data,
                                                 ATCOMMAND_STRING_TERMINATE,
                                                 sizeof(header->data));
        }
    }

    header->data[0] = '\0';
    return true;
}

/**
 * @brief Sends a custom HTTP response in reply to a client's HTTP GET request.
 *
 * @param[in] format Format in which the response data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes in response data.
 * @param[in] data Response content
 *
 * @return true if successful, false otherwise
 */
bool ATHTTP_SendCustomResponse(Calypso_DataFormat_t format,
                               bool encodeAsBase64,
                               uint16_t length,
                               const char *data)
{
    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded = Calypso_GetBase64EncBufSize(length);
        char base64Buffer[lengthEncoded];
        Calypso_EncodeBase64((uint8_t *) data,
                             length,
                             (uint8_t *) base64Buffer,
                             &lengthEncoded);

        /* Recursively call ATHTTP_SendCustomResponse() with the encoded binary data (excluding '\0') */
        return ATHTTP_SendCustomResponse(format,
                                         false,
                                         lengthEncoded - 1,
                                         base64Buffer);
    }

    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpCustomResponse=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}
