/*
 * MovementsController.h
 *
 *  Created on: Aug 26, 2011
 *      Author: leandro
 */

#ifndef MOVEMENTSCONTROLLER_H_
#define MOVEMENTSCONTROLLER_H_
#include "SRV.h"
#include <queue>
using namespace std;

class MovementsController {
private:
	SRV *r;
	bool running;
	queue<float> sonarQueue;
	queue<float> laserQueue;


public:
	MovementsController(SRV *r);
	void start();
	void stop();
	void run();
	void sonarInput();
	void laserInput();
	void globalController();
	virtual ~MovementsController();
};

#endif /* MOVEMENTSCONTROLLER_H_ */
