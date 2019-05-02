#include "mbed.h"
#include "BitTiming.h"

/* BTM parameters */
#define TQ_BITRATE  000010.0            // bitrate of network, TQ_PERIOD = 1/TQ_BITRATE
#define TQ_PHASE1   10                  // number of time quanta in phase segment 2
#define TQ_PHASE2   10                  // number of time quanta in phase segment 2
#define SJW         5                   // maximum segment jump width

bool writing_point = false, sampling_point = false;
EventFlags shared_events;
InterruptIn CAN_RX(USER_BUTTON, PullUp);
DigitalOut led1(LED1);

void main()
{
    BitTimingModule BTM((uint16_t)TQ_BITRATE, TQ_PHASE1, TQ_PHASE2, SJW, &CAN_RX);

    while(true)
    {
        led1 = !(led1);
        wait(2.0f);
    }
}