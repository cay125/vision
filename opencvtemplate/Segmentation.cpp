#include "Segmentation.h"
#include "stdafx.h"
#include <iostream>  
#include <numeric>
#include <algorithm>
#include <opencv2/core.hpp>  
#include <opencv2/highgui.hpp>  
#include <opencv2/imgproc.hpp>
#define HAND_DEBUG 0
#define LED_DEBUG 1
#define LEDline 1
#define LEDDivideLine 0
#define LEDShowBinaryDivided 0
#define LEDShowOriDivided 0
#define ShowOutHand 0
using namespace cv;
using namespace std;
//计算图片投影，用于筛选轮廓
void Projection(Mat& img, Mat& out, int v_thresh, int h_thresh, Mat& other, int* pos = NULL)
{
	Mat v(img.size(), img.type(), Scalar(0, 0, 0)), h(img.size(), img.type(), Scalar(0, 0, 0));//v h为投影图。根据滑动窗口的均值来确定大致筛选范围。
	int row = img.rows, col = img.cols;
	vector<int> vector_cnth;
	for (int i = 0; i < row; i++)
	{
		int cnth = 0;
		for (int j = 0; j < col; j++)
		{
			if (img.at<uchar>(i, j) == 255)
				h.at<uchar>(i, cnth++) = 255;
		}
		vector_cnth.push_back(cnth);
	}
	vector<int> vector_cntv;
	for (int j = 0; j < col; j++)
	{
		int cntv = 0;
		for (int i = 0; i < row; i++)
		{
			if (img.at<uchar>(i, j) == 255)
				v.at<uchar>(cntv++, j) = 255;
		}
		vector_cntv.push_back(cntv);
	}
	int left = col / 2, right = col / 2;
	while (((left - 50) >= 0 && accumulate(vector_cntv.begin() + left - 50, vector_cntv.begin() + left, 0) / 50 > v_thresh) || (left - 100 >= 0 && accumulate(vector_cntv.begin() + left - 50, vector_cntv.begin() + left, 0) < accumulate(vector_cntv.begin() + left - 100, vector_cntv.begin() + left - 50, 0)))
		left -= 20;
	left = left - 50 >= 0 ? left - 50 : 0;
	while (((right + 50) < col && accumulate(vector_cntv.begin() + right, vector_cntv.begin() + right + 50, 0) / 50 > v_thresh)|| (right + 100 < col && accumulate(vector_cntv.begin() + right, vector_cntv.begin() + right + 50, 0) < accumulate(vector_cntv.begin() + right + 50, vector_cntv.begin() + right + 100, 0)))
		right += 20;
	right = right + 50 < col ? right + 50 : col - 1;
	int top = row / 2, bottom = row / 2;
	while (((bottom + 50) < row && accumulate(vector_cnth.begin() + bottom, vector_cnth.begin() + bottom + 50, 0) / 50 > h_thresh-20) || (bottom + 100 < row && accumulate(vector_cnth.begin() + bottom, vector_cnth.begin() + bottom + 50, 0) < accumulate(vector_cnth.begin() + bottom + 50, vector_cnth.begin() + bottom + 100, 0)))
		bottom += 20;
	bottom = bottom + 50 < row ? bottom + 50 : row - 1;
	while (((top - 50) >= 0 && accumulate(vector_cnth.begin() + top - 50, vector_cnth.begin() + top, 0) / 50 > h_thresh) || (top - 100 >= 0 && accumulate(vector_cnth.begin() + top - 50, vector_cnth.begin() + top, 0) < accumulate(vector_cnth.begin() + top - 100, vector_cnth.begin() + top - 50, 0)))
		top -= 20;
	top = top - 50 >= 0 ? top - 50 : 0;
	Mat t = img(Range(top, bottom), Range(left, right)).clone();
	t.copyTo(out);
	if (pos != NULL)
	{
		pos[0] = right;
		pos[1] = top;
		pos[2] = left;
		pos[3] = bottom;
	}
	other = other(Range(top, bottom), Range(left, right));
#if HAND_DEBUG
	imshow("projh", h);
	imshow("projv", v);
#endif
}
//计算LED投影，用于分割字符
void ProjectLED(Mat& img, int* pos)
{
	Mat v(img.size(), img.type(), Scalar(0, 0, 0)), h(img.size(), img.type(), Scalar(0, 0, 0));//v h为投影图。根据投影谷值来分割字符。
	int row = img.rows, col = img.cols;
	vector<int> vector_cnth;
	for (int i = 0; i < row; i++)
	{
		int cnth = 0;
		for (int j = 0; j < col; j++)
		{
			if (img.at<uchar>(i, j) == 255)
				h.at<uchar>(i, cnth++) = 255;
		}
		vector_cnth.push_back(cnth);
	}
	vector<int> vector_cntv;
	for (int j = 0; j < col; j++)
	{
		int cntv = 0;
		for (int i = 0; i < row; i++)
		{
			if (img.at<uchar>(i, j) == 255)
				v.at<uchar>(cntv++, j) = 255;
		}
		vector_cntv.push_back(cntv);
	}
#if LEDDivideLine
	imshow("v", v);
	imshow("h", h);
#endif
	int index1 = 0, index2 = 0;
	int t1 = 0, t2 = 0;
	while (index2 < img.cols && vector_cntv[index2] < 5)
		index2++;
	for (int i = 0; i <= 3; i++)
	{
		index1 = index2 + 1;
		while (index1 < img.cols && vector_cntv[index1] > 5)
			index1++;
		index2 = index1 + 1;
		while (index2 < img.cols && vector_cntv[index2] < 5)
			index2++;
		t2 = (index1 + index2) / 2;
		if ((t2 - t1) < (img.cols*0.6) / 5)//如果当前检测到的分界线距离上一个间隔太小，舍弃之。
		{
			i--;
			continue;
		}
		pos[i] = t2;
		t1 = t2;
#if LEDDivideLine
		line(img, Point2f(pos[i], 0), Point2f(pos[i], img.cols - 1), Scalar(255, 0, 0), 2);
#endif
	}
#if LEDDivideLine
	imshow("line", img);
#endif
}
//快速矩化。速度太慢没有使用
void QuickRect(Mat& img)//快速矩化 传入二值图像
{
	bool flag = true;//将二值图像变成矩形，考虑到速度慢以及存在旋转。没有使用。
	int q1[4] = { 1, -1 , 0, 0 }, q2[4] = { 0,0,1,-1 };
	int row = img.rows, col = img.cols;
	while (flag)
	{
		flag = false;
		for (int i = 1; i < row - 1; i++)
		{
			for (int j = 1; j < col - 1; j++)
			{
				int cnt = 0;
				if (img.at<uchar>(i, j) == 0)
				{
					for (int k = 0; k < 4; k++)
						if (img.at<uchar>(i + q1[k], j + q2[k]) == 255)
							cnt++;
					if (cnt >= 2)
					{
						flag = true;
						img.at<uchar>(i, j) = 255;
					}
				}
				else
				{
					for (int k = 0; k < 4; k++)
						if (img.at<uchar>(i + q1[k], j + q2[k]) == 0)
							cnt++;
					if (cnt == 4)
					{
						flag = true;
						img.at<uchar>(i, j) = 0;
					}
				}
			}
		}
	}
}
bool cmp1(const RotatedRect& a, const RotatedRect& b)
{
	return a.center.x < b.center.x;
}
bool cmp2(const RotatedRect& a, const RotatedRect& b)
{
	return a.center.y < b.center.y;
}
bool cmp3(const RotatedRect& a, const RotatedRect& b)
{
	return a.size.width*a.size.height < b.size.width*b.size.height;
}
//LED预分割
void PreLED(Mat& img, Mat& out, vector<RotatedRect>& rects, int* pos = NULL)
{
	if (pos != NULL)
	{
		for (int i = 0; i < rects.size(); i++)
		{
			rects[i].center.x += pos[2];
			rects[i].center.y += pos[1];
		}
	}
	sort(rects.begin(), rects.end(), cmp1);
	float k = rects[0].size.width > rects[0].size.height ? rects[0].size.height : rects[0].size.width;
	int left = int(rects[0].center.x + k / 2), right = rects[8].center.x - k / 2;
	sort(rects.begin(), rects.end(), cmp2);
	k = rects[0].size.width > rects[0].size.height ? rects[0].size.height : rects[0].size.width;
	int top = rects[1].center.y - k / 2;
	Mat t = img(Range::all(), Range(left, right)).clone();
	t.copyTo(out);

	for (int i = 0; i < rects.size(); i++)
		rects[i].center.x -= left;
}
Mat LEDDigital(Mat& img, vector<RotatedRect>& rects, RotatedRect& LEDRect)
{
	Mat Gaussout, Grayout, Sobelout, Binaryout, OpenOpout, CloseOpout, Tailorout, Finalout, imgbackup(img.size(), img.type()), BinaryDirect, BinaryLine;//保存中间变量用于调试。
	GaussianBlur(img, Gaussout, Size(15, 15), 0, 0);
	cvtColor(Gaussout, Grayout, CV_BGR2GRAY);
	Mat t1, t2;
	//Sobel(Grayout, Sobelout, Grayout.depth(), 0, 1);
	Sobel(Grayout, t1, CV_16S, 0, 1);
	convertScaleAbs(t1, t1);
	Sobel(Grayout, t2, CV_16S, 1, 0, 3);
	convertScaleAbs(t2, t2);
	addWeighted(t1, 0.8, t2, 0.8, 0, Sobelout);
	threshold(Sobelout, Binaryout, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	threshold(Grayout, BinaryDirect, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	Binaryout.copyTo(BinaryLine);
	sort(rects.begin(), rects.end(), cmp2);
	sort(rects.begin(), rects.begin() + 3, cmp1);
	double real_width = max(rects[1].size.height, rects[0].size.width);
	line(BinaryLine, Point2f(rects[1].center.x - real_width / 2 * 1.05, 2 * rects[1].center.y - rects[4].center.y), Point2f(rects[1].center.x + real_width / 2 * 1.05, 2 * rects[1].center.y - rects[4].center.y), Scalar(255, 0, 0));
	morphologyEx(BinaryLine, CloseOpout, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(17, 5)));

	vector< vector< Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(CloseOpout, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
#if LED_DEBUG
	cout << "img size is: " << img.rows << " " << img.cols << endl;
	cout << "num of contour is " << contours.size() << endl;
#endif
	
	for (int i = 0; i<contours.size(); i++)
	{//删除所有子轮廓，只保留一级轮廓
		if (hierarchy[i][3] != -1 || contours[i].size() < 200)
		{
			contours.erase(contours.begin() + i);
			hierarchy.erase(hierarchy.begin() + i);
			i--;
		}
	}
	double AveRectSize = 0;
	for (int i = 0; i < 9; i++)//计算矩形面积均值
	{
		AveRectSize += rects[i].size.height*rects[i].size.width;
	}
	AveRectSize /= 9;
	vector<vector<Point> >::iterator itc = contours.begin();
	while (itc != contours.end())
	{
		Point2f P[4];
		RotatedRect rect = minAreaRect(*itc);
		rect.points(P);
		for (int i = 0; i < 4; i++)
			line(imgbackup, P[i], P[(i + 1) % 4], Scalar(255, 0, 0), 2);
		//cout << "rect " << itc - contours.begin() << " size is: " << rect.size << " contour size is: " << itc->size() << endl;
		if ((rect.size.width*rect.size.height) < (AveRectSize * 5 / 3) && (rect.size.width*rect.size.height) > (AveRectSize * 2 / 3) && rect.center.y < (rects[0].center.y - min(rects[0].size.height, rects[0].size.width) / 2) && (rects[0].center.y - min(rects[0].size.height, rects[0].size.width) / 2 - rect.center.y) < (min(rect.size.height, rect.size.width)*1.6))
		{//如果面积和手写数字矩形面积相比，太小或者太大则不考虑，如果矩形中心在手写数字下方也不考虑
			RotatedRect r = minAreaRect(*itc);
			//LEDRect = minAreaRect(*itc);
			double d = r.size.width / r.size.height;
			d = d < 1 ? 1 / d : d;
			if (d < 5 && d > 3)
			{
				Point2f P[4];
				LEDRect = r;
				LEDRect.points(P);
#if LEDline
				for (int i = 0; i < 4; i++)
					line(img, P[i], P[(i + 1) % 4], Scalar(255, 0, 0), 2);
#endif
				//drawContours(img, contours, itc - contours.begin(), Scalar(0, 255, 0));
#if LED_DEBUG
				//cout << "contour after filter size is " << itc->size() << " rect is " << rect.size << " " << endl;
#endif
			}
		}
		itc++;
	}
#if LEDline
	imshow("outLED", img);
#endif
	//moveWindow("outLED", 0, 0);
#if LED_DEBUG
	imshow("Gray", Grayout);
	imshow("sobel", Sobelout);
	imshow("binaryout", Binaryout);
	imshow("close", CloseOpout);
	imshow("backup", imgbackup);
	imshow("binaryline", BinaryLine);
#endif
	return BinaryDirect;
}
void HandDigital(Mat& img, vector<RotatedRect>& rects, int* pos = NULL)
{
	Mat Gaussout, Grayout, Sobelout, Binaryout, OpenOpout, CloseOpout, Tailorout, Finalout, imgbackup(img.size(), img.type());//保存中间变量用于调试
	GaussianBlur(img, Gaussout, Size(15, 15), 0, 0);
	cvtColor(Gaussout, Grayout, CV_BGR2GRAY);
	Mat t1, t2;
	Sobel(Grayout, t1, CV_16S, 0, 1);
	convertScaleAbs(t1, t1);
	Sobel(Grayout, t2, CV_16S, 1, 0, 3);
	convertScaleAbs(t2, t2);
	addWeighted(t1, 0.8, t2, 0.8, 0, Sobelout);
	threshold(Sobelout, Binaryout, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//morphologyEx(Binaryout, CloseOpout, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(4, 4)));
	Binaryout.copyTo(CloseOpout);
	Projection(CloseOpout, Tailorout, 50, 110, img, pos);//根据投影找到手写数字最可能在的地方
	vector< vector< Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(Tailorout, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
#if HAND_DEBUG
	cout << "img size is: " << img.rows << " " << img.cols << endl;
	cout << "num of contour is " << contours.size() << endl;
#endif
	for (int i = 0; i<contours.size(); i++)
	{//删除所有子轮廓，只保留一级轮廓
		if (hierarchy[i][3] != -1 || contours[i].size() < 100)
		{
			contours.erase(contours.begin() + i);
			hierarchy.erase(hierarchy.begin() + i);
			i--;
		}
	}
	vector<vector<Point> >::iterator itc = contours.begin();
	while (itc != contours.end())
	{
		Point2f P[4];
		RotatedRect rect = minAreaRect(*itc);
		rect.points(P);
		for (int i = 0; i < 4; i++)
			line(imgbackup, P[i], P[(i + 1) % 4], Scalar(255, 0, 0), 2);
		//cout << "rect " << itc - contours.begin() << " size is: " << rect.size << endl;
		if (rect.size.width*rect.size.height < 20150 && rect.size.width*rect.size.height > 1200)
		{//如果面积太小或者太大则不考虑。如果长宽比异常也不考虑
			RotatedRect rect = minAreaRect(*itc);
			double d = rect.size.width / rect.size.height;
			d = d < 1 ? 1 / d : d;
			if (d < 1.95 && d > 1.1)
			{
				rects.push_back(rect);
				//Point2f P[4];
				//rect.points(P);
				//for (int i = 0; i < 4; i++)
				//	line(img, P[i], P[(i + 1) % 4], Scalar(255, 0, 0), 2);
				//drawContours(img, contours, itc - contours.begin(), Scalar(0, 255, 0));
#if HAND_DEBUG
				//cout << "contour after filter size is " << itc->size() << " rect is " << rect.size << " " << "center is: " << rect.center << endl;
#endif
			}
		}
		itc++;
	}
	if (rects.size() > 9)//如果最后发现矩形数量大于9个，将其按照面积大小排序，选择面积最接近的9个。
	{
		int startIndex = -1;
		sort(rects.begin(), rects.end(), cmp3);
		for (int i = 0; i < rects.size() - 9 && startIndex < 0; i++)
		{
			startIndex = i;
			for (int j = i; j < i + 9 - 1; j++)
			{
				double ratio = (rects[j].size.width*rects[j].size.height) / (rects[j + 1].size.width*rects[j + 1].size.height);
				if (ratio<0.6 || ratio >1.6)
				{
					startIndex = -1;
					break;
				}
			}
		}
		startIndex = startIndex < 0 ? rects.size() - 9 : startIndex;
		for (int i = 0; i < startIndex; i++)
			rects.erase(rects.begin());
		for (int i = startIndex + 9; i < rects.size(); i++)
			rects.erase(rects.begin() + 9);
	}
	for (int j = 0; j < rects.size(); j++)
	{
		Point2f P[4];
		rects[j].points(P);
		for (int i = 0; i < 4; i++)
			line(img, P[i], P[(i + 1) % 4], Scalar(255, 0, 0), 2);
	}
#if ShowOutHand
	imshow("outHand", img);
#endif
#if HAND_DEBUG
	imshow("Gray", Grayout);
	imshow("sobel", Sobelout);
	imshow("binaryout", Binaryout);
	imshow("close", CloseOpout);
	imshow("backup", imgbackup);
	imshow("tailor", Tailorout);
#endif
}
Mat LEDDivide(Mat& img, int* pos)
{
	Mat Gaussout, Grayout, Sobelout, Binaryout, imgbackup(img.size(), img.type()), BinDirect;
	GaussianBlur(img, Gaussout, Size(5, 5), 0, 0);
	cvtColor(Gaussout, Grayout, CV_BGR2GRAY);
	Mat t1, t2;
	Sobel(Grayout, t1, CV_16S, 0, 1);
	convertScaleAbs(t1, t1);
	Sobel(Grayout, t2, CV_16S, 1, 0, 3);
	convertScaleAbs(t2, t2);
	addWeighted(t1, 0.8, t2, 0.8, 0, Sobelout);
	threshold(Sobelout, Binaryout, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	threshold(Grayout, BinDirect, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//imshow("binary", Binaryout);

	ProjectLED(Binaryout, pos);
	int t = 0;
	Mat Token[5];
#if LEDShowOriDivided
	for (int i = 0; i < 4; i++)
	{
		Token[i] = img(Range::all(), Range(t, pos[i]));
		//resize(Token[i], Token[i], Size(50, 50));
		t = pos[i];
		stringstream s;
		s << (i + 1);
		imshow("第" + s.str() + "字符", Token[i]);
		cout << "Token " << i + 1 << "size is: " << Token[i].size << endl;
	}
	Token[4] = img(Range::all(), Range(pos[3], img.cols));
	imshow("第5字符", Token[4]);
#endif
#if LEDShowBinaryDivided
	t = 0;
	for (int i = 0; i < 4; i++)
	{
		Token[i] = BinDirect(Range::all(), Range(t, pos[i]));
		Mat tmp;
		morphologyEx(Token[i], tmp, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(2, 2)));
		t = pos[i];
		stringstream s;
		s << (i + 1);
		imshow("第" + s.str() + "字符", Token[i]);
	}
	Token[4] = BinDirect(Range::all(), Range(pos[3], img.cols));
	imshow("第5字符", Token[4]);
#endif
	return BinDirect;
}