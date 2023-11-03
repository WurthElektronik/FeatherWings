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

#ifndef SENSORBOARD_H
#define SENSORBOARD_H

/**         Includes         */
#include "ConfigPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool sensorBoard_Init();

extern bool PADS_2511020213301_simpleInit();
extern bool PADS_2511020213301_readSensorData(float *pressure, float *temp);

extern bool ITDS_2533020201601_simpleInit();
extern bool ITDS_2533020201601_readSensorData(float *accelX, float *accelY,
                                              float *accelZ, float *temp);
extern bool ITDS_2533020201601_readDoubleTapEvent(bool *stateOccured);
extern bool ITDS_2533020201601_readFreeFallEvent(bool *stateOccured);

extern bool TIDS_2521020222501_simpleInit();
extern bool TIDS_2521020222501_readSensorData(float *temp);

extern bool HIDS_2525020210001_simpleInit();
extern bool HIDS_2525020210001_readSensorData(float *humidity, float *temp);

extern bool HIDS_2525020210002_simpleInit();
extern bool HIDS_2525020210002_readSensorData(float *humidity, float *temp);
#ifdef __cplusplus
}
#endif

#endif /* SENSORBOARD_H */
