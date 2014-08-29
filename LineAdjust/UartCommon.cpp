#include "StdAfx.h"
#include "LineAdjust.h"
#include "UartCommon.h"
#include "hal_uart.h"

void HandleUartEvent(uint8 port, uint8 event)
{	
	//Find the special CUartCommon Object
	//TODO:
	CUartCommon*    m_pUartCommon=&(theApp.m_cUartCommon);

	//Handle the event 

	//Handle the receive event
	if(event | HAL_UART_RX_FULL)
	{
		printf("The event means rx full!\r\n");
	}

	if(NULL != m_pUartCommon->cbRcvFun)
	{
		m_pUartCommon->cbRcvFun(event);
	}
}

CUartCommon::CUartCommon(void)
{
	/* UART Configuration */
	//uartConfig.port					= iComNum;
	m_uartConfig.configured           = TRUE;		
	m_uartConfig.flowControl          = FALSE;
	m_uartConfig.flowControlThreshold = 32;
	m_uartConfig.rx.maxBufSize        = 255;
	m_uartConfig.tx.maxBufSize        = 255;
	m_uartConfig.idleTimeout          = 0;
	m_uartConfig.intEnable            = TRUE;
	m_uartConfig.callBackFunc         = HandleUartEvent;

	m_bComStatus = FALSE;
	m_threadCloseFlag = TRUE;
	m_iSendNum = 0;
	m_iRcvNum = 0;
}

CUartCommon::~CUartCommon(void)
{
}



int CUartCommon::SendData(uint8 *buf, uint16 length)
{
	uint8 result;
	result = HalUARTWrite(m_uartConfig.port,buf, (uint16)length);
	return result;
}

int CUartCommon::ReceiveData(uint8 *buf, uint16 length)
{
	return HalUARTRead (m_uartConfig.port, buf, length);
}

int CUartCommon::RegisterRcvByteCallBack(UART_RCV_CALLBACK rcvCallBack)
{
	if(rcvCallBack != NULL)
	{
		//m_uartConfig.callBackFunc = rcvCallBack;	
		cbRcvFun = rcvCallBack;
	}
	return 0;
}

int CUartCommon::SetUartParameter(char comStr[20], uint32 baudRate)
{
	switch(baudRate)
	{
	case	9600:
		baudRate  = 9600;; //波特率9600，无校验，8个数据位，1个停止位
		m_uartConfig.baudRate             = 9600;//HAL_UART_BR_9600;
		break;
	case	19200:
		baudRate  = 19200;; //波特率19200，无校验，8个数据位，1个停止位
		m_uartConfig.baudRate             = 19200; //HAL_UART_BR_19200;
		break;
	case	38400:
		baudRate  = 38400;; //波特率38400，无校验，8个数据位，1个停止位
		m_uartConfig.baudRate             = 38400;//HAL_UART_BR_38400;
		break;
	case	57600:
		baudRate  = 57600;; //波特率57600，无校验，8个数据位，1个停止位
		m_uartConfig.baudRate             = 57600;//HAL_UART_BR_57600;
		break;
	case	2:
		baudRate  = 115200; //波特率115200，无校验，8个数据位，1个停止位
		m_uartConfig.baudRate             = 115200;//HAL_UART_BR_115200;
		break;
	default:
		return -1;
		//break;
	}
	strcpy_s(m_strUartName, 20, comStr);
	
	CString tmpStr;
	tmpStr.Format("%s", comStr);
	int pos = tmpStr.ReverseFind('C');
	
	CString strComNum = tmpStr.Right((tmpStr.GetLength()-pos) - strlen("COM"));
	if(strComNum.GetLength() == 0)
	{
		return -1;
	}
	m_uartConfig.port = atoi(strComNum.GetBuffer());

	return 0;
}

int CUartCommon::Open(void)
{
	uint8 ret=0;
	
	ret = HalUART0pen(m_strUartName, m_uartConfig.baudRate, 'N',8, 1,&m_uartConfig);

	if(HAL_UART_SUCCESS == ret)
	{		
		this->m_bComStatus = TRUE;
		return 0;
	}
	else
	{
		AfxMessageBox(TEXT("cannot open serial port"));
		return -1;
	}
}

int CUartCommon::Close(void)
{	
	this->m_bComStatus = FALSE;
	HalUARTClose(m_uartConfig.port);
	return 0;
}

bool CUartCommon::GetStatus(void)
{
	return this->m_bComStatus;
}

