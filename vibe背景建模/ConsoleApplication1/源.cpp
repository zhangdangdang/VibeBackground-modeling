#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"  
#include<opencv2/imgproc/imgproc.hpp>  
#include <iostream>  

using namespace cv;

int lmain() {

	Mat img = imread("0.jpg");//��˵��Ů����˫���˲���������Ч��Ŷ������Ȥ��ͬѧ��������

	Mat out1, out2, out3;

	medianBlur(img, out1, 35);
	bilateralFilter(img, out2, 25, 25 * 2, 25 / 2);

	namedWindow("��ֵ�˲�", 2);

	imshow("��ֵ�˲�", out1);

	namedWindow("˫���˲�", 2);

	imshow("˫���˲�", out2);


		// Select ROI
		Rect2d r = selectROI(img);

		// Crop image
		Mat imCrop = img(r);

		// Display Cropped Image
		imshow("Image", imCrop);
		



	waitKey(0);

	return 0;
}