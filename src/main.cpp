// Typically                                     // PCM5102
// BCLK - bit clock                              // BCK
// WS - Word Select = LRCLK - left rigth clock   // LCK
// Dout                                          // DIN

//-----Lib-----
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <BLE2902.h>
#include <BLE2904.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <FastLED.h>
#include <NewPing.h>
#include <SD.h>

//-----Dependents-----
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "ArduinoJson/Object/JsonObject.hpp"
#include "HardwareSerial.h"
#include "WString.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"

//-----Pins-----
#define SD_cs 10
#define LED_pin 48
#define BCK 4
#define LCK 6
#define DIN 5
#define RST_pin 17
// #define US_TRIG_pin 1
// #define US_ECHO_pin 2
// #define US_max_dist 300

//-----BT_define-----
#define BT_name "ESP32_Test"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

//-----OtherDefine-----
#define LED_count 1

//-----Json_var-----
File Json;
const char *Json_name = "/config.json";
StaticJsonDocument<256> Json_conf;
DeserializationError Json_error;

//-----BLE_var-----
BLEServer *pServer = nullptr;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool BT_flag = false;

//-----Leds_var-----
CRGB leds[LED_count];
struct colors
{
    String err;
    String success;
    String play;
    String restart;
};
colors Cols;

//-----Other_var-----
uint8_t _volume;
uint32_t global_timer;

//-----Classes-----
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            Serial.println(rxValue.c_str());  // BLE → Serial
        }
    }
};

//-----Object-----
Audio PCM5102;
// NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

//-----Func_init-----
void led_on(String hex_code);
void error404(String error_massage = "", bool serial_activ = true);
void play();
void get_conf();
void rest();
void BT_init();
void BT_connect();
void BT_disconnect();
void Write_to_BT();

//-----Standart-----
void setup()
{
    Serial.begin(115200);
    pinMode(RST_pin, OUTPUT);
    digitalWrite(RST_pin, 1);
    FastLED.addLeds<WS2812, LED_pin, GRB>(leds, LED_count);
    FastLED.setBrightness(100);

    if (!Serial)
    {
        error404("", false);
    }

    if (!SD.begin(SD_cs))
    {
        error404("SD_card dont init");
    }

    get_conf();

    BT_init();

    PCM5102.setPinout(BCK, LCK, DIN);
    PCM5102.setVolume(_volume);

    led_on(Cols.success);
}

void loop()
{
    // if (Serial.available())
    // {
    //     String key = Serial.readString();
    //     if (key == "play")
    //     {
    //         play();
    //     }
    //     else if (key == "restart")
    //     {
    //         rest();
    //     }
    // }

    if (leds[0].r + leds[0].g + leds[0].b != 0)
    {
        if (millis() - global_timer >= 1000)
        {
            led_on("0x000000");
        }
    }

    BT_connect();
    BT_disconnect();
    Write_to_BT();
}

//-----Func_impl-----
void led_on(String hex_code)
{
    leds[0] = strtoul(hex_code.c_str(), 0, 16);
    FastLED.show();
    global_timer = millis();
}

void error404(String error_massage, bool serial_activ)
{
    if (serial_activ)
    {
        Serial.println(error_massage);
    }
    led_on(Cols.err);
    while (true);
}

void get_conf()
{
    Json = SD.open(Json_name);
    if (!Json)
    {
        error404("File not found");
    }

    Json_error = deserializeJson(Json_conf, Json);

    Json.close();

    if (Json_error)
    {
        char err[128];
        sprintf(err, "Error: %s", Json_error.c_str());
        error404(err);
    }

    JsonObject cols = Json_conf["Led_colors"];

    Cols.err = cols["Error"].as<String>();
    Cols.success = cols["Successful"].as<String>();
    Cols.play = cols["Play"].as<String>();
    Cols.restart = cols["Restart"].as<String>();

    _volume = Json_conf["Volume"];
}

void BT_init()
{
    BLEDevice::init(BT_name);

    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT);  // клиент вводит PIN
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setKeySize(16);
    pSecurity->setStaticPIN(123456);  // <-- PIN-код

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Характеристика TX — ESP32 → BLE клиент (NOTIFY)
    pTxCharacteristic =
        pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());

    // Характеристика RX — BLE клиент → ESP32(WRITE)
    BLECharacteristic *pRxCharacteristic =
        pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    pServer->getAdvertising()->start();
}

void BT_connect()
{
    if (deviceConnected && !oldDeviceConnected)
    {
        Serial.println("BLE client connected.");
        oldDeviceConnected = deviceConnected;
    }
}

void BT_disconnect()
{
    if (!deviceConnected && oldDeviceConnected)
    {
        if (!BT_flag)
        {
            global_timer = millis();
            BT_flag = true;
        }
        else if (millis() - global_timer >= 500)
        {
            pServer->startAdvertising();
            Serial.println("BLE client disconnected. Start advertising...");
            oldDeviceConnected = deviceConnected;
        }
    }
}

void Write_to_BT()
{
    if (deviceConnected && Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        input.trim();  // удалить \r и пробелы, если есть

        if (input.length() > 0)
        {
            // Отправка BLE клиенту
            pTxCharacteristic->setValue(input.c_str());
            pTxCharacteristic->notify();
        }
    }
}

void play()
{
    bool status = true;
    uint32_t duration;
    bool dur_stat = false;

    PCM5102.connecttoFS(SD, "/test.mp3");
    led_on(Cols.play);
    while (status)
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
            status = false;
        }

        if (Serial.available())
        {
            String key = Serial.readString();
            if (key == "stop")
            {
                status = false;
            }
        }
    }
}

void rest()
{
    Serial.println("Restarting...");
    led_on(Cols.restart);
    delay(1000);
    digitalWrite(RST_pin, 0);
}
