#include "StdAfx.h"
#include "LineAdjust.h"
#include "MotorController.h"
#include "MotorCtrl6.h"


CMotorCtrl6::CMotorCtrl6(void)
{
	m_pUartCommon = &(theApp.m_cUartCommon);

	memset(m_send_buf, 0, MOTOR_SEND_BUFFER_SIZE);
	m_send_buf[0] = 0xff;
	m_send_buf[1] = 0xaa;
	m_send_buf[2] = 1;  //485 ID
	CalCheckSum();
}


CMotorCtrl6::~CMotorCtrl6(void)
{

}


void CMotorCtrl6::CalCheckSum(void)
{
	int i = 0;
	BYTE sum=0;

	for(i=0; i<MOTOR_SEND_BUFFER_SIZE-1; i++)
	{
		sum += m_send_buf[i];
	}
	m_send_buf[i] = sum;
}

void CMotorCtrl6::SendSyncCommand(void)
{
	CalCheckSum();
	//Send the protocol package
	if(m_pUartCommon->GetStatus())
	{
		m_pUartCommon->SendData(m_send_buf,MOTOR_SEND_BUFFER_SIZE);			
	}
	else
	{
		::AfxMessageBox(TEXT("The COM interface is closed!\r\n"), 0, 0);
	}
}

//step table is step table value,
//step angle is the 180 when the angle is 1.8C
void CMotorCtrl6::SetMotorStepTable(BYTE motorNum, int stepTable, int stepAngle)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;  
	m_send_buf[4] = 0x01;  //CMD	
	m_send_buf[5] = stepTable & 0x00ff;
	m_send_buf[6] = (stepTable & 0xff00) >> 8;
	m_send_buf[7] = stepAngle;  //if the step Angle is 1.8, the value is 180		
	SendSyncCommand();	
}

void CMotorCtrl6::SetMotorPulseNumber(BYTE motorNum, unsigned int pulseNum)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;
	m_send_buf[4] = 0x03;  //command	
	m_send_buf[5] =  pulseNum & 0x000000ff;
	m_send_buf[6] = (pulseNum & 0x0000ff00) >> 8;
	m_send_buf[7] = (pulseNum & 0x00ff0000) >> 16;
	SendSyncCommand();	
}

//direction = 0 clock , 1 is anticlock
//example startFrequence is 50 HZ
void CMotorCtrl6::SetMotorDirectionFrequence(BYTE motorNum, bool direction, unsigned int startFrequence)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;	
	m_send_buf[4] = 0x04;  //CMD
	m_send_buf[5] =  direction;
	m_send_buf[6] = (startFrequence & 0x0000ff);
	m_send_buf[7] = (startFrequence & 0x0000ff00) >> 8;
	SendSyncCommand();	
}

 //accerate is frequence, speed is RPM
//example accerate is 10/50 HZ, RPM 100~200RPM
void CMotorCtrl6::SetMotorAccerateAndSpeed(BYTE motorNum,unsigned int accerate, unsigned int speed)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;	
	m_send_buf[4] = 0x05;
	m_send_buf[5] = (accerate & 0x0000ff);
	m_send_buf[6] = (accerate & 0x0000ff00) >> 8;
	m_send_buf[7] = (speed & 0x0000ff);
	m_send_buf[8] = (speed & 0x0000ff00) >> 8;

	SendSyncCommand();	
}

void CMotorCtrl6::RunClockContinue(BYTE motorNum)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;	
	m_send_buf[4] = 0x09;
	SendSyncCommand();	
}

void CMotorCtrl6::RunAntiClockContinue(BYTE motorNum)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[2] = motorNum;
	m_send_buf[3] = 0x03;
	m_send_buf[4] = 0x09;
	SendSyncCommand();
}

void CMotorCtrl6::StopMotor(BYTE motorNum)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;	
	m_send_buf[4] = 0x06;
	SendSyncCommand();
}

void CMotorCtrl6::RunMotor(BYTE motorNum)
{
	memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	m_send_buf[3] = motorNum;	
	m_send_buf[4] = 0x09;
	SendSyncCommand();
}

void CMotorCtrl6::OpenLed(BYTE motorNum)
{
	//memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	//m_send_buf[3] = motorNum;
	//m_send_buf[4] = 0x0c;
	//m_send_buf[5] = 0x05;
	//m_send_buf[6] = 0x01;
	//SendSyncCommand();
}


void CMotorCtrl6::CloseLed(BYTE motorNum)
{

	//memset(m_send_buf + 3, 0, MOTOR_SEND_BUFFER_SIZE - 3);
	//m_send_buf[3] = motorNum;	
	//m_send_buf[4] = 0x0c;
	//m_send_buf[5] = 0x05;
	//m_send_buf[6] = 0x00;
	//SendSyncCommand();
}
