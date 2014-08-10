#include "StdAfx.h"
#include "CameraThread.h"
#include "resource.h"
#include "lineadjustdlg.h"

CCameraThread::CCameraThread(CLineAdjustDlg *cLineAdj, CvCapture *pCamera, CMotorCtrl *pMotorCtrl, int index)
	: m_bSelfRefresh(false)
	, m_pTimer(0)
{
	m_cLineAdj = cLineAdj;
	m_pCamera = pCamera;
	m_iIndex = index;
	m_pMotorCtrl = pMotorCtrl;
}


CCameraThread::~CCameraThread(void)
{
}


void CCameraThread::ThreadFunc(void *pMsg)
{
	int calculateOffset(const IplImage *, IplImage *, double *);
	IplImage *srcImage, *dstImage;
	double scale;

	srcImage = cvQueryFrame(m_pCamera);
	dstImage = cvCloneImage(srcImage);
	calculateOffset(srcImage, dstImage, &scale);
	if (m_pMotorCtrl) m_pMotorCtrl->PostMsg(NULL, 0);
	m_cLineAdj->drawToDC(dstImage, m_iIndex, TRUE);
	cvReleaseImage(&dstImage);
}

CCameraThread *g_pCameraThread;

void CCameraThread::SetSelfRefresh(bool bSelfRefresh)
{
	m_bSelfRefresh = bSelfRefresh;
	g_pCameraThread = this;

	if (m_bSelfRefresh && m_pTimer == 0) {
		m_pTimer = SetTimer(NULL, 0, 500, SelfRefresh);
	}

	if (!m_bSelfRefresh && m_pTimer != 0) {
		KillTimer(NULL, m_pTimer);
		m_pTimer = 0;
	}
}

void CALLBACK CCameraThread::SelfRefresh(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	g_pCameraThread->PostMsg(NULL, 0);
}
