/*
 * Message.h
 *
 *  Created on: Nov 3, 2011
 *      Author: leandro
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

class Message {
private:
	char *msg;
	int length;
public:
	Message(char *msg, int length);
	virtual ~Message();
	char *getMsg();
	int getLength();
};

#endif /* MESSAGE_H_ */
