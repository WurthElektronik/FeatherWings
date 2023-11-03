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
 * @brief AT commands for MQTT functionality.
 */

#ifndef AT_MQTT_H_INCLUDED
#define AT_MQTT_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>

#include "ATFile.h"

#define MQTT_MAX_TOPIC_LENGTH 128 /**< Maximum length of topic names */
#define MQTT_MAX_MESSAGE_LENGTH \
    128 /**< Maximum length of message (e.g. used in set will command) */
#define MQTT_MAX_NUM_TOPICS_TO_SUBSCRIBE                                     \
    4 /**< Maximum number of topics that can be subscribed / unsubscribed in \
         one go */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flags for ATMQTT_Create()
 */
typedef enum ATMQTT_CreateFlags_t {
    ATMQTT_CreateFlags_IPv4 = (1 << 0), /**< IPv4 connection */
    ATMQTT_CreateFlags_IPv6 = (1 << 1), /**< IPv6 connection */
    ATMQTT_CreateFlags_URL =
        (1 << 2), /**< Server address is an URL and not an IP address */
    ATMQTT_CreateFlags_Secure = (1 << 3),           /**< Secure connection */
    ATMQTT_CreateFlags_SkipDomainVerify = (1 << 4), /**< Skip domain verify */
    ATMQTT_CreateFlags_SkipCertVerify =
        (1 << 5), /**< Skip certificate verify */
    ATMQTT_CreateFlags_SkipDateVerify = (1 << 6), /**< Skip date verify */
    ATMQTT_CreateFlags_NumberOfValues =
        7 /**< Number of flags in this enumeration */
} ATMQTT_CreateFlags_t;

/**
 * @brief MQTT security methods
 */
typedef enum ATMQTT_SecurityMethod_t {
    ATMQTT_SecurityMethod_SSLV3,
    ATMQTT_SecurityMethod_TLSV1,
    ATMQTT_SecurityMethod_TLSV1_1,
    ATMQTT_SecurityMethod_TLSV1_2,
    ATMQTT_SecurityMethod_SSLV3_TLSV1_2,
    ATMQTT_SecurityMethod_NumberOfValues
} ATMQTT_SecurityMethod_t;

/**
 * @brief MQTT protocol versions
 */
typedef enum ATMQTT_ProtocolVersion_t {
    ATMQTT_ProtocolVersion_v3_1,
    ATMQTT_ProtocolVersion_v3_1_1,
    ATMQTT_ProtocolVersion_NumberOfValues
} ATMQTT_ProtocolVersion_t;

/**
 * @brief MQTT quality of service
 */
typedef enum ATMQTT_QoS_t {
    ATMQTT_QoS_QoS0,
    ATMQTT_QoS_QoS1,
    ATMQTT_QoS_QoS2,
    ATMQTT_QoS_NumberOfValues
} ATMQTT_QoS_t;

extern const char *ATMQTT_QoSStrings[ATMQTT_QoS_NumberOfValues];

/**
 * @brief Options for ATMQTT_Set()
 */
typedef enum ATMQTT_SetOption_t {
    ATMQTT_SetOption_User,
    ATMQTT_SetOption_Password,
    ATMQTT_SetOption_Will,
    ATMQTT_SetOption_KeepAlive,
    ATMQTT_SetOption_Clean,
    ATMQTT_SetOption_NumberOfValues
} ATMQTT_SetOption_t;

/**
 * @brief MQTT server info
 */
typedef struct ATMQTT_ServerInfo_t {
    char address[CALYPSO_MAX_HOST_NAME_LENGTH];
    uint16_t port;
} ATMQTT_ServerInfo_t;

/**
 * @brief MQTT security configuration
 */
typedef struct ATMQTT_SecurityParams_t {
    ATMQTT_SecurityMethod_t securityMethod;
    uint32_t cipher; /**< Bitmask of ATSocket_Cipher_t */
    char privateKeyFile[ATFILE_FILENAME_MAX_LENGTH + 1];
    char certificateFile[ATFILE_FILENAME_MAX_LENGTH + 1];
    char CAFile[ATFILE_FILENAME_MAX_LENGTH + 1];
    char DHKey[ATFILE_FILENAME_MAX_LENGTH + 1];
} ATMQTT_SecurityParams_t;

/**
 * @brief MQTT connection parameters
 */
typedef struct ATMQTT_ConnectionParams_t {
    ATMQTT_ProtocolVersion_t protocolVersion;
    uint8_t blockingSend;
    Calypso_DataFormat_t format;
} ATMQTT_ConnectionParams_t;

/**
 * @brief MQTT subscription info
 */
typedef struct ATMQTT_SubscribeTopic_t {
    char topic[MQTT_MAX_TOPIC_LENGTH];
    ATMQTT_QoS_t QoS;
} ATMQTT_SubscribeTopic_t;

/**
 * @brief MQTT set will parameters
 */
typedef struct ATMQTT_SetWillParams_t {
    char topic[MQTT_MAX_TOPIC_LENGTH]; /**< Will topic string */
    ATMQTT_QoS_t QoS;                  /**< Quality of service */
    uint8_t
        retain; /**< 0: Do not retain will message, 1: retain will message */
    uint16_t messageLength; /**< Number of bytes in will message */
    char message[MQTT_MAX_MESSAGE_LENGTH]; /**< Will message*/
} ATMQTT_SetWillParams_t;

typedef union ATMQTT_SetValues_t {
    char username[512];          /**< User name */
    char password[256];          /**< Password */
    ATMQTT_SetWillParams_t will; /**< Will parameters */
    uint16_t keepAliveSeconds;   /**< Keep alive time in seconds */
    uint8_t clean; /**< 0 = persistent connection, 1 = clean connection */
} ATMQTT_SetValues_t;

extern bool ATMQTT_Create(char *clientID, uint32_t flags,
                          ATMQTT_ServerInfo_t serverInfo,
                          ATMQTT_SecurityParams_t securityParams,
                          ATMQTT_ConnectionParams_t connectionParams,
                          uint8_t *pIndex);
extern bool ATMQTT_Delete(uint8_t index);
extern bool ATMQTT_Connect(uint8_t index);
extern bool ATMQTT_Disconnect(uint8_t index);
extern bool ATMQTT_Publish(uint8_t index, char *topic, ATMQTT_QoS_t QoS,
                           uint8_t retain, uint16_t messageLength,
                           char *pMessage);
extern bool ATMQTT_Subscribe(uint8_t index, uint8_t numOfTopics,
                             ATMQTT_SubscribeTopic_t *pTopics);
extern bool ATMQTT_Unsubscribe(uint8_t index, char *topic1, char *topic2,
                               char *topic3, char *topic4);
extern bool ATMQTT_Set(uint8_t index, ATMQTT_SetOption_t option,
                       ATMQTT_SetValues_t *pValues);

#ifdef __cplusplus
}
#endif

#endif /* AT_MQTT_H_INCLUDED */
