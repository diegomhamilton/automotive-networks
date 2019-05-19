#include "CANtroller.h"

/*
States:
0 = SoF -> Omitted
1 = GetID1 -> 11bits
2 = EVAL1 -> 1Bit
- = Regular Data Frame -> Type 0
3 = EVAL2 -> 1 Bit
- = Regular Remote Frame -> Type 1 
4 = GetID2 -> 18Bits 
5 = EVAL3 -> 1 Bit -> Form Error if EVAL1 = 0
- = Extended Data Frame -> Type 2
- = Extended Remote Frame -> Type 3
6 = Res_Norm -> 1 Bit // Type 0 || Type 1 -> Expect 1, no error
7 = Res_Ext -> 2 Bits // Type 2 || Type 3 -> Expect 11, no error
8 = Get DLC -> 4 Bits
9 = Get Data -> DLC * 8 Bits // Type 0 || Type 2
10 = UnCRC -> 15 Bits -> CRC Error if UNCRC = -1
11 = CRCDel -> 1 Bit -> Form Error
12 = ACK -> 1 Bit -> Ack Error if transmitting & Ack = 1;
13 = Rest -> 8 Bits -> AckDel + EoF, Form Error
*/

int LastBit = 0;
int RepBitCounter = 0;
bool Sending = false;
bool repeat_frame = false;
int sent_Bit = 1;

int decoder_step(struct Frame* Fr, int Bit) {
    // Execution time ~= 10us
    static int BufferPos = 0;
    static int ReadBuffer[200] = {0};
    static int CRCBuild[200] = { 0 };
    static int CRCBuildSize = 0;
    static int IFS = 0;
    static int State = 0;
    static int StateSize = 0;
    static int DecType;
    
    int ErrorState = Bit; // 3 for stuff error, 4 for form error, 5 for CRC error, 6 for Ack Error, -1 = Frame End, -2 = IFS End.
	int Auxil;
	
    printf("st = %d, sz = %d\r\n", State, StateSize);

    if (bus_idle == true) {
        printf("initializing\r\n");
        //bus_idle, reseta tudo e prepara pra ler frame.
        if (Bit == '0')
        {
            LastBit = 0;
            BufferPos = 0;
            bus_idle = false;
            State = 1;
            Bit = bit_destf(0);
            StateSize = 11;
            CRCBuildSize = 1;
        }
    }
    else {
            //Destuff o bit se estiver antes do Ack.
            if (State < 11)
                Bit = bit_destf(Bit);

            //Destuff retorna -1 se o bit for um stuffing, -2 se for bitstuff error.
            if (Bit > -1)
            {
                if (Sending && (Bit != sent_Bit)) {
                    if ((State == 2 || State == 4 || State == 12)) {
                        //ACK
                        if (State == 12) {
                            ErrorState = 6;
                        }
                        else {
                            ErrorState = -4;
                        }
                        //Fase de Arbitação
                        repeat_frame = true;
                    }
                    else {
                        ErrorState = 2;
                    }

                    return ErrorState;
                }
                ReadBuffer[BufferPos] = Bit;
                BufferPos++;
                StateSize--;
                CRCBuild[CRCBuildSize] = Bit;
                CRCBuildSize++;
                ErrorState = Bit;
            }
            else if (Bit == -2) {
                ErrorState = 3;
            }

            //StateSize == 0 acontece quando acaba de ler x bits de um estado, pode ser unit�rio.
            if (StateSize == 0)
            {
                switch (State)
                {
                case 1: //ID1, SoF � levemente ignorado pois � levado em considera��o quando bus_idle = 1.
                    Fr->ID1 = BufferEatInt(ReadBuffer, 11);
                    FlushFrame(ReadBuffer);
                    BufferPos = 0;
                    State = 2;
                    StateSize = 1;
                    break;
                case 2: //RTR, se a frame for extendida, troca RTR de lugar depois.
                    Fr->RTR = BufferEatInt(ReadBuffer, 1);
                    BufferPos = 0;
                    State = 3;
                    StateSize = 1;
                    break;
                case 3: //IDE, troca o lugar do SRR = RTR se IDE ==1, FORM ERROR (4) se SRR == 0 e IDE == 1.
                    Fr->IDE = BufferEatInt(ReadBuffer, 1);
                    if (Fr->IDE == 1)
                    {
                        State = 4;
                        StateSize = 18;
                        Fr->SRR = Fr->RTR;
                        if (Fr->SRR == 0)
                            ErrorState = 4;
                    }
                    else
                    {
                        State = 6;
                        StateSize = 1;
                    }
                    BufferPos = 0;
                    break;
                case 4: //ID2
                    Fr->ID2 = BufferEatInt(ReadBuffer, 18);
                    BufferPos = 0;
                    State = 5;
                    StateSize = 1;
                    break;
                case 5: //RTR da frame extendida.
                    Fr->RTR = BufferEatInt(ReadBuffer, 1);
                    BufferPos = 0;
                    State = 7;
                    StateSize = 2;
                    break;
                case 6: //1Bit reservado
                    State = 8;
                    StateSize = 4;
                    BufferPos = 0;
                    break;
                case 7: //2 Bits reservados, quantidade de bits a ler determinada em estado anterior, n�o acontece form error se os bits reservados estiverem errados. 
                    State = 8;
                    StateSize = 4;
                    BufferPos = 0;
                    break;
                case 8: //DLC, se for igual a 0, ele pula o DATA.
                    Fr->DLC = BufferEatInt(ReadBuffer, 4);
                    if (Fr->RTR != 0 || Fr->DLC == 0) 
                    {
                        State = 10;
                        StateSize = 15;
                    }
                    else
                    {
                        State = 9;
                        StateSize = (Fr->DLC) * 8;
                    }
                    BufferPos = 0;
                    break;
                case 9: //DATA
                    BufferEatData(ReadBuffer, Fr->Data, Fr->DLC);
                    State = 10;
                    StateSize = 15;
                    BufferPos = 0;
                    break;
                case 10: //CRC, meu beb�.
                    CRCBuild[CRCBuildSize] = -1;
                    ErrorState = UnCRC(CRCBuild);
                    if (ErrorState != 0)
                        ErrorState = 3;
                    else
                        ErrorState = Bit;

                    State = 11;
                    StateSize = 1;
                    BufferPos = 0;
                    break;
                case 11: //CRC Del, FORM ERROR se for 0?
                    State = 12;
                    StateSize = 1;
                    Auxil = BufferEatInt(ReadBuffer, 1);

                    if ((Auxil) != 1)
                    {
                        ErrorState = 4;
                    }

                    BufferPos = 0;
                    break;
                case 12: //Ack, ACK ERROR se est� enviando a frame e recebe 1.
                    State = 13;
                    StateSize = 1;
                    Auxil = BufferEatInt(ReadBuffer, 1);
                    if ((Auxil) == 1 && Sending == 1)
                        ErrorState = 6;
                    BufferPos = 0;
                    break;

                default:  //State 13+, aproveitado pra retornar valor negativo se frame e IFS terminaram.
                    StateSize = 1;
                    Auxil = BufferEatInt(ReadBuffer, 1);
                    BufferPos = 0;
                    if (State >= 20) 
                    {
                        ErrorState = -1; //FRAME END, n�o acontecem mais erros.
                        if (Auxil == 1 && State>20)
                        {
                            IFS++;
                            if (IFS >= 3)
                                ErrorState = -2; //IFS END, j� pode receber novas frames!
                        }
                        else
                            IFS = 0;
                    }
                    if ((Auxil - 0) == 0 && State < 20)
                    {
                            ErrorState = 4; //FORM ERROR poss�vel.
                    }
                    State = State + 1;
                    break;
                }
            }
    }

    return (ErrorState); 
    /* Retorna -1 se a frame acabou, -2 se o IFS acabou, -3 pra ERROR FRAME END, -4 para arbitração perdida
    2 para BIT ERROR, 3 pra STUFF ERROR, 4 pra FORM ERROR, 5 pra CRC ERROR, 6 pra ACK ERROR
    outrora retorna o bit. */
}

void frame_builder() {

}

int bit_destf(int Bit) {
	int Ret;
    static int RepBitCounter = 0;

	RepBitCounter++;

	if (Bit != LastBit)
	{
		LastBit = Bit;
		if (RepBitCounter == 6)
		{
			Ret = -1;
		}
		else
			Ret = Bit;

		RepBitCounter = 1;
	}
	else
	{
		if (RepBitCounter >= 6)
			Ret = -2;
		else
			Ret = Bit;
	}

	return Ret;
}

void encoder_exe() {
    // if message to send, encode frame
    // if message transmitting, write bit
    // if
}

void frame_interpreter(struct Frame Fr, int* EncFrame) {
	int A;
	int* Stuffed;
	int Type;

	if (Fr.IDE == 1 && Fr.SRR == 0)
		Type = 99;
	else
		Type = Fr.RTR + (2 * Fr.IDE);

	switch (Type)
	{
	case 0: //Data, Normal
		ToBin(EncFrame + 1, Fr.ID1, 11);
		FillInt(EncFrame, "0",12);
		FillInt(EncFrame, "0",13);
		ToBin(EncFrame + 15, Fr.DLC,4);
		if(Fr.DLC!=0)
		HexToBin(EncFrame + 19, Fr.Data);
		*(EncFrame + 19 + Fr.DLC*8) = -1;
		NaiveCRC(EncFrame);
		Stuffed = bit_stf(EncFrame);
		A = GetDataSize(Stuffed);
		CopyInt(EncFrame, Stuffed, A, 0);
		FillInt(EncFrame, "1111111111-1", A);
		break;
	case 1: //Data, Remote
		ToBin(EncFrame + 1, Fr.ID1, 11);
		FillInt(EncFrame, "1", 12);
		FillInt(EncFrame, "0", 13);
		ToBin(EncFrame + 15, Fr.DLC, 4);
		*(EncFrame + 19) = -1;
		NaiveCRC(EncFrame);
		Stuffed = bit_stf(EncFrame);
		A = GetDataSize(Stuffed);
		CopyInt(EncFrame, Stuffed, A, 0);
		FillInt(EncFrame, "1111111111-1", A);
		break;
	case 2: //Extended, Normal
		ToBin(EncFrame + 1, Fr.ID1, 11);
		FillInt(EncFrame, "1", 12);
		FillInt(EncFrame, "1", 13);
		ToBin(EncFrame + 14, Fr.ID2, 18);
		FillInt(EncFrame, "0", 32);
		FillInt(EncFrame, "00", 33);
		ToBin(EncFrame + 35, Fr.DLC, 4);
		if (Fr.DLC != 0)
		HexToBin(EncFrame + 39, Fr.Data);
		*(EncFrame + 39 + Fr.DLC * 8) = -1;
		NaiveCRC(EncFrame);
		Stuffed = bit_stf(EncFrame);
		A = GetDataSize(Stuffed);
		CopyInt(EncFrame, Stuffed, A, 0);
		FillInt(EncFrame, "1111111111-1", A);
		break;
	case 3: //Extended, Remote
		ToBin(EncFrame + 1, Fr.ID1, 11);
		FillInt(EncFrame, "1", 12);
		FillInt(EncFrame, "1", 13);
		ToBin(EncFrame + 14, Fr.ID2, 18);
		FillInt(EncFrame, "1", 32);
		FillInt(EncFrame, "00", 33);
		ToBin(EncFrame + 35, Fr.DLC, 4);
		*(EncFrame + 39) = -1;
		NaiveCRC(EncFrame);
		Stuffed = bit_stf(EncFrame);
		A = GetDataSize(Stuffed);
		CopyInt(EncFrame, Stuffed, A, 0);
		FillInt(EncFrame, "1111111111-1", A);
		break;
	default:
		printf("FORM ERROR!!!");
		A = 0;
		break;
	}
}

//Stuff um array de inteiros que termina em -1.
int* bit_stf(int FrameS[200])
{
    int Stuffed[200];
	int FrameSweep = 0;
	int StuffedSweep = 0;
	int LastBitStf = FrameS[0];
	
	while (FrameS[FrameSweep] != -1)
	{
		RepBitCounter++;
		if (LastBitStf != FrameS[FrameSweep])
		{
			Stuffed[StuffedSweep] = FrameS[FrameSweep];
			StuffedSweep++;
			RepBitCounter = 1;
			LastBitStf = FrameS[FrameSweep];
		}
		else
		{
			Stuffed[StuffedSweep] = FrameS[FrameSweep];
			StuffedSweep++;
			if (RepBitCounter >= 5)
			{
				LastBitStf = (FrameS[FrameSweep] + 1) % 2;
				Stuffed[StuffedSweep] = LastBitStf;
				StuffedSweep++;
				RepBitCounter = 1;
			}
		}
		FrameSweep++;
	}
	Stuffed[StuffedSweep] = -1;

	return Stuffed;
}

void error_handler_exe() {

}