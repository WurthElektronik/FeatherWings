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

#ifndef AT_HTTP_H_INCLUDED
#define AT_HTTP_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>

#include "ATSocket.h"

#define ATHTTP_RECEIVE_BUFFER_SIZE CALYPSO_RECEIVE_BUFFER_SIZE
#define ATHTTP_RECEIVE_HEADER_SIZE CALYPSO_RECEIVE_BUFFER_SIZE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Connection flags. Used as parameter for ATHTTP_Connect().
 */
typedef enum ATHTTP_ConnectFlags_t
{
    ATHTTP_ConnectFlags_None             = 0,
    ATHTTP_ConnectFlags_IgnoreProxy      = (1 << 0),
    ATHTTP_ConnectFlags_HostExist        = (1 << 1),
    ATHTTP_ConnectFlags_NumberOfValues   = 2
} ATHTTP_ConnectFlags_t;

/**
 * @brief HTTP methods. Used as parameter for ATHTTP_SendRequest().
 */
typedef enum ATHTTP_Method_t
{
    ATHTTP_Method_Get,
    ATHTTP_Method_Post,
    ATHTTP_Method_Head,
    ATHTTP_Method_Options,
    ATHTTP_Method_Put,
    ATHTTP_Method_Delete,
    ATHTTP_Method_Connect,
    ATHTTP_Method_NumberOfValues
} ATHTTP_Method_t;

/**
 * @brief HTTP request flags. Used as parameter for ATHTTP_SendRequest().
 */
typedef enum ATHTTP_RequestFlags_t
{
    ATHTTP_RequestFlags_None            = 0,
    ATHTTP_RequestFlags_ChunkStart      = (1 << 0),
    ATHTTP_RequestFlags_ChunkEnd        = (1 << 1),
    ATHTTP_RequestFlags_DropBody        = (1 << 2),
    ATHTTP_RequestFlags_NumberOfValues  = 3
} ATHTTP_RequestFlags_t;

/**
 * @brief HTTP header fields. Used as parameter for ATHTTP_SetHeader() and ATHTTP_GetHeader().
 */
typedef enum ATHTTP_HeaderField_t
{
    ATHTTP_HeaderField_ResponseAge,
    ATHTTP_HeaderField_ResponseAllow,
    ATHTTP_HeaderField_ResponseCacheControl,
    ATHTTP_HeaderField_ResponseConnection,
    ATHTTP_HeaderField_ResponseContentEncoding,
    ATHTTP_HeaderField_ResponseContentLanguage,
    ATHTTP_HeaderField_ResponseContentLength,
    ATHTTP_HeaderField_ResponseContentLocation,
    ATHTTP_HeaderField_ResponseContentRange,
    ATHTTP_HeaderField_ResponseContentType,
    ATHTTP_HeaderField_ResponseDate,
    ATHTTP_HeaderField_ResponseEtag,
    ATHTTP_HeaderField_ResponseExpires,
    ATHTTP_HeaderField_ResponseLastModified,
    ATHTTP_HeaderField_ResponseLocation,
    ATHTTP_HeaderField_ResponseProxyAuth,
    ATHTTP_HeaderField_ResponseRetryAfter,
    ATHTTP_HeaderField_ResponseServer,
    ATHTTP_HeaderField_ResponseSetCookie,
    ATHTTP_HeaderField_ResponseTrailer,
    ATHTTP_HeaderField_ResponseTxEncoding,
    ATHTTP_HeaderField_ResponseUpgrade,
    ATHTTP_HeaderField_ResponseVary,
    ATHTTP_HeaderField_ResponseVia,
    ATHTTP_HeaderField_ResponseWwwAuth,
    ATHTTP_HeaderField_ResponseWarning,
    ATHTTP_HeaderField_RequestAccept,
    ATHTTP_HeaderField_RequestAcceptCharset,
    ATHTTP_HeaderField_RequestAcceptEncoding,
    ATHTTP_HeaderField_RequestAcceptLanguage,
    ATHTTP_HeaderField_RequestAllow,
    ATHTTP_HeaderField_RequestAuth,
    ATHTTP_HeaderField_RequestCacheControl,
    ATHTTP_HeaderField_RequestConnection,
    ATHTTP_HeaderField_RequestContentEncoding,
    ATHTTP_HeaderField_RequestContentLanguage,
    ATHTTP_HeaderField_RequestContentLocation,
    ATHTTP_HeaderField_RequestContentType,
    ATHTTP_HeaderField_RequestCookie,
    ATHTTP_HeaderField_RequestDate,
    ATHTTP_HeaderField_RequestExpect,
    ATHTTP_HeaderField_RequestForwarded,
    ATHTTP_HeaderField_RequestFrom,
    ATHTTP_HeaderField_RequestHost,
    ATHTTP_HeaderField_RequestIfMatch,
    ATHTTP_HeaderField_RequestIfModifiedSince,
    ATHTTP_HeaderField_RequestIfNoneMatch,
    ATHTTP_HeaderField_RequestIfRange,
    ATHTTP_HeaderField_RequestIfUnmodifiedSince,
    ATHTTP_HeaderField_RequestOrigin,
    ATHTTP_HeaderField_RequestProxyAuth,
    ATHTTP_HeaderField_RequestRange,
    ATHTTP_HeaderField_RequestTe,
    ATHTTP_HeaderField_RequestTxEncoding,
    ATHTTP_HeaderField_RequestUpgrade,
    ATHTTP_HeaderField_RequestUserAgent,
    ATHTTP_HeaderField_RequestVia,
    ATHTTP_HeaderField_RequestWarning,
    ATHTTP_HeaderField_NumberOfValues
} ATHTTP_HeaderField_t;

/**
 * @brief HTTP header persistency parameter. Used as parameter for ATHTTP_SetHeader().
 */
typedef enum ATHTTP_HeaderPersistency_t
{
    ATHTTP_HeaderPersistency_NotPersistent,
    ATHTTP_HeaderPersistency_Persistent,
    ATHTTP_HeaderPersistency_NumberOfValues
} ATHTTP_HeaderPersistency_t;

/**
 * @brief Stores HTTP response data.
 *
 * @see ATHTTP_ReadResponseBody()
 */
typedef struct ATHTTP_ResponseBody_t
{
    /**
     * @brief HTTP client handle.
     */
    uint8_t clientHandle;

    /**
     * @brief Indicates if there is more data to be fetched (response body).
     */
    bool hasMoreData;

    /**
     * @brief Format of the response body data.
     */
    Calypso_DataFormat_t format;

    /**
     * @brief Number of bytes in response body buffer.
     */
    uint16_t length;

    /**
     * @brief Response body.
     * Note that the response might be split into chunks. The attribute hasMoreData is
     * set to true, if the body data is fragmented and there is more data to be fetched.
     */
    char body[ATHTTP_RECEIVE_BUFFER_SIZE];
} ATHTTP_ResponseBody_t;

/**
 * @brief Stores HTTP header data.
 *
 * @see ATHTTP_GetHeader()
 */
typedef struct ATHTTP_HeaderData_t
{
    /**
     * @brief HTTP client handle.
     */
    uint8_t clientHandle;

    /**
     * @brief Format of the header data.
     */
    Calypso_DataFormat_t format;

    /**
     * @brief Number of bytes in header data buffer.
     */
    uint16_t length;

    /**
     * @brief Header data.
     */
    char data[ATHTTP_RECEIVE_HEADER_SIZE];
} ATHTTP_HeaderData_t;

extern bool ATHTTP_Create(uint8_t *clientHandle);
extern bool ATHTTP_Destroy(uint8_t clientHandle);
extern bool ATHTTP_Connect(uint8_t clientHandle,
                           const char *host,
                           uint8_t flags,
                           const char *privateKey,
                           const char *certificate,
                           const char *rootCaCertificate);
extern bool ATHTTP_Disconnect(uint8_t clientHandle);
extern bool ATHTTP_SetProxy(ATSocket_Descriptor_t proxy);
extern bool ATHTTP_SendRequest(uint8_t clientHandle,
                               ATHTTP_Method_t method,
                               const char *uri,
                               uint8_t flags,
                               Calypso_DataFormat_t format,
                               bool encodeAsBase64,
                               uint16_t length,
                               const char *data,
                               uint32_t *status);
extern bool ATHTTP_ReadResponseBody(uint8_t clientHandle,
                                    Calypso_DataFormat_t format,
                                    bool decodeBase64,
                                    uint16_t length,
                                    ATHTTP_ResponseBody_t *responseBody);
extern bool ATHTTP_SetHeader(uint8_t clientHandle,
                             ATHTTP_HeaderField_t field,
                             ATHTTP_HeaderPersistency_t persistency,
                             Calypso_DataFormat_t format,
                             bool encodeAsBase64,
                             uint16_t length,
                             const char *data);
extern bool ATHTTP_GetHeader(uint8_t clientHandle,
                             ATHTTP_HeaderField_t field,
                             Calypso_DataFormat_t format,
                             bool decodeBase64,
                             uint16_t length,
                             ATHTTP_HeaderData_t *header);
extern bool ATHTTP_SendCustomResponse(Calypso_DataFormat_t format,
                                      bool encodeAsBase64,
                                      uint16_t length,
                                      const char *data);

#ifdef __cplusplus
}
#endif

#endif /* AT_HTTP_H_INCLUDED */
