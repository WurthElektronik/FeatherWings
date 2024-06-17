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

void setup() {
    // setting GPIO5 as input with internal pull-up
    pinMode(5, INPUT_PULLUP);
    // a falling edge on gpio5 causes an interrupt
    attachInterrupt(5, BtnHandler, FALLING);

    strip.begin();
}

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