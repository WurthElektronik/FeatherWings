/**
 * \file
 * \brief Main file for the WE-ThyoneWirelessFeatherWing.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
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
#include "thyoneIBoard.h"
#include "atecc608a.h"

#define THYONE_DEFAULT_RF_CHANNEL 21
#define THYONE_DEFAULT_RF_PROFILE 0

/*Configure the FeatherWing either as a transmitter or a receiver*/
#define Transmitter 0

// USB-Serial debug Interface
TypeSerial *SerialDebug;

// UART port for Thyone-I
Uart ThyoneIUART(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
String msg1;

// Thyone I object
THYONEI *thyoneI;

// Serial communication port for Thyone-I
TypeHardwareSerial *SerialThyoneI;

void SERCOM1_Handler() { ThyoneIUART.IrqHandler(); }

void setup() {
    delay(5000);
    uint8_t serialNrThy[4] = {0};

    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialThyoneI = HSerial_create(&ThyoneIUART);

    // Create serial port for Thyone FeatherWing with baud 115200 and 8N1
    HSerial_beginP(SerialThyoneI, 115200, (uint8_t)SERIAL_8N1);
    pinPeripheral(10, PIO_SERCOM);
    pinPeripheral(11, PIO_SERCOM);

    thyoneI = THYONEI_Create(SerialDebug, SerialThyoneI);

    if (!THYONEI_simpleInit(thyoneI)) {
        SSerial_printf(SerialDebug, "Thyone init failed \r\n");
    }

    if (ThyoneI_GetSerialNumber(thyoneI, serialNrThy)) {
        SSerial_printf(SerialDebug,
                       "Thyone-I default source address %02X%02X%02X%02X \r\n",
                       serialNrThy[3], serialNrThy[2], serialNrThy[1],
                       serialNrThy[0]);
    }

    /*The following code shows how to set the parameters of the Thyone radio
    module. In this case, the values are set to factory default. The settings
    are non-volatile. Each write operation performs a write to the flash memory
    and resets the module. Hence, this operation has to be limited to one time
    configuration only.*/
    ThyoneI_TXPower_t txPower;
    if (ThyoneI_GetTXPower(thyoneI, &txPower)) {
        if (txPower != ThyoneI_TXPower_8) {
            // Set the transmit power to 8 dBm
            if (!ThyoneI_SetTXPower(thyoneI, ThyoneI_TXPower_8)) {
                SSerial_printf(SerialDebug, "Thyone set power failed \r\n");
            }
        }
    } else {
        SSerial_printf(SerialDebug, "Thyone get power failed \r\n");
    }

    uint8_t rfChannel;
    if (ThyoneI_GetRFChannel(thyoneI, &rfChannel)) {
        if (rfChannel != THYONE_DEFAULT_RF_CHANNEL) {
            // Set the RF channel channel 21
            if (!ThyoneI_SetRFChannel(thyoneI, THYONE_DEFAULT_RF_CHANNEL)) {
                SSerial_printf(SerialDebug, "Thyone set channel failed \r\n");
            }
        }
    } else {
        SSerial_printf(SerialDebug, "Thyone get power failed \r\n");
    }
    uint8_t rfProfile;
    if (ThyoneI_GetRFProfile(thyoneI, &rfProfile)) {
        if (rfProfile != THYONE_DEFAULT_RF_PROFILE) {
            // Set the RF profile to long range 125 kbit/s mode
            if (!ThyoneI_SetRFProfile(thyoneI, THYONE_DEFAULT_RF_PROFILE)) {
                SSerial_printf(SerialDebug,
                               "Thyone set RF profile failed \r\n");
            }
        }
    } else {
        SSerial_printf(SerialDebug, "Thyone get power failed \r\n");
    }
}
void loop() {
#if Transmitter
    unsigned char sendBuffer[32] = "Hello from Thyone-I FeatherWing";
    /*Broadcast Hello message to all the peers in the network*/
    if (ThyoneI_TransmitBroadcast(thyoneI, sendBuffer, 32)) {
        SSerial_printf(SerialDebug, "Broadcast sent! \r\n");
    } else {
        SSerial_printf(SerialDebug, "Broadcast send failed \r\n");
    }
    delay(1000);
#else
    PacketThyoneI dataReceived;
    dataReceived = THYONEI_receiveData(thyoneI);
    // Print the received packet
    if (dataReceived.length != 0) {
        SSerial_printf(SerialDebug,
                       "Received Data from %02X%02X%02X%02X RSSI : %i dBm\r\n",
                       dataReceived.sourceAddress >> 24 & 0xFF,
                       dataReceived.sourceAddress >> 16 & 0xFF,
                       dataReceived.sourceAddress >> 8 & 0xFF,
                       dataReceived.sourceAddress & 0xFF, dataReceived.RSSI);
        SSerial_printf(SerialDebug, "Payload[%u byte]: %s\r\n",
                       dataReceived.length, dataReceived.data);
    }
#endif
}