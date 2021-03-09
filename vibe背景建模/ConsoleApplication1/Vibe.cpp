#include <opencv2/opencv.hpp>    
#include <iostream>    
#include "ViBe.h"    


using namespace std;
using namespace cv;

int c_xoff[9] = { -1,  0,  1, -1, 1, -1, 0, 1, 0 };  //x的邻居点，9宫格  
int c_yoff[9] = { -1,  0,  1, -1, 1, -1, 0, 1, 0 };  //y的邻居点    

ViBe_BGS::ViBe_BGS(void)
{
	cout << "gouzao" << endl;
}
ViBe_BGS::~ViBe_BGS(void)
{
	cout << "xigou" << endl;
}

/**************** Assign space and init ***************************/
void ViBe_BGS::init(const Mat _image)  //成员函数初始化  
{
	for (int i = 0; i < NUM_SAMPLES; i++) //可以这样理解，针对一帧图像，建立了20帧的样本集  
	{
		m_samples[i] = Mat::zeros(_image.size(), CV_8UC1);  //针对每一帧样本集的每一个像素初始化为8位无符号0，单通道  
	}
	m_mask = Mat::zeros(_image.size(), CV_8UC1); //初始化   
	m_foregroundMatchCount = Mat::zeros(_image.size(), CV_8UC1);  //每一个像素被判断为前景的次数，初始化  
}

/**************** Init model from first frame ********************/
void ViBe_BGS::processFirstFrame(const Mat _image)
{
	RNG rng;    //随机数产生器                                      
	int row, col;

	for (int i = 0; i < _image.rows; i++)                     //指针实现
	{
		for (int j = 0; j < _image.cols; j++)
		{
			for (int k = 0; k < NUM_SAMPLES; k++)
			{
				uchar* pSampRow = m_samples[k].ptr<uchar>(i);
				int random = rng.uniform(0, 9);  //随机产生0-9的随机数，主要用于定位中心像素的邻域像素  

				row = i + c_yoff[random]; //定位中心像素的邻域像素   
				if (row < 0)   //下面四句主要用于判断是否超出边界  
					row = 0;
				if (row >= _image.rows)
					row = _image.rows - 1;
				const uchar* pImgRow = _image.ptr<uchar>(row);

				col = j + c_xoff[random];
				if (col < 0)    //下面四句主要用于判断是否超出边界  
					col = 0;
				if (col >= _image.cols)
					col = _image.cols - 1;
				pSampRow[j] = pImgRow[col];
			}
		}
	}
}


/**************** Test a new frame and update model ********************/
void ViBe_BGS::testAndUpdate(const Mat _image)
{
	RNG rng;

	for (int i = 0; i < _image.rows; i++)
	{
		uchar* pForegroundMatchRow = m_foregroundMatchCount.ptr<uchar>(i);
		uchar* pMaskRow = m_mask.ptr<uchar>(i);

		const uchar* pImgRow = _image.ptr<uchar>(i);

		for (int j = 0; j < _image.cols; j++)
		{
			int matches(0), count(0);
			float dist;

			//uchar* pSampContRow = m_samples[count].ptr<uchar>(i);

			while (matches < MIN_MATCHES && count < NUM_SAMPLES) //自己注释的前边条件，逐个像素判断，当匹配个数大于阀值MIN_MATCHES，或整个样本集遍历完成跳出  
			{
				uchar* pSampContRow = m_samples[count].ptr<uchar>(i);
				dist = abs(/*m_samples[count].at<uchar>(i, j)*/pSampContRow[j] - /*_image.at<uchar>(i, j)*/pImgRow[j]); //当前帧像素值与样本集中的值做差，取绝对值   
				if (dist < RADIUS)  //当绝对值小于阀值是，表示当前帧像素与样本值中的相似  
					matches++;

				count++;  //取样本值的下一个元素作比较  
			}

			if (matches >= MIN_MATCHES)  //匹配个数大于阀值MIN_MATCHES个数时，表示作为背景  
			{
				// It is a background pixel    
				pForegroundMatchRow[j] = 0; //被检测为前景的个数赋值为0  

				pMaskRow[j] = 0;      //该像素点值也为0  

											 // 如果一个像素是背景点，那么它有 1 / defaultSubsamplingFactor 的概率去更新自己的模型样本值    
				int random = rng.uniform(0, SUBSAMPLE_FACTOR);   //以1 / defaultSubsamplingFactor概率更新背景  

				// 同时也有 1 / SUBSAMPLE_FACTOR 的概率去更新它的的模型样本值    
				uchar* pSampRandRowi = m_samples[random].ptr<uchar>(i);
				if (random == 0)
				{
					//random = rng.uniform(0, NUM_SAMPLES);
					//pSampRandRowi[j] = pImgRow[j];
					pSampRandRowi[j] = pImgRow[j];
					//m_samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
				}

				// 同时也有 1 / defaultSubsamplingFactor 的概率去更新它的邻居点的模型样本值    
				random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					int row, col;
					random = rng.uniform(0, 9);
					row = i + c_yoff[random];
					if (row < 0)   //下面四句主要用于判断是否超出边界  
						row = 0;
					if (row >= _image.rows)
						row = _image.rows - 1;

					random = rng.uniform(0, 9);
					col = j + c_xoff[random];
					if (col < 0)   //下面四句主要用于判断是否超出边界  
						col = 0;
					if (col >= _image.cols)
						col = _image.cols - 1;
					uchar* pSampRandRowrow = m_samples[random].ptr<uchar>(row);
					random = rng.uniform(0, NUM_SAMPLES);
					//m_samples[random].at<uchar>(row, col) = _image.at<uchar>(i, j);  //模型样本值更新  
					pSampRandRowrow[col] = pImgRow[j];
				}
			}

			else  //匹配个数小于阀值MIN_MATCHES个数时，表示作为前景  
			{
				// It is a foreground pixel    

				pForegroundMatchRow[j]++;                  //检测为前景的个数加1  
														   // Set background pixel to 255    

				pMaskRow[j] = 255;       //前景点用白色（255）表示
												 //如果某个像素点连续N次（这里为50次）被检测为前景，则认为一块静止区域被误判为运动，将其更新为背景点    
				if (pForegroundMatchRow[j] > 25)//50
				{
					int random = rng.uniform(0, SUBSAMPLE_FACTOR);
					if (random == 0)
					{
						random = rng.uniform(0, NUM_SAMPLES);
						uchar* pSampRandRowi = m_samples[random].ptr<uchar>(i);
						pSampRandRowi[j] = pImgRow[j];

					}
				}
			}
		}
	}
}

Mat ViBe_BGS::viberun(Mat img,int countss)
{
	
	Mat image = img;
	if (image.empty())
		printf("can not open image file  \n");

	Mat gray, mask;
	
	
	cvtColor(image, gray, CV_RGB2GRAY); //转化a为灰度图像
	/*bilateralFilter(gray, gray, 25, 25 * 2, 25 / 2);
		imshow("双边滤波", gray);*/
	mask = Mat::zeros(gray.size(),gray.type());
	if (countss == 1)
	{
		init(gray);
		processFirstFrame(gray);
		cout << "背景模型初始化完成" << endl;
		
	}
	else
	{
		
		double t = getTickCount();
		testAndUpdate(gray);
		mask = getMask();
		morphologyEx(mask, mask, MORPH_OPEN, Mat());   //形态学处理消除前景图像中的小噪声，这里用的开运算 
		imshow("Vibemask", mask);
		t = (getTickCount() - t) / getTickFrequency();
		double fps = 1 / t;
		cout << "fps: " << fps/*t*/ << endl;
		
	}
	return mask;
}