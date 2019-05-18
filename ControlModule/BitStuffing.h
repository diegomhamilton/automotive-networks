

int RepBitCounter = 0;
int LastBit = 0;
int Flag = 0;
int Stuffed[200];
int DeStuffed[200];


//Destuff os bits de um por um.
int DeStuff(int Bit)
{
	int Ret;
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

//Stuff um array de inteiros que termina em -1.
int* BitStuf(int FrameS[200])
{
	RepBitCounter = 0;
	int FrameSweep = 0;
	int StuffedSweep = 0;
	LastBit = FrameS[0];
	while (FrameS[FrameSweep] != -1)
	{
		RepBitCounter++;
		if (LastBit != FrameS[FrameSweep])
		{
			Stuffed[StuffedSweep] = FrameS[FrameSweep];
			StuffedSweep++;
			RepBitCounter = 1;
			LastBit = FrameS[FrameSweep];
		}
		else
		{
			Stuffed[StuffedSweep] = FrameS[FrameSweep];
			StuffedSweep++;
			if (RepBitCounter >= 5)
			{
				LastBit = (FrameS[FrameSweep] + 1) % 2;
				Stuffed[StuffedSweep] = LastBit;
				StuffedSweep++;
				RepBitCounter = 1;
			}
		}
		FrameSweep++;
	}
	Stuffed[StuffedSweep] = -1;

	return Stuffed;
}
