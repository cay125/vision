#ifndef SEGMENTATION_H
#define SEGMENTATION_H
#include <opencv2/core.hpp>  
#include <opencv2/highgui.hpp>  
#include <opencv2/imgproc.hpp>

void Projection(cv::Mat& img, cv::Mat& out, int v_thresh, int h_thresh, cv::Mat& other, int* pos = NULL);
void ProjectLED(cv::Mat& img, int* pos);
void QuickRect(cv::Mat& img);
bool cmp1(const cv::RotatedRect& a, const cv::RotatedRect& b);
bool cmp2(const cv::RotatedRect& a, const cv::RotatedRect& b);
bool cmp3(const cv::RotatedRect& a, const cv::RotatedRect& b);
void PreLED(cv::Mat& img, cv::Mat& out, std::vector<cv::RotatedRect>& rects, int* pos = NULL);
cv::Mat LEDDigital(cv::Mat& img, std::vector<cv::RotatedRect>& rects, cv::RotatedRect& LEDRect);
void HandDigital(cv::Mat& img, std::vector<cv::RotatedRect>& rects, int* pos = NULL);
cv::Mat LEDDivide(cv::Mat& img, int* pos);


#endif // !SEGMENTATION_H
