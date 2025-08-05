/**
 * \file
 * \brief Main file for the WE-ThyoneWirelessFeatherWing.
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
#include <ThyoneI/ThyoneI.h>
#include <we_print.h>

/*Connect the pins 1 and 2 on JP1 to use the Wake-up feature on Pin GPIO9*/
static const WE_Arduino_Pin_t pin_SleepWakeUp = WE_ARDUINO_PIN(9);

/*Connect the pins 3 and 4 on JP1 to use the mode change feature on Pin
 * GPIOA3/Pin17*/
static const WE_Arduino_Pin_t pin_Mode = WE_ARDUINO_PIN(17);

static ThyoneI_Pins_t ThyoneI_pins = {
    .ThyoneI_Pin_Reset = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_SleepWakeUp = WE_PIN((void *)&pin_SleepWakeUp),
    .ThyoneI_Pin_Boot = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_Mode = WE_PIN((void *)&pin_Mode),
    .ThyoneI_Pin_Busy = WE_PIN_UNDEFINED(),
};

static WE_UART_t ThyoneI_uart;

/*Configure the FeatherWing either as a transmitter or a receiver*/
#define Transmitter 1
#define SETEBOS_MODE_PIN 14

#define THYONE_DEFAULT_RF_CHANNEL 21
#define THYONE_DEFAULT_RF_PROFILE (ThyoneI_Profile_t) ThyoneI_Profile_125kbit

String msg1;

char sendBuffer[224];
char temp[64];

#if Transmitter == 0
static void RxCallback(uint8_t *payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi);
#endif
void setup() {
    delay(3000);
    uint8_t serialNrThy[4] = {0};

#if ((WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off) || \
     defined(WE_APP_PRINT_ENABLED))
    WE_Print_Init();
#endif

    /*Set the mode pin on Setebos to Thyone-I mode*/
    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, HIGH);

    ThyoneI_uart.baudrate = THYONEI_DEFAULT_BAUDRATE;
    ThyoneI_uart.flowControl = WE_FlowControl_NoFlowControl;
    ThyoneI_uart.parity = WE_Parity_None;
    ThyoneI_uart.uartInit = WE_UART_RXPin11_TXPin10_Init;
    ThyoneI_uart.uartDeinit = WE_UART_RXPin11_TXPin10_DeInit;
    ThyoneI_uart.uartTransmit = WE_UART_RXPin11_TXPin10_Transmit;

    if (!ThyoneI_Init(&ThyoneI_uart, &ThyoneI_pins,
                      ThyoneI_OperationMode_CommandMode,
#if Transmitter == 1
                      NULL
#else
                      RxCallback
#endif
                      )) {
        WE_APP_PRINT("Thyone init failed \r\n");
    }

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_APP_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
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
                WE_APP_PRINT("Thyone set power failed \r\n");
            }
        }
    } else {
        WE_APP_PRINT("Thyone get power failed \r\n");
    }

    uint8_t rfChannel;
    if (ThyoneI_GetRFChannel(&rfChannel)) {
        if (rfChannel != THYONE_DEFAULT_RF_CHANNEL) {
            // Set the RF channel channel 21
            if (!ThyoneI_SetRFChannel(THYONE_DEFAULT_RF_CHANNEL)) {
                WE_APP_PRINT("Thyone set channel failed \r\n");
            }
        }
    } else {
        WE_APP_PRINT("Thyone get power failed \r\n");
    }
    ThyoneI_Profile_t rfProfile;
    if (ThyoneI_GetRfProfile(&rfProfile)) {
        if (rfProfile != THYONE_DEFAULT_RF_PROFILE) {
            // Set the RF profile to long range 125 kbit/s mode
            if (!ThyoneI_SetRfProfile(THYONE_DEFAULT_RF_PROFILE)) {
                WE_APP_PRINT("Thyone set RF profile failed \r\n");
            }
        }
    } else {
        WE_APP_PRINT("Thyone get power failed \r\n");
    }
}
void loop() {
#if Transmitter == 1
    sprintf(sendBuffer, "Hello World!\r\n");
    WE_APP_PRINT(sendBuffer);

    /*Broadcast Hello message to all the peers in the network*/
    if (ThyoneI_TransmitBroadcast((unsigned char *)sendBuffer,
                                  strlen(sendBuffer))) {
    } else {
        WE_APP_PRINT("Broadcast send failed \r\n");
    }
    delay(5000);
#endif
}

#if Transmitter == 0
/* callback for data reception */
static void RxCallback(uint8_t *payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi) {
    int i = 0;
    WE_APP_PRINT("Received data from address 0x%02lx with %d dBm:\n-> ",
                 sourceAddress, rssi);
    WE_APP_PRINT("0x ");
    for (i = 0; i < payload_length; i++) {
        WE_APP_PRINT("%02x ", *(payload + i));
    }
    WE_APP_PRINT("\n-> ");
    for (i = 0; i < payload_length; i++) {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT("\n");
}
#endif
