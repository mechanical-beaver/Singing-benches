#pragma once

#include <Arduino.h>
#include <GyverDBFile.h>
#include <SettingsGyver.h>

#define First_Upload

#define WIFI_AP_SSID "MusicBOX"
#define WIFI_AP_PASS "94278394"
#define ADMIN_PASS "2783"

#define WIFI_STA_SSID ""
#define WIFI_STA_PASS ""

extern GyverDBFile db;
extern SettingsGyver web;

void web_begin();
void web_loop();

DB_KEYS(kk, wifi_ssid, wifi_pass, admin_pass, led_status, dac_status, uart_status, wifi_sta);
