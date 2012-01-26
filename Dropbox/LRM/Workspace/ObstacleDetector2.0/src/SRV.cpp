#include "SRV.h"
#include <stdlib.h>
#include <string>
#include <boost/thread.hpp>
#include <iostream>
#include <stdio.h>
#include "networklayer/Message.h"


SRV::SRV(string foreignAddress, unsigned short foreignPort) {

	this->address = foreignAddress;
	this->port = foreignPort;
	this->socket = new TCPSocket();
	this->running = false;
	this->frameData = NULL;
	this->sonarRange = 0;

}

void bufferHandler(SRV *robo) {
	Receiver *rec = robo->getReceiver();
	int lenImg = 0;
	bool fim = false;
	char *imgBuffer = (char *) calloc(1500, sizeof(char));
	while(robo->isRunning()) {
		if (!rec->isBufferEmpty()) {
			Message *m = rec->topBuffer();
			rec->popBuffer();
			if (m == NULL) {
				continue;
			}
			int len = m->getLength();

			//se a mensagem eh de comando para se mover, nao prossiga
			if (len < 5) continue;

			char *s = m->getMsg();
			char cabecalho[10];
			strncpy(cabecalho, s, 10);
			if (cabecalho[0] == '#' && cabecalho[1] == '#'&& cabecalho[2] == 'I'
					&& cabecalho[3] == 'M' && cabecalho[4] == 'J') {

				imgBuffer = (char *)realloc(imgBuffer, len*sizeof(char));

				if (imgBuffer == NULL) {
					cout << "buffer pra imagem nao foi criado." << endl;
					continue;
				}

				//cout << "cabecalho = " << cabecalho << endl;

				for(int i = 10; i < len; i++) {
					imgBuffer[lenImg++] = s[i];
					if ((s[i] == -1) && (s[i + 1] == -39)) {
						fim = true;
					}
				}
			} else {

				if (cabecalho[0] == '#' && cabecalho[1] == '#'
						&& cabecalho[2] == 'p' && cabecalho[3] == 'i'
								&& cabecalho[4] == 'n' && cabecalho[5] == 'g') {
					float cm;
					sscanf(s, "%*s %f", &cm);

					cm = cm / 100.0;
					cm = cm * 2.54;

					robo->setSonarRange(cm);

					//cout << "sonar (cm) = " << cm << endl;

					continue;

				}

				if (imgBuffer == NULL) {
					cout << "buffer pra imagem nao foi criado." << endl;
					continue;
				}

				imgBuffer = (char *)realloc(imgBuffer, (lenImg+len)*sizeof(char));

				for(int i = 0; i < len; i++) {
					imgBuffer[lenImg++] = s[i];
					if ((s[i] == -1) && (s[i + 1] == -39)) {
						fim = true;
					}
				}
			}

			//cout  << " len = " << len << endl;

			if (fim) {
				//cout << "chegou ao fim com << " << lenImg << " bytes.\n";
				fim = false;
				robo->setFrameData(imgBuffer, lenImg);
				lenImg = 0;
			}


		}
	}
}


void SRV::start() {
	this->running = true;
	this->socket->connect(address, port);
	this->receiver = new Receiver(this->socket);
	this->receiver->start();
	this->movementsSender = new Sender(this->socket, "5", 0);
	float sl = (float)SRV::MILLI_IMAGE_REQUEST/1000.0;
	Sender *sImage = new Sender(this->socket, "I", sl);
	sImage->start();
	automaticSenders.insert(automaticSenders.begin(), sImage);
	//Sender *sSonar = new Sender(this->socket, "p", .3);
	//sSonar->start();
	//automaticSenders.insert(automaticSenders.begin(), sSonar);
	//Sender *sStop = new Sender(this->socket, "5", .3);
	//sStop->start();
	//automaticSenders.insert(automaticSenders.begin(), sStop);


	boost::thread t(bufferHandler, this);

}

bool SRV::isRunning() {
	return this->running;
}

Receiver *SRV::getReceiver() {
	return this->receiver;
}

char *SRV::getFrameData() {
	return this->frameData;
}

int SRV::getDataLength() {
	return this->dataLength;
}

void SRV::setFrameData(char *dados, int length) {

	this->frameData = (char *)realloc(this->frameData, length * sizeof(char));

	if (this->frameData != NULL) {
		for(int i = 0; i < length; i++) {
			this->frameData[i] = dados[i];
		}
	} else {
		cout << "Problema na alocacao de espaco" << endl;
		//exit(-1);
	}
	this->dataLength = length;
}

void SRV::useMotor(int pforca_d, int pforca_e, int ptempo) {
	char forca_e[2];
	char forca_d[2];
	char tempo[2];

	char comando[6];

	if (pforca_d < 0) pforca_d = pforca_d + 256;
	if (pforca_e < 0) pforca_e = pforca_e + 256;

	strcpy (forca_e," ");
	strcpy (forca_d," ");
	strcpy (tempo," ");

	forca_e[0] = char(pforca_e);
	forca_d[0] = char(pforca_d);
	tempo[0] = char(ptempo);

	strcpy(comando, "M");
	strcat(comando, forca_e);
	strcat(comando, forca_d);
	strcat(comando, tempo);
	this->movementsSender->send(comando);
	/*this->movementsSender->send("M");
	this->movementsSender->send(forca_e);
	this->movementsSender->send(forca_d);
	this->movementsSender->send(tempo);*/
}

void SRV::startMotor()
{
	cout << "Startou o motor\n";
	useMotor(0,0,0);
}


void SRV::restart() {
	this->movementsSender->send("$!");
}

void SRV::turnLaserOn()
{
	this->movementsSender->send("l");
}

void SRV::turnLaserOff()
{
	this->movementsSender->send("L");
}

void SRV::turnLeft() {
	this->movementsSender->send("0");
}

void SRV::turnRight() {
	this->movementsSender->send(".");
}

void SRV::driftBackLeft() {
	this->movementsSender->send("1");
}

void SRV::driftBackRight() {
	this->movementsSender->send("3");
}

void SRV::moveBack() {
	this->movementsSender->send("2");
}

void SRV::moveForward() {
	this->movementsSender->send("8");
}

void SRV::stop() {
	this->movementsSender->send("5");
}

void SRV::driftLeft() {
	this->movementsSender->send("7");
}

void SRV::driftRight() {
	this->movementsSender->send("9");
}

void SRV::driveLeft() {
	this->movementsSender->send("4");
}

void SRV::driveRight() {
	this->movementsSender->send("6");
}

void SRV::normalVision() {
	this->movementsSender->send("g_");
}
void SRV::frameDifferencingVision() {
	this->movementsSender->send("g0");
}
void SRV::colorSegmentationVision() {
	this->movementsSender->send("g1");
}
void SRV::edgeDetectionVision() {
	this->movementsSender->send("g2");
}
void SRV::obstacleDetectionVision() {
	this->movementsSender->send("g4");
}

void SRV::smallResolution() {
	this->movementsSender->send("a");
}

void SRV::standardResolution() {
	this->movementsSender->send("b");
}

void SRV::bigResolution() {
	this->movementsSender->send("c");
}

void SRV::hugeResolution() {
	this->movementsSender->send("d");
}

void SRV::increaseMotorLevel() {
	this->movementsSender->send("+");
}

void SRV::decreaseMotorLevel() {
	this->movementsSender->send("-");
}

void SRV::trimMotorBalanceLeft() {
	this->movementsSender->send("<");
}

void SRV::trimMotorBalanceRight() {
	this->movementsSender->send(">");
}

void SRV::rotateLeft() {
	this->movementsSender->send("0");
}

void SRV::rotateRight() {
	this->movementsSender->send(".");
}

float SRV::getSonarRange() {
	return this->sonarRange;
}

void SRV::setSonarRange(float s) {
	this->sonarRange = s;
}
