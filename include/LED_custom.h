#pragma once

#include <Arduino.h>
#include <Config.h>
#include <FastLED.h>

#include <cstdint>
#include <cstdlib>

#include "fastspi_types.h"

#ifdef LED
#define LED_TYPE WS2812
#define COLOR_SEQUENCE GRB

#define LED_COUNT 1

extern CRGB leds[LED_COUNT];

void led_init();
void led_on(String hex_code);
void led_lag_off();

#define LED_INIT() led_init()
#define LED_ON(hex) led_on(hex)
#define LED_LAG() led_lag_off()
#else
#define LED_INIT() ((void)0)
#define LED_ON(hex) ((void)0)
#define LED_LAG() ((void)0)
#endif
