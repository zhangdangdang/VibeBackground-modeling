#pragma once    
#include  <iostream>    
#include "opencv2/opencv.hpp"   
//#include "stdafx.h"

#define NUM_SAMPLES 15  //20      //每个像素点的样本个数    
#define MIN_MATCHES 2       //#min指数    
#define RADIUS 20         //Sqthere半径20    
#define SUBSAMPLE_FACTOR 16//16 5 2 //子采样概率，决定背景更新的概率  
using namespace cv;
using namespace std;


class ViBe_BGS
{
public:
	ViBe_BGS(void);  //构造函数  
	~ViBe_BGS(void);  //析构函数，对开辟的内存做必要的清理工作  

	void init(const Mat _image);   //初始化    
	void processFirstFrame(const Mat _image); //利用第一帧进行建模   
	void testAndUpdate(const Mat _image);  //判断前景与背景，并进行背景跟新   
	Mat getMask(void) { return m_mask; };  //得到前景  
	Mat viberun(Mat img,int i);
private:
	Mat m_samples[NUM_SAMPLES];  //每一帧图像的每一个像素的样本集  
	Mat m_foregroundMatchCount;  //统计像素被判断为前景的次数，便于更新  
	Mat m_mask;  //前景提取后的一帧图像  
};
