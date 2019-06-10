/* Final project for Automotive Networks Course at CIn, UFPE.
 *
 * Professor: Divanilson Campello.
 * Teaching Assistants:
 *  Paulo Freitas de Araújo Filho
 *  Saulo Vinicius Ferreira Barreto
 *  Nathan Martins Freire 
 *
 * CAN controller module.
 *
 * Authors:
 *  Diego Hamilton
 *  Lucas Cavalcanti
 * 
 * Microcontroller: NUCLEO-STM32F303RE
 * Developed with the ARM Mbed IDE. 
 */

#ifndef CAN_H
#define CAN_H

#include "mbed.h"
#include "EventDefs.h"
#include "BitTiming.h"
#include "Decoder.h"
#include "Encoder.h"

/* Global variables */

/* BTM parameters */
#define TQ_BITRATE  000010.0            // bitrate of network, TQ_PERIOD = 1/TQ_BITRATE
#define TQ_PHASE1   10                  // number of time quanta in phase segment 2
#define TQ_PHASE2   10                  // number of time quanta in phase segment 2
#define SJW         5                   // maximum segment jump width

class CANtroller {
    public:
        /* Public methods */
        CANtroller(PinName rx);
        void updateIdle(bool idle);
        /* Public atributes */
        BitTimingModule* btm;
        Decoder* decoder;
        Encoder* encoder;
        InterruptIn* can_rx;
    private:
        /* Private methods */
        /* Private atributes */
        bool idle;
};

#endif
