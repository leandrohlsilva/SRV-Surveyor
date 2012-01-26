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
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#define FATOR_DESVIO 3

#include "SRV.h"

using namespace cv;
using namespace std;

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	r->start();

	r->startMotor();

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

	char* data = NULL;
	int tamanhoImagem = 0;

	while(1) {

		data = r->getFrameData();
		tamanhoImagem = r->getDataLength();

		if (data != NULL) {

			sf::Image Image;
			if (!Image.LoadFromMemory(data,tamanhoImagem)) {
				//cout << "Problema com a imagem" << endl;
				continue;
			} else {


			}
		} else {
			continue;
		}


		Mat imageRGB;
		try {
			Mat aux(240, 320, CV_8UC3, data);
			imageRGB = imdecode(aux, 1);
			imshow( "imageRGB", imageRGB );

		} catch (Exception e) {
			cout << "Excecao " << e.msg << " aconteceu" << endl;
			continue;
		}


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
		}

		if (!primeiroPonto) {

			mediaDireita /= ptsDireita;
			mediaEsquerda /= ptsEsquerda;
			mediaCentro /= ptsCentro;

			//sincronizacao entre o buffer do teclado e o buffer da imagem
			//sf::Sleep(.1);



			if (mediaCentro > (float)(imgDesvioPadrao.rows*0.8)
					&& mediaEsquerda > mediaCentro && mediaDireita > mediaCentro) {
				cout << "Canto. Gire" << endl;
				r->useMotor(-120, 120, 50);
			} else

				if (mediaCentro < (float)(imgDesvioPadrao.rows*0.5)) {
					r->useMotor(80,80,10);
				} else if (mediaCentro < (float)(imgDesvioPadrao.rows*0.8)) {

					//temos que ir para a direcao onde o obstaculo esta mais longe
					if (mediaDireita < mediaEsquerda) {
						if (mediaDireita < mediaCentro) {
							//analisa o quanto pode virar e vira pra direita
							cout << "Vira pra direita" << endl;
							r->useMotor(80, -10, 10);
						} else {
							//analisa o quanto pode andar e continua reto
							cout << "Vai reto" << endl;
							r->useMotor(60, 60, 10);
						}
					} else {
						if (mediaEsquerda < mediaCentro) {
							//analisa o quanto pode virar e vira pra esquerda
							cout << "Vira pra esquerda" << endl;
							r->useMotor(-10, 80, 10);
						} else {
							//analisa o quanto pode andar e continua reto
							cout << "Vai reto" << endl;
							r->useMotor(60, 60, 10);
						}
					}

				} else {
					cout << "tem obstaculo e precisa girar " << endl;

					if (proxObstaculoDireita < proxObstaculoEsquerda) {
						//gira pra direita
						cout << "Gira pra direita" << endl;
						r->useMotor(70, -70, 10);
					} else {
						//gira pra esquerda
						cout << "Gira pra esquerda" << endl;
						r->useMotor(-70, 70, 10);
					}
				}
		}


		imshow("imgDesvioPadrao", imgDesvioPadrao);

		//cout << "MediaDireita  = " << mediaDireita << endl;
		//cout << "MediaEsquerda = " << mediaEsquerda << endl;
		//cout << "MediaCentro   = " << mediaCentro << endl;


		// funçao que aguarda evento de teclado
		unsigned char tecla = (unsigned char) cv::waitKey(500);

		if (tecla == 27) break;

	}

	//r->stop();

	delete r;

	return 0;
}
