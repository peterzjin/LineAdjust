#pragma once
#include "opencv2/opencv.hpp"
//#include "lineadjustdlg.h"
#include "threadloop.h"
#include "motorctrl.h"

class CLineAdjustDlg;

class CCameraThread :
	public CThreadLoop
{
public:
	CCameraThread(CLineAdjustDlg* cLineAdj, CvCapture *pCamera, CMotorCtrl *pMotorCtrl, int index);
	~CCameraThread(void);
	void ThreadFunc(void *pMsg);
private:
	int m_iIndex;
	CvCapture *m_pCamera;
	CLineAdjustDlg* m_cLineAdj;
	CMotorCtrl *m_pMotorCtrl;
public:
	void SetSelfRefresh(bool bSelfRefresh);
private:
	bool m_bSelfRefresh;
	UINT_PTR m_pTimer;
	static void CALLBACK SelfRefresh(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);
};

