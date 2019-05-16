#ifndef BTM_H
#define BTM_H

#include "mbed.h"

/* Macros */
#define min(a, b) (((a) < (b)) ? (a) : (b))

/* BTM Machine States */
typedef enum
{
    SYNC_ST,
    P1_ST,
    P2_ST,
    HS_ST
} btm_states_t;

extern bool falling_edge;               // defined @ BitTiming.cpp

/* BTM parameters */
#define TQ_BITRATE  000010.0            // bitrate of network, TQ_PERIOD = 1/TQ_BITRATE
#define TQ_PHASE1   10                  // number of time quanta in phase segment 2
#define TQ_PHASE2   10                  // number of time quanta in phase segment 2
#define SJW         5                   // maximum segment jump width

/* BTM execute return parameters*/
#define NOTHING     0
#define WRITING_P   0x01
#define SAMPLING_P  0x10

uint8_t btm_exe(bool bus_idle);

#endif