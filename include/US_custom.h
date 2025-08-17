#pragma once

#include <Arduino.h>
#include <Config.h>
#include <NewPing.h>

#define US_max_dist 300

#ifdef US
extern NewPing US_sensor;
extern bool Trig_flag;

void trig_us();
void trig_param_change(char param);
#define TRIG_CHANGE(par) trig_param_change(par)
#else
#define TRIG_CHANGE(par) ((void)0)
#endif
