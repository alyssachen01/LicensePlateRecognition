#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <opencv\cv.h>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ml\ml.hpp>
#include <io.h>

using namespace std;
using namespace cv;
using namespace cv::ml;

enum Featrue { HOG, LBP, MatchTemplate };
extern string myvalue;

class PlateRecognizer
{
public:
	PlateRecognizer() {};
	~PlateRecognizer() {};
	PlateRecognizer(Featrue feature);
	void init(Featrue feature);
	void train(vector<Mat> images, vector<int> label);
	void savemodel(string path);
	bool readmodel(string path);
	void predict(Mat image, int &result);
	void predict(Mat image, string &result);
private:
	Ptr<SVM> model;
	HOGDescriptor *hog;
	Featrue featruemodel;
	Mat dealHOG(Mat inMat);
	Mat dealLBP(Mat inMat);
	void remove_border(Mat src, Mat &dst);
	double distance_c(Mat m1, Mat m2);
	vector<Mat> temp;
	vector<string> templabel;
	void filesearch(string path, int layer, vector <string> &filepathnames);
};