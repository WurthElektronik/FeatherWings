![WE Logo](../../assets/WE_Logo_small_t.png)

# Thyone Wireless FeatherWing

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

The quick start example for the Thyone Wireless FeatherWing demonstrates the very basic functionality of the Thyone-I radio module, that is to transmit and receive data.

1. In the quick start application, the Thyone-I can be configured either as a transmitter or a receiver.
```C
/*Configure the FeatherWing either as a transmitter or a receiver*/
#define Transmitter 1
```

2. Setup - The debug as well as the Thyone-I UART interfaces are initialized. Additionally, the basic configuration of the Thyone-I (radio channel, radio profile and transmit power) is done.

```C
void setup() {
    delay(3000);
    uint8_t serialNrThy[4] = {0};

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    ThyoneI_pins.ThyoneI_Pin_SleepWakeUp.pin = 9;
    ThyoneI_pins.ThyoneI_Pin_Mode.pin = 17;

    if (!ThyoneI_Init(&ThyoneI_pins, THYONEI_DEFAULT_BAUDRATE,
                      WE_FlowControl_NoFlowControl,
                      ThyoneI_OperationMode_CommandMode,
#if Transmitter == 1
                      NULL
#else
                      RxCallback
#endif
                      )) {
        WE_DEBUG_PRINT("Thyone init failed \r\n");
    }

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_DEBUG_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
                       serialNrThy[3], serialNrThy[2], serialNrThy[1],
                       serialNrThy[0]);
    }

    /*The following code shows how to set the parameters of the Thyone radio
    module. In this case, the values are set to factory default. The settings
    are non-volatile. Each write operation performs a write to the flash memory
    and resets the module. Hence, this operation has to be limited to one time
    configuration only.*/
    ThyoneI_TXPower_t txPower;
    if (ThyoneI_GetTXPower(&txPower)) {
        if (txPower != ThyoneI_TXPower_8) {
            // Set the transmit power to 8 dBm
            if (!ThyoneI_SetTXPower(ThyoneI_TXPower_8)) {
                WE_DEBUG_PRINT("Thyone set power failed \r\n");
            }
        }
    } else {
        WE_DEBUG_PRINT("Thyone get power failed \r\n");
    }

    uint8_t rfChannel;
    if (ThyoneI_GetRFChannel(&rfChannel)) {
        if (rfChannel != THYONE_DEFAULT_RF_CHANNEL) {
            // Set the RF channel channel 21
            if (!ThyoneI_SetRFChannel(THYONE_DEFAULT_RF_CHANNEL)) {
                WE_DEBUG_PRINT("Thyone set channel failed \r\n");
            }
        }
    } else {
        WE_DEBUG_PRINT("Thyone get power failed \r\n");
    }
    ThyoneI_Profile_t rfProfile;
    if (ThyoneI_GetRfProfile(&rfProfile)) {
        if (rfProfile != THYONE_DEFAULT_RF_PROFILE) {
            // Set the RF profile to long range 125 kbit/s mode
            if (!ThyoneI_SetRfProfile(THYONE_DEFAULT_RF_PROFILE)) {
                WE_DEBUG_PRINT("Thyone set RF profile failed \r\n");
            }
        }
    } else {
        WE_DEBUG_PRINT("Thyone get power failed \r\n");
    }
}
```
3. In the main application, if the Transmitter flag is 1 then the module either transmits a hello message periodically. Otherwise, the main loop does nothing and the callback function set in the init will be called whenever data is received.
```C
void loop() {
#if Transmitter == 1
    sprintf(sendBuffer, "Hello World!\r\n");
    WE_DEBUG_PRINT(sendBuffer);

    /*Broadcast Hello message to all the peers in the network*/
    if (ThyoneI_TransmitBroadcast((unsigned char *)sendBuffer,
                                  strlen(sendBuffer))) {
    } else {
        WE_DEBUG_PRINT("Broadcast send failed \r\n");
    }
    delay(1000);
#endif
}

#if Transmitter == 0
/* callback for data reception */
static void RxCallback(uint8_t *payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi) {
    int i = 0;
    WE_DEBUG_PRINT("Received data from address 0x%02lx with %d dBm:\n-> ",
                   sourceAddress, rssi);
    WE_DEBUG_PRINT("0x ");
    for (i = 0; i < payload_length; i++) {
        WE_DEBUG_PRINT("%02x ", *(payload + i));
    }
    WE_DEBUG_PRINT("\n-> ");
    for (i = 0; i < payload_length; i++) {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT("\n");
}
#endif
```

## Running the example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.


![Running quick start example](../../assets/VSCode.png)
