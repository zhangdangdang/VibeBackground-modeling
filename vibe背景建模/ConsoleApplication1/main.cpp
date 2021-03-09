#include<opencv2/opencv.hpp>
#include"Vibe.h"
#include"detect.h"
using namespace cv;

int main()
{
	VideoCapture capture;
	capture.open("jiaben1.mp4");
	//capture.open(0);
	if (!capture.isOpened())
	{
		printf("can not open video file  \n");
		return -1;
	}
	Mat frame;
	Mat maskvibe;
	static int counts = 0;
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	//namedWindow("MOG2", CV_WINDOW_AUTOSIZE);
	//namedWindow("KNN", CV_WINDOW_AUTOSIZE);
	Mat maskMOG2, maskKNN;
	Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2(500, 25,false);//true
	Ptr<BackgroundSubtractor> pKNN = createBackgroundSubtractorKNN();
	ViBe_BGS vibeimg;
	detect detecrec;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	while (capture.read(frame))
	{
		cout << "a: " << frame.rows<<frame.cols << endl;
		resize(frame, frame, Size(320, 240));
		cout << "a: " << frame.rows << frame.cols << endl;
		//int a = frame.channels();
		//cout << "a: " << a << endl;
		/*cvtColor(frame, frame, CV_RGB2BGR);
		bilateralFilter(frame, frame, 25, 25 * 2, 25 / 2);
		imshow("双边滤波", frame);*/
		double t = getTickCount();
		imshow("input", frame);
		counts++;
		//cout << "0: " << counts << endl;
		//pMOG2->apply(frame, maskMOG2);
		//pKNN->apply(frame, maskKNN);
		maskvibe = vibeimg.viberun(frame,counts);
		detecrec.detectmask(maskvibe, frame);

		
		//imshow("Vibe", maskvibe);
		//cout << "fps: " << counts << endl;
		//对处理后的帧进行开操作，减少视频中较小的波动造成的影响.
		//morphologyEx(maskMOG2, maskMOG2, MORPH_OPEN, kernel, Point(-1, -1));
		//morphologyEx(maskKNN, maskKNN, MORPH_OPEN, kernel, Point(-1, -1));
		t = (getTickCount() - t) / getTickFrequency();
		double fps = 1 / t;
		cout << "总fps: " << fps/*t*/ << endl;
		//imshow("MOG2", maskMOG2);
		//imshow("KNN", maskKNN);
		waitKey(3);

	}

	capture.release();
	return 0;

}