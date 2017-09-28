#include "stdafx.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include<iostream>
using namespace cv;
using namespace std;

#define WINDOW_NAME1 "【原始图片】"        //为窗口标题定义的宏 
#define WINDOW_NAME2 "【匹配窗口】"        //为窗口标题定义的宏 

Mat SrcImg;              //输入图像
Mat DstImg;              //ROI
bool selectFlag = false;  //
						 /*------------------------------------------------------/
						 /功能：鼠标处理事件函数
						 /event:鼠标事件  x,y: 鼠标当前坐标 flag:
						 /param：函数指针
						 /------------------------------------------------------*/
void OnMouseMove(int event, int x, int y, int flag, void* param = NULL);

int g_nMatchMethod= CV_TM_SQDIFF_NORMED;
Mat g_resultImage;

int expectNum=2;
void on_Matching(Mat g_srcImage, Mat g_templateImage);
int main()
{
	SrcImg = imread("1.jpg");

	namedWindow("选择目标", 1);
	setMouseCallback("选择目标", OnMouseMove, NULL);
	imshow("选择目标", SrcImg);
	
	while (1)
	{
		waitKey(100);
		if (selectFlag == true)              //框选目标完成
		{
			destroyWindow("选择目标");
			on_Matching(SrcImg, DstImg);
			break;
		}
	}
	waitKey(0);
    return 0;
}


void OnMouseMove(int event, int x, int y, int flag, void* param /* = NULL */)
{
	static bool drawing = false;    //鼠标点击flag
	static Rect rect(-1, -1, -1, -1);  //ROI
	static Point Origin(-1, -1);     //起点
	//左键按下，且不移动
	if (event == CV_EVENT_LBUTTONDOWN && !CV_EVENT_MOUSEMOVE)
	{
		drawing = true;
		Origin = Point(x, y);    //获取坐标
		rect = Rect(x, y, 0, 0);   //设置lefttop 坐标
	}
	else if ((event == CV_EVENT_MOUSEMOVE) && ((flag && CV_EVENT_FLAG_LBUTTON)))
	{
		//鼠标移动，且左键已按下
		//cout << "框选中！" << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		//如果左键弹起，说明ROI区域选择结束
		//若ROI长宽为0则返回提示
		if (drawing)
		{
			rect.x = Origin.x;
			rect.y = Origin.y;
			rect.width = x- Origin.x;
			rect.height = y- Origin.y;
			drawing = false;
		}
		if (rect.height == 0 || rect.width == 0)
		{
			cout << "You have not choose any rectangle area !" << endl;
		}
		namedWindow("目标", 1);
		SrcImg(rect).copyTo(DstImg);
		imshow("目标", DstImg);
		//imwrite("DstImg.jpg", DstImg);
		//rectangle(SrcImg, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), cvScalar(0, 255, 0));
		//imshow("选择目标", SrcImg);
		selectFlag = true;
		
	}
}

int find_dst(Mat result, Rect dst[]);
void on_Matching(Mat g_srcImage,Mat g_templateImage)
{
	Mat showResult;
	g_srcImage.copyTo(showResult);
	

	//【2】初始化用于结果输出的矩阵
	int resultImage_rows = g_srcImage.rows - g_templateImage.rows + 1;
	int resultImage_cols = g_srcImage.cols - g_templateImage.cols + 1;
	g_resultImage.create(resultImage_rows, resultImage_cols, CV_32FC1);

	//【3】进行匹配和标准化
	matchTemplate(g_srcImage, g_templateImage, g_resultImage, g_nMatchMethod);
	normalize(g_resultImage, g_resultImage, 0, 1, NORM_MINMAX, -1, Mat());

	//【4】查找所有符合模板的位置
	Rect dst[10];
	find_dst(g_resultImage, dst);

	//【6】绘制出矩形，并显示最终结果
	for (int i = 0; i < expectNum; i++)
	{
		rectangle(showResult,dst[i], Scalar(0, 0, 255), 2, 8, 0);
		rectangle(g_resultImage, dst[i], Scalar(0, 0, 255), 2, 8, 0);
	}
	//double minValue; double maxValue; Point minLocation; Point maxLocation;
	//Point matchLocation;
	//minMaxLoc(g_resultImage, &minValue, &maxValue, &minLocation, &maxLocation, Mat());

	////【5】对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值有着更高的匹配结果. 而其余的方法, 数值越大匹配效果越好
	////此句代码的OpenCV2版为：
	////if( g_nMatchMethod  == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED )
	////此句代码的OpenCV3版为：
	//if (g_nMatchMethod == TM_SQDIFF || g_nMatchMethod == TM_SQDIFF_NORMED)
	//{
	//	matchLocation = minLocation;
	//}
	//else
	//{
	//	matchLocation = maxLocation;
	//}

	////【6】绘制出矩形，并显示最终结果
	//rectangle(showResult, matchLocation, Point(matchLocation.x + g_templateImage.cols, matchLocation.y + g_templateImage.rows), Scalar(0, 0, 255), 2, 8, 0);
	//rectangle(g_resultImage, matchLocation, Point(matchLocation.x + g_templateImage.cols, matchLocation.y + g_templateImage.rows), Scalar(0, 0, 255), 2, 8, 0);
	imshow(WINDOW_NAME1, showResult);
	imshow(WINDOW_NAME2, g_resultImage);
	//cout << g_resultImage;
	
}

double calc_bright(Mat mat, int row, int col);
int find_dst(Mat result,Rect dst[])
{
	int num = 0;
	Rect rect1;
	for (int i = 5; i < result.rows-5; i++)
	{
		float * data = result.ptr<float>(i);
		for (int j = 5; j < result.cols * 1-5; j++)
		{
			
			if (data[j] <= 0.2)
			{
				cout << g_resultImage.at<float>(i, j) << " ";
				cout << data[j]<<endl;
				if (calc_bright(result,i,j)<=0.3 && !rect1.contains(Point(j, i)))
				{
					dst[num].x = j;
					dst[num].y = i;
					dst[num].width = 0 + DstImg.cols;
					dst[num].height = 0 + DstImg.rows;
					if (num == 0)
					{
						rect1.x = j - DstImg.cols / 4;
						rect1.y = i - DstImg.rows / 4;
						rect1.width = 0 + DstImg.cols/2;
						rect1.height = 0 + DstImg.rows/2;
					}
					num++;
					if(num==expectNum)
					  return 0;
				}
			}
		}
	}
}

double calc_bright(Mat mat, int row, int col)
{
	double sum, valid=5;
	sum = mat.at<float>(row, col) + mat.at<float>(row, col - 1) + mat.at<float>(row, col + 1) + mat.at<float>(row - 1, col) + mat.at<float>(row + 1, col);
	return sum/5;
}