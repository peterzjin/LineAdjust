#include "StdAfx.h"
#include "LineAdjust.h"
#include "StepMotor.h"

CStepMotor::CStepMotor(int num)
	:m_iMoterNum(num)
{
	m_iSpeedConfig = 200;
	m_bDirection = 0; //forward	
	m_iStartFrequency = 50;
	m_iPluseNum = 1600;
	m_iRoundNum = 1;
	m_bFirstFlag = 1;
}

CStepMotor::~CStepMotor()
{

}

void CStepMotor::RunRollback(unsigned int pulseNum)
{
	if(m_bDirection) //is rollback
	{
		m_bDirection = 0;
		//theApp.m_MotorCtrl.SetMotorDirectionFrequence(m_iMoterNum, m_bDirection, m_iStartFrequency);
		theApp.m_pMotorController->SetMotorDirectionFrequence(m_iMoterNum, m_bDirection, m_iStartFrequency);
		Sleep(200);
	}
	if((pulseNum != m_iPluseNum) || (m_bFirstFlag == 1))
	{
		m_bFirstFlag = 0;
		m_iPluseNum = pulseNum;
		//theApp.m_MotorCtrl.SetMotorPulseNumber(m_iMoterNum, m_iPluseNum);
		theApp.m_pMotorController->SetMotorPulseNumber(m_iMoterNum, m_iPluseNum);
		Sleep(200);
	}

	//theApp.m_MotorCtrl.RunMotor(m_iMoterNum);
	theApp.m_pMotorController->RunMotor(m_iMoterNum);
	Sleep(100);	
}

void CStepMotor::RunForward(unsigned int pulseNum)
{
	if(!m_bDirection) //is forward
	{
		m_bDirection = 1;
		//theApp.m_MotorCtrl.SetMotorDirectionFrequence(m_iMoterNum, m_bDirection, m_iStartFrequency);
		theApp.m_pMotorController->SetMotorDirectionFrequence(m_iMoterNum, m_bDirection, m_iStartFrequency);
		Sleep(200);
	}
	if((pulseNum != m_iPluseNum) || (m_bFirstFlag == 1))
	{
		m_bFirstFlag = 0;
		m_iPluseNum = pulseNum;
		//theApp.m_MotorCtrl.SetMotorPulseNumber(m_iMoterNum, m_iPluseNum);
		theApp.m_pMotorController->SetMotorPulseNumber(m_iMoterNum, m_iPluseNum);
		Sleep(200);
	}

	//theApp.m_MotorCtrl.RunMotor(m_iMoterNum);
	theApp.m_pMotorController->RunMotor(m_iMoterNum);
	Sleep(100);
}
