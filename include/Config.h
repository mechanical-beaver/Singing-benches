#pragma once

#include <Arduino.h>

#include <cstdint>

//-----Mode-----
#define SD_Card

#define JSON

#define UART

#define N_LEDS
//=====Mode=====

//-----Pins-----

//  ---SPI_CD---
// Pins // ESP32 // ESP32-S3 //
// MISO //  19   //   13     //
// SCK  //  18   //   12     //
// MOSI //  23   //   11     //
// CS   //  5    //   10     //
#define SD_cs 10
//  ===SPI_CD===

//  ---PCM5101---
// Typically                                     // PCM5101
// BCLK - bit clock                              // BCK
// WS - Word Select = LRCLK - left rigth clock   // LCK
// Dout                                          // DIN

#define BCK 4
#define LCK 6
#define DAT 5

// #define BCK 26
// #define LCK 24
// #define DIN 25
//  ===PCM5101===

//  ---WS2812---
#define LED_PIN 48
//  ===WS2812===

//=====Pins=====

extern uint8_t _volume;
extern uint8_t global_volume;

//-----JSON-----
#ifdef JSON
#include <ArduinoJson.h>
#include <SD.h>

#include "ArduinoJson/Misc/SerializedValue.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"
#include "FS.h"

extern JsonDocument config;

#ifdef N_LEDS
struct colors
{
    uint32_t err;
    uint32_t success;
    uint32_t play;
    uint32_t restart;
};
extern colors _cols;
#endif

void get_conf();
void change_json();
String get_txt(const char* Path);

#define GET_CONF() get_conf()
#define CHANGE_JSON() change_json()
#else
#define GET_CONF() ((void)0)
#define CHANGE_JSON() ((void)0)
#endif
//=====JSON=====
