#pragma once
#include "threadloop.h"
class CMotorCtrl :
	public CThreadLoop
{
public:
	CMotorCtrl(void);
	~CMotorCtrl(void);
private:
	void ThreadFunc(void *pMsg);
};

