#include "BitTiming.h"

BitTimingModule::BitTimingModule(uint16_t tq_bit_rate, uint8_t tq_phase1, uint8_t tq_phase2, \
                                    uint8_t sjw, InterruptIn* RxPin)
{
    this->curr_st = SYNC_ST;
    this->tq_bit_rate = tq_bit_rate;
    this->tq_phase1 = tq_phase1;
    this->tq_phase2 = tq_phase2;
    this->sjw = sjw;
    this->can_rx = RxPin;
    this->idle = false;
    this->falling_edge = false;

    this->can_rx->fall(this, &BitTimingModule::fallingEdgeISR);
    this->exe.start(this, &BitTimingModule::execute);
}

void BitTimingModule::updateIdle(bool idle)
{
    this->idle = idle;
}

void BitTimingModule::execute()
{
    uint16_t btm_counter = 0;
    uint16_t tseg1, tseg2;
    uint8_t st_debug = 0;
    this->clock.attach(callback(this, &BitTimingModule::clockISR), 1.0/((float)this->tq_bit_rate));

    while(true)
    {
        sampling_point = false;
        writing_point = false;

        switch (this->curr_st)
        {                
            case SYNC_ST:
                // Sync segment
                st_debug = 0;
                
                btm_counter = 0;
                tseg1 = this->tq_phase1;
                tseg2 = this->tq_phase2;

                if (this->falling_edge)
                {
                    this->falling_edge = false;
                }
                // add writing point
                writing_point = true;
                this->curr_st = P1_ST;
                break;
            case P1_ST:
                // Phase Segment 1
                st_debug = 1;

                if (this->falling_edge && tseg1 == this->tq_phase1)
                {
                    this->falling_edge = false;
                    int8_t e = min(this->sjw, btm_counter);
                    tseg1 += e;
                }

                if(btm_counter == tseg1)
                {
                    // pc.printf("p1->p2\r\n");
                    // add sampling point
                    sampling_point = true;
                    this->curr_st = P2_ST;
                }
                break;
            case P2_ST:
                // Phase Segment 2
                st_debug = 2;
                
                if (this->falling_edge && tseg2 == this->tq_phase2)
                {
                    this->falling_edge = false;
                    int8_t e = min(this->sjw, abs((btm_counter - tseg1) - tseg2));
                    tseg2 = tseg2 - e;
                }

                if(btm_counter == tseg1 + tseg2)
                {
                    this->curr_st = SYNC_ST;
                }

                break;
            case HS_ST:
                // Hard Sync operations
                st_debug = 0;
                btm_counter = 0;

                tseg1 = this->tq_phase1;
                tseg2 = this->tq_phase2;
                
                this->falling_edge = false;
                this->curr_st = P1_ST;
                break;
            default:
                printf("Unrecognized state\r\n");
                break;
        }
        btm_counter++;

        /* Plot parameters in Arduino IDE Serial Plotter format */
        for(int i = 0; i < 6; i++)
            printf("%d %d %d\n", st_debug, writing_point-2, sampling_point-4);
            
        shared_events.wait_any(TQ_SIGNAL);
    }

}

void BitTimingModule::clockISR()
{
    shared_events.set(TQ_SIGNAL);
}

void BitTimingModule::fallingEdgeISR()
{
    this->falling_edge = true;

    if (this->idle)
        this->curr_st = HS_ST;
}