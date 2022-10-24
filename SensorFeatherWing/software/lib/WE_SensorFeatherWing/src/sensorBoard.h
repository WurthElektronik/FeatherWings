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

#ifndef SENSORBOARD_H
#define SENSORBOARD_H

/**         Includes         */
#include "ConfigPlatform.h"

/**         Functions definition         */
#define LENGTH_OF_NAMES 16

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    padsTemperature,
    padsPressure,
    padsProperties
} PADS_properties_t;

typedef struct {
    TypeSerial *serialDebug;
    char nameType[LENGTH_OF_NAMES];
    float data[padsProperties];
    const char *dataNames[padsProperties];
} PADS;

PADS *PADSCreate(TypeSerial *serialDebug);
void PADSDestroy(PADS *pads);
bool PADS_simpleInit(PADS *self);
bool PADS_readSensorData(PADS *self);

typedef enum {
    itdsXAcceleration,
    itdsYAcceleration,
    itdsZAcceleration,
    itdsTemperature,
    itdsDoubleTap,
    itdsFreeFall,
    itdsProperties
} ITDS_properties_t;

typedef struct {
    TypeSerial *serialDebug;
    char nameType[LENGTH_OF_NAMES];
    float data[itdsProperties];
    const char *dataNames[itdsProperties];
} ITDS;
ITDS *ITDSCreate(TypeSerial *serialDebug);
void ITDSDestroy(ITDS *itds);
bool ITDS_readSensorData(ITDS *self);
bool ITDS_simpleInit(ITDS *self);
bool ITDS_readdoubletap(ITDS *self);
bool ITDS_readfreefall(ITDS *self);
typedef enum { tidsTemperature, tidsProperties } TIDS_properties_t;
typedef struct {
    TypeSerial *serialDebug;
    char nameType[LENGTH_OF_NAMES];
    float data[tidsProperties];
    const char *dataNames[tidsProperties];
} TIDS;

TIDS *TIDSCreate(TypeSerial *serialDebug);
void TIDSDestroy(TIDS *tids);
bool TIDS_simpleInit(TIDS *self);
bool TIDS_readSensorData(TIDS *self);
typedef enum {
    hidsTemperature,
    hidsRelHumidity,
    hidsProperties
} HIDS_properties_t;
typedef struct {
    TypeSerial *serialDebug;
    char nameType[LENGTH_OF_NAMES];
    float data[hidsProperties];
    const char *dataNames[hidsProperties];
} HIDS;

HIDS *HIDSCreate(TypeSerial *serialDebug);
void HIDSDestroy(HIDS *tids);
bool HIDS_simpleInit(HIDS *self);
bool HIDS_readSensorData(HIDS *self);

#define FREE_FALL_IGNORE_DURATION 200

#define DOUBLE_TAP_IGNORE_DURATION 400

#define SINGLE_TAP_IGNORE_DURATION 20

#ifdef __cplusplus
}
#endif

#endif /* SENSORBOARD_H */