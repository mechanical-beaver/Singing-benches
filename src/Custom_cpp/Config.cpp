#include <Config.h>
#include <Global.h>

#include <cstdint>

#include "HardwareSerial.h"
#include "WString.h"

JsonDocument config;
JsonDocument statistic;

uint32_t _hex2val(String path0, String path1);
String _val2hex(uint32_t value);

#ifdef N_LEDS
colors _cols;
#endif

#ifdef JSON
const char *Json_path = "/config.json";
const char *Stat_path = "/statistic.json";

uint8_t _volume;
uint8_t global_volume;
uint32_t _volume_rest_tout;

uint16_t general_start;
uint16_t web_start;
uint16_t button_start;

void get_conf()
{
    File File = SD.open(Json_path);
    if (!File)
    {
        ERR("File not found");
    }

    DeserializationError err = deserializeJson(config, File);

    File.close();

    if (err)
    {
        char err_msg[128];
        sprintf(err_msg, "Error: %s", err.c_str());
        ERR(err_msg);
    }

#ifdef N_LEDS
    _cols.err = _hex2val("LED", "error");
    _cols.success = _hex2val("LED", "successful");
    _cols.play = _hex2val("LED", "play");
    _cols.restart = _hex2val("LED", "rest");
#endif

    _volume = config["DAC"]["volume"];
    _volume_rest_tout = config["DAC"]["rest_tout"];
}

void change_json()
{
    SD.remove(Json_path);

    File File = SD.open(Json_path, FILE_WRITE);
    if (!File)
    {
        echo("File not found");
    }

    config["LED"]["error"] = _val2hex(_cols.err);
    config["LED"]["successful"] = _val2hex(_cols.success);
    config["LED"]["play"] = _val2hex(_cols.play);
    config["LED"]["rest"] = _val2hex(_cols.restart);

    config["DAC"]["volume"] = _volume;
    config["DAC"]["rest_tout"] = _volume_rest_tout;

    if (serializeJson(config, File) == 0)
    {
        echo("failed");
        delay(250);
    }
    echo("Success");
    File.close();
}

void get_stat()
{
    File File = SD.open(Stat_path);
    if (!File)
    {
        echo("File not found");
        return;
    }

    DeserializationError err = deserializeJson(statistic, File);

    File.close();

    if (err)
    {
        char err_msg[128];
        sprintf(err_msg, "Error: %s", err.c_str());
        echo(err_msg);
        return;
    }

    general_start = statistic["starts"]["general"];
    web_start = statistic["starts"]["web"];
    button_start = statistic["starts"]["button"];

    for (uint8_t i = 0; i < tracks.size(); i++)
    {
        String name = tracks[i].name;
        tracks[i].start = statistic["tracks"][name]["start"];
    }
}

void save_stat()
{
    SD.remove(Stat_path);

    File File = SD.open(Stat_path, FILE_WRITE);
    if (!File)
    {
        echo("File not found");
        return;
    }

    statistic["starts"]["general"] = general_start;
    statistic["starts"]["web"] = web_start;
    statistic["starts"]["button"] = button_start;

    for (uint8_t i = 0; i < tracks.size(); i++)
    {
        String name = tracks[i].name;

        statistic["tracks"][name]["name"] = tracks[i].name;
        statistic["tracks"][name]["path"] = tracks[i].path;
        statistic["tracks"][name]["start"] = tracks[i].start;
    }

    if (serializeJson(statistic, File) == 0)
    {
        echo("failed");
        delay(250);
    }
    echo("Success");
    File.close();
}
#else
uint8_t _volume = global_volume = 5;
#endif

String get_txt(const char *Path)
{
    File file = SD.open(Path);
    if (!file)
    {
        echo("Not found file");
        return "";
    }

    String out;

    while (file.available())
    {
        out += file.readString();
    }

    return out;
}

uint32_t _hex2val(String path0, String path1)
{
    const char *hexStr = config[path0][path1];
    return strtoul(hexStr, nullptr, 16);
}

String _val2hex(uint32_t value)
{
    char buffer[10];
    sprintf(buffer, "0x%06x", value);
    return String(buffer);
}
