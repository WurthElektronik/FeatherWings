#ifndef DAPHNISIFEATHERWINGEXAMPLE_H
#define DAPHNISIFEATHERWINGEXAMPLE_H

#include <stdbool.h>
#include <DaphnisI/DaphnisI.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
/**
 * @brief Initializes the sensor board and individual sensors.
 *
 * This function initializes the I2C communication and sets up the sensors in
 * default mode. It checks the initialization status of each sensor and returns
 * false if any initialization fails.
 *
 * @return True if all sensors are successfully initialized, false otherwise.
 *
 * @see SensorFeatherWing example for detailed usage.
 */
extern bool DaphnisI_Featherwing_SensorSetup(void);

/**
 * @brief Reads and prints sensor values from various sensors.
 *
 * This function reads data from multiple sensors and prints the values to the
 * debug output. It includes atmospheric pressure, temperature, acceleration,
 * double tap event, free fall event, and humidity readings.
 *
 * @see SensorFeatherWing example for detailed usage.
 */
extern void Print_SensorValues(void);

/**
 * @brief Reads sensor data and converts it into a byte array for LoRaWAN
 * transmission.
 *
 * This function reads data from various sensors, converts the float values into
 * a byte array, and stores them in the provided payload array. The index is
 * updated to reflect the position in the payload array.
 *
 * @param payloadP Pointer to the byte array where the sensor data will be
 * stored.
 * @param indexP Pointer to the current index in the payload array.
 */
extern void DaphnisI_Featherwings_ReadAndConvertSensorData(uint8_t *payloadP,
                                                           uint8_t *indexP);

/**
 * @brief In this example the module, that is already registered at AWS, TTN
 * network, etc., is joining the network with the provided application EUI and
 * application key (USER ACTION REQUIRED). After joining, the module transmits
 * sensor data from the SensorFeatherWing every 60 seconds and is able to
 * receive data from the server.
 */
extern void DaphnisI_Featherwing_OTAA_Session(void);

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Daphnis
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from DaphnisI_ATEvent_t). Some events might in fact be
 * responses to AT commands that are not included in DaphnisI_ATEvent_t.
 */
extern void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event,
                                        char *eventText);

#ifdef __cplusplus
}
#endif

#endif  // DAPHNISI_FEATHERWINGS_EXAMPLE_H
