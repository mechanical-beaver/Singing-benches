#include <Arduino.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <FastLED.h>
#include <NewPing.h>
#include <SD.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "ArduinoJson/Object/JsonObject.hpp"
#include "HardwareSerial.h"
#include "WString.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"

// Typically                                     // PCM5102
// BCLK - bit clock                              // BCK
// WS - Word Select = LRCLK - left rigth clock   // LCK
// Dout                                          // DIN

#define SD_cs 10

#define LED_pin 48
#define LED_count 1

#define BCK 4  // 3
#define LCK 6  // 1
#define DIN 5  // 9

#define RST_pin 17

// #define US_TRIG_pin 1
// #define US_ECHO_pin 2
// #define US_max_dist 300

CRGB leds[LED_count];
Audio PCM5102;
// NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

File Json;
const char* Json_name = "/config.json";
StaticJsonDocument<256> Json_conf;
DeserializationError Json_error;

struct colors
{
    String err;
    String success;
    String play;
    String restart;
};
colors Cols;
uint8_t _volume;

uint32_t global_timer;

void led_on(String hex_code);
void error404(String error_massage = "", bool serial_activ = true);
void play();
void get_conf();
void rest();

void setup()
{
    Serial.begin(115200);
    pinMode(RST_pin, OUTPUT);
    digitalWrite(RST_pin, 1);
    FastLED.addLeds<WS2812, LED_pin, GRB>(leds, LED_count);
    FastLED.setBrightness(100);

    if (!Serial)
    {
        error404("", false);
    }

    if (!SD.begin(SD_cs))
    {
        error404("SD_card dont init");
    }

    get_conf();

    PCM5102.setPinout(BCK, LCK, DIN);
    PCM5102.setVolume(_volume);

    led_on(Cols.success);
}

void loop()
{
    if (Serial.available())
    {
        String key = Serial.readString();
        if (key == "play")
        {
            play();
        }
        else if (key)
        {
            rest();
        }
    }

    if (leds[0].r + leds[0].g + leds[0].b != 0)
    {
        if (millis() - global_timer >= 2000)
        {
            led_on("0x000000");
        }
    }
}

void led_on(String hex_code)
{
    leds[0] = strtoul(hex_code.c_str(), 0, 16);
    FastLED.show();
    global_timer = millis();
}

void error404(String error_massage, bool serial_activ)
{
    if (serial_activ)
    {
        Serial.println(error_massage);
    }
    led_on(Cols.err);
    while (true);
}

void get_conf()
{
    Json = SD.open(Json_name);
    if (!Json)
    {
        error404("File not found");
    }

    Json_error = deserializeJson(Json_conf, Json);

    Json.close();

    if (Json_error)
    {
        char err[128];
        sprintf(err, "Error: %s", Json_error.c_str());
        error404(err);
    }

    JsonObject cols = Json_conf["Led_colors"];

    Cols.err = cols["Error"].as<String>();
    Cols.success = cols["Successful"].as<String>();
    Cols.play = cols["Play"].as<String>();
    Cols.restart = cols["Restart"].as<String>();

    _volume = Json_conf["Volume"];
}

void play()
{
    bool status = true;
    uint32_t duration;
    bool dur_stat = false;

    PCM5102.connecttoFS(SD, "/test.mp3");
    led_on(Cols.play);
    while (status)
    {
        PCM5102.loop();

        if (!dur_stat)
        {
            if (PCM5102.getAudioFileDuration() != 0)
            {
                duration = PCM5102.getAudioFileDuration() + 1;
                dur_stat = true;
            }
        }

        if (dur_stat && duration == PCM5102.getAudioCurrentTime())
        {
            status = false;
        }

        if (Serial.available())
        {
            String key = Serial.readString();
            if (key == "stop")
            {
                status = false;
            }
        }
    }
}

void rest()
{
    Serial.println("Restarting...");
    led_on(Cols.restart);
    digitalWrite(RST_pin, 0);
}
