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

2. Configuration: The following parameters need to be set-up before building the code.
    * HIDS sensor part number.
    > **_NOTE:_** This depends on the HW version of the Sensor FeatherWing if its version 3.0 or more then the HIDS_PART_NUMBER is 2525020210002 otherwise its 2525020210001.
```C
#define HIDS_PART_NUMBER 2525020210002
```

3. Setup - The sensors are initialized after setting up the I<sup>2</sup>C interface. The debug UART interface is initialized by calling the init function. A communication check is performed by reading out the ID of each of the sensors.

```C
void setup() {
    delay(2000);

    // Using the USB serial port for debug messages
#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    if (!sensorBoard_Init()) {
        WE_DEBUG_PRINT("I2C init failed \r\n");
    }
    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit()) {
        WE_DEBUG_PRINT("PADS init failed \r\n");
    }

    if (!ITDS_2533020201601_simpleInit()) {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
    }

    if (!TIDS_2521020222501_simpleInit()) {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit()) {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit()) {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#endif
}
```
4. In the main application, a single sensor data measurement is triggered, data read, processed and displayed on the debug interface periodically.
```C
void loop() {
    WE_DEBUG_PRINT("----------------------------------------------------\r\n");
    // Read and print sensor values
    if (PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp)) {
        WE_DEBUG_PRINT("WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
                       PADS_pressure, PADS_temp);
    }
    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, &ITDS_temp)) {
        WE_DEBUG_PRINT(
            "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g Temp: "
            "%f °C \r\n",
            ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp);
    }
    if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent)) {
        WE_DEBUG_PRINT("WSEN_ITDS(DoubleTap): State %s \r\n",
                       ITDS_doubleTapEvent ? "True" : "False");
    }
    if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent)) {
        WE_DEBUG_PRINT("WSEN_ITDS(FreeFall): State %s \r\n",
                       ITDS_freeFallEvent ? "True" : "False");
    }
    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_TIDS(Temperature): %f °C\r\n", TIDS_temp);
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                       HIDS_temp);
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                       HIDS_temp);
    }
#endif

    WE_DEBUG_PRINT("----------------------------------------------------\r\n");
    WE_DEBUG_PRINT("\r\n");
    delay(1000);
}
```
### Running the example

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio code.
3. **Build** and **Upload** the code from the PlatformIO tab as shown in the Figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.

![Running quick start example](../../assets/VSCode.png)
