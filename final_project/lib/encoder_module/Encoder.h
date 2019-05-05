/* Final project for Automotive Networks Course at CIn, UFPE.
 *
 * Professor: Divanilson Campello.
 * Teaching Assistants:
 *  Paulo Freitas de Araújo Filho
 *  Saulo Vinicius Ferreira Barreto
 *  Nathan Martins Freire 
 *
 * Encoder Module for a CAN controller.
 *
 * Authors:
 *  Diego Hamilton
 *  Lucas Cavalcanti
 * 
 * Microcontroller: NUCLEO-STM32F303RE
 * Developed with the ARM Mbed IDE. 
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "mbed.h"
#include "EventDefs.h"


/* Frame Interpreter Machine States */
typedef enum
{
    TRANSMITTING_ST,
    ERROR_HANDLER_ENC_ST,
    IDLE_ENC_ST,
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
#ifndef SHARED_EVENTS
#define SHARED_EVENTS
extern Timer timer; 
extern EventFlags shared_events;
#endif

class Encoder
{
    public:
        Encoder();
    private:
        /* Private methods */
        void execute();                     // state machine of Frame Interpreter Module
        /* Private atributes */
        Thread exe;                         // execution thread
};

#endif