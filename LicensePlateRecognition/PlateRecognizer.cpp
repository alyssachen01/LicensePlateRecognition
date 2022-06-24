#include "pch.h"
#include "PlateRecognizer.h"

#define USE_OPENCV
string myvalue;
void PlateRecognizer::filesearch(string path, int layer, vector <string>& filepathnames)
{
	struct _finddata_t   filefind;
	string  curr = path + "\\*.*";
	int   done = 0;
	intptr_t handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)
		return;

	while (!(done = _findnext(handle, &filefind)))
	{
		//printf("测试的--%s\n", filefind.name);
		if (!strcmp(filefind.name, "..")) {
			continue;
		}
		if ((_A_SUBDIR == filefind.attrib))
		{
			curr = path + "\\" + filefind.name;
			filesearch(curr, layer + 1, filepathnames);
		}
		else
		{
			curr = path + "\\" + filefind.name;
			filepathnames.push_back(curr);
		}
	}
	_findclose(handle);
}

Mat PlateRecognizer::dealHOG(Mat inMat)
{
	Mat result = Mat::zeros(1, 972, CV_32FC1);
	//HOGDescriptor *hog = new HOGDescriptor(Size(28, 28), Size(14, 14), Size(7, 7), Size(7, 7), 9);
	vector<float>descriptors;//存放结果    为HOG描述子向量
	hog->compute(inMat, descriptors, Size(1, 1), Size(0, 0)); //Hog特征计算，检测窗口移动步长(1,1)
															  //cout << "HOG描述子向量个数    : " << descriptors.size() << endl;
	int  number = descriptors.size();
	for (int n = 0; n < number; n++)
	{
		result.at<float>(0, n) = descriptors[n];//第1个样本的特征向量中的第n个元素
	}
	return result;
}

Mat PlateRecognizer::dealLBP(Mat img)
{
	uchar   RITable[256];
	int     temp = 0;
	int     val = 0;
	cv::Mat result;
	result.create(img.rows - 2, img.cols - 2, img.type());
	result.setTo(0);
	for (int i = 0; i < 256; i++)
	{
		val = i;
		for (int j = 0; j < 7; j++)
		{
			temp = i >> 1;
			if (val > temp)
			{
				val = temp;
			}
		}
		RITable[i] = val;
	}
	for (int i = 1; i < img.rows - 1; i++)
	{
		for (int j = 1; j < img.cols - 1; j++)
		{
			uchar center = img.at<uchar>(i, j);
			uchar code = 0;

			code |= (img.at<uchar>(i - 1, j - 1) >= center) << 7;               //按位或
			code |= (img.at<uchar>(i - 1, j) >= center) << 6;
			code |= (img.at<uchar>(i - 1, j + 1) >= center) << 5;
			code |= (img.at<uchar>(i, j + 1) >= center) << 4;
			code |= (img.at<uchar>(i + 1, j + 1) >= center) << 3;
			code |= (img.at<uchar>(i + 1, j) >= center) << 2;
			code |= (img.at<uchar>(i + 1, j - 1) >= center) << 1;
			code |= (img.at<uchar>(i, j - 1) >= center) << 0;

			result.at<uchar>(i - 1, j - 1) = RITable[code];
		}
	}
	Mat	outMat = result.reshape(1, 1);
	outMat.convertTo(outMat, CV_32FC1);
	return outMat;
}

// 裁剪
void PlateRecognizer::remove_border(Mat src, Mat& dst)
{
	int height = src.rows, width = src.cols;//图像的高和宽
	int ymin = 0, ymax = height, xmin = 0, xmax = width;
	for (int i = 0; i < height; i++)
	{
		bool bk = false;
		for (int j = 0; j < width; j++)
			if (src.at<uchar>(i, j) == 255)
			{
				ymin = i;
				bk = true;
				break;
			}
		if (bk)	break;
	}
	for (int i = height - 1; i >= 0; i--)
	{
		bool bk = false;
		for (int j = 0; j < width; j++)
			if (src.at<uchar>(i, j) == 255)
			{
				ymax = i + 1;
				bk = true;
				break;
			}
		if (bk)	break;
	}
	for (int i = 0; i < width; i++)
	{
		bool bk = false;
		for (int j = 0; j < height; j++)
			if (src.at<uchar>(j, i) == 255)
			{
				xmin = i;
				bk = true;
				break;
			}
		if (bk)	break;
	}
	for (int i = width - 1; i >= 0; i--)
	{
		bool bk = false;
		for (int j = 0; j < height; j++)
			if (src.at<uchar>(j, i) == 255)
			{
				xmax = i + 1;
				bk = true;
				break;
			}
		if (bk)	break;
	}
	Mat char_area = src(Rect(Point(xmin, ymin), Point(xmax, ymax))).clone();
	char_area.copyTo(dst);
}

double PlateRecognizer::distance_c(Mat m1, Mat m2)
{
	if (m1.size() != m2.size())return -1;
	double sum = 0;
	int width = m1.cols, height = m1.rows;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ((m1.at<uchar>(i, j) == 0 && m2.at<uchar>(i, j) != 0)
				|| (m1.at<uchar>(i, j) != 0 && m2.at<uchar>(i, j) == 0)) sum += 1;
		}
	return sum;
}

PlateRecognizer::PlateRecognizer(Featrue feature)
{
	featruemodel = feature;
	model = cv::ml::SVM::create();
	model->setType(SVM::C_SVC);
	model->setKernel(SVM::LINEAR);
	model->setDegree(0);
	model->setGamma(1);
	model->setCoef0(0);
	model->setC(1);
	model->setNu(0);
	model->setP(0);
	model->setTermCriteria(CvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01));
	hog = new HOGDescriptor(Size(20, 40), Size(15, 15), Size(5, 5), Size(5, 5), 9);
}

void PlateRecognizer::init(Featrue feature)
{
	featruemodel = feature;
	if (featruemodel == HOG || featruemodel == LBP)
	{
		model = cv::ml::SVM::create();
		model->setType(SVM::C_SVC);
		model->setKernel(SVM::LINEAR);
		model->setDegree(0);
		model->setGamma(1);
		model->setCoef0(0);
		model->setC(1);
		model->setNu(0);
		model->setP(0);
		model->setTermCriteria(CvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01));
		hog = new HOGDescriptor(Size(20, 40), Size(15, 15), Size(5, 5), Size(5, 5), 9);
	}
}

void PlateRecognizer::train(vector<Mat> images, vector<int> label)
{
	Mat trainingImages;
	Mat trainImg = Mat(Size(20, 40), CV_8UC3);
	if (images.size() != label.size())
		return;
	for (int i = 0; i < images.size(); i++)
	{
		if (featruemodel == LBP)
		{
			Mat rb;
			remove_border(images[i], rb);
			::resize(rb, trainImg, trainImg.size());
			trainingImages.push_back(dealLBP(trainImg));
		}
		else
		{
			::resize(images[i], trainImg, trainImg.size());
			Mat hogMat = dealHOG(trainImg);
			trainingImages.push_back(hogMat);
		}
	}
	Mat trainingData;
	Mat classes;
	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(label).copyTo(classes);
	Ptr<TrainData> tData = TrainData::create(trainingData, ROW_SAMPLE, classes);
	model->train(tData);
}

void PlateRecognizer::savemodel(string path)
{
	model->save(path);
}

bool PlateRecognizer::readmodel(string path)
{
	if (featruemodel == HOG || featruemodel == LBP)
	{
		if ((_access(path.c_str(), 0)) == -1)
			return false;
		FileStorage svm_fs(path, FileStorage::READ);
		if (svm_fs.isOpened())
		{
			Ptr<ml::SVM> svm = Algorithm::load<ml::SVM>(path);
			model = svm;
		}
	}
	else
	{
		temp.clear();
		templabel.clear();
		vector <string> filepathnames;
		filesearch(path, 2, filepathnames);
		for (int i = 0; i < filepathnames.size(); i++)
		{
			Mat img = imread(filepathnames[i], -1);
			if (img.empty())
				return false;
			::resize(img, img, Size(20, 40));
			int index = filepathnames[i].find_last_of(".");
			string label;
			if (filepathnames.size() == 34)
				label = filepathnames[i].substr(index - 1, 1);
			else
				label = filepathnames[i].substr(index - 2, 2);
			temp.push_back(img);
			templabel.push_back(label);
		}
	}
}

void PlateRecognizer::predict(Mat image, int& result)
{
	Mat SrcImage;
	if (featruemodel == LBP)
	{
		remove_border(image, image);
		::resize(image, image, Size(20, 40));
		SrcImage = dealLBP(image);
	}
	else
	{
		Mat trainImg = Mat(Size(20, 40), CV_8UC3);
		::resize(image, trainImg, trainImg.size());
		SrcImage = dealHOG(trainImg);
	}
	result = (int)model->predict(SrcImage);
}

void PlateRecognizer::predict(Mat image, string& result)
{
#ifdef USE_OPENCV
	if (temp.size() == 0) return;
	int maxscore = 0, maxindex = 0;
	Mat resultMat;
	remove_border(image, image);
	resize(image, image, Size(20, 40));
	for (int i = 0; i < temp.size(); i++)
	{
		matchTemplate(image, temp[i], resultMat, CV_TM_CCOEFF);
		//进行匹配和标准化
		double minValue;
		double maxValue;
		//location point
		Point minLocP;
		Point maxLocP;
		//通过函数 minMaxLoc 定位最匹配的位置，越大越好
		minMaxLoc(resultMat, &minValue, &maxValue, &minLocP, &maxLocP, Mat());
		if (maxValue > maxscore)
		{
			maxscore = maxValue;
			maxindex = i;
		}
	}
	result = templabel[maxindex];
	myvalue = to_string(maxscore);
#else
	if (temp.size() == 0) return;
	int mindis = 9999999, minindex = 0;
	Mat resultMat;
	remove_border(image, image);
	if (image.cols < 8) { result = "1"; return; };
	::resize(image, image, Size(20, 40));
	for (int i = 0; i < temp.size(); i++)
	{
		double d = distance_c(image, temp[i]);
		if (mindis >= d)
		{
			mindis = d;
			minindex = i;
		}
	}
	result = templabel[minindex];
#endif
}