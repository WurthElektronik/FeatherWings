#ifndef CONFIFPLATFORM_H
#define CONFIFPLATFORM_H

/**         Includes         */

#define ARDUINO_PLATFORM 1

#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG 1
#endif

#ifdef ARDUINO_PLATFORM
#include "ArduinoPlatform.h"
#endif

#ifdef BASE_PLATFORM
#include "BasePlatform.h"
#endif

/**         Functions definition         */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIFPLATFORM_H */