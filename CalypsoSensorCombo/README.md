![WE Logo](../assets/WE_Logo_small_t.png)

# Calypso WiFi FeatherWing

The Calypso-Sensor-Combo is an example with two WE FeatherWings: [Calypso Wi-Fi FeatherWing](../CalypsoFeatherWing) and [Sensor FeatherWing](../SensorFeatherWing) and [cloud connection](../CalypsoFeatherWing/lib/WE_CalypsoFeatherWing/examples/aws/README.md).

For more information on how to create and send information to any MQTT broker or Cloud IoT-Hub please read the [Calyspo FeatherWing documentation](../CalypsoFeatherWing/lib/examples).







## Parameters

* **MQTT_CLIENT_ID**: Azure IoT Hub **device name**
* **MQTT_SERVER_ADDRESS**: Azure IoT Hub **hostname**. It can be found in the IoT Hub -> Overview
* **MQTT_PORT**: **8883**
* **MQTT_TOPIC**: ```devices/<device name>/messages/events/```
* **MQTT_USER_NAME**: ``` hostname/device name ```
* **MQTT_PASSWORD**: Shared Access Signature (**SAS**)

**Example**
```

#define MQTT_CLIENT_ID "we-iot-device1"
#define MQTT_SERVER_ADDRESS "we-iothub.azure-devices.net"
#define MQTT_PORT 8883
#define MQTT_TOPIC "devices/we-iot-device1/messages/events/"
#define MQTT_USER_NAME "we-iothub.azure-devices.net/we-iot-device1"
#define MQTT_PASSWORD                                                     \
    "SharedAccessSignature "                                              \
    "sr=we-iothub.azure-devices.net%2Fdevices%2Fweiot-device1&sig=" \
    "K3WUERTHz4PqbigWqMTcw2PzpIaMGs9kngJx7rpVry7g%3D&se=1660504504"
```