//-----Lib-----
#include <AUDIO_custom.h>
#include <Arduino.h>
#include <BT_custom.h>
#include <Config.h>
#include <Global.h>
#include <LED_custom.h>
#include <UART_custom.h>
#include <US_custom.h>
//=====Lib=====

//-----Dependents-----
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "HardwareSerial.h"
#include "WString.h"
#include "esp32-hal.h"
#include "pgmspace.h"
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

    BT_INIT();

    I2S_init();

    xTaskCreatePinnedToCore(Task1code, "Task1", 30000, NULL, 1, &Task1, 0);
    delay(500);
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
    delay(500);

    get_music_list(SD, "/music");

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
    uint32_t old_millis;
    bool dist_lag = false;
    const uint8_t meas_lag = 250;
    uint16_t dist;

    for (;;)
    {
#ifdef BT
        if (BT_message.length() != 0)
        {
            core0msg = BT_message;
        }
#endif

#ifdef UART
        if (Serial.available())
        {
            core0msg = Serial.readString();
        }
#endif

        if (core0msg.length() != 0)
        {
            command_processing(core0msg);
            core0msg = "";
        }

#ifdef BT
        BT_message = "";
#endif
        BT_USR_STATUS();
        if (!Play_flag)
        {
            LED_LAG();
        }
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
