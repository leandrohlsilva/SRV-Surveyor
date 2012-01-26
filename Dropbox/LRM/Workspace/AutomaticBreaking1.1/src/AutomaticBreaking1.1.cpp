//============================================================================
// Name        : AutomaticBreaking.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Const.h"
#include "SRV.h"
#include "MovementsController.h"

using namespace boost::interprocess;

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	MovementsController *mc = new MovementsController(r);

	mc->start();

	sf::RenderWindow App(sf::VideoMode(320, 240, 32), "SRV Joy"); // create the main window SFML
	const sf::Input& Input = App.GetInput(); // get a reference to the input manager associated to the window

	//
	// menu
	//

	printf("> WELCOME!\n");
	printf("> Use:\n");
	printf("> Setas para movimentar.\n");
	printf("> L para ativar o laser.\n");
	printf("> N para desativar o laser.\n");
	printf("> S para obter distancia usando o sonar.\n");
	printf("> D para obter distancia usando a camera.\n");
	printf("> \n");

	//
	//
	//

	App.Display();

	r->startMotor();

	//Insert data in the array
	while (App.IsOpened())
	{

		//
		// pede imagem (retorna ponteiro da imagem)
		//

		char *img;
		int tamanhoImagem;
		img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		img = (char *) r->getImage(&tamanhoImagem,0);

		//
		// para mostrar imagem na tela...
		//

		sf::Image Image;
		Image.LoadFromMemory(img,tamanhoImagem);

		sf::Sprite Sprite(Image);
		App.Clear();
		App.Draw(Sprite);
		App.Display();


		sf::Event Event;
		while (App.GetEvent(Event))
		{
			if (Event.Type == sf::Event::Closed) App.Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape)) App.Close();
		}


		bool upKeyDown = Input.IsKeyDown(sf::Key::Up);
		bool leftKeyDown = Input.IsKeyDown(sf::Key::Left);
		bool rightKeyDown = Input.IsKeyDown(sf::Key::Right);
		bool downKeyDown = Input.IsKeyDown(sf::Key::Down);

		if ((upKeyDown) && (r->isAbleToRun())) {

			if (leftKeyDown) {
				r->driftLeft();
			} else if (rightKeyDown) {
				r->driftRight();
			} else if (downKeyDown) {
				r->stop();
			} else {
				r->moveForward();
			}

		} else if (downKeyDown) {

			if (leftKeyDown) {
				r->driftBackLeft();
			} else if (rightKeyDown) {
				r->driftBackRight();
			} else if (upKeyDown) {
				r->stop();
			} else {
				r->moveBack();
			}
		} else if ((leftKeyDown) && (!rightKeyDown) && (r->isAbleToRun())) {
			r->driveLeft();
		} else if ((!leftKeyDown) && (rightKeyDown) && (r->isAbleToRun())) {
			r->driveRight();
		} else {
			r->stop();
		}


		bool sKeyDown = Input.IsKeyDown(sf::Key::S);
		bool lKeyDown = Input.IsKeyDown(sf::Key::L);
		bool nKeyDown = Input.IsKeyDown(sf::Key::N);
		bool dKeyDown = Input.IsKeyDown(sf::Key::D);
		bool escKeyDown = Input.IsKeyDown(sf::Key::Escape);

		if (sKeyDown) {
			float retSonar = r->getSonar();
			printf("\nSonar: %f\n",retSonar);
		}
		if (dKeyDown) {
			float retDistCamera = r->getDistanceCamera();
			printf("\nDCamera: %f\n",retDistCamera);
		}
		if (lKeyDown) r->turnLaserOn();
		if (nKeyDown) r->turnLaserOff();
		if (escKeyDown) break;

	}



	return 0;
}
