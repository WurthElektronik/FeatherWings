# Introduction

**Note:** KaaIoT does not natively support LoRaWAN and does not function as a LoRaWAN Network Server (LNS).
This means that LoRaWAN gateways cannot send messages directly to KaaIoT.
Instead, a LoRaWAN Network Server - such as The Things Network (TTN), ChirpStack,  or a custom LNS - is required to handle device activation, message decoding, and MAC-layer management.
Once processed, application data can be forwarded (e.g., via MQTT, HTTP) from the LNS to KaaIoT for cloud integration, analytics, or device management.

Hence, we recommend using our [guide on how to integrate KaaIoT with TTN](../TTN/TTN_integration.md#). 