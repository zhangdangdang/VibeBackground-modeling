/*
						KNNʵ��
*/
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <sstream>
using namespace cv;
using namespace std;

const int HISTORY_NUM = 7;// 14;// ��ʷ��Ϣ֡��
const int nKNN = 3;// KNN������ж�Ϊ��������ֵ
const float defaultDist2Threshold = 20.0f;// �ҶȾ�����ֵ

struct PixelHistory
{
	unsigned char *gray;// ��ʷ�Ҷ�ֵ
	unsigned char *IsBG;// ��Ӧ�Ҷ�ֵ��ǰ��/�����жϣ�1�����ж�Ϊ������0�����ж�Ϊǰ��
};


int _pmain()
{
	PixelHistory* framePixelHistory = NULL;// ��¼һ֡ͼ����ÿ�����ص����ʷ��Ϣ
	cv::Mat frame, FGMask, FGMask_KNN;
	int keyboard = 0;
	int rows, cols;
	rows = cols = 0;
	bool InitFlag = false;
	int frameCnt = 0;
	int gray = 0;
	VideoCapture capture(0);
	Ptr<BackgroundSubtractorKNN> pBackgroundKnn =
		createBackgroundSubtractorKNN();
	pBackgroundKnn->setHistory(200);//200
	pBackgroundKnn->setDist2Threshold(600);//600
	pBackgroundKnn->setShadowThreshold(0.5);//0.5

	while ((char)keyboard != 'q' && (char)keyboard != 27)
	{
		// ��ȡ��ǰ֡
		if (!capture.read(frame))
			exit(EXIT_FAILURE);

		cvtColor(frame, frame, CV_BGR2GRAY);
		if (!InitFlag)
		{
			// ��ʼ��һЩ����
			rows = frame.rows;
			cols = frame.cols;
			FGMask.create(rows, cols, CV_8UC1);// ���ͼ���ʼ��

			// framePixelHistory����ռ�
			framePixelHistory = (PixelHistory*)malloc(rows*cols * sizeof(PixelHistory));
			for (int i = 0; i < rows*cols; i++)
			{
				framePixelHistory[i].gray = (unsigned char*)malloc(HISTORY_NUM * sizeof(unsigned char));
				framePixelHistory[i].IsBG = (unsigned char*)malloc(HISTORY_NUM * sizeof(unsigned char));
				memset(framePixelHistory[i].gray, 0, HISTORY_NUM * sizeof(unsigned char));
				memset(framePixelHistory[i].IsBG, 0, HISTORY_NUM * sizeof(unsigned char));
			}

			InitFlag = true;
		}
		if (InitFlag)
		{
			FGMask.setTo(Scalar(255));
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < cols; j++)
				{
					gray = frame.at<unsigned char>(i, j);
					int fit = 0;
					int fit_bg = 0;
					// �Ƚ�ȷ��ǰ��/����
					for (int n = 0; n < HISTORY_NUM; n++)
					{
						if (fabs(gray - framePixelHistory[i*cols + j].gray[n]) < defaultDist2Threshold)// �ҶȲ���Ƿ�λ���趨��ֵ��
						{
							fit++;
							if (framePixelHistory[i*cols + j].IsBG[n])// ��ʷ��Ϣ��Ӧ��֮ǰ���ж�Ϊ����
							{
								fit_bg++;
							}
						}
					}
					if (fit_bg >= nKNN)// ��ǰ���ж�Ϊ����
					{
						FGMask.at<unsigned char>(i, j) = 0;
					}
					// ������ʷֵ
					int index = frameCnt % HISTORY_NUM;
					framePixelHistory[i*cols + j].gray[index] = gray;
					framePixelHistory[i*cols + j].IsBG[index] = fit >= nKNN ? 1 : 0;// ��ǰ����Ϊ�����������ʷ��Ϣ

				}
			}
		}

		//pBackgroundKnn->apply(frame, FGMask_KNN);
		imshow("Frame", frame);
		imshow("FGMask", FGMask);
		//imshow("FGMask_KNN", FGMask_KNN);

		keyboard = waitKey(3);//30
		frameCnt++;
	}
	capture.release();

	return 0;

}