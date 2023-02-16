#pragma once
#include "Axis_Io_def.h"

//모션 동작 및 IO 업데이트


//인포지션 관련하여 체크해볼 것
//EMO 전장 구성 체크할 것
	//-> 모션 긴급 정지
//에어리어 센서 동작관련
	//대기위치에서 간섭이 있음. 출발 이후 감지 할 것

class CAxis
{

public:
	CAxis(void);
	~CAxis(void);

protected:
	CCriticalSection m_AxisSyncIntlock;

	BOOL m_bInput[MAX_INPUT_CNT];
	BOOL m_bOutput[MAX_OUTPUT_CNT];
	//DWORD m_dwIndexInput[MAX_INPUT_CNT];
	//DWORD m_dwIndexOutput[MAX_OUTPUT_CNT];
	BOOL m_bAmp[MOT_STATUS_MAX_NUM];
	BOOL m_bHome[MOT_STATUS_MAX_NUM];
	BOOL m_bNegSensor[MOT_STATUS_MAX_NUM];
	BOOL m_bPosSensor[MOT_STATUS_MAX_NUM];
	long m_lCurPos[MOT_STATUS_MAX_NUM];

	BOOL m_bConnected;
	BYTE m_nPortNum;

	double m_dPulseToMm;

	CString m_strAxisName[MOT_STATUS_MAX_NUM];

public:
	BOOL Initialize(BYTE nPortNum, DWORD dwBaudRate = DEFAULT_BAUDRATE);
	void Release();

	//상태체크
	UINT static IOReadThread(LPVOID pParam);
	void IOReadThread();
	BOOL OutputAllRead();
	BOOL InputAllRead();

	BOOL GetNegSensor( int nIdx) { return m_bNegSensor[nIdx]; }
	BOOL GetPosSensor( int nIdx) { return m_bPosSensor[nIdx]; }
	BOOL GetOutput(int nIdx) { return m_bOutput[nIdx]; }
	BOOL GetInput(int nIdx) { return m_bInput[nIdx]; }
	void SetReadIOStart(BOOL bFlag) { m_bIOReadThreadStart = bFlag; }
	long GetCurrentPos(int nIdx) { return m_lCurPos[nIdx]; }
	////

	int ServoOn(BYTE nSlaveNum);								//서보 ON. OFF 상태일 때만 ON
	int ServoOff(BYTE nSlaveNum);								//서보 OFF.
	int Absmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//절대 이동
	int Incmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//상대 이동
	int JogMove(BYTE nSlaveNum, long nVel, BOOL bDir);			//조그 이동		0:DIR_DEC	1:DIR_INC
	//int JogStop(BYTE nSlaveNum);								//조그 스탑
	long GetCurPos(BYTE nSlaveNum);								//현재 위치

	CWinThread *m_pIORead;
	BOOL m_bIOReadThreadStart;

	int Stop(BYTE nSlaveNum);
	int AllStop();
	int SingleHome(BYTE iSlaveNo);
	int AllHome();
	int AlarmReset(BYTE nSlaveNum);
	int Emergency(BYTE nSlaveNum);
	int AllEmergency() ;
	BOOL AmpFaultState(BYTE nSlaveNum);
	int MotReadOriginResult(BYTE nSlaveNum);
	BOOL AxisMoveDone(BYTE nSlveNum);

	BOOL GetPosLimitSensor(BYTE nSlaveNum);
	BOOL GetNegLimitSensor(BYTE nSlaveNum);

	// IO 관련....
	int WriteOutput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal);
	int ReadOutput(BYTE nSlaveNum, DWORD dwNo);
	int WriteInput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal);
	int ReadInput(BYTE nSlaveNum, DWORD dwNo);

	void SetPulseToMm(double dPulseToMm){m_dPulseToMm = dPulseToMm;};
	BOOL GetAmpFaultState(BYTE nSlaveNum) { return m_bAmp[nSlaveNum]; }
	void SetAxisHomeFinished(BYTE nSlaveNum, BOOL bFlag) { m_bHome[nSlaveNum] = bFlag; }
	BOOL AxisIsHomeFinished(BYTE nSlaveNum) { return m_bHome[nSlaveNum]; }
	CString AxisGetAxisName(BYTE nSlaveNum) { return m_strAxisName[nSlaveNum]; }
};

