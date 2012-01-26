//============================================================================
// Name        : ConcurrentReadingInputs.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Read inputs concurrently, Ansi-style
//============================================================================

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Const.h"
#include "MovementsReader.h"
#include "Movement.h"
#include "MovementsController.h"


using namespace std;

int main() {

	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	MovementsController *mc = new MovementsController(r);

	while (1);

	delete mc;

	return 0;
}
