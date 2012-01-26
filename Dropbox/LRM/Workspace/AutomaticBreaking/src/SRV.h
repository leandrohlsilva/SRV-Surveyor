/******************************************************************************
/ * API SRV-1Q
/ *
/ * USP->ICMC->LRM
/ * pessin@gmail.com
/ *****************************************************************************/

#ifndef SRV_H_
#define SRV_H_

#include <SFML/Network.hpp>
#include <boost/signals2/mutex.hpp>

class SRV
{
	private:
		boost::signals2::mutex mutex;
		unsigned short Port;
		sf::IPAddress Address;
		sf::SocketTCP Client;
		int IndiceNomeImagem;
		bool ableToRun;

	public:		

		SRV(sf::IPAddress _Address, unsigned short _Port);
		int ativaLaser();
		int desativaLaser();
		int getDistanceCamera();
		int ativaMotor(int pforca_e, int pforca_d, int ptempo);
		void paraMotor();
		char* getImage(int *tamanhoImg, int grava);
		float getSonar();
		bool isAbleToRun();
		void setAbleToRun(bool a);
};

#endif /*SRV_H_*/
