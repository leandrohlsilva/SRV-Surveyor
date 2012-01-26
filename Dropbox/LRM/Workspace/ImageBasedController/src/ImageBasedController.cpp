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

#ifndef CONSTANTES_H
#define CONSTANTES_H
#include "Const.h"
#endif

#define FATOR_DESVIO 2
#define HIST_SIZE 256

#include "SRV.h"

using namespace cv;

Mat imageTrainer;
int select_object = 0;
int track_object = 0;

cv::Point origin;
cv::Rect selection;
cv::Rect track_window;

void on_mouse( int event, int x, int y, int flags, void* param )
{

	if( imageTrainer.data )
		y = imageTrainer.rows - y;

	if( select_object )
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = selection.x + CV_IABS(x - origin.x);
		selection.height = selection.y + CV_IABS(y - origin.y);

		selection.x = MAX( selection.x, 0 );
		selection.y = MAX( selection.y, 0 );
		selection.width = MIN( selection.width, imageTrainer.cols );
		selection.height = MIN( selection.height, imageTrainer.rows );
		selection.width -= selection.x;
		selection.height -= selection.y;


	}

	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = cvPoint(x,y);
		selection = cvRect(x,y,0,0);
		select_object = 1;
		break;
	case CV_EVENT_LBUTTONUP:
		select_object = 0;
		if( selection.width > 0 && selection.height > 0 )
			track_object = -1;
		break;
	}
}

int main()
{
	SRV *r = new SRV("169.254.0.10",10001);  // instancia o robo

	Mat clip;

	int c;

	cvNamedWindow( "Selection", 1 );
	cvNamedWindow( "Clip", 1 );
	cvSetMouseCallback( "Selection", on_mouse, 0 );
	int ti;
	char *img = (char *) r->getImage(&ti, 0);

	Mat aux(240, 320, CV_8UC3, img);
	Mat frame = imdecode(aux, 1);



	if( !imageTrainer.data )
	{
		/* allocate all the buffers */
		frame.copyTo(imageTrainer);
		frame.copyTo(clip);
	}

	for(;;)
	{
		if( select_object && selection.width > 0 && selection.height > 0 )
		{
			// Exibe uma janela com a regiao selecionada
			//printf("Regiao selecionada = %d,%d,%d,%d\n", selection.y, selection.y + selection.height, selection.x, selection.x + selection.width);
			cv::Mat roi(imageTrainer, selection);
			roi.copyTo(clip);
			imshow( "Clip", roi );
		}

		imshow( "Selection", imageTrainer );

		c = cvWaitKey(33);
		if( (char) c == 27 )
			break;
	}

	cvDestroyWindow("DemoSelection");
	cvDestroyWindow("Selection");

	printf("Calculando a media RGB e o desvio padrao da area %d,%d...\n",clip.cols, clip.rows);

	cv::Scalar dRGB, mRGB;
	cv::meanStdDev(clip, mRGB, dRGB);

	printf("mR = %f, mG = %f, mB = %f\n", mRGB.val[2], mRGB.val[1], mRGB.val[0]);
	printf("dR = %f, dG = %f, dB = %f\n", dRGB.val[2], dRGB.val[1], dRGB.val[0]);

	cvDestroyWindow("Clip");

	while (1)
	{

		char *img;
		int tamanhoImagem;
		img = (char *) calloc (TAMANHO_MAX_IMAGEM,sizeof(char));
		img = (char *) r->getImage(&tamanhoImagem, 0);

		Mat aux(240, 320, CV_8UC3, img);
		Mat imageRGB = imdecode(aux, 1);
		imshow( "imageRGB", imageRGB );

		/*Mat imageHSV;
		cvtColor(imageRGB, imageHSV, CV_RGB2HSV);
		imshow("HSV", imageHSV);

		// separando os canais de cores
		std::vector<cv::Mat> canais;
		split( imageHSV, canais );

		cv::Mat imageH = canais[0];
		cv::Mat imageS = canais[1];
		cv::Mat imageV = canais[2];
		imshow( "imageH", imageH );
		imshow( "imageS", imageS );
		imshow( "imageV", imageV );

		Mat imageThresh;
		threshold(imageRGB, imageThresh, 100, 255, CV_THRESH_BINARY);
		imshow("Thresh", imageThresh);

		Mat imageThreshH;
		threshold(imageH, imageThreshH, 100, 255, CV_THRESH_BINARY);
		imshow("ThreshH", imageThreshH);*/



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

		int x1, x2, y1, y2;
		bool primeiroPonto = true;

		for(int i=0;i<height;i++) for(int j=0;j<width;j++)
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

				data[i*step+j*channels+0]=255;
				data[i*step+j*channels+1]=255;
				data[i*step+j*channels+2]=255;
			}
			else
			{
				data[i*step+j*channels+0]=0;
				data[i*step+j*channels+1]=0;
				data[i*step+j*channels+2]=0;
			}
		}

		if (!primeiroPonto) {
			//printf("Objeto identificado entre (%d,%d) e (%d,%d)\n", x1,y1,x2,y2);
			cv::Point p1(x1, y1);
			cv::Point p2(x2, y2);
			cv::Scalar red;
			cv::rectangle(imgDesvioPadrao, p1, p2, CV_RGB(255,0,0), CV_FILLED, 8, 0);

			//calculando o centro do objeto:
			int cox, coy;
			cox = x1 + (x2 - x1)/2;
			coy = y2 + (y1 - y2)/2;

			//o centro da tela
			int ctx, cty;
			ctx = imgDesvioPadrao.cols/2;
			cty = imgDesvioPadrao.rows/2;
			int margem = sqrt(ctx);
			if (cox < ctx - margem) {
				printf("Virou para esquerda\n");
				r->ativaMotor(-70, 70, 10);
			} else if (cox > ctx + margem) {
				printf("Virou para direita\n");
				r->ativaMotor(70, -70, 10);
			} else {
				printf("esta no centro\n");
			}
		}

		imshow("imgDesvioPadrao", imgDesvioPadrao);




		free(img);

		// funçao que aguarda evento de teclado
		unsigned char tecla = (unsigned char) cv::waitKey(33);

		if (tecla == 27) break;

	}

	delete r;

	return 0;
}
