![WE Logo](../assets/WE_Logo_small_t.png)

# Daphnis-I FeatherWing in P2P mode

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

The quick start example for the Daphnis-I FeatherWing demonstrates the very basic functionality of the Daphnis-I radio module, that is to transmit and receive data using the proprietary P2P mode.

1. In the quick start application, the Daphnis-I can be configured either as a transmitter or a receiver.
```C
/*Configure the FeatherWing either as a transmitter or a receiver*/
#define MODULE_ROLE_TX 0
#define MODULE_ROLE_RX 1
```

2. The setup function, the Daphnis-I module is set up in the P2P mode by setting the "usersetting" parameters if not already set.

```C
void setup() {
    delay(5000);

#if ((WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off) || \
     defined(WE_APP_PRINT_ENABLED))
    WE_Print_Init();
#endif

    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, EVENT_CALLBACK)) {
        WE_APP_PRINT("Initialization Failed.\r\n");
        exit(0);
    }

    DaphnisI_Mode_t daphnisI_mode;

    if (!DaphnisI_GetModeRS(&daphnisI_mode)) {
        WE_APP_PRINT("Failed to get current mode.\r\n");
        exit(0);
    }

    if (daphnisI_mode != DaphnisI_Mode_P2P) {
        if (!DaphnisI_SetModeUS(DaphnisI_Mode_P2P)) {
            WE_APP_PRINT("Failed to set mode.\r\n");
            exit(0);
        }

        if (!DaphnisI_PinReset()) {
            WE_APP_PRINT("Failed to reset module.\r\n");
            exit(0);
        }
    }

    bool setting_changed = false;

    DaphnisI_P2P_Role_t daphnisI_role;

    if (!DaphnisI_GetP2PRoleRS(&daphnisI_role)) {
        WE_APP_PRINT("Failed to get current role.\r\n");
        exit(0);
    }

    if (daphnisI_role != DaphnisI_P2P_Role_Transceiver) {
        if (!DaphnisI_SetP2PRoleUS(DaphnisI_P2P_Role_Transceiver)) {
            WE_APP_PRINT("Failed to set role.\r\n");
            exit(0);
        }

        setting_changed = true;
    }

    uint8_t daphnisI_rf_profile;

    if (!DaphnisI_GetP2PRFProfileRS(&daphnisI_rf_profile)) {
        WE_APP_PRINT("Failed to get current profile.\r\n");
        exit(0);
    }

    if (daphnisI_rf_profile != DAPHNISI_DEFAULT_RF_PROFILE) {
        if (!DaphnisI_SetP2PRFProfileUS(DAPHNISI_DEFAULT_RF_PROFILE)) {
            WE_APP_PRINT("Failed to set profile.\r\n");
            exit(0);
        }

        setting_changed = true;
    }

    uint8_t daphnisI_rf_channel;

    if (!DaphnisI_GetP2PRFChannelRS(&daphnisI_rf_channel)) {
        WE_APP_PRINT("Failed to get current channel.\r\n");
        exit(0);
    }

    if (daphnisI_rf_channel != DAPHNISI_DEFAULT_RF_CHANNEL) {
        if (!DaphnisI_SetP2PRFChannelUS(DAPHNISI_DEFAULT_RF_PROFILE)) {
            WE_APP_PRINT("Failed to set channel.\r\n");
            exit(0);
        }

        setting_changed = true;
    }

    if (setting_changed) {
        if (!DaphnisI_PinReset()) {
            WE_APP_PRINT("Failed to reset module.\r\n");
            exit(0);
        }
    }

#if MODULE_ROLE == MODULE_ROLE_RX
    if (!DaphnisI_P2P_SetRXEnabled(true)) {
        WE_APP_PRINT("Failed to enable receiving.\r\n");
        exit(0);
    }
#endif

    WE_APP_PRINT("Daphnis-I Initialization Succeeded.\r\n");

    DaphnisI_Device_Address_t source_address;

    if (!DaphnisI_GetP2PMACSourceAddressRS(&source_address)) {
        WE_APP_PRINT("Failed to get source address.\r\n");
        exit(0);
    }

    WE_APP_PRINT("Daphnis-I Source Address: ");
    DaphnisI_Print_Key_Addr(source_address, DAPHNISI_ADDRESS_LENGTH);
    WE_APP_PRINT("\r\n");
}
```
3. In the main application, if the Transmitter flag is 1 then the module either transmits "hello world" message periodically. Otherwise, the main loop does nothing and the callback function set in the init will be called whenever data is received.
```C
void loop() {
#if MODULE_ROLE == MODULE_ROLE_TX
    uint8_t payload[224] = "Hello World!";
    uint16_t payload_length = strlen((const char*)payload);

    if (!DaphnisI_P2P_TransmitBroadcast(payload, payload_length)) {
        WE_APP_PRINT("Failed to send packet.\r\n");
        exit(0);
    }

    WE_APP_PRINT("Sent Payload: \r\n-> ");
    DaphnisI_Print_Payload(payload, payload_length);

    WE_Delay(5000);
#elif MODULE_ROLE == MODULE_ROLE_RX
    switch (current_state) {
        case DaphnisI_P2P_SM_DataReceived: {
            WE_APP_PRINT("Received data from address ");
            DaphnisI_Print_Key_Addr(rxData.source_address,
                                    DAPHNISI_ADDRESS_LENGTH);
            WE_APP_PRINT(" with %d dBm: ", rxData.RSSI);
            DaphnisI_Print_Payload(rxData.data, rxData.data_length);
            current_state = DaphnisI_P2P_SM_WaitingForData;
            break;
        }
        default:
            break;
    }
#endif
}
```

## Running the example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.


![Running quick start example](../../assets/VSCode.png)
