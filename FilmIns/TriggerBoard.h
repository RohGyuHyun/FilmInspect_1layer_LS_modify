


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

	//Trigger Thread ����
	CWinThread *m_pThreadTrigger;
	BOOL m_bTriggerThread;

	//Trigger Thread FUNC ���� ����
	int m_nProcess_Type;

	//Trigger ���� ����
	BOOL m_bTriggerStatus;

	//Trigger ��ü
	CUCP *pUCP;
	CTriggerBoard *m_cTrigger;

	//Trigger �ʱ�ȭ
	BOOL Init(CString strINIpath);

	//Trigger ����
	void CloseTrigger();

	//Trigger ����
	BOOL GetTriggerStatus();

	//Trigger Thread FUNC Ʈ���� 1�� �߻�
	void SetTriggerOneShot();
	//Trigger Thread FUNC Ʈ���� ī��Ʈ 0
	void SetTriggerTrigCntReset();
	//Trigger Thread FUNC Ʈ���� ��ġī��Ʈ 0
	void SetTriggerPosCntReset();
	//Trigger Thread FUNC Ʈ���� & ��ġ ī��Ʈ 0
	void SetTrigger_Trig_Pos_Reset();
	//Ʈ���� & ��ġ ī��Ʈ 0 FUNC ����
	BOOL m_bTriggerTrigPosReset;
	//Ʈ���� TimeOut ����
	BOOL m_bTrigger_TimeOut;

	//Trigger Parameter
	void Trigger_Para_Save();
	void Trigger_Para_Load();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMsgReceived(WPARAM wParam, LPARAM lParam);
	
};