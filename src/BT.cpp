#include <BT.h>

#include <cstdint>

#include "HardwareSerial.h"
#include "esp32-hal-uart.h"

BLEServer *BT_server = nullptr;
BLECharacteristic *BT_tx_chrctrstc;

bt_uuid _bt_uuid;
char _bt_name[16];
uint32_t _bt_pass;

bool connect_status = false;
bool old_connect_status = false;
bool BT_flag = false;
uint32_t timer;
bool uart_stat = false;

String BT_massage;

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

void BT_init(bool uart_s)
{
    if (uart_s)
    {
        uart_stat = true;
    }

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
        if (uart_stat)
        {
            Serial.println("BLE client connected");
        }
        old_connect_status = connect_status;
    }

    if (!connect_status && old_connect_status)
    {
        if (!BT_flag)
        {
            timer = millis();
            BT_flag = true;
        }
        else if (millis() - timer >= 500)
        {
            BT_server->startAdvertising();
            if (uart_stat)
            {
                Serial.println("BLE client disconnected. Start advertising...");
            }
            old_connect_status = connect_status;
        }
    }
}
