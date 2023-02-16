#include "stdafx.h"
#include "EthernetCtrl.h"

CEthernetCtrl::CEthernetCtrl()
{	
	m_hThread = NULL;
	m_hComm = INVALID_HANDLE_VALUE;
	m_TxCount = 0;
}


CEthernetCtrl::~CEthernetCtrl()
{
	Terminate();
}

BOOL  CEthernetCtrl::Initialize()
{
    if ( IsOpen() )  return true;

    SOCKET sock = socket(SOCK_DGRAM, AF_INET, IPPROTO_IP);
	
    if (INVALID_SOCKET != sock)
    {
        m_hComm = (HANDLE) sock;
		WatchComm();

		return true;
    }

	return false;
}


void CEthernetCtrl::Terminate()
{
    if (IsOpen())
    {
		shutdown((SOCKET)m_hComm, SD_BOTH);
		closesocket( (SOCKET)m_hComm );
        m_hComm = INVALID_HANDLE_VALUE;
    }	

    // Kill Thread
    if (IsStart())
    {
        SleepEx(100, TRUE);

        if (WaitForSingleObject(m_hThread, 1000L) == WAIT_TIMEOUT)
            TerminateThread(m_hThread, 1L);

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}


BOOL CEthernetCtrl::SendMsg(SUCPSRC *pSrc, SUCPMsg* pMsg)
{
	if(!IsOpen()) return false;	
	
	if(pSrc->UcpSize > UDP_MSS)	return 0;	

    SOCKADDR_IN sockAddr;	

	memset(&sockAddr, 0, sizeof(SOCKADDR_IN));
	sockAddr.sin_addr.S_un.S_un_b.s_b1 = (UCHAR)pSrc->IpAddr.IP0;
	sockAddr.sin_addr.S_un.S_un_b.s_b2 = (UCHAR)pSrc->IpAddr.IP1;
	sockAddr.sin_addr.S_un.S_un_b.s_b3 = (UCHAR)pSrc->IpAddr.IP2;
	sockAddr.sin_addr.S_un.S_un_b.s_b4 = (UCHAR)pSrc->IpAddr.IP3;

    sockAddr.sin_family = AF_INET;

	UVersion Ver;


	sockAddr.sin_port = htons(UCP_PORTNUM);
		
	Ver.bit.Major		 = UCP_VER_MAJOR;
	Ver.bit.Minor		 = UCP_VER_MINOR;
	Ver.bit.BuildYear	 = UCP_VER_BUILD_YEAR;
	Ver.bit.BuildMonth	 = UCP_VER_BUILD_MONTH;
	Ver.bit.BuildDay	 = UCP_VER_BUILD_DAY;

	pMsg->Header.Version = Ver.all;


	pMsg->Header.Status = true;
	pMsg->Header.TransactionID = m_TxCount++;


	return sendto( (SOCKET) m_hComm, (LPCSTR)pMsg, pSrc->UcpSize, 0, (SOCKADDR *) &sockAddr, sizeof(SOCKADDR_IN));
}


bool CEthernetCtrl::WatchComm()
{
    if (!IsStart())
    {
        if (IsOpen())
        {
            HANDLE hThread;
            UINT uiThreadId = 0;
            hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
                                      0,    // stack
                        SocketThreadProc,   // Thread proc
                                    this,   // Thread param
                        CREATE_SUSPENDED,   // creation mode
                            &uiThreadId);   // Thread ID

            if ( NULL != hThread)
            {
                //SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
                ResumeThread( hThread );
                m_hThread = hThread;
                return true;
            }
        }
    }
    return false;
}

void CEthernetCtrl::Run()
{
    DWORD   dwBytes;	

	SOCKADDR_IN sockAddr;
    int sockAddrSize = sizeof (sockAddr);
	
    while( IsOpen() )
    {
		dwBytes = recvfrom((SOCKET)m_hComm, (LPSTR)&m_RxMsg, UDP_MSS, 0, (SOCKADDR *)&sockAddr, &sockAddrSize);
		
		if (dwBytes >= UCPHDR_SIZE && dwBytes <= UDP_MSS)
        {	
			m_RxSrc.UcpSize = (Uint16)dwBytes - UCPHDR_SIZE;

			m_RxSrc.IpAddr.IP0 = sockAddr.sin_addr.S_un.S_un_b.s_b1;
			m_RxSrc.IpAddr.IP1 = sockAddr.sin_addr.S_un.S_un_b.s_b2;
			m_RxSrc.IpAddr.IP2 = sockAddr.sin_addr.S_un.S_un_b.s_b3;
			m_RxSrc.IpAddr.IP3 = sockAddr.sin_addr.S_un.S_un_b.s_b4;
			m_RxSrc.IpPort = ntohs(sockAddr.sin_port);

			memcpy(&m_Version, &m_RxMsg.Header.Version, sizeof(UVersion));

			m_EthConnection = true;
			m_CheckTime = GetTickCount();
			OnDataReceived(&m_RxSrc, &m_RxMsg);
        }
		
    }

	m_hThread = NULL;
}

UINT WINAPI CEthernetCtrl::SocketThreadProc(LPVOID pParam)
{
    CEthernetCtrl* pThis = reinterpret_cast<CEthernetCtrl*>( pParam );
    _ASSERTE( pThis != NULL );

    pThis->Run();

    return 1L;
} // end SocketThreadProc
