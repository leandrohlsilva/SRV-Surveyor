//============================================================================
// Name        : 0.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "SRV.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {

	SRV *robo = new SRV("169.254.0.10", 10001);
	robo->start();

	//robo->startMotor();

	sf::RenderWindow App(sf::VideoMode(320, 240, 32), "SRV Joy");
	const sf::Input& Input = App.GetInput();
	//App.

	App.Display();

	char* data = NULL;
	int tamanhoImagem = 0;

	while(App.IsOpened()) {

		//imshow("MainFrame", robo->getFrame());
		data = robo->getFrameData();
		tamanhoImagem = robo->getDataLength();

		if (data != NULL) {

			sf::Image Image;
			if (!Image.LoadFromMemory(data,tamanhoImagem)) {
				//cout << "Problema com a imagem" << endl;
				continue;
			} else {

				sf::Sprite Sprite(Image);
				App.Clear();
				App.Draw(Sprite);
				App.Display();

			}
		}

		//sf::Sleep(.3);

		sf::Event Event;
		while (App.GetEvent(Event))
		{
			if (Event.Type == sf::Event::Closed) App.Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape)) App.Close();
		}

		//sincronizacao entre o buffer do teclado e o buffer da imagem
		sf::Sleep(.1);


		bool upKeyDown = Input.IsKeyDown(sf::Key::Up);
		bool leftKeyDown = Input.IsKeyDown(sf::Key::Left);
		bool rightKeyDown = Input.IsKeyDown(sf::Key::Right);
		bool downKeyDown = Input.IsKeyDown(sf::Key::Down);
		if (upKeyDown) {

			if (leftKeyDown) {
				robo->driftLeft();
			} else if (rightKeyDown) {
				robo->driftRight();
			} else if (downKeyDown) {
				robo->stop();
			} else {
				robo->moveForward();
			}

		} else if (downKeyDown) {

			if (leftKeyDown) {
				robo->driftBackLeft();
			} else if (rightKeyDown) {
				robo->driftBackRight();
			} else if (upKeyDown) {
				robo->stop();
			} else {
				robo->moveBack();
			}
		} else if ((leftKeyDown) && (!rightKeyDown)) {
			robo->driveLeft();
		} else if ((!leftKeyDown) && (rightKeyDown)) {
			robo->driveRight();
		} else {
			robo->stop();
		}

		if (Event.Type == sf::Event::KeyReleased) {
			if ((!upKeyDown) && (!downKeyDown) && (!leftKeyDown) && (!rightKeyDown)) {
				robo->stop();
			}
		}


		bool lKeyDown = Input.IsKeyDown(sf::Key::L);
		bool nKeyDown = Input.IsKeyDown(sf::Key::N);
		bool escKeyDown = Input.IsKeyDown(sf::Key::Escape);
		bool oneKeyDown = Input.IsKeyDown(sf::Key::Num1);
		bool twoKeyDown = Input.IsKeyDown(sf::Key::Num2);
		bool threeKeyDown = Input.IsKeyDown(sf::Key::Num3);
		bool fourKeyDown = Input.IsKeyDown(sf::Key::Num4);
		bool fiveKeyDown = Input.IsKeyDown(sf::Key::Num5);
		bool plusKeyDown = Input.IsKeyDown(sf::Key::Add);
		bool minusKeyDown = Input.IsKeyDown(sf::Key::Subtract);
		bool commaKeyDown = Input.IsKeyDown(sf::Key::Comma);
		bool slashKeyDown = Input.IsKeyDown(sf::Key::Slash);
		bool aKeyDown = Input.IsKeyDown(sf::Key::A);
		bool bKeyDown = Input.IsKeyDown(sf::Key::B);
		bool cKeyDown = Input.IsKeyDown(sf::Key::C);
		bool dKeyDown = Input.IsKeyDown(sf::Key::D);

		if (oneKeyDown) {
			robo->normalVision();
		} else {
			if (twoKeyDown) {
				robo->frameDifferencingVision();
			}
			if (threeKeyDown) {
				robo->colorSegmentationVision();
			}
			if (fourKeyDown) {
				robo->edgeDetectionVision();
			}
			if (fiveKeyDown) {
				robo->obstacleDetectionVision();
			}
		}


		if (plusKeyDown) robo->increaseMotorLevel();
		if (minusKeyDown) robo->decreaseMotorLevel();
		if (aKeyDown) robo->smallResolution();
		if (bKeyDown) robo->standardResolution();
		if (cKeyDown) robo->bigResolution();
		if (dKeyDown) robo->hugeResolution();
		if (commaKeyDown) robo->trimMotorBalanceLeft();
		if (slashKeyDown) robo->trimMotorBalanceRight();
		if (lKeyDown) robo->turnLaserOn();
		if (nKeyDown) robo->turnLaserOff();
		if (escKeyDown) break;

	}

	delete robo;
	free(data);

	return 0;
}
