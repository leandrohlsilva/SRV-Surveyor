/*
 * Sender.cpp
 *
 *  Created on: Nov 2, 2011
 *      Author: leandro
 */

#include "Sender.h"
#include "TCPSocket.h"
#include <string>
#include <boost/thread.hpp>
#include <SFML/System.hpp>

Sender::Sender(TCPSocket *socket, string cmd, float sleepTime) {
	this->socket = socket;
	this->command = cmd;
	this->sentSize = 0;
	this->sleepTime = sleepTime;
}

Sender::Sender(TCPSocket *socket) {
	string str("");
	Sender(socket, str, 1000);
}

Sender::~Sender() {

}

void run(Sender *s) {
	while(s->isSending()) {
		s->getSocket()->send(s->getCommand().c_str(), s->getCommand().size());
		s->incSentSize();
		sf::Sleep(s->getSleepTime());
	}
}

void Sender::start() {
	this->sending = true;
	boost::thread t(run, this);
}

void Sender::send(char *cmd) {
	string c(cmd);
	this->send(c);
}

void Sender::send(string cmd) {
	cout << cmd << endl;
	this->getSocket()->send(cmd.c_str(), cmd.size());
}
void Sender::stop() {
	this->sending = false;
}

string Sender::getCommand() {
	return this->command;
}

TCPSocket *Sender::getSocket() {
	return this->socket;
}

long long int Sender::getSentSize() {
	return this->sentSize;
}

void Sender::incSentSize() {
	this->sentSize++;
}

bool Sender::isSending() {
	return this->sending;
}

float Sender::getSleepTime() {
	return this->sleepTime;
}

void Sender::setSleepTime(float s) {
	this->sleepTime = s;
}
