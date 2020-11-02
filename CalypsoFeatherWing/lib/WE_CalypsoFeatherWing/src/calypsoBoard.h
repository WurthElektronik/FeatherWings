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
#include "calypso.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define LENGTH_OF_NAME 100
#define RESPONSE_WAIT_TIME 1500
#define EVENT_WAIT_TIME 3000
#define MAX_RETRIES 5

  typedef enum
  {
    calypso_unknown,
    calypso_started,
    calypso_WLAN_connected,
    calypso_MQTT_connected,
    calypso_error
  } Calypso_status_t;

  typedef struct
  {
    char timezone[5];
    char server[128];
  } CALYPSO_SNTPSettings_t;

  typedef struct
  {
    char clientID[23];
    uint32_t flags;
    ATMQTT_ServerInfo_t serverInfo;
    ATMQTT_securityParams_t secParams;
    ATMQTT_connectionParams_t connParams;
    ATMQTT_userOptions_t userOptions;
  } CALYPSO_MQTTSettings_t;

  typedef struct
  {
    ATWLAN_ConnectionArguments_t wifiSettings;
    CALYPSO_MQTTSettings_t mqttSettings;
    CALYPSO_SNTPSettings_t sntpSettings;
  } CalypsoSettings;

  typedef struct
  {
    char data[CALYPSO_LINE_MAX_SIZE];
    int length;
  } PacketCalypso;
  /**
 * @brief CALYPSO Object
 *
 */
  typedef struct
  {
    TypeSerial *serialDebug;
    TypeHardwareSerial *serialCalypso;
    CalypsoSettings settings;
    PacketCalypso bufferCalypso;
    Calypso_status_t status;
    char MAC_ADDR[20];
  } CALYPSO;

  CALYPSO *Calypso_Create(TypeSerial *serialDebug,
                          TypeHardwareSerial *serialCalypso,
                          CalypsoSettings *settings);

  void Calypso_Destroy(CALYPSO *calypso);
  bool Calypso_simpleInit(CALYPSO *self);

  bool Calypso_reboot(CALYPSO *self);
  bool Calypso_WLANconnect(CALYPSO *self);
  bool Calypso_WLANDisconnect(CALYPSO *self);

  bool Calypso_MQTTconnect(CALYPSO *self);
  bool Calypso_MQTTPublishData(CALYPSO *self, char *topic, uint8_t retain, char *data, int length, bool encode);

  bool Calypso_StartProvisioning(CALYPSO *self);

  bool Calypso_setUpSNTP(CALYPSO *self);
  bool Calypso_getTimestamp(CALYPSO *self, Timestamp *timeStamp);

  bool Calypso_fileList(CALYPSO *self);
  bool Calypso_writeFile(CALYPSO *self, const char *path, const char *data, uint16_t dataLength);
#ifdef __cplusplus
}
#endif

#endif /* CALYPSOBOARD_H */