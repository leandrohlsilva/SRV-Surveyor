//*****************************************************************************
// * USP->ICMC->LRM
// * pessin@gmail.com
// ***************************************************************************/

#include <boost/signals2/mutex.hpp>
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

void SRV::turnLaserOn()
{
	sendCommand("l");
}

// ******************************************************************************
// Desativa laser  
// ******************************************************************************

void SRV::turnLaserOff()
{
	sendCommand("L");
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


char * SRV::sendCommand(char *comando) {
	SRV::mutex.lock();
	char Message[200];
	std::size_t tamRetorno;
	Client.Send(comando, 1);
	Client.Receive(Message, sizeof(Message), tamRetorno);
	SRV::mutex.unlock();
	return Message;
}
char * SRV::sendCommand(char *comando, char p1) {
	SRV::mutex.lock();
	char Message[200], aux[2];
	std::size_t tamRetorno;
	Client.Send(comando, 1);
	aux[0] = p1;
	aux[1] = '\0';
	Client.Send(aux, 1);
	Client.Receive(Message, sizeof(Message), tamRetorno);
	SRV::mutex.unlock();
	return Message;
}

char * SRV::sendCommand(char *comando, char p1, char p2) {
	SRV::mutex.lock();
	char Message[200], aux[2];
	std::size_t tamRetorno;
	Client.Send(comando, 1);
	aux[0] = p1;
	aux[1] = '\0';
	Client.Send(aux, 1);
	aux[0] = p2;
	Client.Send(aux, 1);
	Client.Receive(Message, sizeof(Message), tamRetorno);
	SRV::mutex.unlock();
	return Message;
}

char * SRV::sendCommand(char *comando, char p1, char p2, char p3) {
	SRV::mutex.lock();
	char Message[200], aux[2];
	std::size_t tamRetorno;
	Client.Send(comando, 1);
	aux[0] = p1;
	aux[1] = '\0';
	Client.Send(aux, 1);
	aux[0] = p2;
	Client.Send(aux, 1);
	aux[0] = p3;
	Client.Send(aux, 1);
	Client.Receive(Message, sizeof(Message), tamRetorno);
	SRV::mutex.unlock();
	return Message;
}

void SRV::turnLeft() {
	sendCommand("0");
}

void SRV::turnRight() {
	sendCommand(".");
}

void SRV::driftBackLeft() {
	sendCommand("1");
}

void SRV::driftBackRight() {
	sendCommand("3");
}

void SRV::moveBack() {
	sendCommand("2");
}

void SRV::moveForward() {
	sendCommand("8");
}

void SRV::stop() {
	sendCommand("5");
}

void SRV::driftLeft() {
	sendCommand("7");
}

void SRV::driftRight() {
	sendCommand("9");
}

void SRV::driveLeft() {
	sendCommand("4");
}

void SRV::driveRight() {
	sendCommand("6");
}

// ****************************************************************************
// Forças: 0 a 100% [char de 0 a 100, crescente]
//         de -0 a -100, força decescente [char 156 ate 255]
// Tempo de ativação: 0 a 100ms * 10ms
// ****************************************************************************

int SRV::startMotor()
{
	char comando[] = "M";
	sendCommand(comando, 0, 0, 1);

	return(0);
}

// ****************************************************************************
//
// ****************************************************************************

char* SRV::getImage(int *tamanhoImg, int grava)
{
	SRV::mutex.lock();
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

	SRV::mutex.unlock();

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
	SRV::mutex.lock();
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

	SRV::mutex.unlock();

	return cm;
}

bool SRV::isAbleToRun() {
	return this->ableToRun;
}

void SRV::setAbleToRun(bool a) {
	this->ableToRun = a;
}
