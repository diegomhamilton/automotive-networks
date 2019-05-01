#include "BitTiming.h"

BitTimingModule::BitTimingModule(uint16_t tq_bit_rate, uint8_t tq_phase1, uint8_t tq_phase2, \
                                    uint8_t sjw, InterruptIn* RxPin)
{
    this->curr_st = SYNC_ST;
    this->curr_st = curr_st;
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
    this->clock.attach(this, &BitTimingModule::clockISR, 1/(this->tq_bit_rate));

    while(true)
    {
        sampling_point = false;
        writing_point = false;

        switch (curr_st)
        {                
            case SYNC_ST:
                // Sync segment
                st_debug = 0;
                
                btm_counter = 0;
                tseg1 = this->tq_phase1;
                tseg2 = this->tq_phase2;

                if (falling_edge)
                {
                    falling_edge = false;
                }
                // add writing point
                writing_point = true;
                curr_st = P1_ST;
                break;
            case P1_ST:
                // Phase Segment 1
                st_debug = 1;

                if (falling_edge && tseg1 == this->tq_phase1)
                {
                    falling_edge = false;
                    int8_t e = min(this->sjw, btm_counter);
                    tseg1 += e;
                }

                if(btm_counter == tseg1)
                {
                    // pc.printf("p1->p2\r\n");
                    // add sampling point
                    sampling_point = true;
                    curr_st = P2_ST;
                }
                break;
            case P2_ST:
                // Phase Segment 2
                st_debug = 2;
                
                if (falling_edge && tseg2 == this->tq_phase2)
                {
                    falling_edge = false;
                    int8_t e = min(this->sjw, abs((btm_counter - tseg1) - tseg2));
                    tseg2 = tseg2 - e;
                }

                if(btm_counter == tseg1 + tseg2)
                {
                    curr_st = SYNC_ST;
                }

                break;
            case HS_ST:
                // Hard Sync operations
                st_debug = 0;
                btm_counter = 0;

                tseg1 = this->tq_phase1;
                tseg2 = this->tq_phase2;
                
                falling_edge = false;
                curr_st = P1_ST;
                break;
            default:
                printf("Unrecognized state\r\n");
                break;
        }
        btm_counter++;

        /* Plot parameters in Arduino IDE Serial Plotter format */
        for(int i = 0; i < 6; i++)
            printf("%d %d %d\n", st_debug, writing_point-2, sampling_point-4);
        
        this->exe.signal_wait(TQ_SIGNAL);
    }

}

void BitTimingModule::clockISR()
{
    this->exe.signal_set(TQ_SIGNAL);
}

void BitTimingModule::fallingEdgeISR()
{
    falling_edge = true;

    if (this->idle)
        curr_st = HS_ST;
}