#pragma once
#include "afxcmn.h"


// CDialogMainSetup 대화 상자입니다.

class CDialogMainSetup : public CDialog
{
	DECLARE_DYNAMIC(CDialogMainSetup)

public:
	CDialogMainSetup(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogMainSetup();

	typeInspectPara m_InsPara;
	typeSystemPara m_SysPara;

	int m_iIndex;
	CSliderCtrl m_cChSliderCtrl;
	UINT m_img0_ch0;
	int m_thrsh;
	int m_score;
	int m_poslimit;
	long m_ppunch_minarea;
	long m_ppunch_maxarea;
	long m_ppunchbur_tol;
	long m_ppunch_xtol;
	long m_ppunch_ytol;
	double m_ppunch_limit;
	double m_ppunch_width;
	double m_ppunch_height;
	double m_dPunch_Size_Tol;

	void SetRegion(int idx);
	void Update(BOOL flag);


// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAIN_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlSetupOk();
	void ClickBtnenhctrlSetupApply();
	void ClickBtnenhctrlAlignTeach();
	void ClickBtnenhctrlAlignPositionset();
	void ClickBtnenhctrlAlignTest();
	void ClickBtnenhctrlAlignPositionset2();
	void ClickBtnenhctrlAlignTest2();
	void ClickBtnenhctrlChipTeach();
	int m_edit_nChipThreshold;
	void ClickBtnenhctrlChipTest();
	double m_edit_dChipWidth;
	double m_edit_dChipHeight;
	double m_edit_dChipTolerance;
	int m_edit_nChipCX;
	int m_edit_nChipCY;
	double m_dPunch_Size_Tol2;
	int m_edit_nSerialPort;
};
