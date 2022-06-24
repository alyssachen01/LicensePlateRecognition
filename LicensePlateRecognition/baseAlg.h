#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

void medianFilter3x3(cv::Mat src, cv::Mat &dst);
void split2RGB(cv::Mat src, cv::Mat &imageR, cv::Mat &imageG, cv::Mat &imageB);
void equalizeHistRGB(cv::Mat src, cv::Mat &dst);
void binarization(cv::Mat src, cv::Mat &dst, int threshold, bool inv = false);
void binarization_otsu(cv::Mat src, cv::Mat& dst, bool inv = false);
void image_and(cv::Mat src1, cv::Mat src2, cv::Mat &dst);
void mergeRGBimage(cv::Mat src[3], cv::Mat &dst);
