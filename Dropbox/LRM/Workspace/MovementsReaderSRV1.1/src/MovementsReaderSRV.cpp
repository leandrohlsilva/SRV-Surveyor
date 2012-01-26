//============================================================================
// Name        : MovementsReaderSRV.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : SRV's Movement Controller, Ansi-style
//============================================================================

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
#include "Const.h"
#include "MovementsReader.h"
#include "Movement.h"
#include "SRV.h";

int main(int argc, char* argv[]) {

	string arq = "arquivo.txt";
	if (argc > 1) arq = argv[1];

	MovementsReader *reader = new MovementsReader((char*) arq.c_str());

	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	r->startMotor();

	bool loop = false;
	if(argc > 2) loop = (bool) atoi(argv[2]);

	do {

		Movement *mov;
		while (reader->hasNextMovement()) {
			mov = reader->nextMovement();
			switch (mov->getMovement()) {
			case Movement::DriftLeft:
				r->driftLeft();
				break;
			case Movement::Forward:
				r->moveForward();
				break;
			case Movement::DriftRight:
				r->driftRight();
				break;
			case Movement::Left:
				r->driveLeft();
				break;
			case Movement::Stop:
				r->stop();
				break;
			case Movement::Right:
				r->driveRight();
				break;
			case Movement::DriftBackLeft:
				r->driftBackLeft();
				break;
			case Movement::Back:
				r->moveBack();
				break;
			case Movement::DriftBackRight:
				r->driftBackRight();
				break;
			default:
				break;
			}
			sf::Sleep(mov->getDuration());
		}

		reader->restart();

	} while (loop);

	r->stop();

	delete r;
	delete reader;

	return 0;
}
