//// opencvtemplate.cpp: �������̨Ӧ�ó������ڵ㡣
////
//
#include "stdafx.h"  //��VS2017��һЩͷ�ļ�������"stdafx.h"�ļ���
//#include "Segmentation.h"
//#include <iostream>  
//#include <algorithm>
//#include <io.h>
//#include <stdio.h>
//
//using namespace cv;
//using namespace std;
//
//void getFiles(string path, vector<string>& files)
//{
//	//�ļ����  
//	intptr_t  hFile = 0;
//	//�ļ���Ϣ  
//	struct _finddata_t fileinfo;
//	string p;
//	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
//	{
//		do
//		{
//			//�����Ŀ¼,����֮  
//			//�������,�����б�  
//			if ((fileinfo.attrib &  _A_SUBDIR))
//			{
//				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
//					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
//			}
//			else
//			{
//				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
//			}
//		} while (_findnext(hFile, &fileinfo) == 0);
//		_findclose(hFile);
//	}
//}
//int main()
//{
//	vector<string> file_names;
//	string path = "C:\\handwritings";
//	getFiles(path, file_names);
//	vector<int> index;
//	FILE *fpRead;
//	FILE *fpWrite;
//	fopen_s(&fpRead,"D:/The Project Of C++/vision/opencvtemplate/rand_d.txt", "r");
//	fopen_s(&fpWrite, "D:/The Project Of C++/vision/opencvtemplate/errors.txt", "w");
//	int end_index = file_names.size()*0.7;
//	for (int i = 0; i < file_names.size(); i++)
//	{
//		int t;
//		fscanf_s(fpRead, "%d ", &t, sizeof(int));
//		index.push_back(t);
//	}
//	for (int i = 27; i < 100; i++)
//	{
//		if (index[i] - 1 >= file_names.size())
//			continue;
//		cout << "now is in " << i << " " << file_names[index[i]-1] << endl;
//		//if (i == 10 || i == 24 || i == 27 || i == 29 || i == 66 || i == 92 || i == 129 || i == 161 || i == 281)
//		//	continue;
//		Mat img = imread(file_names[index[i]-1]), imgbackup(img.size(), img.type());//21740 15127 00000 00025 00607 02215 07124 03819 15729 21847 7124 3637 2254 3758 6393 3728 6365 3923 5581
//		if (img.empty())
//		{
//			cout << "Read img failed" << endl;
//			continue;
//		}
//		else
//		{
//			img.copyTo(imgbackup);
//			vector<RotatedRect> rects;//����9����д����
//			RotatedRect LEDrect;//����LED����
//			int pos[4] = { 0 };//�м���� ���ڵ���
//			HandDigital(imgbackup, rects, pos);
//			//cout << "total rects find: " << rects.size() << endl;
//			if (rects.size() != 9)
//			{
//				cout << i << " : error when tring to find Hand Writings" << endl;
//				fprintf_s(fpWrite, "Hand error: %s\n", file_names[index[i] - 1].c_str());
//				continue;
//			}
//
//			Mat LEDCut;
//			PreLED(img, LEDCut, rects, pos);//����ͶӰ��9����д���ַ�λ���ָ������λ��
//			Mat binary = LEDDigital(LEDCut, rects, LEDrect);
//			if (LEDrect.size.height == 0 && LEDrect.size.width == 0)
//			{
//				cout << i << " : error when tring to find LED" << endl;
//				fprintf_s(fpWrite, "LED error: %s\n", file_names[index[i] - 1].c_str());
//				continue;
//			}
//			//cout << "LED ANGEL: " << LEDrect.angle << endl;
//
//
//			//Mat rot_img, rotled, rot_binary, rot_bin_led;
//			//if (LEDrect.size.width < LEDrect.size.height)//˵����ʱ���󳤱ȿ�С���������λ��90�ȡ���Ҫ����ת90��ת����
//			//{
//			//	warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());
//			//	warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, 90 + LEDrect.angle, 1.0), LEDCut.size());//����任����תͼƬʹ��ˮƽ
//			//	rotled = rot_img(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
//			//	rot_bin_led = rot_binary(Rect(LEDrect.center.x - LEDrect.size.height / 2, LEDrect.center.y - LEDrect.size.width / 2, LEDrect.size.height, LEDrect.size.width));
//			//}
//			//else
//			//{
//			//	warpAffine(binary, rot_binary, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());
//			//	warpAffine(LEDCut, rot_img, getRotationMatrix2D(LEDrect.center, LEDrect.angle, 1.0), LEDCut.size());//����任����תͼƬʹ��ˮƽ
//			//	getRectSubPix(rot_img, LEDrect.size, LEDrect.center, rotled);
//			//	getRectSubPix(rot_binary, LEDrect.size, LEDrect.center, rot_bin_led);
//			//}
//			////imshow("roteimg", rot_img);
//			////imshow("rot_binary", rot_bin_led);
//			////imshow("result", rotled);
//			//int pos_led[4] = { 0 };
//			//Mat BinDirect = LEDDivide(rotled, pos_led);
//
//			//int t = 0;
//			//Mat Token[5];
//			//stringstream q; string name;
//			//for (int j = 0; j < 4; j++)
//			//{
//			//	Token[j] = BinDirect(Range::all(), Range(t, pos_led[j]));
//			//	resize(Token[j], Token[j], Size(35, 25));
//			//	t = pos_led[j];
//			//	stringstream s, q;
//			//	s << (j + 1);
//			//	q << (i + 1);
//			//	name = "D:\\The Project Of C++\\vision\\opencvtemplate\\segmentation\\" + q.str() + "_" + s.str() + ".jpg";
//			//	imwrite(name, Token[j]);
//			//}
//			//if (pos_led[3] == 0)
//			//{
//			//	cout << i << " : error when tring to find segmentation" << endl;
//			//	continue;
//			//}
//			//q << (i + 1);
//			//Token[4] = BinDirect(Range::all(), Range(pos_led[3], BinDirect.cols));
//			//resize(Token[4], Token[4], Size(35, 25));
//			//name = "D:\\The Project Of C++\\vision\\opencvtemplate\\segmentation\\" + q.str() + "_5.jpg";
//			//imwrite(name, Token[4]);
//		}
//	}
//	fclose(fpWrite);
//	waitKey(0);
//	system("pause");
//	return 0;
//}
//
//
