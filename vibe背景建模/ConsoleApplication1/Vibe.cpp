#include <opencv2/opencv.hpp>    
#include <iostream>    
#include "ViBe.h"    


using namespace std;
using namespace cv;

int c_xoff[9] = { -1,  0,  1, -1, 1, -1, 0, 1, 0 };  //x���ھӵ㣬9����  
int c_yoff[9] = { -1,  0,  1, -1, 1, -1, 0, 1, 0 };  //y���ھӵ�    

ViBe_BGS::ViBe_BGS(void)
{
	cout << "gouzao" << endl;
}
ViBe_BGS::~ViBe_BGS(void)
{
	cout << "xigou" << endl;
}

/**************** Assign space and init ***************************/
void ViBe_BGS::init(const Mat _image)  //��Ա������ʼ��  
{
	for (int i = 0; i < NUM_SAMPLES; i++) //����������⣬���һ֡ͼ�񣬽�����20֡��������  
	{
		m_samples[i] = Mat::zeros(_image.size(), CV_8UC1);  //���ÿһ֡��������ÿһ�����س�ʼ��Ϊ8λ�޷���0����ͨ��  
	}
	m_mask = Mat::zeros(_image.size(), CV_8UC1); //��ʼ��   
	m_foregroundMatchCount = Mat::zeros(_image.size(), CV_8UC1);  //ÿһ�����ر��ж�Ϊǰ���Ĵ�������ʼ��  
}

/**************** Init model from first frame ********************/
void ViBe_BGS::processFirstFrame(const Mat _image)
{
	RNG rng;    //�����������                                      
	int row, col;

	for (int i = 0; i < _image.rows; i++)                     //ָ��ʵ��
	{
		for (int j = 0; j < _image.cols; j++)
		{
			for (int k = 0; k < NUM_SAMPLES; k++)
			{
				uchar* pSampRow = m_samples[k].ptr<uchar>(i);
				int random = rng.uniform(0, 9);  //�������0-9�����������Ҫ���ڶ�λ�������ص���������  

				row = i + c_yoff[random]; //��λ�������ص���������   
				if (row < 0)   //�����ľ���Ҫ�����ж��Ƿ񳬳��߽�  
					row = 0;
				if (row >= _image.rows)
					row = _image.rows - 1;
				const uchar* pImgRow = _image.ptr<uchar>(row);

				col = j + c_xoff[random];
				if (col < 0)    //�����ľ���Ҫ�����ж��Ƿ񳬳��߽�  
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

			while (matches < MIN_MATCHES && count < NUM_SAMPLES) //�Լ�ע�͵�ǰ����������������жϣ���ƥ��������ڷ�ֵMIN_MATCHES�������������������������  
			{
				uchar* pSampContRow = m_samples[count].ptr<uchar>(i);
				dist = abs(/*m_samples[count].at<uchar>(i, j)*/pSampContRow[j] - /*_image.at<uchar>(i, j)*/pImgRow[j]); //��ǰ֡����ֵ���������е�ֵ���ȡ����ֵ   
				if (dist < RADIUS)  //������ֵС�ڷ�ֵ�ǣ���ʾ��ǰ֡����������ֵ�е�����  
					matches++;

				count++;  //ȡ����ֵ����һ��Ԫ�����Ƚ�  
			}

			if (matches >= MIN_MATCHES)  //ƥ��������ڷ�ֵMIN_MATCHES����ʱ����ʾ��Ϊ����  
			{
				// It is a background pixel    
				pForegroundMatchRow[j] = 0; //�����Ϊǰ���ĸ�����ֵΪ0  

				pMaskRow[j] = 0;      //�����ص�ֵҲΪ0  

											 // ���һ�������Ǳ����㣬��ô���� 1 / defaultSubsamplingFactor �ĸ���ȥ�����Լ���ģ������ֵ    
				int random = rng.uniform(0, SUBSAMPLE_FACTOR);   //��1 / defaultSubsamplingFactor���ʸ��±���  

				// ͬʱҲ�� 1 / SUBSAMPLE_FACTOR �ĸ���ȥ�������ĵ�ģ������ֵ    
				uchar* pSampRandRowi = m_samples[random].ptr<uchar>(i);
				if (random == 0)
				{
					//random = rng.uniform(0, NUM_SAMPLES);
					//pSampRandRowi[j] = pImgRow[j];
					pSampRandRowi[j] = pImgRow[j];
					//m_samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
				}

				// ͬʱҲ�� 1 / defaultSubsamplingFactor �ĸ���ȥ���������ھӵ��ģ������ֵ    
				random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					int row, col;
					random = rng.uniform(0, 9);
					row = i + c_yoff[random];
					if (row < 0)   //�����ľ���Ҫ�����ж��Ƿ񳬳��߽�  
						row = 0;
					if (row >= _image.rows)
						row = _image.rows - 1;

					random = rng.uniform(0, 9);
					col = j + c_xoff[random];
					if (col < 0)   //�����ľ���Ҫ�����ж��Ƿ񳬳��߽�  
						col = 0;
					if (col >= _image.cols)
						col = _image.cols - 1;
					uchar* pSampRandRowrow = m_samples[random].ptr<uchar>(row);
					random = rng.uniform(0, NUM_SAMPLES);
					//m_samples[random].at<uchar>(row, col) = _image.at<uchar>(i, j);  //ģ������ֵ����  
					pSampRandRowrow[col] = pImgRow[j];
				}
			}

			else  //ƥ�����С�ڷ�ֵMIN_MATCHES����ʱ����ʾ��Ϊǰ��  
			{
				// It is a foreground pixel    

				pForegroundMatchRow[j]++;                  //���Ϊǰ���ĸ�����1  
														   // Set background pixel to 255    

				pMaskRow[j] = 255;       //ǰ�����ð�ɫ��255����ʾ
												 //���ĳ�����ص�����N�Σ�����Ϊ50�Σ������Ϊǰ��������Ϊһ�龲ֹ��������Ϊ�˶����������Ϊ������    
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
	
	
	cvtColor(image, gray, CV_RGB2GRAY); //ת��aΪ�Ҷ�ͼ��
	/*bilateralFilter(gray, gray, 25, 25 * 2, 25 / 2);
		imshow("˫���˲�", gray);*/
	mask = Mat::zeros(gray.size(),gray.type());
	if (countss == 1)
	{
		init(gray);
		processFirstFrame(gray);
		cout << "����ģ�ͳ�ʼ�����" << endl;
		
	}
	else
	{
		
		double t = getTickCount();
		testAndUpdate(gray);
		mask = getMask();
		morphologyEx(mask, mask, MORPH_OPEN, Mat());   //��̬ѧ��������ǰ��ͼ���е�С�����������õĿ����� 
		imshow("Vibemask", mask);
		t = (getTickCount() - t) / getTickFrequency();
		double fps = 1 / t;
		cout << "fps: " << fps/*t*/ << endl;
		
	}
	return mask;
}