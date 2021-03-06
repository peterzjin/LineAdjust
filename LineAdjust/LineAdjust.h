
// LineAdjust.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "DlgComSetting.h"
#include "UartCommon.h"
#include "MotorPannel.h"
#include "MotorCtrl.h"
#include "MotorController.h"
#include "StepMotor.h"
//#include "MotorCtrl6.h"

// CLineAdjustApp:
// 有关此类的实现，请参阅 LineAdjust.cpp
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
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	
	DECLARE_MESSAGE_MAP()
};

extern CLineAdjustApp theApp;