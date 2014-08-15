
// LineAdjustDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LineAdjust.h"
#include "LineAdjustDlg.h"
#include "afxdialogex.h"
#include "CameraThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLineAdjustDlg �Ի���


int g_PicIDs[MAX_CAMERA_NUM] = {IDC_CAMERA1, IDC_CAMERA2, IDC_CAMERA3};

CLineAdjustDlg::CLineAdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineAdjustDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < CAMERA_NUM; i++) {
		m_pCamera[i] = NULL;
		m_pCameraThread[i] = NULL;
		m_iCameraPic[i] = g_PicIDs[i];
	}
}

void CLineAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLineAdjustDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLineAdjustDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLineAdjustDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLineAdjustDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CLineAdjustDlg ��Ϣ�������

BOOL CLineAdjustDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLineAdjustDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLineAdjustDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLineAdjustDlg::drawToDC(IplImage* image, int index)
{
	int ID = m_iCameraPic[index];
	CDC* pDC = GetDlgItem(ID)->GetDC();
    HDC pHdc = pDC->GetSafeHdc();

    CRect rect;
    GetDlgItem(ID)->GetClientRect(&rect);

    CvvImage cimg;
    cimg.CopyOf(image);
    cimg.DrawToHDC(pHdc, rect);

    ReleaseDC(pDC);
}

void CLineAdjustDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (m_pCamera[0] == NULL) {
		for (int i = 0; i < CAMERA_NUM; i++) {
			m_pCamera[i] = cvCaptureFromCAM(i);
			if (!m_pCamera[i]) {
				// Open Camera Error!
				for (int j = 0; j < i; j++) {
					cvReleaseCapture(&m_pCamera[j]);
					m_pCamera[j] = NULL;
				}
				MessageBox(_T("Camera Open Error!"), _T("Error"), MB_ICONERROR|MB_OK);
				return;
			}
		}

		m_pMotorCtrl = new CMotorCtrl();

		for (int i = 0; i < CAMERA_NUM; i++) {
			m_pCameraThread[i] = new CCameraThread(this, m_pCamera[i], m_pMotorCtrl, i);
			m_pCameraThread[i]->PostMsg(NULL, 0);
			// Enable the self-refresh mode.
			m_pCameraThread[i]->SetSelfRefresh(TRUE);
		}
	}
}

void CLineAdjustDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (m_pCamera[0] != NULL) {
		m_pMotorCtrl->StopThread();
		for (int i = 0; i < CAMERA_NUM; i++) {
			m_pCameraThread[i]->SetSelfRefresh(FALSE);
			m_pCameraThread[i]->StopThread();
			cvReleaseCapture(&m_pCamera[i]);
			m_pCamera[i] = NULL;
			delete m_pCameraThread[i];
		}
		delete m_pMotorCtrl;
	}
}

void CLineAdjustDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedButton2();
	if (MessageBox( _T("Exit?"), _T(""), MB_YESNO | MB_ICONQUESTION ) == IDYES) {
		PostMessage(WM_CLOSE);
	}
}
