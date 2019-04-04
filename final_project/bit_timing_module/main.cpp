/* Final project for Automotive Networks Course at CIn, UFPE.
 * Professor: Divanilson Campello.
 *
 * Bit Timing Module (BTM) for a CAN controller.
 *
 * Authors:
 *  Diego Hamilton
 *  Lucas Cavalcanti
 */

#include "mbed.h"

/* Macros */
#define min(a, b) (((a) < (b)) ? (a) : (b))

/* BTM parameters */
#define TQ_PHASE1   10                   // number of time quanta in phase segment 2
#define TQ_PHASE2   10                   // number of time quanta in phase segment 2
#define TQ_CLKPRE   1                   // prescaler for bit timing clock
#define SJW         5                   // maximum segment jump width

/* BTM Machine States */
typedef enum
{
    SYNC_ST,
    P1_ST,
    P2_ST,
    HS_ST
} btm_states_t;

/* Pins declaration */
Serial pc(USBTX, USBRX, 115200);
DigitalOut led1(LED1);
InterruptIn CAN_RX(USER_BUTTON);

/* Time management objects declaration */
Ticker btmTicker;
Ticker hsStimulusTicker;
// Timeout hsStimulusTimeout;

/* General functions declaration */

/* Interrupt service routines */
void btmTickerISR();
void fallingEdgeISR();
void hsEnable();

/* General variables declaration */
uint16_t btm_counter = 0;
bool tick = false;
btm_states_t curr_st = SYNC_ST;
bool falling_edge = false;
uint16_t tseg1, tseg2;
bool bus_idle = true;

int main()
{
    btmTicker.attach(&btmTickerISR, 0.5);
    CAN_RX.fall(&fallingEdgeISR);
    hsStimulusTicker.attach(&hsEnable, 10);

    while(true)
    {
        if(tick)
        {
            tick = false;

            pc.printf("%d, ", btm_counter);
            switch (curr_st)
            {                
                case SYNC_ST:
                    pc.printf("s\r\n");
                    btm_counter = 0;
                    tseg1 = TQ_PHASE1;
                    tseg2 = TQ_PHASE2;

                    if (falling_edge)
                    {
                        falling_edge = false;
                        pc.printf("SYNC OK\r\n");
                    }
                    // add writing point
                    curr_st = P1_ST;
                    break;
                case P1_ST:
                    pc.printf("p1\r\n");

                    if (falling_edge)
                    {
                        falling_edge = false;
                        int8_t e = min(SJW, btm_counter);
                        tseg1 += e;
                        pc.printf("SYNC ERROR 1 = %d, new segment size = %d \r\n", e, tseg1);
                    }

                    if(btm_counter == tseg1)
                    {
                        pc.printf("p1->p2\r\n");
                        // add sampling point
                        curr_st = P2_ST;
                    }
                    break;
                case P2_ST:
                    pc.printf("p2\r\n");
                    
                    if (falling_edge)
                    {
                        falling_edge = false;
                        int8_t e = min(SJW, abs((btm_counter - tseg1) - tseg2));
                        tseg2 = tseg2 - e;
                        pc.printf("SYNC ERROR 2 = %d, new segment size = %d \r\n", e, tseg2);
                    }

                    if(btm_counter == tseg1 + tseg2)
                    {
                        pc.printf("p2->sync\r\n");
                        curr_st = SYNC_ST;
                    }

                    break;
                case HS_ST:
                    pc.printf("s\r\n");
                    btm_counter = 0;

                    tseg1 = TQ_PHASE1;
                    tseg2 = TQ_PHASE2;
                    
                    falling_edge = false;
                    curr_st = P1_ST;
                    break;
                default:
                    pc.printf("!!! deu ruim !!!\r\n");
                    break;
            }
            btm_counter++;
        }
    }
}

void btmTickerISR()
{
    tick = true;
}

void fallingEdgeISR()
{
    falling_edge = true;
}

void hsEnable()
{
    bus_idle = !bus_idle;
    led1 = bus_idle;
}