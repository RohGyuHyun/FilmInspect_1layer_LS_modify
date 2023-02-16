


#pragma once

#define MSG_RECEIVED								WM_USER + 50

#define TRIGGER_THREAD_DEFULT_FUNC					0
#define TRIGGER_THREAD_ONE_SHOT_FUNC				1
#define TRIGGER_THREAD_TRIG_CNT_RESET_FUNC			2
#define TRIGGER_THREAD_POS_CNT_REST_FUNC			3
#define TRIGGER_THREAD_CONNECTION_STATUS_FUNC		4
#define TRIGGER_THREAD_POS_CNT_RESET_FUNC			5

#define TRIGGER_TIMEOUT								5000

// CTriggerBoard

class CTriggerBoard : public CWnd
{

public:
	CTriggerBoard();
	virtual ~CTriggerBoard();

	//Trigger Thread 변수
	CWinThread *m_pThreadTrigger;
	BOOL m_bTriggerThread;

	//Trigger Thread FUNC 선택 변수
	int m_nProcess_Type;

	//Trigger 상태 변수
	BOOL m_bTriggerStatus;

	//Trigger 객체
	CUCP *pUCP;
	CTriggerBoard *m_cTrigger;

	//Trigger 초기화
	BOOL Init(CString strINIpath);

	//Trigger 종료
	void CloseTrigger();

	//Trigger 상태
	BOOL GetTriggerStatus();

	//Trigger Thread FUNC 트리거 1개 발생
	void SetTriggerOneShot();
	//Trigger Thread FUNC 트리거 카운트 0
	void SetTriggerTrigCntReset();
	//Trigger Thread FUNC 트리거 위치카운트 0
	void SetTriggerPosCntReset();
	//Trigger Thread FUNC 트리거 & 위치 카운트 0
	void SetTrigger_Trig_Pos_Reset();
	//트리거 & 위치 카운트 0 FUNC 상태
	BOOL m_bTriggerTrigPosReset;
	//트리거 TimeOut 변수
	BOOL m_bTrigger_TimeOut;

	//Trigger Parameter
	void Trigger_Para_Save();
	void Trigger_Para_Load();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMsgReceived(WPARAM wParam, LPARAM lParam);
	
};