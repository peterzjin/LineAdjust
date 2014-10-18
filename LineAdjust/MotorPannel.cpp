// MotorPannel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LineAdjust.h"
#include "MotorPannel.h"
#include "afxdialogex.h"
#include "MotorCtrl.h"


// CMotorPannel �Ի���

IMPLEMENT_DYNAMIC(CMotorPannel, CDialogEx)

CMotorPannel::CMotorPannel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotorPannel::IDD, pParent)
	, m_clockDirect(0)
	, m_antiClockDirect(false)
	, m_pluseNum(_T("1600"))
	, m_strMotorNum(_T("1"))
	, m_bClockRotate(FALSE)
	, m_bLedOn(FALSE)
	, m_strMotorSpeed(_T("200"))
{
	m_pUartCommon = &(theApp.m_cUartCommon);	

}

CMotorPannel::~CMotorPannel()
{
}


void CMotorPannel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MOTOR_NUM, m_motorNum);
	DDX_Control(pDX, IDC_COMBO1, m_motorControlType);
	DDX_Text(pDX, IDC_EDIT2, m_pluseNum);
	DDX_Text(pDX, IDC_EDIT_MOTOR_NUM, m_strMotorNum);
	DDX_Radio(pDX, IDC_RADIO1, m_bClockRotate);
	DDX_Radio(pDX, IDC_RADIO3, m_bLedOn);
	DDX_Text(pDX, IDC_EDIT3, m_strMotorSpeed);
	DDX_Control(pDX, IDC_SLIDER1, m_speedSlider);
}


BEGIN_MESSAGE_MAP(CMotorPannel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMotorPannel::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CLOCK_ROTATE, &CMotorPannel::OnBnClickedButtonClockRotate)
	ON_BN_CLICKED(IDC_BUTTON_ANTI_CLOCK_ROTATE3, &CMotorPannel::OnBnClickedButtonAntiClockRotate3)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR_STOP, &CMotorPannel::OnBnClickedButtonMotorStop)
	ON_BN_CLICKED(IDC_BUTTON_CLOCK_ROTATE2, &CMotorPannel::OnBnClickedButtonClockRotate2)
	ON_BN_CLICKED(IDC_RADIO3, &CMotorPannel::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CMotorPannel::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CMotorPannel::OnBnClickedButtonSet)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMotorPannel::OnNMCustomdrawSlider1)
	ON_EN_CHANGE(IDC_EDIT3, &CMotorPannel::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CMotorPannel ��Ϣ�������


void CMotorPannel::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


BOOL CMotorPannel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_motorNum.SetWindowTextA(_T("1"));

	m_motorControlType.InsertString(0, _T("400"));
	m_motorControlType.InsertString(1, _T("800"));
	m_motorControlType.InsertString(2, _T("1600"));
	m_motorControlType.InsertString(3, _T("3200"));

	m_motorControlType.SetCurSel(1);
	m_speedSlider.SetRangeMin(0);
	m_speedSlider.SetRangeMax(2000);
	m_speedSlider.SetPos(atoi(m_strMotorSpeed.GetString()));

	return TRUE;  // return TRUE unless you set the focus to a control	
}


void CMotorPannel::OnBnClickedButtonClockRotate()
{		
	UpdateData(TRUE);

	BYTE motorNum = atoi(m_strMotorNum.GetString());

	//theApp.m_MotorCtrl.RunClockContinue(motorNum);
	theApp.m_pMotorController->RunClockContinue(motorNum);
}


void CMotorPannel::OnBnClickedButtonAntiClockRotate3()
{
	UpdateData(TRUE);

	BYTE motorNum = atoi(m_strMotorNum.GetString());

	//theApp.m_MotorCtrl.RunAntiClockContinue(motorNum);
	theApp.m_pMotorController->RunAntiClockContinue(motorNum);
}


void CMotorPannel::OnBnClickedButtonMotorStop()
{
	UpdateData(TRUE);

	BYTE motorNum = atoi(m_strMotorNum.GetString());

	//theApp.m_MotorCtrl.StopMotor(motorNum);
	theApp.m_pMotorController->StopMotor(motorNum);
}


void CMotorPannel::OnBnClickedButtonClockRotate2()
{
	UpdateData(TRUE);
	BYTE motorNum = atoi(m_strMotorNum.GetString());
	//theApp.m_MotorCtrl.RunMotor(motorNum);
	theApp.m_pMotorController->RunMotor(motorNum);
}

//Led On
void CMotorPannel::OnBnClickedRadio3()
{
	UpdateData(TRUE);
	BYTE motorNum = atoi(m_strMotorNum.GetString());
	//theApp.m_MotorCtrl.OpenLed(motorNum);	
	theApp.m_pMotorController->OpenLed(motorNum);	
}


void CMotorPannel::OnBnClickedRadio4()
{
	UpdateData(TRUE);
	BYTE motorNum = atoi(m_strMotorNum.GetString());
//	theApp.m_MotorCtrl.CloseLed(motorNum);	
	theApp.m_pMotorController->CloseLed(motorNum);	
}


void CMotorPannel::OnBnClickedButtonSet()
{
	UpdateData(TRUE);
	BYTE motorNum = atoi(m_strMotorNum.GetString());

	unsigned int pluseNum = atoi(m_pluseNum.GetString());	
	//theApp.m_MotorCtrl.SetMotorPulseNumber(motorNum, pluseNum);	
	theApp.m_pMotorController->SetMotorPulseNumber(motorNum, pluseNum);	
	Sleep(200);

	//theApp.m_MotorCtrl.SetMotorDirectionFrequence(motorNum, (!m_bClockRotate), 50);
	theApp.m_pMotorController->SetMotorDirectionFrequence(motorNum, (!m_bClockRotate), 50);
	Sleep(200);

	unsigned int motorSpeed = atoi(m_strMotorSpeed.GetString());	
	//theApp.m_MotorCtrl.SetMotorAccerateAndSpeed(motorNum, 50, motorSpeed);
	theApp.m_pMotorController->SetMotorAccerateAndSpeed(motorNum, 50, motorSpeed);
	Sleep(100);	
}


void CMotorPannel::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	m_strMotorSpeed.Format(_T("%d"), this->m_speedSlider.GetPos());
	UpdateData(FALSE);
}


void CMotorPannel::OnEnChangeEdit3()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int speed = atoi(this->m_strMotorSpeed.GetString());
	if(speed < 0 || speed > 2000)
	{
		::AfxMessageBox("Max speed is 2000!\r\n");
		return;
	}
	m_speedSlider.SetPos(speed);
	UpdateData(FALSE);
}
