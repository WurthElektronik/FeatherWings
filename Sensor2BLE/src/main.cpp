/**
 * \file
 * \brief Main file for the Sensor2BLE.
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
#include "sensorBoard.h"

// USB-Serial debug Interface
TypeSerial *SerialDebug;

Uart ProteusIIIUART(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);

// Proteus object
PROTEUSIII *proteusIII;

// Serial communication port for Proteus-III
TypeHardwareSerial *SerialProteusIII;

// Sensors
// WE absolute pressure sensor object
PADS *sensorPADS;
// WE 3-axis acceleration sensor object
ITDS *sensorITDS;
// WE Temperature sensor object
TIDS *sensorTIDS;
// WE humidity sensor object
HIDS *sensorHIDS;

unsigned long eventattrdelay = 3;
unsigned long eventattrlastupdate = 0;

void uint32touint8array(uint8_t *result, uint32_t value);

uint32_t int8arraytouint32(uint8_t *value);

bool namechanged(uint8_t *newname, uint8_t *oldname);

void SERCOM1_Handler()
{
    ProteusIIIUART.IrqHandler();
}

struct sensorperiod
{
    unsigned long sensorperiod;
    unsigned long lastupdate;
} padssensorperiod, itdssensorperiod, hidssensorperiod, tidssensorperiod;

static void connected()
{
    SSerial_printf(SerialDebug, "device connected \n");
}
static void channelopen()
{
    SSerial_printf(SerialDebug, "device started notifications \n");
    uint8_t payload[28] = {0};
    payload[0] = 7;
    payload[1] = 1;
    payload[2] = 0;
    uint32touint8array(&payload[3], padssensorperiod.sensorperiod);
    payload[7] = 7;
    payload[8] = 1;
    payload[9] = 1;
    uint32touint8array(&payload[10], itdssensorperiod.sensorperiod);
    payload[14] = 7;
    payload[15] = 1;
    payload[16] = 2;
    uint32touint8array(&payload[17], tidssensorperiod.sensorperiod);
    payload[21] = 7;
    payload[22] = 1;
    payload[23] = 3;
    uint32touint8array(&payload[24], hidssensorperiod.sensorperiod);
    PROTEUSIII_Transmit(proteusIII, payload, 28);
}

static void disconnected()
{
    SSerial_printf(SerialDebug, "device disconnected \n");
}

static void datareceived(uint8_t *data, int length)
{
    // fisrt 6 bytes mac and 7th byte is RSSI
    int byteidx = 7;
    while (byteidx < length)
    {
        int packetlength = data[byteidx];
        int packetfunction = data[byteidx + 1];
        int packetsensortype = data[byteidx + 2];
        switch (packetfunction)
        {
        case 1:
            switch (packetsensortype)
            {
            case 0:
                padssensorperiod.sensorperiod = int8arraytouint32(&data[byteidx + 3]);
                break;
            case 1:
                itdssensorperiod.sensorperiod = int8arraytouint32(&data[byteidx + 3]);
                break;
            case 2:
                tidssensorperiod.sensorperiod = int8arraytouint32(&data[byteidx + 3]);
                break;
            case 3:
                hidssensorperiod.sensorperiod = int8arraytouint32(&data[byteidx + 3]);
                break;
            }
            break;
        }
        byteidx += packetlength;
    }
}

static PROTEUSIII_cb callbacks = {
    .datareceived = datareceived,
    .connected = connected,
    .disconnected = disconnected,
    .channelopen = channelopen,
};

void setup()
{
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialProteusIII = HSerial_create(&ProteusIIIUART);

    // Create serial port for Proteus FeatherWing with baud 115200 and 8N1
    HSerial_beginP(SerialProteusIII, 115200, (uint8_t)SERIAL_8N1);
    pinPeripheral(10, PIO_SERCOM);
    pinPeripheral(11, PIO_SERCOM);

    proteusIII = PROTEUSIII_Create(SerialDebug, SerialProteusIII, &callbacks);

    uint8_t fwversion[3] = {};
    if (PROTEUSIII_GetFWVersion(proteusIII, fwversion))
    {
        SSerial_printf(SerialDebug, "read version %d.%d.%d", fwversion[2],
                       fwversion[1], fwversion[0]);
        if ((fwversion[2] != 1) || (fwversion[1] != 4))
        {
            SSerial_printf(SerialDebug, "unsupported FW version \n");
            exit(0);
        }
    }

    uint8_t readcfg[2] = {};
    if (PROTEUSIII_GetCFGFlags(proteusIII, readcfg))
    {
        uint8_t writecfg[2] = {0x10, 0x00};
        if ((readcfg[0] != 0x10) && PROTEUSIII_SetCFGFlags(proteusIII, writecfg))
        {
            SSerial_printf(SerialDebug, "flags set \n");
        }
    }

    uint8_t timing = -1;
    if (PROTEUSIII_GetConnectionTiming(proteusIII, &timing) && timing != 12)
    {
        if (PROTEUSIII_SetConnectionTiming(proteusIII, 12))
        {
            SSerial_printf(SerialDebug, "timing set \n");
        }
    }
    uint8_t oldname[8];
    uint8_t btmacaddress[6] = {};
    if (PROTEUSIII_GetBTMacAddress(proteusIII, btmacaddress))
    {
        char newname[10];
        sprintf(newname, "W-%02x%02x%02x", btmacaddress[2], btmacaddress[1],
                btmacaddress[0]);
        if (PROTEUSIII_GetDeviceName(proteusIII, oldname) && namechanged((uint8_t *)newname, oldname))
        {
            SSerial_printf(SerialDebug, "need new name\n");
            if (PROTEUSIII_SetAdvertisingFlags(proteusIII, 1))
            {
                if (PROTEUSIII_SetDeviceName(proteusIII, (uint8_t *)newname))
                {
                    SSerial_printf(SerialDebug, "new name set\n");
                }
            }
        }
    }

    uint8_t bname[] = {'F', 'W', 'I', 'N', 'G'};

    if (PROTEUSIII_SetBeaconData(proteusIII, bname, 5))
    {
        SSerial_printf(SerialDebug, "beacon data set\n");
    }

    // Create sensor objects
    sensorPADS = PADSCreate(SerialDebug);
    sensorITDS = ITDSCreate(SerialDebug);
    sensorTIDS = TIDSCreate(SerialDebug);
    sensorHIDS = HIDSCreate(SerialDebug);

    padssensorperiod = {5000, 0};
    itdssensorperiod = {30, 0};
    tidssensorperiod = {5000, 0};
    hidssensorperiod = {5000, 0};

    // Initialize the sensors in default mode
    if (!PADS_simpleInit(sensorPADS))
    {
        SSerial_printf(SerialDebug, "PADS init failed \r\n");
    }
    if (!ITDS_simpleInit(sensorITDS))
    {
        SSerial_printf(SerialDebug, "ITDS init failed \r\n");
    }
    if (!TIDS_simpleInit(sensorTIDS))
    {
        SSerial_printf(SerialDebug, "TIDS init failed \r\n");
    }
    if (!HIDS_simpleInit(sensorHIDS))
    {
        SSerial_printf(SerialDebug, "HIDS init failed \r\n");
    }
}

void loop()
{
    PROTEUSIII_Receive(proteusIII);
    uint8_t payload[54] = {0};
    int payloadlength = 0;
    unsigned long currtime = millis();
    if (((currtime - padssensorperiod.lastupdate) >= padssensorperiod.sensorperiod) && PADS_readSensorData(sensorPADS))
    {
        SSerial_printf(
            SerialDebug, "WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
            sensorPADS->data[padsPressure], sensorPADS->data[padsTemperature]);
        payload[0] = 11;
        payload[1] = 0;
        payload[2] = 0;
        uint32touint8array(&payload[3], sensorPADS->data[padsPressure] * 100);
        uint32touint8array(&payload[7], sensorPADS->data[padsTemperature] * 100);
        payloadlength += 11;
        padssensorperiod.lastupdate = currtime;
    }
    if (((currtime - itdssensorperiod.lastupdate) >= itdssensorperiod.sensorperiod) && ITDS_readSensorData(sensorITDS))
    {
        // SSerial_printf(SerialDebug,
        //                "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g Temp: %f °C\r\n",
        //                sensorITDS->data[itdsXAcceleration],
        //                sensorITDS->data[itdsYAcceleration],
        //                sensorITDS->data[itdsZAcceleration],
        //                sensorITDS->data[itdsTemperature]);
        payload[payloadlength] = 19;
        payload[payloadlength + 1] = 0;
        payload[payloadlength + 2] = 1;
        uint32touint8array(&payload[payloadlength + 3], sensorITDS->data[itdsXAcceleration] * 1000);
        uint32touint8array(&payload[payloadlength + 7], sensorITDS->data[itdsYAcceleration] * 1000);
        uint32touint8array(&payload[payloadlength + 11], sensorITDS->data[itdsZAcceleration] * 1000);
        uint32touint8array(&payload[payloadlength + 15], sensorITDS->data[itdsTemperature] * 100);
        payloadlength += 19;
        itdssensorperiod.lastupdate = currtime;
    }
    if ((currtime - eventattrlastupdate) >= eventattrdelay)
    {
        if (ITDS_readdoubletap(sensorITDS) && sensorITDS->data[itdsDoubleTap])
        {
            SSerial_printf(SerialDebug, "tap: %f °C\r\n",
                           sensorITDS->data[itdsDoubleTap]);
            payload[payloadlength] = 3;
            payload[payloadlength + 1] = 2;
            payload[payloadlength + 2] = 1;
            payloadlength += 3;
            sensorITDS->data[itdsDoubleTap] = 0;
        }
        if (ITDS_readfreefall(sensorITDS) && sensorITDS->data[itdsFreeFall])
        {
            SSerial_printf(SerialDebug, "free fall: %f °C\r\n",
                           sensorITDS->data[itdsFreeFall]);
            payload[payloadlength] = 3;
            payload[payloadlength + 1] = 3;
            payload[payloadlength + 2] = 1;
            payloadlength += 3;
            sensorITDS->data[itdsFreeFall] = 0;
        }
        // ITDS_readstationary(sensorITDS);
        eventattrlastupdate = currtime;
    }
    if (((currtime - tidssensorperiod.lastupdate) >= tidssensorperiod.sensorperiod) && TIDS_readSensorData(sensorTIDS))
    {
        SSerial_printf(SerialDebug, "WSEN_TIDS(Temperature): %f °C\r\n",
                       sensorTIDS->data[tidsTemperature]);
        payload[payloadlength] = 7;
        payload[payloadlength + 1] = 0;
        payload[payloadlength + 2] = 2;
        uint32touint8array(&payload[payloadlength + 3], sensorTIDS->data[tidsTemperature] * 100);
        payloadlength += 7;
        tidssensorperiod.lastupdate = currtime;
    }
    if (((currtime - hidssensorperiod.lastupdate) >= hidssensorperiod.sensorperiod) && HIDS_readSensorData(sensorHIDS))
    {
        SSerial_printf(SerialDebug, "WSEN_HIDS: RH: %f %% Temp: %f °C\r\n",
                       sensorHIDS->data[hidsRelHumidity],
                       sensorHIDS->data[hidsTemperature]);
        payload[payloadlength] = 11;
        payload[payloadlength + 1] = 0;
        payload[payloadlength + 2] = 3;
        uint32touint8array(&payload[payloadlength + 3], sensorHIDS->data[hidsRelHumidity] * 100);
        uint32touint8array(&payload[payloadlength + 7], sensorHIDS->data[hidsTemperature] * 100);
        payloadlength += 11;
        hidssensorperiod.lastupdate = currtime;
    }
    if (payloadlength != 0)
    {
        PROTEUSIII_Transmit(proteusIII, payload, payloadlength);
    }
}

void uint32touint8array(uint8_t *result, uint32_t value)
{
    result[0] = (value & 0x000000ff);
    result[1] = (value & 0x0000ff00) >> 8;
    result[2] = (value & 0x00ff0000) >> 16;
    result[3] = (value & 0xff000000) >> 24;
}

uint32_t int8arraytouint32(uint8_t *value)
{
    return value[0] | (value[1] << 8) | (value[2] << 16) | (value[3] << 24);
}

bool namechanged(uint8_t *newname, uint8_t *oldname)
{
    for (int i = 0; i < 8; i++)
    {
        if (oldname[i] != newname[i])
        {
            return true;
        }
    }
    return false;
}