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
#include "events.h"

static bool requestPending;
static bool eventPending;
static size_t lengthResponse;

void Calypso_Sendbytes(CALYPSO *self, const char *sendCmd);
bool Calypso_SendRequest(CALYPSO *self, const char *sendCmd);

void Calypso_HandleEvents(CALYPSO *self);
static void Calypso_HandleRxLine(CALYPSO *self, char *rxPacket, uint16_t rxLength);
bool Calypso_waitForReply(CALYPSO *self, Calypso_CNFStatus_t expectedStatus, bool reset_confirmstate);
bool Calypso_appendArgumentString(char *pOutString, const char *pInArgument, char delimeter);
void Calypso_RxBytes(CALYPSO *self);
bool Calypso_waitForEvent(CALYPSO *self);
bool Calypso_MQTTCreate(CALYPSO *self);
bool Calypso_MQTTConnToBroker(CALYPSO *self);
bool Calypso_MQTTSet(CALYPSO *self);

bool ATFile_open(CALYPSO *self, const char *fileName, uint32_t options, uint16_t fileSize, uint32_t *fileID, uint32_t *secureToken);
bool ATFile_close(CALYPSO *self, uint32_t fileID, char *certFileName, char *signature);
bool ATFile_write(CALYPSO *self, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                  bool encodeToBase64, uint16_t bytestoWrite, char *data, uint16_t *writtenBytes);
bool ATFile_read(CALYPSO *self, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format,
                 uint16_t bytesToRead, Calypso_DataFormat_t *pOutFormat, uint16_t *byteRead, char *data);

static Calypso_CNFStatus_t cmdConfirmation;
static char RxBuffer[CALYPSO_LINE_MAX_SIZE]; /* data buffer for RX */
char requestBuffer[CALYPSO_LINE_MAX_SIZE];
char *pRequestCommand;

char eventbuffer[CALYPSO_LINE_MAX_SIZE];
char eventArguments[CALYPSO_LINE_MAX_SIZE];
char *pEventBuffer;

/**
 * @brief  Allocate memory and initialize the calypso object
 * @param  serialDebug Pointer to the serial debug 
 * @param  serialCalypso Pointer to the calypso serial object
 * @param  settings calypso settings
 * @retval calypso object
 */
CALYPSO *Calypso_Create(TypeSerial *serialDebug,
                        TypeHardwareSerial *serialCalypso,
                        CalypsoSettings *settings)
{
    CALYPSO *allocateInit = (CALYPSO *)malloc(sizeof(CALYPSO));
    allocateInit->serialDebug = serialDebug;
    allocateInit->serialCalypso = serialCalypso;
    allocateInit->bufferCalypso.length = 0;
    allocateInit->status = calypso_unknown;

    memset(allocateInit->bufferCalypso.data, '\0', sizeof(allocateInit->bufferCalypso.data));

    allocateInit->settings.wifiSettings = settings->wifiSettings;
    allocateInit->settings.mqttSettings = settings->mqttSettings;
    allocateInit->settings.sntpSettings = settings->sntpSettings;
    return allocateInit;
}

/**
 * @brief  Free up the memory assigned to the calypso object
 * @param  calypso Pointer to the object.
 * @retval none
 */
void Calypso_Destroy(CALYPSO *calypso)
{
    if (calypso)
    {
        free(calypso);
    }
}

/**
 * @brief  Reboot calypso and check comms 
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_simpleInit(CALYPSO *self)
{
#if SERIAL_DEBUG
    SSerial_printf(self->serialDebug, "Starting Calypso...\n\r");
    SSerial_printf(self->serialDebug, "Sending reboot\n\r");
#endif
    if (Calypso_reboot(self))
    {
        delay(RESPONSE_WAIT_TIME);
        return true;
    }
    return false;
}

/**
 * @brief  Reboot calypso
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_reboot(CALYPSO *self)
{
    if (Calypso_SendRequest(self, "AT+reboot\r\n"))
    {
        if (Calypso_waitForEvent(self))
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "%s\r\n", self->bufferCalypso);
#endif
            return true;
        }
    }
    return false;
}

/**
 * @brief  Connect to the access point with parameters in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_WLANconnect(CALYPSO *self)
{
    bool ret = false;
    pRequestCommand = &requestBuffer[0];

    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+wlanConnect=");

    ret = ATWLAN_addConnectionArguments(pRequestCommand, self->settings.wifiSettings, STRING_TERMINATE);

    if (ret)
    {
        ret = Calypso_appendArgumentString(pRequestCommand, CRLF, STRING_TERMINATE);
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "WiFi connection fail, check parameters\r\n");
#endif
        return false;
    }

    if (Calypso_SendRequest(self, pRequestCommand))
    {
        if (Calypso_waitForEvent(self))
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "%s\r\n", self->bufferCalypso);
#endif
            return true;
        }
    }
    return false;
}

/**
 * @brief  Disconnect from access point
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_WLANDisconnect(CALYPSO *self)
{
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+wlanDisconnect\r\n");
    return (Calypso_SendRequest(self, pRequestCommand));
}

/**
 * @brief  Start provisioning 
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_StartProvisioning(CALYPSO *self)
{
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+provisioningStart\r\n");
    return (Calypso_SendRequest(self, pRequestCommand));
}

/**
 * @brief  Set up SNTP client with parameters in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_setUpSNTP(CALYPSO *self)
{
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);

    strcat(pRequestCommand, "AT+netAppSet=sntp_client,");

    /* Set time zone */
    strcat(pRequestCommand, "time_zone,");
    strcat(pRequestCommand, self->settings.sntpSettings.timezone);

    if (!Calypso_SendRequest(self, pRequestCommand))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "SNTP time_zone set faile\r\n");
#endif
        return false;
    }

    /* Set Server*/
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+netAppSet=sntp_client,");
    strcat(pRequestCommand, "server_address,0,");
    strcat(pRequestCommand, self->settings.sntpSettings.server);
    if (!Calypso_SendRequest(self, pRequestCommand))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "SNTP set server failed\r\n");
#endif
        return false;
    }

    /* Enable*/
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+netAppSet=sntp_client,");
    strcat(pRequestCommand, "enable, 1");
    Calypso_SendRequest(self, pRequestCommand);

    /* Synchronize calypso time with sntp server */
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+netAppUpdateTime");
    if (!Calypso_SendRequest(self, pRequestCommand))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "SNTP enable failed\r\n");
#endif
        return false;
    }
    return true;
}

/**
 * @brief  Get the current time
 * @param  self Pointer to the calypso object.
 * @param timeStamp Pointer to timestamp returned by calypso
 * @retval true if successful false in case of failure
 */
bool Calypso_getTimestamp(CALYPSO *self, Timestamp *timeStamp)
{
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);

    /* Get Time */
    strcpy(pRequestCommand, "AT+GET=general,time");
    if (Calypso_SendRequest(self, pRequestCommand))
    {

        /* parse timestamp out of response */
        if (0 < self->bufferCalypso.length)
        {
            if (0 == strncmp(self->bufferCalypso.data, "+get:", 5))
            {
                char currentParameter[5];
                char *parameters = &(self->bufferCalypso.data[0]);

                Calypso_getNextArgumentString(&parameters, currentParameter, CONFIRM_DELIM);

                Calypso_getNextArgumentString(&parameters, currentParameter, ARGUMENT_DELIM);
                timeStamp->hour = atoi(currentParameter);

                Calypso_getNextArgumentString(&parameters, currentParameter, ARGUMENT_DELIM);
                timeStamp->minute = atoi(currentParameter);

                Calypso_getNextArgumentString(&parameters, currentParameter, ARGUMENT_DELIM);
                timeStamp->second = atoi(currentParameter);

                Calypso_getNextArgumentString(&parameters, currentParameter, ARGUMENT_DELIM);
                timeStamp->day = atoi(currentParameter);

                Calypso_getNextArgumentString(&parameters, currentParameter, ARGUMENT_DELIM);
                timeStamp->month = atoi(currentParameter);

                Calypso_getNextArgumentString(&parameters, currentParameter, STRING_TERMINATE);
                timeStamp->year = atoi(currentParameter);
                return true;
            }
        }
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "SNTP get time failed \r\n");
#endif
    }
    return false;
}

/**
 * @brief Create and Connect to the MQTT broker with parameters in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTconnect(CALYPSO *self)
{
    if (self->status == calypso_WLAN_connected)
    {

        if (Calypso_MQTTCreate(self))
        {
            if (Calypso_MQTTSet(self))
            {
                return (Calypso_MQTTConnToBroker(self));
            }
        }
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "MQTT connect fail: Not connected to Wi-Fi\r\n");
#endif
    }
    return false;
}
/**
 * @brief  Publish data to the MQTT broker
 * @param  self Pointer to the calypso object.
 * @param  topic Pointer to MQTT topic
 * @param  retain 0=do not retain, 1=retain message
 * @param  data Pointer to the data to be published
 * @param  length data length
 * @param  encode 0=do not encode, 1=base64 encode
 * @retval true if successful false in case of failure 
 */
bool Calypso_MQTTPublishData(CALYPSO *self, char *topic, uint8_t retain, char *data, int length, bool encode)
{
    bool ret = false;
    int index = MQTT_SOCKET_INDEX;
    pRequestCommand = &requestBuffer[0];
    if (self->status == calypso_MQTT_connected)
    {

        memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
        strcpy(pRequestCommand, "AT+mqttPublish=");

        if (encode)
        {
            uint32_t elen = 0;
            char out[CALYPSO_LINE_MAX_SIZE];
            Calypso_encodeBase64((uint8_t *)data, length, (uint8_t *)out, &elen);
            ret = ATMQTT_addArgumentsPublish(pRequestCommand, index, topic, ATMQTT_QOS_QOS1, retain, elen, out);
        }
        else
        {
            ret = ATMQTT_addArgumentsPublish(pRequestCommand, index, topic, ATMQTT_QOS_QOS1, retain, length, data);
        }
        if (ret)
        {
            if (Calypso_SendRequest(self, pRequestCommand))
            {
                return (Calypso_waitForEvent(self));
            }
        }
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Publish failed : Not connected to MQTT broker\r\n");
#endif
    }
    return false;
}

/**
 * @brief  Set MQTT username with parameter in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTSet(CALYPSO *self)
{

    bool ret = false;
    int index = MQTT_SOCKET_INDEX;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+mqttSet=");

    ret = ATMQTT_addArgumentsSet(pRequestCommand, index, ATMQTT_SET_OPTION_user, self->settings.mqttSettings.userOptions.userName);

    if (ret)
    {
        if (Calypso_SendRequest(self, pRequestCommand))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief  Connect to the MQTT broker with parameters in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTConnToBroker(CALYPSO *self)
{
    bool ret = false;
    int index = MQTT_SOCKET_INDEX;
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+mqttConnect=");

    ret = Calypso_appendArgumentInt(pRequestCommand, index, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), STRING_TERMINATE);
    if (ret)
    {
        ret = Calypso_appendArgumentString(pRequestCommand, CRLF, STRING_TERMINATE);
    }

    if (ret)
    {
        if (Calypso_SendRequest(self, pRequestCommand))
        {
            Calypso_HandleEvents(self);
            if (self->status == calypso_MQTT_connected)
            {
                return true;
            }
        }
    }
    return false;
}
/**
 * @brief  Create a MQTT socket with parameters in the settings
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_MQTTCreate(CALYPSO *self)
{
    bool ret = false;
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+mqttCreate=");
    ret = ATMQTT_addArgumentsCreate(pRequestCommand, self->settings.mqttSettings.clientID,
                                    self->settings.mqttSettings.flags, self->settings.mqttSettings.serverInfo,
                                    self->settings.mqttSettings.secParams, self->settings.mqttSettings.connParams);
    if (ret)
    {
        ret = Calypso_appendArgumentString(pRequestCommand, CRLF, STRING_TERMINATE);
    }

    return (Calypso_SendRequest(self, pRequestCommand));
}

/**
 * @brief  Get the list of files in the file system
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_fileList(CALYPSO *self)
{
    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileGetFileList");
    return (Calypso_SendRequest(self, pRequestCommand));
}

/**
 * @brief Create/open and write data to a file
 * @param  self Pointer to the calypso object.
 * @param  path Pointer to the file path including the filename
 * @param  data Pointer to data to be written
 * @param  dataLength Length of data to write
 * @retval true if successful false in case of failure
 */
bool Calypso_writeFile(CALYPSO *self, const char *path, const char *data, uint16_t dataLength)
{
    bool ret = false;
    uint32_t fileID;
    uint32_t sToken;

    int bytesRemaining = strlen(data);
    int startIdx = 0;
    int writeSize = 0;
    uint16_t bytesWritten = 0;
    char dataToWrite[CALYPSO_FILE_WRITE_SIZE_MAX] = {0};

    ret = ATFile_open(self, path, ATFILE_OPEN_CREATE | ATFILE_OPEN_OVERWRITE, dataLength, &fileID, &sToken);
    if (ret)
    {
        while (bytesRemaining > 0)
        {
            if (bytesRemaining > CALYPSO_FILE_WRITE_SIZE_MAX)
            {
                writeSize = CALYPSO_FILE_WRITE_SIZE_MAX - 1;
            }
            else
            {
                writeSize = bytesRemaining;
            }
            memcpy(dataToWrite, data + startIdx, writeSize);
            dataToWrite[writeSize] = '\0';

            ret = ATFile_write(self, fileID, startIdx, Calypso_DataFormat_Binary, false, writeSize, dataToWrite, &bytesWritten);
            if (!ret)
            {
                break;
            }
            startIdx = startIdx + bytesWritten;
            bytesRemaining = bytesRemaining - bytesWritten;
        }
    }
    if (ret)
    {
        return (ATFile_close(self, fileID, NULL, NULL));
    }
    else
    {
        ret = ATFile_close(self, fileID, NULL, NULL);
        return false;
    }
}

/**
 * @brief  Open a file 
 * @param  self Pointer to the calypso object
 * @param  fileName Pointer to full file path
 * @param  options  File open options
 * @param  fileSize Max file size
 * @param  fileID Pointer to file identifier
 * @param  secureToken Pointer to secure token
 * @retval true if successful false in case of failure
 */
bool ATFile_open(CALYPSO *self, const char *fileName, uint32_t options, uint16_t fileSize, uint32_t *fileID, uint32_t *secureToken)
{
    bool ret = false;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileOpen=");

    if (fileSize < FILE_MIN_SIZE)
    {
        fileSize = FILE_MIN_SIZE;
    }

    ret = ATFile_AddArgumentsFileOpen(pRequestCommand, fileName, options, fileSize);

    if (ret)
    {
        ret = Calypso_SendRequest(self, pRequestCommand);
    }

    if (ret)
    {
        char *temp = self->bufferCalypso.data;
        ret = ATFile_ParseResponseFileOpen(&temp, fileID, secureToken);
    }
    return ret;
}

/**
 * @brief  Close a file
 * @param  self Pointer to the calypso object.
 * @param  fileID File to close
 * @param  certFileName Pointer to certificate file name
 * @param  signature Pointer to signature
 * @retval true if successful false in case of failure
 */
bool ATFile_close(CALYPSO *self, uint32_t fileID, char *certFileName, char *signature)
{
    bool ret = false;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileClose=");

    ret = ATFile_AddArgumentsFileClose(pRequestCommand, fileID, certFileName, signature);

    if (ret)
    {
        ret = Calypso_SendRequest(self, pRequestCommand);
    }

    return ret;
}

/**
 * @brief  Write to a file
 * @param  self Pointer to the calypso object.
 * @param  fileID FileID to write 
 * @param  offset Write offset
 * @param  format Dataformat
 * @param  encodeToBase64 boolean 
 * @param  bytestoWrite Number of bytes to write
 * @param  data Pointer to data to be written
 * @param  writtenBytes Number of bytes successfully written
 * @retval true if successful false in case of failure
 */
bool ATFile_write(CALYPSO *self, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeToBase64, uint16_t bytestoWrite, char *data, uint16_t *writtenBytes)
{
    bool ret = false;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileWrite=");

    ret = ATFile_AddArgumentsFileWrite(pRequestCommand, fileID, offset, format, encodeToBase64, bytestoWrite, data);

    if (ret)
    {
        Calypso_SendRequest(self, pRequestCommand);
    }

    if (ret)
    {
        char *temp = self->bufferCalypso.data;
        ret = ATFile_ParseResponseFileWrite(&temp, writtenBytes);
    }

    return ret;
}

/**
 * @brief  Read to a file
 * @param  self Pointer to the calypso object.
 * @param  fileID FileID to write 
 * @param  offset read offset
 * @param  format Dataformat
 * @param  bytestoRead Number of bytes to read
 * @param  pOutFormat Pointer to format of output data
 * @param  byteRead Number of bytes successfully read
 * @param  data Pointer to data to be written
 * @retval true if successful false in case of failure
 */
bool ATFile_read(CALYPSO *self, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToRead, Calypso_DataFormat_t *pOutFormat, uint16_t *byteRead, char *data)
{
    bool ret = false;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileRead=");

    ret = ATFile_AddArgumentsFileRead(pRequestCommand, fileID, offset, format, bytesToRead);

    if (ret)
    {
        ret = Calypso_SendRequest(self, pRequestCommand);
    }

    if (ret)
    {
        char *temp = self->bufferCalypso.data;
        ret = ATFile_ParseResponseFileRead(&temp, pOutFormat, byteRead, data);
    }

    return ret;
}

/**
 * @brief  Delete a file
 * @param  self Pointer to the calypso object.
 * @param  fileName Pointer to filename
 * @param  secureToken Secure token
 * @retval true if successful false in case of failure
 */
bool ATFile_del(CALYPSO *self, const char *fileName, uint32_t secureToken)
{
    bool ret = false;

    pRequestCommand = &requestBuffer[0];
    memset(pRequestCommand, 0, CALYPSO_LINE_MAX_SIZE);
    strcpy(pRequestCommand, "AT+fileDel=");

    ret = ATFile_AddArgumentsFileDel(pRequestCommand, fileName, secureToken);

    if (ret)
    {
        ret = Calypso_SendRequest(self, pRequestCommand);
    }

    return ret;
}

/**
 * @brief  Send a request to calypso and wait for the response
 * @param  self Pointer to the calypso object.
 * @param  sendCmd Pointer to command
 * @retval true if successful false in case of failure
 */
bool Calypso_SendRequest(CALYPSO *self, const char *sendCmd)
{
    bool ret = false;
    int retries = 0;
    while (!ret)
    {
        Calypso_Sendbytes(self, sendCmd);
        ret = Calypso_waitForReply(self, Calypso_CNFStatus_Success, true);
        retries++;
        if (retries == MAX_RETRIES)
        {
            break;
        }
    }
    return ret;
}

/**
 * @brief  Send bytes on to the calypso serial port
 * @param  self Pointer to the calypso object.
 * @param  sendCmd Pointer to the data to send
 * @retval true if successful false in case of failure
 */
void Calypso_Sendbytes(CALYPSO *self, const char *sendCmd)
{
    requestPending = true;
    lengthResponse = 0;
#if SERIAL_DEBUG
    SSerial_printf(self->serialDebug, "Sending to Calypso: ");
    SSerial_writeB(self->serialDebug, sendCmd, strlen(sendCmd));
    SSerial_printf(self->serialDebug, "\r\n");
#endif
    int written = 1;
    while (written)
    {
        while (HSerial_available(self->serialCalypso))
        {
            HSerial_read(self->serialCalypso);
        }
        if (strlen(sendCmd) <= HSerial_availableForWrite(self->serialCalypso))
        {
            HSerial_writeB(self->serialCalypso, sendCmd, strlen(sendCmd));
            HSerial_writeB(self->serialCalypso, "\r\n", 2);
            written = 0;
        }
    }
    HSerial_flush(self->serialCalypso);
}
/**
 * @brief  Wait for an event from calypso
 * @param  self Pointer to the calypso object.
 * @retval true if successful false in case of failure
 */
bool Calypso_waitForEvent(CALYPSO *self)
{
    int count = 0;
    int time_step_ms = 5; /* 5ms */
    int max_count = EVENT_WAIT_TIME / time_step_ms;
    eventPending = true;
    while (eventPending)
    {
        Calypso_RxBytes(self);
        if (count >= max_count)
        {
            break;
        }
        /* wait */
        count++;
        delay(time_step_ms);
    }
    return (!eventPending);
}

/**
 * @brief  Wait for calypso response
 * @param  self Pointer to the calypso object.
 * @param  expectedStatus Expected response
 * @param  reset_confirmstate reset confirm state
 * @retval true if successful false in case of failure
 */
bool Calypso_waitForReply(CALYPSO *self, Calypso_CNFStatus_t expectedStatus, bool reset_confirmstate)
{
    int count = 0;
    int time_step_ms = 5; /* 5ms */
    int max_count = RESPONSE_WAIT_TIME / time_step_ms;

    if (reset_confirmstate)
    {
        cmdConfirmation = Calypso_CNFStatus_Invalid;
    }

    while (1)
    {
        Calypso_RxBytes(self);
        if (Calypso_CNFStatus_Invalid != cmdConfirmation)
        {
            requestPending = false;
            if (cmdConfirmation == expectedStatus)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        if (count >= max_count)
        {
            /* received no correct response within timeout */
            requestPending = false;
            return false;
        }

        /* wait */
        count++;
        delay(time_step_ms);
    }
    return false;
}

/**
 * @brief  Handle events from calypso
 * @param  self Pointer to the calypso object.
 * @retval none
 */
void Calypso_HandleEvents(CALYPSO *self)
{
    strcpy(eventbuffer, self->bufferCalypso.data);
    pEventBuffer = eventbuffer;
    bool ret = false;
    ATEvent_t event;

    ATEvent_parseEventName(&pEventBuffer, &event);

    switch (event)
    {
    case ATEvent_Startup:
    {
        char value[32];
        Calypso_getNextArgumentString(&pEventBuffer, value, ARGUMENT_DELIM);
        Calypso_getNextArgumentString(&pEventBuffer, value, ARGUMENT_DELIM);
        Calypso_getNextArgumentString(&pEventBuffer, value, ARGUMENT_DELIM);
        self->status = calypso_started;
        strcpy(self->MAC_ADDR, value);
        // SSerial_printf(self->serialDebug, "Calypso MAC address: %s\r\n", self->MAC_ADDR);
        eventPending = false;
        break;
    }

    case ATEvent_NetappIP4Aquired:
    {
        // ATEvent_parseEventArgumentValues(&pEventBuffer, event, &eventArguments[0]);
        self->status = calypso_WLAN_connected;
        eventPending = false;
        // ATEvent_NetappIP4Aquired_t *ip4Acquired = (ATEvent_NetappIP4Aquired_t *)eventArguments;
        // SSerial_printf(self->serialDebug, "Calypso connected. IP acquired: %s\r\n", ip4Acquired->address);
        break;
    }
    case ATEvent_MQTTOperation:
    {
        char value[32];
        int connackCode = 0;

        ret = Calypso_getNextArgumentString(&pEventBuffer, value, ARGUMENT_DELIM);
        if (ret)
        {
            ret = false;
            if (0 == strcasecmp(value, "connack"))
            {
                ret = Calypso_getNextArgumentInt(&pEventBuffer, &connackCode, INTFLAGS_SIZE16, STRING_TERMINATE);
                if (ret)
                {
                    switch (connackCode)
                    {
                    case 0:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection accepted %i\r\n", connackCode);
#endif
                        self->status = calypso_MQTT_connected;
                        break;
                    case 1:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused, Unacceptable protocol version %i\r\n", connackCode);
#endif
                        break;
                    case 2:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused,Identifier rejected %i\r\n", connackCode);
#endif
                        break;
                    case 3:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused, Server unavailable %i\r\n", connackCode);
#endif
                        break;
                    case 4:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused, Bad user name or password %i\r\n", connackCode);
#endif
                        break;
                    case 5:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused, not authorized %i\r\n", connackCode);
#endif
                        break;
                    case 256:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection accepted %i\r\n", connackCode);
#endif
                        self->status = calypso_MQTT_connected;
                        break;
                    default:
#if SERIAL_DEBUG
                        SSerial_printf(self->serialDebug, "MQTT connection refused %i\r\n", connackCode);
#endif
                        break;
                    }
                }
            }
            else if (0 == strcasecmp(value, "puback"))
            {
                // SSerial_printf(self->serialDebug, "MQTT Puback\r\n");
            }
        }

        eventPending = false;
        break;
    }

    default:
    {
        break;
    }
    }
}

/**
 * @brief  Process the line received from calypso
 * @param  self Pointer to the calypso object.
 * @param  rxPacket Pointer to the line received
 * @param  rxLength Length of the line
 * @retval none
 */
void Calypso_HandleRxLine(CALYPSO *self, char *rxPacket, uint16_t rxLength)
{
    /* AT command was sent to module. Waiting fot response*/
    if (requestPending)
    {
        /* If starts with 'O', check if response is "OK\r\n" */
        if (('O' == rxPacket[0]) || ('o' == rxPacket[0]))
        {
            if (0 == strncasecmp(&rxPacket[0], RESPONSE_OK, strlen(RESPONSE_OK)))
            {
                cmdConfirmation = Calypso_CNFStatus_Success;
            }
            else
            {
                cmdConfirmation = Calypso_CNFStatus_Failed;
            }
        }
        /* If starts with 'E', check if response is "Error:[arguments]\r\n" */
        else if (('E' == rxPacket[0]) || ('e' == rxPacket[0]))
        {
            if (!(0 == strncasecmp(&rxPacket[0], RESPONSE_Error, strlen(RESPONSE_Error))))
            {

                cmdConfirmation = Calypso_CNFStatus_Success;
            }
            else
            {
                cmdConfirmation = Calypso_CNFStatus_Failed;
            }
        }
        else
        {
            if (rxLength < CALYPSO_LINE_MAX_SIZE)
            {
                memcpy(self->bufferCalypso.data, RxBuffer, rxLength);
                lengthResponse += rxLength;
                self->bufferCalypso.length = lengthResponse;
            }
        }
    }
    else
    /* If no request is pending, an event was received*/
    {
        if (rxLength != 0)
        {
            memcpy(self->bufferCalypso.data, RxBuffer, rxLength);
            self->bufferCalypso.length = rxLength;
            Calypso_HandleEvents(self);
        }
    }
}

/**
 * @brief  Receive bytes on the calypso UART port
 * @param  self Pointer to the calypso object.
 * @retval none
 */
void Calypso_RxBytes(CALYPSO *self)
{
    uint16_t RxByteCounter = 0;
    uint8_t readBuffer;
    bool returnFound = false;
    unsigned long start = millis();
    while (!HSerial_available(self->serialCalypso) &&
           start <= RESPONSE_WAIT_TIME)
        ;
    while (HSerial_available(self->serialCalypso))
    {
        readBuffer = HSerial_read(self->serialCalypso);
        switch (RxByteCounter)
        {
        case 0:
        {
            /* Possible responses: OK, Error, +[event], +[cmdResponse] */
            if (('O' == readBuffer) || ('o' == readBuffer) || ('E' == readBuffer) || ('e' == readBuffer) || ('+' == readBuffer))
            {
                RxBuffer[RxByteCounter++] = readBuffer;
            }
            break;
        }
        default:
        {

            if (RxByteCounter >= CALYPSO_LINE_MAX_SIZE)
            {
                RxByteCounter = 0;
            }

            if (readBuffer == '\r')
            {
                returnFound = true;
            }
            else if (returnFound && (readBuffer == '\n'))
            {
                /* interpret it now */
                RxBuffer[RxByteCounter++] = (uint8_t)'\0';
#if SERIAL_DEBUG
                SSerial_printf(self->serialDebug, "%s\n", RxBuffer);
#endif
                Calypso_HandleRxLine(self, RxBuffer, RxByteCounter);
                returnFound = false;
                RxByteCounter = 0;
            }
            else
            {
                RxBuffer[RxByteCounter++] = readBuffer;
            }
            break;
        }
        }
    }
}
