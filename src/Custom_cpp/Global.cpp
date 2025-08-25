#include <Global.h>

#include "Config.h"
#include "WString.h"

const char *info_path = "/info.txt";

uint32_t global_timer;
uint8_t current_song;

void sd_init()
{
    SPI.begin(12, 13, 11, SD_cs);
    if (!SD.begin(SD_cs))
    {
        ERR("SD dont init");
    }
}

void echo(String msg)
{
    UART_PRINT("\n" + msg);
}

void get_info(char ft)
{
    File file;
    String message;

    if (ft == 'I')
    {
        file = SD.open(info_path);

        if (!file)
        {
            echo("File not init");
        }

        while (file.available())
        {
            message = file.readString();
            echo(message);
        }

        file.close();
    }

#ifdef JSON
    else if (ft == 'J')
    {
        message += config["DAC"].as<String>();
        message += "\n";
        echo(message);
    }
#endif
}

void restart()
{
    LED_ON(_cols.restart);
    delay(1000);
    ESP.restart();
}

String get_command()
{
#ifdef UART
    if (Serial.available())
    {
        return Serial.readString();
    }
#endif
}

void command_processing(String command)
{
    //!
    if (command == "play")
    {
        PCM5102.connecttoFS(SD, tracks[current_song].path.c_str());
        Play_flag = true;
    }
    //!
    else if (command == "stop")
    {
        Play_flag = false;
    }
    //!
    else if (command == "v+")
    {
        volume_change('p');
    }
    else if (command == "v-")
    {
        volume_change('m');
    }
    //?
    else if (command == "info")
    {
        get_info('I');
    }
    else if (command == "json")
    {
        get_info('J');
    }
    //!
    else if (command == "save")
    {
        CHANGE_JSON();
    }
    //!
    else if (command == "rest")
    {
        restart();
    }
    else
    {
    }
}
