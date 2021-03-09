#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"  
#include<opencv2/imgproc/imgproc.hpp>  
#include <iostream>  

using namespace cv;

int lmain() {

	Mat img = imread("0.jpg");//据说丑女经过双边滤波用有美颜效果哦，有兴趣的同学可以试试

	Mat out1, out2, out3;

	medianBlur(img, out1, 35);
	bilateralFilter(img, out2, 25, 25 * 2, 25 / 2);

	namedWindow("中值滤波", 2);

	imshow("中值滤波", out1);

	namedWindow("双边滤波", 2);

	imshow("双边滤波", out2);


		// Select ROI
		Rect2d r = selectROI(img);

		// Crop image
		Mat imCrop = img(r);

		// Display Cropped Image
		imshow("Image", imCrop);
		



	waitKey(0);

	return 0;
}