#include <Global.h>
#include <US_custom.h>

#ifdef US
bool Trig_flag = false;

NewPing US_sensor(US_TRIG_pin, US_ECHO_pin, US_max_dist);

void trig_us()
{
    if (!Play_flag)
    {
        if (!dist_lag)
        {
            old_millis = millis();
            dist = US_sensor.ping_cm();
            dist_lag = true;

            СДЕЛАТЬ ОБРАБОТКУ ОБРЫВА ДАТЧИКА
            // Serial.println(dist);
        }

        if (dist < _trig_dist && !Trig_flag)
        {
            global_timer = millis();
            Trig_flag = true;
        }
        else if (dist < _trig_dist && Trig_flag && millis() - global_timer >= _trig_lag)
        {
            Trig_flag = false;
            Play_flag = true;
        }
        else if (dist > _trig_dist && Trig_flag && millis() - global_timer >= _trig_lag)
        {
            Trig_flag = false;
        }

        if (dist_lag && millis() - old_millis >= meas_lag)
        {
            dist_lag = false;
        }
    }
}

void trig_param_change(char param)
{
    echo("Send value");
    global_timer = millis();
    bool state = false;
    uint16_t value = 65535;
    while (!state)
    {
#ifdef UART
        if (Serial.available())
        {
            value = Serial.parseInt();
        }
#endif

#ifdef BT
        else if (BT_message.length() > 0 && BT_message != "dist" && BT_message != "lag")
        {
            value = BT_message.toInt();
        }
#endif

        if (millis() - global_timer >= 3000 || value != 65535)
        {
            state = true;
        }
    }

    if (value == 65535)
    {
        echo("Value not received");
        return;
    }

    if (param == 'L')
    {
        _trig_lag = value;
    }

    if (param == 'D')
    {
        if (value > US_max_dist) value = US_max_dist;
        if (value < 20) value = 20;

        _trig_dist = value;
    }
}
#endif
