//*****************************************************************************
// * USP->ICMC->LRM
// * pessin@gmail.com
// ***************************************************************************/

#include <boost/signals2/mutex.hpp>
#include <SFML/Network.hpp>
#include <boost/thread.hpp>
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
	return sendCommand(comando, 0, 0, 0);
}
char * SRV::sendCommand(char *comando, char p1) {
	return sendCommand(comando, p1, 0, 0);
}

char * SRV::sendCommand(char *comando, char p1, char p2) {
	return sendCommand(comando, p1, p2, 0);
}

char * SRV::sendCommand(char *comando, char p1, char p2, char p3) {
	//printf("O comando %s foi requisitado\n", comando);
	SRV::mutex.lock();
	char Message[200], aux[2];
	std::size_t tamRetorno;
	Client.Send(comando, 1);
	if (p1 != 0) {
		aux[0] = p1;
		aux[1] = '\0';
		Client.Send(aux, 1);
	}
	if (p2 != 0) {
		aux[0] = p2;
		Client.Send(aux, 1);
	}
	if (p3 != 0) {
		aux[0] = p3;
		Client.Send(aux, 1);
	}
	Client.Receive(Message, sizeof(Message), tamRetorno);
	SRV::mutex.unlock();
	//printf("O comando %s foi completado\n", comando);
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

void SRV::normalVision() {
	Client.Send("g_", 1);
}
void SRV::frameDifferencingVision() {
	Client.Send("g0", 1);
}
void SRV::colorSegmentationVision() {
	Client.Send("g1", 1);
}
void SRV::edgeDetectionVision() {
	Client.Send("g2", 1);
}
void SRV::obstacleDetectionVision() {
	Client.Send("g4", 1);
}

// ****************************************************************************
// Forças: 0 a 100% [char de 0 a 100, crescente]
//         de -0 a -100, força decescente [char 156 ate 255]
// Tempo de ativação: 0 a 100ms * 10ms
// ****************************************************************************

int SRV::startMotor()
{
	char comando[] = "M";
	sendCommand(comando, 1, 1, 1);

	return(0);
}

bool readImage(sf::SocketTCP socket, int *position, char **img) {

	int tm = 15000;
	std::size_t tam_ret_bytes;
	char mensagem[tm];

	socket.Receive(mensagem, tm, tam_ret_bytes);

	//(*img) = realloc((*img), *position*tam_ret_bytes*sizeof(char));

	//strcat(*img, mensagem);

	char *img2 = *img;

	for (unsigned int i=0; i<tam_ret_bytes; i++)
	{
		img2[i+*position] = mensagem[i];
		if ((i > 0) && (mensagem[i-1] == -1) && (mensagem[i] == -39)) {
			//printf("achou marcador de fim de jpeg\n");
			*position += i + 1;
			return true;
		}
	}

	*position += tam_ret_bytes;

	//return tam_ret_bytes == 0;
	return false;

}

// ****************************************************************************
//
// ****************************************************************************
char* SRV::getImage(int *tamanhoImg)
{
	SRV::mutex.lock();

	//sf::Sleep(2);
	/*char *img = NULL;
	boost::thread image(waitImage, this, tamanhoImg, &img);

	int i = 0;
	while((img == NULL) && (i++ < 5)) {
		sf::Sleep(.05);
	}*/

	int tm = 15000;
	std::size_t tam_ret_bytes;

	char mensagem[tm];

	char *img;
	img = (char *) calloc (tm,sizeof(char));

	Client.Send("I", 1);

	int tmH = 6;
	char header[tmH];
	Client.Receive(header, tmH * sizeof(char), tam_ret_bytes);

	//printf("Tipo de resolucao %c\n", header[5]);
	if (header[5] != '5') {
		printf("Erro na leitura do cabecalho\n");
		SRV::mutex.unlock();
		return NULL;
	}

	char sizeImage[4];

	Client.Receive(sizeImage, 4*sizeof(char), tam_ret_bytes);
	unsigned int tamImagem = 0 | (sizeImage[0]) | ((sizeImage[1]) << 8) |
			((sizeImage[2]) << 16) | ((sizeImage[3]) << 24);

	//printf("tamImagem = %d\n", tamImagem);
	if ((tamImagem < 0) || (tamImagem > 100*1024)) {
		//printf("Imagem com tamanho incorreto\n");
		//SRV::mutex.unlock();
		//return NULL;
	}

	int bytesImagem = 0;

	//recebe a imagem em modo nao bloqueante
	Client.SetBlocking(false);
	while(!readImage(Client, &bytesImagem, &img));
	Client.SetBlocking(true);
	*tamanhoImg = bytesImagem;

	SRV::mutex.unlock();

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
