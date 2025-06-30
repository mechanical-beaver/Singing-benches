#include <Arduino.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <FastLED.h>
#include <NewPing.h>
#include <SD.h>

#include <cstdint>

#include "HardwareSerial.h"
#include "WString.h"

// Lib                             //Typically                                   // PCM5102
// SCK - Continuous serial clock   BCLK - bit clock                              // BCK
// FS - Frame sync                 WS - Word Select = LRCLK - left rigth clock   // LCK
// SD - Serial data
// SDIN - data in                                                                // DIN
// SDOUT - data out

// DAC Dout 25, 26 (maybe)

#define SD_cs 4

#define LED_pin 48
#define LED_count 1

#define BCLK 1
#define WS 2
#define Dout 3

#define US_TRIG_pin 5
#define US_ECHO_pin 6
#define US_max_dist 300

CRGB leds[LED_count];

Audio PCM5102;

NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

void Led_on(uint8_t r, uint8_t g, uint8_t b);
void Error403(String error_massage = "", bool serial_activ = true);

void setup()
{
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_pin, GRB>(leds, LED_count);

    if (!Serial)
    {
        Error403("", false);
    }

    if (!SD.begin(SD_cs))
    {
        Error403("SD_card dont init");
    }

    PCM5102.setPinout(BCLK, WS, Dout);
    PCM5102.setVolume(100);

    leds[0] = CRGB(0, 255, 0);
    FastLED.show();
}

void loop()
{
}

void Led_on(uint8_t r, uint8_t g, uint8_t b)
{
    leds[0] = CRGB(r, g, b);
    FastLED.show();
}

void Error403(String error_massage, bool serial_activ)
{
    if (serial_activ)
    {
        Serial.println(error_massage);
    }
    Led_on(255, 0, 0);
    while (true);
}
