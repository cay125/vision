// opencvtemplate.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"  //在VS2017中一些头文件都放在"stdafx.h"文件中
#include "Segmentation.h"
#include <iostream>  
#include <algorithm>
#include <io.h>

using namespace cv;
using namespace std;

int main()
{  
	Mat img = imread("C:/handwritings/videoframe000006331.png"), imgbackup(img.size(), img.type());//21740 15127 00000 00025 00607 02215 07124 03819 15729 21847 7124 3637 2254 3758 6393 3728 6365 3923 5581 3658 6400 21897 18783 7095
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
			//system("pause");
			//return 0;
		}
		cout << "LED ANGEL: " << LEDrect.angle<< endl;


		//Mat rot_img, rotled, rot_binary, rot_bin_led;
		//if (LEDrect.size.width < LEDrect.size.height)//说明此时矩阵长比宽小，即矩阵错位了90度。需要再旋转90度转回来
		//{
		//	warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());
		//	warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());//仿射变换，旋转图片使其水平
		//	rotled = rot_img(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
		//	rot_bin_led = rot_binary(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
		//}
		//else
		//{
		//	warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());
		//	warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());//仿射变换，旋转图片使其水平
		//	getRectSubPix(rot_img, LEDrect.size, LEDrect.center, rotled);
		//	getRectSubPix(rot_binary, LEDrect.size, LEDrect.center, rot_bin_led);
		//}
		////imshow("roteimg", rot_img);
		////imshow("rot_binary", rot_bin_led);
		//imshow("result", rotled);
		//int pos_led[4] = { 0 };
		//LEDDivide(rotled, pos_led);
		////int pos2[4] = { 0 };
		////ProjectLED(rot_bin_led, pos2);

	}
	waitKey(0);
	system("pause");
	return 0;
}

