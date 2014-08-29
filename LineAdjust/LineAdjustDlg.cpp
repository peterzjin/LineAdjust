
// LineAdjustDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LineAdjust.h"
#include "LineAdjustDlg.h"
#include "afxdialogex.h"
#include "CameraThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLineAdjustDlg 对话框


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
	m_pUartCommon = &(theApp.m_cUartCommon);
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
	ON_COMMAND(ID_UART_SET, &CLineAdjustDlg::OnUartSetMenu)
	ON_COMMAND(ID_MENU_MOTOR_CONTROL, &CLineAdjustDlg::OnMenuMotorControl)
	ON_BN_CLICKED(IDC_BUTTON_CLOCK_MAIN, &CLineAdjustDlg::OnBnClickedButtonClockMain)
	ON_BN_CLICKED(IDC_BUTTON_ANTI_CLOCK_MAIN, &CLineAdjustDlg::OnBnClickedButtonAntiClockMain)
END_MESSAGE_MAP()


// CLineAdjustDlg 消息处理程序

BOOL CLineAdjustDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_hMenuMain=LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU_MAIN));//导入资源,创建菜单
	::SetMenu(this->GetSafeHwnd(),m_hMenuMain);//添加到对话框

	//Start the Status Bar
	m_StatusBar.Create(WS_CHILD|WS_VISIBLE|SBT_OWNERDRAW,CRect(0,0,0,0),this,0);

	 RECT CompRect;
  
    this->GetWindowRect(&CompRect);   
	int width = CompRect.right - CompRect.left;
	int strPartDim[3]={width/2,-1};//分割数量
	//分割的大小是这样决定的，如上面分的3个窗口
	//第一大小为100，第二个也为100（即200-100），第三个是状态栏的整个大小减去200.
	m_StatusBar.SetParts(2,strPartDim); //设置状态栏文本?
	m_StatusBar.SetText(_T("激光调教系统"),0,0);
	m_StatusBar.SetText(_T("状态"),1,0); //下面是在状态栏中加入图标
	//m_StatusBar.SetIcon(1, SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_TE),FALSE));//为第二个分栏中加的图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLineAdjustDlg::OnPaint()
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

	#if 1
	if(m_pUartCommon->GetStatus())
		m_StatusBar.SetText("Status: COM ON",1,0); //下面是在状态栏中加入图标
	else
		m_StatusBar.SetText("Status: COM OFF",1,0); //下面是在状态栏中加入图标
#endif
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	// TODO: 在此添加控件通知处理程序代码

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
	// TODO: 在此添加控件通知处理程序代码

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
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButton2();
	if (MessageBox( _T("Exit?"), _T(""), MB_YESNO | MB_ICONQUESTION ) == IDYES) {
		PostMessage(WM_CLOSE);
	}
}


void CLineAdjustDlg::OnUartSetMenu()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_dlgComSetting.DoModal();
	
	if(m_pUartCommon->GetStatus())
		m_StatusBar.SetText("Status: COM ON",1,0); //下面是在状态栏中加入图标
	else
		m_StatusBar.SetText("Status: COM OFF",1,0); //下面是在状态栏中加入图标
}


void CLineAdjustDlg::OnMenuMotorControl()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_dlgMotorPannel.DoModal();
}


void CLineAdjustDlg::OnBnClickedButtonClockMain()
{	
	theApp.m_StepMotor[0]->RunForward(1);
}


void CLineAdjustDlg::OnBnClickedButtonAntiClockMain()
{
	theApp.m_StepMotor[0]->RunRollback(1);
}
