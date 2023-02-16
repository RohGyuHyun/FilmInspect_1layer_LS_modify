// DialogMainSetup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmInsPunch.h"

#include "DialogMainSetup.h"

#include "AreaCamControl.h"
#include "UserSockClient.h"
#include "com_class.h"


#include "FilmInsPunchDlg.h"

// CDialogMainSetup 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogMainSetup, CDialog)

CDialogMainSetup::CDialogMainSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMainSetup::IDD, pParent)
	, m_img0_ch0(0)
	, m_thrsh(0)

	, m_score(0)
	, m_poslimit(0)
	, m_ppunch_minarea(0)
	, m_ppunch_maxarea(0)
	, m_ppunchbur_tol(0)
	, m_ppunch_xtol(0)
	, m_ppunch_ytol(0)
	, m_ppunch_limit(0)
	, m_ppunch_width(0)
	, m_ppunch_height(0)
	, m_dPunch_Size_Tol(0)
	, m_edit_nChipThreshold(0)
	, m_edit_dChipWidth(0)
	, m_edit_dChipHeight(0)
	, m_edit_dChipTolerance(0)
	, m_edit_nChipCX(0)
	, m_edit_nChipCY(0)
	, m_dPunch_Size_Tol2(0)
	, m_edit_nSerialPort(0)
{
	m_iIndex = 0;
}

CDialogMainSetup::~CDialogMainSetup()
{
}

void CDialogMainSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_cChSliderCtrl);
	DDX_Text(pDX, IDC_EDIT_LIGHT, m_img0_ch0);
	DDX_Text(pDX, IDC_EDIT_TRSH_ALIGN, m_thrsh);
	DDX_Text(pDX, IDC_EDIT_SCORE_ALIGN, m_score);
	DDX_Text(pDX, IDC_EDIT_LIMIT_ALIGN, m_poslimit);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_MINTOL_FILM, m_ppunch_minarea);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_MAXTOL_FILM, m_ppunch_maxarea);
	DDX_Text(pDX, IDC_EDIT_PPUNCHBURR_TOL_FILM, m_ppunchbur_tol);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_XTOL, m_ppunch_xtol);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_YTOL, m_ppunch_ytol);
	DDX_Text(pDX, IDC_EDIT_LIMIT_PUNCH, m_ppunch_limit);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_WIDTH, m_ppunch_width);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_HEIGHT, m_ppunch_height);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_SIZE_TOL, m_dPunch_Size_Tol);
	DDX_Text(pDX, IDC_EDIT_CHIP_BRIGHT, m_edit_nChipThreshold);
	DDV_MinMaxInt(pDX, m_edit_nChipThreshold, 0, 255);
	DDX_Text(pDX, IDC_EDIT_CHIP_WIDTH, m_edit_dChipWidth);
	DDX_Text(pDX, IDC_EDIT_CHIP_HEIGHT, m_edit_dChipHeight);
	DDX_Text(pDX, IDC_EDIT_CHIP_TOL, m_edit_dChipTolerance);
	DDX_Text(pDX, IDC_EDIT_CHIP_CX, m_edit_nChipCX);
	DDX_Text(pDX, IDC_EDIT_CHIP_CY, m_edit_nChipCY);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_SIZE_TOL2, m_dPunch_Size_Tol2);
	DDX_Text(pDX, IDC_EDIT3, m_edit_nSerialPort);
	DDV_MinMaxInt(pDX, m_edit_nSerialPort, 1, 15);
}


BEGIN_MESSAGE_MAP(CDialogMainSetup, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CDialogMainSetup::OnNMCustomdrawSlider1)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogMainSetup 메시지 처리기입니다.

BOOL CDialogMainSetup::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;
	
	m_InsPara = pdlg->GetInspectPara();
	m_SysPara = pdlg->GetSystemPara();

	SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE); 
		
	m_cChSliderCtrl.SetRange(0,255);
	m_img0_ch0 = m_InsPara.m_iParaBase[0];

 
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
BEGIN_EVENTSINK_MAP(CDialogMainSetup, CDialog)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_SETUP_OK, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlSetupOk, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_SETUP_CANCEL, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlSetupApply, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_ALIGN_TEACH, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlAlignTeach, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_ALIGN_POSITIONSET, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlAlignPositionset, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_ALIGN_TEST, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlAlignTest, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_ALIGN_POSITIONSET2, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlAlignPositionset2, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_ALIGN_TEST2, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlAlignTest2, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_CHIP_TEACH, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlChipTeach, VTS_NONE)
	ON_EVENT(CDialogMainSetup, IDC_BTNENHCTRL_CHIP_TEST, DISPID_CLICK, CDialogMainSetup::ClickBtnenhctrlChipTest, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogMainSetup::ClickBtnenhctrlSetupOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	ClickBtnenhctrlSetupApply(); // apply

	pdlg->ModelSave();

	pdlg->m_btnSetup.SetValue(FALSE);

	ShowWindow(SW_HIDE);
}

//APPLY SAVE
void CDialogMainSetup::ClickBtnenhctrlSetupApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	Update(FALSE);

	//Set data
	pdlg->SetInspectPara(m_InsPara);
	pdlg->SetSystemPara(m_SysPara);
}

void CDialogMainSetup::SetRegion(int idx)		// 110829 ytlee		region 저장 관련 Tracker 유무에 따른 순서 꼬임 방지
{
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;
	
	CRect rect;
	long left, top, right, bottom;

	switch(idx)
	{
	case 0:
		//Align search
		pdlg->m_dlgImgRect->GetTracker(0, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
					
		m_InsPara.m_InsRegionAlign[0] = rect;
		//END
		
		break;
	case 1:
		//Align mark
		pdlg->m_dlgImgRect->GetTracker(1, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
					
		m_InsPara.m_InsRegionAlign[1] = rect;
		break;
	case 2:
		//Chip search	
		pdlg->m_dlgImgRect->GetTracker(2, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;

		m_InsPara.m_InsRegionChip[0] = rect;
	
		break;
	case 3:
		//Punch search	
		pdlg->m_dlgImgRect->GetTracker(3, &left, &top, &right, &bottom);		
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;

		m_InsPara.m_InsRegionPunch[0] = rect;
	
		break;

	default:
		AfxMessageBox("Error : invalid index!");
		break;
	}

}

void CDialogMainSetup::Update(BOOL flag)
{
 	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;
	
	m_InsPara = pdlg->GetInspectPara();

	CRect rect;

	if(flag)
	{
		pdlg->m_dlgImgRect->DeleteAll();
		pdlg->m_dlgImgRect->SetActiveTracker(TRUE);
	
		//Alignment//////////////////////////////
		//Align search
		rect = m_InsPara.m_InsRegionAlign[0];
		pdlg->m_dlgImgRect->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 255), "ALIGN-SEARCH");
		pdlg->m_dlgImgRect->Display();

		//Align mark
		rect = m_InsPara.m_InsRegionAlign[1];
		pdlg->m_dlgImgRect->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 0, 255), "ALIGN-MARK REGION");
		pdlg->m_dlgImgRect->Display();


		//Chip///////////////////////////////////
		rect = m_InsPara.m_InsRegionChip[0];
		pdlg->m_dlgImgRect->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 255), "CHIP-SEARCH");
		pdlg->m_dlgImgRect->Display();


		//Punch///////////////////////////////////
		rect = m_InsPara.m_InsRegionPunch[0];
		pdlg->m_dlgImgRect->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 255), "PUNCH-SEARCH");
		pdlg->m_dlgImgRect->Display();
	

		//Parameters
		m_thrsh = m_InsPara.m_iParaAlign[0];
		m_score = m_InsPara.m_iParaAlign[1];
		m_poslimit = m_InsPara.m_iParaAlign[2];

		m_cChSliderCtrl.SetPos(m_InsPara.m_iParaBase[0]);

		m_ppunch_minarea = m_InsPara.m_iParaPunch[0];		// 펀치 면적 최소
		m_ppunch_maxarea = m_InsPara.m_iParaPunch[1];		// 펀치 면적 최대
		m_ppunchbur_tol = m_InsPara.m_iParaPunch[2];		// 펀치 버 허용치
		m_ppunch_xtol = m_InsPara.m_iParaPunch[3];			// 펀치 중심 x
		m_ppunch_ytol = m_InsPara.m_iParaPunch[4];			// 펀치 중심 y
		

		m_ppunch_width = m_InsPara.m_dParaPunch[0];			// 펀치 가로 길이
		m_ppunch_height = m_InsPara.m_dParaPunch[1];		// 세로 세로 길이
		m_dPunch_Size_Tol = m_InsPara.m_dParaPunch[2];		// 펀치 가로세로 허용 오차
		m_dPunch_Size_Tol2 = m_InsPara.m_dParaPunch[3];		// 펀치 가로세로 허용 오차
		m_ppunch_limit = m_InsPara.m_dParaPunch[4];			// 펀치 얼라인 간 허용 거리

		m_edit_nChipThreshold = m_InsPara.m_iParaChip[0];

		m_edit_nChipCX = m_InsPara.m_dParaChip[1];		//칩 센터 x
		m_edit_nChipCY = m_InsPara.m_dParaChip[2];		//칩 센터 y
		m_edit_dChipWidth = m_InsPara.m_dParaChip[3];		//칩 가로 사이즈
		m_edit_dChipHeight = m_InsPara.m_dParaChip[4];	//칩 세로 사이즈
		m_edit_dChipTolerance = m_InsPara.m_dParaChip[5];	//칩 위치 허용오차 (mm)

		CheckDlgButton(IDC_CHECK_SAVE_NG, m_SysPara.m_bParaSys[0]);
		CheckDlgButton(IDC_CHECK_SAVE_GOOD, m_SysPara.m_bParaSys[1]);
		m_edit_nSerialPort = m_SysPara.m_iParaSys[0];

		UpdateData(FALSE);
	
	}
	else
	{
		UpdateData(TRUE);

		//Parameters
		m_InsPara.m_iParaAlign[0] = m_thrsh;
		m_InsPara.m_iParaAlign[1] = m_score;
		m_InsPara.m_iParaAlign[2] = m_poslimit; 

		m_InsPara.m_iParaBase[0] = m_img0_ch0;

		m_InsPara.m_iParaPunch[0] = m_ppunch_minarea;
		m_InsPara.m_iParaPunch[1] = m_ppunch_maxarea;
		m_InsPara.m_iParaPunch[2] = m_ppunchbur_tol;
		m_InsPara.m_iParaPunch[3] = m_ppunch_xtol;
		m_InsPara.m_iParaPunch[4] = m_ppunch_ytol;
		

		m_InsPara.m_iParaChip[0] = m_edit_nChipThreshold;

		m_InsPara.m_dParaChip[1] = m_edit_nChipCX;
		m_InsPara.m_dParaChip[2] = m_edit_nChipCY;
		m_InsPara.m_dParaChip[3] = m_edit_dChipWidth;
		m_InsPara.m_dParaChip[4] = m_edit_dChipHeight;
		m_InsPara.m_dParaChip[5] = m_edit_dChipTolerance;	

		m_InsPara.m_dParaPunch[0] = m_ppunch_width;
		m_InsPara.m_dParaPunch[1] = m_ppunch_height;
		m_InsPara.m_dParaPunch[2] = m_dPunch_Size_Tol;
		m_InsPara.m_dParaPunch[3] = m_dPunch_Size_Tol2;
		m_InsPara.m_dParaPunch[4] = m_ppunch_limit;

		m_SysPara.m_bParaSys[0] = IsDlgButtonChecked(IDC_CHECK_SAVE_NG);
		m_SysPara.m_bParaSys[1] = IsDlgButtonChecked(IDC_CHECK_SAVE_GOOD);
		m_SysPara.m_iParaSys[0] = m_edit_nSerialPort;

		if(IsWindowVisible())	
		{
			//Align search
			SetRegion(0);
			//Align Mark
			SetRegion(1);
			//Chip
			SetRegion(2);
			//Punch
			SetRegion(3);
		}
	}

}
void CDialogMainSetup::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	if(bShow)
	{
		//disp_obj(*pdlg->GetHalMasterImage(), pdlg->m_dlgImgRect->GetView());

		Update(TRUE);
	}
	else
	{
		Update(FALSE);

		pdlg->m_dlgImgRect->DeleteAll();
	}
}

void CDialogMainSetup::ClickBtnenhctrlAlignTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	//Align search
	SetRegion(0);
	//Align mark
	SetRegion(1);

	Update(FALSE);

	Hobject image, modimage, shape;
	Hlong wd, ht;
	
	//Teach
	disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());
		
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_05!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}	

	Hobject EmphaImage;
	emphasize(*pdlg->GetHalImage(), &EmphaImage, 15, 15, 3);

	if(
		Hal_ModelTeachingRect1(pdlg->m_dlgImgRect->GetView(), 
		&EmphaImage, 
		&m_InsPara.m_InsRegionAlign[1],
		wd,
		ht,
		m_InsPara.m_PatParaAlign, 
		m_thrsh, //threshold
		m_thrsh * 0.5, //min threshold
		&m_InsPara.m_dParaAlign[0],//pattern cx 
		&m_InsPara.m_dParaAlign[1],//pattern cy
		&modimage, &m_InsPara.m_PatIDAlign)		
		)
	{
		m_InsPara.m_bCreatMdIDAlign = TRUE;

		pdlg->SetInspectPara(m_InsPara);
	}
	else
	{
		m_InsPara.m_bCreatMdIDAlign = FALSE;
	}
}

void CDialogMainSetup::ClickBtnenhctrlAlignPositionset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	Update(FALSE);

	Hobject modimage, shape, Region, ImageReduced;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	double pat_score;
	Hobject EmphaImage;
	CRect rect, rect2;

	pat_score = (((double)m_score) * 0.01);
	
	disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_06!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}	
	
	rn = 0;

	// BEGIN
	rect = m_InsPara.m_InsRegionAlign[0];
	gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

	reduce_domain(*pdlg->GetHalImage(),Region,&ImageReduced);
	emphasize(ImageReduced, &EmphaImage, 15, 15, 3);
	
	if(m_InsPara.m_bCreatMdIDAlign)
	{
		Hal_ShapePatternFind(pdlg->m_dlgImgRect->GetView(), 
			&EmphaImage, 
			m_InsPara.m_PatIDAlign, 
			m_InsPara.m_PatParaAlign, 
			m_InsPara.m_InsRegionAlign[1],
			wd, ht,
			m_InsPara.m_dParaAlign[0],// pattern cx #1 or #2
			m_InsPara.m_dParaAlign[1],// pattern cy #1 or #2
			pat_score,//Score
			&modimage, 
			&shape, 
			&rx, 
			&ry, 
			&ra, 
			&rs, 
			&rscore, 
			&rn);

		if(rn)
		{
			set_color(pdlg->m_dlgImgRect->GetView(), "green");
			set_tposition(pdlg->m_dlgImgRect->GetView(), 200, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SUCCSS : Align OK !")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 20 + (200), 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SCORE : ") + HTuple(rscore * 100)));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 35 + (200), 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("X : ") + HTuple(rx)));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 50 + (200), 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("Y : ") + HTuple(ry)));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 65 + (200), 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("ANGLE : ") + HTuple(ra)));

			//Position set
			m_InsPara.m_dPatternPosXAlign = rx;
			m_InsPara.m_dPatternPosYAlign = ry;

			pdlg->SetInspectPara(m_InsPara);
		}
		else
		{
			set_color(pdlg->m_dlgImgRect->GetView(), "red");
			set_tposition(pdlg->m_dlgImgRect->GetView(), 200, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : can't find model!")));
		}
	}
	else
	{
		set_color(pdlg->m_dlgImgRect->GetView(), "red");
		set_tposition(pdlg->m_dlgImgRect->GetView(), 200, 10);
		write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : no pattern!")));
	}

}

void CDialogMainSetup::ClickBtnenhctrlAlignTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	Update(FALSE);

	pdlg->InspectionAlign();

}

HBRUSH CDialogMainSetup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->m_BrushBlack;
}

void CDialogMainSetup::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;


	char tmp[50];

	sprintf(tmp, "%d", m_cChSliderCtrl.GetPos());
	GetDlgItem(IDC_EDIT_LIGHT)->SetWindowText(tmp);
	m_img0_ch0 = m_cChSliderCtrl.GetPos();

	pdlg->SetLight(LIGHT_CH_INDEX, m_img0_ch0);

	*pResult = 0;
}


void CDialogMainSetup::ClickBtnenhctrlAlignPositionset2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;
	//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Range, Num;

	Hlong wd, ht;

	long punch_area;
	HTuple Area, punch_x, punch_y;
	HTuple Row0, Col0, Row1, Col1;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{

	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return ;
	}

	disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());

	gen_rectangle1(&Region, m_InsPara.m_InsRegionChip[0].top, 
		                    m_InsPara.m_InsRegionChip[0].left, 
							m_InsPara.m_InsRegionChip[0].bottom, 
							m_InsPara.m_InsRegionChip[0].right);

	//Punching inspection////////////////////////
	reduce_domain(*pdlg->GetHalImage(),Region,&ImageReduced);

	if(pdlg->m_dlgImgRect->GetView())
	{
		set_color(pdlg->m_dlgImgRect->GetView(), HTuple("yellow"));
		disp_obj(Region, pdlg->m_dlgImgRect->GetView());
	}

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(10)); //200~255 사이만 펀칭으로 인정
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else      
	{
		status.Format("Halcon Error Occured at threshold_20!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(m_ppunch_minarea),HTuple(m_ppunch_maxarea));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		set_color(pdlg->m_dlgImgRect->GetView(), "red");
		set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
		write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : No Chip !")));
		return;
	}
	else
	{
		//Punch 면적, 센터 좌표
		area_center(SelectedRegions, &Area, &punch_y, &punch_x);
		punch_area = (Hlong)Area[0];					//펀치 면적 픽셀
		m_ppunch_xtol = (Hlong)punch_x[0];				//펀치 센터 x
		m_ppunch_ytol = (Hlong)punch_y[0];				//펀치 센터 y
/*
		//Punch 가로세로 사이즈
		//Row0 : top, Col0 : left, Row1 : bottom, Col1 : right
		smallest_rectangle1(SelectedRegions, &Row0, &Col0, &Row1, &Col1);
		m_ppunch_width = (Hlong)Col1[0] - (Hlong)Col0[0];
		m_ppunch_height = (Hlong)Row1[0] - (Hlong)Row0[0];
		m_ppunch_width = (m_ppunch_width * PIXEL_SIZE) / 1000;
		m_ppunch_height = (m_ppunch_height * PIXEL_SIZE) / 1000;
*/

		//결과 디스플레이
		if(pdlg->m_dlgImgRect->GetView())
		{
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SUCCESS : Chip OK!")));
			/*
			set_tposition(pdlg->m_dlgImgRect->GetView(), 420, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("AREA : ") + (Hlong)Area[0]) + HTuple(" pixel"));
			*/
			set_tposition(pdlg->m_dlgImgRect->GetView(), 435, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER x : ") + (Hlong)punch_x[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 450, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER y : ") + (Hlong)punch_y[0]));
			
			/*
			set_tposition(pdlg->m_dlgImgRect->GetView(), 465, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("WIDTH : ") + HTuple(m_ppunch_width) + HTuple(" mm")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 480, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("HEIGHT : ") + HTuple(m_ppunch_height) + HTuple(" mm")));
			*/

			//펀칭 중심에 크로스 그리기
			disp_line(pdlg->m_dlgImgRect->GetView(), punch_y, punch_x - 25, punch_y, punch_x + 25);
			disp_line(pdlg->m_dlgImgRect->GetView(), punch_y - 25, punch_x, punch_y + 25, punch_x);
			
			disp_obj(SelectedRegions, pdlg->m_dlgImgRect->GetView());
		}

		UpdateData(FALSE);
	}

}

/*
void CDialogMainSetup::ClickBtnenhctrlAlignPositionset2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;
	//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Range, Num;

	Hlong wd, ht;

	long punch_area;
	HTuple Area, punch_x, punch_y;
	HTuple Row0, Col0, Row1, Col1;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{

	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return ;
	}

	disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());

	gen_rectangle1(&Region, m_InsPara.m_InsRegionChip[0].top, 
		                    m_InsPara.m_InsRegionChip[0].left, 
							m_InsPara.m_InsRegionChip[0].bottom, 
							m_InsPara.m_InsRegionChip[0].right);

	//Punching inspection////////////////////////
	reduce_domain(*pdlg->GetHalImage(),Region,&ImageReduced);

	if(pdlg->m_dlgImgRect->GetView())
	{
		set_color(pdlg->m_dlgImgRect->GetView(), HTuple("yellow"));
		disp_obj(Region, pdlg->m_dlgImgRect->GetView());
	}

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(200),HTuple(255)); //200~255 사이만 펀칭으로 인정
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else      
	{
		status.Format("Halcon Error Occured at threshold_20!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(m_ppunch_minarea),HTuple(m_ppunch_maxarea));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		set_color(pdlg->m_dlgImgRect->GetView(), "red");
		set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
		write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : No Punch !")));
		return;
	}
	else
	{
		//Punch 면적, 센터 좌표
		area_center(SelectedRegions, &Area, &punch_y, &punch_x);
		punch_area = (Hlong)Area[0];					//펀치 면적 픽셀
		m_ppunch_xtol = (Hlong)punch_x[0];				//펀치 센터 x
		m_ppunch_ytol = (Hlong)punch_y[0];				//펀치 센터 y

		//Punch 가로세로 사이즈
		//Row0 : top, Col0 : left, Row1 : bottom, Col1 : right
		smallest_rectangle1(SelectedRegions, &Row0, &Col0, &Row1, &Col1);
		m_ppunch_width = (Hlong)Col1[0] - (Hlong)Col0[0];
		m_ppunch_height = (Hlong)Row1[0] - (Hlong)Row0[0];
		m_ppunch_width = (m_ppunch_width * PIXEL_SIZE) / 1000;
		m_ppunch_height = (m_ppunch_height * PIXEL_SIZE) / 1000;


		//결과 디스플레이
		if(pdlg->m_dlgImgRect->GetView())
		{
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SUCCESS : Punch OK!")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 420, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("AREA : ") + (Hlong)Area[0]) + HTuple(" pixel"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 435, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER x : ") + (Hlong)punch_x[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 450, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER y : ") + (Hlong)punch_y[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 465, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("WIDTH : ") + HTuple(m_ppunch_width) + HTuple(" mm")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 480, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("HEIGHT : ") + HTuple(m_ppunch_height) + HTuple(" mm")));

			//펀칭 중심에 크로스 그리기
			disp_line(pdlg->m_dlgImgRect->GetView(), punch_y, punch_x - 25, punch_y, punch_x + 25);
			disp_line(pdlg->m_dlgImgRect->GetView(), punch_y - 25, punch_x, punch_y + 25, punch_x);
			
			disp_obj(SelectedRegions, pdlg->m_dlgImgRect->GetView());
		}

		UpdateData(FALSE);
	}

}
*/

void CDialogMainSetup::ClickBtnenhctrlAlignTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	Hobject rslt_rgn;

	pdlg->FindPPuchingChip(pdlg->m_dlgImgRect->GetView(), *pdlg->GetHalImage(), m_InsPara.m_InsRegionPunch[0], &rslt_rgn);
}

void CDialogMainSetup::ClickBtnenhctrlChipTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//저장해야할 정보 칩 크기, 칩 센터좌표
	UpdateData(TRUE);

	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Num;
	HTuple Area, chip_cx, chip_cy;
	HTuple Row0, Col0, Row1, Col1;

	Hlong wd, ht;

	double chip_width, chip_height;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{

	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return ;
	}

	//disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());

	gen_rectangle1(&Region, m_InsPara.m_InsRegionChip[0].top, 
		                    m_InsPara.m_InsRegionChip[0].left, 
							m_InsPara.m_InsRegionChip[0].bottom, 
							m_InsPara.m_InsRegionChip[0].right);

	reduce_domain(*pdlg->GetHalImage(),Region,&ImageReduced);


	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(m_edit_nChipThreshold));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else      
	{
		status.Format("Halcon Error Occured at threshold_20!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}

	connection(Region,&ConnectedRegions);

	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(0),HTuple(m_InsPara.m_InsRegionChip[0].Width() * m_InsPara.m_InsRegionChip[0].Height()));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다?
	{
		set_color(pdlg->m_dlgImgRect->GetView(), "red");
		set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
		write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : Can't find the chp !")));

		return;
	}
	else
	{
		//칩 면적, 센터 좌표
		area_center(SelectedRegions, &Area, &chip_cx, &chip_cy);
		m_InsPara.m_dParaChip[0] = (Hlong)Area[0];					//면적 픽셀
		m_edit_nChipCX = (Hlong)chip_cy[0];				//센터 x
		m_edit_nChipCY = (Hlong)chip_cx[0];				//센터 y

		//칩 사이즈
		//Row0 : top, Col0 : left, Row1 : bottom, Col1 : right
		smallest_rectangle1(SelectedRegions, &Row0, &Col0, &Row1, &Col1);
		
		chip_width = (Hlong)Col1[0] - (Hlong)Col0[0];
		m_edit_dChipWidth = ( chip_width * PIXEL_SIZE) / 1000;			// um -> mm
		chip_height = (Hlong)Row1[0] - (Hlong)Row0[0];
		m_edit_dChipHeight = ( chip_height * PIXEL_SIZE) / 1000;		// um -> mm

		//결과 디스플레이
		if(pdlg->m_dlgImgRect->GetView())
		{
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SUCCESS : Chip teaching OK!")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 420, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("AREA : ") + (Hlong)Area[0]) + HTuple(" pixel"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 435, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER x : ") + (Hlong)chip_cy[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 450, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER y : ") + (Hlong)chip_cx[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 465, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("WIDTH : ") + ( chip_width * PIXEL_SIZE) / 1000 + HTuple(" mm")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 480, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("HEIGHT : ") + ( chip_height * PIXEL_SIZE) / 1000 + HTuple(" mm")));

			//펀칭 중심에 크로스 그리기
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("blue"));
			disp_line(pdlg->m_dlgImgRect->GetView(), (Hlong)chip_cx[0], (Hlong)chip_cy[0] - 25, (Hlong)chip_cx[0], (Hlong)chip_cy[0] + 25);
			disp_line(pdlg->m_dlgImgRect->GetView(), (Hlong)chip_cx[0] - 25, (Hlong)chip_cy[0], (Hlong)chip_cx[0] + 25, (Hlong)chip_cy[0]);
			
			disp_obj(SelectedRegions, pdlg->m_dlgImgRect->GetView());
		}
	}
	
	UpdateData(FALSE);
}

void CDialogMainSetup::ClickBtnenhctrlChipTest()
{
	/*
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//티칭 시 칩 정보와 대조하여 양불판정
		UpdateData(TRUE);

	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)AfxGetApp()->m_pMainWnd;

	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Num;
	HTuple Area, chip_cx, chip_cy;
	HTuple Row0, Col0, Row1, Col1;

	Hlong wd, ht;

	long lChip_area, lChip_cx, lChip_cy;
	double chip_width, chip_height;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{

	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return ;
	}

	//disp_obj(*pdlg->GetHalImage(), pdlg->m_dlgImgRect->GetView());

	gen_rectangle1(&Region, m_InsPara.m_InsRegionPunch[0].top, 
		                    m_InsPara.m_InsRegionPunch[0].left, 
							m_InsPara.m_InsRegionPunch[0].bottom, 
							m_InsPara.m_InsRegionPunch[0].right);

	reduce_domain(*pdlg->GetHalImage(),Region,&ImageReduced);


	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(m_edit_nChipThreshold));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else      
	{
		status.Format("Halcon Error Occured at threshold_20!");
		pdlg->WriteErrorList((LPCTSTR)status);
		return;
	}

	connection(Region,&ConnectedRegions);

	//티칭 시 칩 크기의 80 ~ 120%
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(m_InsPara.m_dParaChip[0] * 0.8),HTuple(m_InsPara.m_dParaChip[0] * 1.2));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다?
	{
		set_color(pdlg->m_dlgImgRect->GetView(), "red");
		set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
		write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : Can't find the chp !")));

		return;
	}
	else
	{
		set_color(pdlg->m_dlgImgRect->GetView(), "green");
		disp_obj(SelectedRegions, pdlg->m_dlgImgRect->GetView());

		//칩 면적, 센터 좌표
		area_center(SelectedRegions, &Area, &chip_cx, &chip_cy);
		lChip_area = (Hlong)Area[0];					//면적 픽셀
		lChip_cx = (Hlong)chip_cy[0];				//센터 x
		lChip_cy = (Hlong)chip_cx[0];				//센터 y

		//칩 사이즈
		//Row0 : top, Col0 : left, Row1 : bottom, Col1 : right
		smallest_rectangle1(SelectedRegions, &Row0, &Col0, &Row1, &Col1);
		
		chip_width = (Hlong)Col1[0] - (Hlong)Col0[0];
		chip_width = ( chip_width * PIXEL_SIZE) / 1000;			// um -> mm
		chip_height = (Hlong)Row1[0] - (Hlong)Row0[0];
		chip_height = ( chip_height * PIXEL_SIZE) / 1000;		// um -> mm


		//칩 크기 비교


		//칩 센터 비교


		/*
		//결과 디스플레이
		if(pdlg->m_dlgImgRect->GetView())
		{
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 400, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("SUCCESS : Chip teaching OK!")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 420, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("AREA : ") + (Hlong)Area[0]) + HTuple(" pixel"));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 435, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER x : ") + (Hlong)chip_cy[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 450, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("CENTER y : ") + (Hlong)chip_cx[0]));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 465, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("WIDTH : ") + ( chip_width * PIXEL_SIZE) / 1000 + HTuple(" mm")));
			set_tposition(pdlg->m_dlgImgRect->GetView(), 480, 10);
			write_string(pdlg->m_dlgImgRect->GetView(), HTuple(HTuple("HEIGHT : ") + ( chip_height * PIXEL_SIZE) / 1000 + HTuple(" mm")));

			//펀칭 중심에 크로스 그리기
			set_color(pdlg->m_dlgImgRect->GetView(), HTuple("blue"));
			disp_line(pdlg->m_dlgImgRect->GetView(), (Hlong)chip_cx[0], (Hlong)chip_cy[0] - 25, (Hlong)chip_cx[0], (Hlong)chip_cy[0] + 25);
			disp_line(pdlg->m_dlgImgRect->GetView(), (Hlong)chip_cx[0] - 25, (Hlong)chip_cy[0], (Hlong)chip_cx[0] + 25, (Hlong)chip_cy[0]);
			
			disp_obj(SelectedRegions, pdlg->m_dlgImgRect->GetView());
		}


		UpdateData(FALSE);
	}
			*/
}
