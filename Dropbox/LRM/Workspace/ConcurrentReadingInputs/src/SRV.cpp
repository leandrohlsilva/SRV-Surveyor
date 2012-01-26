//*****************************************************************************
// * USP->ICMC->LRM
// * pessin@gmail.com
// ***************************************************************************/

#include <SFML/Network.hpp>
#include "SRV.h"
#include <stdio.h>
#include <string.h>

// ****************************************************************************
// Construtor (conexao)
// ****************************************************************************

SRV::SRV(sf::IPAddress _Address, unsigned short _Port)
{
	Port = _Port;
	Address = _Address;

	Client.Connect(Port, Address);

	IndiceNomeImagem = 0;
}

// ******************************************************************************
// Ativa laser  
// ******************************************************************************

int SRV::ativaLaser()
{
	// send
	char ToSend[] = "l";
	Client.Send(ToSend, sizeof(ToSend));

	// receive 
	char Message[128];
	std::size_t Received;
	Client.Receive(Message, sizeof(Message), Received);

	return 0;
}

// ******************************************************************************
// Desativa laser  
// ******************************************************************************

int SRV::desativaLaser()
{
	// send	
	char ToSend[] = "L";
	Client.Send(ToSend, sizeof(ToSend));

	// receive 
	char Message[128];
	std::size_t Received;
	Client.Receive(Message, sizeof(Message), Received);

	return 0;
}

// ******************************************************************************
//   
// ******************************************************************************

int SRV::getDistanceCamera()
{
	// send
	Client.Send("R", 1);

	// receive 
	char Message[128];
	std::size_t tamRetorno;
	Client.Receive(Message, sizeof(Message), tamRetorno);

	//printf("D - %s\n", Message);

	if (tamRetorno > 0) Message[tamRetorno-2] = '\0';
	
	while (strstr(Message,"##Range(cm)") == 0)
	{
		Client.Send("R", 1);
		Client.Receive(Message, sizeof(Message), tamRetorno);
	}

	int iDistanciaAteObstaculo;
	char junk[20];

	sscanf(Message,"%s %s %d",junk, junk, &iDistanciaAteObstaculo);
    
    return iDistanciaAteObstaculo;
}

// ****************************************************************************
// Forças: 0 a 100% [char de 0 a 100, crescente]
//         de -0 a -100, força decescente [char 156 ate 255]
// Tempo de ativação: 0 a 100ms * 10ms
// ****************************************************************************

int SRV::ativaMotor(int pforca_e, int pforca_d, int ptempo)
{


	char forca_e[2];
	char forca_d[2];
	char tempo[2];

	if (pforca_d < 0) pforca_d = pforca_d + 256;
	if (pforca_e < 0) pforca_e = pforca_e + 256;

    strcpy (forca_e," ");
    strcpy (forca_d," ");
    strcpy (tempo," ");

	forca_e[0] = char(pforca_e);
    forca_d[0] = char(pforca_d);
    tempo[0] = char(ptempo);

	Client.Send("M", 1);
	Client.Send(forca_e, 1);
	Client.Send(forca_d, 1);
	Client.Send(tempo, 1);

	// receive 
	char Message[128];
	std::size_t Received;
	Client.Receive(Message, sizeof(Message), Received);
	//std::cout << Message;

	return(0);
}

// ****************************************************************************
//
// ****************************************************************************

char* SRV::getImage(int *tamanhoImg, int grava)
{
	int tm = 4500;
	std::size_t tam_ret_bytes;

	char mensagem[4500];

	char *img;
	img = (char *) calloc (15000,sizeof(char));

	int posicao = 0; // posicao no vetor de imagem

	Client.Send("I", 1);
	Client.Receive(mensagem, tm, tam_ret_bytes);

	for (unsigned int i=10; i<tam_ret_bytes; i++)
	{
		img[i-10] = mensagem[i];
		posicao = i-10;
	}

	posicao++;

	int fim = 0;

	for (unsigned int i=0; i<tam_ret_bytes; i++)
	{
		if ((mensagem[i] == -1) && (mensagem[i+1] == -39))
		{
			fim = 1;
		}
	}
	
	while (fim == 0)
	{
		Client.Receive(mensagem, tm, tam_ret_bytes);

		for (unsigned int i=0; i<tam_ret_bytes; i++)
		{
			img[posicao] =  mensagem[i];
			posicao++; 
		}

		for (unsigned int i=0; i<tam_ret_bytes; i++)
		{
			if ((mensagem[i] == -1) && (mensagem[i+1] == -39))
			{
				fim = 1;
			}
		}
	}

	//
	//
	//

	if (grava)
	{
		FILE *f;
		char nomef[20];
		sprintf(nomef,"%d",IndiceNomeImagem);
		strcat(nomef,".jpeg");
		f = fopen(nomef,"wb");
		if (1) printf(">> Gravando imagem %d...\n",IndiceNomeImagem);
		IndiceNomeImagem++;

		for (int i=0; i<posicao; i++) 
		{
			fprintf(f,"%c",img[i]);
			fflush(f);
		}
		
		fclose(f);
	}

	//
	// atualiza variavel com tamanho da imagem e retorna ponteiro
	//

	(*tamanhoImg) = posicao;
	return img;
}

// ******************************************************************************
//   
// ******************************************************************************

float SRV::getSonar()
{
	// send	
	char ToSend[] = "p";
	Client.Send(ToSend, sizeof(ToSend));

	// receive 
	char Message[128];
	std::size_t Received;
	Client.Receive(Message, sizeof(Message), Received);

	double cm;
	char lixo[20];
	//printf("S - %s\n", Message);
	sscanf(Message,"%s %lf",lixo,&cm);

	cm = cm / 100.0;
	cm = cm * 2.54;

	return cm;
}
