#pragma once

#include <Arduino.h>

//-----Mode-----
#define SD_Card
// #define SPIFFS

#define JSON

#define BT
#define UART

// #define US

#define LED
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

// ---US---
#define US_TRIG_pin 17
#define US_ECHO_pin 18
// #define US_TRIG_pin 16
// #define US_ECHO_pin 17
// ===US===

//=====Pins=====

extern uint8_t _volume;

//-----JSON-----
#ifdef JSON
#include <ArduinoJson.h>
#include <SD.h>

#include "ArduinoJson/Misc/SerializedValue.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"
#include "FS.h"

extern JsonDocument config;

#ifdef US
extern uint16_t _trig_dist;
extern uint32_t _trig_lag;
#endif

#ifdef BT
struct bt_uuid
{
    char service[63];
    char tx[63];
    char rx[63];
};
extern bt_uuid _bt_uuid;
extern char _bt_name[16];
extern uint32_t _bt_pass;
#endif

#ifdef LED
struct colors
{
    String err;
    String success;
    String play;
    String restart;
};
extern colors _cols;
#endif

void get_conf();
void change_json();

#define GET_CONF() get_conf()
#define CHANGE_JSON() change_json()
#else
#define GET_CONF() ((void)0)
#define CHANGE_JSON() ((void)0)
#endif
//=====JSON=====
