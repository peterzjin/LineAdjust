#include "StdAfx.h"
#include "ThreadLoop.h"


CThreadLoop::CThreadLoop(void)
	: m_bIsWorking(false)
	, m_bStop(false)
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, _T("MyEvent"));
	m_hExit = CreateEvent(NULL, FALSE, FALSE, _T("ExitEvent"));
	m_hThreadLoop = CreateThread(NULL, 0, s_ThreadLoop, this, 0, NULL);
}


CThreadLoop::~CThreadLoop(void)
{
}


bool CThreadLoop::IsWorking(void)
{
	return m_bIsWorking;
}


DWORD CThreadLoop::ThreadLoop(void)
{
	void *pMsg;
	while (!m_bStop) {
		WaitForSingleObject(m_hEvent, INFINITE);
		if (m_bStop) break;
		m_bIsWorking = TRUE;
		int size = m_pMessage.size();
		while (!m_pMessage.empty()) {
			pMsg = m_pMessage.front();
			m_pMessage.pop();
			ThreadFunc(pMsg);
			delete pMsg;
		}
		m_bIsWorking = FALSE;
	}

	SetEvent(m_hExit);
	return 0;
}

void CThreadLoop::PostMsg(void *pMsg, int len)
{
	char *l_pMsg;

	if (pMsg == NULL && len <= 0) {
		l_pMsg = new char[4];
	} else {
		l_pMsg = new char[len];
		memcpy(l_pMsg, pMsg, len);
	}

	m_pMessage.push(l_pMsg);
	SetEvent(m_hEvent);
}


DWORD WINAPI CThreadLoop::s_ThreadLoop(LPVOID lpParameter)
{
	return ((CThreadLoop *)lpParameter)->ThreadLoop();
}


DWORD CThreadLoop::StopThread(void)
{
	m_bStop = TRUE;
	SetEvent(m_hEvent);
	return WaitForSingleObject(m_hExit, 1000);
}
