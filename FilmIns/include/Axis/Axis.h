#pragma once
#include "Axis_Io_def.h"

//��� ���� �� IO ������Ʈ


//�������� �����Ͽ� üũ�غ� ��
//EMO ���� ���� üũ�� ��
	//-> ��� ��� ����
//����� ���� ���۰���
	//�����ġ���� ������ ����. ��� ���� ���� �� ��

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

	//����üũ
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

	int ServoOn(BYTE nSlaveNum);								//���� ON. OFF ������ ���� ON
	int ServoOff(BYTE nSlaveNum);								//���� OFF.
	int Absmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//���� �̵�
	int Incmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//��� �̵�
	int JogMove(BYTE nSlaveNum, long nVel, BOOL bDir);			//���� �̵�		0:DIR_DEC	1:DIR_INC
	//int JogStop(BYTE nSlaveNum);								//���� ��ž
	long GetCurPos(BYTE nSlaveNum);								//���� ��ġ

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

	// IO ����....
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

