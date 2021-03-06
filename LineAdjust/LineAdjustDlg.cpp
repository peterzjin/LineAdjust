
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
int g_OrderIDs[MAX_CAMERA_NUM] = {IDC_COMBO1, IDC_COMBO2, IDC_COMBO3};
double g_dScale[MAX_SCALE_NUM] = {0, 2, 5, 10, 20, 40, 80, 100};
double g_dStep[MAX_SCALE_NUM] = {0, 8, 20, 40, 80, 160, 320, 400};
int g_iValidNum = 8;

CLineAdjustDlg::CLineAdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineAdjustDlg::IDD, pParent)
	, m_Motor1_pulse(_T("1600"))
	, m_motor3_pulseNum(_T("1600"))
	, m_motor2_pulseNum(_T("1600"))
	, m_iAdjPhase(-1)
	, m_uDataCnt(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < CAMERA_NUM; i++) {
		m_iMap[i] = m_iRMap[i] = i;
		m_pCamera[i] = NULL;
		m_pCameraThread[i] = NULL;
		m_iCameraPic[i] = g_PicIDs[i];
	}
	for (int i = 0; i < MOTOR_NUM; i++) {
		m_iMotorMap[i] = i;
		for (int j = 0; j < MAX_SCALE_NUM; j++) {
			m_dScale[i][j] = g_dScale[j];
			m_dStep[i][j] = g_dStep[j];
		}
	}
	m_iValidNum = g_iValidNum;
	m_pUartCommon = &(theApp.m_cUartCommon);
	InitializeCriticalSection(&m_Lock);
}

void CLineAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < CAMERA_NUM; i++) {
		DDX_Control(pDX, g_OrderIDs[i], m_cOrder[i]);
		m_cOrder[i].SetCurSel(i);
	}
	DDX_Text(pDX, IDC_EDIT_MOTOR_1_PULSE, m_Motor1_pulse);
	DDX_Text(pDX, IDC_EDIT_MOTOR3_PULSE, m_motor3_pulseNum);
	DDX_Text(pDX, IDC_EDIT_MOTOR2_PULSE, m_motor2_pulseNum);
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
	ON_BN_CLICKED(IDC_BUTTON6, &CLineAdjustDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR3_CLOCK, &CLineAdjustDlg::OnBnClickedButtonMotor3Clock)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR3_ANTICLOCK, &CLineAdjustDlg::OnBnClickedButtonMotor3Anticlock)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR2_CLOCK, &CLineAdjustDlg::OnBnClickedButtonMotor2Clock)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR2_ANTICLOCK, &CLineAdjustDlg::OnBnClickedButtonMotor2Anticlock)
	ON_BN_CLICKED(IDC_BUTTON7, &CLineAdjustDlg::OnBnClickedButton7)
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
	int ID = m_iCameraPic[m_iMap[index]];
	ID = m_iCameraPic[m_iRMap[index]];
	ID = m_iCameraPic[m_iMap[index]];
	CDC* pDC = GetDlgItem(ID)->GetDC();
    HDC pHdc = pDC->GetSafeHdc();

    CRect rect;
    GetDlgItem(ID)->GetClientRect(&rect);

    CvvImage cimg;
    cimg.CopyOf(image);
    cimg.DrawToHDC(pHdc, rect);

    ReleaseDC(pDC);
}

void CLineAdjustDlg::LoadConfig()
{
	TCHAR path[256], value[128], key[32], *token;
	double l_dScale[MOTOR_NUM][MAX_SCALE_NUM];
	double l_dStep[MOTOR_NUM][MAX_SCALE_NUM];
	int l_iValidNum, i, j;

	GetCurrentDirectory(256, path);
	sprintf_s(path, "%s//config.ini", path);

	if (!PathFileExists(path))
		goto error;

	for (i = 0; i < MOTOR_NUM; i++) {
		j = 0;
		sprintf_s(key, "SCALE_%d", i + 1);
		GetPrivateProfileString("CONFIG", key, "\0", value, 128, path);
		token = strtok(value, ",");
		while(token != NULL) {
			l_dScale[i][j++] = _tstof(token);
			if (l_dScale[i][j - 1] > MAX_SCALE_VALUE || j >= MAX_SCALE_NUM - 2) goto error;
			token = strtok(NULL, ",");
		}
		if (i == 0)
			l_iValidNum = j;
		else if (j != l_iValidNum)
			goto error;

		j = 0;
		sprintf_s(key, "STEP_%d", i + 1);
		GetPrivateProfileString("CONFIG", key, "\0", value, 128, path);
		token = strtok(value, ",");
		while(token != NULL) {
			l_dStep[i][j++] = _tstof(token);
			if (j >= MAX_SCALE_NUM - 2) goto error;
			token = strtok(NULL, ",");
		}
		if (j != l_iValidNum)
			goto error;
	}

	for (i = 0; i < MOTOR_NUM; i++) {
		m_dScale[i][0] = m_dStep[i][0] = 0;
		for (j = 0; j < l_iValidNum; j++) {
			m_dScale[i][j + 1] = l_dScale[i][j];
			m_dStep[i][j + 1] = l_dStep[i][j];
		}
		m_dScale[i][j + 1] = MAX_SCALE_VALUE;
		m_dStep[i][j + 1] = (m_dStep[i][j] / m_dScale[i][j]) * MAX_SCALE_VALUE;
		m_iValidNum = l_iValidNum + 2;
	}

	return;

error:
	MessageBox(_T("Config Read Error, Use Default!"), _T("Warning"), MB_ICONERROR|MB_OK);
	return;
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

	LoadConfig();
}

void CLineAdjustDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_pCamera[0] != NULL) {
		//m_pMotorCtrl->StopThread();
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
	UpdateData(TRUE);	
	unsigned int pluseNum = atoi(m_Motor1_pulse.GetString());	
	theApp.m_StepMotor[0]->RunForward(pluseNum);
}


void CLineAdjustDlg::OnBnClickedButtonAntiClockMain()
{
	UpdateData(TRUE);
	unsigned int pluseNum = atoi(m_Motor1_pulse.GetString());	
	theApp.m_StepMotor[0]->RunRollback(pluseNum);
}

void CLineAdjustDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	int iIndex[CAMERA_NUM], iMap[CAMERA_NUM];
	bool bErr = false;

	if (m_pCamera[0] == NULL)
		return;

	for (int i = 0; i < CAMERA_NUM; i++) {
		iMap[m_iMap[i]] = i;
		iIndex[i] = m_cOrder[i].GetCurSel();
	}

	for (int i = 0; i < CAMERA_NUM; i++) {
		for (int j = i + 1; j < CAMERA_NUM; j++) {
			if (iIndex[i] == iIndex[j]) {
				bErr = true;
				break;
			}
		}
	}

	if (bErr) {
		MessageBox( _T("Index Select Error"), _T(""), 0);
	} else {
		for (int i = 0; i < CAMERA_NUM; i++) {
			m_iMap[iMap[i]] = iIndex[i];
			m_cOrder[i].SetCurSel(i);
			m_iRMap[iIndex[i]] = iMap[i];
		}
	}
}


void CLineAdjustDlg::OnBnClickedButtonMotor3Clock()
{	
	UpdateData(TRUE);	
	unsigned int pluseNum = atoi(m_motor3_pulseNum.GetString());	
	theApp.m_StepMotor[2]->RunForward(pluseNum);
}


void CLineAdjustDlg::OnBnClickedButtonMotor3Anticlock()
{
	UpdateData(TRUE);
	unsigned int pluseNum = atoi(m_motor3_pulseNum.GetString());	
	theApp.m_StepMotor[2]->RunRollback(pluseNum);
}


void CLineAdjustDlg::OnBnClickedButtonMotor2Clock()
{
		UpdateData(TRUE);	
	unsigned int pluseNum = atoi(m_motor2_pulseNum.GetString());	
	theApp.m_StepMotor[1]->RunForward(pluseNum);
}


void CLineAdjustDlg::OnBnClickedButtonMotor2Anticlock()
{
		UpdateData(TRUE);
	unsigned int pluseNum = atoi(m_motor2_pulseNum.GetString());	
	theApp.m_StepMotor[1]->RunRollback(pluseNum);
}


void CLineAdjustDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButton1();

	if (m_pCamera[0] != NULL) {
		m_pCameraThread[m_iRMap[0]]->StopAdjust();
		m_pCameraThread[m_iRMap[1]]->StartAdjust();
		m_pCameraThread[m_iRMap[2]]->StopAdjust();
		m_iAdjPhase = 1;
	}
}

void CLineAdjustDlg::NextPhase()
{
	switch (m_iAdjPhase) {
	case 1:
		m_pCameraThread[m_iRMap[0]]->StartAdjust();
		m_pCameraThread[m_iRMap[1]]->StopAdjust();
		m_pCameraThread[m_iRMap[2]]->StartAdjust();
		m_iAdjPhase = 2;
		break;
	case 2:
		m_pCameraThread[m_iRMap[0]]->StartAdjust();
		m_pCameraThread[m_iRMap[1]]->StopAdjust();
		m_pCameraThread[m_iRMap[2]]->StopAdjust();
		m_iAdjPhase = 3;
		break;
	case 3:
		m_pCameraThread[m_iRMap[0]]->StopAdjust();
		m_pCameraThread[m_iRMap[1]]->StopAdjust();
		m_pCameraThread[m_iRMap[2]]->StopAdjust();
		m_iAdjPhase = 4;
		break;
	default:
		m_pCameraThread[m_iRMap[0]]->StopAdjust();
		m_pCameraThread[m_iRMap[1]]->StartAdjust();
		m_pCameraThread[m_iRMap[2]]->StopAdjust();
		m_iAdjPhase = 1;
		return;
	}
}

bool CLineAdjustDlg::GetPhase2Data(int idx, double scale, double *ret_scale)
{
	bool ret = false;

	EnterCriticalSection(&m_Lock);

	switch (m_uDataCnt) {
	case 0:
		if (m_iMap[idx] == 0)
			m_uDataCnt = 1;
		else if (m_iMap[idx] == 0)
			m_uDataCnt = 2;

		m_dTmpScale = scale;
		break;
	case 1:
		if (m_iMap[idx] == 3) {
			m_dTmpScale = 0;
			*ret_scale = (m_dTmpScale - scale) / 2;
			ret = true;
		}
		break;
	case 2:
		if (m_iMap[idx] == 0) {
			m_dTmpScale = 0;
			*ret_scale = (scale - m_dTmpScale) / 2;
			ret = true;
		}
		break;
	default:
		break;
	}

	LeaveCriticalSection(&m_Lock);

	return ret;
}

void CLineAdjustDlg::MotorRun(int idx, unsigned int step, bool forward) {
	if (step == 0) return;

	forward ? theApp.m_StepMotor[m_iMotorMap[idx]]->RunForward(step) : theApp.m_StepMotor[m_iMotorMap[idx]]->RunRollback(step);
}