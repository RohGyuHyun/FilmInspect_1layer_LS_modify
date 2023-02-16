// SetUpChipDlg.cpp : ���� �����Դϴ�.
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

#include "SetUpChipDlg.h"


// CSetUpChipDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSetUpChipDlg, CDialog)

CSetUpChipDlg::CSetUpChipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpChipDlg::IDD, pParent)
	, m_map7(0)
	, m_edit_iChipEdgeSize(0)
	, m_pot_line_percent(0)
	, m_pot_width_1(0)
	, m_pot_width_2(0)
	, m_pot_width_3(0)
	, m_pot_width_4(0)
	, m_pot_width_5(0)
	, m_pot_width_6(0)
	, m_pot_average_cnt_x(0)
	, m_pot_average_cnt_y(0)
	, m_out_tol_2(0)
	, m_out_tol_m_2(0)
{
	m_out_tol = 2;
	m_out_trsh = 100;
	m_mold_tol = 4;
	m_map0 = 0;
	m_map1 = 0;
	m_map2 = 0;
	m_map3 = 0;
	m_pot_tol = 30;
	m_pot_trsh = 25;
	m_map4 = 0;
	m_npot_tol = 0;
	m_map5 = 0;
	m_pot_area = 0;
	m_potrange_tol = 0;
	m_broken_thrsh = 0;
	m_map6 = 0;
	m_out_tol_m = 2;
	m_out_trsh_m = 100;

	m_iRectIdx = 0;

	m_bShow = FALSE;

	m_pdlgMotAxisZ = NULL;
}

CSetUpChipDlg::~CSetUpChipDlg()
{
	if(NULL != m_pdlgMotAxisZ)
	{
		delete m_pdlgMotAxisZ;
		m_pdlgMotAxisZ = NULL;
	}

}

void CSetUpChipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_CHIP, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_CHIP, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM2_CHIP, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_CHIP, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_CHIP, m_map4);
	DDX_Text(pDX, IDC_EDIT_MAPNUM6_CHIP, m_map5);
	DDX_Text(pDX, IDC_EDIT_MAPNUM7_CHIP, m_map6);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSH_CHIP, m_out_trsh);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP, m_out_tol);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSH_CHIP_M, m_out_trsh_m);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP_M, m_out_tol_m);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP_M2, m_out_tol_m_2);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSH_CHIP_S, m_out_trsh_s);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP_S, m_out_tol_s);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP_S2, m_out_tol_s_2);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSHHIG_BROKEN, m_broken_thrsh);
	DDX_Text(pDX, IDC_EDIT_MOLD_SIGMA_BROKEN, m_broken_sigma);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSHHIG_CHIP, m_mold_trsh_high);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_CHIP, m_mold_tol);
	DDX_Text(pDX, IDC_EDIT_POT_TRSH_CHIP, m_pot_trsh);
	DDX_Text(pDX, IDC_EDIT_POT_AREA, m_pot_area);
	DDX_Text(pDX, IDC_EDIT_POT_TOL_CHIP, m_pot_tol);
	DDX_Text(pDX, IDC_EDIT_NPOT_TOL_CHIP, m_npot_tol);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_TOL_CHIP, m_potrange_tol);
	DDX_Text(pDX, IDC_EDIT_MAPNUM8_CHIP, m_map7);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSHHIGH_CHIP_M, m_edit_iChipEdgeSize);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_LINE_PERCENT, m_pot_line_percent);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH1, m_pot_width_1);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH2, m_pot_width_2);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH3, m_pot_width_3);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH4, m_pot_width_4);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH5, m_pot_width_5);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_WIDTH6, m_pot_width_6);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_AVERAGE_CNT, m_pot_average_cnt_x);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_AVERAGE_CNT2, m_pot_average_cnt_y);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP2, m_out_tol_2);

	
}


BEGIN_MESSAGE_MAP(CSetUpChipDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpChipDlg �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(CSetUpChipDlg, CDialog)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_FINDCHIP, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipFindchip, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_CHIPOUT, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipChipout, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_MOLD, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipMold, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_POT, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipPot, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpChipDlg::ClickBtnenhctrlChipFindchip()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	//disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	InspectFindChip();
}


BOOL CSetUpChipDlg::InspectFindChip()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	BOOL rslt = TRUE;
	Hobject rslt_region;
	HTuple Mean;
	int i;

	Hobject reduceObj, threObj;
	CString strTemp;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		/*
	20190917 by ���1(�����ķ) / ���2(���ν�ĵ ķ) �б� �߰��ϱ�. 
				���2������ ���� ƼĪ�� ���� �ʴ´�.
		*/
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_BOTTOM:
				rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_region);
				break;
			case VISION_CAM_TOP2:
				rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_region);

				
				break;
		}
		*/
		rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalChipMasterImage(0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_region);


		reduce_domain(*pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0), rslt_region, &reduceObj);
		threshold(reduceObj, &threObj, 180, 255);
		intensity(threObj, reduceObj, &Mean, _);

		set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
		set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].top - 600, pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].left);	
		strTemp.Format("Blue Ch ��չ�� : %d", (Hlong)Mean[0]);
		write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),HTuple(strTemp));

		reduce_domain(*pdlg->m_VisMod->GetHalGreenImage(0, pdlg->m_iBufIdx[0], m_map0), rslt_region, &reduceObj);
		threshold(reduceObj, &threObj, 50, 255);
		intensity(threObj, reduceObj, &Mean, _);

		set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
		set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].top - 300, pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].left);	
		strTemp.Format("Green Ch ��չ�� : %d", (Hlong)Mean[0]);
		write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),HTuple(strTemp));

/*
		if(i == 0)
		{
			HTuple hChipCenterx, hFilmCenterx;
			
			area_center(rslt_region, NULL, &m_hChipCenter, &hChipCenterx);	
			
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter, hChipCenterx, 20, 0);
			
			m_hFilmCenter = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].bottom + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].top) * 0.5;
			
			hFilmCenterx = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].left + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].right) * 0.5;
			
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hFilmCenter, hFilmCenterx, 40, 0);
			
			m_hPunchOffset = m_hFilmCenter - m_hChipCenter;
			
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter + 75, 1000);	
			
			m_lTemp = (double)m_hPunchOffset[0];
			
			m_lstrTemp.Format("%.2f", m_lTemp * pdlg->m_dlgSetup->m_SysPara.m_dPara[0]);
			
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Punch Offset : ") + HTuple(m_lstrTemp)));	
		}
*/

		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}

	return rslt;
}

void CSetUpChipDlg::ClickBtnenhctrlChipChipout()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	long ttime;
	HTuple StartSecond, EndSecond;
	count_seconds(&StartSecond);	


	disp_obj(*pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectChipOut();


	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	ttime = (long)(((double)EndSecond[0]) * 1000);
}

BOOL CSetUpChipDlg::InspectChipOut()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hobject rslt_chip_region, rslt_out_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_BOTTOM:
				rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
				break;
			case VISION_CAM_TOP2:
				rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
				break;
		}

		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			disp_obj(rslt_chip_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
			disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][7], pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			switch(pdlg->m_VisMod->GetVisIndex())
			{
				case VISION_CAM_TOP1:
				case VISION_CAM_BOTTOM:
					rslt = pdlg->m_VisMod->ChipOut(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
										pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),		//Chipout
										rslt_chip_region, pdlg->m_dlgSetup->m_HalInsRgn[i][7],          //chip region
										0,0,0,0,0,0,0,0,
										m_out_trsh, m_out_tol, m_out_tol_2, m_edit_iChipEdgeSize, 
										m_out_trsh_m, m_out_tol_m, m_out_tol_m_2,
										m_out_trsh_s, m_out_tol_s, m_out_tol_s_2,
										&rslt_num, &rslt_out_region);
					break;
				case VISION_CAM_TOP2:
					rslt = pdlg->m_VisMod->ChipOut(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
										pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),		//Chipout
										rslt_chip_region, pdlg->m_dlgSetup->m_HalInsRgn[i][7],          //chip region
										0,0,0,0,0,0,0,0,
										m_out_trsh, m_out_tol, m_out_tol_2, m_edit_iChipEdgeSize, 
										m_out_trsh_m, m_out_tol_m, m_out_tol_m_2,
										m_out_trsh_s, m_out_tol_s, m_out_tol_s_2,
										&rslt_num, &rslt_out_region);
					break;
			}

		
			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
				
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : CHIP OUT");

				disp_obj(rslt_out_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
		
	return rslt;
}


void CSetUpChipDlg::ClickBtnenhctrlChipMold()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	HTuple StartSecond, EndSecond;
	long ttime;
	count_seconds(&StartSecond);	


	disp_obj(*pdlg->m_VisMod->GetHalGreenImage(0, pdlg->m_iBufIdx[0], m_map3), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectChipMold();

	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	ttime = (long)(((double)EndSecond[0]) * 1000);
}


BOOL CSetUpChipDlg::InspectChipMold()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong rslt_broken_num = 0;
	Hobject rslt_chip_region, rslt_mold_region, rslt_broken_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_BOTTOM:
				rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
				break;
			case VISION_CAM_TOP2:
				rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
				break;
		}
		
		if(rslt)
		{

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			switch(pdlg->m_VisMod->GetVisIndex())
			{
				case VISION_CAM_TOP1:
				case VISION_CAM_BOTTOM:
					rslt = pdlg->m_VisMod->ChipMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),   //Ĩ�̸����(Chip�νİ� ���� �̹��� ���)
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),   //Broken
						pdlg->m_dlgSetup->m_HalInsRgn[i][7], rslt_chip_region,
						0,0,0,0,0,0,0,0,
					    m_mold_trsh_high, m_mold_tol, m_broken_thrsh, m_broken_sigma,
						&rslt_num, &rslt_mold_region,
						&rslt_broken_num, &rslt_broken_region);
					break;
				case VISION_CAM_TOP2:
					rslt = pdlg->m_VisMod->ChipMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalGreenImage(0, pdlg->m_iBufIdx[0], m_map0),   //Ĩ�̸����(Chip�νİ� ���� �̹��� ���)
						pdlg->m_VisMod->GetHalGreenImage(0, pdlg->m_iBufIdx[0], m_map0),   //Broken
						pdlg->m_dlgSetup->m_HalInsRgn[i][7], rslt_chip_region,
						0,0,0,0,0,0,0,0,
					    m_mold_trsh_high, m_mold_tol, m_broken_thrsh, m_broken_sigma,
						&rslt_num, &rslt_mold_region,
						&rslt_broken_num, &rslt_broken_region);
					break;
			}

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			disp_obj(rslt_chip_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
			disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][7], pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : CHIP BROKEN/MOLD");

				disp_obj(rslt_mold_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
				disp_obj(rslt_broken_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}

	}
		
	return rslt;
}

void CSetUpChipDlg::ClickBtnenhctrlChipPot()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectPot();
}

BOOL CSetUpChipDlg::InspectPot()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong area = 0;
	Hobject rslt_chip_region, rslt_mold_region, rslt_pot_region[8][6];
	int rslt_pot_average_leng[8][6];
	int rslt_pot_center_leng[8][6];
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			rslt = pdlg->m_VisMod->ChipPot(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),
						   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0], pdlg->m_dlgSetup->m_HalInsRgn[i][7],          //chip region
						   0,0,0,0,0,0,0,0,
						   m_pot_area,
						   m_pot_trsh, m_npot_tol, m_pot_tol, m_pot_line_percent, m_pot_width_1, m_pot_width_2, m_pot_width_3, m_pot_width_4, m_pot_width_5, m_pot_width_6, 
						   m_pot_average_cnt_x, m_pot_average_cnt_y, &rslt_pot_region[i][0], &rslt_num, &area, &m_PotRegion,
						   &rslt_pot_average_leng[i][0], &rslt_pot_center_leng[i][0]);
			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple( HTuple("CHIP POT AREA : ") + HTuple(area) ) );
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple( HTuple("NG : CHIP POT AREA : ") + HTuple(area) ) );
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
		
	return rslt;
}

void CSetUpChipDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];
	
	GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_S)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_S)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_S2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSHHIGH_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_BROKEN)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_AREA)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_NPOT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTRANGE_TOL_CHIP)->EnableWindow(state);
}


BOOL CSetUpChipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	set_draw(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("margin"));

	//CRect rect;
	//GetDlgItem(IDC_STATIC_HIDE_AREA)->GetWindowRect(&rect);

	if( VISION_CAM_TOP2 == pdlg->m_VisMod->GetVisIndex() )
	{
		m_pdlgMotAxisZ = new CSetUpMotAxisZ();
		m_pdlgMotAxisZ->Create(IDD_SETUP_MOTION_AXIS_Z, this);
		//m_pdlgMotAxisZ->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
		m_pdlgMotAxisZ->ShowWindow(SW_HIDE);
	}

	ShowSetupPara(pdlg->m_VisMod->GetVisIndex());

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

HBRUSH CSetUpChipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSetUpChipDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	

	CRect rect;

	if(bShow)
	{
		m_bShow = TRUE;
		ChageStateInspParamInputBox();

		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
				break;
			case VISION_CAM_TOP2:
				disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

				m_pdlgMotAxisZ->ShowWindow(SW_SHOW);
				break;
			case VISION_CAM_BOTTOM:
				disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
				break;
			default:
				AfxMessageBox("Error : invalid index(CSetUpChipDlg::OnShowWindow)!");
				break;
		}

		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		
		if( VISION_CAM_TOP2 == pdlg->m_VisMod->GetVisIndex() )
			m_pdlgMotAxisZ->ShowWindow(SW_HIDE);

		Update(FALSE);
	}
}


void CSetUpChipDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		//Chip out �𼭸�///////////////////////////////
		m_out_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[1];
		m_out_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[0];
		m_out_tol_2 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[30];

		//Chip out �ܺ�///////////////////////////////
		m_out_trsh_s = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[32];
		m_out_tol_s = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[33];
		m_out_tol_s_2 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[34];

		//Chip out �庯///////////////////////////////
		m_out_trsh_m = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[18];
		m_out_tol_m = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[17];
		m_out_tol_m_2 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[31];

		m_edit_iChipEdgeSize = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[2];

		//Chip �̸����//////////////////////////////
		//��������, ������///////////////////////
		m_mold_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[3];
		m_mold_trsh_high = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[5];
		
		m_pot_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[7];
		m_pot_area = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[12];
		m_pot_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[6];
		m_npot_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[8];
		
		m_potrange_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[13];
		m_broken_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[14];
		m_broken_sigma = pdlg->m_dlgSetup->m_InsPara.m_dParaChip[0];

		m_pot_line_percent = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[21];
		
		m_pot_width_1 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[22];
		m_pot_width_2 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[23];
		m_pot_width_3 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[24];
		m_pot_width_4 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[25];
		m_pot_width_5 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[26];
		m_pot_width_6 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[27];
		m_pot_average_cnt_x = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[28];
		m_pot_average_cnt_y = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[29];

		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				m_map0 = 1;//Chip�ν�/����
				m_map1 = 3;//Chipout(ICǥ�� ���)
				m_map2 = 1;//Broken/(ICǥ���� ��ο���� �ߺ��̰�)
				m_map3 = 1;//Ĩ�̸����(��� ��� �ҷ�, ���� IC�鿡�� ��ο� �ҷ�) = broken �����ϼ���
				m_map4 = 1;//Potting
				m_map5 = 1;//Potting�� �����ҷ�
				m_map6 = 1;//Potting�� ��ο��̹�
				m_map7 = 3;//Potting�� ���� �̹�
				break;
			case VISION_CAM_TOP2:
				m_map0 = 0;//Chip�ν�/����
				m_map1 = 0;//Chipout(ICǥ�� ���)
				m_map2 = 0;//Broken/(ICǥ���� ��ο���� �ߺ��̰�)
				m_map3 = 0;//Ĩ�̸����(��� ��� �ҷ�, ���� IC�鿡�� ��ο� �ҷ�) = broken �����ϼ���
				m_map4 = 0;//Potting
				m_map5 = 0;//Potting�� �����ҷ�
				m_map6 = 0;//Potting�� ��ο��̹�
				m_map7 = 0;//Potting�� ���� �̹�

				//IMSI by �Ի� �� ����
				/*
				m_map0 = 1;//Chip�ν�/����
				m_map1 = 1;//Chipout(ICǥ�� ���)
				m_map2 = 1;//Broken/(ICǥ���� ��ο���� �ߺ��̰�)
				m_map3 = 1;//Ĩ�̸����(��� ��� �ҷ�, ���� IC�鿡�� ��ο� �ҷ�) = broken �����ϼ���
				m_map4 = 1;//Potting
				m_map5 = 0;//Potting�� �����ҷ�
				m_map6 = 0;//Potting�� ��ο��̹�
				m_map7 = 0;//Potting�� ���� �̹�
				*/

				
				//Z�� Position	//20190918 by �߰�
				m_pdlgMotAxisZ->SetAxisZPos(AXIS_POS_0, pdlg->m_dlgSetup->m_InsPara.m_iParaChip[38]);
				//m_pdlgMotAxisZ->SetAxisZPos(AXIS_POS_1, pdlg->m_dlgSetup->m_InsPara.m_iParaChip[39]);
				m_pdlgMotAxisZ->UpdatePara(flag);

				break;
		}

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		//��������, ������///////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[6] = m_pot_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[7] = m_pot_trsh; 
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[8] = m_npot_tol;

		//Chip out �𼭸�///////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[0] = m_out_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[1] = m_out_trsh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[30] = m_out_tol_2;

		//Chip out �ܺ�///////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[32] = m_out_trsh_s;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[33] = m_out_tol_s;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[34] = m_out_tol_s_2;

		//Chip out �庯///////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[17] = m_out_tol_m;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[18] = m_out_trsh_m; 
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[31] = m_out_tol_m_2;

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[2] = m_edit_iChipEdgeSize;


		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			
				break;
			case VISION_CAM_TOP2:
				//Z�� Position	//20190918 by �߰�
				pdlg->m_dlgSetup->m_InsPara.m_iParaChip[38] = m_pdlgMotAxisZ->GetAxisZPos(AXIS_POS_0);
				//pdlg->m_dlgSetup->m_InsPara.m_iParaChip[39] = m_pdlgMotAxisZ->GetAxisZPos(AXIS_POS_1);
				break;
		}


		//Chip �̸����//////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[3] = m_mold_tol;
	
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[5] = m_mold_trsh_high;




		if(255 < pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10] = 255;
		}
		if(255 < pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11] = 255;
		}

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[12] = m_pot_area;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[13] = m_potrange_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[14] = m_broken_thrsh;
		pdlg->m_dlgSetup->m_InsPara.m_dParaChip[0] = m_broken_sigma;

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[21] = m_pot_line_percent;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[22] = m_pot_width_1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[23] = m_pot_width_2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[24] = m_pot_width_3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[25] = m_pot_width_4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[26] = m_pot_width_5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[27] = m_pot_width_6;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[28] = m_pot_average_cnt_x;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[29] = m_pot_average_cnt_y;

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[5] = m_map5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[6] = m_map6;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[7] = m_map7;

	}
}

//20150224 ngh ī�޶� �Ķ���� UI ����
void CSetUpChipDlg::ShowSetupPara(int CamIdx)
{
	switch(CamIdx)
	{
		case VISION_CAM_TOP1:
			//Ĩ�ƿ�
			GetDlgItem(IDC_STATIC_CHIP1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MAPNUM1_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MAPNUM2_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MAPNUM3_CHIP)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_GROUP_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_CHIP_OUT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_CHIP_POT)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_S)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_S)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_S2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_M)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSHHIGH_CHIP_M)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_CHIP8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP14)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP15)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP16)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP23)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP24)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP25)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT16)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_EDGE_SIZE)->ShowWindow(FALSE);
			
			GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(FALSE);
		
			//����/���ū
			GetDlgItem(IDC_STATIC_CHIP17)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP18)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP19)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP20)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP21)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP22)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP26)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP27)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_BROKEN)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_SIGMA_BROKEN)->ShowWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_CHIP_MOLD)->ShowWindow(FALSE);
			
			break;
		case VISION_CAM_TOP2:
			GetDlgItem(IDC_STATIC_CHIP4)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MAPNUM5_CHIP)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_GROUP_POTTING)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_POT_RANGE)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_POT_DETAIL)->ShowWindow(FALSE);
			
			GetDlgItem(IDC_EDIT_POTRANGE_LINE_PERCENT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_AVERAGE_CNT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_AVERAGE_CNT2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH1)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH2)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH3)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH4)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH5)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_WIDTH6)->ShowWindow(FALSE);

			GetDlgItem(IDC_EDIT_POT_TRSH_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POT_AREA)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_NPOT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_TOL_CHIP)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_POT1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT18)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT19)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT20)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT21)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT22)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT23)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT24)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT25)->ShowWindow(FALSE);


			GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(FALSE);

			break;
		case VISION_CAM_BOTTOM:
			/*
			GetDlgItem(IDC_STATIC_GROUP_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_CHIP_SEARCH)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_CHIP_OUT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_CHIP_POT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP10)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP11)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP12)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP13)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP14)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP15)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP16)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP17)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP18)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP19)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP20)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP21)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_CHIP22)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_M)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_BROKEN)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_MOLD_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_CHIP_FINDCHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_OUT_TRSHHIGH_CHIP_M)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_EDGE_SIZE)->ShowWindow(FALSE);

			GetDlgItem(IDC_STATIC_GROUP_POTTING)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_NO_POTTING)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_GROUP_POT_RANGE)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT1)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT2)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT3)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT4)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT5)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT6)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT7)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT8)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT9)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_POT10)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POT_TRSH_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POT_AREA)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_NPOT_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_EDIT_POTRANGE_TOL_CHIP)->ShowWindow(FALSE);
			GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(FALSE);
			*/
			break;
		default:
			AfxMessageBox("Error : invalid index(CSetUpBsaeDlg::OnInitDialog)!");
			break;
	}
}

void CSetUpChipDlg::AxisZMovePos(int nIdx, int nVal)
{
	switch(nIdx)
	{
	case 0:	//�� ����
		m_pdlgMotAxisZ->AxisMovePos(AXIS_POS_0, nVal);
		break;
	case 1:

		break;
	}
	
}

void CSetUpChipDlg::ClickBtnenhctrlChipPotTeach()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong area = 0;
	Hobject rslt_chip_region, rslt_mold_region, ChipRgn, rslt_pot_region[8][6];
	int i;
	int rslt_pot_average_leng[8][6];
	int rslt_pot_center_leng[8][6];
	HTuple Min;
	Hlong min;

	UpdateData(TRUE);

	//ù��°�͸� ������
	//Find suitable threshold
	CRect rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[0][0];
	
	gen_rectangle1(&ChipRgn, rect.top, rect.left, rect.bottom, rect.right);

	min_max_gray(ChipRgn, *pdlg->m_VisMod->GetHalChipMasterImage(0), HTuple(0), &Min, _, _);
	min = (Hlong)Min[0];
	min = min + 35;

	if( (0 > min) || (255 < min) )
	{
		min = 70;
	}

	disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalChipMasterImage(0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);

/*
		if(i == 0)
		{
			HTuple hChipCenterx, hFilmCenterx;
			area_center(rslt_chip_region, NULL, &m_hChipCenter, &hChipCenterx);	
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter, hChipCenterx, 20, 0);
			m_hFilmCenter = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].bottom + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].top) * 0.5;
			hFilmCenterx = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].left + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].right) * 0.5;
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hFilmCenter, hFilmCenterx, 40, 0);
			m_hPunchOffset = m_hFilmCenter - m_hChipCenter;
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter + 75, 1000);	
			m_lTemp = (double)m_hPunchOffset[0];
			m_lstrTemp.Format("%.2f", m_lTemp * pdlg->m_dlgSetup->m_SysPara.m_dPara[0]);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Punch Offset : ") + HTuple(m_lstrTemp)));	
		}
*/
		if(rslt)
		{
			//Set chip region
			copy_obj(rslt_chip_region, &pdlg->m_dlgSetup->m_HalInsRgn[i][7], 1, -1);

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");
			
			switch(pdlg->m_VisMod->GetVisIndex())
			{
				case VISION_CAM_TOP1:
					rslt = pdlg->m_VisMod->ChipPot(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0], pdlg->m_dlgSetup->m_HalInsRgn[i][7],          //chip region
									0,0,0,0,0,0,0,0,
								   m_pot_area,
								   m_pot_trsh, m_npot_tol, m_pot_tol, m_pot_line_percent, m_pot_width_1, m_pot_width_2, m_pot_width_3, m_pot_width_4, m_pot_width_5, m_pot_width_6, 
								   m_pot_average_cnt_x, m_pot_average_cnt_y, &rslt_pot_region[i][0], &rslt_num, &area, &m_PotRegion,
								   &rslt_pot_average_leng[i][0], &rslt_pot_center_leng[i][0]);

					HTuple Temp1, Temp2, Temp3, Temp4;
					smallest_rectangle1(rslt_chip_region,&Temp1,&Temp2,&Temp3,&Temp4);


					if(rslt)
					{
						m_pot_area = area;
					}
					else
					{
						m_pot_area = area;
					}
					break;
			}
			
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
	
	UpdateData(FALSE);
}
