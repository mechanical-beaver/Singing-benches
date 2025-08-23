#include <Global.h>
#include <LED_custom.h>
#include <Web.h>

#include <cstdint>

#include "AUDIO_custom.h"
#include "Config.h"

#ifdef N_LEDS

Adafruit_NeoPixel led_strip(LED_COUNT, LED_PIN, LED_TYPE + COLOR_SEQUENCE);

void led_init()
{
    led_strip.begin();
    led_strip.show();
    led_strip.setBrightness(50);
}

void led_on(uint32_t hex_code)
{
    if (db[kk::led_status])
    {
        led_strip.setPixelColor(0, hex_code);
        led_strip.show();
        global_timer = millis();
    }
}

void led_lag_off()
{
    uint32_t color = led_strip.getPixelColor(0);

    if (!Play_flag && color != 0)
    {
        if (millis() - global_timer >= 1000)
        {
            led_strip.clear();
            led_strip.show();
        }
    }
}
#endif
