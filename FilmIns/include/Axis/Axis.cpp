#include "StdAfx.h"
#include "Axis.h"


CAxis::CAxis(void)
{
	m_bConnected = FALSE;
	m_nPortNum = 0;
	m_dPulseToMm = 0.;

	for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
	{
		m_bAmp[i] = FALSE;
		m_bHome[i] = FALSE;
	}

	m_strAxisName[0] = _T("AXIS Z");
}

CAxis::~CAxis(void)
{
	
}

BOOL CAxis::Initialize(BYTE nPortNum, DWORD dwBaudRate)
{
	BOOL rslt = TRUE;

	//접속
	if( !m_bConnected )
	{
		if( !FAS_Connect(nPortNum, dwBaudRate) )
		{
			rslt = FALSE;
		}
		else
		{
			m_nPortNum = nPortNum;
			m_bConnected = TRUE;

			m_bIOReadThreadStart = FALSE;

			m_pIORead = AfxBeginThread(IOReadThread, this, THREAD_PRIORITY_NORMAL);
			m_pIORead->m_bAutoDelete = TRUE;

			for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
			{
				ServoOn(i);
			}
		}
	}
	else
		rslt = FALSE;

	return rslt;
}


void CAxis::Release()
{
	m_bConnected = FALSE;

	for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
		ServoOff(i);

	FAS_Close(m_nPortNum);
}


void CAxis::IOReadThread()
{
	while(TRUE)
	{
		if(!m_bConnected)
			break;

		if(m_bIOReadThreadStart)
		{
			//OutputAllRead();
			//InputAllRead();

			for(int i = 0; i < MOT_STATUS_MAX_NUM; i++) 
			{
				if(m_bNegSensor[i] != GetNegLimitSensor(i))
					m_bNegSensor[i] = GetNegLimitSensor(i);
				
				if(m_bPosSensor[i] != GetPosLimitSensor(i))
					m_bPosSensor[i] = GetPosLimitSensor(i);

				if(m_lCurPos[i] != GetCurPos(i))
					m_lCurPos[i] = GetCurPos(i);
			}
		}	

		Sleep(1);
	}

}

UINT CAxis::IOReadThread(LPVOID pParam)
{
	CAxis *pdlg = (CAxis *)pParam;
	
	pdlg->IOReadThread();

	return 0;
}

BOOL CAxis::OutputAllRead()
{
	BOOL rslt = TRUE;
	//int slave;

	for(int i = 0; i < MAX_OUTPUT_CNT; i++)
	{
		//slave = slave = (i <= 3) ? AXIS_Y : AXIS_X;

		//m_bOutput[i] = ReadOutput(AXIS_Z, m_dwIndexOutput[i]);
	}

	return rslt;
}

BOOL CAxis::InputAllRead()
{
	BOOL rslt = TRUE;
	//int slave;

	for(int i = 0; i < MAX_INPUT_CNT; i++)
	{
		//slave = slave = (i <= 3) ? AXIS_Y : AXIS_X;

		//m_bInput[i] = ReadInput(AXIS_Z, m_dwIndexInput[i]);
	}

	return rslt;
}


//Error
/*
	FMM_OK : 명령이 정상적으로 수행되었습니다.
	FMM_NOT_OPEN : 아직 Board 를 연결하기 전 입니다.
	FMM_INVALID_PORT_NUM : 연결한 Port 중에 nPort 는 존재하지 않습니다.
	FMM_INVALID_SLAVE_NUM : 해당 Port 에 iSlaveNo 의 Slave 는 존재하지 않습니다.
*/


//현재 Motor 의 Actual Position 값을 읽어온다
long CAxis::GetCurPos(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	long lActPos = 0;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetActualPos(m_nPortNum, nSlaveNum, &lActPos);

	if(nRtn != FMM_OK)
	{

	}

	return lActPos;
}

//절대 이동
int CAxis::Absmove(BYTE nSlaveNum, long nPos, DWORD nVel) 
{
	int nRtn = FMM_OK;
	
	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveSingleAxisAbsPos(m_nPortNum, nSlaveNum, nPos, nVel);

	

	if (nRtn == FMM_OK)
	{
		
	}

	return nRtn;
}

//상대 이동
int CAxis::Incmove(BYTE nSlaveNum, long nPos, DWORD nVel) 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	if (!m_bHome[nSlaveNum])
		return -1;

		nRtn = FAS_MoveSingleAxisIncPos(m_nPortNum, nSlaveNum, nPos, nVel);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

int CAxis::JogMove(BYTE nSlaveNum, long nVel, BOOL bDir)
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveVelocity(m_nPortNum, nSlaveNum, nVel, bDir);		//bDir :   0:-  1:+

	if (nRtn != FMM_OK)
	{

	}
	
	return nRtn;
}
/*
int CAxis::JogStop(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}
*/

int CAxis::ServoOn(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

	EZISERVO_AXISSTATUS AxisStatus;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetAxisStatus(m_nPortNum, nSlaveNum, &(AxisStatus.dwValue));

	if (AxisStatus.FFLAG_SERVOON == SERVO_OFF)
		nRtn = FAS_ServoEnable(m_nPortNum, nSlaveNum, TRUE);

	if(nRtn == FMM_OK)
		m_bAmp[nSlaveNum] = TRUE;

	if (AxisStatus.FFLAG_ERRORALL || AxisStatus.FFLAG_ERROVERCURRENT || AxisStatus.FFLAG_ERROVERLOAD)
		nRtn = FAS_ServoAlarmReset(m_nPortNum, nSlaveNum);

	return nRtn;
}

int CAxis::ServoOff(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;

	EZISERVO_AXISSTATUS AxisStatus;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetAxisStatus(m_nPortNum, nSlaveNum, &(AxisStatus.dwValue));

	if (AxisStatus.FFLAG_SERVOON == SERVO_ON)
		nRtn = FAS_ServoEnable(m_nPortNum, nSlaveNum, FALSE);

	if(nRtn == FMM_OK)
	{
		m_bAmp[nSlaveNum] = FALSE;
		m_bHome[nSlaveNum] = FALSE;
	}

	return nRtn;
}

//한 축 정지
int CAxis::Stop(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

//모든 축 정지
int CAxis::AllStop() 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllMoveStop(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

//Single 원점
int CAxis::SingleHome(BYTE iSlaveNo) 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveOriginSingleAxis(m_nPortNum, iSlaveNo);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

//모든 축 원점
int CAxis::AllHome() 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllMoveOriginSingleAxis(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

int CAxis::MotReadOriginResult(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	EZISERVO_AXISSTATUS AxisStatus;
	int rslt = 0;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, (BYTE)nSlaveNum, &(AxisStatus.dwValue));

	//m_AxisSyncIntlock.Unlock();

	if(nRtn == FMM_OK)
	{
		if(AxisStatus.FFLAG_ORIGINRETOK == 1)
			rslt = 0;
		else if(AxisStatus.FFLAG_ORIGINRETURNING == 1)
			rslt = 1;
	}
	else
	{
		
	}

	return rslt;
}

//알람 리셋
int CAxis::AlarmReset(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_ServoAlarmReset(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}


//한 축 긴급 정지
int CAxis::Emergency(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_EmergencyStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

//모든 축 긴급 정지
int CAxis::AllEmergency() 
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllEmergencyStop(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}

BOOL CAxis::GetPosLimitSensor(BYTE nSlaveNum)
{
	ULONGLONG dwInput = 0;
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

	//m_AxisSyncIntlock.Lock();

	nRtn = FAS_GetIOInput((BYTE)m_nPortNum, nSlaveNum, &dwInput);
	
	//m_AxisSyncIntlock.Unlock();
	
	if(nRtn == FMM_OK)
	{
		if(dwInput & SERVO_IN_BITMASK_LIMITP)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{

	}

	return rslt;
}

BOOL CAxis::GetNegLimitSensor(BYTE nSlaveNum)
{
	ULONGLONG dwInput = 0;
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

	//m_AxisSyncIntlock.Lock();

	nRtn = FAS_GetIOInput((BYTE)m_nPortNum, nSlaveNum, &dwInput);
	
	//m_AxisSyncIntlock.Unlock();
	
	if(nRtn == FMM_OK)
	{
		if(dwInput & SERVO_IN_BITMASK_LIMITN)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{

	}
	 
	return rslt;
}


// IO 관련
int CAxis::WriteOutput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal)
{
	int rslt = TRUE;
	DWORD dwIOCLRMask = 0;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();	

	if (dwVal == TRUE)
	{
		if(FAS_SetIOOutput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwNo, dwIOCLRMask) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		if(FAS_SetIOOutput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwIOCLRMask, dwNo) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}		

	//m_AxisSyncIntlock.Unlock();	

	return rslt;
}

int CAxis::ReadOutput(BYTE nSlaveNum, DWORD dwNo)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;
	DWORD dwIOOutput;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();	
	nRtn = FAS_GetIOOutput(m_nPortNum, nSlaveNum, &dwIOOutput);
	//m_AxisSyncIntlock.Unlock();
	
	if(nRtn == FMM_OK)
	{
		if(dwIOOutput & dwNo)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		//rslt = FALSE;
	}

	

	return rslt;
}

int CAxis::WriteInput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal)
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();	

	nRtn = FAS_SetIOInput(m_nPortNum, nSlaveNum, dwNo, dwVal);

	//m_AxisSyncIntlock.Unlock();

	if(nRtn != FMM_OK)
	{
		
	}

	

	return nRtn;
}

int CAxis::ReadInput(BYTE nSlaveNum, DWORD dwNo)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;
	ULONGLONG dwIOInput;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();	

	nRtn = FAS_GetIOInput(m_nPortNum, nSlaveNum, &dwIOInput);

	//m_AxisSyncIntlock.Unlock();	
	
	if(nRtn == FMM_OK)
	{
		if(dwIOInput & dwNo)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		//rslt = FALSE;
	}

	

	return rslt;
}

BOOL CAxis::AmpFaultState(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	EZISERVO_AXISSTATUS state;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, (BYTE)nSlaveNum, &(state.dwValue));

	//m_AxisSyncIntlock.Unlock();

	if(nRtn == FMM_OK)
	{
		if(state.FFLAG_SERVOON == SERVO_ON)
			m_bAmp[nSlaveNum] = TRUE;
		else
			m_bAmp[nSlaveNum] = FALSE;
	}
	else
	{
		
	}

	

	return m_bAmp[nSlaveNum];
}

BOOL CAxis::AxisMoveDone(BYTE nSlveNum)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;
	EZISERVO_AXISSTATUS state;

	if (!m_bConnected)
		return -1;

	//m_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, nSlveNum, &(state.dwValue));

	//m_AxisSyncIntlock.Unlock();

	if(nRtn == FMM_OK)
	{
		if( (state.FFLAG_INPOSITION == 1) && (state.FFLAG_MOTIONING == 0) )
			rslt = TRUE;
	}
	else
	{

	}

	return rslt;
}


/*
BOOL CAxis::InputRead(DWORD dwAdd)
{
	BOOL rslt = TRUE;

	if(!GetConnectState())
	{
		return FALSE;
	}

	if(!m_nReadIOAxis)
	{
		return FALSE;
	}
	m_AxisSyncIntlock.Lock();	
#ifdef MOTION
	ULONGLONG dwIOInput;
	
	if(FAS_GetIOInput((BYTE)m_iPortNo, (BYTE)m_lAxis, &dwIOInput) == FMM_OK)
	{
		if(dwIOInput & dwAdd)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
		rslt = FALSE;

#endif

	m_AxisSyncIntlock.Unlock();	
	return rslt;
}

*/




//---------------------------------------------------------------------------------------






//인포지션 체크 #2
/*
BOOL CAxis::CheckInPostion(int nPortNunm, BYTE nSlaveNum)
{
	int nRtn;
	DWORD dwAxisStatus;
	EZISERVO_AXISSTATUS stAxisStatus;

	nRtn = FAS_GetAxisStatus(nPortNunm, nSlaveNum, &dwAxisStatus);

	stAxisStatus.dwValue = dwAxisStatus;

	if(stAxisStatus.FFLAG_MOTIONING)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
*/


/*
//인포지션 체크 #1
BOOL CDialogManual::GetEzPosComplete(int nMotionNum, long nPos)
{
	long pos;
	BOOL rslt = FALSE;
	pos = GetEZCurPos(nMotionNum);
	//pos = (long)((double)(pos) / m_ManualPara.ManualPulseToMM[nMotionNum]);

	if((nPos - 20) <= pos && pos <= (nPos + 20))
	{
		rslt = TRUE;
	}


	//if(pos == nPos)//if(pos == m_ManualPara.ManualMovePos[nMotionNum][nMotionMoveIdx])
	//{
	//	rslt = TRUE;
	//}

	return rslt;
}
*/

/*
BOOL CAxis::GetLimtSenser(BYTE nSlaveNum, BOOL isDirection)
{
	BOOL rslt = FALSE;
	int MMC_Err;
	DWORD dwInput=0;
	
	if (m_bConnected)
	{
		m_AxisSyncIntlock.Lock();
		MMC_Err = FAS_GetIOInput(m_nPortNum, nSlaveNum, &dwInput);
		m_AxisSyncIntlock.Unlock();
		if(MMC_Err == FMM_OK)
		{
			if(isDirection)
			{
				if(dwInput & SERVO_IN_BITMASK_LIMITP)
				{
					rslt = TRUE;
				}
				else
				{
					rslt = FALSE;
				}
			}
			else
			{
				if(dwInput & SERVO_IN_BITMASK_LIMITN)
				{
					rslt = TRUE;
				}
				else
				{
					rslt = FALSE;
				}
			}
		}
	}

	return rslt;
}

BOOL CAxis::GetHomeSenser(BYTE nSlaveNum)
{
	BOOL rslt = FALSE;
	int MMC_Err;
	DWORD dwInput=0;
		
	if (m_bConnected)
	{
		m_AxisSyncIntlock.Lock();
		MMC_Err = FAS_GetIOInput(m_nPortNum, nSlaveNum, &dwInput);
		//z축은 -limt 정보 가져오기
		m_AxisSyncIntlock.Unlock();

		if(MMC_Err == FMM_OK)
		{
		}
	}
	
	return rslt;
}
*/
/*
BOOL CAxis::EMOCheck(int nPort, BYTE nSlveNum)
{
	int Err=0;

	DWORD dwAxisStatus;

	EZISERVO_AXISSTATUS AxisStatus;

	Err = FAS_GetAxisStatus(nPort,nSlveNum,&dwAxisStatus);

	AxisStatus.dwValue = dwAxisStatus;

	if(AxisStatus.FFLAG_SERVOON == 1)
	{
		return TRUE;
	}
	else
	{

		return FALSE;
	}

}

BOOL CAxis::ServoChk(int nPort, BYTE nSlaveNum)
{

	int Err=0;

	DWORD dwAxisStatus;

	EZISERVO_AXISSTATUS AxisStatus;

	Err = FAS_GetAxisStatus(nPort,nSlaveNum,&dwAxisStatus);

	AxisStatus.dwValue = dwAxisStatus;

	if(AxisStatus.FFLAG_SERVOON == 1)
	{
		return TRUE;
	}
	else
	{

		return FALSE;
	}
}
*/