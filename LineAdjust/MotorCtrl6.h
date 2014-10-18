#pragma once
#include "motorcontroller.h"
#include "threadloop.h"
#include "UartCommon.h"

#define MOTOR_SEND_BUFFER_SIZE     10
#define MOTOR_RCV_BUFFER_SIZE     10

class CMotorCtrl6 :
	public CMotorController
{
public:
	CMotorCtrl6(void);
	~CMotorCtrl6(void);

void CalCheckSum(void);
	void SendSyncCommand(void);

	void SetMotorStepTable(BYTE motorNum, int stepTable, int stepAngle);

	void SetMotorPulseNumber(BYTE motorNum, unsigned int pulseNum);
	//direction = 0 clock , 1 is anticlock, example startFrequence is 50 HZ
	void SetMotorDirectionFrequence(BYTE motorNum, bool direction, unsigned int startFrequence);
	void SetMotorAccerateAndSpeed(BYTE motorNum,unsigned int accerate, unsigned int speed); //accerate is frequence, speed is RPM
	void RunMotor(BYTE motorNum);


	void StopMotor(BYTE motorNum);	
	void RunClockContinue(BYTE motorNum);
	void RunAntiClockContinue(BYTE motorNum);
	
	void OpenLed(BYTE motorNum);
	void CloseLed(BYTE motorNum);

private:
	void ThreadFunc(void *pMsg);

public:
	CUartCommon*    m_pUartCommon;


	BYTE m_send_buf[MOTOR_SEND_BUFFER_SIZE];
	BYTE m_rcv_buf[MOTOR_RCV_BUFFER_SIZE];
};

