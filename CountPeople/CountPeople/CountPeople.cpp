#include "stdafx.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include<iostream>
using namespace cv;
using namespace std;
void DetectAndDraw(Mat origin, Mat change);
void RefreshBackGround(Mat bg, Mat diff, Mat change);
int main()
{
	Mat backGround;
	backGround = imread("backGround.jpg");
	cvtColor(backGround, backGround, CV_BGR2GRAY);
	
	
	VideoCapture cap("vtest.avi");
	waitKey(1000);
	if (!cap.isOpened())
	{
		return -1;
	}
	Mat frame;

	bool stop = false;
	while (!stop)
	{
		cap >> frame;
		if (frame.empty())
			break;
		DetectAndDraw(backGround, frame);
		
		if (waitKey(30) == 'c')
			stop = true;

	}
	waitKey(0);
	return 0;
}

void DetectAndDraw(Mat  origin,Mat change)
{
	Mat  origin_gray, change_gray, diff, diff2;
	Mat element1 = getStructuringElement(MORPH_CROSS, Size(25, 25));
	Mat element2 = getStructuringElement(MORPH_CROSS, Size(15, 15));
	//cvtColor(origin, origin_gray, CV_BGR2GRAY);
	cvtColor(change, change_gray, CV_BGR2GRAY);
	
	//转换为灰度图
	//计算差分图像
	absdiff(origin, change_gray, diff);
	threshold(diff, diff, 80, 255, THRESH_BINARY);
	GaussianBlur(diff, diff, Size(25, 25), 1.5);

	
	//闭运算
	dilate(diff, diff2, element1);
	erode(diff2, diff2, element2);
	//imshow("diff", diff);
	//绘制轮廓
	vector<vector<Point>> contours;
	findContours(diff, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	drawContours(diff2, contours, -1, Scalar(155));
	imshow("diff2", diff2);
	//框出矩形
	Mat frame = diff;
	for (int i = 0; i < contours.size(); i++) {
		Rect bndRect = boundingRect(contours.at(i));
		Point p1, p2;
		p1.x = bndRect.x;
		p1.y = bndRect.y;
		p2.x = bndRect.x + bndRect.width;
		p2.y = bndRect.y + bndRect.height;
		int ratio = bndRect.height / bndRect.width;
		if (bndRect.area()>2200&&ratio>=1&&ratio<=19) {
			rectangle(change, p1, p2, Scalar(255));
		}
	}
	imshow("rect", change);
	RefreshBackGround(origin, diff,change_gray);
	imshow("BG", origin);

}


void RefreshBackGround(Mat bg, Mat diff,Mat change)
{
	Mat_<uchar>::iterator it1=bg.begin<uchar>();
	Mat_<uchar>::iterator it2 = diff.begin<uchar>();
	Mat_<uchar>::iterator it3 = change.begin<uchar>();
	for (; it1 != bg.end<uchar>(); ++it1,++it2,++it3)
	{
		if (*it2 < 20)
			*it1 = 0.7* *it1 + 0.3* *it3;
	}
	//cout << bg.channels() << endl;
}


