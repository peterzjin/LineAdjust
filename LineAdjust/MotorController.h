#pragma once
#include "threadloop.h"
class CMotorController	
{
public:
	CMotorController(void);
	~CMotorController(void);

public:
	virtual void SetMotorStepTable(BYTE motorNum, int stepTable, int stepAngle)=0;

	virtual void SetMotorPulseNumber(BYTE motorNum, unsigned int pulseNum)=0;
	//direction = 0 clock , 1 is anticlock, example startFrequence is 50 HZ
	virtual void SetMotorDirectionFrequence(BYTE motorNum, bool direction, unsigned int startFrequence)=0;
	virtual void SetMotorAccerateAndSpeed(BYTE motorNum,unsigned int accerate, unsigned int speed)=0; //accerate is frequence, speed is RPM
	virtual void RunMotor(BYTE motorNum)=0;


	virtual void StopMotor(BYTE motorNum)=0;	
    virtual void RunClockContinue(BYTE motorNum)=0;
	virtual void RunAntiClockContinue(BYTE motorNum)=0;
	
	virtual void OpenLed(BYTE motorNum)=0;
	virtual void CloseLed(BYTE motorNum)=0;
};

