/*
 * Receiver.h
 *
 *  Created on: Nov 2, 2011
 *      Author: leandro
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <queue>
#include "TCPSocket.h"
#include "Message.h"

class Receiver {
private:
	queue<Message*> *buffer;
	TCPSocket *socket;
	bool receiving;
public:
	Receiver(TCPSocket *socket);
	void start();
	void stop();
	virtual ~Receiver();
	bool isReceiving();
	void pushBuffer(Message * msg);
	void popBuffer();
	Message *topBuffer();
	bool isBufferEmpty();
	void cleanBuffer();
	TCPSocket *getSocket();
	enum Constants {
		MAX_LEN_BUF = 10000
	};
};

#endif /* RECEIVER_H_ */
