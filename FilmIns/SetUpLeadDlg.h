#pragma once


// CSetUpLeadDlg 대화 상자입니다.

class CSetUpLeadDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpLeadDlg)

public:
	CSetUpLeadDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpLeadDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_LEAD_DIALOG };

public:
	int		m_edge_trsh;
	float	m_edge_fact;
	long	m_edge_tol;
	float	m_edge_fact2;
	int		m_edge_trsh2;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	long	m_edge_tol2;
	int		m_dirt_tol;
	int		m_dirt_thrsh;

	CToolTipCtrl m_tip_ctrl_lead;

	//AM#1 TOP
	int		m_edit_AM_threshld_scratch_0;
	int		m_edit_AM_tolerance_scratch_0;
	int		m_edit_AM_average_bright_0;
	int		m_edit_AM_tolerance_bright_0;

	//AM#1 BOTTOM
	int		m_edit_AM_threshld_scratch_1;
	int		m_edit_AM_tolerance_scratch_1;
	int		m_edit_AM_average_bright_1;
	int		m_edit_AM_tolerance_bright_1;

	//AM#2 TOP
	int		m_edit_AM_threshld_scratch_2;
	int		m_edit_AM_tolerance_scratch_2;
	int		m_edit_AM_average_bright_2;
	int		m_edit_AM_tolerance_bright_2;

	//AM#2 BOTTOM
	int		m_edit_AM_threshld_scratch_3;
	int		m_edit_AM_tolerance_scratch_3;
	int		m_edit_AM_average_bright_3;
	int		m_edit_AM_tolerance_bright_3;

	Hobject *m_Image;
	Hobject m_Region;
	Hobject m_Region2;
	RECT	m_RectRegion;
	RECT	m_RectRegion2;
	int		m_iRectIdx;
	BOOL	m_bShow;

public:
	void Update(BOOL flag);
	void ChageStateInspParamInputBox();
	BOOL InspectDnLead();
	BOOL InspectUpLead();

	//20150224 ngh 카메라별 파라메터 UI 정리
	void ShowSetupPara(int CamIdx);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlLeadTest2();
	void ClickBtnenhctrlLeadDnTest();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	int m_map5;
	void ClickBtnenhctrlLeadAmTest();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
