![WE Logo](../assets/WE_Logo_small_t.png)

# Sensor to cloud connectivity using WE Featherwings

## Introduction

Würth Elektronik eiSos offers different product development boards in the FeatherWing form factor. Access to the Feather eco-system offers unlimited possibilities. One such example is described here with the combination of [Adafruit Feather M0 Express](https://www.adafruit.com/product/3403) with the [Sensor FeatherWing](../SensorFeatherWing)  and the [Calypso FeatherWing](../CalypsoWiFiFeatherWing).

A typical IoT application involves sensing the environment through sensors, collecting the sensor data and passing it on to the cloud. The cloud platform then offers possibilities to further process and visualize data. In this example, the data from the sensors on the Sensor FeatherWing is read and forwarded to the cloud platform via Calypso FeatherWing. Currently, the data can be sent to one of the following cloud platforms.

* [**Microsoft Azure**](azure/)
* [**Amazon AWS**](aws/)

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](code.visualstudio.com/docs)
* Follow the instructions to install [PlatformIO IDE](platformio.org/install/ide?install=vscode) extension.
* Please follow the steps described under [Azure](azure/) or [AWS](aws/) to set-up the device in the corresponding cloud platform.


### Code configuration

The quick start example for the Calypso Wi-Fi FeatherWing demonstrates connection to the Moquitto MQTT sever and data transmission.

1. Prerequisites for running this example:
    * A Wi-Fi access point with WPA2 personal security and a known password
    * For Azure, the IoT-Hub has to be configured as described in the [Azure](azure/) section.
    * For AWS, the IoT Core has to be configured as described in the [AWS](aws/).

2. Select the cloud platform of choice.
```C
#define AZURE_CONNECTION 1
#define AWS_CONNECTION 0
```
3. Configure the Wi-Fi network to be used.
```C
// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"
```

4. For Connection to Azure, the following parameters be configured

* **MQTT_CLIENT_ID**: Azure IoT Hub **device name**
* **MQTT_SERVER_ADDRESS**: Azure IoT Hub **hostname**. It can be found in the IoT Hub -> Overview
* **MQTT_PORT**: **8883**
* **MQTT_TOPIC**: ```devices/<device name>/messages/events/```
* **MQTT_USER_NAME**: ``` hostname/device name ```
* **MQTT_PASSWORD**: Shared Access Signature (**SAS**)

For example,
```C
#define MQTT_CLIENT_ID "we-iot-device"
#define MQTT_SERVER_ADDRESS "we-iotHub.azure-devices.net"
#define MQTT_PORT 8883
#define MQTT_TOPIC "devices/we-iot-device/messages/events/"
#define MQTT_USER_NAME "we-iotHub.azure-devices.net/we-iot-device"
#define MQTT_PASSWORD                                               \
    "SharedAccessSignature "                                        \
    "sr=we-iotHub.azure-devices.net%2Fdevices%2Fwe-iot-device&sig=" \
    "DcBBSucFMS15NAN6wLCfMiZtpBb8fgYrUJeq%2BBvbnw%3D&se=1640500745"
```

4. For Connection to AWS, the following parameters be configured

* **MQTT_CLIENT_ID**: AWS IoT core **thing name**
* **MQTT_SERVER_ADDRESS**: AWS IoT core **hostname**. It can be found in the IoT core -> Interact -> HTTPS
* **MQTT_PORT**: **8883**
* **MQTT_TOPIC**: ```test```
* **MQTT_CERTIFICATE**: Download described in the [AWS](aws/).
* **MQTT_PRIVATE_KEY**: Download described in the [AWS](aws/).

For example,
```C
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
```

5. SNTP server parameters.
```C
// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"
```

## Running this example

The cloud connectivity example  written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the Sensor FeatherWing and Calypso FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `Sensor2CloudConnectivity.code-workspace` in Visual Studio code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../assets/VSCode.png)
