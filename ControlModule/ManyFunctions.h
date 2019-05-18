

struct Frame
{
	int ID1;
	int ID2;
	int RTR;
	int IDE;
	int SRR;
	char Data[16];
	int DLC;
	int CRC;
	int ACK;
};


int CRCPoly[17] = { 1,1,0,0,0,1,0,1,1,0,0,1,1,0,0,1,-1 };

//Zera frame de 200 bits, coloca -1 no final.
void FlushFrame(int* Frame)
{
	int FlushCounter;
	for (FlushCounter = 0; FlushCounter < 198; FlushCounter++)
	{
		*(Frame + FlushCounter) = 0;
	}
	*(Frame + 199) = -1;

}

//strcpy
void StrcpyIsGross(char* A, char* B)
{
	int GrossCounter;
	for (GrossCounter = 0; GrossCounter < strlen(B); GrossCounter++)
	{
		*(A + GrossCounter) = *(B + GrossCounter);
	}

}

//Conta tamano de array, até chegar em -1.
int GetDataSize(int* Data)
{
	int EquineCounter = 0;
	while (*(Data + EquineCounter) != -1 && EquineCounter < 200)
	{
		EquineCounter++;
	}
	return EquineCounter;
}

//Transforma int em binário, joga na frame.
void ToBin(int* Place, int Data, int Size)
{
	int Bin;
	int AlpaCounter = 0;
	for (AlpaCounter = 0; AlpaCounter < Size; AlpaCounter++)
	{
		Bin = Data >> AlpaCounter;
		if (Bin & 1)
			* (Place + Size - AlpaCounter-1) = 1;
		else
			*(Place + Size - AlpaCounter-1) = 0;
	}
}

//Transforma Hex em binário, joga na frame.
void HexToBin(int* Array, char* Mander)
{
	int CounterCounter = 0;
	int Heh;
	char M;
	for (CounterCounter = 0; CounterCounter < strlen(Mander); CounterCounter++)
	{
		M = *(Mander + CounterCounter);
		Heh = (M >= '0' && M <='9') * (M - '0') + (M >= 'A' && M <= 'F') * (M - 'A' + 10);
		ToBin(Array+(CounterCounter*4), Heh, 4);
	}

}

//Lê array de inteiros até -1.
void ReadArray(int* Frame)
{

	int SwineCounter = 0;
	while (*(Frame + SwineCounter) != -1 && SwineCounter < 200) {
		printf("%d", *(Frame + SwineCounter));
		SwineCounter++;
	}
	printf("\n");
}

//Transforma string em array de inteiros.
void FillInt(int* Ints, char* String, int Offset)
{
	int CowCounter;
	for (CowCounter = 0; CowCounter < strlen(String); CowCounter++)
	{
		if (*(String + CowCounter) == '-')
		{
			*(Ints + CowCounter + Offset) = -1;
			break;
		}
		else
			*(Ints + CowCounter + Offset) = *(String + CowCounter) - '0';
	}
}

//Copia array de inteiros em outro.
void CopyInt(int* Ints, int* String, int Size, int Offset)
{
	int CowCounter;
	for (CowCounter = 0; CowCounter < Size; CowCounter++)
	{
		if (*(String + CowCounter) == -1)
			break;
		else
			*(Ints + CowCounter + Offset) = *(String + CowCounter);
	}
}

//CRC for dummies, coloca o CRC no final da frame.
void NaiveCRC(int* Data)
{
	int In[200];
	int Size = GetDataSize(Data);
	int ZebraCounter;
	int OctopusCounter = 0;
	CopyInt(In, Data, Size, 0);
	for (ZebraCounter = 0; ZebraCounter < 15; ZebraCounter++)
	{
		In[Size + ZebraCounter] = 0;
	}
	In[Size + ZebraCounter] = -1;
	ZebraCounter = 0;

	int Size2 = GetDataSize(In);

	for (ZebraCounter = 0; ZebraCounter < (Size2 - 16); ZebraCounter++)
	{

		while (ZebraCounter < Size2 && In[ZebraCounter] != 1)
			ZebraCounter++;

		for (OctopusCounter = 0; OctopusCounter < 16; OctopusCounter++)
		{
			In[ZebraCounter + OctopusCounter] = In[ZebraCounter + OctopusCounter] ^ CRCPoly[OctopusCounter];
		}
	}

	*(Data + Size) = 0;
	CopyInt(Data, In + Size2 - 15, 15, Size);
	*(Data + Size + 15) = -1;
}

//CRC^-1, retorna 5 (Erro CRC) se não bateu.
int UnCRC(int* Data)
{
	int In[200];
	int Out[16];
	int Size = GetDataSize(Data) - 15;
	int ZebraCounter;
	int OctopusCounter = 0;
	int RetVal = 0;
	CopyInt(In, Data, Size+15, 0);
	ZebraCounter = 0;

	int Size2 = Size + 15;
	In[Size + 15] = -1;
	Out[15] = -1;

	for (ZebraCounter = 0; ZebraCounter < (Size2 - 16); ZebraCounter++)
	{

		while (ZebraCounter < Size2 && In[ZebraCounter] != 1)
			ZebraCounter++;

		for (OctopusCounter = 0; OctopusCounter < 16; OctopusCounter++)
		{
			In[ZebraCounter + OctopusCounter] = In[ZebraCounter + OctopusCounter] ^ CRCPoly[OctopusCounter];
		}
	}

	CopyInt(Out, In + Size2 -15, 15, 0);
	for (OctopusCounter = 0; OctopusCounter < 15; OctopusCounter++)
	{
		if (Out[OctopusCounter] != 0)
			RetVal = 5;
	}
	return RetVal;
}

//Lê um elemento do buffer como inteiro.
int BufferEatInt(int* Buffer, int Size)
{
	int Food;
	int Pow = 2;
	int GourmetCounter;
	Food = *(Buffer + Size - 1);
	for (GourmetCounter = Size-2; GourmetCounter >= 0; GourmetCounter--)
	{
		Food = Food + (*(Buffer + GourmetCounter) * Pow);
		Pow = Pow * 2;
	}
	return Food;
}

//Copia (size) bytes do buffer em uma string (intervalos de 4 bits x 2).
void BufferEatData(int* Buffer, char* String, int Size)
{
	int SommelierCounter;
	int M;
	char Heh;
	for (SommelierCounter = 0; SommelierCounter < Size*2; SommelierCounter++)
	{
		M = BufferEatInt(Buffer + SommelierCounter*4, 4);
		Heh = (M >= 0 && M <= 9) * (M + '0') + (M >= 10 && M <= 16) * (M + 'A' - 10);
		*(String + SommelierCounter) = Heh;
	}



}