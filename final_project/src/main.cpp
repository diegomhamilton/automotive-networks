#include "mbed.h"
#include "BitTiming.h"

InterruptIn can_rx(USER_BUTTON);
DigitalOut led(LED1);

/* Functions declaration*/
void tickISR();
void fallingEdgeISR();

bool tick = false;
bool bus_idle = false;

void main()
{
    uint8_t btm_command = 0;
    Ticker tq_clock;
    Timer t;

    t.start();
    can_rx.fall(&fallingEdgeISR);
    tq_clock.attach(&tickISR, 1.0/((float)TQ_BITRATE*(TQ_PHASE1 + TQ_PHASE2)));

    while(true)
    {
    if (tick)
    {
        tick = false;
        btm_command = btm_exe(bus_idle);

        switch (btm_command)
        {
            case WRITING_P:
                printf("w, %ld\r\n", t.read_ms());
                break;
            case SAMPLING_P:
                printf("s, %ld\r\n", t.read_ms());
                break;
            default:
                printf(".\r\n");
                break;
        }
    }
    }
}

void tickISR()
{
    tick = true;
}

void fallingEdgeISR()
{
    falling_edge = true;
}