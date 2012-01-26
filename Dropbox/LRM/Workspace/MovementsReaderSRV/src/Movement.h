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
	/*time in 360 degrees*/
	int time;
	/*Power used in the right motor*/
	int rightMotor;
	/*Power used in the right motor*/
	int leftMotor;

public:
	Movement(int leftMotor, int rightMotor, int time);
	int getTime();
	int getLeftMotor();
	int getRightMotor();
	void setTime(int time);
	void setLeftMotor(int leftMotor);
	void setRightMotor(int rightMotor);

};

#endif /* MOVEMENT_H_ */
