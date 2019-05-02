#ifndef DECODER_H
#define DECODER_H

#ifndef MBED_H
#define MBED_H
#include "mbed.h"
#endif

/* Frame Builder Machine States */
typedef enum
{
    RECEIVING_ST,
    ERROR_HANDLER_ST,
    /* RECEIVING states */
    IDLE_ST,
    SOF_ST,
    BSF_ST,
    GET_ID1_ST,
    EVAL,
    GET_ID2_ST,
    RES1_ST,
    RES2_ST,
    DATA_LEN_ST,
    GET_DATA_ST,
    CRC_COMP_ST,
    CRC_DEL_ST,
    ACK_ST,
    ACK_DEL_ST,
    EOF_ST,
    IFS_ST,
    /* ERROR_HANDLER states */
    ERROR_SIGNAL_ST,        // check error type, increment error counters
    RECBIT_COUNTER_ST,      // transmit error frame
} decoder_states_t;

/* Bit DeStuffing Machine States */
typedef enum
{
    READ_ST,
    RECEIVE_ST,
    ERROR_DETECTOR_ST,
    RAISE_ERROR_ST,
    RESET_ST
} bsf_states_t;

/* Global variables */
extern EventFlags shared_events;

class Decoder
{
    public:
    private:
};

#endif