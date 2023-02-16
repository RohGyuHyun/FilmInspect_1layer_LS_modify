#pragma once


// CSetUpMotAxisZ 대화 상자입니다.
#include "axis/axis.h"
#include "afxwin.h"


#define	TIMER_IO_READ		0
#define TIMER_ORIGIN_DLG	1

#define ORIGIN_OK			0
#define ORIGIN_SEARCHING	1

#define AXIS_POS_MAX_NUM	2
#define AXIS_POS_0			0
#define AXIS_POS_1			1


class CSetUpMotAxisZ : public CDialog
{
	DECLARE_DYNAMIC(CSetUpMotAxisZ)

public:
	CAxis					*m_pAxis;
	int						m_nRadioMoveType;			// 0 : Jog 1: Step
	double					m_dJogSpeed;
	int						m_nProcessMoveIdx;
	long					m_lPos[AXIS_POS_MAX_NUM];

	void DisplayReadIO();
	void DispAxisState();
	void DispCurPos();
	void AxisMove();
	void SetPosition();
	void UpdatePara(BOOL bstate);
	void AxisMovePos(int nIdx, int nVal);
	void SetAxisZPos(int nIdx, long pos) { m_lPos[nIdx] = pos; }
	long GetAxisZPos(int nIdx) { return m_lPos[nIdx]; }

public:
	CBtnEnh m_CBtnJog;
	CBtnEnh m_CBtnAxisSensorP;
	CBtnEnh m_CBtnAxisSensorN;
	CBtnEnh m_CBtnAxisHome;
	CBtnEnh m_CBtnAxisName;
	CBtnEnh m_CBtnAxisCurPos;
	CBtnEnh m_ctrl_progress;

	CBtnEnh m_CBtnAxisPos[AXIS_POS_MAX_NUM];

public:
	CSetUpMotAxisZ(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpMotAxisZ();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_MOTION_AXIS_Z };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_combo_move_speed;
	int m_edit_nStepVal;
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlMotionAxisZMoveJog();
	void ClickBtnenhctrlMotionAxisZMoveStep();
	afx_msg void OnCbnSelchangeComboAxisZJogSpeed();
	void ClickBtnenhctrlMotionAxisZHome();
	void ClickBtnenhctrlMotionAxisZStop();
	void ClickBtnenhctrlMotionAxisZServoOn();
	void ClickBtnenhctrlMotionAxisZServoOff();
	void ClickBtnenhctrlMotionAxisZSetpos1();
	void ClickBtnenhctrlMotionAxisZSetpos2();
	void ClickBtnenhctrlMotionAxisZApply();
	void ClickBtnenhctrlMotionAxisZMovepos1();
	void ClickBtnenhctrlMotionAxisZMovepos2();
};
