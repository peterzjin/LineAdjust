/**************************************************************************************************  
 * @file hal_uart.c
 * @brief This file contains the interface to the H/W UART driver.
 * @author Jin Li <jli@sychip.com.cn>  
 * @version 1.0
 * @date 2012-04-09
 **************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
//#include<afx.h>

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_uart.h"
//#include "stdafx.h"

#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */


static	COMSTAT		ComStat;
HANDLE				hCom;				//全局变量，串口句柄

BOOL				m_bThreadAlive;

halUARTBufControl_t	RxBuf;
static uint8		F_ComOpen;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/******************************************************************************
 * @fn      HalUARTInit
 *
 * @brief   Initialize the UART
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTInit(void)
{

	m_bThreadAlive = FALSE;
}


/******************************************************************************
 * @fn      HalUARTOpen
 *
 * @brief   Open a port according to the configuration specified by parameter.
 *
 * @param   port   - UART port
 *          config - contains configuration information
 *
 * @return  Status of the function call
 *****************************************************************************/
uint8 HalUART0pen(char* portnr,					// portnumber (1..4)
							uint32  baud,			// baudrate
							char	parity,			// parity 
							uint8	databits,		// databits 
							uint8	stopbits,		// stopbits 
							halUARTCfg_t *pConfig)		
								
{
	DCB dcb;
	uint32	dwCommEvents=EV_RXCHAR;
	COMMTIMEOUTS		TimeOuts;
	HANDLE			hThread;
	uint32			ThreadID;
	char* szBaud;

	szBaud = (char*)malloc(100);
	if(szBaud == NULL)
		 return -1;
	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);
	if( F_ComOpen==TRUE)
	{
		//MessageBox(NULL,TEXT("Comm has already been Opened."), TEXT("Message"), MB_OK); 
		//TRACE(TEXT("Comm has already been Opened.\n"));
		printf("Comm has already been Opened.\n");
		return -1;
	}
	
	hCom=CreateFile(portnr,//COM2口
					GENERIC_READ|GENERIC_WRITE, //允许读和写
					0, //独占方式
					NULL,
					OPEN_EXISTING, //打开而不是创建
					FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //重叠方式
					NULL);
  if(hCom==(HANDLE)-1)
  {
	  DWORD errReason = GetLastError();
	  printf("%d", errReason);
	CloseHandle(hCom);
	//MessageBox(NULL,TEXT("Open Comm Fail."),TEXT("Message"), MB_OK); 
	//TRACE( TEXT("Open Comm Fail.\n"));
	printf("Open Comm Fail.\n");
	return -1;
  }
  else
  {
	  //MessageBox(NULL,TEXT("Open Comm Success."),TEXT("Message"), MB_OK); 
	  printf("Open Comm Success.\n");
	  F_ComOpen=TRUE;
	  RxBuf.pBuffer = (uint8*)malloc(255);
	  if(RxBuf.pBuffer == NULL)
		return -1;
  }

	SetupComm(hCom,1024,1024); //输入缓冲区和输出缓冲区的大小都是1024

	//设定读超时
	TimeOuts.ReadIntervalTimeout=1000;
	TimeOuts.ReadTotalTimeoutMultiplier=0; //1000;
	TimeOuts.ReadTotalTimeoutConstant=0;//1000;

	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=1000;
	TimeOuts.WriteTotalTimeoutConstant=1000;
	

	//串口超时参数设置
	if (SetCommTimeouts(hCom, &TimeOuts))
	{	
		//设置串口事件掩码				   
		if (SetCommMask(hCom, dwCommEvents))
		{
			//获取串口当前状态
			if (GetCommState(hCom, &dcb))
			{
				//dcb.EvtChar = 'q';
				dcb.fRtsControl = RTS_CONTROL_DISABLE;		// RTS请求发送数据设备应答
				dcb.fOutxCtsFlow = 0;
				dcb.fDtrControl = DTR_CONTROL_DISABLE;		// RTS请求发送数据设备应答
				dcb.fOutxCtsFlow = 0;
				dcb.fDsrSensitivity = 0;
				dcb.fOutX = 0;
				dcb.fInX = 0;

				//建立串口设备控制块
				if (BuildCommDCB(szBaud, &dcb))
				{
					if (SetCommState(hCom, &dcb))
					{	
						; // normal operation... continue
					}
					else
						printf("SetCommState() error");
				}
				else
					printf("BuildCommDCB() error");
			}
			else
				printf("GetCommState() error");
		}
		else
			printf("SetCommMask() error");
	}
	else
		printf("SetCommTimeouts() error");
	free(szBaud);
	szBaud=NULL;

	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CommThread, pConfig, 0, &ThreadID);

  return HAL_UART_SUCCESS;
}



uint8 CommThread( halUARTCfg_t *config)
{	
	DWORD			dwError = 0;
	BOOL			bResult = TRUE;
	uint8			readLen;
	uint8			Event =0;
	uint8			i;

	config->rx.pBuffer = (uint8*)malloc(config->rx.maxBufSize);
	if(config->rx.pBuffer == NULL)
		return -1;

	RxBuf.maxBufSize = config->rx.maxBufSize;

	m_bThreadAlive = TRUE;	
	// Clear comm buffers at startup
	if (hCom)		
		PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	// begin forever loop.  This loop will run as long as the thread is alive.
	while ( m_bThreadAlive)
	{ 
	
		bResult = ClearCommError(hCom, &dwError, &ComStat);

		if (ComStat.cbInQue == 0)
				continue;

		readLen = ReadChar( config->port,config->rx.pBuffer, config->rx.maxBufSize);
		//TRACE("readLen:%x.\n",readLen);

		for(i=0;i<readLen;i++)
		{
			if(RxBuf.bufferTail-RxBuf.bufferHead == RxBuf.maxBufSize)
				return 0;
			else
			{
				RxBuf.pBuffer[RxBuf.bufferTail++] = config->rx.pBuffer[i];
				RxBuf.bufferTail = RxBuf.bufferTail%RxBuf.maxBufSize;
			}
		
		}

		//TRACE("bufferTail:%x.\n",RxBuf.bufferTail);
		if(RxBuf.bufferTail - RxBuf.bufferHead == RxBuf.maxBufSize)
		{
			Event = HAL_UART_RX_FULL;
		}

		if(RxBuf.bufferTail - RxBuf.bufferHead >  RxBuf.maxBufSize*0.8)
		{
			Event = HAL_UART_RX_ABOUT_FULL;
		}

		if(NULL != config->callBackFunc)
		{
			config->callBackFunc( config->port,  Event);
		}
	

	} // close forever loop
	free(config->rx.pBuffer);
	config->rx.pBuffer=NULL;
	return 0;
}


/******************************************************************************
 * @fn      HalUARTClose
 *
 * @brief   Close a port.
 *
 * @param   port   - UART port
 *
 * @return  None
 *****************************************************************************/
void HalUARTClose ( uint8 port )
{
	(void)port;
	if(F_ComOpen==TRUE)
	{
		F_ComOpen=FALSE;
	    CloseHandle(hCom); 
		free(RxBuf.pBuffer);
		RxBuf.pBuffer=NULL;
		//MessageBox(NULL,TEXT("Close Comm Success."),TEXT("Message"), MB_OK); 
	}
	else
	{
	//	MessageBox(NULL,TEXT("Pleae Ensure the Comm in the Open State."),TEXT("Message"), MB_OK); 	
	}
}

/*****************************************************************************
 * @fn      HalUARTRead
 *
 * @brief   Read a buffer from the UART
 *
 * @param   port - USART module designation
 *          buf  - valid data buffer at least 'len' bytes in size
 *          len  - max length number of bytes to copy to 'buf'
 *
 * @return  length of buffer that was read
 *****************************************************************************/
uint8 ReadChar ( uint8 port, uint8 *pBuffer, uint16 length )
{	

	DWORD	dwBytesRead=length;
    BOOL	bReadStatus;
    DWORD	dwErrorFlags;
	OVERLAPPED			m_ov;
	
	(void)port;
	(void)pBuffer;

	memset(&m_ov,0,sizeof(OVERLAPPED));
	ResetEvent(m_ov.hEvent);

    ClearCommError(hCom,&dwErrorFlags,&ComStat);

	if(!ComStat.cbInQue)
	{
		return 0;
	}
       
    dwBytesRead=min(dwBytesRead,(DWORD)ComStat.cbInQue);
	
	bReadStatus=ReadFile(hCom, pBuffer,
								dwBytesRead, &dwBytesRead, &m_ov);

    if(!bReadStatus) 
    {
		if(GetLastError()==ERROR_IO_PENDING)
		{
			GetOverlappedResult(hCom,
                           &m_ov,&dwBytesRead,TRUE);

            // GetOverlappedResult函数的最后一个参数设为TRUE，
            //函数会一直等待，直到读操作完成或由于错误而返回。
			//WaitForSingleObject(m_ov.hEvent,INFINITE);	
			//PurgeComm(hCom,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
			return (uint8)dwBytesRead;
         }
	}
	return (uint8)dwBytesRead;
}


uint8 HalUARTRead ( uint8 port, uint8 *pBuffer, uint16 length )
{	
	uint8 cnt;
	for(cnt=0;cnt<length;cnt++)
	{
		if(RxBuf.bufferTail == RxBuf.bufferHead)
		{
			return 0;
		}
		else
		{
			*pBuffer++= RxBuf.pBuffer[RxBuf.bufferHead++];

			RxBuf.bufferHead = RxBuf.bufferHead%RxBuf.maxBufSize;
		}
	}
	return (uint8)cnt; 
}

/******************************************************************************
 * @fn      HalUARTWrite
 *
 * @brief   Write a buffer to the UART.
 *
 * @param   port - UART port
 *          buf  - pointer to the buffer that will be written, not freed
 *          len  - length of
 *
 * @return  length of the buffer that was sent
 *****************************************************************************/
uint8 HalUARTWrite ( uint8 port, uint8 *pBuffer, uint16 length )
{	

	// TODO: Add your control notification handler code here
	OVERLAPPED m_osWrite;
		
	DWORD dwBytesWrite=length;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	memset(&m_osWrite,0,sizeof(OVERLAPPED));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,pBuffer,
		dwBytesWrite,& dwBytesWrite,&m_osWrite);

	if(!bWriteStat)
	{
		if(GetLastError()==ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent,1000);
			return (uint8)dwBytesWrite;
		}
	}
	return (uint8)dwBytesWrite;
}
/******************************************************************************
 * @fn      HalUARTSuspend
 *
 * @brief   Suspend UART hardware before entering PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTSuspend( void )
{

}

/******************************************************************************
 * @fn      HalUARTResume
 *
 * @brief   Resume UART hardware after exiting PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTResume( void )
{

}

/***************************************************************************************************
 * @fn      HalUARTPoll
 *
 * @brief   Poll the UART.
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTPoll(void)
{

}

/**************************************************************************************************
 * @fn      Hal_UART_RxBufLen()
 *
 * @brief   Calculate Rx Buffer length - the number of bytes in the buffer.
 *
 * @param   port - UART port
 *
 * @return  length of current Rx Buffer
 **************************************************************************************************/
uint16 Hal_UART_RxBufLen( uint8 port )
{

	uint16 RxBufLen = RxBuf.bufferTail - RxBuf.bufferHead;
  	(void)port;
  return RxBufLen;
}

/******************************************************************************
******************************************************************************/
