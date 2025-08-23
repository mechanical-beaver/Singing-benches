#pragma once

#include <AUDIO_custom.h>
#include <Arduino.h>
#include <Config.h>
#include <LED_custom.h>
#include <SD.h>
#include <SPI.h>
#include <UART_custom.h>

#include "WString.h"

extern uint32_t global_timer;
extern uint8_t current_song;

void sd_init();
void echo(String msg);
void get_info(char ft);
void restart();
String get_commad();
void command_processing(String command);
