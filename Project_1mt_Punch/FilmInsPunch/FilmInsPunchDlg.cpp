
// FilmInsPunchDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FilmInsPunch.h"

#include "DialogModelMenu.h"
#include "DialogMainSetup.h"

#include "AreaCamControl.h"
#include "UserSockClient.h"
#include "com_class.h"

#include "FilmInsPunchDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
//메모리 릭 체크 start
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//메모리 릭 체크 end

#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFilmInsPunchDlg 대화 상자




CFilmInsPunchDlg::CFilmInsPunchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilmInsPunchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_Client = new CClientSock;
	m_dlgImgRect = new C_CtrlImageRect;

	m_pInspectThread = NULL;
	
	m_bInspectStart = FALSE;
	m_bInspectFlag = FALSE;
	m_bInspectResult = FALSE;

	m_HalGrabImage = NULL;

	ResetInspectPara();
	ResetSystemPara();
	ResetMainSysRslt();
	

}

void CFilmInsPunchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_tcp);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MODEL, m_label_model);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TITLE, m_ctrlMainTitle);
	DDX_Control(pDX, IDC_LIST_RSLT, m_list_rslt);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_LINE, m_label_mode);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_RUNSTOP, m_btnStartStop);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SETUP, m_btnSetup);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_ONLINE, m_label_online);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_TOTAL, m_label_tot);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GOOD, m_label_good);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_NG, m_label_ng);
}

BEGIN_MESSAGE_MAP(CFilmInsPunchDlg, CDialog)\
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
	ON_MESSAGE(WM_RECEIVE_DATA,OnReceive)
	ON_MESSAGE(WM_ONCONNECT,OnConnect)
	ON_MESSAGE(WM_ONCLOSE, OnClose)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CFilmInsPunchDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFilmInsPunchDlg 메시지 처리기

BOOL CFilmInsPunchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CString strTemp;
#ifdef _FILMINS_1METAL
	SetVisIndex(VISION_CAM_PUNCHING);
#else
	SetVisIndex(VISION_CAM_PUNCHING + 1);
#endif

	m_strDBPath.Format("D:\\DATABASE_%d\\MODEL", GetVisIndex());					// 모델 저장 경로 지정
	m_strRegiPath.Format("SOFTWARE\\FILMINS_%d\\MODEL", GetVisIndex());			// 모델 레지스트리 경로 지정
	m_strLogPath.Format("E:\\Insp_Result_%d", GetVisIndex());						//로그, 이미지 저장경로

	strTemp.Format("FilmIns Inspection %d", GetVisIndex() + 1);
	SetWindowText(strTemp);

	set_system(HTuple("pregenerate_shape_models"),HTuple("false"));

	CRect sys_rect;

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);
	MoveWindow(sys_rect.Width() - DIALOG_MOVE_OFFSET, 0, DIALOG_MOVE_OFFSET, sys_rect.Height(), TRUE);

	strTemp.Format("::CAM #%d(펀칭)", GetVisIndex() + 1);

	m_dlgImgRect->Create(this);
	m_dlgImgRect->MoveWindow(0, 0, sys_rect.Width() - DIALOG_MOVE_OFFSET, sys_rect.Height(), FALSE);
	m_dlgImgRect->SetCamIndex(0, strTemp, this->m_hWnd);
	m_dlgImgRect->ShowWindow(SW_SHOW);
	m_dlgImgRect->SetZoom(0, CAM_WIDTH, CAM_HEIGHT);


#ifdef _CAM

	HTuple wd, ht;
	Herror herr;		
	CString status;
	Hobject image;

	if(m_cCam.Open())
	{
		Sleep(500);

		m_cCam.Grab(GetHalImage());

		set_check("~give_error");
		herr = get_image_pointer1(*GetHalImage(), NULL, NULL, &wd, &ht);
		set_check("give_error");

		if(H_MSG_TRUE == herr)
		{
			m_dlgImgRect->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
		}
		else
		{
			status.Format("Halcon Error Occured at get_image_pointer1_00!");
			WriteErrorList((LPCTSTR)status);
			//return FALSE;
		}

		set_font(m_dlgImgRect->GetView(), "-Arial-12-*-*-*-*-*-1-");
		set_draw(m_dlgImgRect->GetView(), HTuple("margin"));

		WriteStatusList(">>>>>Found the Camera !");
	}
	else
	{
		WriteStatusList(">>>>>Can't find the Camera !");
	}


#else
	/*
	//image load
	CString tmpPath_Name;
	CFileDialog dlgFile(TRUE,"Image","*.*",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		tmpPath_Name = dlgFile.GetPathName();
		read_image(GetHalImage(), HTuple((LPCTSTR)tmpPath_Name));
	}
	else
	{
		gen_image_const(GetHalImage(), HTuple("byte"),CAM_HEIGHT, CAM_WIDTH);
	}

	delete dlgFile;	
	*/
	
#endif

	GetDlgItem(IDC_BTNENHCTRL_MAIN_RUNSTOP)->SetWindowTextA("RUN MODE");
	m_label_mode.SetWindowText("STOP MODE");
	m_label_mode.SetForeColor(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));

	//LIST CONTROL
	LV_COLUMN m_lvColumn;

	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText="TCP/IP";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=700;
	m_list_tcp.InsertColumn(0, &m_lvColumn);

	m_lvColumn.pszText="MESSAGE";
	m_list_rslt.InsertColumn(0, &m_lvColumn);

	//Model load Begin
	char temp[MAX_MOD_NAME];
	C_CtrlModelManageDlg *dlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	dlg->Open();

	sprintf(temp, "%s", dlg->GetName());
	m_label_model.SetCaption(temp);
	CString str0, str1;
	str0 = dlg->GetName();
	str1 = dlg->GetPath();

	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		WriteStatusList("Error : invalid model!");
	}
	else
	{
		//Load setup INI file
		if( LoadInspectPara(dlg->GetPath()) )
		{
			
		}
		else
		{
			WriteStatusList("Error : can't load setup parameter file!");
		}
	}

	LoadSystemPara(m_strDBPath);

	LoadSystemRslt(m_strDBPath);
	
	typeMainSystemRslt sys_rslt;
	sys_rslt = GetMainSysRslt();
	
	sprintf(temp, "%ld", sys_rslt.m_lCnt[0]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[1]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_ng.SetCaption(temp);

	
	delete dlg;
	//Model Load End

	m_dlgSetup = new CDialogMainSetup;
    m_dlgSetup->Create(IDD_DIALOG_MAIN_SETUP, this);
	m_dlgSetup->ShowWindow(SW_HIDE);

	m_ctrlMainTitle.SetForeColor( RGB(255,255,255) );
	m_ctrlMainTitle.SetBackColor( RGB(64, 64, 64) );
	m_ctrlMainTitle.SetWindowText("::Film Inspection Punch 검사(15.05.12.0933)");		//201506231925 by
	//칩 상태로 티칭하기.

	//소켓초기화/////////////////////////////////
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd);

#ifdef _FILMINS_1METAL // 1METAL
	m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, CONTROL_PC_PORT_NUM);
#else							// 2METAL
	m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, CONTROL_PC_PORT_NUM + 1);
#endif

#ifdef _CAM
	//시리얼 포트 초기화
	if(m_cComm.Open(m_SysPara.m_iParaSys[0]))			//vt6 2mt port 3
	{
		SetLight(LIGHT_CH_INDEX, m_InsPara.m_iParaBase[0]);
	}
	else
	{
		WriteStatusList("Serial port Open failed!");
	}
#endif
	
	WriteStatusList(">>>>>Application started!");

	strTemp.Format("FilmIns Inspection %d", GetVisIndex() + 1);
	SetWindowText(strTemp);

	m_BrushBlack.CreateSolidBrush(RGB(0, 0, 0));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFilmInsPunchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFilmInsPunchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HBRUSH CFilmInsPunchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)m_BrushBlack;

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return hbr;
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFilmInsPunchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
	
}

LRESULT CFilmInsPunchDlg::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	int idx0 = (int)para0, idx1 = (int)para1;
	long x, y;
	HTuple gray, gray2;
	HTuple wd, ht;
	Herror herr;
	CString status;

	switch(idx1)
	{
	case 0:	//DISPLAY
		if(m_dlgSetup->IsWindowVisible())
		{
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
		}

		m_dlgImgRect->Display();
		break;
	case 1:	//LIVE ON
		if(!m_bInspectStart) 
		{
			switch(idx0)
			{
			case 0:
				SetTimer(0, 10, NULL);
				break;
			}
		}
		HandleAllMessage();

		break;
	case 2:	//LIVE OFF
		if(!m_bInspectStart) 
		{
			//20100629 lss
			switch(idx0)
			{
			case 0:
				KillTimer(0);
				break;
			}
		}
		HandleAllMessage();

		break;
	case 3:	//IMAGE LOAD
		switch(idx0)
		{
		case 0:
			Hal_OpenHalImage(GetHalImage(), "", FALSE);
			
			set_check("~give_error");
			herr = get_image_pointer1(*GetHalImage(), NULL, NULL, &wd, &ht);
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at get_image_pointer1_02!");
				WriteErrorList((LPCTSTR)status);
				return 0;
			}

			m_dlgImgRect->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
			break;
		}
		break;
	case 4:	//IMAGE SAVE
		switch(idx0)
		{
		case 0:
			Hal_SaveHalImage(GetHalImage(), "", "bmp");
			break;
		}

		break;
	case 5:	//GRAY VALUE
		switch(idx0)
		{
		case 0:
			m_dlgImgRect->GetGrayPos(&x, &y);
			//Get gray 
			get_grayval(*GetHalImage(), y, x, &gray);
			m_dlgImgRect->SetGray((Hlong)gray[0]);

			set_color(m_dlgImgRect->GetView(), HTuple("blue"));
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
			break;
		}

		break;
	case 6:

		break;
	case 7:	//TEST
		Inspection();

		break;
	default:

		break;
	}
	return 0;
}


LRESULT CFilmInsPunchDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
	KillTimer(1);

	m_label_online.SetCaption("ON-LINE");
	m_label_online.SetForeColor(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));

	return 0;
}

LRESULT CFilmInsPunchDlg::OnClose(WPARAM wParam, LPARAM lParam)
{

	delete m_Client;
	m_Client = NULL;

	m_label_online.SetCaption("OFF-LINE");
	m_label_online.SetForeColor(RGB(255, 0, 0));
	m_label_online.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
	m_label_online.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));

	m_Client = new CClientSock();
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd); 

	SetTimer(1, 1000, NULL);


	return 0;
}

LRESULT CFilmInsPunchDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	char temp[PACKET_MAX_NUM];
	CString rcv_dat, rcv_temp, snd_dat;
	CString strCmd, strModel;
	int nCount = 0;

	m_Client->Receive((LPSTR)temp, PACKET_MAX_NUM);
	rcv_temp = temp;

	nCount = count((LPCTSTR)rcv_temp, (LPCTSTR)rcv_temp+rcv_temp.GetLength(), 0x03); // 두개의 패킷이 붙어서 올 경우 ETX 를 기준으로 잘라서 각각 처리한다.
	
	for(int i = 0; i < nCount; i++)
	{
		AfxExtractSubString(rcv_dat, rcv_temp, i, 0x03);
		rcv_dat.Delete(0, 1);

		WriteTCPList((LPCTSTR)rcv_dat, TRUE);
		
		AfxExtractSubString(strCmd, rcv_dat, 1, ',');

		if(!strCmd.Compare("0003"))				//Inspect Start
		{			
			m_bInspectFlag = TRUE;
		}
		//20150624 by 얼라인 체크.
		else if(!strCmd.Compare("0005"))
		{
			m_cCam.Grab(GetHalImage());
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());

			if(InspectionAlign())
			{
				//성공 응답패킷 전송
				snd_dat.Format("%cVIS%d,0004,%d%c", 0x02, GetVisIndex(), 1, 0x03);
				OnSend(snd_dat);
			}
			else
			{
				//실패 응답패킷 전송
				snd_dat.Format("%cVIS%d,0004,%d%c", 0x02, GetVisIndex(), 0, 0x03);
				OnSend(snd_dat);
			}
		}
		//20161025 by 더미 감지
		else if(!strCmd.Compare("0007"))
		{
			m_cCam.Grab(GetHalImage());
			disp_obj(*GetHalImage(), m_dlgImgRect->GetView());

			if(DUMMY_FILM_DETECT_LIMIT < GetHalRegionMean(*GetHalImage(), m_InsPara.m_InsRegionChip[0]))		// 더미구간 종료
			{
				snd_dat.Format("%cVIS%d,0004,%d%c", 0x02, GetVisIndex(), 1, 0x03);
			}
			else if(DUMMY_FILM_DETECT_LIMIT > GetHalRegionMean(*GetHalImage(), m_InsPara.m_InsRegionChip[0]))	//더미구간 진행
			{
				snd_dat.Format("%cVIS%d,0004,%d%c", 0x02, GetVisIndex(), 0, 0x03);
			}
			//성공 응답패킷 전송
			OnSend(snd_dat);
		}
		else if(!strCmd.Compare("0011"))				//Model Change
		{
			AfxExtractSubString(strModel, rcv_dat, 2, ',');

			if(RemoteControlModel(2, strModel))
			{
				//성공 응답패킷 전송
				snd_dat.Format("%cVIS%d,0012,%s%c", 0x02, GetVisIndex(), strModel, 0x03);
				OnSend(snd_dat);
			}
			else
			{
				//실패 응답패킷 전송
				snd_dat.Format("%cVIS%d,0012%c", 0x02, GetVisIndex(), 0x03);
				OnSend(snd_dat);
			}
		}
		else if(!strCmd.Compare("0013"))				//Auto Run
		{
			if(!m_btnStartStop.GetValue())
			{
				m_btnStartStop.SetValue(TRUE);
				ClickBtnenhctrlMainRunstop();
				snd_dat.Format("%cVIS%d,0014%c", 0x02, GetVisIndex(), 0x03);
				OnSend(snd_dat);
			}
		}
		else if(!strCmd.Compare("0023"))
		{
			SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

			snd_dat.Format("%cVIS%d,0024%c", 0x02, GetVisIndex(), 0x03);
			OnSend(snd_dat);
		}
	}

	return 0;
}

void CFilmInsPunchDlg::OnSend(CString data)
{
	int rslt = 1;
	int size = 0;
	char temp[PACKET_MAX_NUM] = {0, };

	WriteTCPList((LPCTSTR)data, FALSE);
	
	sprintf(temp, "%s", data);
	size = strlen(temp);

	m_Client->Send((LPSTR)temp, size);

}

void CFilmInsPunchDlg::OnTimer(UINT_PTR nIDEvent)
{
	typeInspectPara ins_para;
	CString strFrameRate;


	Hobject RegionRect;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case 0:	//LIVE
		
#ifdef _CAM	
		m_cCam.Grab(GetHalImage());
		disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
#endif

		set_color(m_dlgImgRect->GetView(), HTuple("green"));
		disp_line(m_dlgImgRect->GetView(), CAM_HEIGHT * 0.5, 0, CAM_HEIGHT * 0.5, CAM_WIDTH);
		disp_line(m_dlgImgRect->GetView(), 0, CAM_WIDTH * 0.5, CAM_HEIGHT, CAM_WIDTH * 0.5);
		//수평 기준 라인 Display (48mm기준)
		set_color(m_dlgImgRect->GetView(), HTuple("red"));

		// m_dPara[0] : PIXEL_SIZE
		// m_dParaBase[2] : COF_WIDTH
		disp_line(m_dlgImgRect->GetView(), 0, (CAM_WIDTH * 0.5)+(COF_WIDTH * 1000 / PIXEL_SIZE / 2) ,
						CAM_HEIGHT, (CAM_WIDTH * 0.5)+(COF_WIDTH * 1000 / PIXEL_SIZE / 2));
		disp_line(m_dlgImgRect->GetView(), 0, (CAM_WIDTH * 0.5)-(COF_WIDTH * 1000 / PIXEL_SIZE / 2) ,
						CAM_HEIGHT, (CAM_WIDTH * 0.5)-(COF_WIDTH * 1000 / PIXEL_SIZE / 2));

		/////////////////////////////////////////
		//Display film region for start position
		//
		
		ins_para = GetInspectPara();

		//Align inspect region
		set_color(m_dlgImgRect->GetView(), HTuple("green"));
		gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0].top, ins_para.m_InsRegionAlign[0].left, ins_para.m_InsRegionAlign[0].bottom, ins_para.m_InsRegionAlign[0].right);
		disp_obj(RegionRect, m_dlgImgRect->GetView());

		break;
	case 1:	// SOCKET Reconnect try
		{
			m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, CONTROL_PC_PORT_NUM);

			break;
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}
BEGIN_EVENTSINK_MAP(CFilmInsPunchDlg, CDialog)
	ON_EVENT(CFilmInsPunchDlg, IDC_BTNENHCTRL_MAIN_RUNSTOP, DISPID_CLICK, CFilmInsPunchDlg::ClickBtnenhctrlMainRunstop, VTS_NONE)
	ON_EVENT(CFilmInsPunchDlg, IDC_BTNENHCTRL_MAIN_SETUP, DISPID_CLICK, CFilmInsPunchDlg::ClickBtnenhctrlMainSetup, VTS_NONE)
	ON_EVENT(CFilmInsPunchDlg, IDC_BTNENHCTRL_MAIN_MODEL, DISPID_CLICK, CFilmInsPunchDlg::ClickBtnenhctrlMainModel, VTS_NONE)
	ON_EVENT(CFilmInsPunchDlg, IDC_BTNENHCTRL_MAIN_EXIT, DISPID_CLICK, CFilmInsPunchDlg::ClickBtnenhctrlMainExit, VTS_NONE)
END_EVENTSINK_MAP()

void CFilmInsPunchDlg::ClickBtnenhctrlMainRunstop()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	if(m_btnStartStop.GetValue())
	{
		//Stop live
		KillTimer(0);

		m_btnStartStop.SetCaption("STOP");

		m_label_mode.SetCaption("RUN MODE");
		m_label_mode.SetForeColor(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));


    	StartInspectThread();

		WriteStatusList(">>>>RUN MODE(Inspection started!)");
	}
	else
	{
		m_btnStartStop.SetCaption("RUN");

		m_label_mode.SetCaption("STOP MODE");
		m_label_mode.SetForeColor(RGB(255, 0, 0));
		m_label_mode.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
		m_label_mode.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));

		StopInspectThread();

		WriteStatusList(">>>>STOP MODE(Inspection stopped!)");
	}
	
}

void CFilmInsPunchDlg::ClickBtnenhctrlMainSetup()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_dlgSetup->ShowWindow(SW_SHOW);

	WriteStatusList("Setup started!");
}

void CFilmInsPunchDlg::ClickBtnenhctrlMainModel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int idx = 0;
 	CRect rect;
 	char temp[MAX_MOD_NAME];
 	CString str0, str1;	
 	CString Dir;

 	if(m_btnStartStop.GetValue())
 	{
 		MessageBox("Error : 현재 검사 진행 중입니다! 검사 정지 후 재 시도 바랍니다.", NULL, MB_ICONEXCLAMATION);
 		return;
 	}
 
 	//Close setup dialog
 	m_dlgSetup->ShowWindow(SW_HIDE);

	//Stop live
	KillTimer(0);
 
 	CDialogModelMenu dlg;
 
 	GetDlgItem(IDC_BTNENHCTRL_MAIN_MODEL)->GetWindowRect(rect);
 	dlg.m_Rect = rect;
 
 	//20100712 lee
 	idx = (int)dlg.DoModal();
 	if(idx == 2) return; 
 
 	idx = dlg.m_Ret;
 
 	//Model
 	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	
 	switch(idx)
 	{
 	//New
 	case 0:
 		moddlg->DoModal(idx);
 		WriteStatusList("MODEL CHANGE-NEW");
 		//Make directory
 		Dir.Format("%s", moddlg->GetPath());
 		MakeDirectories(Dir);

 		break;
 	//Delete
 	case 1:
 		if(moddlg->DoModal(idx))
 		{
 			WriteStatusList("MODEL CHANGE-DELETE");
 		}
 		break;
 	//Select
 	case 2:
 		//Model save
 		moddlg->Open();
 		sprintf(temp, "%s", moddlg->GetName());
 
 		str0 = moddlg->GetName();
 		str1 = moddlg->GetPath();
 		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
 		{
 			WriteStatusList("Error : invalid model!");
 		}
 		else
 		{

 		}
 
 
 		if(moddlg->DoModal(idx))
 		{
 			WriteStatusList("MODEL CHANGE-SELECT");
 		}

 		//Load setup parameter
 		moddlg->Open();
 		sprintf(temp, "%s", moddlg->GetName());
 		m_label_model.SetCaption(temp);
 		
 		str0 = moddlg->GetName();
 		str1 = moddlg->GetPath();
 		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
 		{
 			WriteStatusList("Error : invalid model!");
		}
 		else
 		{
 			//모델 로딩 이므로 기존 등록 모델 삭제 한다.?
 			clear_all_shape_models(); //20110508 lee
 			//Load setup INI file
 			if( LoadInspectPara(moddlg->GetPath()) )
 			{
				SetLight(LIGHT_CH_INDEX, m_InsPara.m_iParaBase[0]);
 			}
 			else
 			{
 				WriteStatusList("Error : can't load setup parameter file!");
 
 				//Make directory
 				Dir.Format("%s", moddlg->GetPath());
 				MakeDirectories(moddlg->GetPath());
 			}
 		}
 
 		break;
 	default:
 		
 		break;
 	}
 
 	delete moddlg;
}

BOOL CFilmInsPunchDlg::RemoteControlModel(short menu_select, CString model_name)
{
	BOOL rslt = TRUE;
	CString str, mod_name, status, mod_desc, Dir, str0, str1;
	C_CtrlModelManageDlg *dlg;

	switch(menu_select)
	{
	case 0:	//NEW

		break;
	case 1:	//DELETE

		break;
	case 2:	//SELECT
		HandleAllMessage();

		str.Empty();
		str = m_label_model.GetCaption();
		if(0 == str.Compare(model_name))
		{
			//이미 있으면
			status.Format("Warnning : this model already loaded!");
			WriteStatusList((LPCTSTR)status);
			return 0;
		}

		dlg = new C_CtrlModelManageDlg(NULL, m_strDBPath, m_strRegiPath);

		//Model save
		dlg->Open();

		str0 = dlg->GetName();
		str1 = dlg->GetPath();

		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
		{
			WriteStatusList("Error : invalid model!");
			rslt =FALSE;
		}
		else
		{
			
		}

		mod_desc.Format("%s", "REMOTE CONTROLLED");
		if(dlg->RemoteModel(model_name, mod_desc))
		{
			status.Format("%s", dlg->GetName());
			m_label_model.SetCaption((LPCTSTR)status);
			//성공 
			dlg->Open();
			//Make directory
			Dir.Format("%s", dlg->GetPath());
			MakeDirectories(Dir);
			//Load setup INI file

			if( LoadInspectPara(dlg->GetPath()))
			{
				SetLight(LIGHT_CH_INDEX, m_InsPara.m_iParaBase[0]);

				status.Format("Model change OK!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				status.Format("%s", dlg->GetName());
				m_label_model.SetCaption((LPCTSTR)status);

				rslt = TRUE;
			}
			else
			{
				status.Format("Error : can't load setup parameter file!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				rslt = FALSE;
			}

			delete dlg;
		}
		break;
	default:

		break;
	}

	return rslt;
}

void CFilmInsPunchDlg::ClickBtnenhctrlMainExit()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetLight(LIGHT_CH_INDEX, LIGHT_OFF);

	ModelSave();
	
	m_cComm.Close();
#ifdef _CAM
	m_cCam.Close();
#endif

	delete m_Client;
	delete m_dlgImgRect;
//	delete m_dlgSetup;

	exit(0); 
}

BOOL CFilmInsPunchDlg::StartInspectThread()
{
	m_bInspectStart = TRUE;

	HandleAllMessage();

	m_pInspectThread = AfxBeginThread(InspectThread, this, THREAD_PRIORITY_NORMAL);
	if (m_pInspectThread) m_pInspectThread->m_bAutoDelete = TRUE;

	return TRUE;
}

BOOL CFilmInsPunchDlg::StopInspectThread()
{
	m_bInspectStart = FALSE;

	return TRUE;	
}

UINT CFilmInsPunchDlg::InspectThread(LPVOID pParam)
{
	CFilmInsPunchDlg *pdlg = (CFilmInsPunchDlg *)pParam;
	typeMainSystemRslt sys_rslt;
	sys_rslt = pdlg->GetMainSysRslt();

	while(TRUE)
	{
		if(!pdlg->m_bInspectStart) 
		{
			AfxEndThread(0);
            break;
		}
		
		if(pdlg->m_bInspectFlag)
		{
			pdlg->m_bInspectFlag = FALSE; 

			if(pdlg->Inspection())
			{
				//OK
				pdlg->m_bInspectResult = TRUE;
			}
			else
			{
				//NG
				pdlg->m_bInspectResult = FALSE;
			}

			pdlg->InspectResult();
		}

		Sleep(1);
	}

	return 0;
}

void CFilmInsPunchDlg::SetLight(int ch, int val)
{
	char packet[100];

	sprintf(packet, "%c%02dWR%03d%c", 0x02, ch, val, 0x03);
	writeSCL(packet, false);
}

BOOL CFilmInsPunchDlg::SaveSystemPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i;
	CString file_path, pat_path, extention, master_path;
	

	//Make director
	MakeDirectories(path);

	file_path = path + "\\System.par";

	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_System%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_SysPara.m_iParaSys[i]);
			WritePrivateProfileString("SYSTEM_PARAMETER", key, dat, file_path);
		}

		for(i = 0; i < BOOL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_bPara_System%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_SysPara.m_bParaSys[i]);
			WritePrivateProfileString("SYSTEM_PARAMETER", key, dat, file_path);
		}

	}

	fd=0;
	
	return rslt;

}

BOOL CFilmInsPunchDlg::LoadSystemPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	CString file_path, pat_path, extention, master_path;
	int i;

	file_path = path + "\\System.par";

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_System%d", 0, i);
			m_SysPara.m_iParaSys[i] = GetPrivateProfileInt("SYSTEM_PARAMETER", key, 0, file_path);

		}

		for(i = 0; i < BOOL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_bPara_System%d", 0, i);
			m_SysPara.m_bParaSys[i] = GetPrivateProfileInt("SYSTEM_PARAMETER", key, 0, file_path);
		}
	}

	fd = 0;

	return rslt;
}

BOOL CFilmInsPunchDlg::SaveInspectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i;
	CString file_path, pat_path, extention, master_path;
	Herror Herr;

	//Make director
	MakeDirectories(path);

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(i = 0; i < RECT_PARA_MAX_NUM_ALIGN; i++)
		{
			//ALIGN
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Left", 0, i);
 			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionAlign[i].left);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Top", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionAlign[i].top);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Right", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionAlign[i].right);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Bottom", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionAlign[i].bottom);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}
		
		for(i = 0; i < RECT_PARA_MAX_NUM_CHIP; i++)
		{

			//CHIP
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Left", 0, i);
 			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionChip[i].left);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Top", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionChip[i].top);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Right", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionChip[i].right);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Bottom", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionChip[i].bottom);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		for(i = 0; i < RECT_PARA_MAX_NUM_PUNCH; i++)
		{
			//PUNCH
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Left", 0, i);
 			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionPunch[i].left);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Top", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionPunch[i].top);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Right", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionPunch[i].right);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
			
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Bottom", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_InsRegionPunch[i].bottom);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		
		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Punch%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_iParaPunch[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);

			key.Empty();
			key.Format("CAM%d_iPara_Chip%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_iParaChip[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		for(i = 0; i < DBL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_dPara_Punch%d", 0, i);
			dat.Empty();
			dat.Format("%f", m_InsPara.m_dParaPunch[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);

			key.Empty();
			key.Format("CAM%d_dPara_Chip%d", 0, i);
			dat.Empty();
			dat.Format("%f", m_InsPara.m_dParaChip[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}


		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Align%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_iParaAlign[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		for(i = 0; i < DBL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_dPara_Align%d", 0, i);
			dat.Empty();
			dat.Format("%f", m_InsPara.m_dParaAlign[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Base%d", 0, i);
			dat.Empty();
			dat.Format("%d", m_InsPara.m_iParaBase[i]);
			WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		}

		key.Empty();
		key.Format("CAM%d_iPosX_Align_Pos", 0);
		dat.Empty();
		dat.Format("%f", m_InsPara.m_dPatternPosXAlign);
		WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
		
		key.Empty();
		key.Format("CAM%d_iPosY_Align_Pos", 0);
		dat.Empty();
		dat.Format("%f", m_InsPara.m_dPatternPosYAlign);
		WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);

		if(m_InsPara.m_bCreatMdIDAlign)
		{
			pat_path = path + "\\PatternAlign."; 
			extention.Format("m%d", 0);
			pat_path = pat_path + extention;
			set_check("~give_error");
			Herr = write_shape_model(m_InsPara.m_PatIDAlign, HTuple(((LPCTSTR)pat_path)));
			set_check("give_error");
			if(H_MSG_TRUE == Herr)
			{
				m_InsPara.m_bCreatMdIDAlign = TRUE;
			}
			else
			{
				m_InsPara.m_bCreatMdIDAlign = FALSE;
			}
		}


		
	}
	fd=0;
	
	return rslt;
}

BOOL CFilmInsPunchDlg::LoadInspectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	char temp[DAT_STR_LENG];
	CString file_path, pat_path, extention, master_path;
	Herror Herr;
	int i;

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < RECT_PARA_MAX_NUM_ALIGN; i++)
		{
			key.Empty();

			key.Format("CAM%d_Ins_Align%d_InspectRect1_Left", 0, i);
			m_InsPara.m_InsRegionAlign[i].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Top", 0, i);
			m_InsPara.m_InsRegionAlign[i].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Right", 0, i);
			m_InsPara.m_InsRegionAlign[i].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Align%d_InspectRect1_Bottom", 0, i);
			m_InsPara.m_InsRegionAlign[i].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			
			if( (0 >= m_InsPara.m_InsRegionAlign[i].left) || 
				(0 >= m_InsPara.m_InsRegionAlign[i].top) || 
				(0 >= m_InsPara.m_InsRegionAlign[i].right) || 
				(0 >= m_InsPara.m_InsRegionAlign[i].bottom) )
			{
				m_InsPara.m_InsRegionAlign[i].left = 100;
				m_InsPara.m_InsRegionAlign[i].top = 100;
				m_InsPara.m_InsRegionAlign[i].right = 200;
				m_InsPara.m_InsRegionAlign[i].bottom = 200;
			}
		}

		for(i = 0; i < RECT_PARA_MAX_NUM_CHIP; i++)
		{
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Left", 0,  i);
			m_InsPara.m_InsRegionChip[i].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Top", 0,  i);
			m_InsPara.m_InsRegionChip[i].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Right", 0,  i);
			m_InsPara.m_InsRegionChip[i].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Chip%d_InspectRect1_Bottom", 0,  i);
			m_InsPara.m_InsRegionChip[i].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			
			if( (0 >= m_InsPara.m_InsRegionChip[i].left) || 
				(0 >= m_InsPara.m_InsRegionChip[i].top) || 
				(0 >= m_InsPara.m_InsRegionChip[i].right) || 
				(0 >= m_InsPara.m_InsRegionChip[i].bottom) )
			{
				m_InsPara.m_InsRegionChip[i].left = 100;
				m_InsPara.m_InsRegionChip[i].top = 100;
				m_InsPara.m_InsRegionChip[i].right = 200;
				m_InsPara.m_InsRegionChip[i].bottom = 200;
			}
		}

		for(i = 0; i < RECT_PARA_MAX_NUM_PUNCH; i++)
		{
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Left", 0,  i);
			m_InsPara.m_InsRegionPunch[i].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Top", 0,  i);
			m_InsPara.m_InsRegionPunch[i].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Right", 0,  i);
			m_InsPara.m_InsRegionPunch[i].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			key.Empty();
			key.Format("CAM%d_Ins_Punch%d_InspectRect1_Bottom", 0,  i);
			m_InsPara.m_InsRegionPunch[i].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
			
			if( (0 >= m_InsPara.m_InsRegionPunch[i].left) || 
				(0 >= m_InsPara.m_InsRegionPunch[i].top) || 
				(0 >= m_InsPara.m_InsRegionPunch[i].right) || 
				(0 >= m_InsPara.m_InsRegionPunch[i].bottom) )
			{
				m_InsPara.m_InsRegionPunch[i].left = 100;
				m_InsPara.m_InsRegionPunch[i].top = 100;
				m_InsPara.m_InsRegionPunch[i].right = 200;
				m_InsPara.m_InsRegionPunch[i].bottom = 200;
			}
		}

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Punch%d", 0, i);
			m_InsPara.m_iParaPunch[i] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

			key.Empty();
			key.Format("CAM%d_iPara_Chip%d", 0, i);
			m_InsPara.m_iParaChip[i] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
		}

		for(i = 0; i < DBL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_dPara_Punch%d", 0, i);
			GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
			m_InsPara.m_dParaPunch[i] = atof(temp);

			key.Empty();
			key.Format("CAM%d_dPara_Chip%d", 0, i);
			GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
			m_InsPara.m_dParaChip[i] = atof(temp);
		}

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Align%d", 0, i);
			m_InsPara.m_iParaAlign[i] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
		}

		for(i = 0; i < DBL_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_dPara_Align%d", 0, i);
			GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
			m_InsPara.m_dParaAlign[i] = atof(temp);
		}

		for(i = 0; i < INT_PARA_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("CAM%d_iPara_Base%d", 0, i);
			m_InsPara.m_iParaBase[i] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
		}

		key.Empty();
		key.Format("CAM%d_iPosX_Align_Pos", 0);
		GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
		m_InsPara.m_dPatternPosXAlign = atof(temp);
		
		key.Empty();
		key.Format("CAM%d_iPosY_Align_Pos", 0);
		GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
		m_InsPara.m_dPatternPosYAlign = atof(temp);

		//fixed
		m_InsPara.m_PatParaAlign.m_shType = VISIONHALFUNC_MOD_RECT1;
		m_InsPara.m_PatParaAlign.m_AngleStart = 0;
		m_InsPara.m_PatParaAlign.m_AngleExtend = 0;
		m_InsPara.m_PatParaAlign.m_AngleStep = 0;
		sprintf(m_InsPara.m_PatParaAlign.m_Optimize, "none");
		sprintf(m_InsPara.m_PatParaAlign.m_Matric, "ignore_global_polarity");
		sprintf(m_InsPara.m_PatParaAlign.m_SupPix, "interpolation");
		m_InsPara.m_PatParaAlign.m_Overlap = 0;
		
		m_InsPara.m_PatParaAlign.m_MatchNum = 1;
		m_InsPara.m_PatParaAlign.m_Greed = 0.5;
		m_InsPara.m_PatParaAlign.m_ScaleMin = 1.0;
		m_InsPara.m_PatParaAlign.m_ScaleMax = 1.0;
		m_InsPara.m_PatParaAlign.m_ScaleStep = 0;
		//read shape region
		pat_path = path + "\\PatternAlign."; 
		extention.Format("m%d", 0);
		pat_path = pat_path + extention;
		set_check("~give_error");
		Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_InsPara.m_PatIDAlign);
		set_check("give_error");
		if(H_MSG_TRUE == Herr)
		{
			m_InsPara.m_bCreatMdIDAlign = TRUE;
		}
		else
		{
			m_InsPara.m_bCreatMdIDAlign = FALSE;
		}
		
	}

	return rslt;
}


//Load/save system result
BOOL CFilmInsPunchDlg::LoadSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString key;

	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd = CreateFile( path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			key.Format("CNT%d", i);
			m_MainSysRslt.m_lCnt[i] = GetPrivateProfileInt("MAINSYSTEM_RESULT", key, 0, path);
		}
	}
	fd=0;

	return rslt;
}


BOOL CFilmInsPunchDlg::SaveSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString dat, key;

	//Make director
	MakeDirectories(path);
	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd=CreateFile(   path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			dat.Empty();
			dat.Format("%d", m_MainSysRslt.m_lCnt[i]);
			key.Format("CNT%d", i);
			WritePrivateProfileString("MAINSYSTEM_RESULT", key, dat, path);
		}

	}
	fd=0;

	return rslt;
}

void CFilmInsPunchDlg::InspectResult()
{	
	typeInspectPara ins_para;
	ins_para = GetInspectPara();
	typeMainSystemRslt sys_rslt;
	sys_rslt = GetMainSysRslt();

	CString snd_dat;

	char temp[MAX_MOD_NAME];

	if(m_bInspectResult)
	{
		//OK
		sys_rslt.m_lCnt[1]++;
		snd_dat.Format("%cVIS%d,0004,%d,%c", 0x02, GetVisIndex(), 1, 0x03);
	}
	else
	{
		//NG
		sys_rslt.m_lCnt[2]++;

		snd_dat.Format("%cVIS%d,0004,%d,%c", 0x02, GetVisIndex(), 0, 0x03);
	}

	SaveImage();

	OnSend(snd_dat);

	//TOT
	sys_rslt.m_lCnt[0]++;

	sprintf(temp, "%ld", sys_rslt.m_lCnt[0]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[1]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_ng.SetCaption(temp);


	SetMainSysRslt(sys_rslt);

}	

BOOL CFilmInsPunchDlg::SaveImage()
{
	Herror herr;
	CString m_strLotNo, m_strStartTime, Dir, strTemp, status;
	char path[MAX_PATH];
	memset(path, NULL, sizeof(path));

	
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	m_strStartTime.Format("%04d%02d%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	if(m_SysPara.m_bParaSys[0])	//NG
	{
		sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, "LOTNUMBER");

		Dir.Format("%s", path);
		MakeDirectories(Dir);

		sprintf(path, "%s\\VIS%d_%02d%02d%02d.bmp", path, GetVisIndex(), lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);//20100726 lee

		set_check("~give_error");
		herr = write_image(*GetHalImage(), "bmp", 0, path);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
			AfxMessageBox(strTemp);
			status.Format("Halcon Error Occured at write_image_00!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}
	}
	else if(m_SysPara.m_bParaSys[1])	//GOOD
	{
		sprintf(path, "%s\\%s\\%s\\IMAGE\\GOOD", m_strLogPath, m_strStartTime, "LOTNUMBER");

		Dir.Format("%s", path);
		MakeDirectories(Dir);

		sprintf(path, "%s\\VIS%d_%02d%02d%02d.bmp", path, GetVisIndex(), lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);//20100726 lee

		set_check("~give_error");
		herr = write_image(*GetHalImage(), "bmp", 0, path);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
			AfxMessageBox(strTemp);
			status.Format("Halcon Error Occured at write_image_00!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}
	}

	return TRUE;

}

void CFilmInsPunchDlg::ModelSave()
{
	char temp[MAX_MOD_NAME];
	CString str0, str1;
	
	//Model
	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	//Model save
	moddlg->Open();

	sprintf(temp, "%s", moddlg->GetName());
	
	str0 = moddlg->GetName();
	str1 = moddlg->GetPath();
	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		WriteStatusList("Error : invalid model!");
	}
	else
	{		

		if( SaveInspectPara(moddlg->GetPath()) )
		{
			
		}
		else
		{
			WriteStatusList("Error : can't save setup parameter file!");
		}
	}

	SaveSystemPara(m_strDBPath);
	SaveSystemRslt(m_strDBPath);

	delete moddlg;
}

BOOL CFilmInsPunchDlg::Inspection()
{
	BOOL rslt = TRUE;
	CRect rect;
	Hobject rslt_region;

	m_cCam.Grab(GetHalImage());
	disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
	
	//펀치 인식 면적 최소. 최대, 버tol, 얼라인tol
	if(InspectionAlign())
	{
		rect = m_InsPara.m_InsRegionPunch[0];

		if( FindPPuchingChip(m_dlgImgRect->GetView(), *GetHalImage(), rect, &rslt_region) )
		{
			

		}
		else
		{
			rslt = FALSE;
		}
	}
	else
	{
		rslt = FALSE;
	}
	
	

	return rslt;
}

BOOL CFilmInsPunchDlg::InspectionAlign()
{
	BOOL rslt = TRUE;
	
	Hobject modimage, shape, Region, ImageReduced, InvertHallImage;
	Hlong wd, ht;
	double ra, rs, rscore;   
	Hlong rn;	
	double pat_score;
	double offset_x, offset_y;

	CRect rect, rect2;

	Hobject EmphaImage;

	pat_score = (((double)m_InsPara.m_iParaAlign[1]) * 0.01);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(), NULL, NULL, &wd, &ht);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{
		disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_04!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	
	rn = 0;
	int idx = 0;
	
	//BEGIN
	rect = m_InsPara.m_InsRegionAlign[0];
	gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);
	
	reduce_domain(*GetHalImage(),Region,&ImageReduced);
	emphasize(ImageReduced, &EmphaImage, 5, 5, 3);
	
	if(m_InsPara.m_bCreatMdIDAlign)
	{
		Hal_ShapePatternFind(m_dlgImgRect->GetView(), 
			&EmphaImage, 
			m_InsPara.m_PatIDAlign,
			m_InsPara.m_PatParaAlign, 
			m_InsPara.m_InsRegionAlign[1], 
			wd, ht,
			m_InsPara.m_dParaAlign[0],//pattern cx 
			m_InsPara.m_dParaAlign[1],//pattern cy
			pat_score,//Score
			&modimage, 
			&shape, 
			&m_InsPara.m_dParaAlign[2], 
			&m_InsPara.m_dParaAlign[3], 
			&ra, 
			&rs, 
			&rscore, 
			&rn);
	// END
		if(rn)
		{
			//Pos. limit check
			offset_x = m_InsPara.m_dParaAlign[2] - m_InsPara.m_dPatternPosXAlign;
			offset_x = fabs(offset_x);
			offset_x *= PIXEL_SIZE;

			offset_y = m_InsPara.m_dParaAlign[3] - m_InsPara.m_dPatternPosYAlign;
			offset_y = fabs(offset_y);
			offset_y *= PIXEL_SIZE;

			if( (m_InsPara.m_iParaAlign[2] < offset_x) || (m_InsPara.m_iParaAlign[2] < offset_y))
			{
				//Limit error
				set_color(m_dlgImgRect->GetView(), "red");
				set_tposition(m_dlgImgRect->GetView(), 200, 10);
				write_string(m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : Align limited")));
				return FALSE;
			}

 			set_color(m_dlgImgRect->GetView(), "green");
			set_tposition(m_dlgImgRect->GetView(), 200, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("SCORE : ") + HTuple(rscore * 100)));
			set_tposition(m_dlgImgRect->GetView(), 20 + (200), 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("X : ") + HTuple(m_InsPara.m_dParaAlign[2])));
			set_tposition(m_dlgImgRect->GetView(), 35 + (200), 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("Y : ") + HTuple(m_InsPara.m_dParaAlign[3])));
			set_tposition(m_dlgImgRect->GetView(), 50 + (200), 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("ANGLE : ") + HTuple(ra)));
		}
		else
		{
			set_color(m_dlgImgRect->GetView(), "red");
			set_tposition(m_dlgImgRect->GetView(), 200, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : can't find model!")));
			
			rslt = FALSE;
		}
	}
	else
	{
		set_color(m_dlgImgRect->GetView(), "red");
		set_tposition(m_dlgImgRect->GetView(), 200, 10);
		write_string(m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : no pattern!")));

		rslt = FALSE;
	}

	return rslt;
}


//20150624 by
BOOL CFilmInsPunchDlg::FindPPuchingChip(Hlong win, Hobject image, CRect ins_region, Hobject *rslt_region)
{
	BOOL rslt = TRUE; 

	//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Range, Num;

	Hobject ConvxRegion;

	CRect rect;

	Hlong wd, ht;
	HTuple Area, p_x, p_y, Row0, Col0, Row1, Col1;

	double offset_x, offset_y, offset_tol;	
	double p_width, p_height, diff_insp_width, diff_insp_height;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
		//disp_obj(*GetHalImage(), m_dlgImgRect->GetView());
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	//Punching inspection////////////////////////
	reduce_domain(image,Region,&ImageReduced);

/*
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}
*/
	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(200),HTuple(255)); //20100712 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{

	}
	else      
	{
		status.Format("Halcon Error Occured at threshold_20!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	
	int punch_wd = (m_InsPara.m_dParaPunch[0] * MICRO_METER) / PIXEL_SIZE;
	int punch_ht = (m_InsPara.m_dParaPunch[1] * MICRO_METER) / PIXEL_SIZE;
	
	
	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	
	//실제 펀치 사이즈의 80 ~ 120% 까지 허용
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple( (punch_wd*punch_ht)*0.6),HTuple((punch_wd*punch_ht)*1.6));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		set_color(win, "red");
		set_tposition(win, 400, 10);
		write_string(win, HTuple(HTuple("FAIL : No Punch or NG Punch!")));
		return FALSE;
	}
	else
	{
		area_center(SelectedRegions, &Area, &p_y, &p_x);

		set_color(win, HTuple("red"));
		disp_line(win, p_y, p_x - 25, p_y, p_x + 25);
		disp_line(win, p_y - 25, p_x, p_y + 25, p_x);
		disp_obj(SelectedRegions, m_dlgImgRect->GetView());

		//칩 티칭 시 중심과 펀치 중심 비교		
		offset_x = fabs((double)(m_InsPara.m_dParaChip[1] - (Hlong)p_x[0])) * PIXEL_SIZE;
		offset_y = fabs((double)(m_InsPara.m_dParaChip[2] - (Hlong)p_y[0])) * PIXEL_SIZE;
		offset_tol = m_InsPara.m_dParaPunch[4] * MICRO_METER;

		if( (offset_tol < offset_x) || (offset_tol < offset_y))
		{ 
			//Limit error
			set_color(m_dlgImgRect->GetView(), "red");
			set_tposition(m_dlgImgRect->GetView(), 400, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("FAIL : Punch limited")));
			set_tposition(win, 400 + 20, 10);
			write_string(win, HTuple(HTuple("Punch CX : ") + HTuple((Hlong)p_x[0])));
			set_tposition(win, 400 + 35, 10);
			write_string(win, HTuple(HTuple("Punch CY : ") + HTuple((Hlong)p_y[0])));
			set_tposition(win, 400 + 50, 10);
			write_string(win, HTuple(HTuple("Tolerance : ") + HTuple(offset_x/1000) + HTuple("mm / ") + HTuple(offset_y/1000) + HTuple("mm")));
			return FALSE;
		}

		//펀치 사이즈 검사. mm 단위로 비교
		smallest_rectangle1(SelectedRegions, &Row0, &Col0, &Row1, &Col1);

		p_width = (Hlong)Col1[0] - (Hlong)Col0[0];
		p_height = (Hlong)Row1[0] - (Hlong)Row0[0];
		p_width = (p_width * PIXEL_SIZE) / 1000;
		p_height = (p_height * PIXEL_SIZE) / 1000;

		diff_insp_width = p_width - (m_InsPara.m_dParaPunch[0]);
		diff_insp_width = fabs(diff_insp_width);

		diff_insp_height = p_height - (m_InsPara.m_dParaPunch[1]);
		diff_insp_height = fabs(diff_insp_height);

		if( (diff_insp_width > m_InsPara.m_dParaPunch[2]) || (diff_insp_height > m_InsPara.m_dParaPunch[3]) )
		{
			//NG
			set_color(win, "red");
			disp_obj(SelectedRegions, win);

			set_tposition(win, 400, 10);
			write_string(win, HTuple(HTuple("FAIL : Punch size !")));
			set_tposition(win, 400 + 20, 10);
			write_string(win, HTuple(HTuple("Punch Width : ") + HTuple(p_width) + HTuple(" mm")));
			set_tposition(win, 400 + 35, 10);
			write_string(win, HTuple(HTuple("Punch Height : ") + HTuple(p_height) + HTuple(" mm")));

			disp_line(win, p_y, p_x - 25, p_y, p_x + 25);
			disp_line(win, p_y - 25, p_x, p_y + 25, p_x);

			return FALSE;
		}

		//Punch burr검사/////////////////////////////
		union1(SelectedRegions, &SelectedRegions);
		shape_trans(SelectedRegions, &ConvxRegion, "convex");
		erosion_circle(ConvxRegion, &ConvxRegion, 5.5);
		difference(ConvxRegion, SelectedRegions, &SelectedRegions);
		connection(SelectedRegions, &SelectedRegions);
		select_shape(SelectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(m_InsPara.m_iParaPunch[2]),HTuple("max"));

		union1(SelectedRegions, &SelectedRegions);
		count_obj(SelectedRegions, &Num);

		if(0 < (Hlong)Num[0]) //
		{
			//편집 불량으로 처리 
			copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시

			if(win)
			{
				set_color(win, "red");
				set_tposition(win, 400, 10);
				write_string(win, HTuple(HTuple("FAIL : Burr found !")));
				disp_obj(SelectedRegions, win);
			}

			return FALSE;
		}

		//결과 디스플레이
		if(m_dlgImgRect->GetView())
		{
			//GOOD
			set_color(win, HTuple("green"));
			disp_obj(SelectedRegions, win);
			set_tposition(win, 400, 10);
			write_string(win, HTuple(HTuple("SUCCESS : Punch OK!")));
			set_tposition(m_dlgImgRect->GetView(), 420, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("AREA : ") + (Hlong)Area[0]) + HTuple(" pixel"));
			set_tposition(m_dlgImgRect->GetView(), 435, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("CENTER x : ") + (Hlong)p_x[0]));
			set_tposition(m_dlgImgRect->GetView(), 450, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("CENTER y : ") + (Hlong)p_y[0]));
			set_tposition(m_dlgImgRect->GetView(), 465, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("WIDTH : ") + HTuple(p_width) + HTuple(" mm")));
			set_tposition(m_dlgImgRect->GetView(), 480, 10);
			write_string(m_dlgImgRect->GetView(), HTuple(HTuple("HEIGHT : ") + HTuple(p_height) + HTuple(" mm")));

			disp_line(win, p_y, p_x - 25, p_y, p_x + 25);
			disp_line(win, p_y - 25, p_x, p_y + 25, p_x);
			disp_obj(SelectedRegions, m_dlgImgRect->GetView());
		}
	}

	return rslt;

}

int CFilmInsPunchDlg::writeSCL(char *str, bool needResponse)
{
	char ch = 13;
	if (m_cComm.IsOpen())
	{
		m_cComm.Write(str); // 
		m_cComm.Write(&ch); // 
		if (needResponse == TRUE)
		{ 
			Sleep(50);

			char str2[100] = {0,};
			int length = m_cComm.Read(str2, 100);
			if(length > 0)
			{
				char* response =new char[length - 1];
				for (int i = 3; i < length; i++)
				{
					response[i-4] = str2[i];
				}

				//nRtnVal = atof(response);
				//bRtn = 1;
			}
			else
			{
				//nRtnVal = 0;
				//bRtn = 0;
				return false;
			}
		}
	}
	return true;
}

void CFilmInsPunchDlg::WriteStatusList(LPCTSTR lpszItem)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);
  
	memset(name, NULL, sizeof(temp));
	sprintf(name, "System_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	m_list_rslt.InsertItem(0, temp);

	if(LIST_MONI_NUM <= m_list_rslt.GetItemCount())
	{
		m_list_rslt.DeleteItem(LIST_MONI_NUM - 1);
	}

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\SYSTEM_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteLogStatus(path, name, "System_Log.bak", temp, 0);
}

void CFilmInsPunchDlg::WriteTCPList(LPCTSTR lpszItem, BOOL IsRCVorSND)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);

	if(IsRCVorSND)
		sprintf(temp, "[%02d:%02d:%02d %s]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,"CTRL->VIS", lpszItem);
	else
		sprintf(temp, "[%02d:%02d:%02d %s]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,"VIS->CTRL", lpszItem);	

	memset(name, NULL, sizeof(temp));
	sprintf(name, "TCP_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	m_list_tcp.InsertItem(0, temp);

	if(LIST_MONI_NUM <= m_list_tcp.GetItemCount())
	{
		m_list_tcp.DeleteItem(LIST_MONI_NUM - 1);
	}

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\SYSTEM_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteLogStatus(path, name, "TCP_Log.bak", temp, 0);
}

BOOL CFilmInsPunchDlg::WriteLogStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			AfxMessageBox("Error : log file open fail!(WriteLogStatus)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}


void CFilmInsPunchDlg::WriteErrorList(LPCTSTR lpszItem)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);

	memset(name, NULL, sizeof(temp));
	sprintf(name, "Error_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "E:\\Insp_Result\\%04d%02d%02d\\SYSTEM_LOG\\", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteErrorStatus(path, name, "Error_Log.bak", temp, 0);
}

BOOL CFilmInsPunchDlg::WriteErrorStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			AfxMessageBox("Error : log file open fail!(WriteErrorStatus)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}
int CFilmInsPunchDlg::HandleAllMessage()
{
	int returnValue;
	MSG Mess;

	do {
		returnValue=::PeekMessage(&Mess, NULL, 0, 0, PM_REMOVE);
		if (returnValue)
		{
			::TranslateMessage(&Mess);
			::DispatchMessage (&Mess);
		}
	} while(returnValue);
	
	return returnValue;
}


int CFilmInsPunchDlg::GetHalRegionMean(Hobject img, CRect rect)	//20161025 by 이미지 내 특정영역의 평균 밝기 값을 리턴
{
	Hobject region;
	double mean, deviation;

	gen_rectangle1(&region, rect.top, rect.left, rect.bottom, rect.right);
	intensity(region, img,  &mean, &deviation);

	return (int)mean;
	
}

void CFilmInsPunchDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int mean;

	mean = GetHalRegionMean(*GetHalImage(), m_InsPara.m_InsRegionChip[0]);
	
	if( mean > 230 )
		AfxMessageBox("더미구간이 끝남");
	else if( mean < 230 )
		AfxMessageBox("더미구간임");
	/*
	if(m_cCam.Grab(GetHalImage()))
		disp_obj(*GetHalImage(), m_dlgImgRect->GetView());

	return;
*/
			/*
	//m_bInspectFlag = TRUE;
	//bool a= m_btnStartStop.GetValue();
	typeMainSystemRslt sys_rslt;
	sys_rslt = GetMainSysRslt();

	sys_rslt.m_lCnt[0] = 0;
	sys_rslt.m_lCnt[1] = 0;
	sys_rslt.m_lCnt[2] = 0;

	char temp[10];
	sprintf(temp, "%ld", sys_rslt.m_lCnt[0]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[1]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_ng.SetCaption(temp);


	SetMainSysRslt(sys_rslt);
	*/

}
