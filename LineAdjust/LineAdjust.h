
// LineAdjust.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLineAdjustApp:
// �йش����ʵ�֣������ LineAdjust.cpp
//

class CLineAdjustApp : public CWinApp
{
public:
	CLineAdjustApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLineAdjustApp theApp;