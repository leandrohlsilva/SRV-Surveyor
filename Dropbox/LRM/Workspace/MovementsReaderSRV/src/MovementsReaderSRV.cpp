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

	bool loop = false;
	if(argc > 2) loop = (bool) atoi(argv[2]);

	do {

		Movement *mov;
		while (reader->hasNextMovement()) {
			mov = reader->nextMovement();
			cout << "E = " << mov->getLeftMotor() << " ";
			cout << "D = " << mov->getRightMotor() << " ";
			cout << "T = " << mov->getTime() << endl;
			r->ativaMotor(mov->getLeftMotor(), mov->getRightMotor(), mov->getTime());

		}

	} while (loop);

	delete r;
	delete reader;

	return 0;
}
