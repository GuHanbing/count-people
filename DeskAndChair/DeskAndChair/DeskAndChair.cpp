#include "stdafx.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include<iostream>
using namespace cv;
using namespace std;

#define WINDOW_NAME1 "��ԭʼͼƬ��"        //Ϊ���ڱ��ⶨ��ĺ� 
#define WINDOW_NAME2 "��ƥ�䴰�ڡ�"        //Ϊ���ڱ��ⶨ��ĺ� 

Mat SrcImg;              //����ͼ��
Mat DstImg;              //ROI
bool selectFlag = false;  //
						 /*------------------------------------------------------/
						 /���ܣ���괦���¼�����
						 /event:����¼�  x,y: ��굱ǰ���� flag:
						 /param������ָ��
						 /------------------------------------------------------*/
void OnMouseMove(int event, int x, int y, int flag, void* param = NULL);

int g_nMatchMethod= CV_TM_SQDIFF_NORMED;
Mat g_resultImage;

int expectNum=2;
void on_Matching(Mat g_srcImage, Mat g_templateImage);
int main()
{
	SrcImg = imread("1.jpg");

	namedWindow("ѡ��Ŀ��", 1);
	setMouseCallback("ѡ��Ŀ��", OnMouseMove, NULL);
	imshow("ѡ��Ŀ��", SrcImg);
	
	while (1)
	{
		waitKey(100);
		if (selectFlag == true)              //��ѡĿ�����
		{
			destroyWindow("ѡ��Ŀ��");
			on_Matching(SrcImg, DstImg);
			break;
		}
	}
	waitKey(0);
    return 0;
}


void OnMouseMove(int event, int x, int y, int flag, void* param /* = NULL */)
{
	static bool drawing = false;    //�����flag
	static Rect rect(-1, -1, -1, -1);  //ROI
	static Point Origin(-1, -1);     //���
	//������£��Ҳ��ƶ�
	if (event == CV_EVENT_LBUTTONDOWN && !CV_EVENT_MOUSEMOVE)
	{
		drawing = true;
		Origin = Point(x, y);    //��ȡ����
		rect = Rect(x, y, 0, 0);   //����lefttop ����
	}
	else if ((event == CV_EVENT_MOUSEMOVE) && ((flag && CV_EVENT_FLAG_LBUTTON)))
	{
		//����ƶ���������Ѱ���
		//cout << "��ѡ�У�" << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		//����������˵��ROI����ѡ�����
		//��ROI����Ϊ0�򷵻���ʾ
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
		namedWindow("Ŀ��", 1);
		SrcImg(rect).copyTo(DstImg);
		imshow("Ŀ��", DstImg);
		//imwrite("DstImg.jpg", DstImg);
		//rectangle(SrcImg, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), cvScalar(0, 255, 0));
		//imshow("ѡ��Ŀ��", SrcImg);
		selectFlag = true;
		
	}
}

int find_dst(Mat result, Rect dst[]);
void on_Matching(Mat g_srcImage,Mat g_templateImage)
{
	Mat showResult;
	g_srcImage.copyTo(showResult);
	

	//��2����ʼ�����ڽ������ľ���
	int resultImage_rows = g_srcImage.rows - g_templateImage.rows + 1;
	int resultImage_cols = g_srcImage.cols - g_templateImage.cols + 1;
	g_resultImage.create(resultImage_rows, resultImage_cols, CV_32FC1);

	//��3������ƥ��ͱ�׼��
	matchTemplate(g_srcImage, g_templateImage, g_resultImage, g_nMatchMethod);
	normalize(g_resultImage, g_resultImage, 0, 1, NORM_MINMAX, -1, Mat());

	//��4���������з���ģ���λ��
	Rect dst[10];
	find_dst(g_resultImage, dst);

	//��6�����Ƴ����Σ�����ʾ���ս��
	for (int i = 0; i < expectNum; i++)
	{
		rectangle(showResult,dst[i], Scalar(0, 0, 255), 2, 8, 0);
		rectangle(g_resultImage, dst[i], Scalar(0, 0, 255), 2, 8, 0);
	}
	//double minValue; double maxValue; Point minLocation; Point maxLocation;
	//Point matchLocation;
	//minMaxLoc(g_resultImage, &minValue, &maxValue, &minLocation, &maxLocation, Mat());

	////��5�����ڷ��� SQDIFF �� SQDIFF_NORMED, ԽС����ֵ���Ÿ��ߵ�ƥ����. ������ķ���, ��ֵԽ��ƥ��Ч��Խ��
	////�˾�����OpenCV2��Ϊ��
	////if( g_nMatchMethod  == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED )
	////�˾�����OpenCV3��Ϊ��
	//if (g_nMatchMethod == TM_SQDIFF || g_nMatchMethod == TM_SQDIFF_NORMED)
	//{
	//	matchLocation = minLocation;
	//}
	//else
	//{
	//	matchLocation = maxLocation;
	//}

	////��6�����Ƴ����Σ�����ʾ���ս��
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