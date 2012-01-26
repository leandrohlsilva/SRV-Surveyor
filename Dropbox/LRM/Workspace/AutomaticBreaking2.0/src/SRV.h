
#ifndef SRV_H_
#define SRV_H_

#include "networklayer/Sender.h"
#include "networklayer/Receiver.h"
#include "networklayer/TCPSocket.h"
#include <string>
#include <vector>


class SRV
{
private:
	string address;
	unsigned short port;
	TCPSocket *socket;
	vector<Sender*> automaticSenders;
	Sender *movementsSender;
	Receiver *receiver;
	char *frameData;
	float sonarRange;
	int dataLength;
	bool running;


public:
	SRV(string foreignAddress, unsigned short foreignPort);
	void start(bool sonar);
	bool isRunning();
	Receiver *getReceiver();
	char *getFrameData();
	int getDataLength();
	void setFrameData(char *dados, int length);
	void turnLaserOn();
	void turnLaserOff();
	void restart();
	void startMotor();
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
	void rotateLeft();
	void rotateRight();
	void normalVision();
	void frameDifferencingVision();
	void colorSegmentationVision();
	void edgeDetectionVision();
	void obstacleDetectionVision();
	void smallResolution();
	void standardResolution();
	void bigResolution();
	void hugeResolution();
	void increaseMotorLevel();
	void decreaseMotorLevel();
	void trimMotorBalanceLeft();
	void trimMotorBalanceRight();
	bool isAbleToRun();
	void setAbleToRun(bool a);
	float getSonarRange();
	void setSonarRange(float s);

	enum Constants {
		MILLI_IMAGE_REQUEST = 300
	};

};

#endif /*SRV_H_*/
