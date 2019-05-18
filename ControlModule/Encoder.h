int Sending = 0;

void Encode(struct Frame Fr, int* EncFrame)
{
	int A;
	int* Stuffed;
	int Type;
	if (Fr.IDE == 1 && Fr.SRR == 0)
		Type = 99;
	else
		Type = Fr.RTR + (2 * Fr.IDE);

	int WhenAck = 0;

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
		Stuffed = BitStuf(EncFrame);
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
		Stuffed = BitStuf(EncFrame);
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
		Stuffed = BitStuf(EncFrame);
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
		Stuffed = BitStuf(EncFrame);
		A = GetDataSize(Stuffed);
		CopyInt(EncFrame, Stuffed, A, 0);
		FillInt(EncFrame, "1111111111-1", A);
		break;
	default:
		printf("FORM ERROR!!!");
		A = 0;
		break;
	}
	WhenAck = A + 2;
}
