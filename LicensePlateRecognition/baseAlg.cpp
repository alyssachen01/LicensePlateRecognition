#include "pch.h"
#include "baseAlg.h"

uchar Med(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
	int arr[9];
	arr[0] = n1;
	arr[1] = n2;
	arr[2] = n3;
	arr[3] = n4;
	arr[4] = n5;
	arr[5] = n6;
	arr[6] = n7;
	arr[7] = n8;
	arr[8] = n9;
	for (int gap = 9 / 2; gap > 0; gap /= 2)
		for (int i = gap; i < 9; ++i)
			for (int j = i - gap; j >= 0 && arr[j] > arr[j + gap]; j -= gap)
			{
				int temp = arr[j];
				arr[j] = arr[j + gap];
				arr[j + gap] = temp;
			}
	return arr[4];//返回中值
}

inline int getdata(cv::Mat src,int i, int j, int bais)
{
	return src.data[i * src.cols * 3 + 3 * j + bais];
}

int OTSU(cv::Mat srcImage) {
	int nCols = srcImage.cols;
	int nRows = srcImage.rows;
	int threshold = 0;
	int nSumPix[256];
	float nProDis[256];
	for (int i = 0; i < 256; i++)
	{
		nSumPix[i] = 0;
		nProDis[i] = 0;
	}

	//统计灰度集中每个像素在整幅图像中的个数
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			nSumPix[(int)srcImage.at<uchar>(i, j)]++;
		}
	}

	//计算每个灰度级占图像中的概率分布
	for (int i = 0; i < 256; i++)
	{
		nProDis[i] = (float)nSumPix[i] / (nCols * nRows);
	}

	//遍历灰度级【0，255】，计算出最大类间方差下的阈值

	float w0, w1, u0_temp, u1_temp, u0, u1, delta_temp;
	double delta_max = 0.0;
	for (int i = 0; i < 256; i++)
	{
		//初始化相关参数
		w0 = w1 = u0 = u1 = u0_temp = u1_temp = delta_temp = 0;
		for (int j = 0; j < 256; j++)
		{
			//背景部分
			if (j <= i)
			{
				w0 += nProDis[j];
				u0_temp += j * nProDis[j];
			}
			//前景部分
			else
			{
				w1 += nProDis[j];
				u1_temp += j * nProDis[j];
			}
		}
		//计算两个分类的平均灰度
		u0 = u0_temp / w0;
		u1 = u1_temp / w1;
		//依次找到最大类间方差下的阈值
		delta_temp = (float)(w0 * w1 * pow((u0 - u1), 2)); //前景与背景之间的方差(类间方差)
		if (delta_temp > delta_max)
		{
			delta_max = delta_temp;
			threshold = i;
		}
	}
	return threshold;
}

void medianFilter3x3(cv::Mat src, cv::Mat& dst)
{
	if (!src.data)return;
	cv::Mat result(src.size(), src.type());
	for (int i = 0; i < src.rows; ++i)
		for (int j = 0; j < src.cols; ++j) {
			if ((i - 1) > 0 && (i + 1) < src.rows && (j - 1) > 0 && (j + 1) < src.cols) {
				result.data[i * src.cols * 3 + 3 * j] = Med(getdata(src, i, j, 0), getdata(src, i + 1, j + 1, 0),
					getdata(src, i + 1, j, 0), getdata(src, i, j + 1, 0), getdata(src, i + 1, j - 1, 0),
					getdata(src, i - 1, j + 1, 0), getdata(src, i - 1, j, 0), getdata(src, i, j - 1, 0),
					getdata(src, i - 1, j - 1, 0));
				result.data[i * src.cols * 3 + 3 * j + 1] = Med(getdata(src, i, j, 1), getdata(src, i + 1, j + 1, 1),
					getdata(src, i + 1, j, 1), getdata(src, i, j + 1, 1), getdata(src, i + 1, j - 1, 1),
					getdata(src, i - 1, j + 1, 1), getdata(src, i - 1, j, 1), getdata(src, i, j - 1, 1),
					getdata(src, i - 1, j - 1, 1));
				result.data[i * src.cols * 3 + 3 * j + 2] = Med(getdata(src, i, j, 2), getdata(src, i + 1, j + 1, 2),
					getdata(src, i + 1, j, 2), getdata(src, i, j + 1, 2), getdata(src, i + 1, j - 1, 2),
					getdata(src, i - 1, j + 1, 2), getdata(src, i - 1, j, 2), getdata(src, i, j - 1, 2),
					getdata(src, i - 1, j - 1, 2));
			}
			else
			{
				result.data[i * src.cols * 3 + 3 * j] = getdata(src, i, j , 0);
				result.data[i * src.cols * 3 + 3 * j + 1] = getdata(src, i, j, 1);
				result.data[i * src.cols * 3 + 3 * j + 2] = getdata(src, i, j, 2);
			}
		}
	result.copyTo(dst);//拷贝
}
void split2RGB(cv::Mat src, cv::Mat& imageB, cv::Mat& imageG, cv::Mat& imageR)
{
	cv::Mat R(src.size(), CV_8UC1);
	cv::Mat G(src.size(), CV_8UC1);
	cv::Mat B(src.size(), CV_8UC1);
	for (int i = 0; i < src.rows; ++i)
		for (int j = 0; j < src.cols; ++j) {
			B.data[i * src.cols + j] = src.data[i * src.cols * 3 + j * 3];
			G.data[i * src.cols + j] = src.data[i * src.cols * 3 + j * 3 + 1];
			R.data[i * src.cols + j] = src.data[i * src.cols * 3 + j * 3 + 2];
		}
	R.copyTo(imageR);
	G.copyTo(imageG);
	B.copyTo(imageB);
}
void equalizeHistRGB(cv::Mat src, cv::Mat& dst)
{
	
}
void binarization(cv::Mat src, cv::Mat& dst, int threshold, bool inv)
{
	cv::Mat result(src.size(), CV_8UC1);
	for (int i = 0; i < src.rows; ++i)
		for (int j = 0; j < src.cols; ++j) {
			if (inv)
				if (src.data[i * src.cols + j] < threshold)
					result.data[i * src.cols + j] = 255;
				else
					result.data[i * src.cols + j] = 0;
			else
				if (src.data[i * src.cols + j] > threshold)
					result.data[i * src.cols + j] = 255;
				else
					result.data[i * src.cols + j] = 0;
		}
	result.copyTo(dst);
}
void binarization_otsu(cv::Mat src, cv::Mat& dst, bool inv)
{
	binarization(src, dst, OTSU(src), inv);
}
void image_and(cv::Mat src1, cv::Mat src2, cv::Mat& dst)
{
	cv::Mat result(src1.size(), CV_8UC1);
	for (int i = 0; i < src1.rows; ++i)
		for (int j = 0; j < src1.cols; ++j) {
			if (src1.data[i * src1.cols + j] == 255 && src2.data[i * src2.cols + j] == 255)
				result.data[i * src1.cols + j] = 255;
			else
				result.data[i * src1.cols + j] = 0;
		}
	result.copyTo(dst);
}
void mergeRGBimage(cv::Mat src[3], cv::Mat& dst)
{
	cv::Mat result(src[0].size(), CV_8UC3);
	for (int i = 0; i < src[0].rows; ++i)
		for (int j = 0; j < src[0].cols; ++j) {
			result.data[i * src[0].cols * 3 + j * 3] = src[0].data[i * src[0].cols + j];
			result.data[i * src[0].cols * 3 + j * 3 + 1] = src[1].data[i * src[1].cols + j];
			result.data[i * src[0].cols * 3 + j * 3 + 2] = src[2].data[i * src[2].cols + j];
		}
	result.copyTo(dst);
}