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
	unsigned short Port;
	sf::IPAddress Address;
	int IndiceNomeImagem;
	bool ableToRun;
	char *sendCommand(char *comando);
	char *sendCommand(char *comando, char p1);
	char *sendCommand(char *comando, char p1, char p2);
	char *sendCommand(char *comando, char p1, char p2, char p3);
public:
	SRV(sf::IPAddress _Address, unsigned short _Port);
	sf::SocketTCP Client;
	boost::signals2::mutex mutex;
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
	void normalVision();
	void frameDifferencingVision();
	void colorSegmentationVision();
	void edgeDetectionVision();
	void obstacleDetectionVision();
	char* getImage(int *tamanhoImg);
	float getSonar();
	bool isAbleToRun();
	void setAbleToRun(bool a);
};

#endif /*SRV_H_*/
