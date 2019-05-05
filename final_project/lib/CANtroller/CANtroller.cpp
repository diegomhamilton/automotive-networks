#include "CANtroller.h"

EventFlags shared_events;
Timer timer;

CANtroller::CANtroller(PinName rx)
{
    timer.start();
    this->can_rx = new InterruptIn(rx, PullUp);
    this->btm = new BitTimingModule((uint16_t)TQ_BITRATE, (uint8_t)TQ_PHASE1, (uint8_t)TQ_PHASE2, (uint8_t)SJW, can_rx);
    this->decoder = new Decoder();
    this->encoder = new Encoder();
}