#include <Config.h>
#include <Global.h>

#include <cstdint>

#include "WString.h"

JsonDocument config;

uint32_t _hex2val(String path0, String path1);
String _val2hex(uint32_t value);

#ifdef N_LEDS
colors _cols;
#endif

#ifdef JSON
const char *Json_path = "/config.json";
uint8_t _volume;
uint8_t global_volume;

void get_conf()
{
    File Json = SD.open(Json_path);
    if (!Json)
    {
        ERR("File not init");
    }

    DeserializationError Json_error = deserializeJson(config, Json);

    Json.close();

    if (Json_error)
    {
        char err[128];
        sprintf(err, "Error: %s", Json_error.c_str());
        ERR(err);
    }

#ifdef N_LEDS
    _cols.err = _hex2val("LED", "error");
    _cols.success = _hex2val("LED", "successful");
    _cols.play = _hex2val("LED", "play");
    _cols.restart = _hex2val("LED", "rest");
#endif

    _volume = config["DAC"]["volume"];
}

void change_json()
{
    SD.remove(Json_path);

    File Json = SD.open(Json_path, FILE_WRITE);
    if (!Json)
    {
        echo("File not init");
    }

    config["LED"]["error"] = _val2hex(_cols.err);
    config["LED"]["successful"] = _val2hex(_cols.success);
    config["LED"]["play"] = _val2hex(_cols.play);
    config["LED"]["rest"] = _val2hex(_cols.restart);

    config["DAC"]["volume"] = _volume;

    if (serializeJson(config, Json) == 0)
    {
        echo("failed");
        delay(250);
    }
    echo("Success");
    Json.close();
}
#else
uint8_t _volume = 5;
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
