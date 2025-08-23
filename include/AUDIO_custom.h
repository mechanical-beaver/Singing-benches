#pragma once

#include <Audio.h>
#include <Config.h>
#include <FS.h>
#include <SD.h>

#include <cstdint>
#include <cstdlib>

#include "WString.h"

extern Audio PCM5102;

extern std::vector<String> songs;
extern bool Play_flag;
extern String songs_list;
extern uint8_t activ_song;

void I2S_init();
void get_music_list(fs::FS &fs, const char *dirname);
void volume_change(char direction);
void update_volume();
void play();
