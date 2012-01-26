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
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "MovementsController.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Const.h"

#define FATOR_DESVIO 2
#define HIST_SIZE 256

#include "SRV.h"

using namespace cv;
using namespace std;

Mat imageTrainer;
int select_object = 0;
int track_object = 0;

cv::Point origin;
cv::Rect selection;
cv::Rect track_window;


int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	r->startMotor();

	MovementsController *mc = new MovementsController(r);

	mc->start();

	sf::RenderWindow App(sf::VideoMode(320, 240, 32), "SRV Joy"); // create the main window SFML
	const sf::Input& Input = App.GetInput(); // get a reference to the input manager associated to the window


	printf("Calculando a media RGB e o desvio padrao do vermelho\n");

	cv::Scalar dRGB, mRGB;
	//cv::meanStdDev(clip, mRGB, dRGB);
	mRGB.val[2] = 245;
	mRGB.val[1] = 5;
	mRGB.val[0] = 30;
	dRGB.val[2] = 10;
	dRGB.val[1] = 5;
	dRGB.val[0] = 20;

	printf("mR = %f, mG = %f, mB = %f\n", mRGB.val[2], mRGB.val[1], mRGB.val[0]);
	printf("dR = %f, dG = %f, dB = %f\n", dRGB.val[2], dRGB.val[1], dRGB.val[0]);

	App.Display();

	//Insert data in the array
	while (App.IsOpened())
	{
		char *img;
		int tamanhoImagem;
		img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		img = (char *) r->getImage(&tamanhoImagem, 0);


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


		Mat aux(240, 320, CV_8UC3, img);
		Mat imageRGB = imdecode(aux, 1);
		//imshow( "imageRGB", imageRGB );


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

		for(int j=0;j<width;j++) for(int i=0;i<height;i++)
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
			else
			{

			}
		}

		if (!primeiroPonto) {

			mediaDireita /= ptsDireita;
			mediaEsquerda /= ptsEsquerda;
			mediaCentro /= ptsCentro;

			bool upKeyDown = Input.IsKeyDown(sf::Key::Up);
			bool leftKeyDown = Input.IsKeyDown(sf::Key::Left);
			bool rightKeyDown = Input.IsKeyDown(sf::Key::Right);
			bool downKeyDown = Input.IsKeyDown(sf::Key::Down);
			bool sKeyDown = Input.IsKeyDown(sf::Key::S);
			bool lKeyDown = Input.IsKeyDown(sf::Key::L);
			bool nKeyDown = Input.IsKeyDown(sf::Key::N);
			bool dKeyDown = Input.IsKeyDown(sf::Key::D);
			bool escKeyDown = Input.IsKeyDown(sf::Key::Escape);

			if (mediaCentro < (float)(imgDesvioPadrao.rows*0.7)) {
				//permite andar para frente

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


			} else {
				cout << "tem obstaculo detectado por camera " << endl;
				if (downKeyDown) {

					if (leftKeyDown) {
						r->driftBackLeft();
					} else if (rightKeyDown) {
						r->driftBackRight();
					} else if (upKeyDown) {
						r->stop();
					} else {
						r->moveBack();
					}
				} else if (escKeyDown) { break; }
				else r->stop();
			}

		}

		imshow("imgDesvioPadrao", imgDesvioPadrao);

		free(img);

	}

	delete r;

	return 0;
}
