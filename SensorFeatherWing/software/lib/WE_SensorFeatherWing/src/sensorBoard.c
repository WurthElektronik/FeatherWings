/**
 * \file
 * \brief File for the sensor board of the WE IoT design kit.
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

#include "sensorBoard.h"
#include "WSEN_HIDS_2523020210001.h" //Humidity Sensor
#include "WSEN_ITDS_2533020201601.h" //Acelerometer Sensor
#include "WSEN_PADS_2511020213301.h" //Pressure Sensor
#include "WSEN_TIDS_2521020222501.h" //Temperature Sensor

/***************************PADS OBJECT***************************/
/**
 * @brief  Allocate memory and initialize the PADS object
 * @param  serialDebug Pointer to the serial debug
 * @retval PADS object
 */
PADS *PADSCreate(TypeSerial *serialDebug)
{
    PADS *allocateInit = (PADS *)malloc(sizeof(PADS));
    allocateInit->serialDebug = serialDebug;
    strcpy(allocateInit->nameType, "PADS");
    allocateInit->data[padsTemperature] = 0;
    allocateInit->data[padsPressure] = 0;
    allocateInit->dataNames[padsTemperature] = "PADS_T";
    allocateInit->dataNames[padsPressure] = "PADS_P";
    return allocateInit;
}

/**
 * @brief  Free up the memory assigned to the PADS sensor object
 * @param  pads Pointer to the object.
 * @retval none
 */
void PADSDestroy(PADS *pads)
{
    if (pads)
    {
        free(pads);
    }
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool PADS_simpleInit(PADS *self)
{
    // On the WSEN_PADS evaluation board, the SAO pin is set to high by default
    // Using the I2C address 1 = 0x5D

    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    /* initialize the platform I2C drivers - WSEN_PADS */
    const int addr_wsen_pads = PADS_ADDRESS_I2C_1;
    status = I2CInit(addr_wsen_pads);

    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Platform I2C Init error\r\n");
#endif
        return false;
    }

    /* first communication test - WSEN_PADS*/
    status = PADS_getDeviceID(&deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == PADS_DEVICE_ID_VALUE))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug,
                       "WSEN_PADS Communication success\r\n");
        SSerial_printf(self->serialDebug, "DeviceID 0x%02X \r\n",
                       deviceIdValue);
#endif
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "WSEN_PADS Communication fail\r\n");
#endif
        return false;
    }

    /*set ODR to Power down*/
    status = PADS_setOutputDataRate(PADS_outputDataRatePowerDown);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Set ODR error\r\n");
#endif
        return false;
    }

    /*set low pass filter to BW=ODR/20*/
    status = PADS_setLowPassFilterConf(lPfilterBW2);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Set LPF config error\r\n");
#endif
        return false;
    }

    /*enable low pass filter */
    status = PADS_setLowPassFilter(PADS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "LPF enable error\r\n");
#endif
        return false;
    }

    /*enable Block Data Update*/
    status = PADS_setBlockDataUpdate(PADS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "BDU set error\r\n");
#endif
        return false;
    }

    /*select Power Mode [0:low Current Mode; 1:low noise Mode]*/
    status = PADS_setPowerMode(PADS_lowNoise);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Set power mode error\r\n");
#endif
        return false;
    }

    /*enable the Auto Increment */
    status = PADS_setAutoIncrement(PADS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Set auto increment error\r\n");
#endif
        return false;
    }

    return true;
}

/**
 * @brief  Setup the sensor in single conversion mode and trigger a readout on
 * Enter key press
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool PADS_readSensorData(PADS *self)
{
    int8_t status = WE_FAIL;
    I2CSetAddress(PADS_ADDRESS_I2C_1);
    /*Start a conversion*/
    status = PADS_setSingleConvMode(PADS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Set single conv mode error\r\n");
#endif
        return false;
    }

    delay(15);

    status = PADS_getPressure(&self->data[padsPressure]);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Get pressure failed\r\n");
#endif
        return false;
    }
    /*Round to 3 decimal places*/
    self->data[padsPressure] = round(self->data[padsPressure] * 1000) / (float)1000;

    status = PADS_getTemperature(&self->data[padsTemperature]);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Get temperature failed error\r\n");
#endif
        return false;
    }

    /*Round to 2 decimal places*/
    self->data[padsTemperature] = round(self->data[padsTemperature] * 100) / (float)100;
    return true;
}

/***************************ITDS OBJECT***************************/
/**
 * @brief  Allocate memory and initialize the ITDS object
 * @param  serialDebug Pointer to the serial debug
 * @retval ITDS object
 */
ITDS *ITDSCreate(TypeSerial *serialDebug)
{
    ITDS *allocateInit = (ITDS *)malloc(sizeof(ITDS));
    allocateInit->serialDebug = serialDebug;
    strcpy(allocateInit->nameType, "ITDS");
    allocateInit->data[itdsXAcceleration] = 0;
    allocateInit->data[itdsYAcceleration] = 0;
    allocateInit->data[itdsZAcceleration] = 0;
    allocateInit->dataNames[itdsXAcceleration] = "ITDS_X";
    allocateInit->dataNames[itdsYAcceleration] = "ITDS_Y";
    allocateInit->dataNames[itdsZAcceleration] = "ITDS_Z";
    return allocateInit;
}

/**
 * @brief  Free up the memory assigned to the ITDS sensor object
 * @param  self Pointer to the uninitialize object.
 * @retval none
 */
void ITDSDestroy(ITDS *itds)
{
    if (itds)
    {
        free(itds);
    }
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool ITDS_simpleInit(ITDS *self)
{
    int8_t status = 0;
    uint8_t deviceIdValue = 0;

    /* initialize the platform I2C drivers - WSEN_ITDS */
    const int addr_wsen_itds = ITDS_ADDRESS_I2C_1;
    status = I2CInit(addr_wsen_itds);

    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Platform I2C Init error\r\n", 25);
#endif
        return false;
    }

    /* first communication test - WSEN_ITDS */
    status = ITDS_getDeviceID(&deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == ITDS_DEVICE_ID_VALUE))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug,
                       "WSEN_ITDS Communication success\r\n");
        SSerial_printf(self->serialDebug, "DeviceID 0x%02X \r\n",
                       deviceIdValue);
#endif
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "WSEN_ITDS Communication fail\r\n");
#endif
        return false;
    }

    /*Sampling rate of 200 Hz*/
    status = ITDS_setOutputDataRate(odr6);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set ODR fail\r\n");
#endif
        return false;
    }
    /*Enable normal mode*/
    status = ITDS_setOperatingMode(normalOrLowPower);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set operating mode fail\r\n");
#endif
        return false;
    }
    status = ITDS_setpowerMode(ITDS_normalMode);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set power mode fail\r\n");
#endif
        return false;
    }
    /*Enable block data update*/
    status = ITDS_setBlockDataUpdate(ITDS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set BDU fail\r\n");
#endif
        return false;
    }
    /*Enable address auto increment*/
    status = ITDS_setAutoIncrement(ITDS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set auto increment fail\r\n");
#endif
        return false;
    }
    /*Full scale 16g*/
    status = ITDS_setFullScale(sixteenG);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set set full scale fail\r\n");
#endif
        return false;
    }
    /*Filter bandwidth = ODR/2*/
    status = ITDS_setFilteringCutoff(outputDataRate_2);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set filter cutoff fail\r\n");
#endif
        return false;
    }

    return true;
}

/**
 * @brief  Read the acceleration data in normal mode
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool ITDS_readSensorData(ITDS *self)
{
    int8_t status = WE_FAIL;
    I2CSetAddress(ITDS_ADDRESS_I2C_1);
    ITDS_state_t DRDY = ITDS_disable;
    int16_t XRawAcc = 0, YRawAcc = 0, ZRawAcc = 0;

    status = ITDS_getdataReadyState(&DRDY);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "read drdy fail\r\n");
#endif
        return false;
    }

    if (DRDY == ITDS_enable)
    {
        status = ITDS_getRawAccelerationX(&XRawAcc);
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "read acceleraion X fail\r\n");
#endif
            return false;
        }
        XRawAcc = XRawAcc >> 2;
        self->data[itdsXAcceleration] = (float)(XRawAcc);
        self->data[itdsXAcceleration] = (self->data[itdsXAcceleration]) / 1000;
        self->data[itdsXAcceleration] = (self->data[itdsXAcceleration]) * 1.952;
        /*Round to 3 decimal places*/
        self->data[itdsXAcceleration] = round(self->data[itdsXAcceleration] * 1000) / (float)1000;

        status = ITDS_getRawAccelerationY(&YRawAcc);
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "read acceleraion Y fail\r\n");
#endif
            return false;
        }
        YRawAcc = YRawAcc >> 2;
        self->data[itdsYAcceleration] = (float)(YRawAcc);
        self->data[itdsYAcceleration] = (self->data[itdsYAcceleration]) / 1000;
        self->data[itdsYAcceleration] = (self->data[itdsYAcceleration]) * 1.952;
        /*Round to 3 decimal places*/
        self->data[itdsYAcceleration] = round(self->data[itdsYAcceleration] * 1000) / (float)1000;

        status = ITDS_getRawAccelerationZ(&ZRawAcc);
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "read acceleraion Z fail\r\n");
#endif
            return false;
        }
        ZRawAcc = ZRawAcc >> 2;
        self->data[itdsZAcceleration] = (float)(ZRawAcc);
        self->data[itdsZAcceleration] = (self->data[itdsZAcceleration]) / 1000;
        self->data[itdsZAcceleration] = (self->data[itdsZAcceleration]) * 1.952;
        /*Round to 3 decimal places*/
        self->data[itdsZAcceleration] = round(self->data[itdsZAcceleration] * 1000) / (float)1000;
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "ITDS data not ready\r\n");
#endif
        return false;
    }

    return true;
}

/***************************TIDS OBJECT***************************/

/**
 * @brief  Allocate memory and initialize the TIDS object
 * @param  serialDebug Pointer to the serial debug
 * @retval TIDS object
 */
TIDS *TIDSCreate(TypeSerial *serialDebug)
{
    TIDS *allocateInit = (TIDS *)malloc(sizeof(TIDS));
    allocateInit->serialDebug = serialDebug;
    strcpy(allocateInit->nameType, "TIDS");
    allocateInit->data[tidsTemperature] = 0;
    allocateInit->dataNames[tidsTemperature] = "TIDS_T";
    return allocateInit;
}

/**
 * @brief  Free memory allocated to the sensor object
 * @param  self Pointer to the sensor object.
 * @retval none
 */
void TIDSDestroy(TIDS *tids)
{
    if (tids)
    {
        free(tids);
    }
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool TIDS_simpleInit(TIDS *self)
{
    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    /* initialize the platform I2C drivers - WSEN_TIDS */
    const int addr_wsen_tids = TIDS_ADDRESS_I2C_1;
    status = I2CInit(addr_wsen_tids);

    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Platform I2C Init error\r\n");
#endif
        return false;
    }

    /* first communication test - WSEN_TIDS */
    status = TIDS_getDeviceID(&deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == TIDS_DEVICE_ID_VALUE))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug,
                       "WSEN_TIDS Communication success\r\n");
        SSerial_printf(self->serialDebug, "DeviceID 0x%02X \r\n",
                       deviceIdValue);
#endif
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "WSEN_TIDS Communication fail\r\n",
                       30);
#endif
        return false;
    }
    return true;
}

/**
 * @brief  Read the temperature data
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool TIDS_readSensorData(TIDS *self)
{
    int8_t status = WE_FAIL;
    I2CSetAddress(TIDS_ADDRESS_I2C_1);
    TIDS_state_t temperatureDataStatus = TIDS_disable;

    /*Start a conversion*/
    status = TIDS_setSwReset(TIDS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Sw reset fail\r\n");
#endif
        return false;
    }
    status = TIDS_setSwReset(TIDS_disable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Sw reset fail\r\n");
#endif
        return false;
    }

    status = TIDS_setSingleConvMode(TIDS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "set single conversion fail\r\n");
#endif
        return false;
    }

    /*check the temp Data status*/
    status = TIDS_getBusyStatus(&temperatureDataStatus);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "get busy bit failed\r\n");
#endif
        return false;
    }
    if (temperatureDataStatus == TIDS_enable)
    {
        status = TIDS_getTemperature(&self->data[tidsTemperature]);
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "get temperature fail\r\n");
#endif
            return false;
        }
        /*Round to 2 decimal places*/
        self->data[tidsTemperature] = round(self->data[tidsTemperature] * 100) / (float)100;
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Temperature not ready\r\n");
#endif
        return false;
    }

    return true;
}

/***************************HIDS OBJECT***************************/
/**
 * @brief  Allocate memory and initialize the TIDS object
 * @param  serialDebug Pointer to the serial debug
 * @retval TIDS object
 */
HIDS *HIDSCreate(TypeSerial *serialDebug)
{
    HIDS *allocateInit = (HIDS *)malloc(sizeof(HIDS));
    allocateInit->serialDebug = serialDebug;
    strcpy(allocateInit->nameType, "HIDS");
    allocateInit->data[hidsTemperature] = 0;
    allocateInit->dataNames[hidsTemperature] = "HIDS_T";
    allocateInit->data[hidsRelHumidity] = 0;
    allocateInit->dataNames[hidsRelHumidity] = "HIDS_RH";
    return allocateInit;
}

/**
 * @brief  Free memory allocated to the sensor object
 * @param  self Pointer to the sensor object.
 * @retval none
 */
void HIDSDestroy(HIDS *hids)
{
    if (hids)
    {
        free(hids);
    }
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool HIDS_simpleInit(HIDS *self)
{
    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    /* initialize the platform I2C drivers - WSEN_HIDS */
    const int addr_wsen_hids =
        HIDS_ADDRESS_I2C_0; /* only one i2c address for hids */
    status = I2CInit(addr_wsen_hids);

    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "Platform I2C Init error\r\n");
#endif
        return false;
    }

    /* first communication test - WSEN_HIDS */
    status = HIDS_getDeviceID(&deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == HIDS_DEVICE_ID_VALUE))
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug,
                       "WSEN_HIDS Communication success\r\n");
        SSerial_printf(self->serialDebug, "DeviceID 0x%02X \r\n",
                       deviceIdValue);
#endif
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "WSEN_HIDS Communication fail\r\n");
#endif
        return false;
    }

    /*enable Block Data Update*/
    status = HIDS_setBdu(HIDS_enable);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "HIDS BDU set error\r\n");
#endif
        return false;
    }

    /*Set ODR to one shot mode*/
    status = HIDS_setOdr(odr1HZ);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "HIDS one shot set error\r\n");
#endif
        return false;
    }

    /*Power mode to active*/
    status = HIDS_setPowerMode(activeMode);
    if (status != WE_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "HIDS power mode set error\r\n");
#endif
        return false;
    }
    return true;
}

/**
 * @brief  Read the humidity data
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool HIDS_readSensorData(HIDS *self)
{
    int8_t status = WE_FAIL;
    I2CSetAddress(HIDS_ADDRESS_I2C_0);
    HIDS_state_t humStatus = HIDS_disable, tempStatus = HIDS_disable;
    /*Start a conversion*/

    if (WE_FAIL == HIDS_enOneShot(HIDS_enable))
    {
        SSerial_printf(self->serialDebug, "Could not set to one shot \r\n");
    }

    //Get status of data
    HIDS_getHumStatus(&humStatus);
    HIDS_getTempStatus(&tempStatus);

    /*check the data status*/
    if (humStatus == HIDS_enable && tempStatus == HIDS_enable)
    {
        status = HIDS_getHumidity(&(self->data[hidsRelHumidity]));
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "Get humidity failed\r\n");
#endif
            return false;
        }
        /*Round to 2 decimal places*/
        self->data[hidsRelHumidity] = round(self->data[hidsRelHumidity] * 100) / (float)100;

        status = HIDS_getTemperature(&(self->data[hidsTemperature]));
        if (status != WE_SUCCESS)
        {
#if SERIAL_DEBUG
            SSerial_printf(self->serialDebug, "Get temperature failed\r\n");
#endif
            return false;
        }
        /*Round to 2 decimal places*/
        self->data[hidsTemperature] = round(self->data[hidsTemperature] * 100) / (float)100;
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(self->serialDebug, "HIDS data not ready\r\n");
#endif
        return false;
    }

    return true;
}