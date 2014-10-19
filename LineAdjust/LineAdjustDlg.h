
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

#define MOTOR_NUM 3

#define MAX_SCALE_NUM 10

#define MAX_SCALE_VALUE 100

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
	int m_iRMap[CAMERA_NUM];
	int m_iMotorMap[MOTOR_NUM];
	unsigned char m_uDataCnt;
	double m_dTmpScale;
	CRITICAL_SECTION m_Lock;
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
	CString m_Motor1_pulse;
	CString m_motor3_pulseNum;
	afx_msg void OnBnClickedButtonMotor3Clock();
	afx_msg void OnBnClickedButtonMotor3Anticlock();
	CString m_motor2_pulseNum;
	afx_msg void OnBnClickedButtonMotor2Clock();
	afx_msg void OnBnClickedButtonMotor2Anticlock();
	afx_msg void OnBnClickedButton7();

	int m_iAdjPhase;
	double m_dScale[MOTOR_NUM][MAX_SCALE_NUM];
	double m_dStep[MOTOR_NUM][MAX_SCALE_NUM];
	int m_iValidNum;
	void NextPhase(void);
	bool GetPhase2Data(int idx, double scale, double *ret_scale);
	void MotorRun(int idx, unsigned int step, bool forward);
	void LoadConfig();
};
