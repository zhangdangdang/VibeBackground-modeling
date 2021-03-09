#pragma once    
#include  <iostream>    
#include "opencv2/opencv.hpp"   
//#include "stdafx.h"

#define NUM_SAMPLES 15  //20      //ÿ�����ص����������    
#define MIN_MATCHES 2       //#minָ��    
#define RADIUS 20         //Sqthere�뾶20    
#define SUBSAMPLE_FACTOR 16//16 5 2 //�Ӳ������ʣ������������µĸ���  
using namespace cv;
using namespace std;


class ViBe_BGS
{
public:
	ViBe_BGS(void);  //���캯��  
	~ViBe_BGS(void);  //�����������Կ��ٵ��ڴ�����Ҫ��������  

	void init(const Mat _image);   //��ʼ��    
	void processFirstFrame(const Mat _image); //���õ�һ֡���н�ģ   
	void testAndUpdate(const Mat _image);  //�ж�ǰ���뱳���������б�������   
	Mat getMask(void) { return m_mask; };  //�õ�ǰ��  
	Mat viberun(Mat img,int i);
private:
	Mat m_samples[NUM_SAMPLES];  //ÿһ֡ͼ���ÿһ�����ص�������  
	Mat m_foregroundMatchCount;  //ͳ�����ر��ж�Ϊǰ���Ĵ��������ڸ���  
	Mat m_mask;  //ǰ����ȡ���һ֡ͼ��  
};
