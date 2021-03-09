#include<iostream>
#include<fstream>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include"detect.h"
using namespace std;
using namespace cv;
detect::detect()
{
}

detect::~detect()
{
}
//;轮廓绘制与目标检测
void detect::detectmask(const Mat mask, Mat img)
{
	double area = 0;
	int bigcotour = 0,counts;
	Mat result = mask;
	Mat img1 = img;
	cv::Vec4f line_para;//线的参数	
	cv::Point point1, point2;//端点
	cv::Point point0, pointlength1, pointlength2;//
	double b = 0,c=0;
	vector<vector<Point> > contours;
	findContours(result, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//找轮廓函数
																		   //drawContours(img, contours, -1, Scalar(0, 0, 255), 2);//在result上绘制轮廓  
																		   //查找正外接矩形  
	vector<Rect> boundRect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		//rectangle(img1, boundRect[i], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形 
		//rectangle(img1, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		//测量轮廓长度和面积
		double length = arcLength(contours[i], true);
		 area = contourArea(contours[i]);
		 cout << "area--" << i << "---" << area << endl;
		 //原来用周长做判定
		if (area > bigcotour)
		{
			bigcotour = area;
			counts = i;
		}
		//cout << "length--" << i << "---" << length << endl;
		
	}
	


	if (contours.size() != 0&& bigcotour >3)//最大面积大于3
	{
		cout << "counts--" << counts << endl;
		Moments m = moments(contours[counts], true);
		Point p(m.m10 / m.m00, m.m01 / m.m00);
		cout <<"point"<< Mat(p) << endl;
		pointlength1.x = 0;
		pointlength1.y = p.y;
		pointlength2.x = img1.cols;
		pointlength2.y = p.y;
		line(img1, pointlength1, pointlength2, cv::Scalar(0, 255, 0), 2, 8, 0);
		//rectangle(img1, boundRect[c], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形 
		rectangle(img1, boundRect[counts], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形
		//rectangle(result, boundRect[counts], Scalar(0, 255, 0), 2);//在result上绘制正外接矩形
		imshow("矩形", img1);

	}

}