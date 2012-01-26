/*
 * MovementsController.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: leandro
 *
 *  A ideia desta classe eh tornar cada input ter decisoes indepentes um do outro,
 *  mas o controlador geral relaciona cada input e tem a opiniao mais forte final.
 *
 *  Para controlar o que acontece, sera usado log4cpp e ver o que cada input
 *  vai gerar, bem como as decisoes do controller principal.
 *
 */

#ifndef MOVEMENTS_CONTROLLER_H_
#define MOVEMENTS_CONTROLLER_H_

#include "SRV.h"
#include "MovementsController.h"
#include <boost/thread.hpp>
#include <iostream>

using namespace std;

using namespace boost;

bool running = false;

MovementsController::MovementsController(SRV *r) {
	this->r = r;
	//TODO colocar log4cpp aqui

	//inicia os processos
	running = true;

	run();


}

MovementsController::~MovementsController() {
	running = false;
}

void MovementsController::sonarInput() {

	while (running) {
		//TODO colocar log4cpp aqui
		float s = r->getSonar();
		sonarQueue.push(s);
		cout << "S = " << s << " cm" << endl;
		//cout << "sonarInput" << endl;

	}

}

void MovementsController::laserInput() {
	while (running) {
		//TODO colocar log4cpp aqui
		float d = r->getDistanceCamera();
		laserQueue.push(d);
		cout << "D = " << d << " cm" << endl;
		//cout << "laserInput" << endl;
	}
}

void laserInputMethod(MovementsController *mc) {
	mc->laserInput();
}

void sonarInputMethod(MovementsController *mc) {
	mc->sonarInput();
}

void MovementsController::run() {

	cout << "entrou no run" << endl;

	boost::thread sonarThread(sonarInputMethod, this);
	//boost::thread laserThread(laserInputMethod, this);


}


#endif
