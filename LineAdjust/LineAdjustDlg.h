
// LineAdjustDlg.h : ͷ�ļ�
//

#pragma once

#include "opencv2/opencv.hpp"
#include "CameraThread.h"
#include "CvvImage.h"
#include "motorctrl.h"
#include "UartCommon.h"
#include "afxwin.h"

#define MAX_CAMERA_NUM 3

#define CAMERA_NUM 3

// CLineAdjustDlg �Ի���
class CLineAdjustDlg : public CDialogEx
{
// ����
public:
	CLineAdjustDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LINEADJUST_DIALOG };
	HMENU m_hMenuMain;
	CStatusBarCtrl m_StatusBar;
	CUartCommon * m_pUartCommon;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CvCapture *m_pCamera[CAMERA_NUM];
	CCameraThread *m_pCameraThread[CAMERA_NUM];
	CMotorCtrl *m_pMotorCtrl;
	int m_iCameraPic[CAMERA_NUM];
	int m_iMap[CAMERA_NUM];
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	void drawToDC(IplImage* image, int ID);
	
	afx_msg void OnBnClickedButton3();
	afx_msg void OnUartSetMenu();
	afx_msg void OnMenuMotorControl();
	afx_msg void OnBnClickedButtonClockMain();
	afx_msg void OnBnClickedButtonAntiClockMain();
	afx_msg void OnBnClickedButton6();
	CComboBox m_cOrder[MAX_CAMERA_NUM];
};
