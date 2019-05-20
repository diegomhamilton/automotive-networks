#include "BitTiming.h"

bool falling_edge = false;              // defined @ BitTiming.cpp

uint8_t btm_exe(bool bus_idle)
{
    static uint16_t btm_counter = 0;
    static uint16_t tseg1, tseg2;
    static uint8_t st_debug = 0;
    static btm_states_t curr_st;
    static int8_t e = 0;
    static bool writing_point = false;
    static bool sampling_point = false;

    writing_point = false;
    sampling_point = false;

    if (bus_idle && falling_edge)
    {
//        printf("hs\r\n");
        curr_st = HS_ST;
    }

    switch (curr_st)
    {                
        case SYNC_ST:
            // Sync segment
//            printf("sync\r\n");
            st_debug = 0;
            
            btm_counter = 0;
            tseg1 = TQ_PHASE1;
            tseg2 = TQ_PHASE2;

            falling_edge = false;
            writing_point = true;
            
            curr_st = P1_ST;
            break;
        case P1_ST:
            // Phase Segment 1
            st_debug = 1;
//            printf("p1\r\n");
            
            if (falling_edge && tseg1 == TQ_PHASE1)
            {
                falling_edge = false;
                e = min(SJW, btm_counter);
                tseg1 += e;
            }

            if(btm_counter == tseg1)
            {
                sampling_point = true;
                curr_st = P2_ST;
            }
            break;
        case P2_ST:
            // Phase Segment 2
            st_debug = 2;
//            printf("p2\r\n");
            
            if (falling_edge && tseg2 == TQ_PHASE2)
            {
                falling_edge = false;
                e = min(SJW, abs((btm_counter - tseg1) - tseg2));
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

            tseg1 = TQ_PHASE1;
            tseg2 = TQ_PHASE2;
            
            falling_edge = false;
            curr_st = P1_ST;
            break;
        default:
            printf("Unrecognized state\r\n");
            break;
    }
    btm_counter++;

    return ((writing_point) | (sampling_point << 1));
}