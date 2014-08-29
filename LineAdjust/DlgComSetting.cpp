// DlgComSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "LineAdjust.h"
#include "DlgComSetting.h"
#include "afxdialogex.h"


// CDlgComSetting 对话框

IMPLEMENT_DYNAMIC(CDlgComSetting, CDialogEx)

CDlgComSetting::CDlgComSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgComSetting::IDD, pParent)
{
	m_pUartCommon = &(theApp.m_cUartCommon);
}

CDlgComSetting::~CDlgComSetting()
{
}

// CDlgNodeSetting 消息处理程序
BOOL CDlgComSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	FindCommPort(&m_comboComName);
	m_comboComBaud.Clear();
		
	m_comboComBaud.InsertString(0,_T("9600"));
	//m_comboComBaud.InsertString(1,_T("19200"));
	//m_comboComBaud.InsertString(2,_T("38400"));
	//m_comboComBaud.InsertString(3,_T("57600"));
	//m_comboComBaud.InsertString(4, _T("115200"));
	m_comboComBaud.SetCurSel(0);
	if(m_pUartCommon->GetStatus())
	{
		GetDlgItem(IDOK)->SetWindowText(_T("关闭"));
	}
	else
	{
		GetDlgItem(IDOK)->SetWindowText(_T("打开"));
	}
	//m_comStatus = FALSE;
	return TRUE;
}

void CDlgComSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM_NUM, m_comboComName);
	DDX_Control(pDX, IDC_COMBO_COM_BAUD, m_comboComBaud);
}


BEGIN_MESSAGE_MAP(CDlgComSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgComSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgComSetting::OnBnClickedCancel)
END_MESSAGE_MAP()

void  CDlgComSetting::FindCommPort(CComboBox   *pComboBox)
{
	HKEY   hKey;

	#ifdef   _DEBUG
	ASSERT(   pComboBox   !=   NULL   );
	pComboBox-> AssertValid();
	#endif

	if(   ::RegOpenKeyEx(   HKEY_LOCAL_MACHINE,
			TEXT("Hardware\\DeviceMap\\SerialComm"),
			NULL,
			KEY_READ,
			&hKey)   ==   ERROR_SUCCESS)   //   打开串口注册表
	{
		int   i=0;
		char   portName[256],commName[256];
		DWORD   dwLong,dwSize;

		while(1)
		{
			dwLong   =   dwSize   =   sizeof(portName);
			if(   ::RegEnumValue(   hKey,
									i,
									(portName),
									&dwLong,
									NULL,
									NULL,
									(PUCHAR)commName,
									&dwSize   )   ==   ERROR_NO_MORE_ITEMS   )   //   枚举串口
			break;

			pComboBox-> AddString(   (LPCTSTR)commName   );   //   commName就是串口名字
			i++;
		}

		if( pComboBox-> GetCount()   ==   0)
		{
			//::printf<<(   "在HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm里找不到串口!!! "   );			
		}
		else
		{
			pComboBox->SetCurSel(0);
		}
		RegCloseKey(hKey);
	}
}
// CDlgComSetting 消息处理程序


void CDlgComSetting::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	int iSelNum, iComNum, iComBode, baudRate;
	CString comName, strComNum;
		
	this->UpdateData(TRUE);
	
	// TODO: 在此添加控件通知处理程序代码
	if(m_pUartCommon->GetStatus() == FALSE)
	{
		iSelNum = m_comboComName.GetCurSel(); 
		m_comboComName.GetLBText(iSelNum,comName);
		strComNum = comName.Right(comName.GetLength() - strlen("COM"));
		iComNum = atoi(strComNum.GetBuffer());

		/* UART Configuration */
		iComBode = m_comboComBaud.GetCurSel();

		CString strBaudRate;
		m_comboComBaud.GetLBText(iComBode, strBaudRate);
		baudRate = atoi(strBaudRate.GetBuffer());

		char comstr[20];
		if (iComNum <= 0)
        {
            return ;
        }
		else if (iComNum >= 10)
        {
            sprintf_s(comstr,20,"\\\\.\\COM%d",iComNum);			
        }
		else 
		{
            sprintf_s(comstr,20,"COM%d",iComNum);
        }
		
		int result;
		result = m_pUartCommon->SetUartParameter(comstr, baudRate);
		if(result == -1)
		{
			return ;
		}

		uint8 ret=0;
		ret = m_pUartCommon->Open();

		if(-1 == ret)
		{
			AfxMessageBox(TEXT("cannot open serial port"));
			return;	
		}
		else
		{
			//TODO:
			//return;
			//uint8 buf[4]={0x01, 02, 03,04};
			//m_pUartCommon->SendData(buf, 4);
		}

		//pThread=AfxBeginThread(ThreadFunc,this);  //Create the thread

		//Reset the flag
		GetDlgItem(IDOK)->SetWindowText(_T("关闭"));
	}
	else //The com is opened
	{		
		m_pUartCommon->Close();
		GetDlgItem(IDOK)->SetWindowText(_T("打开"));
		m_threadCloseFlag = 1;		
	}
	OnOK();
}


void CDlgComSetting::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
