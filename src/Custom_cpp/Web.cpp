#include <AUDIO_custom.h>
#include <Global.h>
#include <LittleFS.h>
#include <SD.h>
#include <Web.h>
#include <WiFi.h>

#include <cstdint>

#include "Arduino.h"
#include "Config.h"
#include "UART_custom.h"
#include "WiFiType.h"

GyverDBFile db(&LittleFS, "/data.db");
SettingsGyver web("MusicBox", &db);

void wifi_start();

void build(sets::Builder& b)
{
    // Guest
    if (b.beginGroup(""))
    {
        {
            sets::GuestAccess g(b);

            if (b.Select("Музыка:", songs_list))
            {
                activ_song = b.build.value;
            }

            if (b.Slider(1, "Громкость:", 0, 21, 1, "", &global_volume))
            {
                volume_change('s');
            }

            if (b.Button("Играть", 0x00FF00))
            {
                if (!Play_flag)
                {
                    PCM5102.connecttoFS(SD, songs[activ_song].c_str());
                    Play_flag = true;
                }
            }

            b.Link("Автор", "https://www.youtube.com/watch?v=dQw4w9WgXcQ&ab_channel=RickAstley");
        }
        b.endGroup();
    }

    // Admin
    //     Button
    if (b.Button("stop", 0xFF0000))
    {
        Play_flag = false;
    }

    // if (b.Button("update music list", 0x0800FF))
    // {
    //     get_music_list(SD, "/music");
    // }

    if (b.Button("save config", 0xFF0040))
    {
        CHANGE_JSON();
    }

    if (b.Button("restar", 0xFFFF00))
    {
        restart();
    }
    //     Info
    b.Paragraph("Info", get_txt("/info.txt"));

    //     Settings
    if (b.beginGroup("LED"))
    {
        b.Switch(kk::led_status, "OFF/ON");

        b.Color("Error", &_cols.err);
        b.Color("Succsesful", &_cols.success);
        b.Color("Playing", &_cols.play);
        b.Color("Restarting", &_cols.restart);

        b.endGroup();
    }

    if (b.beginGroup("DAC"))
    {
        b.Switch(kk::dac_status, "OFF/ON");
        b.Slider("Volume", 0, 21, 1, "", &_volume);

        b.endGroup();
    };

    if (b.Switch(kk::uart_status, "UART"))
    {
        SWITCH_UART(db[kk::uart_status]);
    }

    if (b.Switch(kk::wifi_sta, "AP/STA"))
    {
        wifi_start();
    }
}

void update(sets::Updater& u)
{
    u.update(1, global_volume);
}

void wifi_start()
{
    if (db[kk::wifi_sta])
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);

        delay(500);

        uint8_t tries = 20;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            if (!--tries)
            {
                echo("Up STA failed");
                WiFi.mode(WIFI_AP);
                WiFi.softAP(db[kk::wifi_ssid], db[kk::wifi_pass]);
                break;
            }
        }
    }
    else
    {
        WiFi.mode(WIFI_AP);

        WiFi.softAP(db[kk::wifi_ssid], db[kk::wifi_pass]);
    }
}

void web_begin()
{
    LittleFS.begin(true);

    web.fs.sd.setFS(SD);

    db.begin();

    db.init(kk::wifi_ssid, WIFI_AP_SSID);
    db.init(kk::wifi_pass, WIFI_AP_PASS);
    db.init(kk::admin_pass, ADMIN_PASS);
    db.init(kk::led_status, (bool)1);
    db.init(kk::dac_status, (bool)1);
    db.init(kk::uart_status, (bool)1);
    db.init(kk::wifi_sta, (bool)1);

    wifi_start();

    // WiFi.mode(WIFI_AP);
    // WiFi.softAP(db[kk::wifi_ssid], db[kk::wifi_pass]);

    web.begin(false, "test");
    web.onBuild(build);
    web.onUpdate(update);

    web.config.sliderTout = 100;
    web.config.requestTout = 0;
    web.config.updateTout = 2500;

    web.setPass(db[kk::admin_pass]);
}

void web_loop()
{
    web.tick();
}
