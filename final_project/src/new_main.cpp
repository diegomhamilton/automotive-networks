#include "mbed.h"
#include "user_functions.h"
#include "BitTiming.h"
#include "CANtroller.h"

#define IGNORE_ERROR_FRAME true         // used for closed-loop tests

InterruptIn transmit(USER_BUTTON, PullUp);
InterruptIn can_rx(D9, PullNone);
DigitalOut  can_tx(D8);
DigitalOut led(LED1);


bool tick = false;
bool bus_idle = true;
bool msg_available = true;
int ReadBuffer[200] = {0};
// bool recessive_error_state = false;
// bool bus_off_state = false;
bool error_state = false;

/* Functions declaration*/
void tickISR();
void transmitISR();
void fallingEdgeISR();

void main() {
    can_tx = 1;				// start bus in recessive state
    uint8_t btm_command = 0;
    Ticker tq_clock;
    Timer t;
    unsigned long int ts1 = 0, ts2 = 0;

    int Bit = 0;
    int curr_rx_bit = 0;
    int curr_tx_bit = 0, last_tx_frame_pos = 0;
    int curr_err_bit = 0;
    int Cat = 0;
    int ErrorFlag = 0;
    bool waiting_ifs = false;

	char Bar[] = {"01100111001000010001010101010101010101010101010101010101010101010101010101010101010000010000101000110111111111110110011100100000110110101010101010101010101010101010101010101100111001001101011111111111011001110010000010111010101010101010101010100100101110000011101111111111101100111001000001000011001011010101101111111111101000100100111110000010000011111001000010001010101010101010101010101010101010101010101010101010101010101010111101111101010110111111111110110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000011100011010000001111111111101100111001000010001010101010101010101010101010101010101010101010101010101010101010000010000101000111000000111111111110100010010011111000001000001111100101000001010111011111000011011111111111011001110010000011101010101010101010101010101010101010101010101010100001001001011101011111111-1"};
	int* Bor = (int *) calloc(strlen(Bar), sizeof(int));
	FillInt(Bor, Bar, 0);

	struct Frame Fr;
	ReadBuffer[199] = -1;
	//Entrada do Decoder, termina em -1.

	Fr.ID1 = -1;
	Fr.ID2 = -1;
	StrcpyIsGross(Fr.Data, "R");
	Fr.DLC = -1;
	Fr.IDE = -1;
	Fr.RTR = -1;
	Fr.SRR = -1;
	Sending = true;
	
    t.start();
    can_rx.fall(&fallingEdgeISR);
    transmit.fall(&transmitISR);
    tq_clock.attach(&tickISR, 1.0/((double)TQ_BITRATE*(TQ_PHASE1 + TQ_PHASE2)));

    while(true)
    {
    if (tick)
    {
        tick = false;
        btm_command = btm_exe(bus_idle);

        switch (btm_command)
        {
            case WRITING_P:
                if (WhenACK) {
                    can_tx = 0;
                }
                else if (error_state && !(IGNORE_ERROR_FRAME)) {
                    sent_Bit = ActiveErrorFrame[curr_err_bit];

                    if (sent_Bit != -1) {
                        can_tx = sent_Bit;
                        if (DEBUG_CODE) printf("%d: err %d\r\n", curr_err_bit, sent_Bit);
                        curr_err_bit += 1;
                    }
                    else {
                        sent_Bit = 1;
                        can_tx = sent_Bit;
                        curr_tx_bit = last_tx_frame_pos;
                    }
                }
                else if (Sending) {
                    sent_Bit = Bor[curr_tx_bit]; 
                    if (sent_Bit != -1) {
                        if (bus_idle && sent_Bit == 0)
                            last_tx_frame_pos = curr_tx_bit;
                        
                        can_tx = sent_Bit;
                        if (DEBUG_CODE) printf("%d: sent %d\r\n", curr_tx_bit, sent_Bit);
                        curr_tx_bit += 1;
                    }
                    else {
                        sent_Bit = 1;
                        can_tx = sent_Bit;         // force recessive bit if not transmitting
                        curr_tx_bit = 0;
                        Sending = false;
                    }
                }
                break;
            case SAMPLING_P:
                Bit = can_rx.read();

                if (error_state || waiting_ifs) {
                    Cat = error_step(Bit, ErrorFlag);
                    ErrorFlag = 0;

                    if (DEBUG_CODE) printf("error %d, cat %d\r\n\n", Bit, Cat);
                    
                    if (Cat == -1) {
                    	waiting_ifs = true;
                    }
                    else if (Cat == -2)
                    {
                        error_state = false;
                        waiting_ifs = false;
                        bus_idle = true;
                    }
                    else if (Cat == 7)
                    {
                        /* OVERLOAD ERROR*/
                        printf("OVERLOAD ERROR\r\n");
                        ErrorFlag = 1;
                    }
                }
                else {
                    ts1 = t.read_us();
                    Cat = decoder_step(&Fr, Bit);
                    ts2 = t.read_us();
                    curr_rx_bit += 1;
                    if (DEBUG_CODE) printf("rcvd %d, cat %d\r\n\n", Bit, Cat);

                    if (Cat > 1) {
                        error_handler_exe(Cat);
                        ErrorFlag = 1;
                        error_state = true;
                    }
                    else if (bus_idle && (Cat == 0)) {
                        bus_idle = false;
                    }
                    else if (Cat == -2) {
                        printf("Last Frame Decoded:\r\n");
                        printf("ID1 = 0x%x\r\n", Fr.ID1);
                        printf("ID2 = 0x%x\r\n", Fr.ID2);
                        printf("RTR = %d\r\n", Fr.RTR);
                        printf("IDE = %d\r\n", Fr.IDE);
                        printf("SRR = %d\r\n", Fr.SRR);
                        printf("DLC = %d\r\n", Fr.DLC);
                        printf("DATA = ");
                        for (int i = 0; i < 2 * Fr.DLC; i += 1) {
                            printf("%c", Fr.Data[i]);
                        }
                        // printf("\r\n");
                        // printf("CRC = ");
                        // for (int i = 0; i < sizeof(Fr.user_CRC)/sizeof(int); i += 1) {
                        //     printf("%d", Fr.user_CRC[i]);
                        // }
                        printf("\r\n\n");
                        curr_rx_bit = 0;
                        bus_idle = true;
                        error_state = false;
                    }
                }
                
                break;
            case NOTHING:
            	if (msg_available && bus_idle)
            	{
            		// if (dummy) {
	            	// 	ts1 = t.read_us();
	            	// 	frame_interpreter(Fr, Bor);
	            	// 	ts2 = t.read_us();
	            	// 	printf("(%d) Encoded frame: ", ts2 - ts1);
	            	// 	for(int b = 0; true; b += 1) {
	            	// 		printf("%d", Bor[b]);
	            	// 		if (Bor[b] == -1)
	        		// 			break;
	            	// 	}
	            	// 	printf("\r\n");
            		// }

                    msg_available = false;
                    Sending = true;
            	}
//                printf(".\r\n");
                break;
            case (WRITING_P | SAMPLING_P):
                printf("delayed\r\n");
                break;
            default:
                printf("ERROR: BTM RETURNS %d !!!!r\n\n\n\n",btm_command);
                break;
        }
    }
    }
}

void tickISR() {
    tick = true;
}

void transmitISR() {
    msg_available = true;
}

void fallingEdgeISR() {
    falling_edge = true;
}