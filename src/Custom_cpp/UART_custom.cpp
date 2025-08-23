#include <Global.h>
#include <UART_custom.h>

#include "HardwareSerial.h"

#ifdef UART
void uart_init()
{
    Serial.begin(UART_BAUDRATE);
    if (!Serial) ERR();
}

void uart_switch(bool stat)
{
    if (stat)
    {
        uart_init();
        Serial.print("Started UART");
    }
    else
    {
        Serial.println("Stoping UART");
        Serial.end();
    }
}

void error404(String error_massage)
{
    if (error_massage != "")
    {
        UART_PRINT(error_massage);
    }
    LED_ON(_cols.err);
    while (true);
}
#endif
