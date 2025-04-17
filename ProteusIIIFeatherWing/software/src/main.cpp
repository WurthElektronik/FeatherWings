/**
 * \file
 * \brief Main file for the WE-ProteusWirelessFeatherWing.
 *
 * \copyright (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */
#include "debug.h"
#include "ProteusIII.h"

ProteusIII_Pins_t ProteusIII_pins;

#define SETEBOS_MODE_PIN 14

/* Is set to true if the channel has been opened */
bool cmd_channelOpen = false;

/* Callback functions for various indications sent by the Proteus-III. */
static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
                       int8_t rssi);
static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength,
                             uint8_t *btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t *btMac);
static void DisconnectCallback(ProteusIII_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t *btMac, uint8_t phyRx,
                              uint8_t phyTx);
static void SleepCallback();
static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower);
static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value);
static void GpioRemoteConfigCallback(ProteusIII_GPIOConfigBlock_t *gpioConfig);
static void ErrorCallback(uint8_t errorCode);

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

    /*Set the mode pin on Setebos to Proteus-III mode*/
    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, LOW);

    if (!ProteusIII_Init(&ProteusIII_pins, PROTEUSIII_DEFAULT_BAUDRATE,
                         WE_FlowControl_NoFlowControl,
                         ProteusIII_OperationMode_CommandMode,
                         callbackConfig)) {
        WE_DEBUG_PRINT("Proteus init failed \r\n");
    }
}

void loop() {}

static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
                       int8_t rssi) {
    int i = 0;
    WE_DEBUG_PRINT(
        "Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) "
        "with RSSI = %d dBm:\r\n-> ",
        btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
    WE_DEBUG_PRINT("0x ");
    for (i = 0; i < payloadLength; i++) {
        WE_DEBUG_PRINT("%02x ", *(payload + i));
    }
    WE_DEBUG_PRINT("\n-> ");
    for (i = 0; i < payloadLength; i++) {
        WE_DEBUG_PRINT("%c", *(payload + i))
    }
    WE_DEBUG_PRINT("\r\n");
}

static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength,
                             uint8_t *btMac, int8_t rssi) {
    int i = 0;
    WE_DEBUG_PRINT(
        "Received beacon data from device with BTMAC "
        "(0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ",
        btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
    WE_DEBUG_PRINT("0x ");
    for (i = 0; i < payloadLength; i++) {
        WE_DEBUG_PRINT("%02x ", *(payload + i));
    }
    WE_DEBUG_PRINT("\n-> ");
    for (i = 0; i < payloadLength; i++) {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT("\r\n");
}

static void ConnectCallback(bool success, uint8_t *btMac) {
    cmd_channelOpen = false;
    WE_DEBUG_PRINT("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ",
                   success ? "Connected" : "Failed to connect", btMac[0],
                   btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_DEBUG_PRINT("\r\n");
}

static void DisconnectCallback(ProteusIII_DisconnectReason_t reason) {
    cmd_channelOpen = false;

    static const char *reasonStrings[] = {"unknown",
                                          "connection timeout",
                                          "user terminated connection",
                                          "host terminated connection",
                                          "connection interval unacceptable",
                                          "MIC failure",
                                          "connection setup failed"};

    WE_DEBUG_PRINT("Disconnected (reason: %s)\n", reasonStrings[reason]);
}

static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload) {
    cmd_channelOpen = true;

    WE_DEBUG_PRINT(
        "Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum "
        "payload = %d",
        btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
    WE_DEBUG_PRINT("\r\n");
}

static void PhyUpdateCallback(bool success, uint8_t *btMac, uint8_t phyRx,
                              uint8_t phyTx) {
    if (success) {
        WE_DEBUG_PRINT(
            "Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated "
            "(RX: %dMBit, TX: %dMBit)",
            btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], phyRx,
            phyTx);
    } else {
        WE_DEBUG_PRINT("Failed to update Phy connection");
    }
    WE_DEBUG_PRINT("\r\n");
}

static void SleepCallback() {
    WE_DEBUG_PRINT("Advertising timeout detected, will go to sleep now\n");
}

static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower) {
    WE_DEBUG_PRINT(
        "Received RSSI indication from device with BTMAC "
        "(0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm and TX power = %d "
        "dBm.\n",
        btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi,
        txPower);
}

static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value) {
    WE_DEBUG_PRINT(
        "GPIO write indication received (remote: %s, GPIO: %u, value: %u)\n",
        remote ? "true" : "false", gpioId, value);
}

static void GpioRemoteConfigCallback(ProteusIII_GPIOConfigBlock_t *gpioConfig) {
    static const char *functionStrings[] = {"disconnected", "input", "output",
                                            "PWM"};

    static const char *pullStrings[] = {"no pull", "pull down", "pull up"};

    WE_DEBUG_PRINT(
        "GPIO remote config indication received (GPIO: %u, function: %s",
        gpioConfig->gpioId, functionStrings[gpioConfig->function]);

    switch (gpioConfig->function) {
        case ProteusIII_GPIO_IO_Disconnected:
            break;

        case ProteusIII_GPIO_IO_Input:
            WE_DEBUG_PRINT(", input type: %s",
                           pullStrings[gpioConfig->value.input]);
            break;

        case ProteusIII_GPIO_IO_Output:
            WE_DEBUG_PRINT(
                ", output level: %s",
                gpioConfig->value.output == ProteusIII_GPIO_Output_High
                    ? "HIGH"
                    : "LOW");
            break;

        case ProteusIII_GPIO_IO_PWM:
            WE_DEBUG_PRINT(", PWM period: %u ms, ratio: %u%%",
                           gpioConfig->value.pwm.period,
                           gpioConfig->value.pwm.ratio);
            break;
    }
    WE_DEBUG_PRINT(")\n");
}

static void ErrorCallback(uint8_t errorCode) {
    WE_DEBUG_PRINT("Error %u\n", errorCode);
}
