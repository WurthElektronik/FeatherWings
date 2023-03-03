/**
 * \file
 * \brief Main file for the WE-CalypsoWiFiFeatherWing.
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
#include "json-builder.h"
#include "sensorBoard.h"

#define AZURE_CONNECTION 1
#define AWS_CONNECTION 0

// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

#if AZURE_CONNECTION
/*MQTT settings - Azure server*/
#define MQTT_CLIENT_ID "cust-test-dev-1"
#define MQTT_SERVER_ADDRESS "cust-test-iot-1.azure-devices.net"
#define MQTT_PORT 8883
#define MQTT_TOPIC "devices/cust-test-dev-1/messages/events/"
#define MQTT_USER_NAME "cust-test-iot-1.azure-devices.net/cust-test-dev-1/?api-version=2021-04-12"
#define MQTT_PASSWORD                                                       \
    "SharedAccessSignature "                                                \
    "sr=cust-test-iot-1.azure-devices.net%2Fdevices%2Fcust-test-dev-1&sig=" \
    "07hjStKVl1mmeiSHscxMCRP8CfUK3qDQymsqV%2FNBc9M%3D&se=1713832997"

#define ROOT_CA_PATH "azrootca"

#define BALTIMORE_CYBERTRUST_ROOT_CERT "-----BEGIN CERTIFICATE-----\n\
MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n\
RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n\
VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n\
DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n\
ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n\
VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n\
mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n\
IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n\
mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n\
XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n\
dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n\
jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n\
BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n\
DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n\
9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n\
jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n\
Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n\
ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n\
R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n\
-----END CERTIFICATE-----"
#endif

#if AWS_CONNECTION
/*MQTT settings - AWS*/
#define MQTT_CLIENT_ID "we-iot-device-t1"
#define MQTT_SERVER_ADDRESS "a18jcdjlx073x-ats.iot.eu-central-1.amazonaws.com"
#define MQTT_PORT 8883
#define MQTT_TOPIC "test"
#define MQTT_CERT_PATH "cert"
#define MQTT_PRIV_KEY_PATH "key"
#define MQTT_USER_NAME "calypso"
#define MQTT_PASSWORD "calypso"

#define MQTT_CERTIFICATE "-----BEGIN CERTIFICATE-----\n\
MIIDWjCCAkKgAwIBAgIVAKp1kf+CB1VVGFgrJHccil3ucTNIMA0GCSqGSIb3DQEB\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDExMjAxNTI2\n\
NTlaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCrTkrKuNBZoJLaPerh\n\
953ydia751Lve7TYW87+4pfc6WAO00hgCSsJGBukQ5iTMFbeOYZwHqnivjy17nyS\n\
r+X3UeCiV2OMNgKdhG5saA7gN7hQl154NVNAd08NAD7ViKjy7hT22xuRWM8xsEgT\n\
h8BrAV1WcspKsqsjZzoc+c3zPkbjRdMmxfGZ90jli/nCerypgQUWSB4bu2jMs/kC\n\
NycJJwXAxEzBJUY6qXg2mvTPsHNSS272CRLgcg5khmpP9gcoA8ZyTlKuDAicc8t2\n\
pkYmRvNWU2DVsRY9lFfaABw5Z/SmQVaBf2Y/0FKBaZPbiDOmkvPXvcCdXNyD3wFc\n\
FBpdAgMBAAGjYDBeMB8GA1UdIwQYMBaAFDEcUkMcHs+IVnn2i7MdFImkNf2fMB0G\n\
A1UdDgQWBBSl0pMEsvIZoC+hP9Zp9VEFRKJhzDAMBgNVHRMBAf8EAjAAMA4GA1Ud\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAL9fsXzHqDHnzqDWBLJf7szxS\n\
A2me9I3QcsPf40fNNHUaSfFddOQmIjtIethrubNDnRjdTGqQ/M0lWh64KATN5reM\n\
IaWrZ/Wm/8VEjOki+kH58lzWF2dUniI7HJN7efKYUqNLyYCqdxOWYa9ZZkn8yc8/\n\
XTYN6nM5kdhDctMj1C7aKC7eDDY9GUbfxRl8CtknJ+evbv9GviW9053J07ALkpRX\n\
rB3MV2VW+oSZn/Ff12i8g5xMBt3wDPwHdjcLEKW6HUsmjLvWeWUQRgUek9+b+k4u\n\
Po2wy9Usok9WEzzzOC4eXil0Xu+3Mf5KAU8nHWdyYAC0VEWFe9Z72kV85TvbJA==\n\
-----END CERTIFICATE-----"

#define MQTT_PRIV_KEY "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAq05KyrjQWaCS2j3q4fed8nYmu+dS73u02FvO/uKX3OlgDtNI\n\
YAkrCRgbpEOYkzBW3jmGcB6p4r48te58kq/l91HgoldjjDYCnYRubGgO4De4UJde\n\
eDVTQHdPDQA+1Yio8u4U9tsbkVjPMbBIE4fAawFdVnLKSrKrI2c6HPnN8z5G40XT\n\
JsXxmfdI5Yv5wnq8qYEFFkgeG7tozLP5AjcnCScFwMRMwSVGOql4Npr0z7BzUktu\n\
9gkS4HIOZIZqT/YHKAPGck5SrgwInHPLdqZGJkbzVlNg1bEWPZRX2gAcOWf0pkFW\n\
gX9mP9BSgWmT24gzppLz173AnVzcg98BXBQaXQIDAQABAoIBABPe5woQ2goreB1c\n\
pUxE3strLR8KvDIPVXDrZV1nh1oWsA/ILlMFfTp2024AcUhRiSIJ5jBHPkmQ65Xm\n\
7ghN4w0HMFlkbaWr9i2zWOO2RlN34ydmB41GEjweGstVRfSa/43+U+w2ikIX3SDU\n\
Y+fwDT/cTqlic1iq1PMsXC9UQrF+TSvyWg/xKRCuHNnBXdMvolYUaWf+JQ1U0mpW\n\
XQV6RoLwFficE0VTLygZJnVBPotTz9djarWQ+UXHcsJT7y9tBGvWqHnmOzIdezUk\n\
rqLwY51YpTAF+SKniGkICpqq/+nCmsWTxyT9HtL6bb1yMB+xURaYfU/NZjWbmBSX\n\
Eb1lVgECgYEA3hwTYVU2maSVMxm68feQFDoovoSwBE21Tf/wd7zYExu0uTOAPfPu\n\
cFnqarVRChNrxPaujacMuK4sEXRtHh/xsn6UcazFK96rZpQKX3vt3x2O9kiuADRJ\n\
2dFIma7aq8/sZicYWp9cXgxuki1BdiKwNwWQZpOF9292AwzTAPu3AUECgYEAxXG+\n\
vF2s8ah/D85vlo65K9iFfDvN8jhf9YQQ6ZpBx9kE5BFFiya1PItnEvJhbdkVF5Yq\n\
YXGdXA6NoYzva/+m/OAFG8YveI/pCnkGoDBdQiKBWV5cQ0/Rrn8BEC2EURGBVLQl\n\
hlSfxZZpm2y9/GMxRH6+IGr868N+iKNTLheFdh0CgYEA0siML9vBpE/H9CXf/0+0\n\
4S1Mi3m63WqtvCc/GzMRUBkECppwgtrjFqaOS9Rk4w4JQXPltbKp5P4N/kaiY2tn\n\
YcC0uah/uiFoQkIOEg23cf1INjxFPRZiW588qfSBu3noXA2QFDiWXP6pVHo0XJuQ\n\
5baXEnHAOlECCuT5vj4jr8ECgYBLgrzXfVvu7+noOaMjiWH4Cs7CPHz+7eCFHQT6\n\
0ivmKnFcZ96Y4SzfAtFgxaHNSQBwDNYYfkMYOdiguC24uAU9IM/TV3BAQ4l0n+SQ\n\
zu5bpKajbxsKAzTF73yQm1fHSVKU+nB/d03DW0r4ThY6uBTXhUFhVIl2AUYbK5tc\n\
PVKlYQKBgASDG3K2pCzFmBAc0l8f7cLh4K8WIOyqaQTIB12Yh2eLlXIlt0KT1O8n\n\
KGoEDN5Sy5hRlC4kmufj1+f+FDXu6yuQ9QEqx0HaG9vezcipzZMU5RlQbYS7FxKi\n\
n1LfYKu6miDABaOP1fB2Qc7eMYcpby6gPilZqale6Htthaz5+J1+\n\
-----END RSA PRIVATE KEY-----"
#endif

// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"

// USB-Serial debug Interface
TypeSerial *SerialDebug;

// Calypso settings
CalypsoSettings calSettings;

// Calypso object
CALYPSO *calypso;

// Serial communication port for Calypso
TypeHardwareSerial *SerialCalypso;

// Sensors
// WE absolute pressure sensor object
PADS *sensorPADS;
// WE 3-axis acceleration sensor object
ITDS *sensorITDS;
// WE Temperature sensor object
TIDS *sensorTIDS;
// WE humidity sensor object
HIDS *sensorHIDS;

int msgID;

char *serializeData();
void setup()
{
    delay(5000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialCalypso = HSerial_create(&Serial1);

    // Create serial port for Calypso WiFi FeatherWing baud 921600, 8E1
    HSerial_beginP(SerialCalypso, 921600, (uint8_t)SERIAL_8E1);
    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SECURITY_TYPE_WPA_WPA2;

#if AZURE_CONNECTION
    // MQTT Settings - AZURE
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags =
        ATMQTT_CREATE_FLAGS_URL | ATMQTT_CREATE_FLAGS_SEC;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;

    calSettings.mqttSettings.secParams.securityMethod =
        ATMQTT_SECURITY_METHOD_TLSV1_2;
    calSettings.mqttSettings.secParams.cipher =
        ATMQTT_CIPHER_TLS_RSA_WITH_AES_256_CBC_SHA;

    calSettings.mqttSettings.connParams.protocolVersion =
        ATMQTT_PROTOCOL_v3_1_1;
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
        ATMQTT_CREATE_FLAGS_URL | ATMQTT_CREATE_FLAGS_SEC;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;

    calSettings.mqttSettings.secParams.securityMethod =
        ATMQTT_SECURITY_METHOD_TLSV1_2;
    calSettings.mqttSettings.secParams.cipher =
        ATMQTT_CIPHER_TLS_RSA_WITH_AES_128_CBC_SHA256;

    strcpy(calSettings.mqttSettings.secParams.certificateFile, MQTT_CERT_PATH);

    strcpy(calSettings.mqttSettings.secParams.privateKeyFile, MQTT_PRIV_KEY_PATH);

    calSettings.mqttSettings.connParams.protocolVersion =
        ATMQTT_PROTOCOL_v3_1_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;
    strcpy(calSettings.mqttSettings.userOptions.userName, MQTT_USER_NAME);

    strcpy(calSettings.mqttSettings.userOptions.passWord, MQTT_PASSWORD);

#endif

    // SNTP settings
    strcpy(calSettings.sntpSettings.timezone, SNTP_TIMEZONE);
    strcpy(calSettings.sntpSettings.server, SNTP_SERVER);

    // Create sensor objects
    sensorPADS = PADSCreate(SerialDebug);
    sensorITDS = ITDSCreate(SerialDebug);
    sensorTIDS = TIDSCreate(SerialDebug);
    sensorHIDS = HIDSCreate(SerialDebug);

    // Initialize the sensors in default mode
    if (!PADS_simpleInit(sensorPADS))
    {
        SSerial_printf(SerialDebug, "PADS init failed \r\n");
    }

    if (!ITDS_simpleInit(sensorITDS))
    {
        SSerial_printf(SerialDebug, "ITDS init failed \r\n");
    }
    if (!TIDS_simpleInit(sensorTIDS))
    {
        SSerial_printf(SerialDebug, "TIDS init failed \r\n");
    }
    if (!HIDS_simpleInit(sensorHIDS))
    {
        SSerial_printf(SerialDebug, "HIDS init failed \r\n");
    }

    calypso = Calypso_Create(SerialDebug, SerialCalypso, &calSettings);

    // Initialize Calypso
    if (!Calypso_simpleInit(calypso))
    {
        SSerial_printf(SerialDebug, "Calypso init failed \r\n");
    }

#if AWS_CONNECTION
    if (!Calypso_fileExists(calypso, MQTT_CERT_PATH))
    {
        Calypso_writeFile(calypso, MQTT_CERT_PATH, MQTT_CERTIFICATE, strlen(MQTT_CERTIFICATE));
    }

    if (!Calypso_fileExists(calypso, MQTT_PRIV_KEY_PATH))
    {
        Calypso_writeFile(calypso, MQTT_PRIV_KEY_PATH, MQTT_PRIV_KEY, strlen(MQTT_PRIV_KEY));
    }
#endif

#if AZURE_CONNECTION
    if (!Calypso_fileExists(calypso, ROOT_CA_PATH))
    {
        Calypso_writeFile(calypso, ROOT_CA_PATH, BALTIMORE_CYBERTRUST_ROOT_CERT, strlen(BALTIMORE_CYBERTRUST_ROOT_CERT));
    }
#endif
    // Connect Calypso WiFi FeatherWing to the Wi-Fi access point
    if (!Calypso_WLANconnect(calypso))
    {
        SSerial_printf(SerialDebug, "WiFi connect fail\r\n");
        return;
    }

    delay(3000);

    // Set up SNTP client on Calypso
    if (!Calypso_setUpSNTP(calypso))
    {
        SSerial_printf(SerialDebug, "SNTP config fail\r\n");
    }

    // Connect to MQTT server
    if (!Calypso_MQTTconnect(calypso))
    {
        SSerial_printf(SerialDebug, "MQTT connect fail\r\n");
    }

    // The message ID acts as the packet number
    msgID = 0;
}

void loop()
{
    if (PADS_readSensorData(sensorPADS))
    {
        SSerial_printf(
            SerialDebug, "WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
            sensorPADS->data[padsPressure], sensorPADS->data[padsTemperature]);
    }
    if (ITDS_readSensorData(sensorITDS))
    {
        SSerial_printf(SerialDebug,
                       "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g\r\n",
                       sensorITDS->data[itdsXAcceleration],
                       sensorITDS->data[itdsYAcceleration],
                       sensorITDS->data[itdsZAcceleration]);
    }
    if (TIDS_readSensorData(sensorTIDS))
    {
        SSerial_printf(SerialDebug, "WSEN_TIDS(Temperature): %f °C\r\n",
                       sensorTIDS->data[tidsTemperature]);
    }
    if (HIDS_readSensorData(sensorHIDS))
    {
        SSerial_printf(SerialDebug, "WSEN_HIDS: RH: %f %% Temp: %f °C\r\n",
                       sensorHIDS->data[hidsRelHumidity],
                       sensorHIDS->data[hidsTemperature]);
    }
    SSerial_printf(SerialDebug,
                   "----------------------------------------------------\r\n");
    SSerial_printf(SerialDebug, "\r\n");

    /*In case of internet connection failure- reconnect*/
    if (calypso->status == calypso_error)
    {
        if (Calypso_reboot(calypso))
        {
            delay(RESPONSE_WAIT_TIME);
        }
        if (!Calypso_WLANconnect(calypso))
        {
            SSerial_printf(SerialDebug, "WiFi connect fail\r\n");
            return;
        }
        if (!Calypso_MQTTconnect(calypso))
        {
            SSerial_printf(SerialDebug, "MQTT connect fail\r\n");
        }
    }

    char *payload = serializeData();

    SSerial_printf(SerialDebug, payload);
    SSerial_printf(SerialDebug, "\r\n");

    /*Publish to MQTT topic*/
    if (!Calypso_MQTTPublishData(calypso, MQTT_TOPIC, 0, payload,
                                 strlen(payload), true))
    {
        SSerial_printf(SerialDebug, "Publish failed\n\r");
        calypso->status = calypso_error;
    }

    /*Clean-up*/
    free(payload);

    delay(5000);
}

/**
 * @brief  Serialize data to send
 * @retval Pointer to serialized data
 */
char *serializeData()
{
    Timestamp timestamp;
    Timer_initTime(&timestamp);

    msgID++;
    if (msgID == INT_MAX)
    {
        msgID = 0;
    }

    // Get the current time from calypso
    if (!Calypso_getTimestamp(calypso, &timestamp))
    {
        SSerial_printf(SerialDebug, "Get time fail\r\n");
    }

    /* convert to unix timestamp */
    unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

    /*Create a JSON object with the device ID, message ID, and time stamp*/
    json_value *payload = json_object_new(1);
    json_object_push(payload, "deviceId", json_string_new(calypso->MAC_ADDR));
    json_object_push(payload, "messageId", json_integer_new(msgID));
    json_object_push(payload, "ts", json_integer_new(unixTime_ms));

    /*Push sensor data into the JSON object*/
    int i;
    for (i = 0; i < padsProperties; i++)
    {
        json_object_push(payload, sensorPADS->dataNames[i],
                         json_double_new(sensorPADS->data[i]));
    }
    for (i = 0; i < itdsProperties; i++)
    {
        json_object_push(payload, sensorITDS->dataNames[i],
                         json_double_new(sensorITDS->data[i]));
    }
    for (i = 0; i < tidsProperties; i++)
    {
        json_object_push(payload, sensorTIDS->dataNames[i],
                         json_double_new(sensorTIDS->data[i]));
    }

    for (i = 0; i < hidsProperties; i++)
    {
        json_object_push(payload, sensorHIDS->dataNames[i],
                         json_double_new(sensorHIDS->data[i]));
    }

    char *buf = (char *)malloc(json_measure(payload));

    json_serialize(buf, payload);

    json_builder_free(payload);

    return buf;
}
