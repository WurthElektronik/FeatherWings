/**
 * \file
 * \brief Main file for the WE-ProteusWirelessFeatherWing.
 *
 * \copyright (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
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

#include "proteusIIIBoard.h"

// USB-Serial debug Interface
TypeSerial *SerialDebug;

// UART port for Proteus-III
Uart ProteusIIIUART(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
String msg1;

// Proteus-III object
PROTEUSIII *proteusIII;

// Serial communication port for Proteus-III
TypeHardwareSerial *SerialProteusIII;

static void connected() { SSerial_printf(SerialDebug, "device connected \n"); }

static void channelopen() {
    SSerial_printf(SerialDebug, "notifications started \n");
}

static void disconnected() {
    SSerial_printf(SerialDebug, "device disconnected \n");
}

static void datareceived(uint8_t *data, int length) {
    SSerial_printf(SerialDebug, "data received \n");
    for (int i = 0; i < length; i++) {
        SSerial_printf(SerialDebug, "%0X ", data[i]);
    }
    SSerial_printf(SerialDebug, "\n");
}

static PROTEUSIII_cb callbacks = {.datareceived = datareceived,
                                  .connected = connected,
                                  .disconnected = disconnected,
                                  .channelopen = channelopen};

void SERCOM1_Handler() { ProteusIIIUART.IrqHandler(); }

void setup() {
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

void loop() {
    PROTEUSIII_Receive(proteusIII);
    SSerial_printf(SerialDebug, "test \n");
}