#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include <cstdlib>

#include "fastspi_types.h"

#define LED_count 1
#define LED_pin 48

extern CRGB leds[LED_count];

struct colors
{
    String err;
    String success;
    String play;
    String restart;
};
extern colors _cols;

void led_on(String hex_code);
void led_lag_off();

#define LED_ON(hex) led_on(hex)
#define LED_LAG() led_lag_off()
