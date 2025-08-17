#pragma once

#include <Audio.h>
#include <Config.h>
#include <FS.h>
#include <SD.h>

#include <cstdlib>

extern Audio PCM5102;

extern std::vector<String> songs;
extern bool Play_flag;

void I2S_init();
void get_music_list(fs::FS &fs, const char *dirname);
void volume_change(bool direction);
void play(const char *mp3 = "");
