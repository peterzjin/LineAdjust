#include "StdAfx.h"
#include "CameraThread.h"
#include "resource.h"
#include "lineadjustdlg.h"

//#define FRAME_MAX_WIDTH 1280
//#define FRAME_MAX_HEIGHT 720
#define FRAME_MAX_WIDTH 800
#define FRAME_MAX_HEIGHT 600

CCameraThread::CCameraThread(CLineAdjustDlg *cLineAdj, CvCapture *pCamera, CMotorCtrl *pMotorCtrl, int index)
	: m_bSelfRefresh(false)
	, m_pTimer(0)
{
	m_cLineAdj = cLineAdj;
	m_pCamera = pCamera;
	m_iIndex = index;
	m_pMotorCtrl = pMotorCtrl;
	cvSetCaptureProperty(m_pCamera, CV_CAP_PROP_FRAME_WIDTH, FRAME_MAX_WIDTH);
	cvSetCaptureProperty(m_pCamera, CV_CAP_PROP_FRAME_HEIGHT, FRAME_MAX_HEIGHT);
}


CCameraThread::~CCameraThread(void)
{
}


void CCameraThread::ThreadFunc(void *pMsg)
{
	int calculateOffset(IplImage *, IplImage *, double *);
	IplImage *srcImage = NULL, *dstImage;
	double scale;

	srcImage = cvQueryFrame(m_pCamera);
	if (!srcImage) return;
	dstImage = cvCloneImage(srcImage);
	/*
	int res = calculateOffset(srcImage, dstImage, &scale);
	if(res == 0){
//		if (m_pMotorCtrl) m_pMotorCtrl->PostMsg(NULL, 0);
	}
	*/
	m_cLineAdj->drawToDC(dstImage, m_iIndex);
	cvReleaseImage(&dstImage);

	/*
	double x = cvGetCaptureProperty(m_pCamera, CV_CAP_PROP_FPS);
	double y = cvGetCaptureProperty(m_pCamera, CV_CAP_PROP_FRAME_WIDTH);
	double z = cvGetCaptureProperty(m_pCamera, CV_CAP_PROP_FRAME_HEIGHT);
	int w = srcImage->width;
	int h = srcImage->height;
	*/
}

CCameraThread *g_pCameraThread[CAMERA_NUM];
UINT_PTR g_TimerID[CAMERA_NUM];

void CCameraThread::SetSelfRefresh(bool bSelfRefresh)
{
	m_bSelfRefresh = bSelfRefresh;

	if (m_bSelfRefresh && m_pTimer == 0) {
		m_pTimer = SetTimer(NULL, 0, 100, SelfRefresh);
		g_pCameraThread[m_iIndex] = this;
		g_TimerID[m_iIndex] = m_pTimer;
	}

	if (!m_bSelfRefresh && m_pTimer != 0) {
		KillTimer(NULL, m_pTimer);
		m_pTimer = 0;
		g_pCameraThread[m_iIndex] = NULL;
		g_TimerID[m_iIndex] = 0;
	}
}

void CALLBACK CCameraThread::SelfRefresh(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	for (int i = 0; i < CAMERA_NUM; i++) {
		if (g_TimerID[i] == nTimerid && g_pCameraThread[i]) {
			g_pCameraThread[i]->PostMsg(NULL, 0);
		}
	}
}
