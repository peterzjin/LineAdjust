#pragma once
#include "afxwin.h"
#include "UartCommon.h"
#include "afxcmn.h"


// CMotorPannel 对话框

class CMotorPannel : public CDialogEx
{
	DECLARE_DYNAMIC(CMotorPannel)

public:
	CMotorPannel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMotorPannel();

// 对话框数据
	enum { IDD = IDD_DIALOG_MOTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_motorNum;
	CComboBox m_motorControlType;
	int m_clockDirect;
	CUartCommon*    m_pUartCommon;
	
	bool m_antiClockDirect;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonClockRotate();

	afx_msg void OnBnClickedButtonAntiClockRotate3();
	afx_msg void OnBnClickedButtonMotorStop();
	afx_msg void OnBnClickedButtonClockRotate2();
	CString m_pluseNum;
	CString m_strMotorNum;
	BOOL m_bClockRotate;
	BOOL m_bLedOn;
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedButtonSet();
	CString m_strMotorSpeed;
	CSliderCtrl m_speedSlider;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit3();
};
