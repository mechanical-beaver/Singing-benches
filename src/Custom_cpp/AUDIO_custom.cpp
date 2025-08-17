#include <AUDIO_custom.h>
#include <Global.h>

#include "HardwareSerial.h"
#include "fl/str.h"
#include "pgmspace.h"

Audio PCM5102;

std::vector<String> songs;

bool Play_flag;

void I2S_init()
{
    PCM5102.setPinout(BCK, LCK, DAT);
    PCM5102.setVolume(_volume);
}

void get_music_list(fs::FS &fs, const char *dirname)
{
    File root = fs.open(dirname);
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
}

void volume_change(bool direction)
{
    if (direction)
        _volume++;
    else if (!direction)
        _volume--;

    if (_volume > 21)
        _volume = 21;
    else if (_volume < 0)
        _volume = 0;

    PCM5102.setVolume(_volume);

    char msg[16];
    sprintf(msg, "Volume: %d", _volume);
    echo(String(msg));
}

void play(const char *mp3)
{
    if (mp3 == "")
    {
        PCM5102.connecttoFS(SD, songs[current_song].c_str());
    }
    else
    {
        PCM5102.connecttoFS(SD, mp3);
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
