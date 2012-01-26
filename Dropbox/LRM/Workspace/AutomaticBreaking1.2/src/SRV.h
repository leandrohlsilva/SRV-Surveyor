/******************************************************************************
/ * API SRV-1Q
/ *
/ * USP->ICMC->LRM
/ * pessin@gmail.com
/ *****************************************************************************/

#ifndef SRV_H_
#define SRV_H_

#include <SFML/Network.hpp>
#include <boost/signals2/mutex.hpp>

class SRV
{
private:
	boost::signals2::mutex mutex;
	unsigned short Port;
	sf::IPAddress Address;
	sf::SocketTCP Client;
	int IndiceNomeImagem;
	bool ableToRun;
	char *sendCommand(char *comando);
	char *sendCommand(char *comando, char p1);
	char *sendCommand(char *comando, char p1, char p2);
	char *sendCommand(char *comando, char p1, char p2, char p3);
public:
	SRV(sf::IPAddress _Address, unsigned short _Port);
	void turnLaserOn();
	void turnLaserOff();
	int getDistanceCamera();
	int startMotor();
	void turnLeft();
	void turnRight();
	void driveLeft();
	void driveRight();
	void driftLeft();
	void driftBackLeft();
	void driftRight();
	void driftBackRight();
	void moveForward();
	void moveBack();
	void stop();
	char* getImage(int *tamanhoImg, int grava);
	float getSonar();
	bool isAbleToRun();
	void setAbleToRun(bool a);
};

#endif /*SRV_H_*/
