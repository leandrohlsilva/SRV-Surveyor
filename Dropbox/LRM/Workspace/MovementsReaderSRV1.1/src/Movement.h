/*
 * Movement.h
 *
 *  Created on: Aug 22, 2011
 *      Author: leandro
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

class Movement {

private:
	float duration;
	char movement;

public:
	Movement(char mov, float time);
	float getDuration();
	char getMovement();
	void setDuration(float dur);
	void setMovement(char mov);
	enum MovementType {
		DriftLeft = '7',
		Forward = '8',
		DriftRight = '9',
		Left = '4',
		Stop = '5',
		Right = '6',
		DriftBackLeft = '1',
		Back = '2',
		DriftBackRight = '3'
	};

};

#endif /* MOVEMENT_H_ */
