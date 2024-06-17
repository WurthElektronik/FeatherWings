![WE Logo](../../assets/WE_Logo_small_t.png)

# ICLED

## Introduction

The [ICLED](https://www.we-online.com/en/components/products/WL-ICLED) was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

Würth Elektronik eiSos provides a Software Development Kit (SDK) with examples to support all WE FeatherWings. Here are the salient features of the WE FeatherWing SDK. 
* The SDK is **open-source** and well documented.
* It uses popular open-source tool chain including an IDE.
* The examples are written in Arduino styled C/C++ for easy understanding.
* The core components of the SDK are written in pure C to enable easy porting to any microcontroller platform.
* Modular structure of the software stack makes it easy to integrate into any project.

### Software components

* **Install IDE**: Use your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/) extension).
* **PlatformIO**: This is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for software development including building, debugging, code-upload and many more. PlatformIO works well on all modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/).
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all necessary functions to utilize a full feature set of the sensors and wireless connectivity modules. More details on the SDK and dowloads under [WCS Software](https://we-online.com/wcs-software).
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from Würth Elektronik eiSos.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
* Follow the instructions under [platformio.org](https://platformio.org/install/ide?install=vscode) to install the IDE extension.


### Quick start example

The quick start example for the ICLED FeatherWing demonstrates configuring the ICLED and showing different annimations.

1. In the quick start application, ICLED are configured to show different Test based Examples. Different example can be implemented by simplily changing the Test number  . 

2. Configuration : The following parameters need to be set-up before building the code.
```C
#include "ICLED.h"
#define ADAFRUIT_FEATHER_M0_EXPRESS

#define TEST6

// initialise instance of ICLED in GRB color system
WLICLED strip(GRB);
#ifdef TEST50
Adafruit_NeoPixel onBoard = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
#endif
// debouncing interrupt switch
unsigned long interrupt_cooldown = 0;
// ISR for GPIO5 falling edge --> sets interrupt =true and clears LEDBuf
void BtnHandler() {
    if (millis() - interrupt_cooldown < 300) return;
    strip.interrupt = true;
    strip.clearChar();
    SSerial_printf(strip.SerialDebug, "Interrupt ! \n");
    interrupt_cooldown = millis();
}
```
3. Setup - The ICLED are initialized after setting up the S<sup>P</sup>I interface. 
```C
void setup() {
    // setting GPIO5 as input with internal pull-up
    pinMode(5, INPUT_PULLUP);
    // a falling edge on gpio5 causes an interrupt
    attachInterrupt(5, BtnHandler, FALLING);

    strip.begin();
}
```
3. In the main application, different application base  example are shown 
```C
void loop() {
#ifdef TEST1  // Initializing sequence
    strip.init_show();
#endif

#ifdef TEST2  // Set different pixels at differnt colour
    strip.set_pixel(0, 0, 0, 50, 128);
    strip.set_screenPixel(6, 3, 0, 50, 0, 128);
    strip.set_pixel(104, 50, 0, 0, 255);
    strip.showLoop(1000);
#endif

#ifdef TEST3  // Show alphabet
    strip.send_Alphabet(50, 50, 0);
#endif

#ifdef TEST4  //  Display "Hello World!"
    uint16_t x = strip.set_string("Hello World!", 12, 0, 50, 0, 50);
    strip.showRun(100, x, 20);
#endif

#ifdef TEST5  // Display rainbow effect
    strip.showRainbow(30, 5);

#endif

#ifdef TEST6  // Display prices
    strip.showPrice("42,69", 12, 50, 0, 0, 50);
#endif

#ifdef TEST7  // show emojis
    uint16_t x = strip.set_emoji(1, 4, 20, 20, 0, 55);
    x = strip.set_emoji(2, x, 20, 20, 0, 55);
    x = strip.set_emoji(3, x, 20, 20, 0, 55);
    x = strip.set_emoji(4, x, 20, 20, 0, 55);
    x = strip.set_emoji(5, x, 20, 20, 0, 55);
    x = strip.set_emoji(6, x, 0, 20, 0, 55);
    strip.showRunLoop(100, x);
#endif

#ifdef TEST8  // HSV and RGB color model test
    // HSV test
    strip.to_HSV();
    for (int i = 0; i < LEDCOUNT; i++) {
        strip.set_pixel(i, 357, 100, 50);
    }
    strip.showLoop(100);  // shows all pixels in one color for HSV coordinates
    strip.to_GRB();
    for (int i = 0; i < LEDCOUNT; i++) {
        strip.set_pixel(i, 0, 227, 11, 50);
    }
    strip.showLoop(100);  // shows same color in GRB
#endif

}
```
### Running the example

1. Clone or download the code.
2. Open the workspace of interest with the filename `<ICLED>.code-workspace` in Visual Studio code.
3. **Build** and **Upload** the code from the PlatformIO tab as shown in the Figure below.
4. After successful upload, click on **Monitor** in the PlatformIO extension tab to view the debug logs in the serial terminal.

![PlatformIO_BUILD](../documentation/assets/ICLEDFeatherWing_PlatformIO.jpg)

