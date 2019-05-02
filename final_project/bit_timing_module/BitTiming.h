/* Final project for Automotive Networks Course at CIn, UFPE.
 *
 * Professor: Divanilson Campello.
 * Teaching Assistants:
 *  Paulo Freitas de Araújo Filho
 *  Saulo Vinicius Ferreira Barreto
 *  Nathan Martins Freire 
 *
 * Bit Timing Module (BTM) for a CAN controller.
 *
 * Authors:
 *  Diego Hamilton
 *  Lucas Cavalcanti
 * 
 * Microcontroller: NUCLEO-STM32F303RE
 * Developed with the ARM Mbed IDE. 
 */

#ifndef BTM_H
#define BTM_H

#include "mbed.h"

#define TQ_SIGNAL 0x01

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

/* Global variables */
extern bool writing_point, sampling_point;
extern EventFlags shared_events;

class BitTimingModule {
    public:
        /* Public methods */
        BitTimingModule(uint16_t tq_bit_rate, uint8_t tq_phase1, uint8_t tq_phase2, uint8_t sjw, InterruptIn* RxPin);
        void updateIdle(bool idle);

        /* Public atributes */
        InterruptIn* can_rx;
    private:
        /* Private methods */
        void execute();                     // state machine of Bit Timing Module
        void clockISR();                    // implements TQ clock
        void fallingEdgeISR();              // detect sync (soft or hard, depending on state of the bus)
        /* Private atributes */
        Ticker clock;
        Thread exe;
        bool idle;
        bool falling_edge;
        btm_states_t curr_st;
        uint16_t tq_bit_rate;
        uint8_t tq_phase1;
        uint8_t tq_phase2;
        uint8_t sjw;
};

#endif