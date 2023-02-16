#pragma once

#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

#include "IF_UCP.h"

class CEthernetCtrl
{
public:
	CEthernetCtrl();
	~CEthernetCtrl();

public:	
	SUCPSRC m_RxSrc;
	SUCPMsg m_RxMsg; 
	Uint16 m_TxCount;

	UVersion	m_Version;	
	BOOL		m_EthConnection;
	DWORD		m_CheckTime;

	virtual void OnDataReceived(SUCPSRC *pSrc, SUCPMsg* pMsg){};

public:
	BOOL Initialize();
	void Terminate();

	BOOL SendMsg(SUCPSRC *pSrc, SUCPMsg* pMsg);

	bool IsOpen() { return ( INVALID_HANDLE_VALUE != m_hComm );}
	bool IsStart(){ return ( NULL != m_hThread ); }

// SocketComm - data
protected:
    HANDLE      m_hComm;        // Serial Comm handle
    HANDLE      m_hThread;      // Thread Comm handle

    bool WatchComm();       // Start Socket thread

// SocketComm - function
protected:
    static UINT WINAPI SocketThreadProc(LPVOID pParam);
// Run function - override to implement a new behaviour
    virtual void Run();
};




