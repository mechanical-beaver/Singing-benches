#include <Global.h>
#include <LED_custom.h>

#ifdef LED

CRGB leds[LED_COUNT];

void led_init()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_SEQUENCE>(leds, LED_COUNT);
    FastLED.setBrightness(100);
}

void led_on(String hex_code)
{
    leds[0] = strtoul(hex_code.c_str(), 0, 16);
    FastLED.show();
    global_timer = millis();
}

void led_lag_off()
{
    if (leds[0].r + leds[0].g + leds[0].b != 0)
    {
        if (millis() - global_timer >= 1000)
        {
            led_on("0x000000");
        }
    }
}
#endif
