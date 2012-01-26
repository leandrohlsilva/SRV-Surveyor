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
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "SRV.h"
#include "MovementsController.h"
#include <boost/thread.hpp>
#include <SFML/System.hpp>
#include <iostream>

using namespace std;
using namespace boost::interprocess;
using namespace boost;

bool running = false;

MovementsController::MovementsController(SRV *r) {
	this->r = r;

}

MovementsController::~MovementsController() {
	running = false;
}

void MovementsController::start() {
	//TODO colocar log4cpp aqui
	//inicia os processos
	running = true;

	run();
}

void MovementsController::stop() {
	running = false;
}

void MovementsController::sonarInput() {

	while (running) {
		//TODO colocar log4cpp aqui
		float s = r->getSonar();
		//float s = 31.0;
		sonarQueue.push(s);
		sf::Sleep(.25);
	}

}

void MovementsController::laserInput() {
	while (running) {
		//TODO colocar log4cpp aqui
		float d = r->getDistanceCamera();
		laserQueue.push(d);
		//cout << "D = " << d << " cm" << endl;
		sf::Sleep(5);
	}
}

void MovementsController::globalController() {
	while (running) {


		int size = sonarQueue.size();

		//cout << "laserQueue.size() = " << size << endl;
		if (size > 0) {
			float l = sonarQueue.front();
			sonarQueue.pop();

			//soh anda se o robo estiver no minimo permitido ou mais de distancia de um objeto
			this->r->setAbleToRun(l > 30);
			if (!r->isAbleToRun()) {
				this->r->stop();
				cout << "PArou pelo sonar" << endl;
			}

			cout << "sonar = " << l << endl;
		}

		size = laserQueue.size();

		//cout << "laserQueue.size() = " << size << endl;
		if (size > 0) {
			float l = laserQueue.front();
			laserQueue.pop();

			//soh anda se o robo estiver a 30cm ou mais de distancia de um objeto
			this->r->setAbleToRun(l > 30);


			cout << "laser = " << l << endl;
		}

		sf::Sleep(.25);

	}

	cout << "It is not more running" << endl;
}

void laserInputMethod(MovementsController *mc) {
	mc->laserInput();
}

void sonarInputMethod(MovementsController *mc) {
	mc->sonarInput();
}

void globalControllerMethod(MovementsController *mc) {
	mc->globalController();
}

void MovementsController::run() {

	cout << "entrou no run" << endl;

	boost::thread sonarThread(sonarInputMethod, this);
	boost::thread globalController(globalControllerMethod, this);

}


#endif
