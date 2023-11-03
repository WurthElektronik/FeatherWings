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
 * @brief AT commands for socket functionality.
 */

#ifndef AT_SOCKET_H_INCLUDED
#define AT_SOCKET_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>

#include "ATFile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Socket family
 */
typedef enum ATSocket_Family_t
{
    ATSocket_Family_INET,
    ATSocket_Family_INET6,
    ATSocket_Family_NumberOfValues,
} ATSocket_Family_t;

/**
 * @brief Socket type
 */
typedef enum ATSocket_Type_t
{
    ATSocket_Type_Stream,
    ATSocket_Type_Datagram,
    ATSocket_Type_NumberOfValues,
} ATSocket_Type_t;

/**
 * @brief Socket protocol
 */
typedef enum ATSocket_Protocol_t
{
    ATSocket_Protocol_TCP,
    ATSocket_Protocol_UDP,
    ATSocket_Protocol_SEC,
    ATSocket_Protocol_NumberOfValues,
} ATSocket_Protocol_t;

/**
 * @brief Level parameter for ATSocket_SetSocketOption()
 */
typedef enum ATSocket_SockOptLevel_t
{
    ATSocket_SockOptLevel_Socket,
    ATSocket_SockOptLevel_IP,
    ATSocket_SockOptLevel_NumberOfValues,
} ATSocket_SockOptLevel_t;

/**
 * @brief Options for ATSocket_SetSocketOption() for level ATSocket_SockOptLevel_Socket.
 */
typedef enum ATSocket_SockOptSocket_t
{
    ATSocket_SockOptSocket_KeepAlive,
    ATSocket_SockOptSocket_KeepAliveTime,
    ATSocket_SockOptSocket_RXNoIPBoundary,
    ATSocket_SockOptSocket_RCVTimeout,
    ATSocket_SockOptSocket_RCVBuf,
    ATSocket_SockOptSocket_NonBlocking,
    ATSocket_SockOptSocket_SecMethod,
    ATSocket_SockOptSocket_SecureMask,
    ATSocket_SockOptSocket_SecureFilesCAFileName,
    ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName,
    ATSocket_SockOptSocket_SecureFilesCertificateFileName,
    ATSocket_SockOptSocket_SecureFilesDHKeyFileName,
    ATSocket_SockOptSocket_SecureDomainNameVerification,
    ATSocket_SockOptSocket_DisableCertificateStore,
    ATSocket_SockOptSocket_NumberOfValues,
} ATSocket_SockOptSocket_t;

/**
 * @brief Options for ATSocket_SetSocketOption() for level ATSocket_SockOptLevel_IP.
 */
typedef enum ATSocket_SockOptIP_t
{
    ATSocket_SockOptIP_MulticastTTL,
    ATSocket_SockOptIP_AddMembership,
    ATSocket_SockOptIP_DropMembership,
    ATSocket_SockOptIP_NumberOfValues,
} ATSocket_SockOptIP_t;

/**
 * @brief Possible values for ATSocket_SockOptSocket_SecMethod option used with
 * ATSocket_SetSocketOption() for level ATSocket_SockOptLevel_Socket.
 */
typedef enum ATSocket_SockOptSecMethod_t
{
    ATSocket_SockOptSecMethod_SSLv3,
    ATSocket_SockOptSecMethod_TLSv1,
    ATSocket_SockOptSecMethod_TLSv1_1,
    ATSocket_SockOptSecMethod_TLSv1_2,
    ATSocket_SockOptSecMethod_SSLv3_TLSv1_2,
    ATSocket_SockOptSecMethod_NumberOfValues,
} ATSocket_SockOptSecMethod_t;

/**
 * @brief Socket ciphers (flags)
 */
typedef enum ATSocket_Cipher_t
{
    ATSocket_Cipher_SSL_RSA_WITH_RC4_128_SHA                        = (1 << 0),
    ATSocket_Cipher_SSL_RSA_WITH_RC4_128_MD5                        = (1 << 1),
    ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA                    = (1 << 2),
    ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_256_CBC_SHA                = (1 << 3),
    ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA              = (1 << 4),
    ATSocket_Cipher_TLS_ECDHE_RSA_WITH_RC4_128_SHA                  = (1 << 5),
    ATSocket_Cipher_TLS_RSA_WITH_AES_128_CBC_SHA256                 = (1 << 6),
    ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA256                 = (1 << 7),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256         = (1 << 8),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA            = (1 << 9),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA            = (1 << 10),
    ATSocket_Cipher_TLS_RSA_WITH_AES_128_GCM_SHA256                 = (1 << 11),
    ATSocket_Cipher_TLS_RSA_WITH_AES_256_GCM_SHA384                 = (1 << 12),
    ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256             = (1 << 13),
    ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384             = (1 << 14),
    ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256           = (1 << 15),
    ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384           = (1 << 16),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256         = (1 << 17),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384         = (1 << 18),
    ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256   = (1 << 19),
    ATSocket_Cipher_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256     = (1 << 20),
    ATSocket_Cipher_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256       = (1 << 21),
    ATSocket_Cipher_NumberOfValues                                  = 22
} ATSocket_Cipher_t;

/**
 * @brief Option value type used for options accepting true/false.
 */
typedef enum ATSocket_OptionState_t
{
    ATSocket_OptionState_Disabled = 0,
    ATSocket_OptionState_Enabled = 1
} ATSocket_OptionState_t;

/**
 * @brief Socket configuration structure.
 */
typedef struct ATSocket_Descriptor_t
{
    ATSocket_Family_t family;
    uint16_t port;
    char address[CALYPSO_MAX_IP_ADDRESS_LENGTH];
} ATSocket_Descriptor_t;

/**
 * @brief Receive timeout option struct.
 * @see ATSocket_SockOptSocket_RCVTimeout
 */
typedef struct ATSocket_ReceiveTimeout_t
{
    uint32_t seconds;
    uint32_t microseconds;
} ATSocket_ReceiveTimeout_t;

/**
 * @brief Used as parameter for ATSocket_SetSocketOption() when joining/leaving a multicast group.
 * @see ATSocket_SockOptIP_AddMembership, ATSocket_SockOptIP_DropMembership
 */
typedef struct ATSocket_MulticastGroup_t
{
    char ipv4Address[32];
    char interfaceAddress[32];
} ATSocket_MulticastGroup_t;

/**
 * @brief Used to set option values using ATSocket_SetSocketOption()
 */
typedef union ATSocket_Options_t
{
    ATSocket_OptionState_t keepAlive;                               /**< Enable/disable TCP keep active message (used with ATSocket_SockOptSocket_KeepAlive)*/
    uint32_t keepAliveTimeSeconds;                                  /**< Keep alive timeout (used with ATSocket_SockOptSocket_KeepAliveTime) */
    ATSocket_OptionState_t rxNoIpBoundary;                          /**< Enable/disable RX IP boundary (used with ATSocket_SockOptSocket_RXNoIPBoundary) */
    ATSocket_ReceiveTimeout_t rcvTimeout;                           /**< Timeout value that specifies maximum amount of time an input function waits until it completes (used with ATSocket_SockOptSocket_RCVTimeout) */
    uint32_t rcvBuf;                                                /**< TCP maximum receive window size (used with ATSocket_SockOptSocket_RCVBuf) */
    ATSocket_OptionState_t nonBlocking;                             /**< Set socket to non-blocking (used with ATSocket_SockOptSocket_NonBlocking) */
    ATSocket_SockOptSecMethod_t secMethod;                          /**< Sets security method to socket (used with ATSocket_SockOptSocket_SecMethod) */
    uint32_t secureMask;                                            /**< Sets specific ciphers as bitmask (of ATSocket_Cipher_t) to TCP secured socket (default value: all ciphers, used with ATSocket_SockOptSocket_SecureMask) */
    char fileName[ATFILE_FILENAME_MAX_LENGTH];                      /**< File name used for ATSocket_SockOptSocket_SecureFilesCAFileName, ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName, ATSocket_SockOptSocket_SecureFilesCertificateFileName, ATSocket_SockOptSocket_SecureFilesDHKeyFileName */
    char secureDomainNameVerification[ATFILE_FILENAME_MAX_LENGTH];  /**< Set a domain name, to check in SSL client connection (used with ATSocket_SockOptSocket_SecureDomainNameVerification) */
    ATSocket_OptionState_t disableCertificateStore;                 /**< Disables the use of the on-board root CA catalogue */
    uint32_t multicastTTL;                                          /**< Set the time-to-live value of outgoing multicast packets (used with ATSocket_SockOptIP_MulticastTTL) */
    ATSocket_MulticastGroup_t multicastGroup;                       /**< Used for joining/leaving a multicast group (UDP, used with ATSocket_SockOptIP_AddMembership, ATSocket_SockOptIP_DropMembership) */
} ATSocket_Options_t;


extern bool ATSocket_Create(ATSocket_Family_t family,
                            ATSocket_Type_t type,
                            ATSocket_Protocol_t protocol,
                            uint8_t *socketID);
extern bool ATSocket_Close(uint8_t socketID);
extern bool ATSocket_Bind(uint8_t socketID, ATSocket_Descriptor_t socket);
extern bool ATSocket_Listen(uint8_t socketID, uint16_t backlog);
extern bool ATSocket_Connect(uint8_t socketID, ATSocket_Descriptor_t remoteSocket);
extern bool ATSocket_Accept(uint8_t socketID, ATSocket_Family_t family);
extern bool ATSocket_SetSocketOption(uint8_t socketID,
                                     ATSocket_SockOptLevel_t level,
                                     uint8_t option,
                                     ATSocket_Options_t *pValues);
extern bool ATSocket_GetSocketOption(uint8_t socketID,
                                     ATSocket_SockOptLevel_t level,
                                     uint8_t option,
                                     ATSocket_Options_t *pValues);
extern bool ATSocket_Receive(uint8_t socketID,
                             Calypso_DataFormat_t format,
                             uint16_t length);
extern bool ATSocket_ReceiveFrom(uint8_t socketID,
                                 ATSocket_Descriptor_t remoteSocket,
                                 Calypso_DataFormat_t format,
                                 uint16_t length);
extern bool ATSocket_Send(uint8_t socketID,
                          Calypso_DataFormat_t format,
                          bool encodeAsBase64,
                          uint16_t length,
                          char *data,
                          uint16_t *bytesSent);
extern bool ATSocket_SendTo(uint8_t socketID,
                            ATSocket_Descriptor_t *remoteSocket,
                            Calypso_DataFormat_t format,
                            bool encodeAsBase64,
                            uint16_t length,
                            char *data,
                            uint16_t *bytesSent);

extern bool ATSocket_ParseSocketFamily(const char *familyString,
                                       ATSocket_Family_t *pOutFamily);
extern bool ATSocket_GetSocketFamilyString(ATSocket_Family_t family, char *pOutFamilyStr);

extern bool ATSocket_AppendSocketDescriptor(char *pAtCommand, ATSocket_Descriptor_t socket, char lastDelim);
extern bool ATSocket_AppendCipherMask(char *pOutStr, uint32_t cipherMask);

#ifdef __cplusplus
}
#endif

#endif /* AT_SOCKET_H_INCLUDED */
