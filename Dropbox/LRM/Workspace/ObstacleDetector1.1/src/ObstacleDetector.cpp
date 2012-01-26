//============================================================================
// Name        : ImageBasedController.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "Const.h"

#define FATOR_DESVIO 2
#define HIST_SIZE 256

#include "SRV.h"

using namespace std;
using namespace cv;

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo



	//printf("Calculando a media RGB e o desvio padrao do vermelho\n");

	cv::Scalar dRGB, mRGB;
	//cv::meanStdDev(clip, mRGB, dRGB);
	mRGB.val[2] = 220;
	mRGB.val[1] = 20;
	mRGB.val[0] = 20;
	dRGB.val[2] = 40;
	dRGB.val[1] = 20;
	dRGB.val[0] = 20;

	//printf("mR = %f, mG = %f, mB = %f\n", mRGB.val[2], mRGB.val[1], mRGB.val[0]);
	//printf("dR = %f, dG = %f, dB = %f\n", dRGB.val[2], dRGB.val[1], dRGB.val[0]);

	sf::RenderWindow App(sf::VideoMode(320, 240, 32), "SRV Joy");
	const sf::Input& Input = App.GetInput();

	while (App.IsOpened())
	{

		char *img;
		int tamanhoImagem;
		img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		img = (char *) r->getImage(&tamanhoImagem, 0);

		if (img != NULL) {

			sf::Image Image;
			if (!Image.LoadFromMemory(img,tamanhoImagem)) {
				//cout << "Problema com a imagem" << endl;
				sf::Sleep(.1);
				continue;
			} else {

				sf::Sprite Sprite(Image);
				App.Clear();
				App.Draw(Sprite);
				App.Display();

			}
		}

		Mat imageRGB;
		//try {
			Mat aux(240, 320, CV_8UC3, img);
			imageRGB = imdecode(aux, 1);
			//imshow( "imageRGB", imageRGB );
		//} catch (Exception e) {
		//	continue;
		//}



		Mat imgDesvioPadrao(imageRGB);

		// get the image data
		int height    = imgDesvioPadrao.rows;
		int width     = imgDesvioPadrao.cols;
		int step      = imgDesvioPadrao.step;
		int channels  = imgDesvioPadrao.channels();
		uchar *data   = imgDesvioPadrao.data;
		double mR = mRGB.val[2];
		double mG = mRGB.val[1];
		double mB = mRGB.val[0];
		double dR = dRGB.val[2];
		double dG = dRGB.val[1];
		double dB = dRGB.val[0];

		double mediaDireita = 0;
		int ptsDireita = 0;
		double mediaCentro = 0;
		int ptsCentro = 0;
		double mediaEsquerda = 0;
		int ptsEsquerda = 0;

		int limitanteEsquerda = imgDesvioPadrao.cols/3;
		int limitanteDireita = (2*imgDesvioPadrao.cols)/3;

		int proxObstaculoDireita = 0;
		int proxObstaculoEsquerda = 0;
		int proxObstaculoCentro = 0;


		int x1, x2, y1, y2;
		bool primeiroPonto = true;

		for(int j=imgDesvioPadrao.cols*.2;j<width*.8;j++) for(int i=height;i>-1;i--)
		{
			if ((data[i*step+j*channels+0] >= (mB-dB*FATOR_DESVIO) && data[i*step+j*channels+0] <= (mB+dB*FATOR_DESVIO)) &&
					(data[i*step+j*channels+1] >= (mG-dG*FATOR_DESVIO) && data[i*step+j*channels+1] <= (mG+dG*FATOR_DESVIO)) &&
					(data[i*step+j*channels+2] >= (mR-dR*FATOR_DESVIO) && data[i*step+j*channels+2] <= (mR+dR*FATOR_DESVIO)) )
			{
				//eh a cor do objeto
				if (primeiroPonto) {
					x1 = j;
					y1 = i;
					x2 = j;
					y2 = i;
					primeiroPonto = false;
				}
				x1 = MIN(x1, j);
				x2 = MAX(x2, j);
				y1 = MAX(y1, i);
				y2 = MIN(y2, i);

				if (j < limitanteEsquerda) {
					mediaEsquerda += i;
					ptsEsquerda++;
					proxObstaculoEsquerda = max(proxObstaculoEsquerda, i);
				} else if (j > limitanteDireita) {
					mediaDireita += i;
					ptsDireita++;
					proxObstaculoDireita = max(proxObstaculoDireita, i);
				} else {
					mediaCentro += i;
					ptsCentro++;
					proxObstaculoCentro = max(proxObstaculoCentro, i);
				}

				cv::Point p1(j, i);
				cv::Point p2(j, 0);
				cv::rectangle(imgDesvioPadrao, p1, p2, CV_RGB(255,0,0), CV_FILLED, 8, 0);

				//nao precisa continuar processando
				break;

			}
		}


		mediaDireita /= ptsDireita;
		mediaEsquerda /= ptsEsquerda;
		mediaCentro /= ptsCentro;

		double min = std::min(mediaDireita, mediaEsquerda);
		min = std::min(min, mediaCentro);

		if (mediaCentro > (float)(imgDesvioPadrao.rows*0.8)
				&& mediaEsquerda > mediaCentro && mediaDireita > mediaCentro) {
			cout << "Canto. Gire" << endl;
			r->ativaMotor(-120, 120, 50);
		} else {

			if (mediaCentro < (float)(imgDesvioPadrao.rows*0.5)) {
				r->ativaMotor(100,100,10);
			} else if (mediaCentro < (float)(imgDesvioPadrao.rows*0.8)) {

				//temos que ir para a direcao onde o obstaculo esta mais longe
				if (mediaDireita < mediaEsquerda) {
					if (mediaDireita < mediaCentro) {
						//analisa o quanto pode virar e vira pra direita
						cout << "Vire para direita" << endl;
						r->ativaMotor(80, -10, 10);
					} else {
						//analisa o quanto pode andar e continua reto
						cout << "Continue reto" << endl;
						r->ativaMotor(60, 60, 10);
					}
				} else {
					if (mediaEsquerda < mediaCentro) {
						//analisa o quanto pode virar e vira pra esquerda
						cout << "Vire para esquerda" << endl;
						r->ativaMotor(-10, 80, 10);
					} else {
						//analisa o quanto pode andar e continua reto
						cout << "Continue reto" << endl;
						r->ativaMotor(60, 60, 10);
					}
				}

			} else {
				//cout << "tem obstaculo e precisa girar" << endl;

				if (proxObstaculoDireita < proxObstaculoEsquerda) {
					//gira pra direita
					cout << "Gire para direita" << endl;
					r->ativaMotor(70, -70, 10);
				} else {
					//gira pra esquerda
					cout << "Gire para esquerda" << endl;
					r->ativaMotor(-70, 70, 10);
				}
			}
		}


		imshow("imgDesvioPadrao", imgDesvioPadrao);

		sf::Event Event;
		while (App.GetEvent(Event))
		{
			if (Event.Type == sf::Event::Closed) App.Close();
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape)) App.Close();
		}


		free(img);

		// funçao que aguarda evento de teclado
		unsigned char tecla = (unsigned char) cv::waitKey(33);

		if (tecla == 27) break;


	}

	delete r;

	return 0;
}
