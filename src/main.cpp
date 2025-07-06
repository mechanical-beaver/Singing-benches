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
#include "esp32-hal.h"

// Typically                                     // PCM5102
// BCLK - bit clock                              // BCK
// WS - Word Select = LRCLK - left rigth clock   // LCK
// Dout                                          // DIN

#define SD_cs 10

#define LED_pin 48
#define LED_count 1

#define BCLK 3
#define WS 1
#define Dout 9

#define US_TRIG_pin 10
#define US_ECHO_pin 11
#define US_max_dist 300

CRGB leds[LED_count];
Audio PCM5102;
NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

File Json;
const char* Json_name = "/config.json";
StaticJsonDocument<256> Json_conf;
DeserializationError Json_error;

struct colors
{
    String err;
    String success;
    String play;
};

colors Cols;

uint8_t _volume;

void led_on(String hex_code);
void error404(String error_massage = "", bool serial_activ = true);
void play();
void get_conf();

void setup()
{
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_pin, GRB>(leds, LED_count);

    if (!Serial)
    {
        error404("", false);
    }

    if (!SD.begin(SD_cs))
    {
        error404("SD_card dont init");
    }

    get_conf();

    PCM5102.setPinout(BCLK, WS, Dout);
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
    }
}

void led_on(String hex_code)
{
    leds[0] = strtoul(hex_code.c_str(), 0, 16);
    FastLED.show();
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

    _volume = Json_conf["Volume"];
}

void play()
{
    bool status = true;
    uint32_t duration;
    bool dur_stat = false;
    uint32_t timer1 = millis();

    PCM5102.connecttoFS(SD, "/test.mp3");

    while (status)
    {
        PCM5102.loop();

        if (!dur_stat)
        {
            duration = PCM5102.getAudioFileDuration();
            if (duration != 0)
            {
                dur_stat = true;
            }
        }

        if (dur_stat && millis() - timer1 >= (duration + 3) * 1000)
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
    Serial.println("end");
}
