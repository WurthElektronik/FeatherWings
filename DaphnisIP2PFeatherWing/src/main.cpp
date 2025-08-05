/**
 * \file
 * \brief Main file for the WE-DaphnisFeatherWing.
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

#include <ConfigPlatform.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <DaphnisI/DaphnisI.h>
#include <we_print.h>

#include <DaphnisI/ATCommands/P2P.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>

#define MODULE_ROLE_TX 0
#define MODULE_ROLE_RX 1

#define MODULE_ROLE MODULE_ROLE_TX

#define DAPHNISI_DEFAULT_RF_CHANNEL (uint8_t)64
#define DAPHNISI_DEFAULT_RF_PROFILE (uint8_t)5

static const WE_Arduino_Pin_t pin_Reset = WE_ARDUINO_PIN(18);

static DaphnisI_Pins_t DaphnisI_pins = {
    .DaphnisI_Pin_Reset = WE_PIN((void*)&pin_Reset),
    .DaphnisI_Pin_WakeUp = WE_PIN_UNDEFINED(),
    .DaphnisI_Pin_Boot = WE_PIN_UNDEFINED(),
};

static WE_UART_t DaphnisI_uart = {
    .uartInit = WE_UART_RXPin0_TXPin1_Init,
    .uartDeinit = WE_UART_RXPin0_TXPin1_DeInit,
    .uartTransmit = WE_UART_RXPin0_TXPin1_Transmit,
    .baudrate = DAPHNISI_DEFAULT_BAUDRATE,
    .flowControl = WE_FlowControl_NoFlowControl,
    .parity = WE_Parity_None,
};

#if MODULE_ROLE == MODULE_ROLE_RX

static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event,
                                       char* eventText);

#define EVENT_CALLBACK DaphnisI_P2P_EventCallback

static uint8_t rxDataBuffer[DAPHNISI_P2P_MAX_PAYLOAD_SIZE];
static DaphnisI_P2P_RxData_t rxData = {.data = rxDataBuffer};

/*
 * State machine for Daphnis-I P2P RX example.
 * */
typedef enum {
    DaphnisI_P2P_SM_WaitingForData,
    DaphnisI_P2P_SM_DataReceived
} DaphnisI_P2P_SM_t;

static volatile DaphnisI_P2P_SM_t current_state =
    DaphnisI_P2P_SM_WaitingForData;

#endif

#ifndef EVENT_CALLBACK
#define EVENT_CALLBACK NULL
#endif

void DaphnisI_Print_Key_Addr(uint8_t* key_addr, uint8_t key_addr_length) {
    if (key_addr == NULL) {
        return;
    }

    for (uint8_t i = key_addr_length - 1; i > 0; i--) {
        WE_APP_PRINT("%02X:", key_addr[i]);
    }
    WE_APP_PRINT("%02X", key_addr[0]);
}

void DaphnisI_Print_Payload(uint8_t* payload, uint8_t payload_length) {
    if (payload == NULL) {
        return;
    }

    WE_APP_PRINT("0x");
    for (uint8_t i = 0; i < payload_length; i++) {
        WE_APP_PRINT("%02X ", *(payload + i));
    }
    WE_APP_PRINT(" (");
    for (uint8_t i = 0; i < payload_length; i++) {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT(")\r\n");
}

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

#if MODULE_ROLE == MODULE_ROLE_RX
static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event,
                                       char* eventText) {
    switch (event) {
        case DaphnisI_ATEvent_P2P_RxData: {
            if (!DaphnisI_P2P_ParseRXDataEvent(&eventText, &rxData)) {
                return;
            }
            current_state = DaphnisI_P2P_SM_DataReceived;
            break;
        }
        default:
            break;
    }
}
#endif