#include "Decoder.h"

Decoder::Decoder()
{
    this->exe_fb.start(callback(this, &Decoder::execute_fb));
    this->exe_bdf.start(callback(this, &Decoder::execute_bdf));
}

void Decoder::execute_fb()
{
    while(true)
    {
        printf("read: %ld\r\n", timer.read_ms());
        shared_events.wait_any(READ_SIGNAL);
    }
}

void Decoder::execute_bdf()
{
    while(true)
    {
        printf("bdf: %ld\r\n", timer.read_ms());
        shared_events.wait_any(SP_SIGNAL);
        shared_events.set(READ_SIGNAL);
    }
}
