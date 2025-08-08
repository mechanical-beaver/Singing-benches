#include <LED.h>

#include <cstdint>

uint32_t timer_0;

CRGB leds[LED_count];
colors _cols;

void led_on(String hex_code)
{
    leds[0] = strtoul(hex_code.c_str(), 0, 16);
    FastLED.show();
    timer_0 = millis();
}

void led_lag_off()
{
    if (leds[0].r + leds[0].g + leds[0].b != 0)
    {
        if (millis() - timer_0 >= 1000)
        {
            led_on("0x000000");
        }
    }
}
