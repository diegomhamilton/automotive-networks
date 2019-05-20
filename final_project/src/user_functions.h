#ifndef USER_FUNCTIONS_H
#define USER_FUNCTIONS_H
#define DEBUG_CODE false

struct Frame
{
	int ID1;
	int ID2;
	int RTR;
	int IDE;
	int SRR;
	char Data[16];  // every two char (HEX char) corresponds to one byte
	int DLC;
	int user_CRC;
	bool ACK;
};

extern const int CRCPoly[17];

//Zera frame de 200 bits, coloca -1 no final.
void FlushFrame(int* Frame);

//strcpy
void StrcpyIsGross(char* A, char* B);

//Conta tamano de array, at� chegar em -1.
int GetDataSize(int* Data);

//Transforma int em bin�rio, joga na frame.
void ToBin(int* Place, int Data, int Size);

//Transforma Hex em binário, joga na frame.
void HexToBin(int* Array, char* Mander);

//Lê array de inteiros até -1.
void ReadArray(int* Frame);

//Transforma string em array de inteiros.
void FillInt(int* Ints, char* String, int Offset);

//Copia array de inteiros em outro.
void CopyInt(int* Ints, int* String, int Size, int Offset);

//CRC for dummies, coloca o CRC no final da frame.
void NaiveCRC(int* Data);

//CRC^-1, retorna 5 (Erro CRC) se n�o bateu.
int UnCRC(int* Data);

//L� um elemento do buffer como inteiro.
int BufferEatInt(int* Buffer, int Size);

//Copia (size) bytes do buffer em uma string (intervalos de 4 bits x 2).
void BufferEatData(int* Buffer, char* String, int Size);

#endif