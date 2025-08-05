//-----Lib-----
#include <Arduino.h>
#include <Audio.h>
#include <NewPing.h>
#include <SD.h>
//=====Lib=====

//-----Dependents-----
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "FS.h"
#include "HardwareSerial.h"
#include "WString.h"
#include "esp32-hal-bt.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include "fl/str.h"
#include "pgmspace.h"
//=====Dependents=====

//-----Mode-----
#define SD_Card
#define SPIFFS

#define JSON

#define BT
#define UART

// #define US

#define LED
//=====Mode=====

//-----Pins-----
// PCM5102
// Typically                                     // PCM5102
// BCLK - bit clock                              // BCK
// WS - Word Select = LRCLK - left rigth clock   // LCK
// Dout                                          // DIN
#define BCK 4
#define LCK 6
#define DIN 5
// #define BCK 27
// #define LCK 25
// #define DIN 26

// SPI_CD
// Pins // ESP32 // ESP32-S3 //
// MISO //  19   //   13     //
// SCK  //  18   //   12     //
// MOSI //  23   //   11     //
// CS   //  5    //   10     //
#define SD_cs 10
//=====Pins=====

//-----US-----
#ifdef US
#define US_TRIG_pin 17
#define US_ECHO_pin 18
// #define US_TRIG_pin 16
// #define US_ECHO_pin 17
//
#define US_max_dist 300

bool Trig_flag = false;

NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

void trig_param_change(char param);
#define TRIG_CHANGE(par) trig_param_change(par)
#else
#define TRIG_CHANGE(par) ((void)0)
#endif
//=====US=====

//-----Json-----
#ifdef JSON
#include <ArduinoJson.h>

#include "ArduinoJson/Misc/SerializedValue.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"

JsonDocument config;
const char *Json_path = "/config.json";

uint8_t _volume;
uint16_t _trig_dist;
uint32_t _trig_lag;

void get_conf();
void change_json();

#define GET_CONF() get_conf()
#define CHANGE_JSON() change_json()
#else
uint8_t _volume = 5;
uint16_t _trig_dist = 100;
uint32_t _trig_lag = 1000;

#define GET_CONF() ((void)0)
#define CHANGE_JSON() ((void)0)
#endif
//=====Json=====

//-----Leds-----
#ifdef LED
#include <FastLED.h>

#include "fastspi_types.h"

#define LED_count 1
#define LED_pin 48

CRGB leds[LED_count];

struct colors
{
    String err;
    String success;
    String play;
    String restart;
};
colors _cols;

void led_on(String hex_code);
void led_lag_off();

#define LED_ON(hex) led_on(hex)
#define LED_LAG() led_lag_off()
#else
#define LED_ON(hex) ((void)0)
#define LED_LAG() ((void)0)
#endif
//=====Leds=====

//-----BT-----
#ifdef BT
#include <BLE2902.h>
#include <BLE2904.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "BLECharacteristic.h"
#include "BLESecurity.h"
// #include "BLEUUID.h"

BLEServer *BT_server = nullptr;
BLECharacteristic *BT_tx_chrctrstc;
bool connect_status = false;
bool old_connect_status = false;
bool BT_flag = false;
String BT_massage;

struct bt_uuid
{
    char service[64];
    char tx[64];
    char rx[64];
};
bt_uuid _bt_uuid;
char _bt_name[16];
uint32_t _bt_pass;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *BT_server)
    {
        connect_status = true;
    }

    void onDisconnect(BLEServer *BT_server)
    {
        connect_status = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *BT_chrctrstc)
    {
        std::string rxValue = BT_chrctrstc->getValue();
        if (rxValue.length() > 0)
        {
            BT_massage = rxValue.c_str();
        }
    }
};

void BT_init();
void BT_user_status();
template <typename T>
void Write_to_BT(T msg);

#define BT_INIT() BT_init()
#define BT_USR_STATUS() BT_user_status()
#define BT_WRITE(msg) Write_to_BT(msg)
#else
#define BT_INIT() ((void)0)
#define BT_USR_STATUS() ((void)0)
#define BT_WRITE(msg) ((void)0)
#endif
//=====BT=====

//-----UART-----
#ifdef UART
void error404(String error_massage = "");
#define ERR(err) error404(err)
#define UART_PRINT(msg) Serial.println(msg)
#else
#define ERR(err) while (1)
#define UART_PRINT(msg) ((void)0)
#endif
//=====UART=====

//-----Path-----
const char *info_path = "/info.txt";
std::vector<String> songs;
//=====Path=====

//-----Other_var-----
uint32_t global_timer;
uint8_t current_song;
bool Play_flag = false;
//=====Other_var=====

//-----Object-----
Audio PCM5102;
//======Object======

//-----Func_init-----
void echo(String msg);
void get_info(char ft);
void volume_change(bool direction);
void restart();
void get_music_list(fs::FS &fs, const char *dirname);
//=====Func_init=====

//-----MultiCore-----
TaskHandle_t Task1;
TaskHandle_t Task2;
void Task1code(void *pvParameters);
void Task2code(void *pvParameters);
//=====MultiCore=====

//-----Standart-----
void setup()
{
#ifdef UART
    Serial.begin(115200);
    if (!Serial) ERR();
#endif

#ifdef LED
    FastLED.addLeds<WS2812, LED_pin, GRB>(leds, LED_count);
    FastLED.setBrightness(100);
#endif

    if (!SD.begin(SD_cs))
    {
        ERR("SD dont init");
    }

    GET_CONF();

    BT_INIT();

    PCM5102.setPinout(BCK, LCK, DIN);
    PCM5102.setVolume(_volume);

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
        if (BT_massage.length() != 0)
        {
            core0msg = BT_massage;
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
            if (core0msg == "play")
            {
                PCM5102.connecttoFS(SD, songs[current_song].c_str());
                Play_flag = true;
            }
            else if (core0msg == "stop")
            {
                Play_flag = false;
            }
            else if (core0msg == "v+")
            {
                volume_change(true);
            }
            else if (core0msg == "v-")
            {
                volume_change(false);
            }
            else if (core0msg == "info")
            {
                get_info('I');
            }
            else if (core0msg == "json")
            {
                get_info('J');
            }
            else if (core0msg == "save")
            {
                CHANGE_JSON();
            }
            else if (core0msg == "dist")
            {
                TRIG_CHANGE('D');
            }
            else if (core0msg == "lag")
            {
                TRIG_CHANGE('L');
            }
            else if (core0msg == "rest")
            {
                restart();
            }
            else
            {
            }
            core0msg = "";
        }

#ifdef US
        if (!Play_flag)
        {
            if (!dist_lag)
            {
                old_millis = millis();
                dist = US_sensor.ping_cm();
                dist_lag = true;

                // СДЕЛАТЬ ОБРАБОТКУ ОБРЫВА ДАТЧИКА
                // Serial.println(dist);
            }

            if (dist < _trig_dist && !Trig_flag)
            {
                global_timer = millis();
                Trig_flag = true;
            }
            else if (dist < _trig_dist && Trig_flag && millis() - global_timer >= _trig_lag)
            {
                Trig_flag = false;
                Play_flag = true;
            }
            else if (dist > _trig_dist && Trig_flag && millis() - global_timer >= _trig_lag)
            {
                Trig_flag = false;
            }

            if (dist_lag && millis() - old_millis >= meas_lag)
            {
                dist_lag = false;
            }
        }
#endif

#ifdef BT
        BT_massage = "";
#endif
        BT_USR_STATUS();
        if (!Play_flag)
        {
            LED_LAG();
        }
        vTaskDelay(1);
    }
}

void Task1code(void *zapvParameters)
{
    for (;;)
    {
        if (Play_flag)
        {
            uint32_t duration;
            bool dur_stat = false;

            LED_ON(_cols.play);

            while (Play_flag)
            {
                PCM5102.loop();

                if (!dur_stat)
                {
                    if (PCM5102.getAudioFileDuration() != 0)
                    {
                        duration = PCM5102.getAudioFileDuration() + 1;
                        dur_stat = true;
                    }
                }

                if (dur_stat && duration == PCM5102.getAudioCurrentTime())
                {
                    Play_flag = false;
                }
            }
            current_song++;

            if (current_song > songs.size() - 1)
            {
                current_song = 0;
            }
        }
        vTaskDelay(1);
    }
}
//=====TaskFunc=====

//-----Func_impl-----
void echo(String msg)
{
    UART_PRINT("\n" + msg);
    BT_WRITE("\n" + msg);
}

#ifdef LED
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
            LED_ON("0x000000");
        }
    }
}
#endif

#ifdef UART
void error404(String error_massage)
{
    if (error_massage != "")
    {
        UART_PRINT(error_massage);
    }
    LED_ON(_cols.err);
    while (true);
}
#endif

#ifdef JSON
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

    _trig_dist = config["US"]["trig_dist"];
    _trig_lag = config["US"]["trig_lag"];

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

    config["US"]["trig_dist"] = _trig_dist;
    config["US"]["trig_lag"] = _trig_lag;
    config["DAC"]["volume"] = _volume;

    if (serializeJson(config, Json) == 0)
    {
        echo("failed");
    }

    Json.close();
}
#endif

#ifdef BT
void BT_init()
{
    BLEDevice::init(_bt_name);

    BLESecurity *BT_security = new BLESecurity();
    BT_security->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    BT_security->setCapability(ESP_IO_CAP_OUT);
    BT_security->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    BT_security->setKeySize(16);
    BT_security->setStaticPIN(_bt_pass);

    BT_server = BLEDevice::createServer();
    BT_server->setCallbacks(new MyServerCallbacks());

    BLEService *BT_service;
    BT_service = BT_server->createService(_bt_uuid.service);

    BT_tx_chrctrstc = BT_service->createCharacteristic(_bt_uuid.tx, BLECharacteristic::PROPERTY_NOTIFY);
    BT_tx_chrctrstc->addDescriptor(new BLE2902());

    BLECharacteristic *BT_rx_chrctrstc = BT_service->createCharacteristic(_bt_uuid.rx, BLECharacteristic::PROPERTY_WRITE);
    BT_rx_chrctrstc->setCallbacks(new MyCallbacks());

    BT_service->start();
    BT_server->getAdvertising()->start();
}

void BT_user_status()
{
    if (connect_status && !old_connect_status)
    {
        UART_PRINT("BLE client connected");
        old_connect_status = connect_status;
    }

    if (!connect_status && old_connect_status)
    {
        if (!BT_flag)
        {
            global_timer = millis();
            BT_flag = true;
        }
        else if (millis() - global_timer >= 500)
        {
            BT_server->startAdvertising();
            UART_PRINT("BLE client disconnected. Start advertising...");
            old_connect_status = connect_status;
        }
    }
}

template <typename T>
void Write_to_BT(T msg)
{
    if (connect_status)
    {
        String input = (String)msg;

        if (input.length() > 0)
        {
            BT_tx_chrctrstc->setValue(input.c_str());
            BT_tx_chrctrstc->notify();
        }
    }
}
#endif

#ifdef US
void trig_param_change(char param)
{
    echo("Send value");
    global_timer = millis();
    bool state = false;
    uint16_t value = 65535;
    while (!state)
    {
        if (Serial.available())
        {
            value = Serial.parseInt();
        }
#ifdef BT
        else if (BT_massage.length() > 0 && BT_massage != "dist" && BT_massage != "lag")
        {
            value = BT_massage.toInt();
            Serial.println(BT_massage);
            Serial.println(value);
        }
#endif

        if (millis() - global_timer >= 3000 || value != 65535)
        {
            state = true;
        }
    }

    if (value == 65535)
    {
        echo("Value not received");
        return;
    }

    if (param == 'L')
    {
        _trig_lag = value;
    }
    if (param == 'D')
    {
        if (value > US_max_dist) value = US_max_dist;
        if (value < 20) value = 20;

        _trig_dist = value;
    }
}
#endif

void get_music_list(fs::FS &fs, const char *dirname)
{
    File root = fs.open(dirname);
    if (!root)
    {
        echo("Failed open dir");
        return;
    }

    if (!root.isDirectory())
    {
        echo("Not dir");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            String filepath = String(file.path());

            filepath.toLowerCase();

            if (filepath.endsWith(".mp3"))
            {
                songs.push_back(filepath);
            }
        }
        file = root.openNextFile();
    }
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

void volume_change(bool direction)
{
    if (direction)
        _volume++;
    else if (!direction)
        _volume--;

    if (_volume > 21)
        _volume = 21;
    else if (_volume < 0)
        _volume = 0;

    PCM5102.setVolume(_volume);
}

void restart()
{
    LED_ON(_cols.restart);
    delay(1000);
    ESP.restart();
}
//=====Func_impl=====
