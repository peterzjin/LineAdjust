#pragma once
#include "afxwin.h"
#include "UartCommon.h"

// CDlgComSetting 对话框

class CDlgComSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgComSetting)

public:
	CDlgComSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgComSetting();
	virtual BOOL OnInitDialog();

	void  FindCommPort(CComboBox   *pComboBox);

// 对话框数据
	enum { IDD = IDD_DLG_COM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
