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
Please use the [Proteus-Connect app](https://we-online.com/wcs-software) for the platform of your choice(Android, iOS, Webapp) on the smart device to test this application.

**Setup** - The debug as well as the Proteus-III UART interfaces are initialized.
```C
void setup() {
    delay(3000);
    // Using the USB serial port for debug messages
#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    ProteusIII_pins.ProteusIII_Pin_SleepWakeUp.pin = 9;
    ProteusIII_pins.ProteusIII_Pin_Mode.pin = 17;

    ProteusIII_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;
    callbackConfig.beaconRxCb = BeaconRxCallback;
    callbackConfig.connectCb = ConnectCallback;
    callbackConfig.disconnectCb = DisconnectCallback;
    callbackConfig.channelOpenCb = ChannelOpenCallback;
    callbackConfig.phyUpdateCb = PhyUpdateCallback;
    callbackConfig.sleepCb = SleepCallback;
    callbackConfig.rssiCb = RssiCallback;
    callbackConfig.gpioWriteCb = GpioWriteCallback;
    callbackConfig.gpioRemoteConfigCb = GpioRemoteConfigCallback;
    callbackConfig.errorCb = ErrorCallback;

    if (!ProteusIII_Init(&ProteusIII_pins, PROTEUSIII_DEFAULT_BAUDRATE,
                         WE_FlowControl_NoFlowControl,
                         ProteusIII_OperationMode_CommandMode,
                         callbackConfig)) {
        WE_DEBUG_PRINT("Proteus init failed \r\n");
    }
}
```
3. In the main application, nothing is done as all the callbacks (device connected, data received, ...) have been registered during setup.
```C
void loop() {}
```

## Running the example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.


![Running quick start example](../../assets/VSCode.png)
