#include "StdAfx.h"
#include "LineAdjust.h"
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
	, m_bAdjust(false)
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

#define STEP_SCALE 0.9

unsigned int CCameraThread::GetStep(int idx, double scale)
{
	int i;
	double step;
	for (i = 0; i < m_cLineAdj->m_iValidNum; i++)
		if (scale < m_cLineAdj->m_dScale[idx][i]) break;

	if (i == m_cLineAdj->m_iValidNum) return 0;

	step = ((m_cLineAdj->m_dStep[idx][i] - m_cLineAdj->m_dStep[idx][i - 1]) / (m_cLineAdj->m_dScale[idx][i] - m_cLineAdj->m_dScale[idx][i - 1])) * (scale - m_cLineAdj->m_dScale[idx][i - 1]) + m_cLineAdj->m_dStep[idx][i - 1];

	return (unsigned int)(step * STEP_SCALE);
}

void CCameraThread::AdjustMotor(double parm_scale)
{
	bool forward = true;
	unsigned int step = 0, idx;
	double scale = parm_scale;
	idx = m_cLineAdj->m_iAdjPhase - 1;

	switch (m_cLineAdj->m_iAdjPhase) {
	case 2:
		if (!m_cLineAdj->GetPhase2Data(idx, parm_scale, &scale))
			break;
	case 1:
	case 3:
		if (scale < 0) {
			forward = false;
			scale = -1 * scale;
		}
		if (scale < 1)
			m_cLineAdj->NextPhase();
		else
			step = GetStep(idx, scale);

		break;
	default:
		m_cLineAdj->m_iAdjPhase = 1;
		return;
	}

	m_cLineAdj->MotorRun(idx, step, forward);
}

void CCameraThread::ThreadFunc(void *pMsg)
{
	int calculateOffset(IplImage *, IplImage *, double *);
	IplImage *srcImage = NULL, *dstImage;
	double scale;

	srcImage = cvQueryFrame(m_pCamera);
	if (!srcImage) return;
	dstImage = cvCloneImage(srcImage);

	if (m_bAdjust) {
		int res = calculateOffset(srcImage, dstImage, &scale);
		if(res == 0){
			AdjustMotor(scale);
		}
	}

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
		m_pTimer = SetTimer(NULL, 0, 500, SelfRefresh);
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

void CCameraThread::StartAdjust(void)
{
	m_bAdjust = true;
}

void CCameraThread::StopAdjust(void)
{
	m_bAdjust = false;
}
