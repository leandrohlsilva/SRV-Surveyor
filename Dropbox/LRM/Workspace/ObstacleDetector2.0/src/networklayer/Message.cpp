/*
 * Message.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: leandro
 */

#include "Message.h"
#include "stdlib.h"

Message::Message(char *msg, int len) {
	this->msg = msg;
	this->length = len;
}

Message::~Message() {
	free(this->msg);
}

char *Message::getMsg() {
	return this->msg;
}

int Message::getLength() {
	return this->length;
}
