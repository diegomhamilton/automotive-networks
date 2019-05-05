#include "mbed.h"
#include "CANtroller.h"

CANtroller can(USER_BUTTON);
DigitalOut led(LED1);

void main()
{
    while(true)
    {
        led = !led;
        wait(2.0f);
    }
}