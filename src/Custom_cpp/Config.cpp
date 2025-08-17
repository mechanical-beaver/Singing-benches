#include <Config.h>
#include <Global.h>

#include <cstdint>

JsonDocument config;

#ifdef US
uint16_t _trig_dist;
uint32_t _trig_lag;
#endif

#ifdef BT
bt_uuid _bt_uuid;
char _bt_name[16];
uint32_t _bt_pass;
#endif

#ifdef LED
colors _cols;
#endif

#ifdef JSON
const char *Json_path = "/config.json";
uint8_t _volume;

void get_conf()
{
    File Json = SD.open(Json_path);
    if (!Json)
    {
        // ERR("File not init");
    }

    DeserializationError Json_error = deserializeJson(config, Json);

    Json.close();

    if (Json_error)
    {
        char err[128];
        sprintf(err, "Error: %s", Json_error.c_str());
        // ERR(err);
    }

#ifdef LED
    _cols.err = config["LED"]["error"].as<String>();
    _cols.success = config["LED"]["successful"].as<String>();
    _cols.play = config["LED"]["play"].as<String>();
    _cols.restart = config["LED"]["rest"].as<String>();
#endif

#ifdef BT
    strncpy(_bt_name, config["BT"]["name"], sizeof(_bt_name));
    _bt_pass = config["BT"]["pass"];

    strncpy(_bt_uuid.service, config["BT"]["UUID"]["service"], sizeof(_bt_uuid.service));
    strncpy(_bt_uuid.tx, config["BT"]["UUID"]["chrctrstc_tx"], sizeof(_bt_uuid.tx));
    strncpy(_bt_uuid.rx, config["BT"]["UUID"]["chrctrstc_rx"], sizeof(_bt_uuid.rx));
#endif

#ifdef US
    _trig_dist = config["US"]["trig_dist"];
    _trig_lag = config["US"]["trig_lag"];
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

#ifdef US
    config["US"]["trig_dist"] = _trig_dist;
    config["US"]["trig_lag"] = _trig_lag;
#endif
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
