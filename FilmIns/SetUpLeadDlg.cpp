// SetUpLeadDlg.cpp : 구현 파일입니다.
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

#include "SetUpLeadDlg.h"


// CSetUpLeadDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpLeadDlg, CDialog)

CSetUpLeadDlg::CSetUpLeadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpLeadDlg::IDD, pParent)
{

	m_edge_trsh = 20;
	m_edge_fact = 2.0f;
	m_edge_tol = 7;
	m_edge_fact2 = 2.0f;
	m_edge_trsh2 = 20;
	m_map0 = 0;
	m_map1 = 0;
	m_map2 = 0;
	m_map3 = 0;
	m_map4 = 0;
	m_map5 = 0;

	m_edge_tol2 = 0;
	m_dirt_tol = 0;
	m_dirt_thrsh = 0;

	//A/M
	m_edit_AM_threshld_scratch_0 = 0;
	m_edit_AM_tolerance_scratch_0 = 0;
	m_edit_AM_average_bright_0 = 0;
	m_edit_AM_tolerance_bright_0 = 0;

	m_edit_AM_threshld_scratch_1 = 0;
	m_edit_AM_tolerance_scratch_1 = 0;
	m_edit_AM_average_bright_1 = 0;
	m_edit_AM_tolerance_bright_1 = 0;

	m_edit_AM_threshld_scratch_2 = 0;
	m_edit_AM_tolerance_scratch_2 = 0;
	m_edit_AM_average_bright_2 = 0;
	m_edit_AM_tolerance_bright_2 = 0;

	m_edit_AM_threshld_scratch_3 = 0;
	m_edit_AM_tolerance_scratch_3 = 0;
	m_edit_AM_average_bright_3 = 0;
	m_edit_AM_tolerance_bright_3 = 0;
	
	//}}AFX_DATA_INIT
	
	m_iRectIdx = 0;

	m_bShow = FALSE;
}

CSetUpLeadDlg::~CSetUpLeadDlg()
{
}

void CSetUpLeadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EDGE_TRSH_LEAD, m_edge_trsh);
	DDX_Text(pDX, IDC_EDIT_EDGE_FACT_LEAD, m_edge_fact);
	DDX_Text(pDX, IDC_EDIT_EDGE_TOL_LEAD, m_edge_tol);
	DDX_Text(pDX, IDC_EDIT_EDGE_FACT2_LEAD, m_edge_fact2);
	DDX_Text(pDX, IDC_EDIT_EDGE_TRSH2_LEAD, m_edge_trsh2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_LEAD, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_LEAD, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM2_LEAD, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM4_LEAD, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_LEAD, m_map4);
	DDX_Text(pDX, IDC_EDIT_EDGE_TOL2_LEAD, m_edge_tol2);
	DDX_Text(pDX, IDC_EDIT_EDGE_TOL_LEAD_DIRT, m_dirt_tol);
	DDX_Text(pDX, IDC_EDIT_EDGE_TRSH_LEAD_DIRT, m_dirt_thrsh);
	DDX_Text(pDX, IDC_EDIT_MAPNUM6_LEAD, m_map5);

	//AM#1 TOP
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_SCRATCH_0, m_edit_AM_threshld_scratch_0);								//긁힘변색 밝기차
	DDX_Text(pDX, IDC_EDIT_TOL_AM_SCRATCH_0, m_edit_AM_tolerance_scratch_0);										//긁힘변색 허용면적
	DDX_Text(pDX, IDC_EDIT_AVERAGE_AM_DISCOLOR_0, m_edit_AM_average_bright_0);	//전체변색 평균밝기
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_DISCOLOR_0, m_edit_AM_tolerance_bright_0);				//전체변색 밝기차

	//AM#1 BOTTOM
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_SCRATCH_1, m_edit_AM_threshld_scratch_1);
	DDX_Text(pDX, IDC_EDIT_TOL_AM_SCRATCH_1, m_edit_AM_tolerance_scratch_1);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_AM_DISCOLOR_1, m_edit_AM_average_bright_1);
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_DISCOLOR_1, m_edit_AM_tolerance_bright_1);

	//AM#2 TOP
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_SCRATCH_2, m_edit_AM_threshld_scratch_2);
	DDX_Text(pDX, IDC_EDIT_TOL_AM_SCRATCH_2, m_edit_AM_tolerance_scratch_2);	
	DDX_Text(pDX, IDC_EDIT_AVERAGE_AM_DISCOLOR_2, m_edit_AM_average_bright_2);
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_DISCOLOR_2, m_edit_AM_tolerance_bright_2);

	//AM#2 BOTTOM
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_SCRATCH_3, m_edit_AM_threshld_scratch_3);
	DDX_Text(pDX, IDC_EDIT_TOL_AM_SCRATCH_3, m_edit_AM_tolerance_scratch_3);	
	DDX_Text(pDX, IDC_EDIT_AVERAGE_AM_DISCOLOR_3, m_edit_AM_average_bright_3);
	DDX_Text(pDX, IDC_EDIT_TRSH_AM_DISCOLOR_3, m_edit_AM_tolerance_bright_3);

	//20160303 by
	//A/M 파라미터 리미트 설정
	/*
	DDV_MinMaxInt(pDX, m_scratch_tol, 1, 50);
	DDV_MinMaxInt(pDX, m_am_teached_average_light, 170, 230);
	DDV_MinMaxInt(pDX, m_am_insp_average_light, 20, 100);
	DDV_MinMaxInt(pDX, m_scratch_tol_2, 1, 50);
	DDV_MinMaxInt(pDX, m_am_teached_average_light_2, 170, 230);
	DDV_MinMaxInt(pDX, m_am_insp_average_light_2, 20, 100);
	*/

}


BEGIN_MESSAGE_MAP(CSetUpLeadDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpLeadDlg 메시지 처리기입니다.

BOOL CSetUpLeadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	m_tip_ctrl_lead.Create(this);
	m_tip_ctrl_lead.AddTool(GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_0), "A/M 전체 영역의 평균 밝기와 불량 영역의 평균밝기 차이");
	m_tip_ctrl_lead.AddTool(GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_0), "A/M 전체 영역에서 불량 영역이 차지 하는 크기");
	m_tip_ctrl_lead.AddTool(GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_0), "전체 변색 검사 시 필요한 평균 기준 밝기(티칭 시 자동입력)");
	m_tip_ctrl_lead.AddTool(GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_0), "전체 변색 검사 시 평균 기준 밝기 차이");
	m_tip_ctrl_lead.SetMaxTipWidth(300);
	m_tip_ctrl_lead.SetDelayTime(TTDT_AUTOPOP, 200000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH CSetUpLeadDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->GetParentBrush();		

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return hbr;
}
BEGIN_EVENTSINK_MAP(CSetUpLeadDlg, CDialog)
	ON_EVENT(CSetUpLeadDlg, IDC_BTNENHCTRL_LEAD_TEST2, DISPID_CLICK, CSetUpLeadDlg::ClickBtnenhctrlLeadTest2, VTS_NONE)
	ON_EVENT(CSetUpLeadDlg, IDC_BTNENHCTRL_LEAD_DN_TEST, DISPID_CLICK, CSetUpLeadDlg::ClickBtnenhctrlLeadDnTest, VTS_NONE)
	ON_EVENT(CSetUpLeadDlg, IDC_BTNENHCTRL_LEAD_AM_TEST, DISPID_CLICK, CSetUpLeadDlg::ClickBtnenhctrlLeadAmTest, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpLeadDlg::ClickBtnenhctrlLeadTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	InspectUpLead();
}

void CSetUpLeadDlg::ClickBtnenhctrlLeadDnTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);

	InspectDnLead();
}


BOOL CSetUpLeadDlg::InspectDnLead()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	UpdateData(FALSE);

	BOOL rslt = TRUE;
	Hlong rslt_num, rslt_num2;
	double rslt_width_avg;
	Hobject rslt_region, rslt_region2, dn_region, not_sr_rgn;


	gen_empty_obj(&not_sr_rgn);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->InLead(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
					  pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),
					  pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),
					  pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1],//하부
					  not_sr_rgn,
					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 3),//dn lead region
					  0, 0, 0, 0, 0, 0, 0, 0,
					  m_edge_trsh2, m_edge_fact2, m_edge_tol2, 
					  &rslt_width_avg, 
					  &rslt_num, &rslt_region, 
					  &rslt_num2, &rslt_region2, 
					  &dn_region);
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
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : BOTTOM LEAD");
		}
	}

	return rslt;
}

BOOL CSetUpLeadDlg::InspectUpLead()
{
	BOOL rslt = TRUE;

	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(FALSE);


	Hlong rslt_num, rslt_num2;
	Hobject rslt_region, up_region, not_sr_rgn;
	Hobject rslt_region2, up_region2;

	gen_empty_obj(&not_sr_rgn);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	//스캔방향결정
	BOOL scan_type;
	scan_type = FALSE;
	switch(pdlg->m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		scan_type = TRUE;
		break;
	case VISION_CAM_TOP2:
		scan_type = FALSE;
		break;
	case VISION_CAM_BOTTOM:

		break;
	default:
		AfxMessageBox("Error : invalid index(CSetUpLeadDlg::InspectUpLead)!");
		break;
	}

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->OutLead(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
					  pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
					  pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),
					  pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map5),
					  pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0],
					  pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][2],
					  pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][3],

					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 27),//not sr black mold result
					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 2),//up lead region
					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 3),//dn lead region
					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 8),//left lead region
					  *pdlg->m_VisMod->GetHalInspectRgn(0, i, 9),//right lead region
					  0, 0, 0, 0, 0, 0, 0, 0,
					  m_edge_trsh, m_edge_fact, m_edge_tol, 
					  m_dirt_thrsh, m_dirt_tol,
					  pdlg->m_dlgSetup->m_SysPara.m_bPara[20], pdlg->m_dlgSetup->m_SysPara.m_bPara[21], scan_type,
					  &rslt_num, &rslt_region, 
					  &rslt_num2, &rslt_region2, 
					  &up_region);
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
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : TOP LEAD");
		}
	}

	return rslt;
}

void CSetUpLeadDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	UpdateData(TRUE);
	// TODO: Add your message handler code here
	ShowSetupPara(pdlg->m_VisMod->GetVisIndex());

	if(bShow)
	{
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1: // 상부1, 하부 에서는 A/M 검사하지 않는다.
		case VISION_CAM_BOTTOM:
			//GetDlgItem(IDC_STATIC_AM)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_MAPNUM5_LEAD)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_GROUP_AM)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_AM1)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_AM2)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_AM3)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_AM4)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_EDIT_TOL_LEADSCRAT)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_BTNENHCTRL_LEAD_AM_TEST)->ShowWindow(SW_HIDE);
			break;
		}

		
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalRedImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox(); //20110610 lee
		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}
}

void CSetUpLeadDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		m_edge_fact = pdlg->m_dlgSetup->m_InsPara.m_dParaLead[0];
		//m_edge_fact = 5;
		m_edge_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[1];
		m_edge_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[2];

		m_edge_fact2 = pdlg->m_dlgSetup->m_InsPara.m_dParaLead[1];
		m_edge_tol2 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[4];
		m_edge_trsh2 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[5];

		m_dirt_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[6];
		m_dirt_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[7];

		//A/M
		m_edit_AM_tolerance_scratch_0 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[8];	//TOP 긁힘/변색 허용면적률
		m_edit_AM_threshld_scratch_0 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[9];//TOP 긁힘/변색 밝기차
		m_edit_AM_average_bright_0 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[10];//TOP 전체변색 기준평균밝기
		m_edit_AM_tolerance_bright_0 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[11];	//TOP 전체변색 허용밝기차

		m_edit_AM_tolerance_scratch_1 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[12];
		m_edit_AM_threshld_scratch_1 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[13];
		m_edit_AM_average_bright_1 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[14];
		m_edit_AM_tolerance_bright_1 = pdlg->m_dlgSetup->m_InsPara.m_iParaLead[15];

		m_edit_AM_tolerance_scratch_2 = m_edit_AM_tolerance_scratch_0;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[16];
		m_edit_AM_threshld_scratch_2 = m_edit_AM_threshld_scratch_0;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[17];
		m_edit_AM_average_bright_2 = m_edit_AM_average_bright_0;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[18];
		m_edit_AM_tolerance_bright_2 = m_edit_AM_tolerance_bright_0;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[19];

		m_edit_AM_tolerance_scratch_3 = m_edit_AM_tolerance_scratch_1;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[20];
		m_edit_AM_threshld_scratch_3 = m_edit_AM_threshld_scratch_1;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[21];
		m_edit_AM_average_bright_3 = m_edit_AM_average_bright_1;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[22];
		m_edit_AM_tolerance_bright_3 = m_edit_AM_tolerance_bright_1;//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[23];

		/*
		m_map0 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[0];
		m_map1 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[1];
		m_map2 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[2];
		m_map3 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[3];
		m_map4 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[4];
		*/

		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_map0 = 3;
			m_map1 = 3;
			m_map2 = 3;
			m_map3 = 3;
			m_map4 = 3;
			m_map5 = 3;
			break;
		case VISION_CAM_TOP2:
			m_map0 = 0;
			m_map1 = 0;
			m_map2 = 0;
			m_map3 = 0;
			m_map4 = 0;
			m_map5 = 0;
			break;
		case VISION_CAM_BOTTOM:
			m_map0 = 1;
			m_map1 = 1;
			m_map2 = 1;
			m_map3 = 1;
			m_map4 = 1;
			m_map5 = 1;
			break;
		default:

			break;
		}


		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_dParaLead[0] = m_edge_fact;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaLead[0] = 5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[1] = m_edge_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[2] = m_edge_trsh;

		pdlg->m_dlgSetup->m_InsPara.m_dParaLead[1] = m_edge_fact2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[4] = m_edge_tol2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[5] = m_edge_trsh2;

		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[6] = m_dirt_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[7] = m_dirt_thrsh;

		//A/M
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[8] = m_edit_AM_tolerance_scratch_0;	//TOP 긁힘/변색 허용면적률 (1 ~ 50%)
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[9] = m_edit_AM_threshld_scratch_0;//TOP 긁힘/변색 밝기차
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[10] = m_edit_AM_average_bright_0;//TOP 전체변색 기준평균밝기 (180~220)
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[11] = m_edit_AM_tolerance_bright_0;	//TOP 전체변색 허용밝기차	(40~100)

		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[12] = m_edit_AM_tolerance_scratch_1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[13] = m_edit_AM_threshld_scratch_1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[14] = m_edit_AM_average_bright_1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[15] = m_edit_AM_tolerance_bright_1;

		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[16] = m_edit_AM_tolerance_scratch_2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[17] = m_edit_AM_threshld_scratch_2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[18] = m_edit_AM_average_bright_2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[19] = m_edit_AM_tolerance_bright_2;

		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[20] = m_edit_AM_tolerance_scratch_3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[21] = m_edit_AM_threshld_scratch_3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[22] = m_edit_AM_average_bright_3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaLead[23] = m_edit_AM_tolerance_bright_3;


		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[5] = m_map5;
/*
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[0] = 2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[1] = 3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[2] = 2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[3] = 3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapLead[4] = 5;
*/
	}
}

void CSetUpLeadDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];
	
	GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->EnableWindow(state);

	GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_0)->EnableWindow(state);

	GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_1)->EnableWindow(state);

	GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_2)->EnableWindow(state);

	GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_3)->EnableWindow(state);
}


void CSetUpLeadDlg::ClickBtnenhctrlLeadAmTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = FALSE;

	UpdateData(FALSE);

	Hlong rslt_num;
	Hobject rslt_region;

	Hobject rslt_ins_region_0_0;
	Hobject rslt_ins_region_0_1;
	Hobject rslt_ins_region_1_0;
	Hobject rslt_ins_region_1_1;

	gen_empty_obj(&rslt_region);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
			rslt = pdlg->m_VisMod->AMLead(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
								   pdlg->m_VisMod->GetHalRedImage(0, pdlg->m_iBufIdx[0], m_map0),
								   pdlg->m_dlgSetup->m_SysPara.m_bPara[15],
								   pdlg->m_dlgSetup->m_SysPara.m_bPara[16],
								   pdlg->m_dlgSetup->m_HalInsRgn[i][8],
							       pdlg->m_dlgSetup->m_HalInsRgn[i][9],
								   pdlg->m_dlgSetup->m_HalInsRgn[i][10],
								   pdlg->m_dlgSetup->m_HalInsRgn[i][11],
								   0, 0, 0, 0, 0, 0, 0, 0,
								   m_edit_AM_threshld_scratch_0, m_edit_AM_tolerance_scratch_0,
								   m_edit_AM_average_bright_0, m_edit_AM_tolerance_bright_0,
								   m_edit_AM_threshld_scratch_1, m_edit_AM_tolerance_scratch_1,
								   m_edit_AM_average_bright_1, m_edit_AM_tolerance_bright_1,
								   m_edit_AM_threshld_scratch_2, m_edit_AM_tolerance_scratch_2,
								   m_edit_AM_average_bright_2, m_edit_AM_tolerance_bright_2,
								   m_edit_AM_threshld_scratch_3, m_edit_AM_tolerance_scratch_3,
								   m_edit_AM_average_bright_3, m_edit_AM_tolerance_bright_3,
								   &rslt_num, &rslt_region, 
								   &rslt_ins_region_0_0, &rslt_ins_region_0_1, &rslt_ins_region_1_0, &rslt_ins_region_1_1);
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
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : A/M SCRATCH");
		}

		set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
		disp_obj(rslt_ins_region_0_0, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		disp_obj(rslt_ins_region_0_1, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		disp_obj(rslt_ins_region_1_0, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		disp_obj(rslt_ins_region_1_1, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	}
}

//20150224 ngh 카메라별 파라메터 UI 정리
void CSetUpLeadDlg::ShowSetupPara(int CamIdx)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	switch(CamIdx)
	{
		case 0:
			if(pdlg->m_dlgSetup->m_SetUpSysDlg.m_20 && pdlg->m_dlgSetup->m_SetUpSysDlg.m_21)
			{
				GetDlgItem(IDC_STATIC_GROUP_OUTLEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_STATIC_OUTLEAD1)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD2)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD3)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD4)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD5)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD6)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD7)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD8)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD9)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD10)->ShowWindow(TRUE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->ShowWindow(TRUE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_TEST2)->ShowWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_GROUP_OUTLEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_STATIC_OUTLEAD1)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD2)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD3)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD6)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD7)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD8)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD9)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD10)->ShowWindow(FALSE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->ShowWindow(FALSE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_TEST2)->ShowWindow(FALSE);
			}
			
			if(pdlg->m_dlgSetup->m_SetUpSysDlg.m_22 && pdlg->m_dlgSetup->m_SetUpSysDlg.m_23)
			{
				GetDlgItem(IDC_STATIC_GROUP_INLEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_STATIC_INLEAD1)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD2)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD3)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD4)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD5)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD6)->ShowWindow(TRUE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_DN_TEST)->ShowWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_GROUP_INLEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_STATIC_INLEAD1)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD2)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD3)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD6)->ShowWindow(FALSE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_DN_TEST)->ShowWindow(FALSE);
			}

			GetDlgItem(IDC_STATIC_GROUP_AM)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_AM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_AM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_AM1_1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_AM1_2)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_GROUP_TOP)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_TOP2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_BOTTOM)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_BOTTOM2)->ShowWindow(FALSE);
			
			GetDlgItem(IDC_STATIC_AM1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM14)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM15)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM16)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_0)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_0)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_0)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_0)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_SCRATCH_3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TOL_AM_SCRATCH_3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_AVERAGE_AM_DISCOLOR_3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_AM_DISCOLOR_3)->ShowWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_LEAD_AM_TEST)->ShowWindow(FALSE);
			break;
		case 1:
			if(pdlg->m_dlgSetup->m_SetUpSysDlg.m_20 && pdlg->m_dlgSetup->m_SetUpSysDlg.m_21)
			{
				GetDlgItem(IDC_STATIC_GROUP_OUTLEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_STATIC_OUTLEAD1)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD2)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD3)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD4)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD5)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD6)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD7)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD8)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD9)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_OUTLEAD10)->ShowWindow(TRUE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->ShowWindow(TRUE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_TEST2)->ShowWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_GROUP_OUTLEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_STATIC_OUTLEAD1)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD2)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD3)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD6)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD7)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD8)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD9)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_OUTLEAD10)->ShowWindow(FALSE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->ShowWindow(FALSE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_TEST2)->ShowWindow(FALSE);
			}
			
			if(pdlg->m_dlgSetup->m_SetUpSysDlg.m_22 && pdlg->m_dlgSetup->m_SetUpSysDlg.m_23)
			{
				GetDlgItem(IDC_STATIC_GROUP_INLEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_STATIC_INLEAD1)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD2)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD3)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD4)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD5)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_INLEAD6)->ShowWindow(TRUE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->ShowWindow(TRUE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_DN_TEST)->ShowWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_GROUP_INLEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_STATIC_INLEAD1)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD2)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD3)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_INLEAD6)->ShowWindow(FALSE);

				GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->ShowWindow(FALSE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_DN_TEST)->ShowWindow(FALSE);
			}

			if(pdlg->m_dlgSetup->m_SetUpSysDlg.m_15 || pdlg->m_dlgSetup->m_SetUpSysDlg.m_16)
			{
				GetDlgItem(IDC_STATIC_GROUP_AM)->ShowWindow(TRUE);

				GetDlgItem(IDC_STATIC_AM1)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM2)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM3)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM4)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM5)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM6)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM7)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_AM8)->ShowWindow(TRUE);

				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_TOL_LEADSCRAT)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT2)->ShowWindow(TRUE);
				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT3)->ShowWindow(TRUE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_AM_TEST)->ShowWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_GROUP_AM)->ShowWindow(FALSE);

				GetDlgItem(IDC_STATIC_AM1)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM2)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM3)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM6)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM7)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_AM8)->ShowWindow(FALSE);

				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_TOL_LEADSCRAT)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT2)->ShowWindow(FALSE);
				GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT3)->ShowWindow(FALSE);

				GetDlgItem(IDC_BTNENHCTRL_LEAD_AM_TEST)->ShowWindow(FALSE);
			}
			break;
		case 2:
			GetDlgItem(IDC_STATIC_GROUP_OUTLEAD)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_OUTLEAD1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_OUTLEAD10)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_FACT_LEAD)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_TRSH_LEAD_DIRT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_TOL_LEAD_DIRT)->ShowWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_LEAD_TEST2)->ShowWindow(FALSE);
			
			GetDlgItem(IDC_STATIC_GROUP_INLEAD)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_INLEAD1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_INLEAD2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_INLEAD3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_INLEAD4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_INLEAD5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_INLEAD6)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_EDGE_TRSH2_LEAD)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_FACT2_LEAD)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_EDGE_TOL2_LEAD)->ShowWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_LEAD_DN_TEST)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_GROUP_AM)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_AM1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_AM8)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TOL_LEADSCRAT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_TRSH_LEADSCRAT3)->ShowWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_LEAD_AM_TEST)->ShowWindow(FALSE);	
			break;
	}
}

BOOL CSetUpLeadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
		if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	m_tip_ctrl_lead.RelayEvent(pMsg);	//20160517

	return CDialog::PreTranslateMessage(pMsg);
}
