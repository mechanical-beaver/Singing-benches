//-----Lib-----
#include <AUDIO_custom.h>
#include <Arduino.h>
#include <Config.h>
#include <Global.h>
#include <LED_custom.h>
#include <UART_custom.h>
#include <Web.h>
//=====Lib=====

//-----Dependents-----
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "WString.h"
#include "esp32-hal.h"
//=====Dependents=====

//-----MultiCore-----
TaskHandle_t Task1;
TaskHandle_t Task2;
void Task1code(void *pvParameters);
void Task2code(void *pvParameters);
//=====MultiCore=====

//-----Standart-----
void setup()
{
    UART_INIT();
    LED_INIT();

    sd_init();

    GET_CONF();

    I2S_init();

    xTaskCreatePinnedToCore(Task1code, "Task1", 30000, NULL, 1, &Task1, 0);
    delay(500);
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
    delay(500);

    get_music_list(SD, "/music");
    get_stat();

    web_begin();

    LED_ON(_cols.success);
}

void loop()
{
}
//=====Standart=====

//-----TaskFunc-----
void Task2code(void *pvParameters)
{
    String core0msg;

    for (;;)
    {
        web_loop();
        LED_LAG();
        update_volume();
        vTaskDelay(1);
    }
}

void Task1code(void *pvParameters)
{
    for (;;)
    {
        if (Play_flag)
        {
            play();
        }
        vTaskDelay(1);
    }
}
//=====TaskFunc=====
