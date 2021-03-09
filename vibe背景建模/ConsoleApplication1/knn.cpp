/*
						KNN实现
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

const int HISTORY_NUM = 7;// 14;// 历史信息帧数
const int nKNN = 3;// KNN聚类后判断为背景的阈值
const float defaultDist2Threshold = 20.0f;// 灰度聚类阈值

struct PixelHistory
{
	unsigned char *gray;// 历史灰度值
	unsigned char *IsBG;// 对应灰度值的前景/背景判断，1代表判断为背景，0代表判断为前景
};


int _pmain()
{
	PixelHistory* framePixelHistory = NULL;// 记录一帧图像中每个像素点的历史信息
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
		// 读取当前帧
		if (!capture.read(frame))
			exit(EXIT_FAILURE);

		cvtColor(frame, frame, CV_BGR2GRAY);
		if (!InitFlag)
		{
			// 初始化一些变量
			rows = frame.rows;
			cols = frame.cols;
			FGMask.create(rows, cols, CV_8UC1);// 输出图像初始化

			// framePixelHistory分配空间
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
					// 比较确定前景/背景
					for (int n = 0; n < HISTORY_NUM; n++)
					{
						if (fabs(gray - framePixelHistory[i*cols + j].gray[n]) < defaultDist2Threshold)// 灰度差别是否位于设定阈值内
						{
							fit++;
							if (framePixelHistory[i*cols + j].IsBG[n])// 历史信息对应点之前被判断为背景
							{
								fit_bg++;
							}
						}
					}
					if (fit_bg >= nKNN)// 当前点判断为背景
					{
						FGMask.at<unsigned char>(i, j) = 0;
					}
					// 更新历史值
					int index = frameCnt % HISTORY_NUM;
					framePixelHistory[i*cols + j].gray[index] = gray;
					framePixelHistory[i*cols + j].IsBG[index] = fit >= nKNN ? 1 : 0;// 当前点作为背景点存入历史信息

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