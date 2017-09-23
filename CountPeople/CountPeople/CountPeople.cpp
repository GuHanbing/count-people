#include "stdafx.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
using namespace cv;
using namespace std;
int main()
{
	Mat origin, change, origin_gray, change_gray, diff, diff2;
	Mat element1 = getStructuringElement(MORPH_CROSS, Size(25, 25));
	Mat element2 = getStructuringElement(MORPH_CROSS, Size(5, 5));
	origin = imread("2.jpg");
	change = imread("1.jpg");
	imshow("背景", origin);
	imshow("人", change);
	//转换为灰度图
	cvtColor(origin, origin_gray, CV_BGR2GRAY);
	cvtColor(change, change_gray, CV_BGR2GRAY);

	//计算差分图像
	absdiff(origin_gray, change_gray, diff);
	threshold(diff, diff, 30, 255, THRESH_BINARY);
	GaussianBlur(diff, diff, Size(25, 25), 1.5);
	//闭运算
	dilate(diff, diff2, element1);
	erode(diff, diff2, element2);

	//imshow("diff", diff);


	//绘制轮廓

	vector<vector<Point>> contours;
	findContours(diff2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	drawContours(diff2, contours, -1, Scalar(155));
	imshow("diff2", diff2);

	//框出矩形
	Mat frame = diff2;
	for (int i = 0; i < contours.size(); i++) {
		Rect bndRect = boundingRect(contours.at(i));
		Point p1, p2;
		p1.x = bndRect.x;
		p1.y = bndRect.y;
		p2.x = bndRect.x + bndRect.width;
		p2.y = bndRect.y + bndRect.height;
		if (bndRect.area()>3000) {
			rectangle(change, p1, p2, Scalar(255));

		}
	}
	imshow("rect", change);
	waitKey(0);
	return 0;
}



