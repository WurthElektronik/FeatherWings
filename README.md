![WE Logo](assets/WE_Logo_small_t.png)

# WE Adafruit FeatherWings

[Adafruit Feather](https://www.adafruit.com/feather) is a complete line of development boards from [Adafruit](https://www.adafruit.com/) and other developers that are both standalone and stackable. They're able to be powered by [LiPo batteries](https://en.wikipedia.org/wiki/Lithium_polymer_battery) for on-the-go use or by their [micro-USB](https://www.we-online.de/katalog/de/em/connectors/input_output_connectors/wr-com) plugs for stationary projects. Feathers are flexible, portable, and as light as their namesake.

[FeatherWings](https://learn.adafruit.com/adafruit-feather/featherwings) are stacking boards and add functionality and room for prototyping. At its core, the Adafruit Feather is a complete ecosystem of products - and the best way to get your project flying.

[Würth Elektronik eiSos](https://www.we-online.de/web/en/wuerth_elektronik/start.php) presents FeatherWings that are **open source** and **fully compatible** with the **Feather form factor**. Through these development boards WE brings a range of wireless connectivity modules, sensors and power modules to the Feather ecosystem.

Hypercharge your prototyping for easy and fast solution testing.

## Quick start guide

### Necessary Steps:

* **Read the documentation!**
1. **Download and install** [Visual Studio Code](https://code.visualstudio.com/download).
![Install VS Code](assets/InstallVSCode.png)

2. **Install** [PlatformIO](https://platformio.org/) extension for the Visual Studio Code
  - **Select extensions** in the lower left
  - **Type** platformio in the *Search windows* and 
  - when PlatformIO IDE extension appears, **press blue Install button**
![Install PlatformIO](assets/InstallPlatformIO.png)
> **Note**: Wait until the Platform IO is installed.
> Instalation process can be slow, depending on the speed of you computer and network connection.

3. After PlatformIO extension is installed follow on screen instructions and **restart Visual Studio Code**.
![RestartVSCode](assets/PlatformIO_Install_finished.png)

4. **Download documentation and example code** from the https://github.com/WurthElektronik/FeatherWings using git clone or simple zip file
![DownloadGit](assets/DownloadGitCode.png)
   
5. There are **4 different** examples cases
   - Sensor FeatherWing example showing the functionality of the Sensor FeatherWing (see below for the detailed description)
   - Thyone-I Wireless FeatherWing example showing the transmit and receive functionality
   - Calypso WiFi FeatherWing example showing the WiFi capabilities
   - Sensor2Cloud connectivity example with 2 different cloud connections to Microsoft Azure and Amazon AWS

6. **Open workspace file** for the case you want to test.                  
![OpenWorkspace](assets/OpenWorkspace.png)




| Image    | Description |
| -------- | ----------- |
|![WE FeatherWings](assets/Stacked-FeatherWings-single-board.png) | [**Sensor FeatherWing** (2501000201291)](/SensorFeatherWing) <ul> <li>Acceleration (WSEN-ITDS)<li>Absolute Pressure (WSEN-PADS)<li>Temperature (WSEN-TIDS)<li>Humidity (WSEN-HIDS)<li>[Sparkfun QWIIC® Connect System](https://www.sparkfun.com/qwiic) to connect additional peripherals </ul> |
|![WE FeatherWings](assets/Stacked-FeatherWings-single-board.png) | [**Thyone-I Wireless FeatherWing** (2611059021001)](/ThyoneWirelessFeatherWing)<ul><li>2.4 GHz Proprietary radio Module<li>Robust wireless link up to 500 m<li>Easy build up networks<li>Connect to Thyone modules or USB-Stick</ul> |
|![WE FeatherWings](assets/Stacked-FeatherWings-single-board.png) | [**Calypso WiFi FeatherWing** (2610039025001)](/CalypsoWiFiFeatherWing)<ul><li>2.4 GHz WiFi-Connectivity <li>Easy and secure connection to Smart Devices<li>Out-of-the-box support to applications like HTTP(S), MQTT(S) <li>AT-style command interface </ul> |
|![WE FeatherWings](assets/Stacked-FeatherWings-single-board.png) | [**MagI³C Power FeatherWing** (2601157100001)](/MagI3CPowerFeatherWing)<ul><li>5 V and 3.3 V power supply<li>Using a range of input voltages (5 V, 9 V, 12 V, 15 V, 18 V and 24 V industrial rails)</ul> |
|![Adafruit Feather](assets/Stacked-FeatherWings-single-board-Mx.png) | Connect any **Adafruit Feather Microcontroller** <br>(e.g. [Adafruit Feather M0 Express](https://www.adafruit.com/product/3403) or [Adafruit Feather M4 Express](https://www.adafruit.com/product/3857))<ul><li>Request sensor data<li>Transfer data to RF-Module<li>Examples available on Github<li>Sourcecode available on Github</ul> |
