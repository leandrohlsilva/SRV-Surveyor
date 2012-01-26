/*
 * Movement.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: leandro
 */

#include "Movement.h"

Movement::Movement(int leftMotor, int rightMotor, int time) {
	this->time = time;
	this->rightMotor = rightMotor;
	this->leftMotor = leftMotor;
}

int Movement::getTime() {
	return time;
}

int Movement::getLeftMotor() {
	return leftMotor;
}

int Movement::getRightMotor() {
	return rightMotor;
}

void Movement::setTime(int time) {
	this->time = time;
}

void Movement::setLeftMotor(int leftMotor) {
	this->leftMotor = leftMotor;
}

void Movement::setRightMotor(int rightMotor) {
	this->rightMotor = rightMotor;
}
