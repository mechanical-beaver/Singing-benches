#include <AUDIO_custom.h>
#include <Global.h>
#include <Web.h>

#include <cstdint>

#include "Arduino.h"
#include "Config.h"

Audio PCM5102;

std::vector<String> songs;

bool Play_flag;
String songs_list;
uint8_t activ_song;
uint32_t _timer;

bool update_flag = false;
uint8_t old_volume;

void I2S_init()
{
    PCM5102.setPinout(BCK, LCK, DAT);
    global_volume = _volume;
    PCM5102.setVolume(global_volume);
}

void get_music_list(fs::FS &fs, const char *dirname)
{
    File root = fs.open(dirname);
    String list;
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

    for (const auto &file : songs)
    {
        int lastSlash = file.lastIndexOf('/');
        int dotPos = file.lastIndexOf('.');
        String name = file.substring(lastSlash + 1, dotPos);
        list += name + ";";
    }
    songs_list = list;
}

void volume_change(char direction)
{
    if (direction == 'p' && global_volume < 21)
        global_volume++;
    else if (direction == 'm' && global_volume > 0)
        global_volume--;

    PCM5102.setVolume(global_volume);

    char msg[16];
    sprintf(msg, "Volume: %d", global_volume);
    echo(String(msg));
}

void update_volume()
{
    if (global_volume != _volume)
    {
        if (Play_flag)
        {
            _timer = 0;
            update_flag = false;
            return;
        }
        else
        {
            if (!update_flag)
            {
                _timer = millis();
                old_volume = global_volume;
                update_flag = true;
            }

            if (old_volume != global_volume)
            {
                _timer = millis();
                old_volume = global_volume;
            }

            if (millis() - _timer >= _volume_rest_tout)
            {
                global_volume = _volume;
                update_flag = false;
            }
        }
    }
}

void play()
{
    if (!db[kk::dac_status])
    {
        echo("DAC off");
        Play_flag = false;
        return;
    }

    echo("play");

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

    echo("end");
}
