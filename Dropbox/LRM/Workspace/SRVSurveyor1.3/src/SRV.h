/******************************************************************************
/ * API SRV-1Q
/ *
/ * USP->ICMC->LRM
/ * pessin@gmail.com
/ *****************************************************************************/

#ifndef SRV_H_
#define SRV_H_

#include <boost/signals2/mutex.hpp>
#include "TCPSocket.h"

class SRV
{
private:
	bool ableToRun;
	string address;
	unsigned short port;
	bool sendingImage;
	char *sendCommand(char *comando);
	char *sendCommand(char *comando, char p1);
	char *sendCommand(char *comando, char p1, char p2);
	char *sendCommand(char *comando, char p1, char p2, char p3);
public:
	boost::signals2::mutex mutex;
	TCPSocket Client;
	SRV(const string &foreignAddress, unsigned short foreignPort);
	void reconnect();
	bool isSendingImage();
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
	enum Const {
		MAX_LIMITE_ENVIO = 1
	};
};

#endif /*SRV_H_*/
