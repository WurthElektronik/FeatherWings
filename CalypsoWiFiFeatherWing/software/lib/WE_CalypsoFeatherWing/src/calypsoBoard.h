/**
 * \file
 * \brief File for the calypso board of the WE IoT design kit.
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
#ifndef CALYPSOBOARD_H
#define CALYPSOBOARD_H

/**         Includes         */
#include "ConfigPlatform.h"
#include "time.h"
#include "Calypso.h"
#include <ATCommands/ATDevice.h>
#include <ATCommands/ATMQTT.h>
#include <ATCommands/ATWLAN.h>
#include <ATCommands/ATEvent.h>
#include <ATCommands/ATNetApp.h>
#include <ATCommands/ATNetCfg.h>
#include <ATCommands/ATHTTP.h>
#ifdef __cplusplus
extern "C" {
#endif
#define LENGTH_OF_NAME 100
#define EVENT_WAIT_TIME 3000UL
#define MAX_RETRIES 5

typedef struct ATMQTT_userOptions_t {
    char userName[256];
    char passWord[256];
} ATMQTT_userOptions_t;

typedef struct {
    char timezone[5];
    char server[128];
} CALYPSO_SNTPSettings_t;

typedef struct {
    char clientID[23];
    uint32_t flags;
    ATMQTT_ServerInfo_t serverInfo;
    ATMQTT_SecurityParams_t secParams;
    ATMQTT_ConnectionParams_t connParams;
    ATMQTT_userOptions_t userOptions;
} CALYPSO_MQTTSettings_t;

typedef struct {
    ATWLAN_ConnectionArguments_t wifiSettings;
    CALYPSO_MQTTSettings_t mqttSettings;
    CALYPSO_SNTPSettings_t sntpSettings;
} CalypsoSettings;

bool Calypso_simpleInit(CalypsoSettings *settings,
                        Calypso_EventCallback_t callback);

bool Calypso_WLANconnect();
bool Calypso_WLANDisconnect();

bool Calypso_WLANGetProfile(uint8_t profileID, ATWLAN_Profile_t *wlanProfile);
bool Calypso_WLANDeleteProfile(uint8_t profileID);

bool Calypso_MQTTconnect();
bool Calypso_MQTTDisconnect();
bool Calypso_MQTTPublishData(char *topic, uint8_t retain, char *data,
                             int length);
bool Calypso_subscribe(uint8_t index, uint8_t numOfTopics,
                       ATMQTT_SubscribeTopic_t *pTopics);
bool Calypso_reboot();
bool Calypso_StartProvisioning();
bool Calypso_StopProvisioning();

bool Calypso_setUpSNTP();
bool Calypso_getTimestamp(Timestamp *timeStamp);

bool Calypso_fileList();
bool Calypso_fileExists(const char *fileName);
bool Calypso_writeFile(const char *path, const char *data, uint16_t dataLength);
bool Calypso_readFile(const char *path, char *data, uint16_t dataLength,
                      uint16_t *outputLength);
bool Calypso_deleteFile(const char *fileName);
bool Calypso_isIPConnected(char *ipAddress);
bool Calypso_HTTPCustomResponse(char *data, int length, bool encode);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSOBOARD_H */
