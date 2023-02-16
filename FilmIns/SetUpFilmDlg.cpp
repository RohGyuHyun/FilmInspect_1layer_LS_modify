// SetUpFilmDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FilmIns.h"

#include "UserSockClient.h"
#include "include/_FuncModuleHal.h"
#include "include/_FuncModuleBasic.h"
#include "include/_VisionModuleFilmIns.h"
#include "include/_CtrlImageRect.h"
#include "include/_CtrlModelManage.h"
#include "DialogModelMenu.h"

#include "SetUpBsaeDlg.h"
#include "DialogProcessing.h"
#include "FilmInsDlg.h"

#include "SetUpFilmDlg.h"


// CSetUpFilmDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSetUpFilmDlg, CDialog)

CSetUpFilmDlg::CSetUpFilmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpFilmDlg::IDD, pParent)
	, m_edit_EditErr_threshold(0)
	, m_edit_EditErr_tolerance(0)
	, m_pf_area(0)
{
	m_mold_fact = 15.0f;
	m_mold_tol = 100;
	m_mold_bright_tol = 25;
	m_mold_bright_tol_white = 230;
	m_map0 = 0;
	m_map1 = 0;
	m_map2 = 3;
	m_map3 = 0;
	m_map4 = 0;
	m_map5 = 0;
	m_map6 = 0;
	m_pf_tol = 0;
	m_pf_trsh = 0;
	m_pf_trshhigh = 0;
	m_mold_tol_light = 0;
	m_pf_tol_nopf = 0;
	m_punchburr_tol = 10;
	m_punch_maxtol = 100000;
	m_punch_mintol = 400000;
	m_punch_error_pos = 200;
	m_pf_thsh_nopf = 0;
	m_thresh_dark = 0;
	m_punch_error_pos2 = 0;
	m_thresh_white = 0;
	//}}AFX_DATA_INIT
	
	m_iRectIdx = 0;
	
	m_bShow = FALSE;
	
	
	m_lSrTeachThr = 0;
}

CSetUpFilmDlg::~CSetUpFilmDlg()
{
}

void CSetUpFilmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MOLD_FACT_FILM, m_mold_fact);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM, m_mold_tol);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM, m_mold_bright_tol);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM_WHITE, m_mold_bright_tol_white);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_FILM, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_FILM, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM6_FILM, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_FILM, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM4_FILM, m_map4);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_FILM, m_map5);
	DDX_Text(pDX, IDC_EDIT_MAPNUM7_FILM, m_map6);
	DDX_Text(pDX, IDC_EDIT_PF_TOL_FILM, m_pf_tol);
	DDX_Text(pDX, IDC_EDIT_PF_TRSH_FILM, m_pf_trsh);
	DDX_Text(pDX, IDC_EDIT_PF_TRSHHIGH_FILM, m_pf_trshhigh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM2, m_mold_tol_light);
	DDX_Text(pDX, IDC_EDIT_PFEDIT_TOL_FILM, m_pf_tol_nopf);
	DDX_Text(pDX, IDC_EDIT_PUNCHBURR_TOL_FILM, m_punchburr_tol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_MAXTOL_FILM, m_punch_maxtol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_MINTOL_FILM, m_punch_mintol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_ERROR_POS, m_punch_error_pos);
	DDX_Text(pDX, IDC_EDIT_PF_TRSHHIGH_FILM2, m_pf_thsh_nopf);
	DDX_Text(pDX, IDC_EDIT_PUNCH_ERROR_POS2, m_punch_error_pos2);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_DARK, m_thresh_dark);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_WHITE, m_thresh_white);
	DDX_Text(pDX, IDC_EDIT_PF_TRSHHIGH_FILM3, m_edit_EditErr_threshold);
	DDX_Text(pDX, IDC_EDIT_PFEDIT_TOL_FILM2, m_edit_EditErr_tolerance);
	DDX_Text(pDX, IDC_EDIT_PF_AREA_FILM, m_pf_area);
}


BEGIN_MESSAGE_MAP(CSetUpFilmDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpFilmDlg �޽��� ó�����Դϴ�.

BOOL CSetUpFilmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

HBRUSH CSetUpFilmDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->GetParentBrush();

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	//return hbr;
}

void CSetUpFilmDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];

	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_FACT_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PFEDIT_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_MINTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_MAXTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCHBURR_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_DARK)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_WHITE)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_WHITE)->EnableWindow(state);
}


BOOL CSetUpFilmDlg::InspectFilmMold()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;
	
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if(pdlg->m_dlgSetup->m_InsPara.m_bCreatFilmVarID[i])
		//if(1)
		{
			ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
			chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
			ocv_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];

			rslt = pdlg->m_VisMod->FilmMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
				   //Mold image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
				   //Ink image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),
				   //User image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),
				   //SR-OUT Scratch image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),
				   ins_rect,
				   chip_ins_rect,
				   ocv_ins_rect,
				   ocv_chars_rgn,
				   
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),//SR region
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 4),//user region
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 5),//SR out region

				   pdlg->m_dlgSetup->m_InsPara.m_FilmVarID[i],
				   0, 0, 0, 0, 0, 0, 0, 0,
				   m_thresh_dark, m_mold_bright_tol, m_thresh_white, m_mold_bright_tol_white, m_mold_fact, m_mold_tol, m_mold_tol_light,
				   &rslt_num,
				   &rslt_rgn,
				   &rslt_sr_rgn,
				   &rslt_notsr_rng,
				   &rslt_user_rgn,
				   &rslt_sr_out_rgn);

			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : FILM MOLD");
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
		}

	
	}

	return TRUE;
}


BOOL CSetUpFilmDlg::InspectFilmPF()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_pf_rgn;


	if(1 > m_pf_trsh)
	{
		m_pf_trsh = 1;
	}

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
		
		rslt = pdlg->m_VisMod->FilmPF(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),			// ����� ǥ���� ���÷���
			pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),						// �̹��� 1
			pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map5),						// �̹��� 2
			ins_rect, chip_ins_rect, *pdlg->m_VisMod->GetHalInspectRgn(0, i, 1),					//SR region, Chip region, PF Teaching region 
			*pdlg->m_VisMod->GetHalInspectRgn(0, i, 6),//vaccume region
			0, 0, 0, 0, 0, 0, 0, 0,
			m_pf_trsh, m_pf_tol, m_pf_area, m_pf_thsh_nopf, m_pf_tol_nopf,
			&rslt_num,
			&rslt_rgn,
			&rslt_pf_rgn);

		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : PF ����");
		}
	}
	
	return rslt;
}


void CSetUpFilmDlg::SRTest(int type, int direct)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	
	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;
	
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	int thresh, thresh_tol, thresh_w, thresh_w_tol;
	int mold_tol, mold_tol_light;
	int sr_screach_thesh, sr_screach_hi_thesh, sr_screach_tol;

	switch(type)
	{
	case 0:
		switch(direct)
		{
		case 0:
			m_thresh_dark = m_thresh_dark + 2;
			if(m_thresh_dark > 100) m_thresh_dark = 0;	
			break;
		case 1:
			m_thresh_dark = m_thresh_dark - 2;
			if(m_thresh_dark < 0) m_thresh_dark = 0;	
			break;
		}

		//�����迭 �ν� 
		//m_thresh_dark = m_lSrTeachThr;
		thresh = m_thresh_dark;
		thresh_w = 0;

		thresh_tol = 255;
		thresh_w_tol = 255;
		mold_tol = 0;
		mold_tol_light = 10000;

		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 1:
		switch(direct)
		{
		case 0:
			m_thresh_white = m_thresh_white + 2;
			if(m_thresh_white > 100) m_thresh_white = 0;	
			break;
		case 1:
			m_thresh_white = m_thresh_white - 2;
			if(m_thresh_white < 0) m_thresh_white = 0;	
			break;
		}

		//�����迭 �ν�
		//m_thresh_white = m_lSrTeachThr;
		thresh_w = m_thresh_white;
		thresh = 0;

		thresh_tol = 255;
		thresh_w_tol = 255;
		mold_tol = 10000;
		mold_tol_light = 0;

		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 2:

		//��ο�迭 �׽�Ʈ
		thresh = m_thresh_dark;
		thresh_w = 0;
		
		thresh_tol = m_mold_bright_tol;
		thresh_w_tol = 255;
		mold_tol = m_mold_tol;
		mold_tol_light = 10000;
		
		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 3:

		//�����迭 �׽�Ʈ
		thresh_w = m_thresh_white;
		thresh = 0;
		
		thresh_tol = 255;
		thresh_w_tol = m_mold_bright_tol_white;
		mold_tol = 10000;
		mold_tol_light = m_mold_tol_light;
		
		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;	
	}

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//if(pdlg->m_dlgSetup->m_InsPara.m_bCreatFilmVarID[i])
		if(1)
		{
			ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
			chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
			ocv_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];
			
			rslt = pdlg->m_VisMod->FilmMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
				//Mold image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
				//Ink image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),
				//User image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),
				//SR-OUT Scratch image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),
				ins_rect,
				chip_ins_rect,
				ocv_ins_rect,
				ocv_chars_rgn,
				
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),//SR region
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 4),//user region
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 5),//SR out region
				
				pdlg->m_dlgSetup->m_InsPara.m_FilmVarID[i],
				0, 0, 0, 0, 0, 0, 0, 0,
				thresh, thresh_tol, thresh_w, thresh_w_tol, m_mold_fact, mold_tol, mold_tol_light,
				&rslt_num,
				&rslt_rgn,
				&rslt_sr_rgn,
				&rslt_notsr_rng,
				&rslt_user_rgn,
				&rslt_sr_out_rgn);
			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : FILM MOLD");
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
		}
	}

	UpdateData(FALSE);
}

void CSetUpFilmDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		m_mold_fact = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[0];
		m_mold_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[1];
		m_mold_bright_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[2];
		m_pf_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[3];

		m_pf_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4];
		m_pf_area = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5];
		m_mold_tol_light = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[6];

		m_pf_tol_nopf = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[9];
		m_edit_EditErr_threshold = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[10];
		m_edit_EditErr_tolerance = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[11];
		
		m_punchburr_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[12];
		m_punch_mintol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[13];
		m_punch_maxtol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[14];
		m_punch_error_pos = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[15];

		m_pf_thsh_nopf = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[19];
		m_punch_error_pos2 = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[20];
		m_mold_bright_tol_white = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[21];
		m_thresh_dark = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[22];
		m_thresh_white = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[23];
		/*
		m_map0 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[0];
		m_map1 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[1];

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2] = pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0];
		m_map2 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2];

		m_map3 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[3];
		m_map4 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[4];
		m_map5 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[5];
		m_map6 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[6];
		*/

		//20141111 lby ���� �ε����� ���� �� ������
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_map0 = 2;
			m_map1 = 2;
			m_map2 = 2;
			m_map3 = 2;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 1;
			break;
		case VISION_CAM_TOP2:
			m_map0 = 0;
			m_map1 = 0;
			m_map2 = 0;
			m_map3 = 0;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 0;
			break;
		case VISION_CAM_BOTTOM:

			break;
		default:

			break;
		}
		
		
		/*
		map0 �������� : 4
		?map1 PUNCH/USER����: ?
		map2 ��ũ���� : 2
		map3 SR OUT PAT. ���� : 4
		map4 ���� : 5
		map5 ���� : 0
		map6 SR�� �ݼӼ� �̹� : 1
		*/

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[0] = m_mold_fact;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[1] = m_mold_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[2] = m_mold_bright_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[3] = m_pf_tol;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = m_pf_trsh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5] = m_pf_area;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[6] = m_mold_tol_light;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[9] = m_pf_tol_nopf;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[10] = m_edit_EditErr_threshold;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[11] = m_edit_EditErr_tolerance;
		
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[12] = m_punchburr_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[13] = m_punch_mintol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[14] = m_punch_maxtol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[15] = m_punch_error_pos;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[19] = m_pf_thsh_nopf;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[20] = m_punch_error_pos2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[21] = m_mold_bright_tol_white;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[22] = m_thresh_dark;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[23] = m_thresh_white;

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[5] = m_map5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[6] = m_map6;

	}
}
BEGIN_EVENTSINK_MAP(CSetUpFilmDlg, CDialog)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_MINUS, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_TEST2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_MINUS2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_TEST3, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest3, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_TEST7, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmTest7, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_TEST_PUNCH, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmTestPunch, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(0, 0);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(0, 1);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(2, 2);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(1, 0);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(1, 1);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest3()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SRTest(3, 2);	
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmTest7()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	disp_obj(*((CFilmInsDlg *)AfxGetApp()->m_pMainWnd)->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectFilmPF();	
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmTestPunch()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hlong rslt_area = 0;

	UpdateData(TRUE);
	
	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect, dont_care_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;

	BOOL PuchErr;
	Hobject rslt_region;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	int	pf_pitch = (int)((pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1] * 1000.) / pdlg->m_dlgSetup->m_SysPara.m_dPara[0]);

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{


		ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
		dont_care_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];

		rslt = 	pdlg->m_VisMod->FindPuchingChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
													pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), 
													*pdlg->m_VisMod->GetHalInspectRgn(0, i, 0), 
													ins_rect, 
													chip_ins_rect,
													dont_care_rect,
													m_pf_thsh_nopf, m_pf_tol_nopf, pf_pitch,
													m_punch_mintol, m_punch_maxtol, m_punchburr_tol, m_punch_error_pos, m_punch_error_pos2, m_edit_EditErr_threshold, m_edit_EditErr_tolerance,
													pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2], &PuchErr, &rslt_region, &rslt_area); 
		
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("PUNCH, AREA : ") + HTuple(rslt_area));

			if(PuchErr)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 50 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("PUNCH ERR"));
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : NO PUNCH");
		}
			
			
	}
	
	UpdateData(FALSE);
}

void CSetUpFilmDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	
	// TODO: Add your message handler code here
	if(bShow)
	{
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox(); //20110610 lee
		Update(TRUE);
		
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}
}

//20150224 ngh ī�޶� �Ķ���� UI ����
/*
void CSetUpFilmDlg::ShowSetupPara(int CamIdx)
{
	switch(CamIdx)
	{
		case 0:
			GetDlgItem(IDC_STATIC_GROUP_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_POT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_SCR)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_MET)->ShowWindow(TRUE);

			GetDlgItem(IDC_STATIC_FILM1)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM2)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM3)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM4)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM5)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM6)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM7)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM8)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM9)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM10)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM11)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM12)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM13)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM14)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM15)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM16)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM17)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM18)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM19)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM20)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM21)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM22)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_FILM23)->ShowWindow(TRUE);

			GetDlgItem(IDC_EDIT_MOLD_TRSH_DARK)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_WHITE)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_WHITE)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM2)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_FACT_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST2)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK2)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS2)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST3)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST2)->ShowWindow(TRUE);


			GetDlgItem(IDC_STATIC_GROUP_PUNCH)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH1)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH2)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH3)->ShowWindow(TRUE);

			GetDlgItem(IDC_STATIC_PUNCH1)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH2)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH3)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH4)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH5)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH6)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH7)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH8)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH9)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH10)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH11)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH12)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH13)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_PUNCH14)->ShowWindow(TRUE);


			GetDlgItem(IDC_EDIT_PUNCH_MINTOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PUNCH_MAXTOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PUNCHBURR_TOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS2)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PF_TOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM2)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM3)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM3)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_PFEDIT_TOL_FILM)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST_PUNCH)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST7)->ShowWindow(TRUE);

			break;
		case 1:
			GetDlgItem(IDC_STATIC_GROUP_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_POT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_SCR)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_MET)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_FILM1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM14)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM15)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM16)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM17)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM18)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM19)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM20)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM21)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM22)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM23)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_MOLD_TRSH_DARK)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_WHITE)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_WHITE)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_FACT_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST3)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST2)->ShowWindow(FALSE);


			GetDlgItem(IDC_STATIC_GROUP_PUNCH)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH1)->ShowWindow(FALSE);
			//GetDlgItem(IDC_STATIC_GROUP_PUNCH2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH3)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_PUNCH1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH6)->ShowWindow(FALSE);
			//GetDlgItem(IDC_STATIC_PUNCH7)->ShowWindow(FALSE);
			//GetDlgItem(IDC_STATIC_PUNCH8)->ShowWindow(FALSE);
			//GetDlgItem(IDC_STATIC_PUNCH9)->ShowWindow(FALSE);
			//GetDlgItem(IDC_STATIC_PUNCH10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH14)->ShowWindow(FALSE);


			GetDlgItem(IDC_EDIT_PUNCH_MINTOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_MAXTOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCHBURR_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS2)->ShowWindow(FALSE);
			//GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM)->ShowWindow(FALSE);
			//GetDlgItem(IDC_EDIT_PF_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PFEDIT_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST_PUNCH)->ShowWindow(FALSE);
			//GetDlgItem(IDC_BTNENHCTRL_FILM_TEST7)->ShowWindow(FALSE);
			break;
		case 2:
			GetDlgItem(IDC_STATIC_GROUP_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_POT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_SCR)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_FILM_MET)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_FILM1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM14)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM15)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM16)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM17)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM18)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM19)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM20)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM21)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM22)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_FILM23)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_MOLD_TRSH_DARK)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_WHITE)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_WHITE)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_FACT_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_MINUS2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_SR_DARK_TEST3)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST2)->ShowWindow(FALSE);


			GetDlgItem(IDC_STATIC_GROUP_PUNCH)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_PUNCH3)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_PUNCH1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_PUNCH14)->ShowWindow(FALSE);


			GetDlgItem(IDC_EDIT_PUNCH_MINTOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_MAXTOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCHBURR_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PF_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_FILM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_PFEDIT_TOL_FILM)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST_PUNCH)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST7)->ShowWindow(FALSE);
			break;
	}
}
*/