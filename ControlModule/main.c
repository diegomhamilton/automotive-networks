#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ManyFunctions.h"
#include "BitStuffing.h"
#include "Encoder.h"
#include "Decoder.h"


int EncodedFrame[200];

//Até agora, Decode uma String em Bar na struct definida em ManyFunctions, depois (re)Encode a struct em um array de Int.
//Detecta alguns erros, mas ainda não processa nada.
//Encoder é atômico, Decoder recebe bits de um por um.
//Espero que goste de animais.

int main()
{
    int BovineCounter = 0;
    int IsStuff = 0;
    int Bit = 0;
	int DeStuffSweep = 0;
	int Cat = 0;
	struct Frame Fr;
	ReadBuffer[199] = -1;
	//Entrada do Decoder, termina em -1.
	char Bar[] = { "0110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011011111111-1" };

	Fr.ID1 = -1;
	Fr.ID2 = -1;
	StrcpyIsGross(Fr.Data, "R");
	Fr.DLC = -1;
	Fr.IDE = -1;
	Fr.RTR = -1;
	Fr.SRR = -1;


	//Enviando elementos da string até encontrar o -. Retorna erro, Frame End e IFS End após 3 recessivos consecutivos.
	//Reseta quando Idle = 1 e recebe um bit 0;
	//Se não tiver erro ou Frame/IFS end, retorna o bit.

	IFS = 0;
	while (Bar[CanaryCounter] != '-')
	{
		Cat = DecoderStep(&Fr, Bar);
		CanaryCounter++;
		if (Cat > 1)
			printf("ERROR %d!\n", Cat);
		else if (Cat == -1)
			printf("Frame End\n");
		else if (Cat == -2)
			printf("IFS End\n");
	}

	printf("Decoded:\n");
	printf("ID1 = 0x%x\n", Fr.ID1);
	printf("ID2 = 0x%x\n", Fr.ID2);
	printf("RTR = %d\n", Fr.RTR);
	printf("IDE = %d\n", Fr.IDE);
	printf("SRR = %d\n", Fr.SRR);
	printf("DLC = %d\n", Fr.DLC);
	if(Fr.RTR==0)
	for (BovineCounter = 0; BovineCounter < Fr.DLC * 2; BovineCounter++)
		printf("%c", Fr.Data[BovineCounter]);
	printf("\n\n");

	//Encoder, ainda não faz nada pra enviar os dados, é
	Encode(Fr, EncodedFrame);
	printf("Encoded:\n");
	ReadArray(EncodedFrame);


    return 0;
}
