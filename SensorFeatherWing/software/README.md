![WE Logo](../../assets/WE_Logo_small_t.png)

# Sensor FeatherWing

## Introduction

The [Sensor FeatherWing](https://www.we-online.de/katalog/en/SENSOR_FeatherWing) was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

Würth Elektronik eiSos provides a Software Development Kit (SDK) with examples to support all WE FeatherWings. Here are the salient features of the WE FeatherWing SDK. 
* The SDK is **open-source** and well documented.
* It uses popular open-source tool chain including an IDE.
* The examples are written in Arduino styled C/C++ for easy understanding.
* The core components of the SDK are written in pure C to enable easy porting to any microcontroller platform.
* Modular structure of the software stack makes it easy to integrate into any project.

### Software components

* **Install IDE**: Use your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/) extension).
* **PlatformIO**: This is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for software development including building, debugging, code-upload and many more. PlatformIO works well on all modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/).
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all necessary functions to utilize a full feature set of the sensors and wireless connectivity modules. More details on the SDK and dowloads under [WCS Software](https://we-online.com/wcs-software).
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from Würth Elektronik eiSos.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
* Follow the instructions under [platformio.org](https://platformio.org/install/ide?install=vscode) to install the IDE extension.


### Quick start example

The quick start example for the Sensor FeatherWing demonstrates configuring the sensors on-board and reading out the sensor data.

1. In the quick start application, all the sensors are configured to operate in simplest possible mode, the single conversion mode. In this mode the sensors are triggered to perform a single measurement and the measured value is read out. 

2. Setup - The sensors are initialized after setting up the I<sup>2</sup>C interface. The debug UART interface is initialized. A communication check is performed by reading out the ID of each of the sensors.

```C
void setup() {
    delay(2000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    // Create sensor objects
    sensorPADS = PADSCreate(SerialDebug);
    sensorITDS = ITDSCreate(SerialDebug);
    sensorTIDS = TIDSCreate(SerialDebug);
    sensorHIDS = HIDSCreate(SerialDebug);

    // Initialize the sensors in default mode
    if (!PADS_simpleInit(sensorPADS)) {
        SSerial_printf(SerialDebug, "PADS init failed \r\n");
    }

    if (!ITDS_simpleInit(sensorITDS)) {
        SSerial_printf(SerialDebug, "ITDS init failed \r\n");
    }
    if (!TIDS_simpleInit(sensorTIDS)) {
        SSerial_printf(SerialDebug, "TIDS init failed \r\n");
    }
    if (!HIDS_simpleInit(sensorHIDS)) {
        SSerial_printf(SerialDebug, "HIDS init failed \r\n");
    }
}
```
3. In the main application, a single sensor data measurement is triggered, data read, processed and displayed on the debug interface periodically.
```C
void loop() {
    SSerial_printf(SerialDebug,
                   "----------------------------------------------------\r\n");
    // Read and print sensor values
    if (PADS_readSensorData(sensorPADS)) {
        SSerial_printf(
            SerialDebug, "WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
            sensorPADS->data[padsPressure], sensorPADS->data[padsTemperature]);
    }
    if (ITDS_readSensorData(sensorITDS)) {
        SSerial_printf(SerialDebug,
                       "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g\r\n",
                       sensorITDS->data[itdsXAcceleration],
                       sensorITDS->data[itdsYAcceleration],
                       sensorITDS->data[itdsZAcceleration]);
    }
    if (TIDS_readSensorData(sensorTIDS)) {
        SSerial_printf(SerialDebug, "WSEN_TIDS(Temperature): %f °C\r\n",
                       sensorTIDS->data[tidsTemperature]);
    }
    if (HIDS_readSensorData(sensorHIDS)) {
        SSerial_printf(SerialDebug, "WSEN_HIDS: RH: %f %% Temp: %f °C\r\n",
                       sensorHIDS->data[hidsRelHumidity],
                       sensorHIDS->data[hidsTemperature]);
    }
    SSerial_printf(SerialDebug,
                   "----------------------------------------------------\r\n");
    SSerial_printf(SerialDebug, "\r\n");
    delay(1000);
}
```
### Running the example

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio code.
3. **Build** and **Upload** the code from the PlatformIO tab as shown in the Figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.

![Running quick start example](../../assets/VSCode.png)
