#ifndef ENCODER_H
#define ENCODER_H

#ifndef MBED_H
#define MBED_H
#include "mbed.h"
#endif

/* Frame Interpreter Machine States */
typedef enum
{
    TRANSMITTING_ST,
    ERROR_HANDLER_ST,
    IDLE_ST,
    /* TRANSMITTING states */
    FRAME_CHECK_ST,
    CRC_ST,                 // calculate cyclic redundancy check
    BSF_ST,                 // add stuff bits
    TRANSMIT_ST,
    /* ERROR_HANDLER states */
    ERROR_CHECK_ST,         // check error type, increment error counters
    TRANSMIT_ERROR_ST,      // transmit error frame
} encoder_states_t;

/* Global variables */
extern EventFlags shared_events;

class Encoder
{
    public:

    private:
        
};

#endif