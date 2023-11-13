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
#include "calypsoBoard.h"
#include "global.h"

CalypsoSettings *Calypso_settings;
#define CALYPSO_DEFAULT_BAUDRATE 921600

bool Calypso_MQTTCreate();
bool Calypso_MQTTConnToBroker();
bool Calypso_MQTTSet();
bool ATFile_open(const char *fileName, uint32_t options, uint16_t fileSize,
                 uint32_t *fileID, uint32_t *secureToken);
bool ATFile_close(uint32_t fileID, char *certFileName, char *signature);
bool ATFile_write(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                  bool encodeToBase64, uint16_t bytestoWrite, char *data,
                  uint16_t *writtenBytes);
bool ATFile_read(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                 uint16_t bytesToRead, uint16_t *byteRead, char *data);
bool ATFile_del(const char *fileName, uint32_t secureToken);
bool ATFile_getInfo(const char *fileName, uint32_t secureToken,
                    ATFile_FileInfo_t *fileInfo);

/**
 * @brief  Reboot calypso and check comms
 * @retval true if successful false in case of failure
 */
bool Calypso_simpleInit(CalypsoSettings *settings,
                        Calypso_EventCallback_t callback) {
    if (!settings) {
        return false;
    }

    Calypso_settings = settings;

    WE_DEBUG_PRINT("Starting Calypso...\r\n");

    if (!Calypso_Init(CALYPSO_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl,
                      WE_Parity_Even, callback)) {
        return false;
    }

    Calypso_SetTimeout(Calypso_Timeout_General, 3000);

    return true;
}
/**
 * @brief  Reboot calypso
 * @retval true if successful false in case of failure
 */
bool Calypso_reboot() { return ATDevice_Reboot(); }
/**
 * @brief  Check if Calypso has an IP address
 * @retval true if successful false in case of failure
 */
bool Calypso_isIPConnected(char *ipAddress) {
    /* Get IP address */
    ATNetCfg_IPv4Config_t ipConfig;
    if (!ATNetCfg_GetIPv4AddressStation(&ipConfig)) {
        return false;
    }

    if (0 == strncmp(ipConfig.ipAddress, "0.0.0.0", 7)) {
        return false;
    }

    if (ipAddress) {
        strcpy(ipAddress, ipConfig.ipAddress);
    }

    return true;
}

/**
 * @brief  Connect to the access point with parameters in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_WLANconnect() {
    return ATWLAN_Connect(Calypso_settings->wifiSettings);
}
/**
 * @brief  Disconnect from access point
 * @retval true if successful false in case of failure
 */
bool Calypso_WLANDisconnect() { return ATWLAN_Disconnect(); }
bool Calypso_WLANDeleteProfile(uint8_t profileID) {
    return ATWLAN_DeleteProfile(profileID);
}
bool Calypso_WLANGetProfile(uint8_t profileID, ATWLAN_Profile_t *wlanProfile) {
    return ATWLAN_GetProfile(profileID, wlanProfile);
}
/**
 * @brief  Start provisioning
 * @retval true if successful false in case of failure
 */
bool Calypso_StartProvisioning() { return ATDevice_StartProvisioning(); }
/**
 * @brief  Stop provisioning
 * @retval true if successful false in case of failure
 */
bool Calypso_StopProvisioning() {
    if (!Calypso_SendRequest("AT+provisioningStop\r\n")) {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General),
                                  Calypso_CNFStatus_Success, NULL);
}
/**
 * @brief  Set up SNTP client with parameters in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_setUpSNTP() {
    /* Enable*/
    if (!ATNetApp_StartApplications(ATNetApp_Application_SntpClient)) {
        return false;
    }

    ATNetApp_OptionValue_t value;
    memset(&value, 0, sizeof(value));
    value.sntp.timeZone = atoi(Calypso_settings->sntpSettings.timezone);
    if (!ATNetApp_Set(ATNetApp_Application_SntpClient,
                      ATNetApp_SntpOption_TimeZone, &value)) {
        WE_DEBUG_PRINT("SNTP time_zone set failed\r\n");
        return false;
    }

    /* Set Server*/
    memset(&value, 0, sizeof(value));
    strcpy(value.sntp.servers[0], Calypso_settings->sntpSettings.server);
    if (!ATNetApp_Set(ATNetApp_Application_SntpClient,
                      ATNetApp_SntpOption_Servers, &value)) {
        WE_DEBUG_PRINT("SNTP set server failed\r\n");
        return false;
    }

    /* Synchronize calypso time with sntp server */
    if (!ATNetApp_UpdateTime()) {
        WE_DEBUG_PRINT("SNTP enable failed\r\n");
        return false;
    }

    return true;
}
/**
 * @brief  Get the current time
 * @param timeStamp Pointer to timestamp returned by calypso
 * @retval true if successful false in case of failure
 */
bool Calypso_getTimestamp(Timestamp *timeStamp) {
    if (!timeStamp) {
        return false;
    }

    ATDevice_Value_t value;
    if (!ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time,
                      &value)) {
        WE_DEBUG_PRINT("SNTP get time failed \r\n");
        return false;
    }

    timeStamp->second = value.general.time.second;
    timeStamp->minute = value.general.time.minute;
    timeStamp->hour = value.general.time.hour;
    timeStamp->day = value.general.time.day;
    timeStamp->month = value.general.time.month;
    timeStamp->year = value.general.time.year;

    return true;
}

bool Calypso_HTTPCustomResponse(char *data, int length, bool encode) {
    return ATHTTP_SendCustomResponse(
        encode ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary, encode,
        length, data);
}
/**
 * @brief Create and Connect to the MQTT broker with parameters in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTconnect() {
    if (Calypso_MQTTCreate()) {
        if (Calypso_MQTTSet()) {
            return (Calypso_MQTTConnToBroker());
        }
    }
    return false;
}
/**
 *Sends the AT+MQTTsubscribe=[index],[numOfTopics],[topic1],[QoSn],[reserved]...
 *command
 *
 *input:
 * -index           index of mqttclient to to subscribe.
 * -numOfTopics     number of topics to which subscribe to
 * -pTopics         Topics to subscribe to. See ATMQTT_subscribeTopic_t
 *
 *return true  if succeeded
 *       false otherwise
 */
bool Calypso_subscribe(uint8_t index, uint8_t numOfTopics,
                       ATMQTT_SubscribeTopic_t *pTopics) {
    return ATMQTT_Subscribe(index, numOfTopics, pTopics);
}

/**
 * @brief  Publish data to the MQTT broker
 * @param  topic Pointer to MQTT topic
 * @param  retain 0=do not retain, 1=retain message
 * @param  data Pointer to the data to be published
 * @param  length data length
 * @param  encode 0=do not encode, 1=base64 encode
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTPublishData(char *topic, uint8_t retain, char *data,
                             int length) {
    switch (Calypso_settings->mqttSettings.connParams.format) {
        case Calypso_DataFormat_Binary: {
            return ATMQTT_Publish(0, topic, ATMQTT_QoS_QoS1, retain, length,
                                  data);
        }
        case Calypso_DataFormat_Base64: {
            uint32_t elen = 0;
            char out[CALYPSO_LINE_MAX_SIZE];
            if (!Calypso_EncodeBase64((uint8_t *)data, length, (uint8_t *)out,
                                      &elen)) {
                return false;
            }
            return ATMQTT_Publish(0, topic, ATMQTT_QoS_QoS1, retain, elen - 1,
                                  out);
        }
        default:
            return false;
    }

    return false;
}
/**
 * @brief  Set MQTT username with parameter in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTSet() {
    ATMQTT_SetValues_t value;
    memset(&value, 0, sizeof(value));

    if (strlen(Calypso_settings->mqttSettings.userOptions.userName) == 0) {
        return true;
    }

    strcpy(value.username, Calypso_settings->mqttSettings.userOptions.userName);
    if (!ATMQTT_Set(0, ATMQTT_SetOption_User, &value)) {
        return false;
    }

    if (strlen(Calypso_settings->mqttSettings.userOptions.passWord) == 0) {
        return true;
    }

    memset(&value, 0, sizeof(value));
    strcpy(value.password, Calypso_settings->mqttSettings.userOptions.passWord);
    if (!ATMQTT_Set(0, ATMQTT_SetOption_Password, &value)) {
        return false;
    }

    return true;
}
/**
 * @brief  Connect to the MQTT broker with parameters in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTConnToBroker() { return ATMQTT_Connect(0); }
/**
 * @brief  Create a MQTT socket with parameters in the settings
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTCreate() {
    uint8_t index;
    return ATMQTT_Create(Calypso_settings->mqttSettings.clientID,
                         Calypso_settings->mqttSettings.flags,
                         Calypso_settings->mqttSettings.serverInfo,
                         Calypso_settings->mqttSettings.secParams,
                         Calypso_settings->mqttSettings.connParams, &index);
}
/**
 * @brief  Disconnect from MQTT connection
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTDisconnect() {
    if (!ATMQTT_Disconnect(0)) {
        return false;
    }

    if (!ATMQTT_Delete(0)) {
        return false;
    }

    return true;
}
/**
 * @brief  Get the list of files in the file system
 * @retval true if successful false in case of failure
 */
bool Calypso_fileList() { return ATFile_GetFileList(); }
/**
 * @brief  Get the list of files in the file system
 * @retval true if successful false in case of failure
 */
bool Calypso_fileExists(const char *fileName) {
    ATFile_FileInfo_t fileInfo;
    return ATFile_getInfo(fileName, 0, &fileInfo);
}
/**
 * @brief  Delete the file
 * @retval true if successful false in case of failure
 */
bool Calypso_deleteFile(const char *fileName) {
    return ATFile_Delete(fileName, 0);
}
/**
 * @brief  Open and read data frpm a file
 * @param  path Pointer to the file path including the filename
 * @param  data Pointer to data buffer
 * @param  dataLength Length of data to read
 * @param  outputLength Length of read bytes
 * @retval true if successful false in case of failure
 */
bool Calypso_readFile(const char *path, char *data, uint16_t dataLength,
                      uint16_t *outputLength) {
    uint32_t fileID;
    uint32_t sToken;
    uint16_t bytesRead;

    if (!ATFile_open(path, ATFile_OpenFlags_Read, 0, &fileID, &sToken)) {
        return false;
    }

    if (!ATFile_read(fileID, 0, Calypso_DataFormat_Binary, dataLength,
                     &bytesRead, data)) {
        return false;
    }

    *outputLength = bytesRead;

    ATFile_close(fileID, NULL, NULL);
    return true;
}
/**
 * @brief Create/open and write data to a file
 * @param  path Pointer to the file path including the filename
 * @param  data Pointer to data to be written
 * @param  dataLength Length of data to write
 * @retval true if successful false in case of failure
 */
bool Calypso_writeFile(const char *path, const char *data,
                       uint16_t dataLength) {
    uint32_t fileID;
    uint32_t sToken;

    if (!ATFile_open(path, ATFile_OpenFlags_Create | ATFile_OpenFlags_Overwrite,
                     ATFILE_FILE_MIN_SIZE > dataLength ? ATFILE_FILE_MIN_SIZE
                                                       : dataLength,
                     &fileID, &sToken)) {
        return false;
    }

    uint16_t bytesWritten;

    if (!ATFile_write(fileID, 0, Calypso_DataFormat_Binary, false, strlen(data),
                      (char *)data, &bytesWritten)) {
        return false;
    }

    return ATFile_close(fileID, NULL, NULL);
}
/**
 * @brief  Open a file
 * @param  fileName Pointer to full file path
 * @param  options  File open options
 * @param  fileSize Max file size
 * @param  fileID Pointer to file identifier
 * @param  secureToken Pointer to secure token
 * @retval true if successful false in case of failure
 */
bool ATFile_open(const char *fileName, uint32_t options, uint16_t fileSize,
                 uint32_t *fileID, uint32_t *secureToken) {
    if (fileSize < ATFILE_FILE_MIN_SIZE) {
        fileSize = ATFILE_FILE_MIN_SIZE;
    }

    return ATFile_Open(fileName, options, fileSize, fileID, secureToken);
}
/**
 * @brief  Close a file
 * @param  fileID File to close
 * @param  certFileName Pointer to certificate file name
 * @param  signature Pointer to signature
 * @retval true if successful false in case of failure
 */
bool ATFile_close(uint32_t fileID, char *certFileName, char *signature) {
    return ATFile_Close(fileID, certFileName, signature);
}
/**
 * @brief  Write to a file
 * @param  fileID FileID to write
 * @param  offset Write offset
 * @param  format Dataformat
 * @param  encodeToBase64 boolean
 * @param  bytestoWrite Number of bytes to write
 * @param  data Pointer to data to be written
 * @param  writtenBytes Number of bytes successfully written
 * @retval true if successful false in case of failure
 */
bool ATFile_write(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                  bool encodeToBase64, uint16_t bytestoWrite, char *data,
                  uint16_t *writtenBytes) {
    return ATFile_Write(fileID, offset, format, encodeToBase64, bytestoWrite,
                        data, writtenBytes);
}

/**
 * @brief  Read to a file
 * @param  fileID FileID to write
 * @param  offset read offset
 * @param  format Dataformat
 * @param  bytestoRead Number of bytes to read
 * @param  pOutFormat Pointer to format of output data
 * @param  byteRead Number of bytes successfully read
 * @param  data Pointer to data to be written
 * @retval true if successful false in case of failure
 */
bool ATFile_read(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                 uint16_t bytesToRead, uint16_t *byteRead, char *data) {
    return ATFile_Read(fileID, offset, format,
                       Calypso_DataFormat_Binary != format, bytesToRead, data,
                       byteRead);
}
/**
 * @brief  Delete a file
 * @param  fileName Pointer to filename
 * @param  secureToken Secure token
 * @retval true if successful false in case of failure
 */
bool ATFile_del(const char *fileName, uint32_t secureToken) {
    return ATFile_Delete(fileName, secureToken);
}
/**
 * @brief  Get file info
 * @param  fileName Pointer to filename
 * @param  secureToken Secure token
 * @retval true if successful false in case of failure
 */
bool ATFile_getInfo(const char *fileName, uint32_t secureToken,
                    ATFile_FileInfo_t *fileInfo) {
    return ATFile_GetInfo(fileName, secureToken, fileInfo);
}

/**
 * @brief Switch the WLAN mode of the module
 * @param  mode mode of WLAN
 * @retval true if successful false in case of failure
 */
bool Calypso_setWLANMode(ATWLAN_SetMode_t mode) {
    if (!ATWLAN_SetMode(mode)) {
        return false;
    }

    return ATDevice_Restart(0);
}

/**
 * @brief Switch the WLAN mode of the module
 * @param  modeP pointer to mode of WLAN
 * @retval true if successful false in case of failure
 */
bool Calypso_getWLANMode(ATWLAN_SetMode_t *modeP) {
    ATWLAN_Settings_t settings;

    settings.connection.role = ATWLAN_SetMode_NumberOfValues;

    ATWLAN_Get(ATWLAN_SetID_Connection, 0, &settings);

    if (settings.connection.role == ATWLAN_SetMode_NumberOfValues) {
        return false;
    }

    *modeP = settings.connection.role;

    return true;
}
