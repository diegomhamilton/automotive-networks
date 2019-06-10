#include "Encoder.h"

Encoder::Encoder()
{
    this->exe.start(callback(this, &Encoder::execute));
}

void Encoder::execute()
{
    while(true)
    {
        printf("wp: %ld\r\n", timer.read_ms());
        shared_events.wait_any(WP_SIGNAL);
    }
}