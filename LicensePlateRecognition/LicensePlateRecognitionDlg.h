
// LicensePlateRecognitionDlg.h: 头文件
//

#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <map>
#include <iostream>
#include "io.h"
#include "PlateRecognizer.h"
#include  <stdio.h>
#include  <stdlib.h>

// CLicensePlateRecognitionDlg 对话框
class CLicensePlateRecognitionDlg : public CDialogEx
{
	// 构造
public:
	CLicensePlateRecognitionDlg(CWnd* pParent = nullptr);	// 标准构造函数

															// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LICENSEPLATERECOGNITION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void DrawcvMat(cv::Mat m_cvImg, UINT ID);

	// 实现
protected:
	HICON m_hIcon;
	int foundmodel = 0;
	int tempmodel = -1;
	vector<vector<Mat>> DisplayImages;
	map<int, string> NumMap, ProvinceMap;
	PlateRecognizer NumModelHOG, ProvinceModelHOG, NumModelLBP, ProvinceModelLBP;
	void Display(int num);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CFont f;
	bool keyframedectected = false;
	afx_msg void OnBnClickedOpenvideo();
	afx_msg void OnBnClickedOpenframe();
	afx_msg void OnBnClickedTrain();
	CString m_result;
	afx_msg void OnBnClickedReco1();
	afx_msg void OnBnClickedReco2();
	afx_msg void OnBnClickedReco3();
	afx_msg void OnBnClickedReco4();
	afx_msg void OnBnClickedReco5();
	afx_msg void OnBnClickedLbpsvm();
	
	CString m_value;
};
