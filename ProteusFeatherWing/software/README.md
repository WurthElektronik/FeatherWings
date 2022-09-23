![WE Logo](../../assets/WE_Logo_small_t.png)

# Proteus-III BLE FeatherWing

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

The quick start example for the Proteus-III BLE FeatherWing demonstrates the very basic functionality of the Proteus-III radio module, that is to connect to a BLE device, transmit and receive data.

**Setup** - The debug as well as the Proteus-III UART interfaces are initialized. Additionally, the basic configuration of the Proteus-III (connection timing, Beacon data and Device name) is done.

```C
void setup() 
{
    delay(3000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialProteusIII = HSerial_create(&ProteusIIIUART);

    // Create serial port for Proteus-III FeatherWing with baud 115200 and 8N1
    HSerial_beginP(SerialProteusIII, 115200, (uint8_t)SERIAL_8N1);
    pinPeripheral(10, PIO_SERCOM);
    pinPeripheral(11, PIO_SERCOM);

    proteusIII = PROTEUSIII_Create(SerialDebug, SerialProteusIII, &callbacks);
    uint8_t timing = -1;
    if (PROTEUSIII_GetConnectionTiming(proteusIII, &timing) && timing != 2) {
        if (PROTEUSIII_SetConnectionTiming(proteusIII, 2)) {
            SSerial_printf(SerialDebug, "timing set \n");
        }
    }
    uint8_t bname[] = {'F', 'W', 'I', 'N', 'G'};
    if (PROTEUSIII_SetBeaconData(proteusIII, bname, 5)) {
        SSerial_printf(SerialDebug, "beacon data set\n");
    }
    if (PROTEUSIII_SetAdvertisingFlags(proteusIII, 1)) {
        SSerial_printf(SerialDebug, "advertising flag set\n");
    }
    uint8_t macaddress[6] = {};
    if (PROTEUSIII_GetBTMacAddress(proteusIII, macaddress)) {
        SSerial_printf(SerialDebug,
                       "mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
                       macaddress[5], macaddress[4], macaddress[3],
                       macaddress[2], macaddress[1], macaddress[0]);
        char newname[10];
        sprintf(newname, "W-%02x%02x%02x", macaddress[2], macaddress[1],
                macaddress[0]);
        SSerial_printf(SerialDebug, "new name %s\n", newname);
        if (PROTEUSIII_SetDeviceName(proteusIII, (uint8_t *)newname)) {
            SSerial_printf(SerialDebug, "new name set\n");
        }
    }
    SSerial_printf(SerialDebug, "current time %lu \n", millis());
    uint8_t serialNr[4] = {0, 1, 2, 3};
    if (PROTEUSIII_Transmit(proteusIII, serialNr, 4)) {
        SSerial_printf(SerialDebug, "data transmitted");
    }
    SSerial_printf(SerialDebug, "after send time %lu \n", millis());

    uint8_t fwversion[3] = {};
    if (PROTEUSIII_GetFWVersion(proteusIII, fwversion)) {
        SSerial_printf(SerialDebug, "read version %d.%d.%d", fwversion[2],
                       fwversion[1], fwversion[0]);
        if ((fwversion[2] != 1) || (fwversion[1] != 4)) {
            SSerial_printf(SerialDebug, "unsupported FW version \n");
            exit(0);
        }
    }
}
```
3. In the main application, the module receives the data transmitted from the peer.
```C
void loop() 
{
    PROTEUSIII_Receive(proteusIII);
    SSerial_printf(SerialDebug, "test \n");
}
```

## Running the example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.


![Running quick start example](../../assets/VSCode.png)
