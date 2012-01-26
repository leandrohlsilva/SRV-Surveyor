/******************************************************************************
/ * API SRV-1Q
/ *
/ * USP->ICMC->LRM
/ * pessin@gmail.com
/ *****************************************************************************/

#ifndef SRV_H_
#define SRV_H_

#include <SFML/Network.hpp>

class SRV
{
	private:

		unsigned short Port;
		sf::IPAddress Address;
		sf::SocketTCP Client;
		int IndiceNomeImagem;

	public:		

		SRV(sf::IPAddress _Address, unsigned short _Port);
		int ativaLaser();
		int desativaLaser();
		int getDistanceCamera();
		int ativaMotor(int pforca_e, int pforca_d, int ptempo);
		//void motorStop();
		char* getImage(int *tamanhoImg, int grava);
		//char* getImage2(int *s);
		float getSonar();
};

#endif /*SRV_H_*/
