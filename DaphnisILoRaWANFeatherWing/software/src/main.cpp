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

#include "DaphnisIFeatherWingExample.h"

#include <ConfigPlatform.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <DaphnisI/DaphnisI.h>
#include <we_print.h>

#include <DaphnisI/ATCommands/ATUserSettings.h>

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
void setup() {
    // Delay for 5 seconds to allow any necessary startup processes to complete
    delay(5000);

#if ((WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off) || \
     defined(WE_APP_PRINT_ENABLED))
    WE_Print_Init();
#endif

    // Initialize the Daphnis-I module with the configured UART and pin settings
    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins,
                       DaphnisI_OTAA_EventCallback)) {
        WE_APP_PRINT("Daphnis Initialization Failed\r\n");
        exit(0);
    }

    DaphnisI_Mode_t daphnisI_mode;

    // Get the current mode of the Daphnis-I module
    if (!DaphnisI_GetModeRS(&daphnisI_mode)) {
        WE_APP_PRINT("Failed to get current mode\r\n");
        exit(0);
    }

    // Check if the current mode is not LoRaWAN
    if (daphnisI_mode != DaphnisI_Mode_Lorawan) {
        // Set the mode to LoRaWAN if it is not already set
        if (!DaphnisI_SetModeUS(DaphnisI_Mode_Lorawan)) {
            WE_APP_PRINT("Failed to set mode\r\n");
            exit(0);
        }
        // Reset the Daphnis-I module to apply the new mode
        if (!DaphnisI_PinReset()) {
            WE_APP_PRINT("Failed to reset module\r\n");
            exit(0);
        }
    }

    // Initialize the sensors
    if (!DaphnisI_Featherwing_SensorSetup()) {
        WE_APP_PRINT("Sensor Initialization Failed\r\n");
        exit(0);
    }
}

void loop() {
    // Start the OTAA session for the Daphnis-I module
    DaphnisI_Featherwing_OTAA_Session();
}
