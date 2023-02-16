#include "stdafx.h"
#include "UmechaProtocol.h"


CallBackCtrl	gpCtrlCallback = NULL;

CUCP::CUCP()
{	
	::GetModuleFileName(NULL, m_sAppPath.GetBuffer(MAX_PATH), MAX_PATH);
	m_sAppPath.ReleaseBuffer();

	if (m_sAppPath.Find('\\') != -1)
	{
		for (int i = m_sAppPath.GetLength() - 1; i >= 0; i--)
		{
			TCHAR ch = m_sAppPath[i];
			m_sAppPath.Delete(i);
			if (ch == '\\')
				break;	
		}
	}

	memset(&m_ProductInfo, 0, sizeof(m_ProductInfo));
	memset(&m_VisionTrigger, 0, sizeof(m_ProductInfo));

	m_hSendingThread = NULL;

	

	memset(&m_Target, 0, sizeof(m_Target));

	m_Target.IpAddr.IP0 = ETH_IP_ADDR0;
	m_Target.IpAddr.IP1 = ETH_IP_ADDR1;
	m_Target.IpAddr.IP2 = ETH_IP_ADDR2;
	m_Target.IpAddr.IP3 = ETH_IP_ADDR3;

	m_Target.IpPort = (Uint16)UCP_PORTNUM;
	
}

CUCP::~CUCP()
{
	Terminate();
}


void CUCP::Terminate()
{
	CEthernetCtrl::Terminate();

	if (m_hSendingThread)
	{
		if (WaitForSingleObject(m_hSendingThread, 300L) == WAIT_TIMEOUT)
			TerminateThread(m_hSendingThread, 1L);
		CloseHandle(m_hSendingThread);
		m_hSendingThread = NULL;
	}
}

BOOL  CUCP::Initialize(CallBackCtrl pCallBack)
{
	// Initialize socket manager
	gpCtrlCallback = pCallBack;

	CEthernetCtrl::Initialize();

	HANDLE hThread;
	UINT uiThreadId = 0;
	hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
		0,    // stack
		SocketSendingThreadProc,   // Thread proc
		this,   // Thread param
		CREATE_SUSPENDED,   // creation mode
		&uiThreadId);   // Thread ID

	//SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
	ResumeThread(hThread);
	m_hSendingThread = hThread;
	
	return TRUE;
}

UINT WINAPI CUCP::SocketSendingThreadProc(LPVOID pParam)
{
	CUCP* pThis = reinterpret_cast<CUCP*>(pParam);
	_ASSERTE(pThis != NULL);

	SUCPSRC pOldTrg;

	memset(&pOldTrg, 0, sizeof(SUCPSRC));

	DWORD Timer = GetTickCount();

	while (pThis->m_hSendingThread)
	{
		if (GetTickCount() - Timer > 100)
		{
			Timer = GetTickCount();

			if(memcmp(&pOldTrg, &pThis->m_Target, sizeof(SUCPSRC))!=0) // 셋팅 정보가 변경 되면 : true.
			{
				memcpy(&pOldTrg, &pThis->m_Target, sizeof(SUCPSRC));
				pThis->GetProductInfo();
				pThis->GetVisionTrig();
			}
			else pThis->GetTrigStatus();
		}

		if (GetTickCount() - pThis->m_CheckTime > 1000)
		{
			pThis->m_EthConnection = false;
			pThis->m_CheckTime = GetTickCount();
			if (gpCtrlCallback) gpCtrlCallback(NULL, NULL);
		}			

		Sleep(1);
	}

	pThis->m_hSendingThread = NULL;

	return 1L;
} // end SocketThreadProc

void CUCP::OnDataReceived(SUCPSRC *pSrc, SUCPMsg* pMsg)
{
	static UINT16 OldTransactionID = -1;
	if (OldTransactionID == pMsg->Header.TransactionID)
	{
		return;
	}
	if (pMsg->Header.VerError)
	{
		return;
	}
	if (!m_hSendingThread) return;

	Uint16 Cmd = (Uint16)pMsg->Header.Cmd;

	m_EthConnection = true;

	switch(Cmd) 
	{
		case GET_PRODUCT_INFO:		
			memcpy(&m_ProductInfo, &pMsg->Body.ProductInfo, sizeof(m_ProductInfo));
			break;

		case GET_TRIG_STATUS:		
			memcpy(&m_VisionTrigger.TrigStatus, &pMsg->Body.VisionTrig.TrigStatus, sizeof(m_VisionTrigger.TrigStatus));
			break;

		case SET_TRIG_POS_RESET:	break;
		case SET_TRIG_ONE_SHOT:		break;
		case SET_TRIG_SAVE:		            break;

		case GET_TRIG_CONFIG:
			memcpy(&m_VisionTrigger.TrigConfig, &pMsg->Body.VisionTrig.TrigConfig, sizeof(m_VisionTrigger.TrigConfig));
			break;
		case SET_TRIG_CONFIG:		break;

		case GET_POS_TRIG:
			memcpy(&m_VisionTrigger.PosTrig, &pMsg->Body.VisionTrig.PosTrig, sizeof(m_VisionTrigger.PosTrig));
			if (m_VisionTrigger.PosTrig.Nums > MAX_POS_TRIG) m_VisionTrigger.PosTrig.Nums = 0;
			break;
		case SET_POS_TRIG:		    break;

		case GET_LINE_TRIG:
			memcpy(&m_VisionTrigger.LineTrig, &pMsg->Body.VisionTrig.LineTrig, sizeof(m_VisionTrigger.LineTrig));
			break;
		case SET_LINE_TRIG:			break;

		case GET_FUSION_TRIG:
			memcpy(&m_VisionTrigger.FusionTrig, &pMsg->Body.VisionTrig, sizeof(m_VisionTrigger.FusionTrig));
			if (m_VisionTrigger.FusionTrig.Nums > MAX_FUSION_TRIG) m_VisionTrigger.FusionTrig.Nums = 0;
			break;

		case SET_FUSION_TRIG:		break;

		case GET_VISION_TRIGGER:
			memcpy(&m_VisionTrigger, &pMsg->Body.VisionTrig, sizeof(m_VisionTrigger));
			if (m_VisionTrigger.PosTrig.Nums > MAX_POS_TRIG) m_VisionTrigger.PosTrig.Nums = 0;
			if (m_VisionTrigger.FusionTrig.Nums > MAX_FUSION_TRIG) m_VisionTrigger.FusionTrig.Nums = 0;
			break;

		case SET_VISION_TRIGGER:		break;

		// add kys 2014.08.18
		case SET_LINE_TRIG_CNT_RESET: break;

	}

	if (gpCtrlCallback) gpCtrlCallback(pSrc, pMsg);
}

void CUCP::GetProductInfo()
{
	SendMsg(0, GET_PRODUCT_INFO);
}


void CUCP::GetTrigStatus()
{
	SendMsg(0, GET_TRIG_STATUS);
}


void CUCP::SetLineTrigCntReset()
{
	SendMsg(0, SET_LINE_TRIG_CNT_RESET);
}

void CUCP::SetTrigPosReset()
{
	SendMsg(0, SET_TRIG_POS_RESET);
}

void CUCP::SetTrigOneShot()
{
	SendMsg(0, SET_TRIG_ONE_SHOT);
}

void CUCP::SetSaveTrig()
{
	SendMsg(0, SET_TRIG_SAVE);
}

void CUCP::GetTrigConfig()
{
	SendMsg(0, GET_TRIG_CONFIG);
}

void CUCP::SetTrigSConfig(STrigConfig *pData)
{
	SendMsg(0, SET_TRIG_CONFIG, sizeof(STrigConfig), (BYTE*)pData);
}

void CUCP::GetPosTrig()
{
	SendMsg(0, GET_POS_TRIG);
}

void CUCP::SetPosTrig(SPositionTrigger *pData)
{
	SendMsg(0, SET_POS_TRIG, sizeof(SPositionTrigger), (BYTE*)pData);
}

void CUCP::GetLineTrig()
{
	SendMsg(0, GET_LINE_TRIG);
}

void CUCP::SetLineTrig(SLineScanTrigger *pData)
{
	SendMsg(0, SET_LINE_TRIG, sizeof(SLineScanTrigger), (BYTE*)pData);
}

void CUCP::GetFusionTrig()
{
	SendMsg(0, GET_FUSION_TRIG);
}

void CUCP::SetFusionTrig(SFusionTrigger *pData)
{
	SendMsg(0, SET_FUSION_TRIG, sizeof(SFusionTrigger), (BYTE*)pData);
}

void CUCP::GetVisionTrig()
{
	SendMsg(0, GET_VISION_TRIGGER);
}

void CUCP::SetVisionTrig(SVisionTrigger *pData)
{
	SendMsg(0, SET_VISION_TRIGGER, sizeof(SVisionTrigger), (BYTE*)pData);
}


BOOL CUCP::SendMsg(BYTE Axis, Uint16 MsgCmd, short MsgSize, BYTE *Buffer)
{
	SUCPMsg Msg;

	memset(&Msg, 0, sizeof(Msg));

	Msg.Header.Cmd = MsgCmd;
	Msg.Header.Status = 0;

	m_Target.UcpSize = MsgSize + UCPHDR_SIZE;

	if(MsgSize) memcpy((void*)&Msg.Body, (void*)Buffer, MsgSize);

	return CEthernetCtrl::SendMsg(&m_Target, &Msg);
}


UINT16 CUCP::ConvPulse(float time)
{
	return (UINT16)(time / USTIME_GAIN +0.5);
}

float CUCP::ConvTime(UINT16 pulse)
{
	return (pulse * USTIME_GAIN);
}