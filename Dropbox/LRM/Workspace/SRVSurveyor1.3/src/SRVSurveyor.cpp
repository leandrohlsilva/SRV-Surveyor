//============================================================================
// Name        : SRVSuveryor1.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Ansi-style
//============================================================================


#include <stdio.h>
#include <string.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "SRV.h"

using namespace sf::Key;


void handleKey(sf::RenderWindow *App, const sf::Input *Input, SRV *r) {

	//Insert data in the array
	while (App->IsOpened())
	{

		sf::Sleep(.1);

		bool upKeyDown = Input->IsKeyDown(sf::Key::Up);
		bool leftKeyDown = Input->IsKeyDown(sf::Key::Left);
		bool rightKeyDown = Input->IsKeyDown(sf::Key::Right);
		bool downKeyDown = Input->IsKeyDown(sf::Key::Down);
		if (upKeyDown) {

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
		} else if ((leftKeyDown) && (!rightKeyDown)) {
			r->driveLeft();
		} else if ((!leftKeyDown) && (rightKeyDown)) {
			r->driveRight();
		} else {
			r->stop();
		}


		bool sKeyDown = Input->IsKeyDown(sf::Key::S);
		bool lKeyDown = Input->IsKeyDown(sf::Key::L);
		bool nKeyDown = Input->IsKeyDown(sf::Key::N);
		bool dKeyDown = Input->IsKeyDown(sf::Key::D);
		bool escKeyDown = Input->IsKeyDown(sf::Key::Escape);
		bool oneKeyDown = Input->IsKeyDown(sf::Key::Num1);
		bool twoKeyDown = Input->IsKeyDown(sf::Key::Num2);
		bool threeKeyDown = Input->IsKeyDown(sf::Key::Num3);
		bool fourKeyDown = Input->IsKeyDown(sf::Key::Num4);
		bool fiveKeyDown = Input->IsKeyDown(sf::Key::Num5);

		if (oneKeyDown) {
			r->normalVision();
		} else {
			if (twoKeyDown) {
				r->frameDifferencingVision();
			}
			if (threeKeyDown) {
				r->colorSegmentationVision();
			}
			if (fourKeyDown) {
				r->edgeDetectionVision();
			}
			if (fiveKeyDown) {
				r->obstacleDetectionVision();
			}
		}

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

}

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	sf::RenderWindow App(sf::VideoMode(320, 240, 32), "SRV Joy"); // create the main window SFML
	const sf::Input& Input = App.GetInput(); // get a reference to the input manager associated to the window

	printf("> WELCOME!\n");
	printf("> Use:\n");
	printf("> Setas para movimentar.\n");
	printf("> L para ativar o laser.\n");
	printf("> N para desativar o laser.\n");
	printf("> S para obter distancia usando o sonar.\n");
	printf("> D para obter distancia usando a camera.\n");
	printf("> \n");


	App.Display();

	r->startMotor();

	boost::thread threadTeclado(handleKey, &App, &Input, r);

	//Insert data in the array
	while (App.IsOpened())
	{

		sf::Event Event;
		while (App.GetEvent(Event))
		{
			if (Event.Type == sf::Event::Closed) App.Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape)) App.Close();
		}

		//
		// pede imagem (retorna ponteiro da imagem)
		//

		char *img;
		int tamanhoImagem = 0;
		//img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		//printf("Iniciando a imagem\n");
		img = (char *) r->getImage(&tamanhoImagem);

		//
		// para mostrar imagem na tela...
		//

		if (img != NULL && tamanhoImagem > 0) {
			sf::Image Image;
			if (Image.LoadFromMemory(img,tamanhoImagem)) {
				//printf("Exibiu a imagem\n");

				sf::Sprite Sprite(Image);
				App.Clear();
				App.Draw(Sprite);
				App.Display();
			} else {
				printf("Sprite nao pode ser carregada\n");
			}

			free(img);
		}


	}

	delete r;

	return 0;
}


