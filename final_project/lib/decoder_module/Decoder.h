/* Final project for Automotive Networks Course at CIn, UFPE.
 *
 * Professor: Divanilson Campello.
 * Teaching Assistants:
 *  Paulo Freitas de Araújo Filho
 *  Saulo Vinicius Ferreira Barreto
 *  Nathan Martins Freire 
 *
 * Decoder Module for a CAN controller.
 *
 * Authors:
 *  Diego Hamilton
 *  Lucas Cavalcanti
 * 
 * Microcontroller: NUCLEO-STM32F303RE
 * Developed with the ARM Mbed IDE. 
 */

#ifndef DECODER_H
#define DECODER_H

#include "mbed.h"
#include "EventDefs.h"

/* Frame Builder Machine States */
typedef enum
{
    RECEIVING_ST,
    ERROR_HANDLER_DEC_ST,
    /* RECEIVING states */
    IDLE_DEC_ST,
    SOF_ST,
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
#ifndef SHARED_EVENTS
#define SHARED_EVENTS
extern Timer timer; 
extern EventFlags shared_events;
#endif

class Decoder
{
    public:
        Decoder();
    private:
        /* Private methods */
        void execute_bdf();                     // state machine of Bit De-Stuffing Module
        void execute_fb();                      // state machine of Frame Builder Module
        /* Private atributes */
        Thread exe_bdf;                         // Bit De-Stuffing execution thread
        Thread exe_fb;                          // Frame Builder execution thread
};

#endif