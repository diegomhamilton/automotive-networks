int Idle = 1; //1 for bus idle. 
int BufferPos = 0;
int ReadBuffer[200] = {0};
int CRCBuild[200] = { 0 };
int CRCBuildSize = 0;
int IFS = 0;
int State;
int StateSize;
int DecType;
int CanaryCounter = 0;

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

int DecoderStep(struct Frame* Fr, char* Bar)
{
	int ErrorState = 0; // 3 for stuff error, 4 for form error, 5 for CRC error, 6 for Ack Error, -1 = Frame End, -2 = IFS End.
	int Bit;
	int Auxil;
		if (Idle == 1)
		{
			//Idle, reseta tudo e prepara pra ler frame.
			if (*(Bar+CanaryCounter) == '0')
			{
				LastBit = 0;
				RepBitCounter = 0;
				BufferPos = 0;
				Idle = 0;
				State = 1;
				Bit = DeStuff(0);
				StateSize = 11;
				CRCBuildSize = 1;
			}
		}
		else
		{
				//Destuff o bit se estiver antes do Ack.
				if (State < 11) 
					Bit = DeStuff(*(Bar+CanaryCounter) - '0');
				else
					Bit = *(Bar+CanaryCounter) -'0';
				//Destuff retorna -1 se o bit for um stuffing, -2 se for bitstuff error.
				if (Bit > -1)
				{
					ReadBuffer[BufferPos] = Bit;
					BufferPos++;
					StateSize--;
					CRCBuild[CRCBuildSize] = Bit;
					CRCBuildSize++;
					ErrorState = Bit;
				}
				else 
					if (Bit == -2)
					ErrorState = 3;

				//StateSize == 0 acontece quando acaba de ler x bits de um estado, pode ser unitário.
				if (StateSize == 0)
				{
					switch (State)
					{
					case 1: //ID1, SoF é levemente ignorado pois é levado em consideração quando Idle = 1.
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
					case 7: //2 Bits reservados, quantidade de bits a ler determinada em estado anterior, não acontece form error se os bits reservados estiverem errados. 
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
					case 10: //CRC, meu bebê.
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
					case 12: //Ack, ACK ERROR se está enviando a frame e recebe 1.
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
							ErrorState = -1; //FRAME END, não acontecem mais erros.
							if (Auxil == 1 && State>20)
							{
								IFS++;
								if (IFS >= 3)
									ErrorState = -2; //IFS END, já pode receber novas frames!
							}
							else
								IFS = 0;
						}
						if ((Auxil - 0) == 0 && State < 20)
						{
								ErrorState = 4; //FORM ERROR possível.
						}
						State = State + 1;
						break;
					}
				}
		}
	return (ErrorState); 
	//Retorna -1 se a frame acabou, -2 se o IFS acabou, 3 pra STUFF ERROR, 4 pra FORM ERROR, 
	//5 pra CRC ERROR, 6 pra ACK ERROR, outrora retorna o bit. 
	//2 Chora.
}