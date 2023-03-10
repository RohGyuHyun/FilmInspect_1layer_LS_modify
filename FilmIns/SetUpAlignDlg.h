#pragma once


// CSetUpAlignDlg 대화 상자입니다.


class CSetUpAlignDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpAlignDlg)

public:
	CSetUpAlignDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpAlignDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_ALIGN_DIALOG };

public:
	CToolTipCtrl m_tip_ctrl;

	int		m_thrsh;
	int		m_score;
	int		m_poslimit;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;

	int m_iRectIdx;
	Hlong m_lMWindowID;
	Hlong m_lMWindowID_SR;
	Hlong m_lMWindowID_Lead;
	Hlong m_lMWindowID_SROut;
	Hlong m_lMWindowID_Chip;
	int m_iIndex;

	long m_lSrTeachThr;
	long m_lLeadInTeachThr;

	int m_edit_iAM_Threshold;	//20160517

	BOOL m_bShow;

	void MasterImageCoverttoHalImageBuffer();
	void SetRegion(int idx);
	void Update(BOOL flag);

	//ngh AutoRegion
	int GetGapSpace(int pf_cnt, double pf_size, int pixel_size);
	void SetAutoRegion();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlAlignSetdefualt();
	void ClickBtnenhctrlAlignSetmaster();
	void ClickBtnenhctrlAlignLoadmaster2();
	void ClickBtnenhctrlAlignTeach();
	void ClickBtnenhctrlAlignPositionset();
	void ClickBtnenhctrlAlignTest();
	void ClickBtnenhctrlAlignInsRegionSrStart();
	void ClickBtnenhctrlAlignInsRegionSroStart();
	void ClickBtnenhctrlAlignInsSr();
	void ClickBtnenhctrlAlignInsRegionSrout();
	void ClickBtnenhctrlAlignInsRegionSrMinus();
	void ClickBtnenhctrlAlignInsRegionSroutMinus();
	void ClickBtnenhctrlAlignInsRegionPfStart();
	void ClickBtnenhctrlAlignInsRegionPf();
	void ClickBtnenhctrlAlignInsRegionPfMinus();
	void ClickBtnenhctrlAlignInsRegionLead();
	void ClickBtnenhctrlAlignInsRegionLeadIn();
	void ClickBtnenhctrlAlignFindchip();
	void ClickBtnenhctrlAlignPotTeach();
	int m_map4;
	void ClickBtnenhctrlAlignInsRegionAm();
	void ClickBtnenhctrlAlignInsRegionAmStart();
	void ClickBtnenhctrlAlignInsRegionMMinus();
	void ClickBtnenhctrlAlignInsRegionMMinusV();
	void ClickBtnenhctrlAlignInsRegionSrMinusV();
	void ClickBtnenhctrlAlignInsRegionSroutMinusV();
	void ClickBtnenhctrlAlignInsRegionPfMinusV();
	void ClickBtnenhctrlAlignInsRegionLeadV();
	void ClickBtnenhctrlAlignInsRegionLeadInV();
	//afx_msg void OnBnClickedButtonAutoRegion();
	void ClickBtnenhctrlAlignInsRegionLeadinStart();
	void ClickBtnenhctrlAlignInsRegionLeadinMinus();
	void ClickBtnenhctrlAlignSetAutoregion();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	//void ClickBtnenhctrlAlignAddExceptRegion();
	//void ClickBtnenhctrlAlignDelExceptRegion2();
};
