![WE Logo](../assets/WE_Logo_small_t.png)

# Example implementation of Custom APIs of Calypso using WE Featherwings

## Introduction

Würth Elektronik eiSos offers different product development boards in the FeatherWing form factor. Access to the Feather eco-system offers unlimited possibilities. One such example is described here with the combination of an [Adafruit Feather M0 Express](https://www.adafruit.com/product/3403) with our [Sensor FeatherWing](../SensorFeatherWing) and [Calypso Wi-Fi FeatherWing](../CalypsoWiFiFeatherWing).

The Calypso Wi-Fi module from the firmware version 2.0 offers a set of RESTful APIs to GET and POST resources available on the host MCU. The so called custom APIs help in building dynamic web pages based on the values provided by the host MCU. These APIs provide a transparent link between the host MCU and the HTTP client.

This example provides a sample implementation for the use of custom APIs. A typical IoT monitoring application involves sensing the environment through sensors, and displaying the same on a web page. In this example, the data from the sensors on the Sensor FeatherWing is read and displayed on the web page of the calypso module using the custom GET API. Additionally, the color of the neo-pixel LED on the M0 feather can be set from the webpage using the custom POST API.

### Installing the tools

- Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
- Follow the instructions to install [PlatformIO IDE](https://platformio.org/platformio-ide) extension.

### Code configuration

1. Prerequisites for running this example:

   - A Wi-Fi access point with WPA2 personal security and a known password and SSID.
   - A device that connects to the same access point and accesses the webpages hosted on the calypso using any browser.

2. Configure the Wi-Fi network to be used.

```C
// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"
```

## Running this example

This example is written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the Sensor FeatherWing and Calypso Wi-Fi FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `SensorDataOnCalypsoWebsite.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../assets/VSCode.png)

5. On boot-up, the Calypso FeatherWing connects automatically to the configured Wi-Fi network.

6. On a device connected to the same Wi-Fi network, open a browser and type in the <IP_address>/custom.html

## Displaying the sensor values

In order to read the current values of the sensors present on the Sensors FeatherWing, type in the following in the "ID" text box and click on "GET button send a "Custom API" GET request. The value is displayed as text below.

| ID           | Value returned                                          |
| ------------ | ------------------------------------------------------- |
| temperature  | Temperature in °C from the WSEN-TIDS temperature sensor |
| humidity     | Humidity in % form the WSEN-HIDS humidity sensor        |
| pressure     | Absolute pressure in kPa from the WSEN-PADS sensor      |
| acceleration | Acceleration in g from the WSEN-ITDS sensor             |

## Change the LED color from web page

With a "Custom API" PUT request, it is possible to send messages to the M0 feather from the web page. In this example, in the "SET" tab of the custom API page, type in "led-color" in the ID field and one of the following values "red", "blue", "green", "off" in the "value1" field to set the corresponding color.
