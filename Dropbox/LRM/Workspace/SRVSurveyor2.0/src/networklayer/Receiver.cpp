/*
 * Receiver.cpp
 *
 *  Created on: Nov 2, 2011
 *      Author: leandro
 */

#include "Receiver.h"
#include "Message.h"
#include <boost/thread.hpp>

Receiver::Receiver(TCPSocket *socket) {
	this->socket = socket;
	this->receiving = false;
	this->buffer = new queue<Message *>();
}

void run(Receiver *r) {
	//socket->recv(buffer, tm);
	while(r->isReceiving()) {
		char b[r->MAX_LEN_BUF];
		int rt = r->getSocket()->recv(b, r->MAX_LEN_BUF);
		if (rt > r->MAX_LEN_BUF) {
			cout << "Retorno maior que o limite definido!" << endl;
			continue;
		}

		Message *m = new Message(b, rt);
		r->pushBuffer(m);
	}
}

void Receiver::start() {
	this->receiving = true;
	boost::thread t(run, this);
}

void Receiver::stop() {
	this->receiving = false;
}

void Receiver::pushBuffer(Message* str) {
	this->buffer->push(str);
}

void Receiver::popBuffer() {
	this->buffer->pop();
}

Message* Receiver::topBuffer() {
	return this->buffer->front();
}

void Receiver::cleanBuffer() {

	delete this->buffer;
	this->buffer = new queue<Message *>();

}

TCPSocket *Receiver::getSocket() {
	return this->socket;
}

bool Receiver::isReceiving() {
	return this->receiving;
}

bool Receiver::isBufferEmpty() {
	return this->buffer->empty();
}

Receiver::~Receiver() {
	delete this->socket;
	delete this->buffer;
}
