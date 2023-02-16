// TriggerBoard.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "TriggerBoard.h"
#include "windows.h"

CTriggerBoard *pMain = NULL;

int EthCallbackFunc(void *pSrc, void *pData)
{
	pMain->SendMessage(MSG_RECEIVED, (WPARAM)pSrc, (LPARAM)pData);
	return 0;
}

UINT TriggerThread(LPVOID pParam)
{
	CTriggerBoard *pdlg = (CTriggerBoard*)pParam;
	DWORD start, end, trig_timeout;
	int cnt = 0;

	HTuple StartSecond, EndSecond;

	while(TRUE)
	{
		if(pdlg->m_bTriggerThread)
		{
			switch(pdlg->m_nProcess_Type)
			{
				//basic
			case TRIGGER_THREAD_DEFULT_FUNC:

				break;
				//OneShotTrigger
			case TRIGGER_THREAD_ONE_SHOT_FUNC:
				for(int i = 0; i < 100; i++)
				{
					pdlg->pUCP->SetTrigOneShot();
					Sleep(10);
				}
				pdlg->m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;
				break;
				//TriggerTrigCntReset
			case TRIGGER_THREAD_TRIG_CNT_RESET_FUNC:
				pdlg->pUCP->SetLineTrigCntReset();
				Sleep(10);
				pdlg->m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;
				break;
				//TriggerPosCntReset
			case TRIGGER_THREAD_POS_CNT_REST_FUNC:
				pdlg->pUCP->SetTrigPosReset();
				Sleep(10);
				pdlg->m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;
				break;
				//TriggerConnectionStatus
			case TRIGGER_THREAD_CONNECTION_STATUS_FUNC:
				pdlg->m_bTriggerStatus = FALSE;
				pdlg->m_bTrigger_TimeOut = FALSE;
				while(TRUE)
				{
					start = GetTickCount();
					
					pdlg->pUCP->Initialize(&EthCallbackFunc);
					Sleep(10);
					end = GetTickCount();

					if(pdlg->pUCP->m_EthConnection == 1)
					{
						pdlg->m_bTriggerStatus = TRUE;
						break;
					}
					trig_timeout = (end - start) / (double)1000;

					if(trig_timeout >= TRIGGER_TIMEOUT)
					{
						pdlg->m_bTriggerStatus = FALSE;
						pdlg->m_bTrigger_TimeOut = TRUE;
						break;
					}
				}
				

				pdlg->m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;
				break;
				//TrigPosition & TriggerCnt Reset
			case TRIGGER_THREAD_POS_CNT_RESET_FUNC:
				pdlg->m_bTriggerTrigPosReset = FALSE;
				pdlg->m_bTrigger_TimeOut = FALSE;
				
				
/*
				count_seconds(&StartSecond);	

				while(!pdlg->m_bTriggerTrigPosReset)
				{*/
					pdlg->pUCP->SetTrigPosReset();
					Sleep(10);
					pdlg->pUCP->SetLineTrigCntReset();
					Sleep(10);
					
					//ngh test 20150515
					pdlg->m_bTriggerTrigPosReset = TRUE;
					pdlg->m_bTrigger_TimeOut = FALSE;
					
					/*
					pdlg->pUCP->GetTrigStatus();
					Sleep(10);
					pdlg->pUCP->GetVisionTrig();
					Sleep(10);


					if(((pdlg->pUCP->m_VisionTrigger.TrigStatus.CurPosition > -5) && (pdlg->pUCP->m_VisionTrigger.TrigStatus.CurPosition < 5)) && 
						((pdlg->pUCP->m_VisionTrigger.TrigStatus.LScanTrigOut_Counter > -5) && (pdlg->pUCP->m_VisionTrigger.TrigStatus.LScanTrigOut_Counter < 5)))
					{
						pdlg->m_bTriggerTrigPosReset = TRUE;
						pdlg->m_bTrigger_TimeOut = FALSE;
						break;
					}
					
					count_seconds(&EndSecond);

					EndSecond = EndSecond - StartSecond;
					trig_timeout = (long)(((double)EndSecond[0]) * 1000);

					if(trig_timeout >= TRIGGER_TIMEOUT)
					{
						pdlg->m_bTriggerTrigPosReset = FALSE;
						pdlg->m_bTrigger_TimeOut = TRUE;
						break;
					}

					Sleep(1);
				}*/
				pdlg->m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;
				break;
			}
		}
		else
		{
			break;
		}
		
		Sleep(1);
	}

	return 0;
}



CTriggerBoard::CTriggerBoard()
{
	WSADATA wsadata;
	WSAStartup(0x0202, &wsadata);

	pUCP = new CUCP();

	m_nProcess_Type = TRIGGER_THREAD_DEFULT_FUNC;

	m_pThreadTrigger = AfxBeginThread(TriggerThread, this, THREAD_PRIORITY_NORMAL);
	m_pThreadTrigger->m_bAutoDelete = TRUE;

	m_bTriggerThread = TRUE;

	m_bTriggerStatus = FALSE;

	m_bTriggerTrigPosReset = FALSE;
	m_bTrigger_TimeOut = FALSE;
}

CTriggerBoard::~CTriggerBoard()
{
	
}

BEGIN_MESSAGE_MAP(CTriggerBoard, CWnd)
	ON_MESSAGE(MSG_RECEIVED, OnMsgReceived)
END_MESSAGE_MAP()

LRESULT CTriggerBoard::OnMsgReceived(WPARAM wParam, LPARAM lParam)
{
	SUCPSRC *pSrc = (SUCPSRC*)wParam;
	SUCPMsg* pMsg = (SUCPMsg*)lParam;

	if (pMsg == NULL && pSrc == NULL) 
	{
		Invalidate(NULL);
		return false;
	}

	switch (pMsg->Header.Cmd)
	{
	case GET_TRIG_STATUS:
		Invalidate(NULL);
		break;
	}

	return true;
}


BOOL CTriggerBoard::Init(CString strINIpath)
{
	pMain = m_cTrigger;

	strINIpath += _T("\\TriggerBoard.INI");
	pUCP->m_ini.m_ini.SetPathName(strINIpath);

	Trigger_Para_Load();

	pUCP->Initialize(&EthCallbackFunc);

	m_nProcess_Type = TRIGGER_THREAD_CONNECTION_STATUS_FUNC;

	while(TRUE)
	{
		if(m_bTriggerStatus)
		{
				break;
		}

		Sleep(1);
	}

	return GetTriggerStatus();
}

BOOL CTriggerBoard::GetTriggerStatus()
{
	return m_bTriggerStatus;
}

void CTriggerBoard::CloseTrigger()
{
	Trigger_Para_Save();

	m_bTriggerThread = FALSE;
	m_pThreadTrigger = NULL;
	pUCP = NULL;
	m_cTrigger = NULL;

	WSACleanup();

	delete m_pThreadTrigger;
	delete pUCP;
	delete m_cTrigger;
}

void CTriggerBoard::SetTriggerOneShot()
{
	m_nProcess_Type = TRIGGER_THREAD_ONE_SHOT_FUNC;
}

void CTriggerBoard::SetTriggerTrigCntReset()
{
	m_nProcess_Type = TRIGGER_THREAD_TRIG_CNT_RESET_FUNC;
}
void CTriggerBoard::SetTriggerPosCntReset()
{
	m_nProcess_Type = TRIGGER_THREAD_POS_CNT_RESET_FUNC;
}

void CTriggerBoard::SetTrigger_Trig_Pos_Reset()
{
	m_nProcess_Type = TRIGGER_THREAD_POS_CNT_RESET_FUNC;
}

void CTriggerBoard::Trigger_Para_Save()
{
	CString strSection, strIP;

	pUCP->GetVisionTrig();
	pUCP->GetTrigConfig();

	strSection.Format(_T("TRIGGER_SYSTEM_INFO"));
	strIP.Format(_T("%d.%d.%d.%d"), pUCP->m_Target.IpAddr.IP0, pUCP->m_Target.IpAddr.IP1, 
									pUCP->m_Target.IpAddr.IP2, pUCP->m_Target.IpAddr.IP3);
	pUCP->m_ini.WriteString(strSection, _T("IP_ADDRESS"), strIP);
	pUCP->m_ini.WriteInt(strSection, _T("PORT_NUM"), pUCP->m_Target.IpPort);
	

	strSection.Format(_T("TRIGGER_CONFIG_INFO"));
	pUCP->m_ini.WriteInt(strSection, _T("AXIS_COUNTER_DIR"), pUCP->m_VisionTrigger.TrigConfig.bits.AxisCounterDir);

	pUCP->m_ini.WriteInt(strSection, _T("MODE_CH0"), pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH0);
	pUCP->m_ini.WriteInt(strSection, _T("ENABLE_CH0"), pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH0);
	pUCP->m_ini.WriteInt(strSection, _T("LOGIC_CH0"), pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH0);

	pUCP->m_ini.WriteInt(strSection, _T("MODE_CH1"), pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH1);
	pUCP->m_ini.WriteInt(strSection, _T("ENABLE_CH1"), pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH1);
	pUCP->m_ini.WriteInt(strSection, _T("LOGIC_CH1"), pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH1);

	pUCP->m_ini.WriteInt(strSection, _T("MODE_CH2"), pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH2);
	pUCP->m_ini.WriteInt(strSection, _T("ENABLE_CH2"), pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH2);
	pUCP->m_ini.WriteInt(strSection, _T("LOGIC_CH2"), pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH2);


	strSection.Format(_T("TRIGGER_POSITION_INFO"));
	pUCP->m_ini.WriteInt(strSection, _T("START_POSITION"), pUCP->m_VisionTrigger.LineTrig.StartPos);
	pUCP->m_ini.WriteInt(strSection, _T("END_POSTION"), pUCP->m_VisionTrigger.LineTrig.EndPos);
	pUCP->m_ini.WriteInt(strSection, _T("TRIGGER_PULSE_WIDTH"), pUCP->m_VisionTrigger.LineTrig.TrigPulseWidth);
	pUCP->m_ini.WriteInt(strSection, _T("GAP_SPACE"), pUCP->m_VisionTrigger.LineTrig.GapSpace);
}

void CTriggerBoard::Trigger_Para_Load()
{
	CString strSection, strIP, temp;

	strSection.Format(_T("TRIGGER_SYSTEM_INFO"));
	temp = pUCP->m_ini.GetString(strSection, _T("IP_ADDRESS"));
	AfxExtractSubString(strIP, temp, 0, '.');
	pUCP->m_Target.IpAddr.IP0 = atoi(strIP);
	AfxExtractSubString(strIP, temp, 1, '.');
	pUCP->m_Target.IpAddr.IP1 = atoi(strIP);
	AfxExtractSubString(strIP, temp, 2, '.');
	pUCP->m_Target.IpAddr.IP2 = atoi(strIP);
	AfxExtractSubString(strIP, temp, 3, '.');
	pUCP->m_Target.IpAddr.IP3 = atoi(strIP);
	pUCP->m_Target.IpPort = pUCP->m_ini.GetInt(strSection, _T("PORT_NUM"), 50010);
	

	strSection.Format(_T("TRIGGER_CONFIG_INFO"));
	pUCP->m_VisionTrigger.TrigConfig.bits.AxisCounterDir = pUCP->m_ini.GetInt(strSection, _T("AXIS_COUNTER_DIR"), 1);

	pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH0 = pUCP->m_ini.GetInt(strSection, _T("MODE_CH0"), 1);
	pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH0 = pUCP->m_ini.GetInt(strSection, _T("ENABLE_CH0"), 1);
	pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH0 = pUCP->m_ini.GetInt(strSection, _T("LOGIC_CH1"), 0);

	pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH1 = pUCP->m_ini.GetInt(strSection, _T("MODE_CH1"), 0);
	pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH1 = pUCP->m_ini.GetInt(strSection, _T("ENABLE_CH1"), 0);
	pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH1 = pUCP->m_ini.GetInt(strSection, _T("LOGIC_CH1"), 0);

	pUCP->m_VisionTrigger.TrigConfig.bits.ModeCH2 = pUCP->m_ini.GetInt(strSection, _T("MODE_CH2"), 0);
	pUCP->m_VisionTrigger.TrigConfig.bits.EnableCH2 = pUCP->m_ini.GetInt(strSection, _T("ENABLE_CH2"), 0);
	pUCP->m_VisionTrigger.TrigConfig.bits.LogicCH2 = pUCP->m_ini.GetInt(strSection, _T("LOGIC_CH2"), 0);


	strSection.Format(_T("TRIGGER_POSITION_INFO"));
	pUCP->m_VisionTrigger.LineTrig.StartPos = pUCP->m_ini.GetInt(strSection, _T("START_POSITION"), 100);
	pUCP->m_VisionTrigger.LineTrig.EndPos = pUCP->m_ini.GetInt(strSection, _T("END_POSTION"), 1500100);
	pUCP->m_VisionTrigger.LineTrig.TrigPulseWidth = pUCP->m_ini.GetInt(strSection, _T("TRIGGER_PULSE_WIDTH"), 9);
	pUCP->m_VisionTrigger.LineTrig.GapSpace = pUCP->m_ini.GetDouble(strSection, _T("GAP_SPACE"), 4.8);

	pUCP->SetTrigSConfig(&pUCP->m_VisionTrigger.TrigConfig);
	pUCP->SetVisionTrig(&pUCP->m_VisionTrigger);
	pUCP->SetSaveTrig();	
}