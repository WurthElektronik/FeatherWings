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
#include "ProteusIII.h"
#include "sensorBoard.h"

ProteusIII_Pins_t ProteusIII_pins;

#define SETEBOS_MODE_PIN 14

float PADS_pressure, PADS_temp;
float ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp;
bool ITDS_doubleTapEvent, ITDS_freeFallEvent;
float TIDS_temp;
float HIDS_humidity, HIDS_temp;

/* Sensor2BLE State Machine */
typedef enum
{
    Sensor2BLE_SM_Idle = 0x0,
    Sensor2BLE_SM_Send_Sensor_Interval = 0x1,
    Sensor2BLE_SM_Send_Sensor_Data = 0x2
} Sensor2BLE_SM_t;

static volatile Sensor2BLE_SM_t currentstate = Sensor2BLE_SM_Idle;

#define HIDS_PART_NUMBER 2525020210002

/* Callback functions for various indications sent by the Proteus-III. */
static void
RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
           int8_t rssi);
static void ConnectCallback(bool success, uint8_t *btMac);
static void DisconnectCallback(ProteusIII_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload);

unsigned long eventattrdelay = 3;
unsigned long eventattrlastupdate = 0;

void uint32touint8array(uint8_t *result, uint32_t value);

uint32_t int8arraytouint32(uint8_t *value);

bool namechanged(uint8_t *newname, uint8_t *oldname);

typedef struct
{
    unsigned long sensorperiod;
    unsigned long lastupdate;
} sensorperiod;

sensorperiod padssensorperiod, itdssensorperiod, hidssensorperiod, tidssensorperiod;

static uint8_t payload[54] = {0};
static int payloadLength = 0;

void setup()
{

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    ProteusIII_pins.ProteusIII_Pin_SleepWakeUp.pin = 9;
    ProteusIII_pins.ProteusIII_Pin_Mode.pin = 17;

    ProteusIII_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;
    callbackConfig.connectCb = ConnectCallback;
    callbackConfig.disconnectCb = DisconnectCallback;
    callbackConfig.channelOpenCb = ChannelOpenCallback;

    /*Set the mode pin on Setebos to Proteus-III mode*/
    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, LOW);

    ProteusIII_Init(&ProteusIII_pins, PROTEUSIII_DEFAULT_BAUDRATE,
                    WE_FlowControl_NoFlowControl,
                    ProteusIII_OperationMode_CommandMode, callbackConfig);

    uint8_t fwversion[3] = {};
    if (ProteusIII_GetFWVersion(fwversion))
    {
        WE_DEBUG_PRINT("read version %d.%d.%d \n", fwversion[2],
                       fwversion[1], fwversion[0]);
        // check for minimum firmware version 1.4.0
        if ((fwversion[2] < 1) ||
            ((fwversion[2] == 1) && fwversion[1] < 4))
        {
            WE_DEBUG_PRINT("unsupported FW version \n");
            exit(0);
        }
    }

    uint8_t readcfg[2] = {0};
    if (ProteusIII_GetCFGFlags((uint16_t *)readcfg))
    {
        uint16_t writecfg = 0x1000;
        if ((readcfg[0] != 0x10) && ProteusIII_SetCFGFlags(writecfg))
        {
            WE_DEBUG_PRINT("flags set \n");
        }
    }

    ProteusIII_ConnectionTiming_t timing;
    if (ProteusIII_GetConnectionTiming(&timing) && timing != ProteusIII_ConnectionTiming_12)
    {
        if (ProteusIII_SetConnectionTiming(ProteusIII_ConnectionTiming_12))
        {
            WE_DEBUG_PRINT("timing set \n");
        }
    }
    uint8_t oldname[10];
    uint16_t oldnameLength = sizeof(oldname);
    uint8_t btmacaddress[6] = {};
    if (ProteusIII_GetBTMAC(btmacaddress))
    {
        char newname[10];
        sprintf(newname, "W-%02x%02x%02x", btmacaddress[2], btmacaddress[1],
                btmacaddress[0]);
        if (ProteusIII_GetDeviceName(oldname, &oldnameLength) && namechanged((uint8_t *)newname, oldname))
        {
            WE_DEBUG_PRINT("need new name\n");
            if (ProteusIII_SetAdvertisingFlags(ProteusIII_AdvertisingFlags_DeviceNameAndUuid))
            {
                if (ProteusIII_SetDeviceName((uint8_t *)newname, 8))
                {
                    WE_DEBUG_PRINT("new name set\n");
                }
            }
        }
    }

    uint8_t bname[] = {'F', 'W', 'I', 'N', 'G'};

    if (ProteusIII_SetBeacon(bname, 5))
    {
        WE_DEBUG_PRINT("beacon data set\n");
    }

    padssensorperiod = {5000, 0};
    itdssensorperiod = {30, 0};
    tidssensorperiod = {5000, 0};
    hidssensorperiod = {5000, 0};

    if (!sensorBoard_Init())
    {
        WE_DEBUG_PRINT("I2C init failed \r\n");
    }
    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit())
    {
        WE_DEBUG_PRINT("PADS init failed \r\n");
    }

    if (!ITDS_2533020201601_simpleInit())
    {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
    }

    if (!TIDS_2521020222501_simpleInit())
    {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
    }

#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#endif
}

void loop()
{
    switch (currentstate)
    {
    default:
    case Sensor2BLE_SM_Idle:
        break;
    case Sensor2BLE_SM_Send_Sensor_Interval:
    {
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
        payloadLength = 28;
        ProteusIII_Transmit(payload, payloadLength);
        WE_Delay(50);
        currentstate = Sensor2BLE_SM_Send_Sensor_Data;
        break;
    }
    case Sensor2BLE_SM_Send_Sensor_Data:
    {
        unsigned long currtime = millis();
        payloadLength = 0;
        if (((currtime - padssensorperiod.lastupdate) >= padssensorperiod.sensorperiod) && PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp))
        {
            WE_DEBUG_PRINT("WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
                           PADS_pressure, PADS_temp);
            payload[0] = 11;
            payload[1] = 0;
            payload[2] = 0;
            uint32touint8array(&payload[3], PADS_pressure * 100);
            uint32touint8array(&payload[7], PADS_temp * 100);
            payloadLength += 11;
            padssensorperiod.lastupdate = currtime;
        }
        if (((currtime - itdssensorperiod.lastupdate) >= itdssensorperiod.sensorperiod) && ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                                                                                                             &ITDS_accelZ, &ITDS_temp))
        {
            // WE_DEBUG_PRINT(SerialDebug,
            //                "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g Temp: %f °C\r\n",
            //                sensorITDS->data[itdsXAcceleration],
            //                sensorITDS->data[itdsYAcceleration],
            //                sensorITDS->data[itdsZAcceleration],
            //                sensorITDS->data[itdsTemperature]);
            payload[payloadLength] = 19;
            payload[payloadLength + 1] = 0;
            payload[payloadLength + 2] = 1;
            uint32touint8array(&payload[payloadLength + 3], ITDS_accelX * 1000);
            uint32touint8array(&payload[payloadLength + 7], ITDS_accelY * 1000);
            uint32touint8array(&payload[payloadLength + 11], ITDS_accelZ * 1000);
            uint32touint8array(&payload[payloadLength + 15], ITDS_temp * 100);
            payloadLength += 19;
            itdssensorperiod.lastupdate = currtime;
        }
        if ((currtime - eventattrlastupdate) >= eventattrdelay)
        {
            if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent) && ITDS_doubleTapEvent)
            {
                WE_DEBUG_PRINT("Double Tap Detected\r\n");
                payload[payloadLength] = 3;
                payload[payloadLength + 1] = 2;
                payload[payloadLength + 2] = 1;
                payloadLength += 3;
            }
            if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent) && ITDS_freeFallEvent)
            {
                WE_DEBUG_PRINT("Free Fall Detected\r\n");
                payload[payloadLength] = 3;
                payload[payloadLength + 1] = 3;
                payload[payloadLength + 2] = 1;
                payloadLength += 3;
            }
            eventattrlastupdate = currtime;
        }
        if (((currtime - tidssensorperiod.lastupdate) >= tidssensorperiod.sensorperiod) && TIDS_2521020222501_readSensorData(&TIDS_temp))
        {
            WE_DEBUG_PRINT("WSEN_TIDS(Temperature): %f °C\r\n", TIDS_temp);
            payload[payloadLength] = 7;
            payload[payloadLength + 1] = 0;
            payload[payloadLength + 2] = 2;
            uint32touint8array(&payload[payloadLength + 3], TIDS_temp * 100);
            payloadLength += 7;
            tidssensorperiod.lastupdate = currtime;
        }
        if (((currtime - hidssensorperiod.lastupdate) >= hidssensorperiod.sensorperiod) &&
#if HIDS_PART_NUMBER == 2525020210001
            HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp)
#elif HIDS_PART_NUMBER == 2525020210002
            HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp)
#else
            false
#endif
        )
        {
            WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                           HIDS_temp);
            payload[payloadLength] = 11;
            payload[payloadLength + 1] = 0;
            payload[payloadLength + 2] = 3;
            uint32touint8array(&payload[payloadLength + 3], HIDS_humidity * 100);
            uint32touint8array(&payload[payloadLength + 7], HIDS_temp * 100);
            payloadLength += 11;
            hidssensorperiod.lastupdate = currtime;
        }
        if (payloadLength != 0)
        {
            ProteusIII_Transmit(payload, payloadLength);
        }
        break;
    }
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

static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
                       int8_t rssi)
{
    int byteidx = 7;
    while (byteidx < payloadLength)
    {
        int packetlength = payload[byteidx];
        int packetfunction = payload[byteidx + 1];
        int packetsensortype = payload[byteidx + 2];
        switch (packetfunction)
        {
        case 1:
            switch (packetsensortype)
            {
            case 0:
                padssensorperiod.sensorperiod = int8arraytouint32(&payload[byteidx + 3]);
                break;
            case 1:
                itdssensorperiod.sensorperiod = int8arraytouint32(&payload[byteidx + 3]);
                break;
            case 2:
                tidssensorperiod.sensorperiod = int8arraytouint32(&payload[byteidx + 3]);
                break;
            case 3:
                hidssensorperiod.sensorperiod = int8arraytouint32(&payload[byteidx + 3]);
                break;
            }
            break;
        }
        byteidx += packetlength;
    }
}

static void ConnectCallback(bool success, uint8_t *btMac)
{
    WE_DEBUG_PRINT("device connected \n");
    currentstate = Sensor2BLE_SM_Idle;
}

static void DisconnectCallback(ProteusIII_DisconnectReason_t reason)
{
    WE_DEBUG_PRINT("device disconnected \n");
    currentstate = Sensor2BLE_SM_Idle;
}

static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload)
{
    WE_DEBUG_PRINT("device started notifications \n");
    currentstate = Sensor2BLE_SM_Send_Sensor_Interval;
}
