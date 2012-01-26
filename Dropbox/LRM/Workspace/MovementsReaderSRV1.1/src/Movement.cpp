/*
 * Movement.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: leandro
 */

#include "Movement.h"

Movement::Movement(char mov, float duration) {
	this->duration = duration;
	this->movement = mov;
}

float Movement::getDuration() {
	return duration;
}

char Movement::getMovement() {
	return this->movement;
}

void Movement::setDuration(float dur) {
	this->duration = dur;
}

void Movement::setMovement(char mov) {
	this->movement = mov;
}
