
// LineAdjust.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "DlgComSetting.h"
#include "UartCommon.h"
#include "MotorPannel.h"
#include "MotorCtrl.h"
#include "MotorController.h"
#include "StepMotor.h"
//#include "MotorCtrl6.h"

// CLineAdjustApp:
// �йش����ʵ�֣������ LineAdjust.cpp
//

class CLineAdjustApp : public CWinApp
{
public:
	CLineAdjustApp();
	CDlgComSetting m_dlgComSetting;
	CUartCommon    m_cUartCommon;
	CMotorPannel   m_dlgMotorPannel;
	CMotorCtrl     m_MotorCtrl;
	CStepMotor     *m_StepMotor[3];
	CMotorController *m_pMotorController;
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	
	DECLARE_MESSAGE_MAP()
};

extern CLineAdjustApp theApp;