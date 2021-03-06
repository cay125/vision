// opencvtemplate.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"  //在VS2017中一些头文件都放在"stdafx.h"文件中
#include "Segmentation.h"
#include <iostream>  
#include <algorithm>

using namespace cv;
using namespace std;

bool cmp(const  vector< Point>&a, const  vector< Point>b)
{
	return contourArea(a) < contourArea(b);
}

int LEDrecognize(Mat img, const char *p = NULL)
{
	Mat Closeout;
	morphologyEx(img, Closeout, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(3, 3)));
	vector< vector< Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(Closeout, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	//cout << "字符 "<<i+1<<" contour size is : " << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++)
	{
		//cout << contours[i].size() << endl;
		if (hierarchy[i][3] != -1 || contours[i].size() < 5)
		{
			contours.erase(contours.begin() + i);
			i--;
		}
	}
	Mat Digit;
	if (contours.size() != 1)
	{
		auto pos = max_element(contours.begin(), contours.end(), cmp);
		Digit = Closeout(boundingRect(contours[pos - contours.begin()]));
	}
	else
	{
		Digit = Closeout(boundingRect(contours[0]));
	}
	if (p != NULL)
		imshow(p, Digit);
	
	//int rows[4] = { Digit.rows*0.2,Digit.rows*0.4,Digit.rows*0.6,Digit.rows*0.8 };
	//int cols[4] = { Digit.cols*0.2,Digit.cols*0.4,Digit.cols*0.6,Digit.cols*0.8 };
	//int cnth[4] = { 0 }, cntv[4] = { 0 };
	int rows[2] = { Digit.rows*0.25,Digit.rows*0.75 };
	int cols[2] = { Digit.cols*0.45,Digit.cols*0.55 };
	int cnth[2] = { 0 }, cntv[2] = { 0 };
	vector<vector<int>> cnthIndex(2);
	vector<vector<int>> cntvIndex(2);
	for (int i = 0; i < 2; i++)
	{
		if (Digit.at<uchar>(rows[i], 0) != 0)
		{
			cnth[i]++;
			cnthIndex[i].push_back(0);
		}
		if (Digit.at<uchar>(rows[i], Digit.cols - 1) != 0)
		{
			cnth[i]++;
			cnthIndex[i].push_back(Digit.cols - 1);
		}
		for (int j = 1; j < Digit.cols; j++)
		{
			if (Digit.at<uchar>(rows[i], j) != Digit.at<uchar>(rows[i], j - 1))
			{
				cnth[i]++;
				cnthIndex[i].push_back(j);
			}
		}
	}
	for (int j = 0; j < 2; j++)
	{
		if (Digit.at<uchar>(0, cols[j]) != 0)
		{
			cntv[j]++;
			cntvIndex[j].push_back(0);
		}
		if (Digit.at<uchar>(Digit.rows - 1, cols[j]) != 0)
		{
			cntv[j]++;
			cntvIndex[j].push_back(Digit.rows - 1);
		}
		for (int i = 1; i < Digit.rows; i++)
		{
			if (Digit.at<uchar>(i, cols[j]) != Digit.at<uchar>(i - 1, cols[j]))
			{
				cntv[j]++;
				cntvIndex[j].push_back(i);
			}
		}
	}
	
	if (cnth[0] == 4 && cnth[1] == 2 && (cntv[0] != 6 || cntv[1] != 6))
		return 4;
	if (cnth[0] == 4 && cnth[1] == 4)
		return 8;
	if (cnth[0] == 4 && cnth[1] == 2 && (cntv[0] == 6 || cntv[1] == 6))
		return 9;
	if (cnth[0] == 2 && cnth[1] == 4 && (cntv[0] == 6 || cntv[1] == 6))
		return 6;
	if (cnth[0] == 2 && cnth[1] == 2 && (cntv[0] == 6 || cntv[1] == 6))
	{
		if(abs(cnthIndex[0][0]-cnthIndex[1][0])<Digit.cols*0.5)
			return 3;
		if (cnthIndex[0][0] > cnthIndex[1][0])
			return 2;
		if (cnthIndex[0][0] < cnthIndex[1][0])
			return 5;
	}
	if (cnthIndex[1][0] > Digit.cols*0.4)
		return 7;
	return 1;
}
int main()
{  
	Mat img = imread("C:/handwritings/videoframe000000000.png"), imgbackup(img.size(), img.type());//21740 15127 00000 00025 00607 02215 07124 03819 15729 21847 7124 3637 2254 3758 6393 3728 6365 3923 5581 3658 6400 21897 18783 7095 6331 12821 137不行 6320识别不行
	if (img.empty())
	{
		cout << "Read img failed" << endl;
	}
	else
	{
		img.copyTo(imgbackup);
		vector<RotatedRect> rects;//保存9个手写矩形
		RotatedRect LEDrect;//保存LED矩形
		int pos[4] = { 0 };//中间变量 用于调试
		HandDigital(imgbackup, rects, pos);
		cout << "total rects find: " << rects.size() << endl;
		if (rects.size() != 9)
		{
			cout << "error when tring to find Hand Writings" << endl;
			system("pause");
			return 0;
		}

		//Mat subrects[9];
		//for (int i = 0; i < 9; i++)
		//{
		//	subrects[i] = imgbackup(rects[i].boundingRect());
		//	stringstream num;
		//	num << i;
		//	//imshow("No."+num.str(), subrects[i]);
		//}

		Mat LEDCut;
		PreLED(img, LEDCut, rects, pos);//根据投影和9个手写数字方位，分割出大致位置
		Mat binary = LEDDigital(LEDCut, rects, LEDrect);
		if (LEDrect.size.height == 0 && LEDrect.size.width == 0)
		{
			cout << "error when tring to find LED" << endl;
			system("pause");
			return 0;
		}
		cout << "LED ANGEL: " << LEDrect.angle<< endl;


		Mat rot_img, rotled, rot_binary, rot_bin_led;
		if (LEDrect.size.width < LEDrect.size.height)//说明此时矩阵长比宽小，即矩阵错位了90度。需要再旋转90度转回来
		{
			warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());
			warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());//仿射变换，旋转图片使其水平
			rotled = rot_img(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
			rot_bin_led = rot_binary(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
		}
		else
		{
			warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());
			warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());//仿射变换，旋转图片使其水平
			getRectSubPix(rot_img, LEDrect.size, LEDrect.center, rotled);
			getRectSubPix(rot_binary, LEDrect.size, LEDrect.center, rot_bin_led);
		}
		//imshow("roteimg", rot_img);
		//imshow("rot_binary", rot_bin_led);
		imshow("result", rotled);
		int pos_led[4] = { 0 };
		vector<Mat> words;
		LEDDivide(rotled, pos_led, words);
		//int pos2[4] = { 0 };
		//ProjectLED(rot_bin_led, pos2);

		for (int i = 0; i < 5; i++)
		{
			stringstream s; s << i;
			printf("%d\n", LEDrecognize(words[i], s.str().c_str()));
		}
	}
	waitKey(0);
	system("pause");
	return 0;
}

