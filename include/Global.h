#pragma once

#include <AUDIO_custom.h>
#include <Arduino.h>
#include <BT_custom.h>
#include <Config.h>
#include <LED_custom.h>
#include <SD.h>
#include <UART_custom.h>
#include <US_custom.h>

#include "FS.h"

extern uint32_t global_timer;
extern uint8_t current_song;

void sd_init();
void echo(String msg);
void get_info(char ft);
void restart();
void command_processing(String command);
