// LicensePlateRecognitionDlg.cpp: 实现文件
//

#include "pch.h"
#include "baseAlg.h"
#include "framework.h"
#include "LicensePlateRecognition.h"
#include "LicensePlateRecognitionDlg.h"
#include "afxdialogex.h"

#define USE_OPENCV
#define MIN_CONTOUR_RATIO  2.0
#define MAX_CONTOUR_RATIO  5.0
#define MIN_CONTOUR_AREA  1000.0
#define MAX_CONTOUR_AREA  50000.0

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace cv;
using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CLicensePlateRecognitionDlg 对话框

CLicensePlateRecognitionDlg::CLicensePlateRecognitionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LICENSEPLATERECOGNITION_DIALOG, pParent)
	, m_result(_T(""))
	, m_value(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLicensePlateRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESULT, m_result);
	m_value = myvalue.c_str();
	DDX_Text(pDX, IDC_VALUE, m_value);
}

BEGIN_MESSAGE_MAP(CLicensePlateRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENVIDEO, &CLicensePlateRecognitionDlg::OnBnClickedOpenvideo)
	ON_BN_CLICKED(IDC_OPENFRAME, &CLicensePlateRecognitionDlg::OnBnClickedOpenframe)
	ON_BN_CLICKED(IDC_TRAIN, &CLicensePlateRecognitionDlg::OnBnClickedTrain)
	ON_BN_CLICKED(IDC_RECO1, &CLicensePlateRecognitionDlg::OnBnClickedReco1)
	ON_BN_CLICKED(IDC_RECO2, &CLicensePlateRecognitionDlg::OnBnClickedReco2)
	ON_BN_CLICKED(IDC_RECO3, &CLicensePlateRecognitionDlg::OnBnClickedReco3)
	ON_BN_CLICKED(IDC_RECO4, &CLicensePlateRecognitionDlg::OnBnClickedReco4)
	ON_BN_CLICKED(IDC_RECO5, &CLicensePlateRecognitionDlg::OnBnClickedReco5)
	ON_BN_CLICKED(IDC_LBPSVM, &CLicensePlateRecognitionDlg::OnBnClickedLbpsvm)
END_MESSAGE_MAP()

//文件路径
void filesearch(string path, int layer, vector <string>& filepathnames)
{
	struct _finddata_t   filefind;
	string  curr = path + "\\*.*";
	int   done = 0;
	intptr_t handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)
		return;

	while (!(done = _findnext(handle, &filefind)))
	{
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

// CLicensePlateRecognitionDlg 消息处理程序

BOOL CLicensePlateRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

									// TODO: 在此添加额外的初始化代码
	f.CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
	GetDlgItem(IDC_STATIC)->SetFont(&f);
	GetDlgItem(IDC_RESULT)->SetFont(&f);
	UpdateData(FALSE);
	if (_access("provincemodel-lbp.xml", 0) != -1 && _access("nummodel-lbp.xml", 0) != -1)
	{
		foundmodel += 1;
		NumModelLBP.init(LBP);
		NumModelLBP.readmodel("nummodel-lbp.xml");
		ProvinceModelLBP.init(LBP);
		ProvinceModelLBP.readmodel("provincemodel-lbp.xml");
	}
	if (_access("provincemodel-hog.xml", 0) != -1 && _access("nummodel-hog.xml", 0) != -1)
	{
		foundmodel += 2;
		NumModelHOG.init(HOG);
		NumModelHOG.readmodel("nummodel-hog.xml");
		ProvinceModelHOG.init(HOG);
		ProvinceModelHOG.readmodel("provincemodel-hog.xml");
	}
	vector<string> filesNum;
	vector<string> filesProvince;
	filesearch("model\\num2", 2, filesNum);
	filesearch("model\\province2", 2, filesProvince);
	string label = "";
	int key = -1;
	for (int i = 0; i < filesNum.size(); i++)
	{
		int index = filesNum[i].find_last_of("\\");
		string labelname = filesNum[i].substr(index - 1, 1);
		if (labelname != label)
		{
			label = labelname;
			key++;
			NumMap.insert(map<int, string>::value_type(key, labelname));
		}
	}
	label = "";
	key = -1;
	for (int i = 0; i < filesProvince.size(); i++)
	{
		int index = filesProvince[i].find_last_of("\\");
		string labelname = filesProvince[i].substr(index - 2, 2);
		if (labelname != label)
		{
			label = labelname;
			key++;
			ProvinceMap.insert(map<int, string>::value_type(key, labelname));
		}
	}
	((CButton*)GetDlgItem(IDC_SVM))->SetCheck(TRUE);
	GetDlgItem(IDC_RECO1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RECO2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RECO3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RECO4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RECO5)->ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLicensePlateRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLicensePlateRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLicensePlateRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//垂直投影
vector<Mat> verticalProjectionMat(Mat srcImg)
{
	bitwise_not(srcImg, srcImg);
	int perPixelValue;//每个像素的值
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[width];//创建用于储存每列白色像素个数的数组
	memset(projectValArry, 0, width * 4);//初始化数组
	for (int col = 0; col < width; col++)
	{
		for (int row = 0; row < height; row++)
		{
			perPixelValue = srcImg.at<uchar>(row, col);
			if (perPixelValue == 0)//白字
			{
				projectValArry[col]++;
			}
		}
	}
	Mat verticalProjectionMat(height, width, CV_8UC1);//垂直投影的画布
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perPixelValue = 255;  //背景设置为白色
			verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
		}
	}
	for (int i = 0; i < width; i++)//垂直投影直方图
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;  //直方图设置为黑色
			verticalProjectionMat.at<uchar>(height - 1 - j, i) = perPixelValue;
		}
	}
	
	vector<Mat> roiList;//用于储存分割出来的每个字符
	int startIndex = 0;//记录进入字符区的索引
	int endIndex = 0;//记录进入空白区域的索引
	bool inBlock = false;//是否遍历到了字符区内
	for (int i = 0; i < srcImg.cols; i++)//cols=width
	{
		if (!inBlock && projectValArry[i] > 0)//进入字符区
		{
			inBlock = true;
			startIndex = i;
		}
		else if (projectValArry[i] == 0 && inBlock)//进入空白区
		{
			endIndex = i;
			inBlock = false;
			Mat roiImg = srcImg(Range(0, srcImg.rows), Range(startIndex, endIndex + 1));
			if (roiImg.cols > (srcImg.cols / 16) || ((roiImg.cols > 2) && projectValArry[i - 1] > 20))
			{
				bitwise_not(roiImg, roiImg);
				if (roiImg.cols < 8)
				{
					Mat pi1 = Mat::zeros(roiImg.rows, 15, CV_8UC1);
					Mat disp1(pi1, Rect((15 - roiImg.cols) / 2, 0, roiImg.cols, roiImg.rows));
					roiImg.copyTo(disp1);
					roiList.push_back(pi1);
				}
				else
					roiList.push_back(roiImg);
			}
		}
	}
	delete[] projectValArry;
	return roiList;
}

void cut_bounder(Mat src, Mat& dst)
{
	int height = src.rows, width = src.cols;//图像的高和宽
	int ymin = 0, ymax = height;
	for (int i = height / 2; i < height; i++)
	{
		int blacknum = 0;
		for (int j = 0; j < width; j++)
			if (src.at<uchar>(i, j) == 255)
				blacknum++;
		if (blacknum < 20)
		{
			ymax = i;
			break;
		}
	}
	for (int i = height / 2; i >= 0; i--)
	{
		int blacknum = 0;
		for (int j = 0; j < width; j++)
			if (src.at<uchar>(i, j) == 255)
				blacknum++;
		if (blacknum < 20)
		{
			ymin = i;
			break;
		}
	}
	Mat char_area = src(Rect(Point(0, ymin), Point(width, ymax))).clone();
	char_area.copyTo(dst);
}

//在窗口显示视频
void CLicensePlateRecognitionDlg::DrawcvMat(cv::Mat m_cvImg, UINT ID)
{
	cv::Mat img;
	CRect rect;

	GetDlgItem(ID)->GetClientRect(&rect);
	if (rect.Width() % 4 != 0)
	{
		rect.SetRect(rect.left, rect.top, rect.left + (rect.Width() + 3) / 4 * 4, rect.bottom);  //调整图像宽度为4的倍数
		GetDlgItem(ID)->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE);
	}

	cv::Rect dst(rect.left, rect.top, rect.right, rect.bottom);
	cv::resize(m_cvImg, img, cv::Size(rect.Width(), rect.Height()));  //使图像适应控件大小

	unsigned int m_buffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	BITMAPINFO* m_bmi = (BITMAPINFO*)m_buffer;
	BITMAPINFOHEADER* m_bmih = &(m_bmi->bmiHeader);
	memset(m_bmih, 0, sizeof(*m_bmih));
	m_bmih->biSize = sizeof(BITMAPINFOHEADER);
	m_bmih->biWidth = img.cols;   //必须为4的倍数
	m_bmih->biHeight = -img.rows; //在自下而上的位图中 高度为负
	m_bmih->biPlanes = 1;
	m_bmih->biCompression = BI_RGB;
	m_bmih->biBitCount = 8 * img.channels();

	if (img.channels() == 1)  //当图像为灰度图像时需要设置调色板颜色
	{
		for (int i = 0; i < 256; i++)
		{
			m_bmi->bmiColors[i].rgbBlue = i;
			m_bmi->bmiColors[i].rgbGreen = i;
			m_bmi->bmiColors[i].rgbRed = i;
			m_bmi->bmiColors[i].rgbReserved = 0;
		}
	}

	CDC* pDC = GetDlgItem(ID)->GetDC();
	::StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, rect.Width(), rect.Height(), img.data, (BITMAPINFO*)m_bmi, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(pDC);
}

//显示到屏幕上
void CLicensePlateRecognitionDlg::Display(int num)
{
	DrawcvMat(DisplayImages[num][0], IDC_KEY);
	DrawcvMat(DisplayImages[num][1], IDC_LICENSE);
	DrawcvMat(DisplayImages[num][2], IDC_PROVINCE);
	DrawcvMat(DisplayImages[num][3], IDC_LICENSENUM1);
	DrawcvMat(DisplayImages[num][4], IDC_LICENSENUM2);
	DrawcvMat(DisplayImages[num][5], IDC_LICENSENUM3);
	DrawcvMat(DisplayImages[num][6], IDC_LICENSENUM4);
	DrawcvMat(DisplayImages[num][7], IDC_LICENSENUM5);
	DrawcvMat(DisplayImages[num][8], IDC_LICENSENUM6);
	m_result = "";
	//省份预测
	if (tempmodel == 2)
	{
		string p;
		ProvinceModelHOG.predict(DisplayImages[num][2], p);
		m_result += p.c_str();
	}
	else if (tempmodel == 1)
	{
		int p;
		ProvinceModelLBP.predict(DisplayImages[num][2], p);
		m_result += ProvinceMap.find(p)->second.c_str();
	}
	else
	{
		int p;
		ProvinceModelHOG.predict(DisplayImages[num][2], p);
		m_result += ProvinceMap.find(p)->second.c_str();
	}
	//字母数字预测
	for (int i = 3; i < 9; i++)
	{
		Mat pi1 = Mat::zeros(DisplayImages[num][i].rows, DisplayImages[num][i].cols + 2, CV_8UC1);
		Mat disp1(pi1, Rect(2, 0, DisplayImages[num][i].cols, DisplayImages[num][i].rows));
		DisplayImages[num][i].copyTo(disp1);
		//模板匹配
		if (tempmodel == 2)
		{
			string p;
			NumModelHOG.predict(pi1, p);
			m_result += p.c_str();
		}
		else if (tempmodel == 1)
		{
			int p;
			NumModelLBP.predict(pi1, p);
			m_result += NumMap.find(p)->second.c_str();
		}
		else
		{
			int p;
			NumModelHOG.predict(pi1, p);
			m_result += NumMap.find(p)->second.c_str();
		}
	}
	UpdateData(FALSE);
}

vector<Rect> preprocess(Mat src, Mat& dst, vector <Mat>& roi)
{
	vector<Rect> rt;
	Mat ss;
	src.copyTo(ss);
	src.copyTo(dst);
	Mat imageRGB[3], imagebin[3];
#ifdef USE_OPENCV
	medianBlur(src, src, 3);
	split(src, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	threshold(imageRGB[0], imagebin[0], 100, 255, CV_THRESH_BINARY);
	threshold(imageRGB[1], imagebin[1], 70, 255, CV_THRESH_BINARY_INV);
	threshold(imageRGB[2], imagebin[2], 50, 255, CV_THRESH_BINARY_INV);
	bitwise_and(imagebin[0], imagebin[1], imagebin[0]);
	bitwise_and(imagebin[0], imagebin[2], imagebin[0]);
#else
	medianBlur(src, src, 3);   //medianFilter3x3(src, src);
	split2RGB(src, imageRGB[0], imageRGB[1], imageRGB[2]);
	for (int i = 0; i < 3; i++)
	{
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	binarization(imageRGB[0], imagebin[0], 100);
	binarization(imageRGB[1], imagebin[1], 70, true);
	binarization(imageRGB[2], imagebin[2], 70, true);
	image_and(imagebin[0], imagebin[1], imagebin[0]);
	image_and(imagebin[0], imagebin[2], imagebin[0]);
#endif

	Mat element = getStructuringElement(MORPH_RECT, Size(21, 15));
	Mat close_result;
	morphologyEx(imagebin[0], close_result, MORPH_CLOSE, element);
	//imshow("ccc", close_result);
	//waitKey();
#ifdef USE_OPENCV
	merge(imageRGB, 3, dst);
#else
	mergeRGBimage(imageRGB, dst);
#endif

	vector<vector<Point> > contours;
	int largest_area = 0, largest_contour_index = 0, second_area = 0, second_contour_index = 0;
	RotatedRect largest_bounding_rect, second_bounding_rect;
	Rect largest_rect, second_rect;
	findContours(close_result, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE); // Find the contours in the image
	if (contours.size() < 1)return rt;
	for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
	{
		double area = contourArea(contours[i]);  //  Find the area of contour
		if (area > largest_area)
		{
			second_area = largest_area;
			second_contour_index = largest_contour_index;
			second_bounding_rect = largest_bounding_rect; // Find the bounding rectangle for biggest contour
			second_rect = largest_rect;
			largest_area = area;
			largest_contour_index = i;               //Store the index of largest contour
			largest_bounding_rect = minAreaRect(contours[i]); // Find the bounding rectangle for biggest contour
			largest_rect = boundingRect(contours[i]);
		}
		else if (area > second_area)
		{
			second_area = area;
			second_contour_index = i;               //Store the index of largest contour
			second_bounding_rect = minAreaRect(contours[i]); // Find the bounding rectangle for biggest contour
			second_rect = boundingRect(contours[i]);
		}
	}

	if (second_area > 1200 && (second_rect.width / second_rect.height) > 1.5)
		if (abs(second_bounding_rect.angle) < 5)
		{
			rectangle(dst, second_rect, Scalar(0, 0, 255), 3);
			Rect in = second_rect;
			in.x -= 1;
			in.y += 1;
			in.width += 2;
			in.height -= 4;
			if (in.x > 0 && in.y > 0 && in.x + in.width < ss.cols && in.y + in.height < ss.rows) {
				roi.push_back(ss(in));
				rt.push_back(second_rect);
			}
		}
		else
		{
			Point2f vertices[4];
			Point2f dstPoints[4];
			dstPoints[0] = Point2f(second_bounding_rect.size.height + 6, second_bounding_rect.size.width - 4);
			dstPoints[1] = Point2f(3, second_bounding_rect.size.width - 4);
			dstPoints[2] = Point2f(3, -2);
			dstPoints[3] = Point2f(second_bounding_rect.size.height + 6, -2);
			second_bounding_rect.points(vertices);//获取矩形的四个点
			if (second_bounding_rect.angle < -70)
				for (int i = 0; i < 4; i++)
				{
					line(dst, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 3);
				}
			double k = (double)(vertices[1].y - vertices[0].y) / (double)(vertices[1].x - vertices[0].x);
			double b = vertices[0].y - k * vertices[0].x;
			vertices[1].y = k * vertices[2].x + b;
			vertices[1].x = vertices[2].x;
			k = (double)(vertices[2].y - vertices[3].y) / (double)(vertices[2].x - vertices[3].x);
			b = vertices[3].y - k * vertices[3].x;
			vertices[3].y = k * vertices[0].x + b;
			vertices[3].x = vertices[0].x;

			Mat M1 = getAffineTransform(vertices, dstPoints);
			Mat deal_img;
			warpAffine(ss, deal_img, M1, Size(second_bounding_rect.size.height + 6, second_bounding_rect.size.width - 4));
			roi.push_back(deal_img);
			rt.push_back(Rect(second_bounding_rect.center, Size(second_bounding_rect.size.height, second_bounding_rect.size.width)));
		}

	if (largest_area > 1200 && (largest_rect.width / largest_rect.height) > 1.5)
		if (abs(largest_bounding_rect.angle) < 5)
		{
			rectangle(dst, largest_rect, Scalar(0, 0, 255), 3);
			Rect in = largest_rect;
			in.x -= 1;
			in.y += 5;
			in.width += 2;
			in.height -= 13;
			if (in.x > 0 && in.y > 0 && in.height > 0 && in.x + in.width < ss.cols && in.y + in.height < ss.rows) {
				roi.push_back(ss(in));
				rt.push_back(largest_rect);
			}
		}
		else
		{
			Point2f vertices[4];
			Point2f dstPoints[4];
			dstPoints[0] = Point2f(largest_bounding_rect.size.height + 6, largest_bounding_rect.size.width - 4);
			dstPoints[1] = Point2f(3, largest_bounding_rect.size.width - 4);
			dstPoints[2] = Point2f(3, -2);
			dstPoints[3] = Point2f(largest_bounding_rect.size.height + 6, -2);
			largest_bounding_rect.points(vertices);//获取矩形的四个点
			if (largest_bounding_rect.angle < -70)
				for (int i = 0; i < 4; i++)
				{
					line(dst, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 3);
				}
			double k = (double)(vertices[1].y - vertices[0].y) / (double)(vertices[1].x - vertices[0].x);
			double b = vertices[0].y - k * vertices[0].x;
			vertices[1].y = k * vertices[2].x + b;
			vertices[1].x = vertices[2].x;
			k = (double)(vertices[2].y - vertices[3].y) / (double)(vertices[2].x - vertices[3].x);
			b = vertices[3].y - k * vertices[3].x;
			vertices[3].y = k * vertices[0].x + b;
			vertices[3].x = vertices[0].x;

			Mat M1 = getAffineTransform(vertices, dstPoints);
			Mat deal_img;
			warpAffine(ss, deal_img, M1, Size(largest_bounding_rect.size.height + 6, largest_bounding_rect.size.width - 4));
			roi.push_back(deal_img);
			rt.push_back(Rect(largest_bounding_rect.center, Size(largest_bounding_rect.size.height, largest_bounding_rect.size.width)));
		}
	return rt;
}

//去除左边框
int delLeftRow(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;
	int cnt = 0;
	for (int i = 0; i < roi_row - 1; i++)
	{
		for (int j = 0; j < roi_col - 1; j++)
		{
			pix = outputArray.at<uchar>(i, j);
			if (pix > 0)
			{
				cnt++;
				break;
			}
		}
	}
	int up = (roi_row >> 3) * 7.5;
	if (cnt >= up)
	{
		cv::Rect m_select;
		m_select = Rect(1, 0, outputArray.cols - 1, outputArray.rows);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delLeftRow(outputArray);
	}
	return 0;
}

//去除右边框
int delRightRow(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;
	int cnt = 0;
	for (int i = roi_row - 1; i > 0; i--)
	{
		for (int j = 0; j < roi_col - 1; j++)
		{
			pix = outputArray.at<uchar>(i, j);
			if (pix > 0)
			{
				cnt++;
				break;
			}
		}
	}
	int up = (roi_row >> 3) * 7.5;
	if (cnt >= up)
	{
		cv::Rect m_select;
		m_select = Rect(0, 0, outputArray.cols - 1, outputArray.rows);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delRightRow(outputArray);
	}
	return 0;
}

//去除上边框
int delUpCol(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;

	int cnt = 0;
	for (int i = 0; i < roi_col - 1; i++)
	{
		for (int j = 0; j < roi_row - 1; j++)
		{
			pix = outputArray.at<uchar>(j, i);
			if (pix > 0)
			{
				cnt++;
				break;
			}
		}
	}
	int up = (roi_col >> 3) * 7;
	if (cnt >= up)
	{
		cv::Rect m_select;
		m_select = Rect(0, 1, outputArray.cols, outputArray.rows - 1);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delUpCol(outputArray);
	}
	return 0;
}

//去除下边框
int delDownCol(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;

	int cnt = 0;
	for (int i = 0; i < roi_col - 1; i++)
	{
		for (int j = roi_row - 1; j >= 0; j--)
		{
			pix = outputArray.at<uchar>(j, i);
			if (pix > 0)
			{
				cnt++;
				break;
			}
		}
	}
	int up = (roi_col >> 3) * 6;
	if (cnt >= up)
	{
		cv::Rect m_select;
		m_select = Rect(0, 0, outputArray.cols, outputArray.rows - 1);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delDownCol(outputArray);
	}
	//waitKey(0);
	return 0;
}

//去除上铆钉
int delUpRivet(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;

	int cnt = 0;
	for (int i = 0; i < roi_col - 1; i++)
	{
		pix = outputArray.at<uchar>(0, i);
		if (pix > 0)
		{
			cnt++;
		}
	}
	int quarter = (roi_col >> 4) * 6;
	if (cnt < quarter)
	{
		cv::Rect m_select;
		m_select = Rect(0, 1, outputArray.cols, outputArray.rows - 1);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delUpRivet(outputArray);
	}
	return 0;
}

//去除下铆钉
int delDownRivet(cv::Mat& outputArray)
{
	int roi_col = outputArray.cols;
	int roi_row = outputArray.rows;
	uchar pix;

	int cnt = 0;
	for (int i = 0; i < roi_col - 1; i++)
	{
		pix = outputArray.at<uchar>(roi_row - 1, i);
		if (pix > 0)
		{
			cnt++;
		}
	}
	int quarter = roi_col >> 4;
	if (cnt < quarter)
	{
		cv::Rect m_select;
		m_select = Rect(0, 0, outputArray.cols, outputArray.rows - 1);
		Mat ROI = outputArray(m_select);
		outputArray = ROI;
		delDownRivet(outputArray);
	}
	return 0;
}

//去掉边框
int delRectRivet(const cv::Mat& inputArray, cv::Mat& outputArray)
{
	outputArray = inputArray;
	cv::Rect m_select;
	/*m_select = Rect(0, 0, outputArray.cols >> 1, outputArray.rows);
	Mat LROI = outputArray(m_select);
	m_select = Rect(outputArray.cols >> 1, 0, outputArray.cols - (outputArray.cols >> 1), outputArray.rows);
	Mat RROI = outputArray(m_select);

	delLeftRow(LROI);
	delRightRow(RROI);

	Mat LR;
	hconcat(LROI, RROI, LR);
	outputArray = LR;*/

	m_select = Rect(0, 0, outputArray.cols, outputArray.rows >> 1);
	Mat UROI = outputArray(m_select);
	m_select = Rect(0, outputArray.rows >> 1, outputArray.cols, outputArray.rows - (outputArray.rows >> 1));
	Mat DROI = outputArray(m_select);

	delUpCol(UROI);
	delDownCol(DROI);

	delUpRivet(UROI);
	delDownRivet(DROI);

	Mat UD;
	vconcat(UROI, DROI, UD);
	outputArray = UD;
	return 0;
}

//基于颜色的车牌提取
void myLPR(Mat srcFrame, Mat& dst)
{
	//克隆图
	Mat img;
	//bgr32位图
	Mat BGRimg;
	//hsv转换图
	Mat HSVimg;
	//处理结果图
	Mat resultImg;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	int imageWidth, imageHeight;
	int maxDif = 50;
	//颜色过滤--找到蓝色区域

	double length, area, rectArea;
	double rectDegree = 0.0; //矩形度
	double long2Short = 1.0; //体态比
	//计算边界序列的参数 长度 面积 矩形 最小矩形
	//并输出每个边界的参数
	int imageCnt = 14;
	double axisLong = 0.0, axisShort = 0.0;
	double temp;
	if (!srcFrame.empty())
	{
		imageWidth = srcFrame.cols;
		imageHeight = srcFrame.rows;
		img = srcFrame.clone();

		//调整格式32fc3
		img.convertTo(BGRimg, CV_32FC3, 1.0 / 255, 0);
		//转换hsv图
		cvtColor(BGRimg, HSVimg, COLOR_BGR2HSV);
		resultImg = Mat::zeros(imageHeight, imageWidth, CV_8UC1);
		Mat regionImg = Mat::zeros(imageHeight, imageWidth, CV_8UC1);
		//inRange(HSVimg, Scalar(200, 0.4, 0.3), Scalar(255, 1, 1), regionImg);

		for (int i = 0; i < imageHeight; ++i) {
			uchar* ptr = regionImg.ptr<uchar>(i);
			for (int j = 0; j < imageWidth; ++j)
			{
				float pixelH = HSVimg.at<Vec3f>(i, j)[0];  //读取通道值
				float pixelS = HSVimg.at<Vec3f>(i, j)[1];
				float pixelV = HSVimg.at<Vec3f>(i, j)[2];

				//颜色定位图像--蓝色
				if (pixelH > 200.0 && pixelH < 255.0)
				{
					if (pixelS > 0.4 && pixelS < 1.0 && pixelV > 0.3 && pixelV < 1.0)
						ptr[j] = 255;
				}
				//黄色车牌
				else if (pixelH > 25.0 && pixelH < 55.0)
				{
					if (pixelS > 0.4 && pixelS < 1.0 && pixelV > 0.3 && pixelV < 1.0)
						ptr[j] = 255;
				}
				else
				{
					ptr[j] = 0;
				}
			}
		}
		//******************画矩形框
		//先预处理图片
		cv::dilate(regionImg, regionImg, kernel);//膨胀
		cv::dilate(regionImg, regionImg, kernel);//膨胀
		cv::dilate(regionImg, regionImg, kernel);//膨胀
		cv::erode(regionImg, regionImg, kernel);//腐蚀
		cv::erode(regionImg, regionImg, kernel);//腐蚀
		cv::erode(regionImg, regionImg, kernel);//腐蚀
		GaussianBlur(regionImg, regionImg, Size(5, 5), 1, 1);

		//检测轮廓
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		Mat mask = regionImg.clone();
		findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
		int cnt = contours.size();
		cout << "轮廓数 " << cnt << endl;
		vector< vector<Point> > contours_poly(contours.size());
		double degree = 0;
		Mat rotImg;
		for (int index = 0; index < contours.size(); index++)
		{
			length = arcLength(Mat(contours[index]), false);//false的意思是绝对值
			area = contourArea(Mat(contours[index]), false);

			if (area > MIN_CONTOUR_AREA && area < MAX_CONTOUR_AREA)
			{
				//最小斜矩形
				RotatedRect rect = minAreaRect(Mat(contours[index]));
				Mat boxPtsMat;
				vector<Point2f> boxPts(4);
				rect.points(boxPts.data());
				boxPoints(rect, boxPtsMat);// The order of the box points: bottom left, top left, top right, bottom right
				//分析区域的形状特征，能不能达到车牌的比例

				//长轴 短轴
				axisLong = sqrt(pow(boxPts[0].x - boxPts[1].x, 2) + pow(boxPts[0].y - boxPts[1].y, 2));
				axisShort = sqrt(pow(boxPts[2].x - boxPts[1].x, 2) + pow(boxPts[2].y - boxPts[1].y, 2));

				if (axisShort > axisLong)
				{
					temp = axisLong;
					axisLong = axisShort;
					axisShort = temp;
				}
				rectArea = axisLong * axisShort;
				rectDegree = area / rectArea;
				//体态比or长宽比满足车牌 最下外接矩形的长轴和短轴的比值
				long2Short = axisLong / axisShort;
				if (long2Short > MIN_CONTOUR_RATIO && long2Short < MAX_CONTOUR_RATIO && rectDegree > 0.63)
				{
					int width = rect.size.width;
					int height = rect.size.height;
					//两个象限的矩形旋转方法不同
					if (width < height)
					{
						degree = rect.angle - 90;
						//交换点的顺序0-1，1-2，2-3，3-0
						float ptr3x = boxPtsMat.at<float>(3, 0);
						float ptr3y = boxPtsMat.at<float>(3, 1);
						for (int i = boxPtsMat.rows - 1; i > 0; --i) {
							for (int j = 0; j < boxPtsMat.cols; ++j)
							{
								boxPtsMat.at<float>(i, j) = boxPtsMat.at<float>(i - 1, j);
							}
						}
						boxPtsMat.at<float>(0, 0) = ptr3x;
						boxPtsMat.at<float>(0, 1) = ptr3y;
					}
					else
						degree = rect.angle;
					cout << "度数" << degree << "尺寸" << width << " " << height << endl;
					cout << "矩形" << boxPtsMat << endl;
					//把宽度设置为长边
					if (width < height)
					{
						temp = width;
						width = height;
						height = temp;
					}
					//自己设置一个ROI
					Mat cutImg = (Mat_<float>(4, 2) << 0, height - 1,
						0, 0,
						width - 1, 0,
						width - 1, height - 1);
					Mat MP = getPerspectiveTransform(boxPtsMat, cutImg);
					warpPerspective(srcFrame, rotImg, MP, Size(width, height));

					//方法2
					//旋转纠正
					//Point center = Point(imageWidth / 2, imageWidth / 2);
					////获得变换矩阵
					//Mat rot(2, 3, CV_32FC1);
					//rot = getRotationMatrix2D(center, degree, 0.8);//getRotationMatrix2D():以X轴正方形为起点，顺时针为负，逆时针为正
					//
					//warpAffine(srcFrame, srcFrame, rot, srcFrame.size());
				}
			}
		}
		//切割后的车牌二值化，提取字符
		Mat license;
		cvtColor(rotImg, license, CV_BGR2GRAY);
		cv::threshold(license, license, 0, 255, CV_THRESH_OTSU);
		//int pix = 0;
		//for (int col = 0; col < license.cols; col++)
		//{
		//	for (int row = 0; row < license.rows; row++)
		//	{
		//		uchar perPixelValue = license.at<uchar>(row, col);
		//		if (perPixelValue == 0)//如果是白底黑字
		//		{
		//			pix++;
		//		}
		//	}
		//}
		//if (pix > license.cols * license.rows * 0.5) {
		//	bitwise_not(license, license);
		//}

		//切割边框&铆钉
		delRectRivet(license, dst);		
	}
}

//打开视频文件
void CLicensePlateRecognitionDlg::OnBnClickedOpenvideo()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_HOGSVM))->GetCheck())
		tempmodel = 0;
	else if (((CButton*)GetDlgItem(IDC_LBPSVM))->GetCheck())
		tempmodel = 1;
	else
		tempmodel = 2;
	if ((foundmodel == 0 || foundmodel == 1) && tempmodel == 0)
	{
		MessageBox(_T("未找到HOG+SVM模型，请先训练模型！"));
		return;
	}
	else if ((foundmodel == 0 || foundmodel == 2) && tempmodel == 1)
	{
		MessageBox(_T("未找到LBP+SVM模型，请先训练模型！"));
		return;
	}
	//模板匹配
	else if (((CButton*)GetDlgItem(IDC_TEMP))->GetCheck())
	{
		NumModelHOG.init(MatchTemplate);
		NumModelHOG.readmodel("model\\MatchTemplate\\num");
		ProvinceModelHOG.init(MatchTemplate);
		ProvinceModelHOG.readmodel("model\\MatchTemplate\\province");
	}
	CString str;
	CFileDialog dlg(TRUE, _T("*"), _T(""), OFN_OVERWRITEPROMPT, _T("Data File(*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetPathName();
		if (str.GetLength() == 0)
		{
			MessageBox(_T("无法打开视频文件"));
			return;
		}
		DisplayImages.clear();
		((CStatic*)GetDlgItem(IDC_KEY))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSE))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_PROVINCE))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM1))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM2))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM3))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM4))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM5))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM6))->SetBitmap(NULL);
		GetDlgItem(IDC_RECO1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECO2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECO3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECO4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECO5)->ShowWindow(SW_HIDE);
		this->RedrawWindow();
		m_result = "";
		VideoCapture cap;
		cap.open(str.GetBuffer());

		if (!cap.isOpened())//如果视频不能正常打开则返回
		{
			MessageBox(_T("无法打开视频文件"));
			return;
		}
		keyframedectected = false;
		Mat frame;
		while (1)
		{
			cap >> frame;
			if (frame.empty())
				break;

			resize(frame, frame, Size(1280, 720));
			vector<Mat> roi;
			//提取车牌
			vector<Rect> rects = preprocess(frame, frame, roi);
			//窗口显示
			DrawcvMat(frame, IDC_VIDEO);
			if (!keyframedectected)
			{
				//roi是一个车牌切割数组
				for (int i = 0; i < roi.size(); i++)
					if ((rects[i].width > 125 && rects[i].y >= (frame.rows / 10) && rects[i].height > 30 && rects[i].y < (frame.rows * 5 / 6)) || keyframedectected)
					{
						//resize(roi, roi, Size(roi.cols * 2, roi.rows * 2), CV_INTER_CUBIC);
						Mat myLicense;
						roi[i].copyTo(myLicense);
						cvtColor(myLicense, myLicense, CV_BGR2GRAY);
#ifdef USE_OPENCV
						threshold(myLicense, myLicense, 0, 255, THRESH_OTSU);
#else
						binarization_otsu(myLicense, myLicense);
#endif
						
						cut_bounder(myLicense, myLicense);
						vector<Mat> words = verticalProjectionMat(myLicense);
						if (words.size() == 7)
						{
							keyframedectected = true;
							vector<Mat> disp;
							Mat ff;
							frame.copyTo(ff);
							disp.push_back(ff);
							disp.push_back(roi[i]);
							disp.push_back(words[0]);
							disp.push_back(words[1]);
							disp.push_back(words[2]);
							disp.push_back(words[3]);
							disp.push_back(words[4]);
							disp.push_back(words[5]);
							disp.push_back(words[6]);
							DisplayImages.push_back(disp);
						}
					}
				if (DisplayImages.size() > 0)
					Display(0);

				/*switch (DisplayImages.size())
				{
				case 5:
					GetDlgItem(IDC_RECO5)->ShowWindow(SW_SHOW);
				case 4:
					GetDlgItem(IDC_RECO4)->ShowWindow(SW_SHOW);
				case 3:
					GetDlgItem(IDC_RECO3)->ShowWindow(SW_SHOW);
				case 2:
					GetDlgItem(IDC_RECO2)->ShowWindow(SW_SHOW);
				case 1:
					GetDlgItem(IDC_RECO1)->ShowWindow(SW_SHOW);
				default:
					break;
				}*/
			}
			UpdateData(FALSE);
			waitKey(10);//每帧延时10毫秒
		}
		cap.release();//释放资源
	}
}

//打开一幅图像
void CLicensePlateRecognitionDlg::OnBnClickedOpenframe()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_HOGSVM))->GetCheck())
		tempmodel = 0;
	else if (((CButton*)GetDlgItem(IDC_LBPSVM))->GetCheck())
		tempmodel = 1;
	else
		tempmodel = 2;
	if ((foundmodel == 0 || foundmodel == 1) && tempmodel == 0)
	{
		MessageBox(_T("未找到HOG+SVM模型，请先训练模型！"));
		return;
	}
	else if ((foundmodel == 0 || foundmodel == 2) && tempmodel == 1)
	{
		return;
	}
	else if (((CButton*)GetDlgItem(IDC_TEMP))->GetCheck())
	{
		NumModelHOG.init(MatchTemplate);
		NumModelHOG.readmodel("model\\MatchTemplate\\num");
		ProvinceModelHOG.init(MatchTemplate);
		ProvinceModelHOG.readmodel("model\\MatchTemplate\\province");
	}
	DisplayImages.clear();
	CString str;
	CFileDialog dlg(TRUE, _T("*"), _T(""), OFN_OVERWRITEPROMPT, _T("Data File(*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		str = dlg.GetPathName();
		cv::Mat frame = cv::imread(str.GetBuffer());
		if (frame.empty())
			return;
		((CStatic*)GetDlgItem(IDC_KEY))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSE))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_PROVINCE))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM1))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM2))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM3))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM4))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM5))->SetBitmap(NULL);
		((CStatic*)GetDlgItem(IDC_LICENSENUM6))->SetBitmap(NULL);
		this->RedrawWindow();
		m_result = "";
		vector<Mat> roi;
		Mat license;
		myLPR(frame, license);
		DrawcvMat(frame, IDC_VIDEO);

		
		Mat clicense = license.clone();
		//切割字符
		vector<Mat> letters = verticalProjectionMat(clicense);
		//DrawcvMat(license, IDC_LICENSE);
		if (letters.size() == 7)
		{
			keyframedectected = true;
			vector<Mat> disp;
			disp.push_back(frame);
			disp.push_back(license);
			disp.push_back(letters[0]);
			disp.push_back(letters[1]);
			disp.push_back(letters[2]);
			disp.push_back(letters[3]);
			disp.push_back(letters[4]);
			disp.push_back(letters[5]);
			disp.push_back(letters[6]);
			DisplayImages.push_back(disp);
		}
		else if (letters.size() > 7)
		{
			keyframedectected = true;
			vector<Mat> disp;
			disp.push_back(frame);
			disp.push_back(license);
			disp.push_back(letters[1]);
			disp.push_back(letters[2]);
			disp.push_back(letters[3]);
			disp.push_back(letters[4]);
			disp.push_back(letters[5]);
			disp.push_back(letters[6]);
			disp.push_back(letters[7]);
			DisplayImages.push_back(disp);
		}

		if (DisplayImages.size() > 0)
			Display(0);

		UpdateData(FALSE);
	}
}

void CLicensePlateRecognitionDlg::OnBnClickedTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<string> filesNum;
	vector<string> filesProvince;
	vector<Mat> NumImages;
	vector<int> NumLabels;
	vector<Mat> ProvinceImages;
	vector<int> ProvinceLabels;
	filesearch("model\\num2", 2, filesNum);
	filesearch("model\\province2", 2, filesProvince);
	string label = "";
	int key = -1;
	for (int i = 0; i < filesNum.size(); i++)
	{
		int index = filesNum[i].find_last_of("\\");
		string labelname = filesNum[i].substr(index - 1, 1);
		if (labelname != label)
		{
			label = labelname;
			key++;
		}
		Mat img = imread(filesNum[i], -1);
		if (!img.empty())
		{
			NumImages.push_back(img);
			NumLabels.push_back(key);
		}
	}
	label = "";
	key = -1;
	for (int i = 0; i < filesProvince.size(); i++)
	{
		int index = filesProvince[i].find_last_of("\\");
		string labelname = filesProvince[i].substr(index - 2, 2);
		if (labelname != label)
		{
			label = labelname;
			key++;
		}
		Mat img = imread(filesProvince[i], -1);
		if (!img.empty())
		{
			ProvinceImages.push_back(img);
			ProvinceLabels.push_back(key);
		}
	}

	if (((CButton*)GetDlgItem(IDC_HOGSVM))->GetCheck())
	{
		NumModelHOG.init(HOG);
		NumModelHOG.train(NumImages, NumLabels);
		NumModelHOG.savemodel("nummodel-hog.xml");
		ProvinceModelHOG.init(HOG);
		ProvinceModelHOG.train(ProvinceImages, ProvinceLabels);
		ProvinceModelHOG.savemodel("provincemodel-hog.xml");
		foundmodel += 2;
	}
	else if (((CButton*)GetDlgItem(IDC_LBPSVM))->GetCheck())
	{
		NumModelLBP.init(LBP);
		NumModelLBP.train(NumImages, NumLabels);
		NumModelLBP.savemodel("nummodel-lbp.xml");
		ProvinceModelLBP.init(LBP);
		ProvinceModelLBP.train(ProvinceImages, ProvinceLabels);
		ProvinceModelLBP.savemodel("provincemodel-lbp.xml");
		foundmodel += 1;
	}
}

void CLicensePlateRecognitionDlg::OnBnClickedReco1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DisplayImages.size() > 0)
		Display(0);
}

void CLicensePlateRecognitionDlg::OnBnClickedReco2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DisplayImages.size() > 1)
		Display(1);
}

void CLicensePlateRecognitionDlg::OnBnClickedReco3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DisplayImages.size() > 2)
		Display(2);
}

void CLicensePlateRecognitionDlg::OnBnClickedReco4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DisplayImages.size() > 3)
		Display(3);
}

void CLicensePlateRecognitionDlg::OnBnClickedReco5()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DisplayImages.size() > 4)
		Display(4);
}

void CLicensePlateRecognitionDlg::OnBnClickedLbpsvm()
{
	// TODO: 在此添加控件通知处理程序代码
}
