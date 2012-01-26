/*
 * Sender.h
 *
 *  Created on: Nov 2, 2011
 *      Author: leandro
 */

#ifndef SENDER_H_
#define SENDER_H_

#include <stack>
#include <string>
#include "TCPSocket.h"

using namespace std;

class Sender {
private:
	TCPSocket *socket;
	string command;
	long long int sentSize;
	bool sending;
	float sleepTime;

public:
	Sender(TCPSocket *socket, string command, float sleepTime);
	Sender(TCPSocket *socket);
	virtual ~Sender();
	bool isSending();
	void start();
	void stop();
	void send(string cmd);
	void send(char *cmd);
	string getCommand();
	TCPSocket *getSocket();
	long long int getSentSize();
	void incSentSize();
	float getSleepTime();
	void setSleepTime(float s);

};

#endif /* SENDER_H_ */
