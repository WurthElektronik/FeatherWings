/**
 * \file
 * \brief File for the sensor board of the WE IoT design kit.
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

#include "sensorBoard.h"
#include "WSEN_HIDS_2525020210001.h"  //Humidity Sensor
#include "WSEN_HIDS_2525020210002.h"  //Humidity Sensor
#include "WSEN_ITDS_2533020201601.h"  //Acelerometer Sensor
#include "WSEN_PADS_2511020213301.h"  //Pressure Sensor
#include "WSEN_TIDS_2521020222501.h"  //Temperature Sensor
#include "global.h"

static WE_sensorInterface_t hids_2525020210001, hids_2525020210002,
    tids_2521020222501, itds_2533020201601, pads_2511020213301;

bool sensorBoard_Init() {
    int8_t status = WE_FAIL;

    status = I2CInit();

    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Platform I2C Init error\r\n");
        return false;
    }

    return true;
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @param  self Pointer to the sensor object.
 * @retval true if successful false in case of failure
 */
bool PADS_2511020213301_simpleInit() {
    // On the WSEN_PADS evaluation board, the SAO pin is set to high by default
    // Using the I2C address 1 = 0x5D

    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    PADS_getDefaultInterface(&pads_2511020213301);

    /* first communication test - WSEN_PADS*/
    status = PADS_getDeviceID(&pads_2511020213301, &deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == PADS_DEVICE_ID_VALUE)) {
        WE_DEBUG_PRINT("WSEN_PADS Communication success\r\n");
        WE_DEBUG_PRINT("DeviceID 0x%02X \r\n", deviceIdValue);
    } else {
        WE_DEBUG_PRINT("WSEN_PADS Communication fail\r\n");
        return false;
    }

    /*set ODR to Power down*/
    status = PADS_setOutputDataRate(&pads_2511020213301,
                                    PADS_outputDataRatePowerDown);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Set ODR error\r\n");
        return false;
    }

    /*set low pass filter to BW=ODR/20*/
    status = PADS_setLowPassFilterConfig(&pads_2511020213301, PADS_lpFilterBW2);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Set LPF config error\r\n");
        return false;
    }

    /*enable low pass filter */
    status = PADS_enableLowPassFilter(&pads_2511020213301, PADS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("LPF enable error\r\n");
        return false;
    }

    /*enable Block Data Update*/
    status = PADS_enableBlockDataUpdate(&pads_2511020213301, PADS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("BDU set error\r\n");
        return false;
    }

    /*select Power Mode [0:low Current Mode; 1:low noise Mode]*/
    status = PADS_setPowerMode(&pads_2511020213301, PADS_lowNoise);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Set power mode error\r\n");
        return false;
    }

    /*enable the Auto Increment */
    status = PADS_enableAutoIncrement(&pads_2511020213301, PADS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Set auto increment error\r\n");
        return false;
    }

    return true;
}

/**
 * @brief  Setup the sensor in single conversion mode and trigger a readout on
 * Enter key press
 * @param  pressure pointer to float to write the pressure data to.
 * @param  temp pointer to float to write the temp data to.
 * @retval true if successful false in case of failure
 */
bool PADS_2511020213301_readSensorData(float *pressure, float *temp) {
    int8_t status = WE_FAIL;

    /*Start a conversion*/
    status = PADS_enableOneShot(&pads_2511020213301, PADS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Set single conv mode error\r\n");
        return false;
    }

    delay(15);

    if (pressure) {
        status = PADS_getPressure_float(&pads_2511020213301, pressure);
        if (status != WE_SUCCESS) {
            WE_DEBUG_PRINT("Get pressure failed\r\n");
            return false;
        }
        /*Round to 3 decimal places*/
        *pressure = round(*pressure * 1000) / (float)1000;
    }

    if (temp) {
        status = PADS_getTemperature_float(&pads_2511020213301, temp);
        if (status != WE_SUCCESS) {
            WE_DEBUG_PRINT("Get temperature failed error\r\n");
            return false;
        }

        /*Round to 2 decimal places*/
        *temp = round(*temp * 100) / (float)100;
    }

    return true;
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @retval true if successful false in case of failure
 */
bool ITDS_2533020201601_simpleInit() {
    int8_t status = 0;
    uint8_t deviceIdValue = 0;

    ITDS_getDefaultInterface(&itds_2533020201601);

    /* first communication test - WSEN_ITDS */
    status = ITDS_getDeviceID(&itds_2533020201601, &deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == ITDS_DEVICE_ID_VALUE)) {
        WE_DEBUG_PRINT("WSEN_ITDS Communication success\r\n");
        WE_DEBUG_PRINT("DeviceID 0x%02X \r\n", deviceIdValue);
    } else {
        WE_DEBUG_PRINT("WSEN_ITDS Communication fail\r\n");
        return false;
    }

    // /*Enable high performance mode*/
    status = ITDS_setOperatingMode(&itds_2533020201601, ITDS_highPerformance);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set operating mode fail\r\n");
        return false;
    }

    // /*Sampling rate of 400 Hz*/
    status = ITDS_setOutputDataRate(&itds_2533020201601, ITDS_odr7);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set ODR fail\r\n");
        return false;
    }
    status = ITDS_setPowerMode(&itds_2533020201601, ITDS_normalMode);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set power mode fail\r\n");
        return false;
    }
    /*Enable block data update*/
    status = ITDS_enableBlockDataUpdate(&itds_2533020201601, ITDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set BDU fail\r\n");
        return false;
    }
    /*Enable address auto increment*/
    status = ITDS_enableAutoIncrement(&itds_2533020201601, ITDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set auto increment fail\r\n");
        return false;
    }
    // /*2g*/
    status = ITDS_setFullScale(&itds_2533020201601, ITDS_twoG);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set set full scale fail\r\n");
        return false;
    }
    /*Filter bandwidth = ODR/2*/
    status =
        ITDS_setFilteringCutoff(&itds_2533020201601, ITDS_outputDataRate_2);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set filter cutoff fail\r\n");
        return false;
    }

    status = ITDS_enableLowNoise(&itds_2533020201601, ITDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set low noise fail\r\n");
        return false;
    }

    ITDS_enableDoubleTapEvent(&itds_2533020201601, ITDS_enable);

    /* Set tap thresholds for x, y and z (5 bits, 1 bit = 1 * full_scale
    / 32)
     */
    /* Corresponds to 4 * 2 / 32 = 0.25 g */
    ITDS_setTapThresholdX(&itds_2533020201601, 4);
    ITDS_setTapThresholdY(&itds_2533020201601, 4);
    ITDS_setTapThresholdZ(&itds_2533020201601, 4);

    /* Enable tap recognition for x, y and z */
    ITDS_enableTapX(&itds_2533020201601, ITDS_enable);
    ITDS_enableTapY(&itds_2533020201601, ITDS_enable);
    ITDS_enableTapZ(&itds_2533020201601, ITDS_enable);

    /* Set priority z-y-x */
    ITDS_setTapAxisPriority(&itds_2533020201601, ITDS_Z_Y_X);

    /* Set quiet time (1 bit = 1 * 4 / ODR) */
    /* Corresponds to 1 * 4 / 400 = 10 ms */
    ITDS_setTapQuietTime(&itds_2533020201601, 1);

    /* Set shock time (1 bit = 1 * 8 / ODR) */
    /* Corresponds to 2 * 8 / 400 = 40 ms */
    ITDS_setTapShockTime(&itds_2533020201601, 2);

    /* Set latency time (1 bit = 1 * 32 / ODR) */
    /* Corresponds to 5 * 32 / 400 = 400 ms */
    ITDS_setTapLatencyTime(&itds_2533020201601, 5);

    /* Set minimum fall duration (1 bit = 1 * 1 / ODR) */
    /* Corresponds to 6 / 200 = 30 ms */
    ITDS_setFreeFallDurationLSB(&itds_2533020201601, 6);

    /* Set free-fall threshold (value is encoded - see documentation of
     * FREE_FALL_REG for details) */
    /* Corresponds to 10 * 31.25mg = 312.5mg */
    ITDS_setFreeFallThreshold(&itds_2533020201601, ITDS_ten);

    /* Interrupts are active high */
    ITDS_setInterruptActiveLevel(&itds_2533020201601, ITDS_activeHigh);

    // /* Interrupts are push-pull */
    ITDS_setInterruptPinType(&itds_2533020201601, ITDS_pushPull);

    /* Latched mode */
    ITDS_enableLatchedInterrupt(&itds_2533020201601, ITDS_enable);

    /* Enable interrupts */
    ITDS_enableInterrupts(&itds_2533020201601, ITDS_enable);

    ITDS_enableDoubleTapINT0(&itds_2533020201601, ITDS_enable);

    ITDS_enableFreeFallINT0(&itds_2533020201601, ITDS_enable);

    return true;
}

/**
 * @brief  Read the acceleration data in normal mode
 * @param  accelX pointer to float to write the x axis acceleration data to.
 * @param  accelY pointer to float to write the y axis acceleration data to.
 * @param  accelZ pointer to float to write the z axis acceleration data to.
 * @param  temp pointer to float to write the temperature data to.
 * @retval true if successful false in case of failure
 */
bool ITDS_2533020201601_readSensorData(float *accelX, float *accelY,
                                       float *accelZ, float *temp) {
    int8_t status = WE_FAIL;
    ITDS_state_t DRDY = ITDS_disable;
    int16_t XRawAcc = 0, YRawAcc = 0, ZRawAcc = 0;

    status = ITDS_isAccelerationDataReady(&itds_2533020201601, &DRDY);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("read drdy fail\r\n");
        return false;
    }

    if (DRDY == ITDS_enable) {
        if (accelX) {
            status = ITDS_getRawAccelerationX(&itds_2533020201601, &XRawAcc);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("read acceleraion X fail\r\n");
                return false;
            }
            XRawAcc = XRawAcc >> 2;
            *accelX = (float)(XRawAcc);
            *accelX = *accelX / 1000;
            *accelX = *accelX * 0.244;
            /*Round to 3 decimal places*/
            *accelX = round(*accelX * 1000) / (float)1000;
        }

        if (accelY) {
            status = ITDS_getRawAccelerationY(&itds_2533020201601, &YRawAcc);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("read acceleraion Y fail\r\n");
                return false;
            }
            YRawAcc = YRawAcc >> 2;
            *accelY = (float)(YRawAcc);
            *accelY = *accelY / 1000;
            *accelY = *accelY * 0.244;
            /*Round to 3 decimal places*/
            *accelY = round(*accelY * 1000) / (float)1000;
        }

        if (accelZ) {
            status = ITDS_getRawAccelerationZ(&itds_2533020201601, &ZRawAcc);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("read acceleraion Z fail\r\n");
                return false;
            }
            ZRawAcc = ZRawAcc >> 2;
            *accelZ = (float)(ZRawAcc);
            *accelZ = *accelZ / 1000;
            *accelZ = *accelZ * 0.244;
            /*Round to 3 decimal places*/
            *accelZ = round(*accelZ * 1000) / (float)1000;
        }

        if (temp) {
            status = ITDS_getTemperature12bit(&itds_2533020201601, temp);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("read temperature fail\r\n");
                return false;
            }
        }
    } else {
        WE_DEBUG_PRINT("ITDS data not ready\r\n");
        return false;
    }

    return true;
}

/**
 * @brief  Read the double tap event
 * @param  stateOccured pointer to bool to store state occured.
 * @retval true if successful false in case of failure
 */
bool ITDS_2533020201601_readDoubleTapEvent(bool *stateOccured) {
    int8_t status = WE_FAIL;
    ITDS_state_t tap;
    status = ITDS_getDoubleTapState(&itds_2533020201601, &tap);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("read tap fail\r\n");
        return false;
    }
    if (tap == ITDS_enable) {
        // This is needed to reset the bit to 0
        status = ITDS_isTapEvent(&itds_2533020201601, &tap);
        if (status != WE_SUCCESS) {
            WE_DEBUG_PRINT("read tap fail\r\n");
            return false;
        }
    }
    *stateOccured = (bool)tap;
    return true;
}

/**
 * @brief  Read the free fall event
 * @param  stateOccured pointer to bool to store state occured.
 * @retval true if successful false in case of failure
 */
bool ITDS_2533020201601_readFreeFallEvent(bool *stateOccured) {
    int8_t status = WE_FAIL;
    ITDS_state_t freefall;
    status = ITDS_isFreeFallEvent(&itds_2533020201601, &freefall);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("read freefall fail\r\n");
        return false;
    }
    *stateOccured = (bool)freefall;
    return true;
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @retval true if successful false in case of failure
 */
bool TIDS_2521020222501_simpleInit() {
    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    TIDS_getDefaultInterface(&tids_2521020222501);

    /* first communication test - WSEN_TIDS */
    status = TIDS_getDeviceID(&tids_2521020222501, &deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == TIDS_DEVICE_ID_VALUE)) {
        WE_DEBUG_PRINT("WSEN_TIDS Communication success\r\n");
        WE_DEBUG_PRINT("DeviceID 0x%02X \r\n", deviceIdValue);
    } else {
        WE_DEBUG_PRINT("WSEN_TIDS Communication fail\r\n", 30);
        return false;
    }
    return true;
}

/**
 * @brief  Read the temperature data
 * @param  temp pointer to float to write the temperature data to.
 * @retval true if successful false in case of failure
 */
bool TIDS_2521020222501_readSensorData(float *temp) {
    int8_t status = WE_FAIL;
    TIDS_state_t temperatureDataStatus = TIDS_enable;

    /*Start a conversion*/
    status = TIDS_softReset(&tids_2521020222501, TIDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Sw reset fail\r\n");
        return false;
    }
    status = TIDS_softReset(&tids_2521020222501, TIDS_disable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Sw reset fail\r\n");
        return false;
    }

    status = TIDS_enableOneShot(&tids_2521020222501, TIDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("set single conversion fail\r\n");
        return false;
    }

    /*check the temp Data status*/
    do {
        if (TIDS_isBusy(&tids_2521020222501, &temperatureDataStatus) !=
            WE_SUCCESS) {
            return false;
        }
        WE_Delay(5);
    } while (temperatureDataStatus == TIDS_enable);

    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("get busy bit failed\r\n");
        return false;
    }

    if (temp) {
        status = TIDS_getTemperature(&tids_2521020222501, temp);
        if (status != WE_SUCCESS) {
            WE_DEBUG_PRINT("get temperature fail\r\n");
            return false;
        }
        /*Round to 2 decimal places*/
        *temp = round(*temp * 100) / (float)100;
    }

    return true;
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @retval true if successful false in case of failure
 */
bool HIDS_2525020210001_simpleInit() {
    int8_t status = WE_FAIL;
    uint8_t deviceIdValue = 0;

    HIDS_getDefaultInterface(&hids_2525020210001);

    /* first communication test - WSEN_HIDS */
    status = HIDS_getDeviceID(&hids_2525020210001, &deviceIdValue);

    if ((status == WE_SUCCESS) && (deviceIdValue == HIDS_DEVICE_ID_VALUE)) {
        WE_DEBUG_PRINT("WSEN_HIDS Communication success\r\n");
        WE_DEBUG_PRINT("DeviceID 0x%02X \r\n", deviceIdValue);
    } else {
        WE_DEBUG_PRINT("WSEN_HIDS Communication fail\r\n");
        return false;
    }

    /*enable Block Data Update*/
    status = HIDS_enableBlockDataUpdate(&hids_2525020210001, HIDS_enable);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("HIDS BDU set error\r\n");

        return false;
    }

    /*Set ODR to one shot mode*/
    status = HIDS_setOutputDataRate(&hids_2525020210001, HIDS_oneShot);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("HIDS one shot set error\r\n");
        return false;
    }

    /*Power mode to active*/
    status = HIDS_setPowerMode(&hids_2525020210001, HIDS_activeMode);
    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("HIDS power mode set error\r\n");
        return false;
    }
    return true;
}

/**
 * @brief  Read the humidity data
 * @param  humidity pointer to float to write the humidity data to.
 * @param  temperature pointer to float to write the temperature data to.
 * @retval true if successful false in case of failure
 */
bool HIDS_2525020210001_readSensorData(float *humidity, float *temp) {
    int8_t status = WE_FAIL;
    HIDS_state_t humStatus = HIDS_disable, tempStatus = HIDS_disable;
    /*Start a conversion*/

    if (WE_FAIL == HIDS_enableOneShot(&hids_2525020210001, HIDS_enable)) {
        WE_DEBUG_PRINT("Could not set to one shot \r\n");
    }

    // Get status of data
    HIDS_isHumidityDataAvailable(&hids_2525020210001, &humStatus);
    HIDS_isTemperatureDataAvailable(&hids_2525020210001, &tempStatus);

    /*check the data status*/
    if (humStatus == HIDS_enable && tempStatus == HIDS_enable) {
        if (humidity) {
            status = HIDS_getHumidity_float(&hids_2525020210001, humidity);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("Get humidity failed\r\n");
                return false;
            }
            /*Round to 2 decimal places*/
            *humidity = round(*humidity * 100) / (float)100;
        }

        if (temp) {
            status = HIDS_getTemperature_float(&hids_2525020210001, temp);
            if (status != WE_SUCCESS) {
                WE_DEBUG_PRINT("Get temperature failed\r\n");
                return false;
            }
            /*Round to 2 decimal places*/
            *temp = round(*temp * 100) / (float)100;
        }

    } else {
        WE_DEBUG_PRINT("HIDS data not ready\r\n");
        return false;
    }

    return true;
}

/**
 * @brief  Check the communication with the sensor and init sensor settings
 * @retval true if successful false in case of failure
 */
bool HIDS_2525020210002_simpleInit() {
    int8_t status = WE_FAIL;

    HIDS_Get_Default_Interface(&hids_2525020210002);

    /* first communication test - WSEN_HIDS */
    status = HIDS_Sensor_Init(&hids_2525020210002);

    if (status == WE_SUCCESS) {
        WE_DEBUG_PRINT("WSEN_HIDS Communication success\r\n");

    } else {
        WE_DEBUG_PRINT("WSEN_HIDS Communication fail\r\n");
        return false;
    }

    return true;
}

/**
 * @brief  Read the humidity data
 * @param  humidity pointer to float to write the humidity data to.
 * @param  temperature pointer to float to write the temperature data to.
 * @retval true if successful false in case of failure
 */
bool HIDS_2525020210002_readSensorData(float *humidity, float *temp) {
    int8_t status = WE_FAIL;
    int32_t humidity_temp, temperature_temp;
    status = HIDS_Sensor_Measure_Raw(&hids_2525020210002, HIDS_MEASURE_HPM,
                                     &temperature_temp, &humidity_temp);

    if (status != WE_SUCCESS) {
        WE_DEBUG_PRINT("Could not make measurement\r\n");
        return false;
    }

    if (humidity) {
        *humidity = humidity_temp / (float)1000;
    }

    if (temp) {
        *temp = temperature_temp / (float)1000;
    }

    return true;
}
