/**
 * \file
 * \brief File for the calypso of the WE IoT design kit.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */

#ifndef CALYPSO_H
#define CALYPSO_H

#include "ConfigPlatform.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CALYPSO_FILE_WRITE_SIZE_MAX 512
#define CALYPSO_LINE_MAX_SIZE 2048

#define FILENAME_MAX_LENGTH (uint8_t)180
#define MQTT_MAX_TOPIC_LENGTH 128
#define MQTT_MAX_MESSAGE_LENGTH 128

#define ATWLAN_BSSID_LENGTH (29)
#define ATWLAN_SSID_MAX_LENGTH (32)
#define ATWLAN_SECURITYKEY_LENGTH (32)

#define COMMAND_PREFIX "AT+"
#define COMMAND_DELIM (char)'='
#define CONFIRM_PREFIX (char)'+'
#define CONFIRM_DELIM (char)':'
#define EVENT_PREFIX (char)'+'
#define EVENT_DELIM (char)':'
#define ARGUMENT_DELIM (char)','
#define BITMASK_DELIM (char)'|'
#define CRLF "\r\n"

#define RESPONSE_OK "OK"
#define RESPONSE_Error "error"

#define STRING_TERMINATE '\0'
#define STRING_EMPTY ""
#define MQTT_SOCKET_INDEX 0

#define INTFLAGS_SIZE (uint16_t)(7)
#define INTFLAGS_SIZE8 (uint16_t)(1)
#define INTFLAGS_SIZE16 (uint16_t)(2)
#define INTFLAGS_SIZE32 (uint16_t)(4)

#define INTFLAGS_SIGN (uint16_t)(0x18)
#define INTFLAGS_SIGNED (uint16_t)(0x08)
#define INTFLAGS_UNSIGNED (uint16_t)(0x10)

#define INTFLAGS_NOTATION (uint16_t)(0x60)
#define INTFLAGS_NOTATION_HEX (uint16_t)(0x20)
#define INTFLAGS_NOTATION_DEC (uint16_t)(0x40)

#define FILENAME_MAX_LENGTH (uint8_t)180
#define FILE_MIN_SIZE (uint16_t)4096
#define FILE_FAILSAFE_MIN_SIZE (uint16_t)8192

#define ATFILE_OPEN_CREATE (uint32_t)(1 << 0)
#define ATFILE_OPEN_READ (uint32_t)(1 << 1)
#define ATFILE_OPEN_WRITE (uint32_t)(1 << 2)
#define ATFILE_OPEN_OVERWRITE (uint32_t)(1 << 3)
#define ATFILE_OPEN_CREATE_FAILSAFE (uint32_t)(1 << 4)
#define ATFILE_OPEN_CREATE_SECURE (uint32_t)(1 << 5)
#define ATFILE_OPEN_CREATE_NOSIGNATURE (uint32_t)(1 << 6)
#define ATFILE_OPEN_CREATE_STATIC_TOKEN (uint32_t)(1 << 7)
#define ATFILE_OPEN_CREATE_VENDOR_TOKEN (uint32_t)(1 << 8)
#define ATFILE_OPEN_CREATE_PUBLIC_WRITE (uint32_t)(1 << 9)
#define ATFILE_OPEN_CREATE_PUBLIC_READ (uint32_t)(1 << 10)
#define ATFILE_OPEN_NUM_OF_BITS 11

#define GENERATE_ENUM(PREFIX, ENUM) PREFIX##ENUM,
#define GENERATE_STRING(PREFIX, STRING) #STRING,

#define ATWLAN_SECURITY_TYPE(GENERATOR)        \
    GENERATOR(ATWLAN_SECURITY_TYPE_, OPEN)     \
    GENERATOR(ATWLAN_SECURITY_TYPE_, WEP)      \
    GENERATOR(ATWLAN_SECURITY_TYPE_, WPA)      \
    GENERATOR(ATWLAN_SECURITY_TYPE_, WPA2)     \
    GENERATOR(ATWLAN_SECURITY_TYPE_, WPA_WPA2) \
    GENERATOR(ATWLAN_SECURITY_TYPE_, NumberOfValues)

#define ATWLAN_SECURITY_EAP(GENERATOR)              \
    GENERATOR(ATWLAN_SECURITY_EAP_, TLS)            \
    GENERATOR(ATWLAN_SECURITY_EAP_, TTLS_TLS)       \
    GENERATOR(ATWLAN_SECURITY_EAP_, TTLS_MSCHAPv2)  \
    GENERATOR(ATWLAN_SECURITY_EAP_, TTLS_PSK)       \
    GENERATOR(ATWLAN_SECURITY_EAP_, PEAP0_TLS)      \
    GENERATOR(ATWLAN_SECURITY_EAP_, PEAP0_MSCHAPv2) \
    GENERATOR(ATWLAN_SECURITY_EAP_, PEAP0_PSK)      \
    GENERATOR(ATWLAN_SECURITY_EAP_, PEAP1_TLS)      \
    GENERATOR(ATWLAN_SECURITY_EAP_, PEAP1_PSK)      \
    GENERATOR(ATWLAN_SECURITY_EAP_, NumberOfValues)

#define ATMQTT_SECURITY_METHOD(GENERATOR)             \
    GENERATOR(ATMQTT_SECURITY_METHOD_, SSLV3)         \
    GENERATOR(ATMQTT_SECURITY_METHOD_, TLSV1)         \
    GENERATOR(ATMQTT_SECURITY_METHOD_, TLSV1_1)       \
    GENERATOR(ATMQTT_SECURITY_METHOD_, TLSV1_2)       \
    GENERATOR(ATMQTT_SECURITY_METHOD_, SSLV3_TLSV1_2) \
    GENERATOR(ATMQTT_SECURITY_METHOD_, NumberOfValues)

#define ATMQTT_CIPHER(GENERATOR)                                             \
    GENERATOR(ATMQTT_CIPHER_, SSL_RSA_WITH_RC4_128_SHA)                      \
    GENERATOR(ATMQTT_CIPHER_, SSL_RSA_WITH_RC4_128_MD5)                      \
    GENERATOR(ATMQTT_CIPHER_, TLS_RSA_WITH_AES_256_CBC_SHA)                  \
    GENERATOR(ATMQTT_CIPHER_, TLS_DHE_RSA_WITH_AES_256_CBC_SHA)              \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA)            \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_RSA_WITH_RC4_256_CBC_SHA)            \
    GENERATOR(ATMQTT_CIPHER_, TLS_RSA_WITH_AES_128_CBC_SHA256)               \
    GENERATOR(ATMQTT_CIPHER_, TLS_RSA_WITH_AES_256_CBC_SHA256)               \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256)       \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA)          \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA)          \
    GENERATOR(ATMQTT_CIPHER_, TLS_RSA_WITH_AES_128_GCM_SHA256)               \
    GENERATOR(ATMQTT_CIPHER_, TLS_RSA_WITH_AES_256_GCM_SHA384)               \
    GENERATOR(ATMQTT_CIPHER_, TLS_DHE_RSA_WITH_AES_128_GCM_SHA256)           \
    GENERATOR(ATMQTT_CIPHER_, TLS_DHE_RSA_WITH_AES_256_GCM_SHA384)           \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256)         \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384)         \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256)       \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384)       \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256) \
    GENERATOR(ATMQTT_CIPHER_, TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256)   \
    GENERATOR(ATMQTT_CIPHER_, TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256)     \
    GENERATOR(ATMQTT_CIPHER_, NumberOfValues)

#define ATMQTT_PROTOCOL(GENERATOR)      \
    GENERATOR(ATMQTT_PROTOCOL_, v3_1)   \
    GENERATOR(ATMQTT_PROTOCOL_, v3_1_1) \
    GENERATOR(ATMQTT_PROTOCOL_, NumberOfValues)

#define ATMQTT_QOS(GENERATOR)    \
    GENERATOR(ATMQTT_QOS_, QOS0) \
    GENERATOR(ATMQTT_QOS_, QOS1) \
    GENERATOR(ATMQTT_QOS_, QOS2) \
    GENERATOR(ATMQTT_QOS_, NumberOfValues)

#define ATMQTT_SET_OPTION(GENERATOR)         \
    GENERATOR(ATMQTT_SET_OPTION_, user)      \
    GENERATOR(ATMQTT_SET_OPTION_, password)  \
    GENERATOR(ATMQTT_SET_OPTION_, will)      \
    GENERATOR(ATMQTT_SET_OPTION_, keepAlive) \
    GENERATOR(ATMQTT_SET_OPTION_, clean)     \
    GENERATOR(ATMQTT_SET_OPTION_, NumberOfValues)

#define ATMQTT_CREATE_FLAGS_IP4 (1 << 0)
#define ATMQTT_CREATE_FLAGS_IP6 (1 << 1)
#define ATMQTT_CREATE_FLAGS_URL (1 << 2)
#define ATMQTT_CREATE_FLAGS_SEC (1 << 3)
#define ATMQTT_CREATE_FLAGS_SKIP_DOMAIN_VERIFY (1 << 4)
#define ATMQTT_CREATE_FLAGS_SKIP_CERT_VERIFY (1 << 5)
#define ATMQTT_CREATE_FLAGS_SKIP_DATE_VERIFY (1 << 6)
#define ATMQTT_CREATE_FLAGS_NUM_OF_BITS 7
    typedef enum Calypso_CNFStatus_t
    {
        Calypso_CNFStatus_Success = 0x00,
        Calypso_CNFStatus_Error,
        Calypso_CNFStatus_Failed,
        Calypso_CNFStatus_Invalid,
    } Calypso_CNFStatus_t;

    typedef enum ATWLAN_SecurityType_t
    {
        ATWLAN_SECURITY_TYPE(GENERATE_ENUM)
    } ATWLAN_SecurityType_t;

    typedef enum ATWLAN_Security_EAP_t
    {
        ATWLAN_SECURITY_EAP(GENERATE_ENUM)
    } ATWLAN_Security_EAP_t;

    typedef struct ATWLAN_SecurityParams_t
    {
        ATWLAN_SecurityType_t securityType;
        char securityKey[ATWLAN_SECURITYKEY_LENGTH];
    } ATWLAN_SecurityParams_t;

    typedef struct ATWLAN_SecurityEnterpriseParams_t
    {
        char extUser[32];
        char extAnonUser[32];
        ATWLAN_Security_EAP_t eapMethod;
    } ATWLAN_SecurityEnterpriseParams_t;

    typedef struct ATWLAN_ConnectionArguments_t
    {
        char SSID[ATWLAN_SSID_MAX_LENGTH];
        char BSSID[ATWLAN_BSSID_LENGTH];
        ATWLAN_SecurityParams_t securityParams;
        ATWLAN_SecurityEnterpriseParams_t securityExtParams;
    } ATWLAN_ConnectionArguments_t;

    typedef enum ATMQTT_SecurityMethods_t
    {
        ATMQTT_SECURITY_METHOD(GENERATE_ENUM)
    } ATMQTT_SecurityMethods_t;

    typedef enum ATMQTT_Ciphers_t
    {
        ATMQTT_CIPHER(GENERATE_ENUM)
    } ATMQTT_Ciphers_t;

    typedef enum ATMQTT_ProtocolVersion_t
    {
        ATMQTT_PROTOCOL(GENERATE_ENUM)
    } ATMQTT_ProtocolVersion_t;

    typedef enum ATMQTT_QoS_t
    {
        ATMQTT_QOS(GENERATE_ENUM)
    } ATMQTT_QoS_t;

    typedef enum ATMQTT_SetOption_t
    {
        ATMQTT_SET_OPTION(GENERATE_ENUM)
    } ATMQTT_SetOption_t;

    typedef struct ATMQTT_ServerInfo_t
    {
        char address[128];
        uint16_t port;

    } ATMQTT_ServerInfo_t;

    typedef struct ATMQTT_securityParams_t
    {
        ATMQTT_SecurityMethods_t securityMethod;
        ATMQTT_Ciphers_t cipher;
        char privateKeyFile[FILENAME_MAX_LENGTH + 1];
        char certificateFile[FILENAME_MAX_LENGTH + 1];
        char CAFile[FILENAME_MAX_LENGTH + 1];
        char DHKey[FILENAME_MAX_LENGTH + 1];
    } ATMQTT_securityParams_t;

    typedef enum Calypso_DataFormat_t
    {
        Calypso_DataFormat_Binary,
        Calypso_DataFormat_Base64,
        Calypso_DataFormat_NumberOfValues,
    } Calypso_DataFormat_t;

    typedef struct ATMQTT_connectionParams_t
    {
        ATMQTT_ProtocolVersion_t protocolVersion;
        uint8_t blockingSend;
        Calypso_DataFormat_t format;
    } ATMQTT_connectionParams_t;

    typedef struct ATMQTT_subscribeTopic_t
    {
        char topicString[MQTT_MAX_TOPIC_LENGTH];
        ATMQTT_QoS_t QoS;
    } ATMQTT_subscribeTopic_t;

    typedef struct ATMQTT_userOptions_t
    {
        char userName[256];
        char passWord[256];
    } ATMQTT_userOptions_t;

    typedef enum ATSocket_Family_t
    {
        ATSocket_Family_INET,
        ATSocket_Family_INET6,
        ATSocket_Family_NumberOfValues,
    } ATSocket_Family_t;
    typedef struct ATMQTT_setWillParams_t
    {
        char topic[MQTT_MAX_TOPIC_LENGTH];
        ATMQTT_QoS_t QoS;
        uint8_t retain;
        uint16_t messageLength;
        char message[MQTT_MAX_MESSAGE_LENGTH];
    } ATMQTT_setWillParams_t;

    bool Calypso_appendArgumentString(char *pOutString, const char *pInArgument, char delimeter);
    bool ATWLAN_addConnectionArguments(char *pOutString, ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim);
    bool Calypso_getNextArgumentString(char **pInArguments, char *pOutargument, char delim);
    bool Calypso_encodeBase64(uint8_t *inputData, uint32_t inputLength, uint8_t *outputData, uint32_t *outputLength);
    bool Calypso_decodeBase64(uint8_t *inputData, uint32_t inputLength, uint8_t *outputData, uint32_t *outputLength);
    bool Calypso_getCmdName(char **pInAtCmd, char *pCmdName, char delim);
    bool Calypso_getNextArgumentInt(char **pInArguments, void *pOutargument, uint16_t intflags, char delim);
    bool ATSocket_parseSocketFamily(const char *familyString, ATSocket_Family_t *pOutFamily);
    bool Calypso_StringToInt(void *pOutInt, const char *pInString, uint16_t intFlags);
    bool ATMQTT_addArgumentsCreate(char *pAtCommand, char *clientID, uint32_t flags,
                                   ATMQTT_ServerInfo_t serverInfo, ATMQTT_securityParams_t securityParams,
                                   ATMQTT_connectionParams_t connectionParams);
    bool Calypso_appendArgumentInt(char *pOutString, uint32_t pInValue, uint16_t intflags, char delimeter);

    bool ATMQTT_addArgumentsSet(char *pAtCommand, uint8_t index, uint8_t option, void *pValues);
    bool ATMQTT_addArgumentsPublish(char *pAtCommand, uint8_t index, char *topicString, ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char *pMessage);
    bool ATMQTT_addArgumentsSubscribe(char *pAtCommand, uint8_t index, uint8_t numOfTopics, ATMQTT_subscribeTopic_t *pTopics);

    bool ATFile_AddArgumentsFileOpen(char *pAtCommand, const char *fileName, uint32_t options, uint16_t fileSize);
    bool ATFile_AddArgumentsFileClose(char *pAtCommand, uint32_t fileID, const char *certName, const char *signature);
    bool ATFile_AddArgumentsFileDel(char *pAtCommand, const char *fileName, uint32_t secureToken);
    bool ATFile_AddArgumentsFileRead(char *pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToRead);
    bool ATFile_AddArgumentsFileWrite(char *pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeToBase64, uint16_t bytesToWrite, char *data);

    bool ATFile_ParseResponseFileOpen(char **pAtCommand, uint32_t *fileID, uint32_t *secureToken);
    bool ATFile_ParseResponseFileRead(char **pAtCommand, Calypso_DataFormat_t *pOutFormat, uint16_t *bytesRead, char *data);
    bool ATFile_ParseResponseFileWrite(char **pAtCommand, uint16_t *bytesWritten);
#ifdef __cplusplus
}
#endif

#endif