/*
 ***************************************************************************************************
 * This file is part of Sensors SDK:
 * https://www.we-online.com/sensors, https://github.com/WurthElektronik/Sensors-SDK_STM32
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE (license_terms_wsen_sdk.pdf)
 * LOCATED IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Driver file for the WSEN-HIDS-2525020210002 sensor.
 */
#include <stdio.h>
#include "../global/global.h"
#include "WSEN_HIDS_2525020210002.h"

/**
 * @brief Default sensor interface configuration.
 */
static const WE_sensorInterface_t hidsDefaultSensorInterface =
	{
		.sensorType = WE_HIDS,
		.interfaceType = WE_i2c,
		.options = {.i2c = {.address = HIDS_ADDRESS, .burstMode = 1, .protocol = WE_i2cProtocol_Raw, .useRegAddrMsbForMultiBytesRead = 1, .reserved = 0},
					.spi = {.chipSelectPort = 0, .chipSelectPin = 0, .burstMode = 0, .reserved = 0},
					.readTimeout = 1000,
					.writeTimeout = 1000},
		.handle = 0};
/***** STATUIC variables *****/

/**
 * @brief Read data from sensor.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] numBytesToRead Number of bytes to be read
 * @param[out] data Target buffer
 * @return Error Code
 */
static inline int8_t HIDS_ReadData(WE_sensorInterface_t *sensorInterface, uint8_t *data, uint16_t numBytesToRead)
{
	return WE_ReadReg(sensorInterface, 0xFF, numBytesToRead, data);
}

/**
 * @brief generate CRC for the data bytes
 * @param[in] data input
 * @param[in] count of data bytes
 * @retval Error code
 */
static uint8_t HIDS_GenerateCRC(const uint8_t *data, uint16_t count)
{
	uint16_t current_byte;
	uint8_t crc = CRC8_INIT;
	uint8_t crc_bit;

	/* calculates 8-Bit checksum with given polynomial */
	for (current_byte = 0; current_byte < count; ++current_byte)
	{
		crc ^= (data[current_byte]);
		for (crc_bit = 8; crc_bit > 0; --crc_bit)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ CRC8_POLYNOMIAL;
			}
			else
			{
				crc = (crc << 1);
			}
		}
	}
	return crc;
}

/**
 * @brief CRC check
 * @param[in] data input
 * @param[in] count of data bytes
 * @retval Error code
 */
static int8_t HIDS_CheckCRC(const uint8_t *data, uint16_t count, uint8_t checksum)
{
	if (HIDS_GenerateCRC(data, count) != checksum)
	{
		return WE_FAIL;
	}
	return WE_SUCCESS;
}

/**
 * @brief Write data to sensor.
 *
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[in] numBytesToRead Number of bytes to be read
 * @param[out] data Target buffer
 * @return Error Code
 */
static inline int8_t HIDS_WriteData(WE_sensorInterface_t *sensorInterface, uint8_t *data, uint16_t numBytesToWrite)
{
	/* 0xFF can be used here because it will not be used in the WE_WriteReg with WE_i2c_fifo and useRegAddrMsbForMultiBytesRead = 1; */
	return WE_WriteReg(sensorInterface, 0xFF, numBytesToWrite, data);
}

/**
 * @brief Returns the default sensor interface configuration.
 * @param[out] sensorInterface Sensor interface configuration (output parameter)
 * @return Error code
 */
int8_t HIDS_Get_Default_Interface(WE_sensorInterface_t *sensorInterface)
{
	*sensorInterface = hidsDefaultSensorInterface;
	return WE_SUCCESS;
}

/**
 * @brief Set the measurement type of the sensor
 * @param[in] sensorInterface Pointer to sensor interface
 * @retval Error code
 */
int8_t HIDS_Set_Measurement_Type(WE_sensorInterface_t *sensorInterface, hids_measureCmd_t meausurementCmd)
{
	int8_t status = WE_FAIL;
	uint8_t temp = meausurementCmd;
	status = HIDS_WriteData(sensorInterface, &temp, 1);

	/* mandatory wait for measurement to be performed, see user manual of 2525020210002 */
	WE_Delay(10);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}

	return WE_SUCCESS;
}

/**
 * @brief Measure the data in milli values
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] Temperature value in milli
 * @param[out] Humidity value in milli
 * @retval Error code
 */
int8_t HIDS_Sensor_Measure_Raw(WE_sensorInterface_t *sensorInterface, hids_measureCmd_t measureCmd, int32_t *temperatureRaw, int32_t *humidityRaw)
{

	if (WE_SUCCESS != HIDS_Set_Measurement_Type(sensorInterface, measureCmd))
	{
		/* error! */
		return WE_FAIL;
	}

	uint8_t dataBytes[6] = {0};
	int8_t status = WE_FAIL;
	uint16_t t_ticks = 0;
	uint16_t rh_ticks = 0;

	status = HIDS_ReadData(sensorInterface, dataBytes, 6);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	status = HIDS_CheckCRC(&dataBytes[0], HIDS_WORD_SIZE, dataBytes[2]);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	status = HIDS_CheckCRC(&dataBytes[3], HIDS_WORD_SIZE, dataBytes[5]);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}

	t_ticks = ((uint16_t)dataBytes[0] << 8) | ((uint16_t)dataBytes[1]);
	rh_ticks = ((uint16_t)dataBytes[3] << 8) | ((uint16_t)dataBytes[4]);

	*temperatureRaw = (int32_t)(((21875 * t_ticks) >> 13) - 45000); /* >> 13 == / 8192; temperatureRaw to be divided by 1000 for reaching rh percent */
	*humidityRaw = (int32_t)(((15625 * rh_ticks) >> 13) - 6000);	/* humidityRaw to be divided by 1000 for reaching rh percent */
	return WE_SUCCESS;
}

/**
 * @brief read the sensor serial number
 * @param[in] sensorInterface Pointer to sensor interface
 * @param[out] Serial number in Pointer to serial number as 32 bit integer
 * @retval Error code
 */
int8_t HIDS_Sensor_Read_SlNo(WE_sensorInterface_t *sensorInterface, uint32_t *serialNo)
{
	int8_t status = WE_FAIL;
	uint8_t dataBytes[6] = {0};
	hids_measureCmd_t measureCmd = HIDS_MEASURE_SERIAL_NUMBER;
	if (WE_FAIL == HIDS_Set_Measurement_Type(sensorInterface, measureCmd))
	{
		/* error! */
		return WE_FAIL;
	}
	status = HIDS_ReadData(sensorInterface, dataBytes, 6);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	status = HIDS_CheckCRC(&dataBytes[0], HIDS_WORD_SIZE, dataBytes[2]);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	status = HIDS_CheckCRC(&dataBytes[3], HIDS_WORD_SIZE, dataBytes[5]);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	*serialNo = ((uint32_t)dataBytes[0] << 24) | ((uint32_t)dataBytes[1] << 16) | ((uint32_t)dataBytes[3] << 8) | ((uint32_t)dataBytes[4]);
	return WE_SUCCESS;
}

/**
 * @brief initilaize the seonsor
 * @param[in] sensorInterface Pointer to sensor interface
 * @retval Error code
 */
int8_t HIDS_Sensor_Init(WE_sensorInterface_t *sensorInterface)
{
	uint32_t serialNo = 0;
	if (WE_SUCCESS != HIDS_Reset(sensorInterface))
	{
		/* error! */
		return WE_FAIL;
	}
	if (WE_SUCCESS != HIDS_Sensor_Read_SlNo(sensorInterface, &serialNo))
	{
		/* error! */
		return WE_FAIL;
	}
	return (serialNo > 0) ? WE_SUCCESS : WE_FAIL;
}

/**
 * @brief reset the sensor
 * @param[in] sensorInterface Pointer to sensor interface
 */
int8_t HIDS_Reset(WE_sensorInterface_t *sensorInterface)
{

	int8_t status = WE_FAIL;
	uint8_t temp = HIDS_SOFT_RESET;
	status = HIDS_WriteData(sensorInterface, &temp, 1);
	if (status != WE_SUCCESS)
	{
		/* error! */
		return WE_FAIL;
	}
	WE_Delay(10);
	return WE_SUCCESS;
}
