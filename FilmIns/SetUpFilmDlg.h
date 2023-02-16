#pragma once


// CSetUpFilmDlg 대화 상자입니다.

class CSetUpFilmDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpFilmDlg)

public:
	CSetUpFilmDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpFilmDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_FILM_DIALOG };

public:
	float	m_mold_fact;
	long	m_mold_tol;
	int		m_mold_bright_tol;
	int		m_mold_bright_tol_white;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	int		m_map5;
	int		m_map6;

	int		m_pf_trsh;
	int		m_pf_trshhigh;

	long	m_pf_tol;
	long	m_mold_tol_light;
	long	m_pf_tol_nopf;
	long	m_punchburr_tol;
	long	m_punch_maxtol;
	long	m_punch_mintol;
	long	m_punch_error_pos;
	long	m_punch_error_pos2;

	int		m_pf_thsh_nopf;
	int		m_thresh_dark;
	int		m_thresh_white;

	int m_edit_EditErr_threshold;
	int m_edit_EditErr_tolerance;
	int m_pf_area;

	int		 m_iRectIdx;
	BOOL	 m_bShow;
	long	 m_lSrTeachThr;
	Hobject *m_Image;
	Hobject	 m_Region;
	Hobject  m_HallRegion;
	RECT	 m_RectRegion;
	RECT	 m_PFRectRegion;
	RECT	 m_DontCareRegion;

public:
	BOOL InspectFilmMold();
	BOOL InspectFilmPF();
	void SRTest(int type, int direct);
	void ChageStateInspParamInputBox();
	void Update(BOOL flag);

	//20150224 ngh 카메라별 파라메터 UI 정리
	//void ShowSetupPara(int CamIdx);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlFilmSrDark();
	void ClickBtnenhctrlFilmSrDarkMinus();
	void ClickBtnenhctrlFilmSrDarkTest2();
	void ClickBtnenhctrlFilmSrDark2();
	void ClickBtnenhctrlFilmSrDarkMinus2();
	void ClickBtnenhctrlFilmSrDarkTest3();
	void ClickBtnenhctrlFilmTest7();
	void ClickBtnenhctrlFilmTestPunch();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
