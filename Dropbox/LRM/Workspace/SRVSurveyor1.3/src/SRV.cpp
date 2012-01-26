#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>
#include <SFML/Window.hpp>
#include "SRV.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Const.h"


// ****************************************************************************
// Construtor (conexao)
// ****************************************************************************

SRV::SRV(const string &foreignAddress, unsigned short foreignPort)
{
	address = foreignAddress;
	port = foreignPort;
	Client.connect(foreignAddress, foreignPort);
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
	Client.send("R", 1);
	// receive
	unsigned int lenBuffer = 128;
	char Message[lenBuffer];
	Client.recv(Message, lenBuffer);

	//printf("D - %s\n", Message);

	while (strstr(Message,"##Range(cm)") == 0)
	{
		Client.send("R", 1);
		Client.recv(Message, lenBuffer);
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
	unsigned int lenBuffer = 128;
	char Message[lenBuffer], aux[2];
	Client.send(comando, 1);
	if (p1 != 0) {
		aux[0] = p1;
		aux[1] = '\0';
		Client.send(aux, 1);
	}
	if (p2 != 0) {
		aux[0] = p2;
		Client.send(aux, 1);
	}
	if (p3 != 0) {
		aux[0] = p3;
		Client.send(aux, 1);
	}
	Client.recv(Message, lenBuffer);
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
	Client.send("g_", 1);
}
void SRV::frameDifferencingVision() {
	Client.send("g0", 1);
}
void SRV::colorSegmentationVision() {
	Client.send("g1", 1);
}
void SRV::edgeDetectionVision() {
	Client.send("g2", 1);
}
void SRV::obstacleDetectionVision() {
	Client.send("g4", 1);
}

int SRV::startMotor()
{
	char comando[] = "M";
	sendCommand(comando, 1, 1, 1);

	return(0);
}

bool readImage(TCPSocket socket, int *position, char **img) {

	int tm = 500;
	char mensagem[tm];

	while (1) {

		int bufferLen = socket.recv(mensagem, tm);

		char *img2 = *img;

		for (int i=0; i<bufferLen; i++)
		{
			img2[i+*position] = mensagem[i];
			if ((i > 0) && (mensagem[i-1] == -1) && (mensagem[i] == -39)) {
				//printf("achou marcador de fim de jpeg\n");
				*position += i + 1;
				return true;
			}
		}

		*position += bufferLen;

	}


	return false;
}

void SRV::reconnect() {
	//this->Client.close();
	//this->Client.connect(this->address, this->port);
	//this->Client.send("$!", 2);
}

void controleImagem(SRV *r) {

	time_t start,end;
	time (&start);
	double dif = 0;
	bool reconectou = false;
	while (r->isSendingImage()) {
		time (&end);
		dif = difftime(end, start);
		if (dif > MAX_LIMITE_ENVIO) {
			if (!reconectou) {
				printf("Deu reconnect\n");
				reconectou = true;
			}
			r->Client.send("I", 1);
			//exit(1);
		}
		//r->Client.send("I", 1);
	}

}

bool SRV::isSendingImage() {
	return this->sendingImage;
}

// ****************************************************************************
//
// ****************************************************************************
char* SRV::getImage(int *tamanhoImg)
{

	time_t start,end;
	time (&start);
	SRV::mutex.lock();

	this->sendingImage = true;

	boost::thread threadControleImagem(controleImagem, this);

	int tm = 15000;

	char *img;
	img = (char *) calloc (tm,sizeof(char));

	Client.send("I", 1*sizeof(char));

	int tmH = 6;
	char header[tmH];
	Client.recv(header, tmH * sizeof(char));

	char mensagem[tm];

	if (header[2] != 'I' || header[3] != 'M' || header[4] != 'J') {
		printf("Erro na leitura do cabecalho\n");

		//ajuda a limpar o buffer
		Client.recv(mensagem, tm);

		SRV::mutex.unlock();
		return NULL;
	}

	char sizeImage[4];

	Client.recv(sizeImage, 4*sizeof(char));

	int bytesImagem = 0;

	bool ok = false;

	while (1) {

		int bufferLen = Client.recv(mensagem, tm);

		for (int i=0; i<bufferLen; i++)
		{
			img[i+bytesImagem] = mensagem[i];
			if ((i > 0) && (mensagem[i-1] == -1) && (mensagem[i] == -39)) {
				ok = true;
				break;
			}
		}

		bytesImagem += bufferLen;
		if (ok) break;

	}

	*tamanhoImg = bytesImagem;

	SRV::mutex.unlock();

	this->sendingImage = false;
	time (&end);

	//printf("tempo = %f\n", difftime(start, end));

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
	Client.send(ToSend, sizeof(ToSend));

	// receive 
	unsigned int lenBuffer = 128;
	char Message[lenBuffer];
	Client.recv(Message, lenBuffer);

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
