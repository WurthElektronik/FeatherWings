![WE Logo](../assets/WE_Logo_small_t.png)

# Calypso Wi-Fi FeatherWing

## Introduction

<!--Goals-->

The Würth Elektronik eiSos Calypso Wi-Fi FeatherWing is a development board that offers a secure 2.4 GHz Wi-Fi connectivity solution. It is fully compatible to the [Adafruit](https://www.adafruit.com/) popular [Feather line](https://www.adafruit.com/feather) of development boards and extends the Feathers with Wi-Fi connectivity..

The Calypso Wi-Fi FeatherWing consists of the [Calypso radio module](https://www.we-online.com/catalog/en/CALYPSO) that offers Wi-Fi connectivity based on IEEE 802.11 b/g/n with a fully featured TCP/IP (IPv4 and IPv6) stack.

![Calypso Wi-FI FeatherWing](assets/CalypsoFeatherWing_s.png)


With out-of-the-box support to commonly used network applications like SNTP, HTTP(S), MQTT(S) Calypso offers an easy and secure solution to any IoT application.

It has an AT-style command interface on the standard UART and hence can be connected to any of the Feather micro-controller boards. The [Arduino](https://www.arduino.cc/) (C/C++) drivers and examples made available makes it easy to build a prototype to kick-start the application development.

The Calypso Wi-FI FeatherWing board provides an easy to use platform for learning, experimenting and prototyping [cloud connectivity](lib/examples/../WE_CalypsoWiFiFeatherWing/examples/) applications. It offers easy access to the multiple cloud platforms which then provide the data storage, visualization and other advanced data analyses available in your chosen cloud. For more examples, please see [CalypsoSensorCombo](../CalypsoSensorCombo) 

For more information about the hardware, please go to the [hardware repository](https://github.com/WE-eiSmart/FeatherWings-Hardware) or download [Calypso WiFi FeatherWing user manual](link.to.com\document).
Feel free to check our [YouTube channel](https://www.youtube.com/user/WuerthElektronik/videos) for video tutorials, hands-ons and webinars relating to our products.

## Software

The Calypso Wi-Fi FeatherWing was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

Würth Elektronik eiSos provides a Software Development Kit (SDK) with examples to support all WE FeatherWings. Here are the salient features of the WE FeatherWing SDK. 
* The SDK is **open-source** and well documented.
* It uses popular open-source tool chain including an IDE.
* The examples are written in Arduino styled C/C++ for easy understanding.
* The core components of the SDK are written in pure C to enable easy porting to any microcontroller platform.
* Modular structure of the software stack makes it easy to integrate into any project.

### Necessary Steps

* **Install IDE**: your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [Platform IO](https://platformio.org/) extension.
* **PlatformIO**: is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for the software development including building, debugging, code-upload and many more. PlatformIO works well on all the modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/)
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all the necessary functions to utilize full feature set of the sensors and wireless connectivity modules. More details on the SDK and downloads under [WCS Software](we-online.com/wcs-software)
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from WE.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](code.visualstudio.com/docs)
* Follow the instructions to install [PlatformIO IDE](platformio.org/install/ide?install=vscode) extension.
* Download and install [Mosquitto MQTT broker](https://mosquitto.org/download/). Follow the instructions for the installation and configuration on the [Mosquitto MQTT broker man page](https://mosquitto.org/man/mosquitto-8.html).

> Note: For more information about usage with the cloud check [Calypso cloud examples](lib/WE_CalypsoWiFiFeatherWing/examples).

## Example

The quick start examples in the SDK are written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the [WE FeatherWing SDK](/)
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.
   ![Running quick start example](assets/VSCode.png)


### Quick start example

The quick start example for the Calypso Wi-Fi FeatherWing demonstrates connection to the Moquitto MQTT sever and data transmission.

1. Prerequisites for running this example:
    * A Wi-Fi access point with WPA2 personal security and a known password
    * Mosquitto MQTT broker running in the same network. Make sure to note the IP address and port of the Mosquitto server. 

2. Configuration: The following parameters needs to be set-up before building the code.
    * Wi-Fi access point credentials.
    * MQTT server parameters
    * SNTP server parameters.
```C
// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

/*MQTT settings - Mosquitto server*/
#define MQTT_CLIENT_ID "calypso"
#define MQTT_SERVER_ADDRESS "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_TOPIC "timeStamp"

// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"
```
3. Setup - The debug as well as the Calypso UART interfaces are initialized. The calypso is configured with the given parameters. Further, the module connects to the give Wi-Fi, SNTP server and the Mosquitto MQTT server.

```C
void setup() {
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

    // MQTT Settings - Mosquitto broker(non-secure for demo purposes only)
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags = ATMQTT_CREATE_FLAGS_IP4;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;
    calSettings.mqttSettings.connParams.protocolVersion = ATMQTT_PROTOCOL_v3_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;
    strcpy(calSettings.mqttSettings.userOptions.userName, MQTT_CLIENT_ID);
    strcpy(calSettings.mqttSettings.userOptions.passWord, MQTT_CLIENT_ID);

    // SNTP settings
    strcpy(calSettings.sntpSettings.timezone, SNTP_TIMEZONE);
    strcpy(calSettings.sntpSettings.server, SNTP_SERVER);

    calypso = Calypso_Create(SerialDebug, SerialCalypso, &calSettings);

    // Initialize Calypso
    if (!Calypso_simpleInit(calypso)) {
        SSerial_printf(SerialDebug, "Calypso init failed \r\n");
    }

    // Connect Calypso WiFi FeatherWing to the Wi-Fi access point
    if (!Calypso_WLANconnect(calypso)) {
        SSerial_printf(SerialDebug, "WiFi connect fail\r\n");
        return;
    }

    delay(3000);

    // Set up SNTP client on Calypso
    if (!Calypso_setUpSNTP(calypso)) {
        SSerial_printf(SerialDebug, "SNTP config fail\r\n");
    }

    // Connect to MQTT server
    if (!Calypso_MQTTconnect(calypso)) {
        SSerial_printf(SerialDebug, "MQTT connect fail\r\n");
    }
    msgID = 0;
}
```
4. In the main application, the module gets the current time from the SNTP server, converts it to UNIX timestamp format and publishes this timestamp along with device ID and a packet number to the MQTT server periodically.
```C
void loop() {
    Timestamp timestamp;
    Timer_initTime(&timestamp);

    msgID++;
    if (msgID == INT_MAX) {
        msgID = 0;
    }

    // Get the current time from calypso
    if (!Calypso_getTimestamp(calypso, &timestamp)) {
        SSerial_printf(SerialDebug, "Get time fail\r\n");
    }

    /* convert to unix timestamp */
    unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

    /*Create a JSON object with the device ID, message ID, and time stamp*/
    json_value *payload = json_object_new(1);
    json_object_push(payload, "deviceId", json_string_new(calypso->MAC_ADDR));
    json_object_push(payload, "messageId", json_integer_new(msgID));
    json_object_push(payload, "ts", json_integer_new(unixTime_ms));

    char *buf = (char *)malloc(json_measure(payload));
    json_serialize(buf, payload);

    /*Publish to MQTT topic*/
    if (!Calypso_MQTTPublishData(calypso, MQTT_TOPIC, 0, buf, strlen(buf),
                                 true)) {
        SSerial_printf(SerialDebug, "Publish failed\n\r");
    }

    /*Clean-up*/
    json_builder_free(payload);
    free(buf);

    delay(5000);
}
```
5. It is possible to view the messages on another MQTT client in the same network by subscribing to the same topic.
