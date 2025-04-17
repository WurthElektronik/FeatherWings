![WE Logo](../../assets/WE_Logo_small_t.png)

# Thyone Wireless Sensors

## Introduction

Würth Elektronik eiSos provides a software development kit (SDK) with examples to support all WE FeatherWings. Here are the salient features of the WE FeatherWing SDK.

* The SDK is open-source and well documented.
* It uses popular open-source tool chain including an IDE.
* The examples are written in Arduino-styled C/C++ for quick prototyping.
* The core components of the SDK are written in pure C to enable easy porting to any microcontroller platform.
* Development platform independent (Windows, Linux or MAC).
* Modular structure of the software stack makes it easy to integrate into any project.


### Software components

* **Install IDE**: your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [Platform IO](https://platformio.org/) extension).
* **PlatformIO**: is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for the software development including building, debugging, code-upload and many more. PlatformIO works well on all the modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/).
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all the necessary functions to utilize full feature set of the sensors and wireless connectivity modules. More details on the SDK and dowloads under [WCS Software](https://we-online.com/wcs-software).
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from WE.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
* Follow the instructions to install the [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) extension.

### Quick start example

In this example, the sensor Featherwing is stacked with the Setebos-I Wireless Featherwing and the Feather M0 express. The M0 feather reads the sensor data and transmits it wirelessly over Thyone-I Proprietary radio link.

>[!NOTE]
>For using the Setebos-I FeatherWing in Thyone-I mode, the solder bridge
jumper SJ4 shall be shorted.

1. Configuration: The following parameters need to be set-up before building the code.
    * HIDS sensor part number.
    > **_NOTE:_** This depends on the HW version of the Sensor FeatherWing if its version 3.0 or more then the HIDS_PART_NUMBER is 2525020210002 otherwise its 2525020210001.
```C
#define HIDS_PART_NUMBER 2525020210002
```
2. Setup - First, the Setebos-I FeatherWing is configured to operate in Thyone-I mode by setting the mode pin to high. The debug as well as the Thyone-I UART interfaces are initialized. The sensors are initialized after setting up the I<sup>2</sup>C interface. A communication check is performed by reading out the ID of each of the sensors.


```C
void setup() {
    delay(5000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    ThyoneI_pins.ThyoneI_Pin_SleepWakeUp.pin = 9;
    ThyoneI_pins.ThyoneI_Pin_Mode.pin = 17;

    /*Set the mode pin on Setebos to Thyone-I mode*/
    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, HIGH);

    if (!ThyoneI_Init(&ThyoneI_pins, THYONEI_DEFAULT_BAUDRATE,
                      WE_FlowControl_NoFlowControl,
                      ThyoneI_OperationMode_CommandMode, NULL)) {
        WE_DEBUG_PRINT("Thyone init failed \r\n");
    }

    uint8_t serialNrThy[4] = {0};

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_DEBUG_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
                       serialNrThy[3], serialNrThy[2], serialNrThy[1],
                       serialNrThy[0]);
    }

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
3. In the main application, the module transmits a message with the values read from the sensors every 5 seconds.
```C
void loop() {
    sprintf(sendBuffer, "Hello world!\r\n");
    // Read and print sensor values
    if (PADS_2511020213301_readSensorData(&PADS_pressure, NULL)) {
        sprintf(temp, "Pre: %f kPa\r\n", PADS_pressure);
        strcat(sendBuffer, temp);
    }
    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, NULL)) {
        sprintf(temp, "Acc: X:%f g Y:%f g  Z:%f g\r\n", ITDS_accelX,
                ITDS_accelY, ITDS_accelZ);
        strcat(sendBuffer, temp);
    }
    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        sprintf(temp, "Temp: %f C\r\n", TIDS_temp);
        strcat(sendBuffer, temp);
    }
    if (
#if HIDS_PART_NUMBER == 2525020210001
        HIDS_2525020210001_readSensorData(&HIDS_humidity, NULL)
#elif HIDS_PART_NUMBER == 2525020210002
        HIDS_2525020210002_readSensorData(&HIDS_humidity, NULL)
#else
        false
#endif
    ) {
        sprintf(temp, "RH: %f %%\r\n", HIDS_humidity);
        strcat(sendBuffer, temp);
    }
    WE_DEBUG_PRINT(sendBuffer);

    /*Broadcast Hello message to all the peers in the network*/
    if (ThyoneI_TransmitBroadcast((unsigned char *)sendBuffer,
                                  strlen(sendBuffer))) {
    } else {
        WE_DEBUG_PRINT("Broadcast send failed \r\n");
    }
    delay(5000);
}
```

## Running the example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.


![Running quick start example](../../assets/VSCode.png)
