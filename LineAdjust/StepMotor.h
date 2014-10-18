#pragma once
#include "threadloop.h"
#include "UartCommon.h"
#include "MotorController.h"

class CStepMotor
{
public:
	CStepMotor(int num);
	~CStepMotor();

	void RunRollback(unsigned int roundNum);
	void RunForward(unsigned int roundNum);

public:
	int m_iMoterNum;
	int m_iSpeedConfig;
	bool m_bDirection;
	int m_iPluseNum;
	int m_iStartFrequency;
	int m_iRoundNum;
	bool m_bFirstFlag;
};
