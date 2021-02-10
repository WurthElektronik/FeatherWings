![WE Logo](../assets/WE_Logo_small_t.png)

# Sensor to cloud connectivity using WE Featherwings

## Introduction

Würth Elektronik eiSos offers different product development boards in the FeatherWing form factor. Access to the Feather eco-system offers unlimited possibilities. One such example is described here with the combination of an [Adafruit Feather M0 Express](https://www.adafruit.com/product/3403) with our [Sensor FeatherWing](../SensorFeatherWing)  and [Calypso Wi-Fi FeatherWing](../CalypsoWiFiFeatherWing).

A typical IoT application involves sensing the environment through sensors, collecting the sensor data and passing it on to the cloud. The cloud platform then offers possibilities to further process and visualize data. In this example, the data from the sensors on the Sensor FeatherWing is read and forwarded to the cloud platform via Calypso Wi-Fi FeatherWing. Currently, the data can be sent to one of the following cloud platforms.

* [Microsoft Azure](azure/)
* [Amazon AWS](aws/)

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
* Follow the instructions to install [PlatformIO IDE](https://platformio.org/platformio-ide) extension.
* Please follow the steps described under [Azure](azure/) or [AWS](aws/) to set-up the device in the corresponding cloud platform.


### Code configuration

The quick start example for the Calypso Wi-Fi FeatherWing demonstrates a connection to the Mosquitto MQTT server and data transmission.

1. Prerequisites for running this example:
    * A Wi-Fi access point with WPA2 personal security and a known password and SSID.
    * For Azure, the IoT-Hub has to be configured as described in the [Azure](azure/) section.
    * For AWS, the IoT Core has to be configured as described in the [AWS](aws/) section.

2. Select the cloud platform of your choice.
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

4. For connection to Azure, the following parameters can be configured

* **MQTT_CLIENT_ID**: Azure IoT Hub **device name**
* **MQTT_SERVER_ADDRESS**: Azure IoT Hub **hostname**. It can be found in the IoT Hub -> Overview
* **MQTT_PORT**: **8883**
* **MQTT_TOPIC**: ```devices/<device name>/messages/events/```
* **MQTT_USER_NAME**: ``` hostname/device name ```
* **MQTT_PASSWORD**: Shared Access Signature (**SAS**). Creation described in the [Azure](azure/).

For example,
```C
#define MQTT_CLIENT_ID "we-iot-device"
#define MQTT_SERVER_ADDRESS "we-exampleHub.azure-devices.net"
#define MQTT_PORT 8883
#define MQTT_TOPIC "devices/we-iot-device/messages/events/"
#define MQTT_USER_NAME "example_for_documentation.azure-devices.net/we-iot-device"
#define MQTT_PASSWORD                                               \
    "SharedAccessSignature "                                        \
    "sr=we-iotHub.azure-devices.net%2Fdevices%2Fwe-iot-device&sig=" \
    "example_for_documentation=1111111111"
```

5. For connection to AWS, the following parameters can be configured

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
#define MQTT_SERVER_ADDRESS "example_for_documentation.iot.amazonaws.com"
#define MQTT_PORT 8883
#define MQTT_TOPIC "test"
#define MQTT_CERT_PATH "cert"
#define MQTT_PRIV_KEY_PATH "key"
#define MQTT_USER_NAME "calypso"
#define MQTT_PASSWORD "secret"

#define MQTT_CERTIFICATE "-----BEGIN CERTIFICATE-----\n\
1111111111111111111111111111111111111111111111111111111111111111\n\
example_for_documentation\n\

-----END CERTIFICATE-----"

#define MQTT_PRIV_KEY "-----BEGIN RSA PRIVATE KEY-----\n\
1111111111111111111111111111111111111111111111111111111111111111\n\
example_for_documentation\n\
-----END RSA PRIVATE KEY-----"
#endif
```

6. SNTP server parameters.
```C
// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"
```

## Running this example

The cloud connectivity example is written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the Sensor FeatherWing and Calypso Wi-Fi FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `Sensor2CloudConnectivity.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../assets/VSCode.png)
