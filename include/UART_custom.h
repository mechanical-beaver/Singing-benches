#pragma once

#include <Arduino.h>
#include <Config.h>

#ifdef UART
#define UART_BAUDRATE 115200

void uart_init();
void error404(String error_massage = "");

#define UART_INIT() uart_init()
#define ERR(err) error404(err)
#define UART_PRINT(msg) Serial.println(msg)
#else
#define UART_INIT() ((void)0)
#define ERR(err) while (1)
#define UART_PRINT(msg) ((void)0)
#endif
