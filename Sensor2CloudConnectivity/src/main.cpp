/**
 * \file
 * \brief Main file for the WE-CalypsoWiFiFeatherWing.
 *
 * \copyright (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
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
#include "json-builder.h"
#include "sensorBoard.h"

#define AZURE_CONNECTION 0
#define AWS_CONNECTION 1

#define HIDS_PART_NUMBER 2525020210001

// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

#if AZURE_CONNECTION
/*MQTT settings - Azure server*/
#define MQTT_CLIENT_ID "iot-test-dev-1"
#define MQTT_SERVER_ADDRESS "iot-hub-1.azure-devices.net"
#define MQTT_PORT 8883
#define MQTT_TOPIC "devices/iot-test-dev-1/messages/events/"
#define MQTT_USER_NAME "iot-hub-1.azure-devices.net/iot-test-dev-1/?api-version=2021-04-12"
#define MQTT_PASSWORD                                                \
    "SharedAccessSignature "                                         \
    "sr=iot-hub-1.azure-devices.net%2Fdevices%2Fiot-test-dev-1&sig=" \
    "px4bCMMLj8fV7ioP2pHG*****%2FKvT52uNB8f%2B1hhHLk%3D&se=**********"

#define ROOT_CA_PATH "azrootca"

#define DIGICERT_CYBERTRUST_ROOT_CERT "-----BEGIN CERTIFICATE-----\n\
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n\
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n\
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n\
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n\
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n\
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n\
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n\
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n\
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n\
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n\
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n\
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n\
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n\
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n\
MrY=\n\
-----END CERTIFICATE-----"
#endif

#if AWS_CONNECTION
/*MQTT settings - AWS*/
#define MQTT_CLIENT_ID "test_dev"
#define MQTT_SERVER_ADDRESS "************-ats.iot.us-east-1.amazonaws.com"
#define MQTT_PORT 8883
#define MQTT_TOPIC "test"
#define MQTT_CERT_PATH "cert"
#define MQTT_PRIV_KEY_PATH "key"
#define MQTT_USER_NAME "calypso"
#define MQTT_PASSWORD "calypso"
#define ROOT_CA_PATH "root"

#define STARFIELD_ROOT_CERT "-----BEGIN CERTIFICATE-----\n\
MIIEDzCCAvegAwIBAgIBADANBgkqhkiG9w0BAQUFADBoMQswCQYDVQQGEwJVUzEl\n\
MCMGA1UEChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMp\n\
U3RhcmZpZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQw\n\
NjI5MTczOTE2WhcNMzQwNjI5MTczOTE2WjBoMQswCQYDVQQGEwJVUzElMCMGA1UE\n\
ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMpU3RhcmZp\n\
ZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwggEgMA0GCSqGSIb3\n\
DQEBAQUAA4IBDQAwggEIAoIBAQC3Msj+6XGmBIWtDBFk385N78gDGIc/oav7PKaf\n\
8MOh2tTYbitTkPskpD6E8J7oX+zlJ0T1KKY/e97gKvDIr1MvnsoFAZMej2YcOadN\n\
+lq2cwQlZut3f+dZxkqZJRRU6ybH838Z1TBwj6+wRir/resp7defqgSHo9T5iaU0\n\
X9tDkYI22WY8sbi5gv2cOj4QyDvvBmVmepsZGD3/cVE8MC5fvj13c7JdBmzDI1aa\n\
K4UmkhynArPkPw2vCHmCuDY96pzTNbO8acr1zJ3o/WSNF4Azbl5KXZnJHoe0nRrA\n\
1W4TNSNe35tfPe/W93bC6j67eA0cQmdrBNj41tpvi/JEoAGrAgEDo4HFMIHCMB0G\n\
A1UdDgQWBBS/X7fRzt0fhvRbVazc1xDCDqmI5zCBkgYDVR0jBIGKMIGHgBS/X7fR\n\
zt0fhvRbVazc1xDCDqmI56FspGowaDELMAkGA1UEBhMCVVMxJTAjBgNVBAoTHFN0\n\
YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xMjAwBgNVBAsTKVN0YXJmaWVsZCBD\n\
bGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8w\n\
DQYJKoZIhvcNAQEFBQADggEBAAWdP4id0ckaVaGsafPzWdqbAYcaT1epoXkJKtv3\n\
L7IezMdeatiDh6GX70k1PncGQVhiv45YuApnP+yz3SFmH8lU+nLMPUxA2IGvd56D\n\
eruix/U0F47ZEUD0/CwqTRV/p2JdLiXTAAsgGh1o+Re49L2L7ShZ3U0WixeDyLJl\n\
xy16paq8U4Zt3VekyvggQQto8PT7dL5WXXp59fkdheMtlb71cZBDzI0fmgAKhynp\n\
VSJYACPq4xJDKVtHCN2MQWplBqjlIapBtJUhlbl90TSrE9atvNziPTnNvT51cKEY\n\
WQPJIrSPnNVeKtelttQKbfi3QBFGmh95DmK/D5fs4C8fF5Q=\n\
-----END CERTIFICATE-----"

#define MQTT_CERTIFICATE "-----BEGIN CERTIFICATE-----\n\
-----END CERTIFICATE-----"

#define MQTT_PRIV_KEY "-----BEGIN RSA PRIVATE KEY-----\n\
-----END RSA PRIVATE KEY-----"
#endif

// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"

// Calypso settings
CalypsoSettings calSettings;

int msgID;

char *buf;

char macAddress[18];

void Calypso_EventCallback(char *eventText);

static float PADS_pressure, PADS_temp;
static float ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp;
static bool ITDS_doubleTapEvent, ITDS_freeFallEvent;
static float TIDS_temp;
static float HIDS_humidity, HIDS_temp;

/* Startup State Machine */
typedef enum
{
    Calypso_Sensor2Cloud_SM_ModuleUp,
    Calypso_Sensor2Cloud_SM_WLAN_Connect,
    Calypso_Sensor2Cloud_SM_SNTP_Setup,
    Calypso_Sensor2Cloud_SM_MQTT_Connect,
    Calypso_Sensor2Cloud_SM_Publish_Data,
    Calypso_Sensor2Cloud_SM_Publish_Ack,
    Calypso_Sensor2Cloud_SM_Idle,
    Calypso_Sensor2Cloud_SM_Error
} Calypso_Sensor2Cloud_SM_t;

static volatile Calypso_Sensor2Cloud_SM_t calypsoSensor2CloudCurrentState =
    Calypso_Sensor2Cloud_SM_Idle;

static bool sntpSetupFinished = false;

void serializeData();

void setup()
{
    delay(3000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SecurityType_WPA_WPA2;

#if AZURE_CONNECTION
    // MQTT Settings - AZURE
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags =
        ATMQTT_CreateFlags_URL | ATMQTT_CreateFlags_Secure;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;

    calSettings.mqttSettings.secParams.securityMethod =
        ATMQTT_SecurityMethod_SSLV3_TLSV1_2;
    calSettings.mqttSettings.secParams.cipher =
        ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA256;

    calSettings.mqttSettings.connParams.protocolVersion =
        ATMQTT_ProtocolVersion_v3_1_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;
    strcpy(calSettings.mqttSettings.userOptions.userName, MQTT_USER_NAME);
    strcpy(calSettings.mqttSettings.userOptions.passWord, MQTT_PASSWORD);
    strcpy(calSettings.mqttSettings.secParams.CAFile, ROOT_CA_PATH);

#endif

#if AWS_CONNECTION
    // MQTT Settings - AWS
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags =
        ATMQTT_CreateFlags_URL | ATMQTT_CreateFlags_Secure;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;

    calSettings.mqttSettings.secParams.securityMethod =
        ATMQTT_SecurityMethod_TLSV1_2;
    calSettings.mqttSettings.secParams.cipher =
        ATSocket_Cipher_TLS_RSA_WITH_AES_128_CBC_SHA256;

    strcpy(calSettings.mqttSettings.secParams.certificateFile, MQTT_CERT_PATH);
    strcpy(calSettings.mqttSettings.secParams.privateKeyFile, MQTT_PRIV_KEY_PATH);
    strcpy(calSettings.mqttSettings.secParams.CAFile, ROOT_CA_PATH);

    calSettings.mqttSettings.connParams.protocolVersion =
        ATMQTT_ProtocolVersion_v3_1_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;

#endif

    // SNTP settings
    strcpy(calSettings.sntpSettings.timezone, SNTP_TIMEZONE);
    strcpy(calSettings.sntpSettings.server, SNTP_SERVER);

    if (!sensorBoard_Init())
    {
        WE_DEBUG_PRINT("I2C init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }
    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit())
    {
        WE_DEBUG_PRINT("PADS init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }

    if (!ITDS_2533020201601_simpleInit())
    {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }

    if (!TIDS_2521020222501_simpleInit())
    {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }
#endif

    // Initialize Calypso
    if (!Calypso_simpleInit(&calSettings, &Calypso_EventCallback))
    {
        WE_DEBUG_PRINT("Calypso init failed \r\n");
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
    }

    while (calypsoSensor2CloudCurrentState != Calypso_Sensor2Cloud_SM_WLAN_Connect && calypsoSensor2CloudCurrentState != Calypso_Sensor2Cloud_SM_SNTP_Setup)
    {
        switch (calypsoSensor2CloudCurrentState)
        {
        case Calypso_Sensor2Cloud_SM_ModuleUp:
        {
            WE_DEBUG_PRINT("module has started\r\n");
            // check if autoconnect feature will connect fine if not
            // manually connect to chosen ap
            WE_Delay(30);
            if (calypsoSensor2CloudCurrentState == Calypso_Sensor2Cloud_SM_ModuleUp)
            {
                calypsoSensor2CloudCurrentState =
                    Calypso_Sensor2Cloud_SM_WLAN_Connect;
            }

#if AZURE_CONNECTION
            if (!Calypso_fileExists(ROOT_CA_PATH))
            {
                if (!Calypso_writeFile(ROOT_CA_PATH, DIGICERT_CYBERTRUST_ROOT_CERT, strlen(DIGICERT_CYBERTRUST_ROOT_CERT)))
                {
                    WE_DEBUG_PRINT("write file fail\r\n");
                    calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
                    break;
                }
            }
#endif

#if AWS_CONNECTION
            if (!Calypso_fileExists(MQTT_CERT_PATH))
            {
                if (!Calypso_writeFile(MQTT_CERT_PATH, MQTT_CERTIFICATE, strlen(MQTT_CERTIFICATE)))
                {
                    WE_DEBUG_PRINT("write file fail\r\n");
                    calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
                    break;
                }
            }

            if (!Calypso_fileExists(MQTT_PRIV_KEY_PATH))
            {
                if (!Calypso_writeFile(MQTT_PRIV_KEY_PATH, MQTT_PRIV_KEY, strlen(MQTT_PRIV_KEY)))
                {
                    WE_DEBUG_PRINT("write file fail\r\n");
                    calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
                    break;
                }
            }

            if (!Calypso_fileExists(ROOT_CA_PATH))
            {
                if (!Calypso_writeFile(ROOT_CA_PATH, STARFIELD_ROOT_CERT, strlen(STARFIELD_ROOT_CERT)))
                {
                    WE_DEBUG_PRINT("write file fail\r\n");
                    calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
                    break;
                }
            }
#endif
            break;
        }
        case Calypso_Sensor2Cloud_SM_Error:
        {
            WE_DEBUG_PRINT("Error state in start up \r\n");
            return;
        }
        default:
            break;
        }
    }

    WE_DEBUG_PRINT("Startup finished\r\n");
    // The message ID acts as the packet number
    msgID = 0;
}

void loop()
{
    switch (calypsoSensor2CloudCurrentState)
    {
    case Calypso_Sensor2Cloud_SM_Publish_Data:
    {
        if (PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp))
        {
            WE_DEBUG_PRINT("WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
                           PADS_pressure, PADS_temp);
        }
        if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                              &ITDS_accelZ, &ITDS_temp))
        {
            WE_DEBUG_PRINT(
                "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g Temp: "
                "%f °C \r\n",
                ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp);
        }
        if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent))
        {
            WE_DEBUG_PRINT("WSEN_ITDS(DoubleTap): State %s \r\n",
                           ITDS_doubleTapEvent ? "True" : "False");
        }
        if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent))
        {
            WE_DEBUG_PRINT("WSEN_ITDS(FreeFall): State %s \r\n",
                           ITDS_freeFallEvent ? "True" : "False");
        }
        if (TIDS_2521020222501_readSensorData(&TIDS_temp))
        {
            WE_DEBUG_PRINT("WSEN_TIDS(Temperature): %f °C\r\n", TIDS_temp);
        }
#if HIDS_PART_NUMBER == 2525020210001
        if (HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp))
        {
            WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                           HIDS_temp);
        }
#elif HIDS_PART_NUMBER == 2525020210002
        if (HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp))
        {
            WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                           HIDS_temp);
        }
#endif
        WE_DEBUG_PRINT(
            "----------------------------------------------------\r\n");
        WE_DEBUG_PRINT("\r\n");

        serializeData();

        WE_DEBUG_PRINT(buf);
        WE_DEBUG_PRINT("\r\n");

        /*Publish to MQTT topic*/
        if (!Calypso_MQTTPublishData(MQTT_TOPIC, 0, buf, strlen(buf)))
        {
            WE_DEBUG_PRINT("Publish failed\n\r");
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }

        WE_DEBUG_PRINT("Data published\n\r");

        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Idle;
        /*Clean-up*/
        free(buf);
        break;
    }
    case Calypso_Sensor2Cloud_SM_Publish_Ack:
    {
        WE_DEBUG_PRINT("Publish Data Ack received\n\r");
        WE_Delay(5000);
        calypsoSensor2CloudCurrentState =
            Calypso_Sensor2Cloud_SM_Publish_Data;
        break;
    }
    case Calypso_Sensor2Cloud_SM_WLAN_Connect:
    {
        if (!Calypso_WLANconnect())
        {
            WE_DEBUG_PRINT("WiFi connect fail\r\n");
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }
        else if (calypsoSensor2CloudCurrentState ==
                 Calypso_Sensor2Cloud_SM_WLAN_Connect)
        {
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Idle;
        }
        break;
    }
    case Calypso_Sensor2Cloud_SM_SNTP_Setup:
    {
        WE_DEBUG_PRINT("WiFi connected\r\n");
        if (!Calypso_setUpSNTP())
        {
            WE_DEBUG_PRINT("SNTP config fail\r\n");
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }
        else
        {
            sntpSetupFinished = true;
            calypsoSensor2CloudCurrentState =
                Calypso_Sensor2Cloud_SM_MQTT_Connect;
        }
        break;
    }
    case Calypso_Sensor2Cloud_SM_MQTT_Connect:
    {
        if (!Calypso_MQTTconnect())
        {
            WE_DEBUG_PRINT("MQTT connect fail\r\n");
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }
        else if (calypsoSensor2CloudCurrentState ==
                 Calypso_Sensor2Cloud_SM_MQTT_Connect)
        {
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Idle;
        }
        break;
    }
    case Calypso_Sensor2Cloud_SM_Error:
    {
        WE_DEBUG_PRINT("Application Error\n\r");
        calypsoSensor2CloudCurrentState =
            Calypso_Sensor2Cloud_SM_Idle;
        return;
    }
    default:
        break;
    }
}

/**
 * @brief  Serialize data to send
 */
void serializeData()
{
    Timestamp timestamp;
    Timer_initTime(&timestamp);

    msgID++;
    if (msgID == INT_MAX)
    {
        msgID = 0;
    }

    // Get the current time from calypso
    if (!Calypso_getTimestamp(&timestamp))
    {
        WE_DEBUG_PRINT("Get time fail\r\n");
    }

    /* convert to unix timestamp */
    unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

    /*Create a JSON object with the device ID, message ID, and time stamp*/
    json_value *payload = json_object_new(1);
    json_object_push(payload, "deviceId", json_string_new(macAddress));
    json_object_push(payload, "messageId", json_integer_new(msgID));
    json_object_push(payload, "ts", json_integer_new(unixTime_ms));

    /*Push sensor data into the JSON object*/

    // PADS data
    json_object_push(payload, "PADS_T",
                     json_double_new(PADS_temp));
    json_object_push(payload, "PADS_P",
                     json_double_new(PADS_pressure));

    // ITDS data
    json_object_push(payload, "ITDS_X",
                     json_double_new(ITDS_accelX));
    json_object_push(payload, "ITDS_Y",
                     json_double_new(ITDS_accelY));
    json_object_push(payload, "ITDS_Z",
                     json_double_new(ITDS_accelZ));
    json_object_push(payload, "ITDS_T",
                     json_double_new(ITDS_temp));
    json_object_push(payload, "ITDS_Double_Tap",
                     json_double_new((double)ITDS_doubleTapEvent));
    json_object_push(payload, "ITDS_Free_Fall",
                     json_double_new((double)ITDS_freeFallEvent));

    // TIDS data
    json_object_push(payload, "TIDS_T",
                     json_double_new(TIDS_temp));

    // HIDS data
    json_object_push(payload, "HIDS_T",
                     json_double_new(HIDS_temp));
    json_object_push(payload, "HIDS_RH",
                     json_double_new(HIDS_humidity));

    buf = (char *)malloc(json_measure(payload));

    json_serialize(buf, payload);

    json_builder_free(payload);
}

void Calypso_EventCallback(char *eventText)
{
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    if (ATEvent_Invalid == event)
    {
        return;
    }

    switch (event)
    {
    case ATEvent_Startup:
    {
        ATEvent_Startup_t startUp;
        if (ATEvent_ParseStartUpEvent(&eventText, &startUp))
        {
            strcpy(macAddress, startUp.MACAddress);
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_ModuleUp;
        }
        else
        {
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }
        break;
    }
    case ATEvent_NetappIP4Acquired:
    {
        ATEvent_NetappIP4Acquired_t ipAcquired;
        calypsoSensor2CloudCurrentState =
            ATEvent_ParseNetappIP4AcquiredEvent(&eventText, &ipAcquired)
                ? (sntpSetupFinished ? Calypso_Sensor2Cloud_SM_MQTT_Connect : Calypso_Sensor2Cloud_SM_SNTP_Setup)
                : Calypso_Sensor2Cloud_SM_Error;
        break;
    }
    case ATEvent_MQTTConnack:
    {
        ATEvent_MQTTConnack_t mqttConnack;
        if (!ATEvent_ParseMQTTConnackEvent(&eventText, &mqttConnack) ||
            (mqttConnack.returnCode != MQTTConnack_Return_Code_Accepted))
        {
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Error;
        }
        else
        {
            calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Publish_Data;
        }
        break;
    }
    case ATEvent_MQTTPuback:
    {
        calypsoSensor2CloudCurrentState = Calypso_Sensor2Cloud_SM_Publish_Ack;
        break;
    }
    default:
        break;
    }
}
