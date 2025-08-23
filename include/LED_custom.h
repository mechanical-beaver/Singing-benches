#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Config.h>

#include <cstdint>
#include <cstdlib>

#ifdef N_LEDS
#define LED_TYPE NEO_KHZ800
#define COLOR_SEQUENCE NEO_GRB

#define LED_COUNT 1

extern Adafruit_NeoPixel led_strip;

void led_init();
void led_on(uint32_t hex_code);
void led_lag_off();

#define LED_INIT() led_init()
#define LED_ON(hex) led_on(hex)
#define LED_LAG() led_lag_off()
#else
#define LED_INIT() ((void)0)
#define LED_ON(hex) ((void)0)
#define LED_LAG() ((void)0)
#endif
