![WE Logo](../../../../assets/WE_Logo_small_t.png)

# Calypso WiFi FeatherWing Examples

## Mosquito MQTT broker

For more information about the usage of Mosquito MQTT broker please read [Calypso WiFi FeatherWing main page](../../../).

## Cloud connectivity

For more information of cloud connection please check

* [**Microsoft Azure example**](azure/)
* [**Amazon AWS example**](aws/)

# Calypso WiFi FeatherWing

The Calypso-Sensor-Combo is an example with two WE FeatherWings: [Calypso Wi-Fi FeatherWing](../CalypsoWiFiFeatherWing) and [Sensor FeatherWing](../SensorFeatherWing) and [cloud connection](../CalypsoWiFiFeatherWing/lib/WE_CalypsoWiFiFeatherWing/examples/aws/README.md).

For more information on how to create and send information to any MQTT broker or Cloud IoT-Hub please read the [Calypso FeatherWing documentation](../CalypsoWiFiFeatherWing/lib/examples).

In the visualization chapters use device with the sensor data and export the data to the Power BI or SQL Database and later to Excel.

## Parameters

* **MQTT_CLIENT_ID**: Azure IoT Hub **device name**
* **MQTT_SERVER_ADDRESS**: Azure IoT Hub **hostname**. It can be found in the IoT Hub -> Overview
* **MQTT_PORT**: **8883**
* **MQTT_TOPIC**: ```devices/<device name>/messages/events/```
* **MQTT_USER_NAME**: ``` hostname/device name ```
* **MQTT_PASSWORD**: Shared Access Signature (**SAS**)

**Example**
```C

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

For the data visualization, please read Azure [Calypso FeatherWing documentation](../CalypsoWiFiFeatherWing/lib/examples/azure) 

## Visualize data

For the information how to create SQL database, please read appropriate chapter in the [Calypso FeatherWing documentation](../CalypsoWiFiFeatherWing/lib/examples)

### Necessary Steps
1. Follow the steps from the [Calypso FeatherWing cloud example](../CalypsoWiFiFeatherWing/lib/examples)
2. Differences from the [Calypso FeatherWing cloud example](../CalypsoWiFiFeatherWing/lib/examples) are
   * Create SQL database for the sensor values to be filled with the streal amalytics job created in the [Calypso FeatherWing cloud example](../CalypsoWiFiFeatherWing/lib/examples)
   * Go to already created database on already created server and create new table in the **Query window (preview)** to save the stream analytics job data of the real-time sensor values from the IoTHub.

#### Create SQL Database Table

```SQL

CREATE TABLE <tablename> (
	EventID bigint IDENTITY(1,1) NOT NULL PRIMARY KEY,
	deviceId varchar(18) NULL,
	messageId int NULL,
    "PADS_T[°C]" float NULL,
    "PADS_P[kPa]" float NULL,
    "ITDS_X[mg]" float NULL,
    "ITDS_Y[mg]" float NULL,
    "ITDS_Z[mg]" float NULL,
    "TIDS_T[°C]" float NULL,
    "HIDS_T[°C]" float NULL,
    "HIDS_RH[%%]" float NULL,
	EventProcessedUtcTime datetime2(7) NULL,
	PartitionId int NOT NULL,
	EventEnqueuedUtcTime datetime2(7) NULL,
	IoTHub nvarchar(4000) NULL,
	ts bigint NULL,
) GO;

```

3. Visualize the same way as in the [basic cloud example](../CalypsoWiFiFeatherWing/lib/examples).

