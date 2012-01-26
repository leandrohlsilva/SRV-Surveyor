//============================================================================
// Name        : DesktopRemoteControl.cpp
// Author      : Leandro Henrique Lourenco da Silva
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>

#include <stdio.h>
#include <string.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#ifndef CONSTANTES_H
        #define CONSTANTES_H
        #include "Const.h"
#endif   

#include "SRV.h"

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo
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
	
	while (App.IsOpened())
    {
		//
		// pede imagem (retorna ponteiro da imagem)
		//

		char *img;
		int tamanhoImagem;
		img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		img = (char *) r->getImage(&tamanhoImagem,1);

		//
		// para mostrar imagem na tela...
		//

		sf::Image Image;
		Image.LoadFromMemory(img,tamanhoImagem);

		sf::Sprite Sprite(Image);
		App.Clear();
		App.Draw(Sprite);
		App.Display();

		//
		// process events (teclado)
		//

        sf::Event Event;
        while (App.GetEvent(Event))
        {
            if (Event.Type == sf::Event::Closed) App.Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape)) App.Close();
        }

		//
		// setas
		//

		if (Event.Key.Code == sf::Key::Left && Event.Type == sf::Event::KeyPressed) r->ativaMotor(-70,70,12); // menos de 20 graus
		if (Event.Key.Code == sf::Key::Right && Event.Type == sf::Event::KeyPressed) r->ativaMotor(70,-70,12); // menos de 20 graus
		if (Event.Key.Code == sf::Key::Up && Event.Type == sf::Event::KeyPressed) r->ativaMotor(70,70,54); // menos de 30 cm
		if (Event.Key.Code == sf::Key::Down && Event.Type == sf::Event::KeyPressed) r->ativaMotor(-70,-70,54); // menos de 30 cm

		//
		// comandos {sonar, ativa laser, desativa laser}
		//

		bool sKeyDown = Input.IsKeyDown(sf::Key::S);
		bool lKeyDown = Input.IsKeyDown(sf::Key::L);
		bool nKeyDown = Input.IsKeyDown(sf::Key::N);
		bool dKeyDown = Input.IsKeyDown(sf::Key::D);

		if (sKeyDown) { float retSonar = r->getSonar(); printf("\nSonar: %f\n",retSonar); }
		if (dKeyDown) { float retDistCamera = r->getDistanceCamera(); printf("\nDCamera: %f\n",retDistCamera); }
		if (lKeyDown) r->ativaLaser();
		if (nKeyDown) r->desativaLaser();
	}

	return 0;
}
