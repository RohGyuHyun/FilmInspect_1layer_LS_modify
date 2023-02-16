#pragma once


// CSetUpChipDlg ��ȭ �����Դϴ�.
#include "SetUpMotAxisZ.h"

class CSetUpChipDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpChipDlg)

public:
	CSetUpChipDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSetUpChipDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SETUP_CHIP_DIALOG };

public:
	
	int	m_out_trsh;
	int	m_out_tol;
	int m_out_tol_2;

	int	m_out_trsh_m;
	int m_out_tol_m;
	int	m_out_tol_m_2;

	int	m_out_trsh_s;
	int	m_out_tol_s;
	int	m_out_tol_s_2;
	int m_edit_iChipEdgeSize;


	long	m_mold_tol;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	int		m_map5;
	int		m_map6;
	int		m_map7;
	long	m_pot_tol;
	int		m_pot_trsh;
	long	m_npot_tol;
	int		m_mold_trsh_high;
	long	m_pot_area;
	int		m_potrange_tol;
	int		m_broken_thrsh;
	double	m_broken_sigma;

	//HTuple m_hPunchOffset;		
	//HTuple m_hFilmCenter;	
	//HTuple m_hChipCenter;			
	//long   m_lTemp;
	//CString m_lstrTemp;
	int		m_iRectIdx;
	Hobject m_PotRegion;
	BOOL	m_bShow;

	CSetUpMotAxisZ	*m_pdlgMotAxisZ;

public:
	void ChageStateInspParamInputBox();
	void Update(BOOL flag);
	BOOL InspectChipMold();
	BOOL InspectChipOut();
	BOOL InspectFindChip();
	BOOL InspectPot();

	//20150224 ngh ī�޶� �Ķ���� UI ����
	void ShowSetupPara(int CamIdx);

	void ClickBtnenhctrlChipFindchip();
	void ClickBtnenhctrlChipChipout();
	void ClickBtnenhctrlChipMold();
	void ClickBtnenhctrlChipPot();

	void ClickBtnenhctrlChipPotTeach();

	void AxisZMovePos(int nIdx, int nVal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	int m_pot_line_percent;
	int m_pot_width_1;
	int m_pot_width_2;
	int m_pot_width_3;
	int m_pot_width_4;
	int m_pot_width_5;
	int m_pot_width_6;
	int m_pot_average_cnt_x;
	int m_pot_average_cnt_y;

};
