#pragma once

#include <queue>
#include "afxmt.h"

class CThreadLoop
{
public:
	CThreadLoop(void);
	~CThreadLoop(void);
private:
	bool m_bIsWorking;
	bool m_bStop;
	std::queue<void *> m_pMessage;
	HANDLE m_hEvent;
	HANDLE m_hExit;
	HANDLE m_hThreadLoop;
public:
	bool IsWorking(void);
	void PostMsg(void *pMsg, int len);
private:
	DWORD ThreadLoop(void);
	virtual void ThreadFunc(void *pMsg) = 0;
public:
	static DWORD WINAPI s_ThreadLoop(LPVOID lpParameter);
	DWORD StopThread(void);
};

