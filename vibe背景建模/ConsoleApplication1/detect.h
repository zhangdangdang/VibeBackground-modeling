#pragma once
#include<iostream>
#include<fstream>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
class detect
{
public:
	detect();
	~detect();
	void  detectmask(const Mat mask,Mat img);

private:

};

