#pragma once

#include <Arduino.h>
#include <BLE2902.h>
#include <BLE2904.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Config.h>

#include "BLECharacteristic.h"
#include "BLESecurity.h"

#ifdef BT
extern BLECharacteristic *BT_tx_chrctrstc;
extern bool connect_status;
extern bool old_connect_status;
extern bool BT_flag;
extern String BT_message;

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
#else
#define BT_INIT() ((void)0)
#define BT_USR_STATUS() ((void)0)
#define BT_WRITE(msg) ((void)0)
#endif
