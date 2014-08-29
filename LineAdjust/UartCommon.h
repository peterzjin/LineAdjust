#pragma once

#include "hal_uart.h"

typedef  int (*UART_RCV_CALLBACK)(uint8 event);

class CUartCommon
{
public:
	CUartCommon(void);
	~CUartCommon(void);
	
	int SetUartParameter(char comStr[20], uint32 baud);
	int SendData(uint8 *buf, uint16 length);
	int ReceiveData(uint8 *buf, uint16 length);

	int RegisterRcvByteCallBack(UART_RCV_CALLBACK rcvCallBack);
	int Open(void);
	int Close(void);
	bool GetStatus(void);
public:
	halUARTCfg_t		m_uartConfig;
	UART_RCV_CALLBACK cbRcvFun;

private:
	int m_iSendNum;
	int m_iRcvNum;
	char m_strUartName[20];
	bool m_threadCloseFlag;
	bool m_bComStatus;
	
};
