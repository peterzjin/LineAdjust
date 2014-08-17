#pragma once
#include "afxwin.h"
#include "UartCommon.h"

// CDlgComSetting �Ի���

class CDlgComSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgComSetting)

public:
	CDlgComSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgComSetting();
	virtual BOOL OnInitDialog();

	void  FindCommPort(CComboBox   *pComboBox);

// �Ի�������
	enum { IDD = IDD_DLG_COM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboComName;
	CComboBox m_comboComBaud;
	
	CUartCommon*    m_pUartCommon;
	bool m_threadCloseFlag;
	bool m_comStatus;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
