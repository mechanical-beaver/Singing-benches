#include <Global.h>

const char *info_path = "/info.txt";

uint32_t global_timer;
uint8_t current_song;

void sd_init()
{
    if (!SD.begin(SD_cs))
    {
        ERR("SD dont init");
    }
}

void echo(String msg)
{
    UART_PRINT("\n" + msg);
    BT_WRITE("\n" + msg);
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
        message += config["US"].as<String>();
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

void command_processing(String command)
{
    if (command == "play")
    {
        PCM5102.connecttoFS(SD, songs[current_song].c_str());
        Play_flag = true;
    }
    else if (command == "stop")
    {
        Play_flag = false;
    }
    else if (command == "v+")
    {
        volume_change(true);
    }
    else if (command == "v-")
    {
        volume_change(false);
    }
    else if (command == "info")
    {
        get_info('I');
    }
    else if (command == "json")
    {
        get_info('J');
    }
    else if (command == "save")
    {
        CHANGE_JSON();
    }
    else if (command == "dist")
    {
        TRIG_CHANGE('D');
    }
    else if (command == "lag")
    {
        TRIG_CHANGE('L');
    }
    else if (command == "rest")
    {
        restart();
    }
    else
    {
    }
}
