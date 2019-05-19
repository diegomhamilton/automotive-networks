#include "mbed.h"
#include "BitTiming.h"
#include "CANtroller.h"
#include "user_functions.h"

InterruptIn can_rx(USER_BUTTON);
DigitalOut led(LED1);

/* Functions declaration*/
void tickISR();
void fallingEdgeISR();

bool tick = false;
bool bus_idle = true;
int ReadBuffer[200] = {0};

void main()
{
    uint8_t btm_command = 0;
    Ticker tq_clock;
    Timer t;
    unsigned long int ts1 = 0, ts2 = 0;

    int BovineCounter = 0;
    int IsStuff = 0;
    int Bit = 0;
    int curr_bit = 0;
	int DeStuffSweep = 0;
	int Cat = 0;
	struct Frame Fr;
	ReadBuffer[199] = -1;
	//Entrada do Decoder, termina em -1.
	char Bar[] = { "011001110010000011111001010101010101010101010101010101010101010101010101010101011001100111011011111111-1" };
	int* Bor = (int *) calloc(strlen(Bar), sizeof(int));
	FillInt(Bor, Bar, 0);

	Fr.ID1 = -1;
	Fr.ID2 = -1;
	StrcpyIsGross(Fr.Data, "R");
	Fr.DLC = -1;
	Fr.IDE = -1;
	Fr.RTR = -1;
	Fr.SRR = -1;


    t.start();
    can_rx.fall(&fallingEdgeISR);
    tq_clock.attach(&tickISR, 1.0/((float)TQ_BITRATE*(TQ_PHASE1 + TQ_PHASE2)));

    while(true)
    {
    if (tick)
    {
        tick = false;
        btm_command = btm_exe(bus_idle);

        switch (btm_command)
        {
            case WRITING_P:
//                printf("w, %ld\r\n", t.read_ms());
                break;
            case SAMPLING_P:
                if (Bor[curr_bit] != -1) {
                    ts1 = t.read_us();
                    Cat = decoder_step(&Fr, Bor[curr_bit]);
                    ts2 = t.read_us();
                    curr_bit += 1;
//                    printf("s, sent = %d, result = %d, bitcnt= %d,t=%ld\r\n\n", Bor[curr_bit - 1], Cat, curr_bit, ts2 - ts1);
                }
                else {
                    printf("Decoded:\r\n");
					printf("ID1 = 0x%x\r\n", Fr.ID1);
					printf("ID2 = 0x%x\r\n", Fr.ID2);
					printf("RTR = %d\r\n", Fr.RTR);
					printf("IDE = %d\r\n", Fr.IDE);
					printf("SRR = %d\r\n", Fr.SRR);
					printf("DLC = %d\r\n", Fr.DLC);
				    curr_bit = 0;
                }
                break;
            case NOTHING:
//                printf(".\r\n");
                break;
            case WRITING_P | SAMPLING_P:
                printf("delayed\r\n");
                break;
            default:
                printf("%d, %d !!!!!\r\n\n\n\n",btm_command, SAMPLING_P);
                break;
        }
    }
    }
}

void tickISR()
{
    tick = true;
}

void fallingEdgeISR()
{
    falling_edge = true;
}