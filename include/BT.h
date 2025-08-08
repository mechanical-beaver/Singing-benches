#pragma once

#include <Arduino.h>
#include <BLE2902.h>
#include <BLE2904.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "BLECharacteristic.h"
#include "BLESecurity.h"

extern BLECharacteristic *BT_tx_chrctrstc;
extern bool connect_status;
extern bool old_connect_status;
extern bool BT_flag;
extern String BT_massage;

struct bt_uuid
{
    char service[64];
    char tx[64];
    char rx[64];
};
extern bt_uuid _bt_uuid;
extern char _bt_name[16];
extern uint32_t _bt_pass;

void BT_init(bool uart_s = true);
void BT_user_status();
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

#define BT_INIT() BT_init(true)
#define BT_USR_STATUS() BT_user_status()
#define BT_WRITE(msg) Write_to_BT(msg)
