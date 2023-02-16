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
#include "LightCtrlDlg.h"
#include "PasswordDlg.h"


//�޸� �� üũ start
#ifdef _DEBUG
#define new DEBUG_NEW

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif
//�޸� �� üũ end


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// CFilmInsDlg ��ȭ ����
CFilmInsDlg::CFilmInsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilmInsDlg::IDD, pParent)
	, m_edit_test(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	int i, j;

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_bInspectStart[i] = FALSE;

		m_dlgImgRect[i] = NULL;
		m_iMapIdx[i] = 0;
		m_iNGViewIdx[i] = 0;
		m_iNGSubViewIdx[i] = 0;
		m_iRslt[i] = 0;

		m_bCheckGrab = FALSE;

		m_pThread[i] = NULL;
		m_pInspectThread[i] = NULL;

		m_iBufIdx[i] = 0;//Buffer index
		m_iGrbIdx[i] = 0;//Grab index
		m_iPrcIdx[i] = 0;//Process index

		m_iNGDeviceIdx[i] = 0;
		m_bInspectStartNGView[i] = 0;

		m_iPrcDevIdx[i] = 0;
		m_iGrbDevIdx[i] = 0;

		//NG Scroll
		m_iNGScrollIdx[i] = 0;
		m_iNGScrollLastNum[i] = 0;

		for(j = 0; j < SUB_RGN_MAX_NUM; j++)
		{
			m_bChkNGKind[i][j] = 0;
		}
	}

	m_pDeleSearchThread = NULL;
	m_bDeleteFolder = FALSE;
	m_bDeleteFolderStart = FALSE;

	for(i = 0; i < 4; i++)
	{
		m_iLightChIdx[i] = 0;
		m_iLightOnOffIdx[i] = 0;
	}

	memset(m_sNGLotID, NULL, sizeof(char) * MAX_PATH * IMG_RECT_MAX_NUM);
	memset(m_sNGDate, NULL, sizeof(char) * MAX_PATH * IMG_RECT_MAX_NUM);

	memset(m_VisFlagData, NULL, sizeof(typeVisionFlagData) * IMG_RECT_MAX_NUM);

	//Live
	m_bLiveOverlayDisp = TRUE;
	
	m_BrushBlack.CreateSolidBrush(RGB(0, 0, 0));

	m_lColorMode = 1;
	m_lProfileMode = 1;

	m_VisMod = new C_VisionModule();
	m_dlgSetup = NULL; 

	m_cTriggerBoard = NULL;
	
	//Comizoa DIO Handle
	m_hComiDevice64 = NULL;
	//m_hComiDevice32 = NULL;

	m_img0_ch0 = 0;	m_img0_ch1 = 0;	m_img0_ch2 = 0;	m_img0_ch3 = 0;
	m_img1_ch0 = 0;	m_img1_ch1 = 0;	m_img1_ch2 = 0;	m_img1_ch3 = 0;
	m_img2_ch0 = 0;	m_img2_ch1 = 0;	m_img2_ch2 = 0;	m_img2_ch3 = 0;
	m_img3_ch0 = 0;	m_img3_ch1 = 0;	m_img3_ch2 = 0;	m_img3_ch3 = 0;
	m_img4_ch0 = 0;	m_img4_ch1 = 0;	m_img4_ch2 = 0;	m_img4_ch3 = 0;
	m_img5_ch0 = 0;	m_img5_ch1 = 0;	m_img5_ch2 = 0;	m_img5_ch3 = 0;
	m_img6_ch0 = 0;	m_img6_ch1 = 0;	m_img6_ch2 = 0;	m_img6_ch3 = 0;
	m_img7_ch0 = 0;	m_img7_ch1 = 0;	m_img7_ch2 = 0;	m_img7_ch3 = 0;
	m_img8_ch0 = 0;	m_img8_ch1 = 0;	m_img8_ch2 = 0;	m_img8_ch3 = 0;
	m_img9_ch0 = 0;	m_img9_ch1 = 0;	m_img9_ch2 = 0;	m_img9_ch3 = 0;
	m_img10_ch0 = 0; m_img10_ch1 = 0; m_img10_ch2 = 0; m_img10_ch3 = 0;
	m_img11_ch0 = 0; m_img11_ch1 = 0; m_img11_ch2 = 0; m_img11_ch3 = 0;

	m_bIsColor = FALSE;
	m_strStartTime.Empty();

	m_bLineCamOn = FALSE;

	m_dlgAM_Reference = NULL;
	m_dlgAM_Reference = new CDialogAMReference();

	m_nNGIndex = 0;
	m_bNGBegin = FALSE;
}

CFilmInsDlg::~CFilmInsDlg()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		if(NULL != m_dlgImgRect[i])
		{
			delete m_dlgImgRect[i];	
			m_dlgImgRect[i] = NULL;
		}
	}

	if(NULL != m_dlgSetup)
	{
		delete m_dlgSetup;
		m_dlgSetup = NULL;
	}

	if(NULL != m_Client)
	{
		delete m_Client;
		m_Client = NULL;
	}

	delete m_dlgAM_Reference;
	m_dlgAM_Reference = NULL;

	if(NULL != m_VisMod)
	{
		delete m_VisMod;
		m_VisMod = NULL;
	}
}

void CFilmInsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TITLE, m_ctrlMainTitle);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_LINE, m_label_mode);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_ONLINE, m_label_online);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_TOTAL, m_label_tot);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GOOD, m_label_good);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_NG, m_label_ng);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GRABTIME, m_label_gtime);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_PROCESSTIME, m_label_ptime);
	DDX_Control(pDX, IDC_LIST_RSLT, m_list_rslt);
	DDX_Control(pDX, IDC_LIST_TCP, m_list_tcp);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MODEL, m_label_model);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH0, m_img0_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH1, m_img0_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH2, m_img0_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH3, m_img0_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH0, m_img1_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH1, m_img1_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH2, m_img1_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH3, m_img1_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH0, m_img2_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH1, m_img2_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH3, m_img2_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH4, m_img2_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH0, m_img3_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH1, m_img3_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH2, m_img3_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH3, m_img3_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH0, m_img4_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH1, m_img4_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH2, m_img4_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH3, m_img4_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH0, m_img5_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH1, m_img5_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH2, m_img5_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH3, m_img5_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH0, m_img6_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH1, m_img6_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH2, m_img6_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH3, m_img6_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH0, m_img7_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH1, m_img7_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH2, m_img7_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH3, m_img7_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH0, m_img8_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH1, m_img8_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH2, m_img8_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH3, m_img8_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH0, m_img9_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH1, m_img9_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH2, m_img9_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH3, m_img9_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH0, m_img10_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH1, m_img10_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH2, m_img10_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH3, m_img10_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH0, m_img11_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH1, m_img11_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH2, m_img11_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH3, m_img11_ch3);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP0, m_map0);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP1, m_map1);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP2, m_map2);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP3, m_map3);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP4, m_map4);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP5, m_map5);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP6, m_map6);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP7, m_map7);
	DDX_Control(pDX, IDC_SCROLLBAR_NG, m_scroll_ng);
	DDX_Control(pDX, IDC_BTNENHCTRL_SETUP, m_btnSetup);
	DDX_Control(pDX, IDC_BTNENHCTRL_RUNSTOP, m_btnStartStop);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_NGVIEW, m_label_ngview);
	DDX_Text(pDX, IDC_EDIT_TEST, m_edit_test);
}

BEGIN_MESSAGE_MAP(CFilmInsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_STN_CLICKED(IDC_RECT_IMG_MAP0, &CFilmInsDlg::OnRectImgMap0)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP1, &CFilmInsDlg::OnRectImgMap1)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP2, &CFilmInsDlg::OnRectImgMap2)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP3, &CFilmInsDlg::OnRectImgMap3)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP4, &CFilmInsDlg::OnRectImgMap4)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP5, &CFilmInsDlg::OnRectImgMap5)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP6, &CFilmInsDlg::OnRectImgMap6)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP7, &CFilmInsDlg::OnRectImgMap7)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP8, &CFilmInsDlg::OnRectImgMap8)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP9, &CFilmInsDlg::OnRectImgMap9)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP10, &CFilmInsDlg::OnRectImgMap10)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP11, &CFilmInsDlg::OnRectImgMap11)
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
	ON_MESSAGE(USER_MSG_VIS, OnVisMessage)
	ON_MESSAGE(USER_MSG_SOCKET, OnSocketMessage)
	ON_MESSAGE(USER_MSG_LINESCAN_MODULE, OnLinescanMessage)
	ON_MESSAGE(WM_ONCONNECT,OnConnect)
	ON_MESSAGE(WM_ONCLOSE, OnClose)
	ON_MESSAGE(WM_RECEIVE_DATA,OnReceive)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, &CFilmInsDlg::OnBnClickedButton1)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TCP, &CFilmInsDlg::OnNMRClickListTcp)
END_MESSAGE_MAP()


BOOL CFilmInsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.
	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.@
	
	//1PC 3Client Selector
	int m_cmdLine = 0;
	CString strTemp;
	SYSTEMTIME	lpSystemTime;	

	CWinApp *App = AfxGetApp();
	m_cmdLine = atoi(App->m_lpCmdLine);
	m_VisMod->SetVisIndex(m_cmdLine);														//ī�޶� Ÿ�Կ� ���� �б⸦ ���� ����

	GetLocalTime(&lpSystemTime);
	m_strStartTime.Format("%04d%02d%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
 
	InitCamType(m_VisMod->GetVisIndex());

	SetLightIndex(m_VisMod->GetVisIndex(), m_bLineCamOn);	 											// ���� �ε���

	SetStaticMapType(m_VisMod->GetVisIndex(), m_bLineCamOn);										// �˻� �׸� �ε���
	
	m_VisMod->LoadSystemPara(m_strDBPath);

	InitDialog();

	WriteStatusList("Application started!");
	
#ifdef _CAM
	OnOffLight(FALSE, m_bLineCamOn);
#endif

	UpdateLight(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CFilmInsDlg::InitCamType(int nType)
{
	int frIdx, chIdx, iPortNum, i, j;
	CString strTemp;
	CRect sys_rect;

	m_strDBPath.Format("D:\\DATABASE_%d\\MODEL", nType);				// �� ���� ��� ����
	m_strRegiPath.Format("SOFTWARE\\FILMINS_%d\\MODEL", nType);		// �� ������Ʈ�� ��� ����
	m_strLogPath.Format("E:\\Insp_Result_%d", nType);					//�α�, �̹��� ������

#ifdef _CAM
	//Comizoa DIO Device handle
	if(COMI_LoadDll())
	{
		m_hComiDevice64 = COMI_LoadDevice(0xB402, 0);	// DO 64

		WriteStatusList("Success : I/O Initialize OK !");
	}
	else
	{
		WriteStatusList("Error : I/O Initialize failed !");
	}
#endif


	switch(nType)
	{
	case VISION_CAM_TOP1:
		strTemp.Format("::CAM #%d(���) 1 METAL", m_VisMod->GetVisIndex() + 1);			// ���÷��� ����ǥ���� �̸�

		frIdx = M_DEV0;																	// �����ӱ׷��� ���� �ε���
		chIdx = 0;																		// �����ӱ׷��� ä�� �ε���

		iPortNum = SOCKET_PORT_NUM1;													// ���� ��Ʈ
		m_bIsColor = FALSE;																// ī�޶� �÷� ��������
		m_bLineCamOn = FALSE;															// ���ν�ĵ ��뿩��

		m_lWidth[0] = WIDTH;
		m_lHeight[0] = HEIGHT;

#ifdef _CAM
		if(m_cCam.Open(frIdx, chIdx, m_bIsColor))
			WriteStatusList("Success : CAMERA Initialize OK !");
		else
			WriteStatusList("Error : CAMERA Initialize Failed !");
#endif

		for(i = 0; i < AREACAM_MAX_BUF_NUM; i++)
			for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				gen_image_const(m_VisMod->GetHalImage(0, i, j),HTuple("byte"),WIDTH,HEIGHT);

		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_OCVAUTOTEACH)->EnableWindow(TRUE);		
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("MONO MODE");
		break;
	case VISION_CAM_TOP2:
		strTemp.Format("::CAM #%d(���) 1 METAL", m_VisMod->GetVisIndex() + 1);

		iPortNum = SOCKET_PORT_NUM2;
		m_bIsColor = TRUE;
		m_bLineCamOn = TRUE;

		m_lWidth[0] = LINE_CAM_WIDTH;
		m_lHeight[0] = LINE_CAM_HEIGHT;

#ifdef _CAM
		if(m_cLineCam.Open_LineScanCam(this->m_hWnd, m_bIsColor))
			WriteStatusList("Success : Line CAMERA Initialize OK !");
		else
			WriteStatusList("Error : Line CAMERA Initialize Failed !");

		//Initialize Trigger Module
		m_cTriggerBoard = new CTriggerBoard();
		m_cTriggerBoard->m_cTrigger = (CTriggerBoard*)this;

		if(m_cTriggerBoard->Init(m_strDBPath))
		{
			//20190921 by �߰�
			m_cTriggerBoard->SetTrigger_Trig_Pos_Reset();
			WriteStatusList("Success : Trigger Board Initialize OK !");
		}
		else
			WriteStatusList("Error : Trigger Board Initialize Failed !");

		//Initialize LIGHT
		m_cSerial.Open(LINE_LIGHT_COM_PORT, LINE_LIGHT_COM_BAUDRATE);

		if(m_cSerial.IsOpen())
			WriteStatusList("Success : Line Light Initialize OK !");
		else
			WriteStatusList("Error : Line Light Initialize Failed !");

		m_ThreadNGImage = new CImageSave();

#endif

		for(i = 0; i < LINE_CAM_MAX_BUF_NUM; i++)
			for(j = 0; j < LINE_IMAGE_MAP_MAX_NUM; j++)
				gen_image_const(m_VisMod->GetHalImage(0, i, j), HTuple("byte"), LINE_CAM_WIDTH, LINE_CAM_HEIGHT);
	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("COLOR MODE");
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_OCVAUTOTEACH)->EnableWindow(FALSE);
		break;
	case VISION_CAM_BOTTOM:
		strTemp.Format("::CAM #%d(�Ϻ�) 1 METAL", m_VisMod->GetVisIndex() + 1);

		frIdx = M_DEV0;
		chIdx = 1;

		iPortNum = SOCKET_PORT_NUM3;
		m_bIsColor = TRUE;
		m_bLineCamOn = FALSE;

		m_lWidth[0] = WIDTH;
		m_lHeight[0] = HEIGHT;

#ifdef _CAM
		if(m_cCam.Open(frIdx, chIdx, m_bIsColor))
			WriteStatusList("Success : CAMERA Initialize OK !");
		else
			WriteStatusList("Error : CAMERA Initialize Failed !");
#endif

		for(i = 0; i < AREACAM_MAX_BUF_NUM; i++)
			for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				gen_image_const(m_VisMod->GetHalImage(0, i, j),HTuple("byte"),WIDTH,HEIGHT);

		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("COLOR MODE");
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_OCVAUTOTEACH)->EnableWindow(FALSE);
		break;
	default:
		break;
	}


	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);
	MoveWindow(sys_rect.Width() - MAINVIEW_MOVE_OFFSET, 0, MAINVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_dlgImgRect[i] = new C_CtrlImageRect;
		//m_dlgImgRect[i]->Create(IMAGE_RECT_DIALOG, this);
		m_dlgImgRect[i]->Create(this);
		m_dlgImgRect[i]->MoveWindow(0, 0, sys_rect.Width() - MAINVIEW_MOVE_OFFSET, sys_rect.Height(), FALSE);
		m_dlgImgRect[i]->SetCamIndex(0, strTemp, this->m_hWnd);
		m_dlgImgRect[i]->ShowWindow(SW_SHOW);
		
		m_dlgImgRect[i]->SetZoom(i, (long)m_lWidth[0], (long)m_lHeight[0]);

		set_font(m_dlgImgRect[i]->GetView(), "-Arial-12-*-*-*-*-*-1-");
		set_draw(m_dlgImgRect[i]->GetView(), HTuple("margin"));
	}

	set_system(HTuple("pregenerate_shape_models"),HTuple("false"));
	//MAP, NG display Begin
	Hlong idc[IMAGE_MAP_MAX_NUM];
	idc[0] = IDC_RECT_IMG_MAP0;	idc[1] = IDC_RECT_IMG_MAP1;	idc[2] = IDC_RECT_IMG_MAP2;		idc[3] = IDC_RECT_IMG_MAP3;
	idc[4] = IDC_RECT_IMG_MAP4; idc[5] = IDC_RECT_IMG_MAP5;	idc[6] = IDC_RECT_IMG_MAP6;		idc[7] = IDC_RECT_IMG_MAP7;
	idc[8] = IDC_RECT_IMG_MAP8;	idc[9] = IDC_RECT_IMG_MAP9;	idc[10] = IDC_RECT_IMG_MAP10;	idc[11] = IDC_RECT_IMG_MAP11;

	for(i = 0; i < IMAGE_MAP_MAX_NUM; i++)
	{
		set_check("~father");
 		open_window(0, 0, 80, 80, (Hlong)(GetDlgItem(idc[i]))->m_hWnd, "transparent", "", &m_lWindowID[i]);
		set_check("father");

		set_part(m_lWindowID[i], 0, 0, m_lHeight[0], m_lWidth[0]);
	}

	Hlong ngidc[IMAGE_NG_NUM];

	for(i = 0; i < IMAGE_NG_NUM; i++)
	{
		ngidc[i] = IDC_STATIC_NGRECT0 + i;

		set_check("~father");
		open_window(0, 0, 300, 290, (Hlong)(GetDlgItem(ngidc[i]))->m_hWnd, "transparent", "", &m_lNGWindowID[i]);
		set_check("father");

		set_draw(m_lNGWindowID[i], HTuple("margin"));
	}
	
	//�����ʱ�ȭ/////////////////////////////////
	m_Client = new CClientSock;
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd); 

#ifdef _CAM
	m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, iPortNum);
#endif

	strTemp.Format("FilmIns Inspection %d", nType + 1);
	SetWindowText(strTemp);

	return;
}

void CFilmInsDlg::InitDialog()
{
	typeMainSystemRslt sys_rslt;
	char temp[MAX_MOD_NAME];
	CRect temp_rect;
	
	//Model load Begin
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
		if( m_VisMod->LoadInspectPara(dlg->GetPath()) )
			m_VisMod->LoadInspectSelectPara(dlg->GetPath());
		else
			WriteStatusList("Error : can't load setup parameter file!");
	}

	m_strFullPath = dlg->GetPath();//20160922 by

	delete dlg;
	//Model Load End

	//���̾�α� �ʱ�ȭ
	m_dlgProcessing.Create(IDD_DIALOG_PROCESSING, this);
	m_dlgProcessing.ShowWindow(SW_HIDE);

	m_dlgSetup = new CSetUpBsaeDlg;
    m_dlgSetup->Create(IDD_SETUP_BASE_DIALOG, this);
	m_dlgSetup->ShowWindow(SW_HIDE);

	//Init result list
	LV_COLUMN m_lvColumn;

	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText=" ";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=700;
	m_list_rslt.InsertColumn(0, &m_lvColumn);

	//TCP
	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText="TCP/IP";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=700;
	m_list_tcp.InsertColumn(0, &m_lvColumn);

	//NG scroll
	m_scroll_ng.SetScrollRange(0, NG_MAX_NUM);
    m_scroll_ng.SetScrollPos(0);
    m_scroll_ng.EnableScrollBar(ESB_ENABLE_BOTH);
 
    SCROLLINFO info = {sizeof(SCROLLINFO), SIF_PAGE, 0};
    info.nPage = 0;
    m_scroll_ng.SetScrollInfo(&info);

	//20160922 by
	//AM ���۷��� ���÷��� ���̾�α�
	CRect sys_rect;

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);

	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP2:
		if(NULL != m_dlgAM_Reference)
		{
			m_dlgAM_Reference->Create(IDD_DIALOG_AM_REFERENCE, this);
			
			GetDlgItem(IDC_STATIC_NGRECT0)->GetClientRect(&temp_rect);

			m_dlgAM_Reference->MoveWindow((sys_rect.Width() - MAINVIEW_MOVE_OFFSET) -315, temp_rect.top, 285 , sys_rect.Height() - 25);
			m_dlgAM_Reference->ResizeView();

			m_dlgAM_Reference->ShowWindow(SW_HIDE);
		}
		break;
	}
	
	//Load/save system result
	m_VisMod->LoadSystemRslt(m_strDBPath);
	sys_rslt = m_VisMod->GetMainSysRslt();
	
	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[4]);
	m_label_gtime.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);

	m_ctrlMainTitle.SetForeColor( RGB(255,255,255) );
	m_ctrlMainTitle.SetBackColor( RGB(64, 64, 64) );

	m_label_mode.SetWindowText("STOP MODE");
	m_label_mode.SetForeColor(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFilmInsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		CPen pen,*oldPen;
		CBrush brush,*oldBrush;

		CPaintDC dc(this);

		GetClientRect(&rect);

		//����� ä���
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldPen=dc.SelectObject(&pen);
		brush.CreateSolidBrush(RGB(0, 0, 0));
		oldBrush=dc.SelectObject(&brush);
		
		dc.Rectangle(&rect);

		dc.SelectObject(oldPen);
		pen.DeleteObject();
		dc.SelectObject(oldBrush);
		brush.DeleteObject();
	
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CFilmInsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CFilmInsDlg::HandleAllMessage()
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

void CFilmInsDlg::DoEvents()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}
void CFilmInsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USER MESSAGE BEGIN
//
LRESULT CFilmInsDlg::OnVisMessage(WPARAM para0, LPARAM para1)
{
	int i;

	CRect sys_rect;

	CString status;

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	typeSystemPara sys_para;
	typeInspectRslt ins_rslt;	

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	sys_para = m_VisMod->GetSystemPara(0);
	ins_rslt = m_VisMod->GetInspectRslt(0);

	switch((int)para0)
	{
	case 0:
		//Grab process///////////////////////////////////////////////////////
		//Display result
		//(0 : grab/1 : process, int buf_idx, BOOL chk_ng_type)
		InspectResult((int)para0, (int)para1);  //20100609  lee

		break;
	case 1:
		//Inspect process/////////////////////////////////////////////////////

		//Display result
		//(0 : grab/1 : process, int buf_idx, BOOL chk_ng_type)
		InspectResult((int)para0, (int)para1);

		//��ǥ NG type�� �˾Ƴ���.
		CheckNGKind(0, (int)para1);
		//Process end

		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			m_VisFlagData[0].m_iInsErrNum[i] = ins_rslt.m_iRsltErrNum[i];//��������
			m_VisFlagData[0].m_iInsFindChip[i] = ins_rslt.m_iRsltChipFind[i];

			if(1 == ins_rslt.m_iRslt[i])
			{
				if(2 == ins_rslt.m_iRsltChipFind[i])
				{
					 m_VisFlagData[0].m_iInsRsltType[i] = PNCH;
					 sprintf(m_VisFlagData[0].m_sInsRsltType[i], "��ġ*****");
				}
				else if(3 == ins_rslt.m_iRsltChipFind[i])
				{
					m_VisFlagData[0].m_iInsRsltType[i] = PCND;
					sprintf(m_VisFlagData[0].m_sInsRsltType[i], "�ҷ�-��ġ");
				}
				else
				{
					m_VisFlagData[0].m_iInsRsltType[i] = 0;
					sprintf(m_VisFlagData[0].m_sInsRsltType[i], "��ǰ*****");
				}
			}
			else if(2 == ins_rslt.m_iRsltChipFind[i]) 
			{

				m_VisFlagData[0].m_iInsRsltType[i] = PNCH;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "��ġ*****");
			}
			else if(1 != ins_rslt.m_iRsltAlign[i]) // �켱���� ���� Align���� �켱
			{
				m_VisFlagData[0].m_iInsRsltType[i] = AMIF;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "�ҷ�-����");

			}
			else if(-20 == ins_rslt.m_iRsltChipFind[i])
			{
				m_VisFlagData[0].m_iInsRsltType[i] = NOCP;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "�ҷ�-��Ĩ");
				
			}
			else if( (-50 >= ins_rslt.m_iRsltChipFind[i]) && (-60 < ins_rslt.m_iRsltChipFind[i]) )
			{
				m_VisFlagData[0].m_iInsRsltType[i] = MKND;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "�ҷ�-��ŷ");
				
			}
			else
			{
				m_VisFlagData[0].m_iInsRsltType[i] = m_bChkNGKind[0][i];

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "�ҷ�-��Ÿ");

				char temp[100];
				sprintf(temp, "[NGType:%d/%s]\n", m_VisFlagData[0].m_iInsRsltType[i], m_VisFlagData[0].m_sInsRsltType[i]);	
			}
		}
		break;
	//20100629 lss
	case 2:

		break;
	//Lot start
	case 3:
		
		break;
	//Lot end
	case 4:

		break;
	//Vision start
	case 5:

		break;
	//NG display
	case 6:
	
		break;
	case 7:

		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::OnVisMessage)!");
		break;
	}


	return 0;
}

LRESULT CFilmInsDlg::OnLinescanMessage(WPARAM para0, LPARAM para1)
{
	OnOffLight(FALSE, TRUE);

	return 0;
}

LRESULT CFilmInsDlg::OnSocketMessage(WPARAM para0, LPARAM para1)
{
	int idx = (int)para1;
	int nOffset, nCofNum;
	int i;
	CString strPacket, strTemp1, strTemp2;

	switch(idx)
	{
	case 0:	//Find Start ��� ����
		nOffset = (int)m_VisFlagData[0].m_dData[0];
		strPacket.Format("%cVIS%d,0006,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), nOffset, PACKET_CHAR_ETX);
		break;
	case 1:	//Align Start ��� ����
		nOffset = (int)(m_VisFlagData[0].m_dData[1] + ALIGN_OFFSET_DATA);
		strPacket.Format("%cVIS%d,0008,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), nOffset, PACKET_CHAR_ETX);
		break;
	case 2: //Inspectiong start ��� ����
		nCofNum = m_VisMod->GetCOFNum(0);
		nOffset = (int)(m_VisFlagData[0].m_dData[2] + ALIGN_OFFSET_DATA);
		strTemp1.Format("%cVIS%d,0004,%d,%04d,%04d,%d", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), m_iPrcIdx[0], m_iPrcDevIdx[0], nOffset, nCofNum);

		for(i = 0; (i < nCofNum) && (i < SUB_RGN_MAX_NUM); i++)
		{
			strTemp2.Format(",%03d", m_VisFlagData[0].m_iInsRsltType[i]);
			strTemp1 += strTemp2;
		}
		strTemp2.Format("%c", PACKET_CHAR_ETX);
		strPacket = strTemp1 + strTemp2;
		break;
	case 3: //Grab Start �������
		strPacket.Format("%cVIS%d,0002,%d,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), m_iGrbIdx[0], m_iGrbDevIdx[0], PACKET_CHAR_ETX);
		break;
	case 4:	//OCV Teach
		strPacket.Format("%cVIS%d,0026,%d,%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), (int)para0, PACKET_CHAR_ETX);
		break;
	case 5: //Model change

		break;
	case 6:	//Auto run
		strPacket.Format("%cVIS%d,0014%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	case 7:	//Grab&Display
		strPacket.Format("%cVIS%d,0016%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	case 8:	//Lot Start

		break;
	case 9:	//Lot End

		break;
	case 10: //NG Display
		strPacket.Format("%cVIS%d,0022%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	case 11: //Grab ���
		strPacket.Format("%cVIS%d,0000%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
	}

	OnSend(strPacket);

	return 0;
}


LRESULT CFilmInsDlg::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	
	// TODO: Add your control notification handler code here
	char temp[100];
	long x, y;
	HTuple gray, gray2;
	int loop;
	HTuple wd, ht;
	int rslt = 1;
	//Halcon image initialize
	HTuple StartSecond, EndSecond;

	typeInspectRslt ins_rslt;
//	typeShiftRslt shift_rslt;
	
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	Herror herr;
	CString status;



//	typeVisionResultData VisRsltData;




	switch((int)para1)
	{
	//Display
	case 0:
		if(m_dlgSetup->IsWindowVisible())
		{
			switch(m_VisMod->GetVisIndex())
			{
			case VISION_CAM_TOP1:
				DisplayView((int)para0, m_iBufIdx[0], 2);
				break;
			case VISION_CAM_TOP2:
				DisplayView((int)para0, m_iBufIdx[0], 0);
				break;
			case VISION_CAM_BOTTOM:
				DisplayView((int)para0, m_iBufIdx[0], 0);
				break;
			}
		}
		else
			DisplayView((int)para0, m_iBufIdx[0], m_iMapIdx[0]);			

		m_dlgImgRect[para0]->Display();

		break;
	
	//Live on
	case 1:
#ifdef _CAM
		if(!m_bInspectStart[0]) 
		{
			if(m_bLineCamOn)
			{
				AfxMessageBox("LineScan Camera Not Live!");
				return 0;
			}

			m_bLiveOverlayDisp = FALSE;

			//20100629 lss
			switch(para0)
			{
			case 0:
				SetTimer(0, 50, NULL);

				OnOffLight(TRUE, m_bLineCamOn);
				break;
			}
		}
		HandleAllMessage();
#endif
		break;
	//Live off
	case 2:
#ifdef _CAM
		if(!m_bInspectStart[0])
		{
			//20100629 lss
			switch(para0)
			{
			case 0:
				KillTimer(0);

				OnOffLight(FALSE, m_bLineCamOn);
				break;
			}
		}

		HandleAllMessage();
#endif
		break;
	//Image load
	case 3:
		switch(para0)
		{
			case 0:
			{
				if(Hal_OpenHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", FALSE))
				{
				
					/*
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						status.Format("Halcon Error Occured at get_image_pointer1_02!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return 0;
					}
					*/
				
					if(m_bIsColor)
					{
						decompose3(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]));
					
						//20190909 by ���� ���� ��� �� ȸ�� ��Ų �����̹Ƿ� ������� ����. �ּ�ó����.
						//if(m_bLineCamOn)
							//rotate_image(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), 90, "constant");
					}

					set_check("~give_error");
					herr = get_image_pointer1(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), NULL, NULL, &wd, &ht);
					set_check("give_error");

					m_dlgImgRect[para0]->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
					
					disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
					disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				}
			}
				
			break;
		}

		break;
	//Image save
	case 4:
		//20100629 lss
		switch(para0)
		{
		case 0:
			//20190909 by ���� ���� �� ���ν�ĵ �÷� �̹��� �뷮 ������ jpeg���� ������.
			if(m_bLineCamOn)
				//Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", "jpeg 80");		
				Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", "bmp");
			else
				Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", "bmp");
			break;
		case 1:
			if(m_bLineCamOn)
				//Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, 2, 0), "", "jpeg 80");
				Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, 2, 0), "", "bmp");
			else
				Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, 2, 0), "", "bmp");
			break;
		}

		break;
	//Gray value
	case 5:
		//20100629 lss
		switch(para0)
		{
		case 0:
			m_dlgImgRect[para0]->GetGrayPos(&x, &y);
			//Get gray 

			//get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
			switch(m_lColorMode)
			{
			case 1:
				get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
				break;
			case 2:
				get_grayval(*m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
				break;
			case 3:
				get_grayval(*m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
				break;
			case 0:	
				get_grayval(*m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
				break;	
			}

			m_dlgImgRect[para0]->SetGray((Hlong)gray[0]);

			set_color(m_dlgImgRect[para0]->GetView(), HTuple("blue"));
			disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
			 //20110418 lee
			if(m_lProfileMode == 1)
			{
				for(loop=0;loop<m_lWidth[para0]-1;loop++)
				{
					//get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
					//get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
					switch(m_lColorMode)
					{
					case 1:
						get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
						get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
						break;
					case 2:
						get_grayval(*m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
						get_grayval(*m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
						break;
					case 3:
						get_grayval(*m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
						get_grayval(*m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
						break;
					case 0:	
						get_grayval(*m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
						get_grayval(*m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
						break;	
					}

					//disp_line(m_dlgImgRect[(int)para0]->GetView(), y - (Hlong)gray[0], loop, y - (Hlong)gray2[0], loop+1);				
				}
			}
			
 	        else if(m_lProfileMode == 2)
			{
				set_color(m_dlgImgRect[para0]->GetView(), HTuple("yellow")); 
				for(loop=0;loop<m_lHeight[para0]-1;loop++)
				{
					//get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
					//get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
					switch(m_lColorMode)
					{
					case 1:
						get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
						get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
						break;
					case 2:
						get_grayval(*m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
						get_grayval(*m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
						break;
					case 3:
						get_grayval(*m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
						get_grayval(*m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
						break;
					case 0:	
						get_grayval(*m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
						get_grayval(*m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
						break;	
					}

					//disp_line(m_dlgImgRect[(int)para0]->GetView(), loop, x + (Hlong)gray[0], loop+1, x + (Hlong)gray2[0]);				
				}
			}
			/*
			else if(m_lProfileMode == 3)
			{
				for(loop=0;loop<m_lWidth[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), y - (Hlong)gray[0], loop, y - (Hlong)gray2[0], loop+1);				
				}
				set_color(m_dlgImgRect[para0]->GetView(), HTuple("yellow")); 
				for(loop=0;loop<m_lHeight[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), loop,x + (Hlong)gray[0], loop+1, x + (Hlong)gray2[0]);				
				}
			}
			*/

			break;
		}

		break;
		//Region confirm
		case 6:
			
			break;
		//Test
		case 7:
			//20100629 lss
			switch((int)para0)
			{
			case 0:
				if(!m_bInspectStart[0])
				{
					m_bLiveOverlayDisp = TRUE;
					
					count_seconds(&StartSecond);
					
					typeInspectPara para;
					para = m_VisMod->GetInspectPara(0);
					para.m_bIsPunchSkip[0] = 0;
					para.m_bIsPunchSkip[1] = 0;
					m_VisMod->SetInspectPara(0, para);

					//20190916 by ���ν�ĵ �˻� �б� �߰�
					switch(m_VisMod->GetVisIndex())
					{
					case VISION_CAM_TOP1:
						m_iRslt[0] = m_VisMod->Inspection((int)para0, m_iBufIdx[0]);
						break;
					case VISION_CAM_TOP2:
						m_iRslt[0] = m_VisMod->Inspection_Linescan((int)para0, m_iBufIdx[0]);
						break; 
					case VISION_CAM_BOTTOM:
						m_iRslt[0] = m_VisMod->Inspection_Bottom((int)para0, m_iBufIdx[0]);
						break;
					default:
						AfxMessageBox("Error : invalid index(CFilmInsDlg::Test()!");
						break;
					}
					

					ins_rslt = m_VisMod->GetInspectRslt((int)para0);
//					shift_rslt = m_VisMod->GetShiftRslt((int)para0, m_iBufIdx[0]);
					
					sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
					m_label_ptime.SetCaption(temp);


					if(m_bIsColor)
					{
						switch(m_lColorMode)
						{
						case 1:
							disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 2:
							disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 3:
							disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 0:	
							disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;	
						}
					}
					else
					{
						disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
					}


					//m_VisMod->PushRsltData(0);


					
					//Set image
					m_VisMod->SetHalNGImage(0, m_iBufIdx[0], m_iNGViewIdx[0]);

					//Set result/////////////////////////////					
					//Set result region
					m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
					
					//Set result data
					m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], /*shift_rslt,*/ ins_rslt);
					

					ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, TRUE, FALSE, sys_para.m_bPara[33], sys_para.m_bPara[31]);			
					//NG view
					ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, FALSE, FALSE, sys_para.m_bPara[33], sys_para.m_bPara[31]);
					

					/*
					if(sys_para.m_bPara[33])
					{
						ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, TRUE, FALSE, TRUE ,FALSE);			
						//NG view
						ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, FALSE, FALSE, TRUE ,FALSE);
					}
					else
					{
						ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, TRUE, FALSE, FALSE ,FALSE);			
						//NG view
						ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, FALSE, FALSE, FALSE ,FALSE);
					}
					*/


					count_seconds(&EndSecond);
					EndSecond = EndSecond - StartSecond;
					sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
					m_VisMod->SetMainSysRslt(sys_rslt);
				}
				else
				{
					WriteStatusList("WARRNING : �˻� ���� �� �Դϴ�!");
				}
				break;
			}

		break;
	default:
		AfxMessageBox("Error : invalid index!");
		break;
	}

	return 0;
}


//
//USER MESSAGE END

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USER FUNCTION BEGIN
//
BOOL CFilmInsDlg::StartInspectThread()
{
	m_bInspectStart[0] = TRUE;

	HandleAllMessage();
	
	m_pThread[0] = AfxBeginThread(GrabThreadCam0, this, THREAD_PRIORITY_NORMAL);
	if (m_pThread[0]) m_pThread[0]->m_bAutoDelete = TRUE;

	m_pInspectThread[0] = AfxBeginThread(InspectThread, this, THREAD_PRIORITY_NORMAL);
	if (m_pInspectThread[0]) m_pInspectThread[0]->m_bAutoDelete = TRUE;

	return TRUE;	
}

BOOL CFilmInsDlg::StopInspectThread()
{
	m_bInspectStart[0] = FALSE;

	return TRUE;	
}

//20150225 ngh AutoDelete
UINT CFilmInsDlg::DeleteSearchThread(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;
	CString strFolder;
	
	while(TRUE)
	{
		if(pdlg->m_bDeleteFolderStart)
		{
			pdlg->m_bDeleteFolder = FALSE;
			pdlg->m_bDeleteFolderStart = FALSE;
			if(pdlg->StartAutoDelete_Img())
			{
				pdlg->m_bDeleteFolder = TRUE;
				break;
				//return 0;
			}
			else
			{
				pdlg->m_bDeleteFolder = FALSE;
				break;
				//return 0;
			}
		}
		Sleep(1);
	}

	return 0;
}


UINT CFilmInsDlg::GrabThreadCam0(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;

	typeInspectPara ins_para;
	//typeShiftRslt shift_rslt;

	while(TRUE)
	{ 
		//�˻� ���� ��ȣ ��� 
		if (!pdlg->m_bInspectStart[0]) 
		{
			AfxEndThread(0);
            break;
		}

		//Grab flag
		if( (pdlg->m_VisFlagData[0].m_bFlag[7]) && (pdlg->m_bInspectStart[0]) )//7 : grab start�� �Ǿ����� �÷��� on
		{
			pdlg->m_VisFlagData[0].m_bFlag[7] = FALSE;

			pdlg->m_VisMod->SetNGViewMode(FALSE, -1);

			//Grab Start
			ins_para = pdlg->m_VisMod->GetInspectPara(0);
			
			pdlg->GrabProcess(0, pdlg->m_iGrbIdx[0], ins_para.m_iParaBase[1], pdlg->m_bLineCamOn);

			//result display
			::SendMessage(pdlg->m_hWnd, USER_MSG_VIS, 0, pdlg->m_iGrbIdx[0]);

			//Grab�Ϸ� flag
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 3);
		}
		Sleep(1);
	}
	return 0;
}

UINT CFilmInsDlg::InspectThread(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;


	HTuple Width, Height, Pointer, Type;
	Hobject RegionRect;
	Hlong chip_offset;

	CString status;

	typeInspectPara ins_para;
	typeInspectRslt ins_rslt;
	typeSystemPara sys_para;
	sys_para = pdlg->m_VisMod->GetSystemPara(0);

	while(TRUE)
	{ 
		//�˻� ���� Flag�� Reset �Ǹ� ���� �� ������. 
		if(!pdlg->m_bInspectStart[0]) 
		{
			AfxEndThread(0);
            break;
		}

		//�ܼ� Grab and display
		if(pdlg->m_VisFlagData[0].m_bFlag[3])
		{
			ins_para = pdlg->m_VisMod->GetInspectPara(0);

			pdlg->m_VisFlagData[0].m_bFlag[3] = FALSE;

			//ù��° ��ǰã�� �� ��� �Ի� �� ���÷���
			//Grab
			//Light ON
			pdlg->OnOffLight(TRUE, pdlg->m_bLineCamOn);
			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img7_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img7_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img7_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img7_ch3);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalGreenImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalBlueImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));

			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_dlgImgRect[0]->GetView()); 

			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("green"));
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), pdlg->m_lHeight[0] * 0.5, 0, pdlg->m_lHeight[0] * 0.5, pdlg->m_lWidth[0]);
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0,pdlg-> m_lWidth[0] * 0.5, pdlg->m_lHeight[0], pdlg->m_lWidth[0] * 0.5);

			//Disp align inspect region
			gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
			disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
			gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
			disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

			//���� ���� ���� Display (48mm����)
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("red"));

			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0, (pdlg->m_lWidth[0] * 0.5)+(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							pdlg->m_lHeight[0], (pdlg->m_lWidth[0] * 0.5)+(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2));
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0, (pdlg->m_lWidth[0] * 0.5)-(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							pdlg->m_lHeight[0], (pdlg->m_lWidth[0] * 0.5)-(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2));

			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 7);
		}
		//Find Start first chip
		else if(pdlg->m_VisFlagData[0].m_bFlag[0])
		{
			pdlg->m_VisFlagData[0].m_bFlag[0] = FALSE;

			//ù��° ��ǰã�� 
			//Grab
			//Light ON
			pdlg->OnOffLight(TRUE, pdlg->m_bLineCamOn);
		
			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img7_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img7_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img7_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img7_ch3);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalGreenImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalBlueImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));

			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_dlgImgRect[0]->GetView()); 

			//Find chip(������ �Ʒ��� �˻� ���� �κ��� �ִ��� �˻�, ������ �߰��� 126�̻�)
			if(pdlg->m_VisMod->FindFirstChip(pdlg->m_dlgImgRect[0]->GetView(), pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), 50, &chip_offset))
			{
				pdlg->m_VisFlagData[0].m_dData[0] = WIDTH - chip_offset;
			}
			else
			{
				pdlg->m_VisFlagData[0].m_dData[0] = ALIGN_FAILED_DATA;
			}

			//SendPacket 
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 0);
		}
		//Align
		else if(pdlg->m_VisFlagData[0].m_bFlag[4])
		{
			ins_para = pdlg->m_VisMod->GetInspectPara(0);	

			pdlg->m_VisFlagData[0].m_bFlag[4] = FALSE;

			//Grab
			//Light ON
			pdlg->OnOffLight(TRUE, pdlg->m_bLineCamOn);

			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img0_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img0_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img0_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img0_ch3);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_VisMod->GetHalGreenImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_VisMod->GetHalBlueImage(0, 0, ins_para.m_iParaMapAlign[0]));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]));

			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_dlgImgRect[0]->GetView()); 


			//Init
			pdlg->m_VisMod->ResetInspectRslt(0); 

			//�˻� 
 			if(pdlg->m_VisMod->InspectionFirstAlign(0, 0)) 
			{
				ins_rslt = pdlg->m_VisMod->GetInspectRslt(0); 

				if(1 == ins_rslt.m_iRsltAlign[0])
				{
					set_color(pdlg->m_dlgImgRect[0]->GetView(), "green");
					set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
					write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(ins_rslt.m_dScoreAlign[0][0] * 100)));
					set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 40 + (0 * 200), 10);
					write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(ins_rslt.m_dScoreAlign[0][1] * 100)));

					pdlg->m_VisFlagData[0].m_dData[1] = ins_rslt.m_dOffsetX[0];

					//Disp inspect region
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView());
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView());

					::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1); //Align ����
				}
				else
				{
					set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
					write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : can't find model!")));

					pdlg->m_VisFlagData[0].m_dData[1] = ALIGN_FAILED_DATA;

					//Disp inspect region
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

					::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1);	//Align ����
				}
			}
			else
			{
				//Fail
				set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
				set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
				write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : can't find model!")));

				pdlg->m_VisFlagData[0].m_dData[1] = ALIGN_FAILED_DATA;

				//Disp inspect region
				gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
				disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
				gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
				disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

				::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1);	//Align ����
			}
		}
		//Main process
		else if(pdlg->m_VisFlagData[0].m_bFlag[10])
		{
			pdlg->m_VisFlagData[0].m_bFlag[10] = FALSE;

			//Process
			pdlg->InspectProcess(0, pdlg->m_iPrcIdx[0], pdlg->m_iMapIdx[0]);

			::SendMessage(pdlg->m_hWnd, USER_MSG_VIS, 1, pdlg->m_iPrcIdx[0]); 

			//Align offset
			ins_rslt = pdlg->m_VisMod->GetInspectRslt(0); 
			pdlg->m_VisFlagData[0].m_dData[2] = ins_rslt.m_dOffsetX[0];//�����ϸ� 0���� �Ѿ�´�.

			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 2);
		}

		//NG view
		if(pdlg->m_bInspectStartNGView[0])
		{
			pdlg->m_bInspectStartNGView[0] = FALSE;
			pdlg->NGViewDisp();
			
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 10);

		}

		Sleep(1);
	}

	return 0;
}

//lineCamOn �߰�
void CFilmInsDlg::GrabProcess(int cam_idx, int grab_idx, int img_cnt, BOOL lineCamOn, BOOL bTest)
{
	int i = 0;
	//CString status;
	
	HTuple StartSecond, EndSecond;
	count_seconds(&StartSecond);	

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);

	//status.Format("Grab Start %d_%d", cam_idx, grab_idx);
	//WriteStatusList((LPCTSTR)status);


	//m_dlgImgRect[0]->ClickBtnenhctrlZoomfit();

#ifdef _CAM
	if(lineCamOn)	//���ν�ĵ �׷�
	{			

		SetLineLight(m_iLightChIdx[0], m_img0_ch0);
		SetLineLight(m_iLightChIdx[1], m_img0_ch1);
		SetLineLight(m_iLightChIdx[2], m_img0_ch2);

		::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 11);

		m_cLineCam.Grab(cam_idx, grab_idx, 0, m_VisMod->GetHalImage(cam_idx, grab_idx, 0),
						m_VisMod->GetHalRedImage(cam_idx, grab_idx, 0), 
						m_VisMod->GetHalGreenImage(cam_idx, grab_idx, 0),
						m_VisMod->GetHalBlueImage(cam_idx, grab_idx, 0));

		if(bTest)
			::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 11);
	}
	else	//����� �׷�
	{
		//Light on
		OnOffLight(TRUE, m_bLineCamOn);

		for(i = 0; (i < img_cnt) && (i < IMAGE_MAP_MAX_NUM ); i++)
		{
			switch(i)
			{
			case 0:
				SetLight(m_iLightChIdx[0], m_img0_ch0);
				SetLight(m_iLightChIdx[1], m_img0_ch1);
				SetLight(m_iLightChIdx[2], m_img0_ch2);
				SetLight(m_iLightChIdx[3], m_img0_ch3);
				break;
			case 1:
				SetLight(m_iLightChIdx[0], m_img1_ch0);
				SetLight(m_iLightChIdx[1], m_img1_ch1);
				SetLight(m_iLightChIdx[2], m_img1_ch2);
				SetLight(m_iLightChIdx[3], m_img1_ch3);
				break;
			case 2:
				SetLight(m_iLightChIdx[0], m_img2_ch0);
				SetLight(m_iLightChIdx[1], m_img2_ch1);
				SetLight(m_iLightChIdx[2], m_img2_ch2);
				SetLight(m_iLightChIdx[3], m_img2_ch3);
				break;
			case 3:
				SetLight(m_iLightChIdx[0], m_img3_ch0);
				SetLight(m_iLightChIdx[1], m_img3_ch1);
				SetLight(m_iLightChIdx[2], m_img3_ch2);
				SetLight(m_iLightChIdx[3], m_img3_ch3);
				break;
			case 4:
				SetLight(m_iLightChIdx[0], m_img4_ch0);
				SetLight(m_iLightChIdx[1], m_img4_ch1);
				SetLight(m_iLightChIdx[2], m_img4_ch2);
				SetLight(m_iLightChIdx[3], m_img4_ch3);
				break;
			case 5:
				SetLight(m_iLightChIdx[0], m_img5_ch0);
				SetLight(m_iLightChIdx[1], m_img5_ch1);
				SetLight(m_iLightChIdx[2], m_img5_ch2);
				SetLight(m_iLightChIdx[3], m_img5_ch3);
				break;
			case 6:
				SetLight(m_iLightChIdx[0], m_img6_ch0);
				SetLight(m_iLightChIdx[1], m_img6_ch1);
				SetLight(m_iLightChIdx[2], m_img6_ch2);
				SetLight(m_iLightChIdx[3], m_img6_ch3);
				break;
			case 7:
				SetLight(m_iLightChIdx[0], m_img7_ch0);
				SetLight(m_iLightChIdx[1], m_img7_ch1);
				SetLight(m_iLightChIdx[2], m_img7_ch2);
				SetLight(m_iLightChIdx[3], m_img7_ch3);
				break;
			case 8:
				SetLight(m_iLightChIdx[0], m_img8_ch0);
				SetLight(m_iLightChIdx[1], m_img8_ch1);
				SetLight(m_iLightChIdx[2], m_img8_ch2);
				SetLight(m_iLightChIdx[3], m_img8_ch3);
				break;
			case 9:
				SetLight(m_iLightChIdx[0], m_img9_ch0);
				SetLight(m_iLightChIdx[1], m_img9_ch1);
				SetLight(m_iLightChIdx[2], m_img9_ch2);
				SetLight(m_iLightChIdx[3], m_img9_ch3);
				break;
			case 10:
				SetLight(m_iLightChIdx[0], m_img10_ch0);
				SetLight(m_iLightChIdx[1], m_img10_ch1);
				SetLight(m_iLightChIdx[2], m_img10_ch2);
				SetLight(m_iLightChIdx[3], m_img10_ch3);
				break;
			case 11:
				SetLight(m_iLightChIdx[0], m_img11_ch0);
				SetLight(m_iLightChIdx[1], m_img11_ch1);
				SetLight(m_iLightChIdx[2], m_img11_ch2);
				SetLight(m_iLightChIdx[3], m_img11_ch3);
				break;
			}

			if(m_bIsColor)
				m_cCam.Grab(cam_idx, grab_idx, i, m_VisMod->GetHalImage(cam_idx, grab_idx, i), m_bIsColor, 
							m_VisMod->GetHalRedImage(cam_idx, grab_idx, i), m_VisMod->GetHalGreenImage(cam_idx, grab_idx, i), m_VisMod->GetHalBlueImage(cam_idx, grab_idx, i));
			else
				m_cCam.Grab(cam_idx, grab_idx, i, m_VisMod->GetHalImage(cam_idx, grab_idx, i));
		}

		OnOffLight(FALSE, m_bLineCamOn);
	}

#endif

	//status.Format("Grab End %d_%d", cam_idx, grab_idx);
	//WriteStatusList((LPCTSTR)status);
	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	m_lGrabTime = (long)(((double)EndSecond[0]) * 1000);
}


void CFilmInsDlg::InspectProcess(int cam_idx, int prc_idx, int map_Idx)
{
	//CString status;
	HTuple StartSecond, EndSecond;
	
	count_seconds(&StartSecond);

	//NG review data
	memset(m_sNGLotID[0], NULL, sizeof(m_sNGLotID));
	memset(m_sNGDate[0], NULL, sizeof(m_sNGDate));
	m_iNGDeviceIdx[0] = 0;

	m_iRslt[0] = 1;

	//20190916 by ���ν�ĵ �˻� �б� �߰�
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		m_iRslt[0] = m_VisMod->Inspection(cam_idx, prc_idx);

		disp_obj(*m_VisMod->GetHalImage(0, prc_idx, map_Idx), m_dlgImgRect[0]->GetView());			//20191204 by ī�޶� �� �̹��� ��� ���� �߰�
		break;
	case VISION_CAM_TOP2:
		m_iRslt[0] = m_VisMod->Inspection_Linescan(cam_idx, prc_idx);

		if(1 != m_iRslt[0])
		{
			//NG �˻� ����� ����Ʈ�� ��´�			//20191101 by
			m_VisMod->PushRsltData(0);
		}

		disp_obj(*m_VisMod->GetHalRedImage(0, prc_idx, map_Idx), m_dlgImgRect[0]->GetView());

		break; 
	case VISION_CAM_BOTTOM:
		m_iRslt[0] = m_VisMod->Inspection_Bottom(cam_idx, prc_idx);

		disp_obj(*m_VisMod->GetHalImage(0, prc_idx, map_Idx), m_dlgImgRect[0]->GetView());
		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
		break;
	}
			
	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	m_lProcessTime = (long)(((double)EndSecond[0]) * 1000);
}

void CFilmInsDlg::InspectResult(int grab_proc_type, int buf_idx)
{
	char temp[100];
	int i;
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();
	
	typeInspectRslt ins_rslt;
	ins_rslt = m_VisMod->GetInspectRslt(0);
	
	//typeShiftRslt ������� ����
//	typeShiftRslt shift_rslt;
//	shift_rslt = m_VisMod->GetShiftRslt(0, buf_idx);
	
	typeLotInfo lot_info;
	
	switch(grab_proc_type)
	{
	case 0:
		sys_rslt.m_lCnt[4] = m_lGrabTime;
		break;
		//Process
	case 1:
		//Lot info.
		lot_info = m_VisMod->GetLotInfo();

		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			//total
			sys_rslt.m_lCnt[1]++;
			lot_info.m_lTot++;
			
			if(1 == ins_rslt.m_iRslt[i])
			{
				if(2 == ins_rslt.m_iRsltChipFind[i])
				{
					//punch
					lot_info.m_lPunch++;
				}
				else
				{
					//good
					lot_info.m_lGood++;
				}
				
				//good
				sys_rslt.m_lCnt[2]++;
			}
			else
			{
				//ng
				sys_rslt.m_lCnt[3]++;
				lot_info.m_lNG++;
			}
		}
		sys_rslt.m_lCnt[5] = m_lProcessTime;
		
		lot_info.m_fRatio = (int)( ((float)lot_info.m_lGood) / ((float)(lot_info.m_lGood + lot_info.m_lNG)) * 100000 );
		lot_info.m_fRatio = lot_info.m_fRatio * 0.001;
		m_VisMod->SetLotInfo(lot_info);
		LotDisp(lot_info);

		//Set result/////////////////////////////
		m_iNGViewIdx[0]++;

		if(NG_VIEW_MAX_NUM <= m_iNGViewIdx[0])
		{
			m_iNGViewIdx[0] = 0;
		}

		//Set image
		m_VisMod->SetHalNGImage(0, buf_idx, m_iNGViewIdx[0]);
		//Set result region
		//(int cam_idx, int ng_view_idx)
		m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
		//Set result data
		m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], /*shift_rslt,*/ ins_rslt); // ����� �ʿ� (vision module ���� shift_rslt ���� �Ұ�)
		
		//Display
		//(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, long ng_view_start)

		ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, TRUE, FALSE, sys_para.m_bPara[33], sys_para.m_bPara[31]);			
		//NG view
		ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, FALSE, FALSE, sys_para.m_bPara[33] , sys_para.m_bPara[31]);


		//��� ���÷��� �ɼ� �������� ���÷��� �Ѵ�. //20110509 lee
		/*
		if(sys_para.m_bPara[33])
		{
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, TRUE, FALSE, TRUE ,TRUE);			
			//NG view
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, FALSE, FALSE, TRUE ,TRUE);
		}
		else
		{
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, TRUE, FALSE, FALSE ,TRUE);			
			//NG view
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, FALSE, FALSE, FALSE ,TRUE);
		}
		*/

	
		break;
	}

	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);

	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[4]);
	m_label_gtime.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);
	
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	//Monitoring
	sprintf(temp, "%d", m_iGrbIdx[0]);
	GetDlgItem(IDC_STATIC_GRABIDX)->SetWindowText(temp);
	
	sprintf(temp, "%d", m_iBufIdx[0]);
	GetDlgItem(IDC_STATIC_BUFFERIDX)->SetWindowText(temp);
	
	sprintf(temp, "%d", m_iPrcIdx[0]);
	GetDlgItem(IDC_STATIC_PRCESSIDX)->SetWindowText(temp);

	sprintf(temp, "%d", m_iNGViewIdx[0]);
	GetDlgItem(IDC_STATIC_NGVIEWIDX)->SetWindowText(temp);
}

BOOL CFilmInsDlg::WriteLogStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
#ifdef _CAM
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
#endif

	return TRUE;
}


BOOL CFilmInsDlg::WriteLogAVI(CString path, CString file, CString bak_file, CString log, long limit_line)
{
#ifdef _CAM
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

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
			AfxMessageBox("Error : log file open fail!(WriteLogAVI)" );
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
	if(0 == line)
	{
	    result = fseek(stream, 0L, SEEK_END);
		if(result)
		{
			fclose(stream);
			return FALSE;
		}
		memset(szTime, NULL, sizeof(szTime));
		sprintf(szTime, "PC,CAM,LOTID,DATE,TIME,DEVICE_INDEX,DEFECT_INDEX,DEFECT_TYPE,AREA,SIZEX,SIZEY,POSX,POSY");
		fputs(szTime, stream);
		fputs("\n", stream);	
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
#endif
	return TRUE;
}

void CFilmInsDlg::WriteStatusList(LPCTSTR lpszItem)
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

double CFilmInsDlg::CalculateContrastValues()
{
	// Setting parameter
	int AREA_SIZE = 40;				// üũ �ϴ� ������ ũ�� width & height = AREA_SIZE x 2
	int CHECK_CNT = 5;				// ���� ���� CHECK_CNT x CHECK_CNT ���� ���� ���
	HTuple ldGray = HTuple(6);
	HTuple direction = HTuple(0);

	int colSize = m_lWidth[0] / (CHECK_CNT+1);
	int rowSize = m_lHeight[0] / (CHECK_CNT+1);
	HTuple total;
	HRegion rect;
	double contrast = .0;
	double tmp = .0;

	set_color(m_dlgImgRect[0]->GetView(), HTuple("red"));

	total = HTuple(.0);
	for(int col = 1 ; col < CHECK_CNT + 1; col++)
	{
		int cCol = colSize * col;
		HTuple contrastSum = HTuple(.0);
		for(int row = 1; row < CHECK_CNT + 1; row++)
		{
			int cRow = rowSize * row;
			rect = HRegion::GenRectangle1(cRow - AREA_SIZE, cCol - AREA_SIZE, cRow + AREA_SIZE, cCol + AREA_SIZE);
			rect.DispRegion(m_dlgImgRect[0]->GetView());

			rect.CoocFeatureImage(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]),
				ldGray,
				direction,
				&tmp, // ignore Correlation
				&tmp, // ignore Homogeneity
				&contrast);

			//mean = rect.Intensity(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), &deviation);
			set_tposition(m_dlgImgRect[0]->GetView(), cRow + AREA_SIZE, cCol);
			write_string(m_dlgImgRect[0]->GetView(), HTuple(contrast));
			contrastSum = contrastSum + contrast;
		}
		set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, cCol);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(contrastSum / HTuple(CHECK_CNT)));
		total = total + contrastSum;
	}

	total = total/HTuple(CHECK_CNT*CHECK_CNT);

	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Column Avg:"));
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 100, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Total Avg: ").Concat(total));

	double result;
	tuple_real(total, &result);
	return result;
}


int CFilmInsDlg::CalculateGrayValues()
{
	// Setting parameter
	int AREA_SIZE = 40;				// üũ �ϴ� ������ ũ�� width & height = AREA_SIZE x 2
	int CHECK_CNT = 5;				// ���� ���� CHECK_CNT x CHECK_CNT ���� ���� ���

	int colSize = m_lWidth[0] / (CHECK_CNT+1);
	int rowSize = m_lHeight[0] / (CHECK_CNT+1);
	HTuple mean, total;
	HRegion rect;
	double deviation = .0;

	set_color(m_dlgImgRect[0]->GetView(), HTuple("orange red"));

	total = HTuple(.0);
	for(int col = 1 ; col < CHECK_CNT + 1; col++)
	{
		int cCol = colSize * col;
		HTuple meanSum = HTuple(.0);
		for(int row = 1; row < CHECK_CNT + 1; row++)
		{
			int cRow = rowSize * row;
			rect = HRegion::GenRectangle1(cRow - AREA_SIZE, cCol - AREA_SIZE, cRow + AREA_SIZE, cCol + AREA_SIZE);
			rect.DispRegion(m_dlgImgRect[0]->GetView());
			mean = rect.Intensity(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), &deviation);
			set_tposition(m_dlgImgRect[0]->GetView(), cRow + AREA_SIZE, cCol);
			write_string(m_dlgImgRect[0]->GetView(), HTuple(mean));
			meanSum = meanSum + mean;
		}
		set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, cCol);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(meanSum / HTuple(CHECK_CNT)));
		total = total + meanSum;
	}
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Column Avg:"));
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 100, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Total Avg: ").Concat(total/HTuple(CHECK_CNT*CHECK_CNT)));

	Hlong gray_total;
	tuple_round(total, &gray_total);
	return (int) gray_total / (CHECK_CNT*CHECK_CNT);
}

void CFilmInsDlg::UpdateLight(BOOL update)
{
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	if(update)
	{
		//Map#0
		ins_para.m_iLightVar[0][0] = m_img0_ch0;//use 1 instead of 0
		ins_para.m_iLightVar[0][1] = m_img0_ch1;
		ins_para.m_iLightVar[0][2] = m_img0_ch2;
		ins_para.m_iLightVar[0][3] = m_img0_ch3;

		//Map#1
		ins_para.m_iLightVar[1][0] = m_img1_ch0;
		ins_para.m_iLightVar[1][1] = m_img1_ch1;
		ins_para.m_iLightVar[1][2] = m_img1_ch2;
		ins_para.m_iLightVar[1][3] = m_img1_ch3;

		//Map#2
		ins_para.m_iLightVar[2][0] = m_img2_ch0;
		ins_para.m_iLightVar[2][1] = m_img2_ch1;
		ins_para.m_iLightVar[2][2] = m_img2_ch2;
		ins_para.m_iLightVar[2][3] = m_img2_ch3;

		//Map#3
		ins_para.m_iLightVar[3][0] = m_img3_ch0;
		ins_para.m_iLightVar[3][1] = m_img3_ch1;
		ins_para.m_iLightVar[3][2] = m_img3_ch2;
		ins_para.m_iLightVar[3][3] = m_img3_ch3;

		//Map#4
		ins_para.m_iLightVar[4][0] = m_img4_ch0;
		ins_para.m_iLightVar[4][1] = m_img4_ch1;
		ins_para.m_iLightVar[4][2] = m_img4_ch2;
		ins_para.m_iLightVar[4][3] = m_img4_ch3;

		//Map#5
		ins_para.m_iLightVar[5][0] = m_img5_ch0;
		ins_para.m_iLightVar[5][1] = m_img5_ch1;
		ins_para.m_iLightVar[5][2] = m_img5_ch2;
		ins_para.m_iLightVar[5][3] = m_img5_ch3;

		//Map#6
		ins_para.m_iLightVar[6][0] = m_img6_ch0;
		ins_para.m_iLightVar[6][1] = m_img6_ch1;
		ins_para.m_iLightVar[6][2] = m_img6_ch2;
		ins_para.m_iLightVar[6][3] = m_img6_ch3;

		//Map#7
		ins_para.m_iLightVar[7][0] = m_img7_ch0;
		ins_para.m_iLightVar[7][1] = m_img7_ch1;
		ins_para.m_iLightVar[7][2] = m_img7_ch2;
		ins_para.m_iLightVar[7][3] = m_img7_ch3;

		//Map#8
		ins_para.m_iLightVar[8][0] = m_img8_ch0;
		ins_para.m_iLightVar[8][1] = m_img8_ch1;
		ins_para.m_iLightVar[8][2] = m_img8_ch2;
		ins_para.m_iLightVar[8][3] = m_img8_ch3;

		//Map#9
		ins_para.m_iLightVar[9][0] = m_img9_ch0;
		ins_para.m_iLightVar[9][1] = m_img9_ch1;
		ins_para.m_iLightVar[9][2] = m_img9_ch2;
		ins_para.m_iLightVar[9][3] = m_img9_ch3;

		//Map#10
		ins_para.m_iLightVar[10][0] = m_img10_ch0;
		ins_para.m_iLightVar[10][1] = m_img10_ch1;
		ins_para.m_iLightVar[10][2] = m_img10_ch2;
		ins_para.m_iLightVar[10][3] = m_img10_ch3;

		//Map#11
		ins_para.m_iLightVar[11][0] = m_img11_ch0;
		ins_para.m_iLightVar[11][1] = m_img11_ch1;
		ins_para.m_iLightVar[11][2] = m_img11_ch2;
		ins_para.m_iLightVar[11][3] = m_img11_ch3;

		m_VisMod->SetInspectPara(0, ins_para);
	}
	else
	{
		//Map#0
		m_img0_ch0 = ins_para.m_iLightVar[0][0];//use 1 instead of 0
		m_img0_ch1 = ins_para.m_iLightVar[0][1];
		m_img0_ch2 = ins_para.m_iLightVar[0][2];
		m_img0_ch3 = ins_para.m_iLightVar[0][3];

		//Map#1
		m_img1_ch0 = ins_para.m_iLightVar[1][0];
		m_img1_ch1 = ins_para.m_iLightVar[1][1];
		m_img1_ch2 = ins_para.m_iLightVar[1][2];
		m_img1_ch3 = ins_para.m_iLightVar[1][3];

		//Map#2
		m_img2_ch0 = ins_para.m_iLightVar[2][0];
		m_img2_ch1 = ins_para.m_iLightVar[2][1];
		m_img2_ch2 = ins_para.m_iLightVar[2][2];
		m_img2_ch3 = ins_para.m_iLightVar[2][3];

		//Map#3
		m_img3_ch0 = ins_para.m_iLightVar[3][0];
		m_img3_ch1 = ins_para.m_iLightVar[3][1];
		m_img3_ch2 = ins_para.m_iLightVar[3][2];
		m_img3_ch3 = ins_para.m_iLightVar[3][3];

		//Map#4
		m_img4_ch0 = ins_para.m_iLightVar[4][0];
		m_img4_ch1 = ins_para.m_iLightVar[4][1];
		m_img4_ch2 = ins_para.m_iLightVar[4][2];
		m_img4_ch3 = ins_para.m_iLightVar[4][3];

		//Map#5
		m_img5_ch0 = ins_para.m_iLightVar[5][0];
		m_img5_ch1 = ins_para.m_iLightVar[5][1];
		m_img5_ch2 = ins_para.m_iLightVar[5][2];
		m_img5_ch3 = ins_para.m_iLightVar[5][3];

		//Map#6
		m_img6_ch0 = ins_para.m_iLightVar[6][0];
		m_img6_ch1 = ins_para.m_iLightVar[6][1];
		m_img6_ch2 = ins_para.m_iLightVar[6][2];
		m_img6_ch3 = ins_para.m_iLightVar[6][3];

		//Map#7
		m_img7_ch0 = ins_para.m_iLightVar[7][0];
		m_img7_ch1 = ins_para.m_iLightVar[7][1];
		m_img7_ch2 = ins_para.m_iLightVar[7][2];
		m_img7_ch3 = ins_para.m_iLightVar[7][3];

		//Map#8
		m_img8_ch0 = ins_para.m_iLightVar[8][0];
		m_img8_ch1 = ins_para.m_iLightVar[8][1];
		m_img8_ch2 = ins_para.m_iLightVar[8][2];
		m_img8_ch3 = ins_para.m_iLightVar[8][3];

		//Map#9
		m_img9_ch0 = ins_para.m_iLightVar[9][0];
		m_img9_ch1 = ins_para.m_iLightVar[9][1];
		m_img9_ch2 = ins_para.m_iLightVar[9][2];
		m_img9_ch3 = ins_para.m_iLightVar[9][3];

		//Map#10
		m_img10_ch0 = ins_para.m_iLightVar[10][0];
		m_img10_ch1 = ins_para.m_iLightVar[10][1];
		m_img10_ch2 = ins_para.m_iLightVar[10][2];
		m_img10_ch3 = ins_para.m_iLightVar[10][3];

		//Map#11
		m_img11_ch0 = ins_para.m_iLightVar[11][0];
		m_img11_ch1 = ins_para.m_iLightVar[11][1];
		m_img11_ch2 = ins_para.m_iLightVar[11][2];
		m_img11_ch3 = ins_para.m_iLightVar[11][3];

		UpdateData(update);
	}
}

void CFilmInsDlg::OnOffLight(BOOL LightOn, BOOL bUseLine)
{
	if(bUseLine)
	{
		if(m_cSerial.IsOpen())
		{
			char data[128];
			memset(data, NULL, sizeof(char) * 128);

			if(LightOn)
				sprintf(data, ":1W%d%d%d%d%d\r\n", m_iLightChIdx[0] + 70, 1, 1, 1, 1);
			else
				sprintf(data, ":1W%d%d%d%d%d\r\n", m_iLightChIdx[0] + 70, 0, 0, 0, 0);

			//20190910 by �����ڿ��� ���� �����ߴ� ���� ��������
			/*
			if(LightOn)
				sprintf(data, "setonex ffffffff%c", 0x0D);
			else
				sprintf(data, "setonex 00000000%c", 0x0D);
			*/
			
			m_cSerial.Write(data, strlen(data));
		}
	}
	else
	{
		//Light OFF
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], LightOn);
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], LightOn);
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], LightOn);

		if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], LightOn);
	}

	
}

void CFilmInsDlg::SetLineLight(int ch, int var)
{
	if(m_cSerial.IsOpen())
	{
		char data[128];
		memset(data, NULL, sizeof(char) * 128);

		sprintf(data, ":1W%d%03d\r\n", ch, var);
		
		//sprintf(data, "setbrightness %d %d%c", ch, var, 0x0D);	//20190910 by �����ڿ��� ���� �����ߴ� ���� ��������
		
		m_cSerial.Write(data, strlen(data));
	}
}


BYTE CFilmInsDlg::byValue2Hexa_High(BYTE byData)
{
	BYTE byValue1 = (byData >> 4);

	return byValue2NibbleAscii(byValue1);
}

BYTE CFilmInsDlg::byValue2Hexa_Low(BYTE byData)
{
	BYTE byValue1 = (byData & 0x0f);

	return byValue2NibbleAscii(byValue1);
}

BYTE CFilmInsDlg::byValue2NibbleAscii(BYTE byData)
{
	if((0 <= byData) && (byData <=9))
	{
		return byData + '0';
	}

	if(byData == 10)	byData = 'A';
	if(byData == 11)	byData = 'B';
	if(byData == 12)	byData = 'C';
	if(byData == 13)	byData = 'D';
	if(byData == 14)	byData = 'E';
	if(byData == 15)	byData = 'F';
	return byData;

	return '*';
}

void CFilmInsDlg::SetLight(int ch, int var)
{
	int rslt = 0;
	int bit = 128;
	int stNum = 0;
	HANDLE hDevice = NULL;

	if(ch < 0 || ch > 6)
		return;

	switch(ch)
	{
	case 0:	// 0 ~ 6 ch = m_hComiDevice64
		stNum = 8;
		hDevice = m_hComiDevice64;
		break;
	case 1: 
		stNum = 17;
		hDevice = m_hComiDevice64;
		break;
	case 2:
		stNum = 26;
		hDevice = m_hComiDevice64;
		break;
	case 3:
		stNum = 35;
		hDevice = m_hComiDevice64;
		break;
	case 4:
		stNum = 44;
		hDevice = m_hComiDevice64;
		break;
	case 5:
		stNum = 53;
		hDevice = m_hComiDevice64;
		break;
	case 6:
		stNum = 62;
		hDevice = m_hComiDevice64;
		break;
	case 7:	// 7 ~ 9 ch = m_hComiDevice32
		//stNum = 8;
		//hDevice = m_hComiDevice32;
		break;
	case 8:
		//stNum = 17;
		//hDevice = m_hComiDevice32;
		break;
	case 9:
		//stNum = 26;
		//hDevice = m_hComiDevice32;
		break;
	default:
		AfxMessageBox("Error : Invaild Light Index (CFilmInsDlg::SetLight())");
		return;

	}

	for(int i = 0; i < 8; i++)
	{
		rslt = var / bit;
		COMI_DO_PutOne(hDevice, stNum - i, !rslt);
		var = var - (rslt * bit);

		bit /= 2;
	}
}

void CFilmInsDlg::UpdateMapButton(int idx)
{
	m_map0.SetForeColor(RGB(255, 255, 255));
	m_map1.SetForeColor(RGB(255, 255, 255));
	m_map2.SetForeColor(RGB(255, 255, 255));
	m_map3.SetForeColor(RGB(255, 255, 255));
	m_map4.SetForeColor(RGB(255, 255, 255));
	m_map5.SetForeColor(RGB(255, 255, 255));
	m_map6.SetForeColor(RGB(255, 255, 255));
	m_map7.SetForeColor(RGB(255, 255, 255));

	switch(idx)
	{
	case 0:
		m_map0.SetForeColor(RGB(255, 255, 0));  
		break;
	case 1:
		m_map1.SetForeColor(RGB(255, 255, 0));  
		break;
	case 2:
		m_map2.SetForeColor(RGB(255, 255, 0));  
		break;
	case 3:
		m_map3.SetForeColor(RGB(255, 255, 0));  
		break;
	case 4:
		m_map4.SetForeColor(RGB(255, 255, 0));  
		break;
	case 5:
		m_map5.SetForeColor(RGB(255, 255, 0));  
		break;
	case 6:
		m_map6.SetForeColor(RGB(255, 255, 0));  
		break;
	case 7:
		m_map7.SetForeColor(RGB(255, 255, 0));  
		break;
	}
}

void CFilmInsDlg::ModelSave()
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
		m_dlgProcessing.ShowWindow(SW_SHOW);
		HandleAllMessage();
		UpdateLight(TRUE);
		//Save setup INI file
		if( m_VisMod->SaveInspectPara(moddlg->GetPath()) )
		{
			m_VisMod->SaveInspectSelectPara(moddlg->GetPath());
		}
		else
		{
			WriteStatusList("Error : can't save setup parameter file!");
		}

		m_dlgProcessing.ShowWindow(SW_HIDE);
	}

	delete moddlg;

}

void CFilmInsDlg::CheckNGKind(int cam_idx, int buf_idx)
{
	//NG type�� ��� ����(no display).
	ResultCheck_Display(cam_idx, buf_idx, m_iNGViewIdx[0], FALSE, FALSE, TRUE, FALSE ,FALSE); //20110509 lee
}

void CFilmInsDlg::LotDisp(typeLotInfo lot)
{
	char tmp[200];

	typeLotInfo lot_info;

	lot_info = m_VisMod->GetLotInfo();

	memset(tmp, NULL, sizeof(tmp));

	sprintf(tmp, "%s", lot_info.m_sNo);
	GetDlgItem(IDC_STATIC_LOTNO)->SetWindowText(tmp);
	
	sprintf(tmp, "%s", lot_info.m_sStart);
	GetDlgItem(IDC_STATIC_LOT_STIME)->SetWindowText(tmp);

	sprintf(tmp, "%s", lot_info.m_sEnd);
	GetDlgItem(IDC_STATIC_LOT_ETIME)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lDeviceIdx);
	GetDlgItem(IDC_STATIC_LOT_DEVICE)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lTot);
	GetDlgItem(IDC_STATIC_LOT_TOT)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lGood);
	GetDlgItem(IDC_STATIC_LOT_GOOD)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lNG);
	GetDlgItem(IDC_STATIC_LOT_NG)->SetWindowText(tmp);

	sprintf(tmp, "%f", lot_info.m_fRatio);
	GetDlgItem(IDC_STATIC_LOT_RATIO)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lPunch);
	GetDlgItem(IDC_STATIC_LOT_PUNCH)->SetWindowText(tmp);
}


void CFilmInsDlg::WriteLot(typeLotInfo lot_info)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\LOT_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	memset(temp, NULL, sizeof(temp));

	memset(name, NULL, sizeof(temp));
	sprintf(name, "Lot_Log%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	sprintf(temp, "No. : %s", lot_info.m_sNo);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);	

	sprintf(temp, "START TIME : %s", lot_info.m_sStart);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "END TIME : %s", lot_info.m_sEnd);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "DEVICE ID : %ld", lot_info.m_lDeviceIdx);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "TOTAL DEVICE : %ld", lot_info.m_lTot);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "GOOD DEVICE : %ld", lot_info.m_lGood);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "NG DEVICE : %ld", lot_info.m_lNG);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "RATIO(GOOD/NG) : %f", lot_info.m_fRatio);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "PUNCH DEVICED : %ld", lot_info.m_lPunch);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);
}

BOOL CFilmInsDlg::CheckDeleteDate()
{
	CString str;
	CTime now_time = CTime::GetCurrentTime();
	int temp_year, temp_month, temp_day;
	CTime delete_time;
	int TempDeleteDay=0;
	int i=0;
	temp_year=0;
	temp_month=0;
	temp_day=0;

	temp_day = now_time.GetDay();
	temp_month = now_time.GetMonth();
	temp_year = now_time.GetYear();

	m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(0);

	int save_day = m_dlgSetup->m_SysPara.m_iPara[0];
	if( (temp_day - save_day) < 0)
	{
		if(now_time.GetMonth() == 2 || now_time.GetMonth() == 4 || now_time.GetMonth() == 6 ||
			now_time.GetMonth() == 8 || now_time.GetMonth() == 9 || now_time.GetMonth() == 11)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 5 || now_time.GetMonth() == 7 || now_time.GetMonth() == 10 || now_time.GetMonth() == 12)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 30 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 3)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 28 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
	}
	else if( (temp_day - save_day) == 0)
	{
		if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 0;
		}
		else
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 0;
		}
	}
	else
	{
		m_iDel_Year = now_time.GetYear();
		m_iDel_Month = now_time.GetMonth();
		m_iDel_RecentDay = temp_day - save_day;
	}
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  ���� �Ⱓ�� ������ ���� ���
	int save_month = m_dlgSetup->m_SysPara.m_iPara[0];
	if( (temp_month - save_month) < 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		for(i = 0; i <= (save_month - temp_month); i++)
		{
			if(temp_month == i+1)
			{
				m_iDel_Month = 12 - (save_month - temp_month);
			}
		}
	}
	else if( (temp_month - save_month) == 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		m_iDel_Month = 12;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		m_iDel_Month = temp_month - save_month;
	}
*/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	/*  //���� �Ⱓ 3���� �� ���
	if( (temp_month - 2) <= 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		if(temp_month == 1)
			m_iDel_Month = 10;
		else if(temp_month == 2)
			m_iDel_Month = 11;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		if(temp_month == 3)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
		}
		else
			m_iDel_Month = temp_month - 3;
	}
	*/
	return TRUE;
}

BOOL CFilmInsDlg::DeleteFolderSearch(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				return FALSE;
			}
		}
	}
	ff.Close();

	return TRUE;

}

BOOL CFilmInsDlg::DeleteFolder(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				DeleteFolder(Filename);
				RemoveDirectory(Filename);
			}
			else
			{
				DeleteFile(Filename);
			}
		}
	}
	ff.Close();

	return TRUE;
}


BOOL CFilmInsDlg::AutoTeachOCV()
{
	BOOL tot_rslt = TRUE;
	int rslt = 1;
	double rx, ry, rscore;
	Hlong rn;
	double offset_x, offset_y;
	Hobject ModImage, Shape, Region;
	CRect rect;
	double cx, cy;

	//Set inspect data
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
	m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(0);

	disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_dlgSetup->m_InsPara.m_iParaMapOCV[0]), m_dlgImgRect[0]->GetView());
	
	//Check punching & align
	rslt = m_VisMod->InspectionAlignForAutoOCVTeach(0, m_iBufIdx[0], &ModImage, &Shape, 
												   &rx, &ry, &rscore, &rn,
												   &offset_x, &offset_y);

	if(1 == rslt)
	{
		//Total char center
		cx = m_dlgSetup->m_InsPara.m_dCharCenterX[m_dlgSetup->m_InsPara.m_iParaOCV[2]];
		cy = m_dlgSetup->m_InsPara.m_dCharCenterY[m_dlgSetup->m_InsPara.m_iParaOCV[2]];

		rect = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1];

  		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].top = rect.top + offset_y;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].left = rect.left + offset_x;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].bottom = rect.bottom + offset_y;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].right = rect.right + offset_x;
		
		//Teach
		if(m_dlgSetup->m_SetUpOCVDlg.ExtRegionSet())
		{
			m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20141216 lss
			//m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20141216 lss
			//m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20150109 by
			//m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20150109 by

			if(m_dlgSetup->m_SetUpOCVDlg.ExtOCVTeach())
			{

			}
			else
			{
				tot_rslt = FALSE;
			}
		}
		else
		{
			tot_rslt = FALSE;
		}

		/*
		//OCV teach region ���� 
		dx = m_dlgSetup->m_InsPara.m_dCharCenterX[m_dlgSetup->m_InsPara.m_iParaOCV[2]] - cx;
		dy = m_dlgSetup->m_InsPara.m_dCharCenterY[m_dlgSetup->m_InsPara.m_iParaOCV[2]] - cy;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + dx;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + dy;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + dx;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + dy;
  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + offset_x, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + offset_x); 
		*/	  

  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + offset_x, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + offset_x); 		
		set_color(m_dlgImgRect[0]->GetView(), "yellow");
		disp_obj(Region, m_dlgImgRect[0]->GetView());

		//OCV teach region
  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].top, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].left, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].bottom, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].right); 
		set_color(m_dlgImgRect[0]->GetView(), "green");
		disp_obj(Region, m_dlgImgRect[0]->GetView());

		//Align model
		set_color(m_dlgImgRect[0]->GetView(), "blue");
		disp_obj(Shape, m_dlgImgRect[0]->GetView());
		set_color(m_dlgImgRect[0]->GetView(), "green");
		disp_obj(ModImage, m_dlgImgRect[0]->GetView());

		//Teach������ �������� ���� ���´�.
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1] = rect;

		//Set data
		m_VisMod->SetInspectPara(0, m_dlgSetup->m_InsPara);
		m_VisMod->SetSystemPara(0, m_dlgSetup->m_SysPara);

		//Save
		//ModelSave();//20150717 by 
	}
	else if(2 == rslt)
	{
		//Punching
		//Retry after one step moving
		set_color(m_dlgImgRect[0]->GetView(), "red");
		set_tposition(m_dlgImgRect[0]->GetView(), 10, 10);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : OCV teaching fail-punching chip!")));

		tot_rslt = FALSE;
	}
	else
	{
		//Error
		//Retry after one step moving
		set_color(m_dlgImgRect[0]->GetView(), "red");
		set_tposition(m_dlgImgRect[0]->GetView(), 10, 10);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : OCV teaching fail-can't find model/no pattern!")));

		tot_rslt = FALSE;
	}

	//Align search
  	gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].top, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].left, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].bottom, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].right); 
	set_color(m_dlgImgRect[0]->GetView(), "yellow");
	disp_obj(Region, m_dlgImgRect[0]->GetView());

	// 20110530 Dongwhe* BEGIN 2nd Align Search Region Display
	gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].top, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].left, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].bottom, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].right); 
	set_color(m_dlgImgRect[0]->GetView(), "yellow");
	disp_obj(Region, m_dlgImgRect[0]->GetView());
	// 20110530 Dongwhe* END

	return tot_rslt;
}

void CFilmInsDlg::NGViewDisp()
{
	HTuple StartSecond, EndSecond;
	char temp[1000];
	char path[MAX_PATH];
	char full_path[MAX_PATH];
	int i;
	int para0 = 0;	
	int devIdx = 0;
	int ngidx = 0;

	CString str, str0, str1;
	CRect sys_rect;
	CString Dir;
	Herror Herr;
	CString status;
	
	typeInspectRslt ins_rslt;
//	typeShiftRslt shift_rslt;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0); 

	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	memset(temp, NULL, 1000);
	sprintf(temp, "%d", m_iNGDeviceIdx[0] + 1);
	GetDlgItem(IDC_STATIC_NG_DEVICENO)->SetWindowText(temp);

	GetDlgItem(IDC_STATIC_NG_DATE)->SetWindowText(m_sNGDate[0]);
	GetDlgItem(IDC_STATIC_NG_LOTNO)->SetWindowText(m_sNGLotID[0]);
	
	//NG index
 	m_iNGViewIdx[0]++;

	if(NG_VIEW_MAX_NUM <= m_iNGViewIdx[0]) 
	{
		m_iNGViewIdx[0] = 0;
	} 

	m_VisMod->SetNGViewMode(TRUE, m_iNGViewIdx[0]);

	//Read NG View image
	memset(path, NULL, sizeof(path));
	sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_sNGDate[0], m_sNGLotID[0]);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	for(i = 0; i < ins_para.m_iParaBase[1]; i++)
	{
		//20190909 by if�� �߰�
		switch(m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_BOTTOM:
				sprintf(full_path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, m_sNGLotID[0], m_VisMod->GetVisIndex(), m_iNGDeviceIdx[0] + 1, i);
				break;
			case VISION_CAM_TOP2:
				devIdx = m_iNGDeviceIdx[0] / m_dlgSetup->m_InsPara.m_iParaBase[0];
				sprintf(full_path, "%s\\%s_VIS%d[%04d]_%d.jpg", path, m_sNGLotID[0], m_VisMod->GetVisIndex(), (devIdx * m_dlgSetup->m_InsPara.m_iParaBase[0]) + 1, i);
				break;
		}
		
		set_check("~give_error");

		Herr = read_image(m_VisMod->GetHalImage(0, m_iNGViewIdx[0], i), HTuple((LPCTSTR)full_path));
		
		set_check("give_error");
		if(H_MSG_TRUE == Herr)
		{
			//AfxMessageBox("load ok"); 
		}
		else
		{
			gen_image_const(m_VisMod->GetHalNGImage(0, m_iNGViewIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
			//AfxMessageBox("load fail"); 
			return;
		}

		switch(m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				break;
			case VISION_CAM_TOP2:
			case VISION_CAM_BOTTOM:
				if(m_bIsColor)
					decompose3(*m_VisMod->GetHalImage(0, m_iNGViewIdx[0], i), 
													m_VisMod->GetHalRedImage(0, m_iNGViewIdx[0], i),
													m_VisMod->GetHalGreenImage(0, m_iNGViewIdx[0], i), 
													m_VisMod->GetHalBlueImage(0, m_iNGViewIdx[0], i));
				break;
			default:
				AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
				break;
		}
	}

	//::PostMessage(m_hWnd, USER_MSG_IMG_RECT, 0, 7);
	//Inspection/////////////////////////////////
	m_bLiveOverlayDisp = TRUE;

	count_seconds(&StartSecond);

	//�˻� ������ ���� 
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		m_iRslt[0] = m_VisMod->Inspection(para0, m_iNGViewIdx[0]);
		break;
	case VISION_CAM_TOP2:
		//m_iRslt[0] = m_VisMod->Inspection_Linescan(para0, m_iNGViewIdx[0]);	//20191101 by IMSI
		break; 
	case VISION_CAM_BOTTOM:
		m_iRslt[0] = m_VisMod->Inspection_Bottom(para0, m_iNGViewIdx[0]);
		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
		break;
	}

//	shift_rslt = m_VisMod->GetShiftRslt(para0, m_iNGViewIdx[0]);

	count_seconds(&EndSecond);
	EndSecond = EndSecond - StartSecond;
	sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);



	//�ҷ� ���� Display
	disp_obj(*m_VisMod->GetHalImage(para0, m_iNGViewIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());


	//20191101 by		
	if(m_VisMod->GetVisIndex() == VISION_CAM_TOP2)
	{
		//���� Ĩ��Ʈ�϶� pop_front �� �Ѵ�.
		ngidx = m_iNGDeviceIdx[0] / m_dlgSetup->m_InsPara.m_iParaBase[0];

		if(m_bNGBegin)
		{
			m_bNGBegin = FALSE;
			m_nNGIndex = ngidx;
		}

		if(ngidx > m_nNGIndex)
		{
			m_nNGIndex = ngidx;

			//if(0 != m_iNGDeviceIdx[0])
			m_VisMod->NGListPopFront();
		}

		m_VisMod->SetInspectNGRsltRgn2(0, m_iNGViewIdx[0]);

		ins_rslt = m_VisMod->GetInspRslt();
	}
	else
	{
		m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);

		//Set result data
		ins_rslt = m_VisMod->GetInspectRslt(para0);
	}

	m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], /*shift_rslt,*/ ins_rslt);

	//�ι�° ���ڴ� ������ �ʴ´�. ���� NULL�� �־���.
	ResultCheck_Display(para0, NULL, m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE, FALSE);//main view
	ResultCheck_Display(para0, NULL, m_iNGViewIdx[0], FALSE, FALSE, FALSE, TRUE, FALSE);//ng view	

	m_VisMod->SetNGViewMode(TRUE,-1);
}

BOOL CFilmInsDlg::Test()
{
	int rslt = 0;

	HTuple gray;
	HTuple wd, ht;

	//Halcon image initialize
	HTuple StartSecond, EndSecond;

	typeInspectRslt ins_rslt;
//	typeShiftRslt shift_rslt;
	
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	count_seconds(&StartSecond);

	//20190916 by ���ν�ĵ �˻� �б� �߰�
	//rslt = m_VisMod->Inspection(0, m_iBufIdx[0]);
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		rslt = m_VisMod->Inspection(0, m_iBufIdx[0]);
		break;
	case VISION_CAM_TOP2:
		rslt = m_VisMod->Inspection_Linescan(0, m_iBufIdx[0]);
		break; 
	case VISION_CAM_BOTTOM:
		rslt = m_VisMod->Inspection_Bottom(0, m_iBufIdx[0]);
		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
		break;
	}
					
	
	ins_rslt = m_VisMod->GetInspectRslt(0);
//	shift_rslt = m_VisMod->GetShiftRslt(0, m_iBufIdx[0]);
	
	count_seconds(&EndSecond);
	EndSecond = EndSecond - StartSecond;
	sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	
	//Set result/////////////////////////////
	//Set image
	m_VisMod->SetHalNGImage(0, m_iBufIdx[0], m_iNGViewIdx[0]);
	
	//Set result region
	m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
	
	//Set result data
	m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], /*shift_rslt,*/ ins_rslt);
	
	ResultCheck_Display(0, m_iBufIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//main view //20110509 lee
	ResultCheck_Display(0, m_iBufIdx[0], m_iNGViewIdx[0], FALSE, FALSE, FALSE, TRUE ,FALSE);//ng vie  //20110509 lee

	return rslt;
}


void CFilmInsDlg::WriteTCPList(LPCTSTR lpszItem, BOOL IsRCVorSND)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	//sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);
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

BOOL CFilmInsDlg::ScrollCheck(int cur_scroll_idx)
{
	BOOL rslt = FALSE;
	
	if( (cur_scroll_idx >= m_iNGScrollIdx[0]) && ( (0 <= m_iNGSubViewIdx[0]) && (IMAGE_NG_NUM > m_iNGSubViewIdx[0]) ) )
	{
		rslt = TRUE;
	}
	
	return rslt;
}

void CFilmInsDlg::ScrollCheck()
{
	m_iNGSubViewIdx[0]++;
}

void CFilmInsDlg::ResultCropImg(Hobject *Img, int cof_idx, int nIdx, int ngtype)
{
	Herror herr = H_MSG_TRUE;
	char path[MAX_PATH];
	char item[10];
	CString strTemp, status, Dir;

	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	Hobject obj_r, obj_g, obj_b, obj_img;

	decompose3(*Img, &obj_r, &obj_g, &obj_b);	//20191204 by test

	switch(ngtype)
	{
	case OSCD:
		obj_img = obj_g;
		sprintf(item, "OSCD");
		break;
	case OSCO:
		sprintf(item, "OSCO");
		obj_img = obj_b;
		break;
	case BRKN:
		sprintf(item, "BRKN");
		obj_img = obj_g;
		break;
	case NOCP:
		sprintf(item, "NOCP");
		obj_img = obj_b;
		break;
	case AMSC:
		//sprintf(item, "AMSC");
	case AMDI:
		sprintf(item, "AMDS_AMSC");
		obj_img = obj_r;
		break;
	}

	if(sys_para.m_bPara[31])
	{
		memset(path, NULL, sizeof(path));
		sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, m_strLotNo);

		Dir.Format("%s", path);
		MakeDirectories(Dir);
		
		set_check("~give_error");
		
		sprintf(path, "%s\\%s_VIS%d[%04d]_%d[%s].bmp", path, m_strLotNo, m_VisMod->GetVisIndex(), (m_iPrcDevIdx[0] + cof_idx) + 1, nIdx, item);

		//20200802 by �κ� NG �̹��� ���� ǰ�� ����
		//herr =write_image(obj_img, "jpeg 50", 0, path);
		//herr =write_image(obj_img, "jpeg", 0, path);
		herr =write_image(obj_img, "bmp", 0, path);

		if(H_MSG_TRUE == herr)
		{
		
		}
		else
		{
			strTemp.Format(_T("�̹��� ���忡 �����Ͽ����ϴ�. �ϵ��ũ �뷮 �Ǵ� ���� ��θ� Ȯ���� �ּ���!!"));
			AfxMessageBox(strTemp);
			status.Format("Halcon Error Occured at write_image_01!(ResultCropImg)");
			m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}
	}
}

void CFilmInsDlg::ResultLog(int cam_idx, int buf_idx, int ng_view_idx, int cof_idx, int defect_idx, int defect_type,
							long area, double cx, double cy, double sizex, double sizey, BOOL save)
{
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	//char path_2[MAX_PATH];

	HTuple Num;
	int i;
	Herror herr = H_MSG_TRUE;

	CString status, strTemp, Dir;
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	CFileStatus fs;
	int devIdx = 0;

	TypeImageSavePara save_para;

	if(defect_type)
	{
		//m_strLogPath
		memset(path, NULL, sizeof(path));
		sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
		Dir.Format("%s", path);
		MakeDirectories(Dir);

		//Log//PC,LOTID,DEVICE_IDX,DEFECT_IDX,POS_X,POS_Y,
		sprintf(temp, "%d,0,%s,%04d-%02d-%02d,%02d:%02d:%02d,%d,%d,%d,%f,%f,%f,%f,%f", m_VisMod->GetVisIndex(), m_strLotNo, 
											lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
											lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
											(m_iPrcDevIdx[0] + cof_idx) + 1, defect_idx, defect_type,
											(float)area, cx, cy, sizex, sizey);

		WriteLogAVI(path, "Defect_File.csv", "Defect_File.bak", temp, 0);

		if(save)
		{
			//NG image
			if(sys_para.m_bPara[31])
			{
				for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
				{
				
					memset(path, NULL, sizeof(path));

					sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, m_strLotNo);

					Dir.Format("%s", path);
					MakeDirectories(Dir);

				
					set_check("~give_error");
					
					//20190909 by if�� �߰�
					switch(m_VisMod->GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							sprintf(path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, m_strLotNo, m_VisMod->GetVisIndex(), (m_iPrcDevIdx[0] + cof_idx) + 1, i);
							herr = write_image(*m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i), "bmp", 0, path);
							break;
						case VISION_CAM_TOP2:
#ifdef _CAM
							devIdx = (m_iPrcDevIdx[0] + cof_idx) / m_dlgSetup->m_InsPara.m_iParaBase[0];

							sprintf(path, "%s\\%s_VIS%d[%04d]_%d.jpg", path, m_strLotNo, m_VisMod->GetVisIndex(), (devIdx * m_dlgSetup->m_InsPara.m_iParaBase[0]) + 1/*m_iPrcDevIdx[0] + cof_idx*/, i);

							if(!CFile::GetStatus(path, fs)) {
								//herr = write_image(*m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i), "jpeg 70", 0, path);

								save_para.nImageType = 1;
								sprintf(save_para.strPath, "%s", path);
								//copy_image(*m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i), &save_para.hSaveImage);
								save_para.hSaveImage = *m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i);
								m_ThreadNGImage->SetSavePara(save_para);
							}
#else
							//sprintf(path_2, "%s\\%s_VIS%d[%04d]_%d.bmp", path, m_strLotNo, m_VisMod->GetVisIndex(), (m_iPrcDevIdx[0] + cof_idx) + 1, i);//

							sprintf(path, "%s\\%s_VIS%d[%04d]_%d.jpg", path, m_strLotNo, m_VisMod->GetVisIndex(), (m_iPrcDevIdx[0] + cof_idx) + 1, i);
							
							herr = write_image(*m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i), "jpeg 80", 0, path);

							//CFile::Rename(path, path_2);//

#endif

							break;
					}
					
					set_check("give_error");
					
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						strTemp.Format(_T("�̹��� ���忡 �����Ͽ����ϴ�. �ϵ��ũ �뷮 �Ǵ� ���� ��θ� Ȯ���� �ּ���!!"));
						AfxMessageBox(strTemp);
						status.Format("Halcon Error Occured at write_image_00!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return;
					}
				}
			}
		}
	}
	else
	{
		
		if(save)
		{
			//Good
			if(sys_para.m_bPara[30])
			{
				//Save good image
				for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
				{
					memset(path, NULL, sizeof(path));
					sprintf(path, "%s\\%s\\%s\\IMAGE\\GOOD", m_strLogPath, m_strStartTime, m_strLotNo);

					Dir.Format("%s", path);
					MakeDirectories(Dir);
					
					//20190909 by if�� �߰�
					set_check("~give_error");
					switch(m_VisMod->GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							sprintf(path, "%s\\%sPC%d[%d]_%d.bmp", path, m_strLotNo, cam_idx, m_iPrcDevIdx[0] + cof_idx, i);
							herr = write_image(*m_VisMod->GetHalImage(0, m_iPrcIdx[0], i), "bmp", 0, path);
							break;
						case VISION_CAM_TOP2:
							sprintf(path, "%s\\%sPC%d[%d]_%d.jpg", path, m_strLotNo, cam_idx, m_iPrcDevIdx[0] + cof_idx, i);
							herr = write_image(*m_VisMod->GetHalImage(0, m_iPrcIdx[0], i), "jpg", 0, path);
							break;
					}
					set_check("give_error");
					
					
					
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						strTemp.Format(_T("�̹��� ���忡 �����Ͽ����ϴ�. �ϵ��ũ �뷮 �Ǵ� ���� ��θ� Ȯ���� �ּ���!!"));
						AfxMessageBox(strTemp);
						status.Format("Halcon Error Occured at write_image_01!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return;
					}
				}
			}
		}
	
	}
}

/*
void CFilmInsDlg::ResultAMLog(int cam_idx, int buf_idx, int ng_view_idx, int device_idx, int ng_idx, int aver)
{
#ifdef _CAM
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	
	Hobject SubRgn, TotRgn;
	HTuple Num;
	
	CString status, strTemp;
	
	CString Dir;

	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(cam_idx);
	
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	
	//m_strLogPath
	memset(path, NULL, sizeof(path));
	sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	CString smodel;
	smodel = m_label_model.GetCaption();
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
	
	//Log
	sprintf(temp, "%04d%02d%02d,%02d:%02d:%02d,%s,%s,%d,%d,%d",  
		lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
		lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
		smodel, m_strLotNo,
		GetProcDeviceIndex(0) + device_idx, ng_idx, aver);

	WriteLogError(path, "AMLog_File.csv", "AMLog_File.bak", temp, 0, AMSC);
#endif
}
*/

void CFilmInsDlg::ResultPotLog(int cam_idx, int buf_idx, int ng_view_idx, int device_idx, int *num1, int *num2)
{
#ifdef _CAM
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	
	Hobject SubRgn, TotRgn;
	HTuple Num;
	
	CString status, strTemp;
	
	CString Dir;

	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(cam_idx);
	
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	
	//m_strLogPath
	memset(path, NULL, sizeof(path));
	sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	CString smodel;
	smodel = m_label_model.GetCaption();
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
	
	//Log
	sprintf(temp, "%04d%02d%02d,%02d:%02d:%02d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",  
		lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
		lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
		smodel, m_strLotNo,
		m_iPrcDevIdx[0] + device_idx, num1[0], num1[1], num1[2], num1[3], num1[4], num1[5],   num2[0], num2[1], num2[2], num2[3], num2[4], num2[5]);

	WriteLogError(path, "PotLog_File.csv", "PotLog_File.bak", temp, 0, PCOD);
#endif
}

BOOL CFilmInsDlg::WriteLogError(CString path, CString file, CString bak_file, CString log, long limit_line, int type)
{
#ifdef _CAM
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

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
			AfxMessageBox("Error : log file open fail!(WriteLogError)" );
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
	if(0 == line)
	{
	    result = fseek(stream, 0L, SEEK_END);
		if(result)
		{
			fclose(stream);
			return FALSE;
		}
		memset(szTime, NULL, sizeof(szTime));
		switch(type)
		{
		case AMSC:
			sprintf(szTime, "DATE,TIME,DEVICEID,LOTID,DEVICE_INDEX,NG_INDEX,AVERAGE_VALUE");
			break;

		case PCOD:
			sprintf(szTime, "DATE,TIME,DEVICEID,LOTID,DEVICE_INDEX, Y_TOP, Y_BOTTOM, X_LEFT_TOP, X_LEFT_BOT, X_RIGHT_TOP, X_RIGHT_BOT, CY_TOP, CY_BOTTOM, CX_LEFT_TOP, CX_LEFT_BOT, CX_RIGHT_TOP, CX_RIGHT_BOT");
			break;

		}
		fputs(szTime, stream);
		fputs("\n", stream);	
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
#endif
	return TRUE;
}

BOOL CFilmInsDlg::RemoteControlModel(short menu_select, CString model_name)
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
		m_dlgProcessing.ShowWindow(SW_SHOW);
		HandleAllMessage();

		str.Empty();
		str = m_label_model.GetCaption();
		if(0 == str.Compare(model_name))
		{
			//�̹� ������
			status.Format("Warnning : this model already loaded!");
			WriteStatusList((LPCTSTR)status);
			m_dlgProcessing.ShowWindow(SW_HIDE);
			return FALSE;
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
			//���� 
			dlg->Open();
			//Make directory
			Dir.Format("%s", dlg->GetPath());
			MakeDirectories(Dir);
			//Load setup INI file
			if( m_VisMod->LoadInspectPara(dlg->GetPath()))
			{
				m_VisMod->LoadInspectSelectPara(dlg->GetPath());		// 110723 ytlee

				status.Format("Model change OK!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				status.Format("%s", dlg->GetName());
				m_label_model.SetCaption((LPCTSTR)status);

				UpdateLight(FALSE);

				ResetCount();

				rslt = TRUE;
			}
			else
			{
				status.Format("Error : can't load setup parameter file!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				rslt = FALSE;
			}

			m_dlgProcessing.ShowWindow(SW_HIDE);
		}

		delete dlg;

		break;
	default:

		break;
	}

	return rslt;
}

void CFilmInsDlg::SetLightIndex(int camidx, BOOL isLineScan)
{
	/*
		0 : �� ����
		1 : �����Ʈ
		2 : ����
		3 : �����簢
	*/
	switch(camidx)
	{
	case VISION_CAM_TOP1:
		m_iLightChIdx[0] = 0;			//���� ä�� �ε���. ���� �輱�� �߸��Ǿ� �����Ʈ�� �����簢 �ڸ��� �ٲ�.
		m_iLightChIdx[1] = 3;
		m_iLightChIdx[2] = 2;
		m_iLightChIdx[3] = 1;

		m_iLightOnOffIdx[0] = 0;		//���� �¿��� �ε���
		m_iLightOnOffIdx[1] = 27;
		m_iLightOnOffIdx[2] = 18;
		m_iLightOnOffIdx[3] = 9;
		break;
	case VISION_CAM_TOP2:

		m_iLightChIdx[0] = 11;
		m_iLightChIdx[1] = 12;
		m_iLightChIdx[2] = 13;
		m_iLightChIdx[3] = 14;
		
		m_iLightOnOffIdx[0] = 0;
		m_iLightOnOffIdx[1] = 0;
		m_iLightOnOffIdx[2] = 0;
		m_iLightOnOffIdx[3] = 0;
		break;	
	case VISION_CAM_BOTTOM:
		m_iLightChIdx[0] = 6;
		m_iLightChIdx[1] = 4;
		m_iLightChIdx[2] = 5;
		m_iLightChIdx[3] = -1;

		m_iLightOnOffIdx[0] = 54;
		m_iLightOnOffIdx[1] = 36;
		m_iLightOnOffIdx[2] = 45;
		m_iLightOnOffIdx[3] = -1;
		break;

	default:
		m_iLightChIdx[0] = 0;
		m_iLightChIdx[1] = 0;
		m_iLightChIdx[2] = 0;
		m_iLightChIdx[3] = 0;

		m_iLightOnOffIdx[0] = 0;
		m_iLightOnOffIdx[1] = 0;
		m_iLightOnOffIdx[2] = 0;
		m_iLightOnOffIdx[3] = 0;

		break;
	}
}

void CFilmInsDlg::OnSend(CString data)
{
	int rslt = 1;
	int size = 0;
	char temp[PACKET_MAX_NUM] = {0, };
	
	sprintf(temp, "%s", data);
	size = strlen(temp);

	WriteTCPList(temp, FALSE);

	m_Client->Send((LPSTR)temp, size);

}

void CFilmInsDlg::ResultCheck_Display(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, BOOL chk_ng_type, BOOL chk_ng_Disp, BOOL ng_view_data_save) //20220509 lee
{
	char temp[DAT_STR_LENG];
	int i, j;
	int ng_idx = 0;
	long rslt_num = 0;
	long rslt_num2 = 0;
	long rslt_num3 = 0;
	long sx, sy, ex, ey;
	int cur_scroll_idx = 0;
	int nRow = 0, nCol = 0;
	int nStep = 0;
	HTuple Mean;
	double interval;
	CString strCaption;
	
	BOOL save[SUB_RGN_MAX_NUM];

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		save[i] = TRUE;	
	}
	
	Herror hrr;
	Hobject obj_r, obj_g, obj_b;
	Hobject *RsltRegion, SelectRgn, *RsltRegion2, SelectRgn2, *RsltRegion3, SelectRgn3, CropImage;
	HTuple Num, CRow, CCol, Row0, Col0, Row1, Col1, Area;
	HTuple CRow2, CCol2, Row2, Col2, Row3, Col3, Area2;
	HTuple CRow3, CCol3, Row4, Col4, Row5, Col5, Area3;
	CRect rect;
	int pot_average_length, pot_center_length;
	
	typeInspectNGRslt ins_ng_rslt;
	ins_ng_rslt = m_VisMod->GetInspectNGRslt(cam_idx, ng_view_idx);
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);
	
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);
	
	//SYSTEMTIME	lpSystemTime;
	//GetLocalTime(&lpSystemTime);
	
	//set_color(m_dlgImgRect[0]->GetView(), "white");
	//disp_arrow(m_dlgImgRect[0]->GetView(), 100.0, 100.0, 20500.0, 100.0, 100.0);

	
	set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
	
	if(chk_ng_type)
	{
		//No display
		for(i = 0; i < SUB_RGN_MAX_NUM; i++)
		{
			m_bChkNGKind[cam_idx][i] = 0;
		}
	}
	else
	{
		if(1 == m_iRslt[cam_idx])
		{
			if(main_view)
			{
				sprintf(temp, "GOOD");
				m_label_ngview.SetCaption(temp);
				m_label_ngview.SetForeColor(RGB(0, 255, 0));
			}
		}
		else
		{
			if(main_view)
			{
				sprintf(temp, "NG");
				m_label_ngview.SetCaption(temp);
				m_label_ngview.SetForeColor(RGB(255, 0, 0));
			}
			
			for(i = 0; i < IMAGE_NG_NUM; i++)
			{
				clear_window(m_lNGWindowID[i]);
			}
			
		}
		
		m_iNGSubViewIdx[cam_idx] = 0;
		
		if(main_view)
		{
			//NG scroll
			m_iNGScrollIdx[cam_idx] = 0;

			m_iNGScrollLastNum[cam_idx] = 0;//ǥ���� ��ü�ҷ�����
			
			if(auto_mode)
			{
				memset(temp, NULL, sizeof(temp));

				GetDlgItem(IDC_STATIC_NG_DATE)->SetWindowText(m_sNGDate[cam_idx]);
				GetDlgItem(IDC_STATIC_NG_LOTNO)->SetWindowText(m_sNGLotID[cam_idx]);
				sprintf(temp, "%d", m_iNGDeviceIdx[cam_idx]);
				GetDlgItem(IDC_STATIC_NG_DEVICENO)->SetWindowText(temp);
			}
		}
		else
		{
			//NG scroll
			m_scroll_ng.SetScrollRange(0, m_iNGScrollLastNum[cam_idx]);
		}		
	}

	//Display
	for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
	{	
		switch(m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
		case VISION_CAM_BOTTOM:
			nRow = ins_para.m_InsRegionChip[i][0].left;
			nCol = 200;

			nStep = 80;
			break;
		case VISION_CAM_TOP2:
			nRow = ins_para.m_InsRegionFilm[i][0].left;
			nCol = ins_para.m_InsRegionFilm[i][0].top;

			nStep = 350;
			break;
			//nCol = nCol + nStep;  �� �༮�� ���󰡸鼭 �����ϱ�.	20190930 by IMSI
		}


		if(chk_ng_type)
		{
			//No display
			
		}
		else
		{
			if(ins_ng_rslt.m_InsRslt.m_bRegionSkip[i])
			{
				continue;
			}
			
			if(main_view)
			{
				//set_font(m_dlgImgRect[i]->GetView(), "-Bold-20-*-*-*-*-*-1-");
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
				set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);

				write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple(HTuple("CHIP#") + HTuple(i + 1)) );

				nCol = nCol + nStep; 

				//set_font(m_dlgImgRect[i]->GetView(), "-Arial-12-*-*-*-*-*-1-");
			}
			
			if(1 == ins_ng_rslt.m_InsRslt.m_iRslt[i])
			{
				if(main_view)
				{ 
					//Good
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					write_string(m_dlgImgRect[cam_idx]->GetView(), "GOOD");
					
					if(auto_mode)
					{
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, 0, GOOD, 0, 0., 0., 0., 0., TRUE);
					}
				}

				nCol = nCol + nStep; 
			} 
			else
			{
				if(main_view)
				{
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG");
				}

				nCol = nCol + nStep; 
			}
		}
		
		//Align//////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltAlign[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				if(main_view && chk_ng_Disp)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Pattern
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 2);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Rect + cross
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 1);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Score
					for(j = 0; j < PATTERN_INS_NUM; j++)
					{
						if(0 < ins_ng_rslt.m_InsRslt.m_dScoreAlign[i])
						{
							set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
							set_tposition(m_dlgImgRect[cam_idx]->GetView(), ins_ng_rslt.m_InsRslt.m_dCYAlign[i][j] - 150, ins_ng_rslt.m_InsRslt.m_dCXAlign[i][j]);
							write_string(m_dlgImgRect[cam_idx]->GetView(), (Hlong)(ins_ng_rslt.m_InsRslt.m_dScoreAlign[i][j] * 100.));
						}
					}
				}
			}
			break;
		case -10:
			//Align fail
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = AMIF;
				}
			}
			else
			{
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMIF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN �νĽ���");
					
					if(auto_mode)
					{
						//20150811 by
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						area_center(*RsltRegion, &Area, &CRow, &CCol);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);

						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMIF, (Hlong)Area[0], (double)CCol[0], (double)CRow[0], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN �νĽ���");
						strCaption.Format("CHIP #%d NG : ALIGN �νĽ���", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -11:
			//Align fail-no pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN �νĽ���-NO DATA");
					
					if(auto_mode)
					{					
						//20150811 by
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						area_center(*RsltRegion, &Area, &CRow, &CCol);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);

						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, (Hlong)Area[0], (double)CCol[0], (double)CRow[0], rect.Width(), rect.Height(), save[i]);
						//Align fail
						//ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN �νĽ���-NO DATA");
						strCaption.Format("CHIP #%d NG : ALIGN �νĽ���-NO DATA", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
				
			}
			
			break;
		case -12:
			//Align limit error
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = AMPL;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMPL, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN �νĽ���-POS.LMT.");
					
					if(auto_mode)
					{					
						//20150811 by
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						area_center(*RsltRegion, &Area, &CRow, &CCol);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);

						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMPL, (Hlong)Area[0], (double)CCol[0], (double)CRow[0], rect.Width(), rect.Height(), save[i]);
						//Align fail
						//ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMPL, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN �νĽ���-POS.LMT.");
						strCaption.Format("CHIP #%d NG : ALIGN �νĽ���-POS.LMT.", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Chip///////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case 2:
			//Punching chip
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{	
				//20110509 lee
				if(main_view && chk_ng_Disp)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					 
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "SKIP : PUNCH CHIP");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : PUNCH CHIP", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
					}
				}
			}
			break;
		case 3:
			//No chip
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PCND;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//No chip
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PCND, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 19);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : PF����/PUNCH BURR(�����ҷ�)");
					
					if(auto_mode)
					{
						//20150811 by
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 19);
						area_center(*RsltRegion, &Area, &CRow, &CCol);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);

						//No chip
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PCND, (Hlong)Area[0], (double)CCol[0], (double)CRow[0], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 19);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : PF����/PUNCH BURR(�����ҷ�)");
						strCaption.Format("CHIP #%d NG : PF����/PUNCH BURR(�����ҷ�)", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}	
			break;
		case -20:
			//No chip
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = NOCP;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//No chip
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NOCP, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : NO/WRONG CHIP");
					
					if(auto_mode)
					{
						//20150811 by
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						area_center(*RsltRegion, &Area, &CRow, &CCol);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);

						//No chip
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NOCP, (Hlong)Area[0], (double)CCol[0], (double)CRow[0], rect.Width(), rect.Height(), save[i]);
						//ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NOCP, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
/*
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
												
						crop_rectangle1(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[0]), &CropImage, sy, sx, ey, ex);
						ResultCropImg(&CropImage, i, 0, NOCP);
*/
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : NO/WRONG CHIP");
						strCaption.Format("CHIP #%d NG : NO/WRONG CHIP", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
			
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Chipout
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipOut[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
			}
			break;
		case -21:
			//Chipout
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = OSCO;
				}				
			}
			else
			{
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 12);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					//Chip out
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : CHIP OUT");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					//count_obj(*RsltRegion, &Num);
		
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
		
					for(long cho = 0; rslt_num > cho; cho++)
					{
						test2 = abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;

							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
													
							crop_rectangle1(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &CropImage, sy, sx, ey, ex);
							ResultCropImg(&CropImage, i, j, OSCO);
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Chipout
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 12);
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM); j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							hrr = decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &obj_r, &obj_g, &obj_b);	//20191204 by test
							//disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							if(H_MSG_TRUE == hrr)
								disp_obj(obj_b, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							else
								disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);

							//chipout
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : CHIP OUT");
							strCaption.Format("CHIP #%d NG : CHIP OUT", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 300, sx + 10);	//20200802 by sy + 15 -> sy + 300
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
										HTuple( ( (((Hlong)Col1[j]-(Hlong)Col0[j]) + 1) * m_VisMod->m_dPixelSizeData ) + HTuple("um, ") + HTuple("Dy : ") + 
										HTuple( ( ((Hlong)Row1[j]-(Hlong)Row0[j]) + 1) * m_VisMod->m_dPixelSizeData ) + HTuple("um, ") )));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipBroken[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			break;
		case -22:
			//Broken
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = BRKN;
				}						  
			}
			else
			{
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BRKN, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 13);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : CHIP BROKEN");
				
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : Chip �̸����", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BRKN, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;

							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;

							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
													
							crop_rectangle1(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[2]), &CropImage, sy, sx, ey, ex);
							ResultCropImg(&CropImage, i, j, BRKN);
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 13);
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
														
							hrr = decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &obj_r, &obj_g, &obj_b);	//20191204 by test
							//disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[2]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							if(H_MSG_TRUE == hrr)
								disp_obj(obj_g, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							else
								disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[2]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : CHIP BROKEN");
							strCaption.Format("CHIP #%d NG : CHIP BROKEN", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
				
		//Chip �̸� ����(Broken, ��������)
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipMold[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -23:
			//Mold
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = OSCD;
				}						  
			}
			else
			{
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 14);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : Chip �̸��������");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : Chip �̸����", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;

							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;

							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
													
							crop_rectangle1(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &CropImage, sy, sx, ey, ex);
							ResultCropImg(&CropImage, i, j, OSCD);
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 14);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							hrr = decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &obj_r, &obj_g, &obj_b);	//20191204 by test
							//disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[3]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							if(H_MSG_TRUE == hrr)
								disp_obj(obj_b, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							else
								disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[3]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : Chip �̸��������");
							strCaption.Format("CHIP #%d NG : Chip �̸��������", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
				  default:
					  AfxMessageBox("Error : invaild index!");
					  break;
		}
		
		//Pot
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//Pot
				if(main_view)
				{
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}

					//��ǰ

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					Herror herr;		
					CString status;
					
					set_check("~give_error");
					herr = area_center(*RsltRegion, &Area, &CRow, &CCol);
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						status.Format("Halcon Error Occured at tuple_sort_00!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
					}	
					
					//NPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple("POT AREA : ") + HTuple(Area) ) );

					//20150821 by //////////////////////
					HTuple ChipRow2, ChipColumn2, ChipRow3, ChipColumn3;
					int length[LENGTH_POINT_NUM] = {0, };
					int Centerlength[LENGTH_POINT_NUM] = {0, };
					//int pot_average_length, pot_center_length;

					//#1 Y top
					if(0 < ins_para.m_iParaChip[22])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 0);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 0);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 0);

						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);

						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));

						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0], (Hlong)ChipColumn2[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0]-20, (Hlong)ChipColumn2[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#Y Top length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#Y Top leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));

						length[0] = pot_average_length;
						Centerlength[0] = pot_center_length;
					}

					//#2 Y bottom
					if(0 < ins_para.m_iParaChip[23])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 1);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 1);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 1);

						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);

						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow3[0], (Hlong)ChipColumn3[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow3[0]+20, (Hlong)ChipColumn3[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#Y Bottom length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#Y Bottom leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));
						length[1] = pot_average_length;
						Centerlength[1] = pot_center_length;
					}

					//#3 X left top
					if(0 < ins_para.m_iParaChip[24])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 2);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 2);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 2);

						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
						
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0], (Hlong)ChipColumn2[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0]-20, (Hlong)ChipColumn2[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X L Top length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X L Top leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));
						length[2] = pot_average_length;
						Centerlength[2] = pot_center_length;
					}

					//#4 X left bottom
					if(0 < ins_para.m_iParaChip[25])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 3);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 3);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 3);

						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);

						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0], (Hlong)ChipColumn2[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0]-20, (Hlong)ChipColumn2[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X L Bot length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X L Bot leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));
						length[3] = pot_average_length;
						Centerlength[3] = pot_center_length;
					}

					//#5 X right top
					if(0 < ins_para.m_iParaChip[26])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 4);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 4);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 4);

						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);

						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0], (Hlong)ChipColumn3[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0]+20, (Hlong)ChipColumn3[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X R Top length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X R Top leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));
						length[4] = pot_average_length;
						Centerlength[4] = pot_center_length;
					}

					//#6 X right bottom
					if(0 < ins_para.m_iParaChip[27])
					{
						RsltRegion = m_VisMod->GetHalNGPotRsltRgn(cam_idx, ng_view_idx, i, 5);
						pot_average_length = m_VisMod->GetHalNGPotRsltAverageLength(cam_idx, ng_view_idx, i, 5);
						pot_center_length = m_VisMod->GetHalNGPotRsltCenterLength(cam_idx, ng_view_idx, i, 5);
						
						smallest_rectangle1(*RsltRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);

						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
						disp_cross(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0], (Hlong)ChipColumn3[0], 50, 0);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)ChipRow2[0]+20, (Hlong)ChipColumn3[0]);
						//write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X R Bot length : " ) + HTuple( (Hlong)(pot_average_length))  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)(pot_center_length)) + HTuple(")") ));
						write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple( "#X R Bot leng:" ) + HTuple( (Hlong)(pot_average_length))  + HTuple( "um" ) ));
						length[5] = pot_average_length;
						Centerlength[5] = pot_center_length;
					}

					if(auto_mode && ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i] == 1)	
					{
						ResultPotLog(cam_idx, buf_idx, ng_view_idx, i, length, Centerlength);
					}
				}
			}
			break;
		case -24:
			//No pot
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = NPTD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Pot
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					Herror herr;		
					CString status;
					
					set_check("~give_error");
					herr = area_center(*RsltRegion, &Area, &CRow, &CCol);		// 120127 ytlee		wrongchip �˻� ���ܿ��� ����
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						status.Format("Halcon Error Occured at area_center_01!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
					}	

					if(auto_mode && ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i] == 1)	
					{
						//ResultPotLog(cam_idx, buf_idx, ng_view_idx, i, (Hlong)Area[0]);
					}
					
					//NPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple("NG : NO/UNDER POTTING : ") + HTuple(Area) ) );
					
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NO POTTING", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}

					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);

						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						area_center(*RsltRegion, &Area, _, _);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						
						strCaption.Format("CHIP #%d NG : NO/UNDER POTTING :", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(HTuple(strCaption) + HTuple(Area) ));
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("NG : NO/UNDER POTTING : ") + HTuple(Area) ));
						
						ScrollCheck();
					}
				}
			}
			break;

		case -26:	//���� ���� �ҷ� �߰�20150818 by 

			break;

		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -25:
			//Double pot
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = DPTD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, DPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Pot
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					
					//DPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple("NG : DOUBLE POTTING : ")  + HTuple(Area) );
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
				
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, DPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						area_center(*RsltRegion, &Area, _, _);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						strCaption.Format("CHIP #%d NG : DOUBLE POTTING :", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(HTuple(strCaption) + HTuple(Area) ));
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("NG : DOUBLE POTTING : ") + HTuple(Area) ));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -26:	//���� ���� �ҷ� �߰�20150818 by 

			break;

		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Pot particle///////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipPotP[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//�νı� ����
				if(main_view)
				{
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 18);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
				}					
			}
			break;
		case -26:
			//Pot particle
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PPCD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//�νı� ����
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 18);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/

					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 16);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : POT Particle");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 16);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : POT Particle");
							strCaption.Format("CHIP #%d NG : POT Particle", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:																																																																																																																																																																																																																						
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//���������ҷ�///////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipPotR[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
			
			}
			break;
		case -27:
			//���������ҷ�
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = ESAD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, ESAD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 17);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ���������ҷ�");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, ESAD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 17);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[5]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ���������ҷ�");
							strCaption.Format("CHIP #%d NG : ���������ҷ�", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Film///////////////////////////////
		//Mold
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmMold[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//SR out pattern region
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("white"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 29);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -30:
			//No film pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Film data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 24);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM NO DATA");
					
					if(auto_mode)
					{					
						//Film data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 24);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : FILM NO DATA"));
						strCaption.Format("CHIP #%d NG : FILM NO DATA", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -31:
			//���� ����
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = RSTD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, RSTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 23);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM ����/��ũ ����/����");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, RSTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//��������
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 23);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : FILM ����/��ũ ����/����");
							strCaption.Format("CHIP #%d NG : FILM ����/��ũ ����/����", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmPF[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
				}
			}
			break;
		case -32:
			//PF ����
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PFHD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PFHD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						
						char temp[100];
						sprintf(temp, "[NG save:PFHD]\n");
						TRACE(temp);

						save[i] = FALSE;
						
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 25);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM PF ����/����");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
									
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PFHD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//��������
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 25);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : FILM PF ����/����");
							strCaption.Format("CHIP #%d NG : FILM PF ����/����", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Metal particle/////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmMetal[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
				}
			}
			break;
		case -33:
			//Metal particle
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = SRMP;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, SRMP, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : SR�� METAL PART.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : FILM PF ����", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, SRMP, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}

					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
				
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//��������
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[6]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : SR�� METAL PART.");
							strCaption.Format("CHIP #%d NG : SR�� METAL PART.", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}


		//Lead///////////////////////////////
		if(chk_ng_type)
		{
			//No display
				
		}
		else
		{
			//20110509 lee
			if(main_view && chk_ng_Disp)
			{
				//Top search
				/*	20191205 by �˻翵���� �׸��� �ʴ´�
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 30);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				*/
			}
		}
		//Top
		switch(ins_ng_rslt.m_InsRslt.m_iRsltLeadTop[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			
			break;
		case -40:
			//Top lead ng
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = LFPT;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					//Lead ��ΰ���
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : LEAD ����/����(OUT)");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 32);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					//Image2/////////////////////
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 34);
					disp_obj(*RsltRegion2, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();

					//Image3/////////////////////(����)
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 36);
					disp_obj(*RsltRegion3, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();

					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}

					for(long cho=0;rslt_num2>cho;cho++)
					{
						test2=abs((long)((Hlong)Row3[cho]-(Hlong)Row2[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col3[cho]-(Hlong)Col2[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow2[cho],(Hlong)CCol2[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow2[cho], (Hlong)CCol2[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}


					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : LEAD ����/����(���)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}

					if(ng_view_data_save)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							save[i] = FALSE;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
					m_iNGScrollLastNum[cam_idx]+=rslt_num2;
					m_iNGScrollLastNum[cam_idx]+=rslt_num3;
				}
				else
				{
					//Image
					//��������
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 32);
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							strCaption.Format("CHIP #%d NG : LEAD ����/����(OUT)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}

					//Image2
					//��������
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 34);
					
					//������
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col2[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row2[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col3[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row3[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion2, &SelectRgn2, j + 1);
							disp_obj(SelectRgn2, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							strCaption.Format("CHIP #%d NG : LEAD ����/����(OUT)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col3[j]-(Hlong)Col2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row3[j]-(Hlong)Row2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							
							
							ScrollCheck();
						}
					}

					//Image3
					//��������
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 36);
					
					//������
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col4[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row4[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col5[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row5[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion3, &SelectRgn3, j + 1);
							disp_obj(SelectRgn3, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD ����/����(OUT)");
							strCaption.Format("CHIP #%d NG : LEAD ����/����(OUT)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col5[j]-(Hlong)Col4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row5[j]-(Hlong)Row4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Bottom
		if(chk_ng_type)
		{
			//No display
			
		}
		else
		{
			//20110509 lee
			if(main_view && chk_ng_Disp)

			{
				//Bottom search
				/*	20191205 by �˻翵���� �׸��� �ʴ´�
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 31);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				*/
			}
		}

		switch(ins_ng_rslt.m_InsRslt.m_iRsltLeadBot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -41:
			//Bottom lead ng
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = LFPB;
				}					
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					//Image2/////////////////
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					//Image3/////////////////
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}	
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					//Lead �Ϻΰ���
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : LEAD ����/����(IN)");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 33);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					//Image2/////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 35);
					disp_obj(*RsltRegion2, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();

					//Image3/////////////////////(����)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 37);
					disp_obj(*RsltRegion3, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();

					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NG : LEAD ����/����(�Ϻ�)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Image//////////////////
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						//Image2/////////////////
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						//Image3/////////////////
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							save[i] = FALSE;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
					m_iNGScrollLastNum[cam_idx]+=rslt_num2;
					m_iNGScrollLastNum[cam_idx]+=rslt_num3;
				}
				else
				{
					//Image//////////////////////
					//��������
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 33);
										
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[2]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD ����/����(IN)");
							strCaption.Format("CHIP #%d NG : LEAD ����/����(IN)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));	
							
							ScrollCheck();
						}
					}

					//Image2/////////////////////
					//��������
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 35);
										
					//������
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col2[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row2[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col3[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row3[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[3]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion2, &SelectRgn2, j + 1);
							disp_obj(SelectRgn2, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							strCaption.Format("CHIP #%d NG : LEAD ����/����(IN)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col3[j]-(Hlong)Col2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row3[j]-(Hlong)Row2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));					
							
							ScrollCheck();
						}
					}

					//Image3/////////////////////
					//��������
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 37);
										
					//������
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col4[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row4[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col5[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row5[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion3, &SelectRgn3, j + 1);
							disp_obj(SelectRgn3, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							strCaption.Format("CHIP #%d NG : LEAD ����/����(IN)", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col5[j]-(Hlong)Col4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row5[j]-(Hlong)Row4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}

		//AM scratch//////////////////////////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltAM[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//�˻翵���� NG view �� ǥ���Ѵ�. 20150807 by

				if(0 == i)
				{
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));

					if(sys_para.m_bPara[15])
					{
						cur_scroll_idx++;
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 61);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						DisplayDeviceGood(RsltRegion, cur_scroll_idx, ng_view_idx, ins_para.m_iParaMapLead[4]);
						
						cur_scroll_idx++;
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 62);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						DisplayDeviceGood(RsltRegion, cur_scroll_idx, ng_view_idx, ins_para.m_iParaMapLead[4]);
					}

					if(sys_para.m_bPara[16])
					{

						cur_scroll_idx++;
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 63);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						DisplayDeviceGood(RsltRegion, cur_scroll_idx, ng_view_idx, ins_para.m_iParaMapLead[4]);

						cur_scroll_idx++;
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 64);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						DisplayDeviceGood(RsltRegion, cur_scroll_idx, ng_view_idx, ins_para.m_iParaMapLead[4]);
					}
				}

		

				//��ǰ �α�
				//ResultAMLog(cam_idx, buf_idx, ng_view_idx, i, j, (Hlong)Mean[0]);// 20160303 by ����
							/*
							//ƼĪ ��
							ins_para.m_iParaLead[30]	//AM#1 TOP :  Area
							ins_para.m_iParaLead[31]	//AM#1 TOP : Mean
							ins_para.m_iParaLead[32]	//AM#1 BOTTOM :  Area
							ins_para.m_iParaLead[33]	//AM#1 BOTTOM : Mean

							ins_para.m_iParaLead[34]	//AM#2 TOP :  Area
							ins_para.m_iParaLead[35]	//AM#2 TOP : Mean
							ins_para.m_iParaLead[36]	//AM#2 BOTTOM :  Area
							ins_para.m_iParaLead[37]	//AM#2 BOTTOM : Mean

							*/
				
			}
			break;
		case -42:
		case -43:
			//AM SCRATCH/DISCOLOR
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = ( ins_ng_rslt.m_InsRslt.m_iRsltAM[i] == -42 ) ? AMSC : AMDI;//AMSC;
				}					
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, ( ins_ng_rslt.m_InsRslt.m_iRsltAM[i] == -42 ) ? AMSC : AMDI, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					//AM��ũ��ġ �ҷ�
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : A/M SCRATCH,FADING");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					if(sys_para.m_bPara[15])
					{
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 61);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 62);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					}
					
					if(sys_para.m_bPara[16])
					{
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 63);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 64);
						disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					}

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 60);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);

					if(auto_mode)
					{						
						//Image//////////////////
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMSC, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);						
							save[i] = FALSE;

							ng_idx++;
						}

					}

					if(ng_view_data_save)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;

							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
													
							crop_rectangle1(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[0]), &CropImage, sy, sx, ey, ex);
							ResultCropImg(&CropImage, i, ng_idx, AMSC);
						}

					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else	//NG View
				{
					//Image//////////////////////
					//AM
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 60);

					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER_CHIP;//SUB_NG_VIEW_INNER;

							interval = (double)(ey - sy) / (double)NG_VIEW_TEXT_GAP;

							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							hrr = decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), &obj_r, &obj_g, &obj_b);	//20191204 by test
							//disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							if(H_MSG_TRUE == hrr)
								disp_obj(obj_r, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							else
								disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
														
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : A/M SCRATCH,FADING");
							strCaption.Format("CHIP #%d NG : A/M SCRATCH,FADING", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], (Hlong)(sy + (10*interval)), sx);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));	

							///AM �ҷ� �߻��� Area ������ ���
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], (Hlong)(sy + (20*interval)), sx);
							area_center(SelectRgn, &Area, _, _);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("����ũ�� : ") + HTuple((Hlong)Area[0])) );

							//AM �ҷ� �߻��� ��� ������
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], (Hlong)(sy + (30*interval)), sx);
							intensity(SelectRgn, *m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), &Mean, _);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("��չ�� : ") + HTuple((Hlong)Mean[0])) );


							ScrollCheck();
						}
					}

				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//OCV////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltOCV[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			if(chk_ng_type)
			{
				//No display

			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//Good
				if(main_view)
				{
					//OCV total search(pattern)
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					//OCV total modimage(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 41);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Character search(pattern modimage)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 42);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Character pattern(pattern shape)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 43);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -50:
			//�μ�ǰ��-CAN'T FIND PATTERN-��ü pattern	
			if(chk_ng_type)
			{ 
				//TRACE("Marking NOT FIND Error Log Save\n");

				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = MKND;
				}					
			}
			else
			{			
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //20101007 lee
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						//TRACE("Marking Error Log Save\n");
						save[i] = FALSE;
						ng_idx++;
					}

					break;
				}
				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-NO CHARS.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : �μ�ǰ��-NO CHARS.", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//������
//						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVPatNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //20101007 lee
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							//TRACE("Marking Error Log Save\n");
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++)//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : �μ�ǰ��-NO CHARS."));
						strCaption.Format("CHIP #%d NG : �μ�ǰ��-NO CHARS.", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -51:
			//�μ�ǰ��-NO PATTERN-���� pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//OCV pattern data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-NO DATA");
					
					if(auto_mode)
					{
						//OCV pattern data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : �μ�ǰ��-NO DATA"));
						strCaption.Format("CHIP #%d NG : �μ�ǰ��-NO DATA", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -52:
			//�μ�ǰ��-CAN'T FIND PATTERN-���� character
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = MKND;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //201007 lee
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Sub search region(if can't find, use)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 44);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-NO CHAR.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					if(auto_mode)
					{	
						//Log
						//������
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //201007 lee
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					if(ng_view_data_save)
					{
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++)//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 44);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVPatNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : �μ�ǰ��-NO CHAR.");
							strCaption.Format("CHIP #%d NG : �μ�ǰ��-NO CHAR.", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		case -53:
			//�μ�ǰ��-NO PATTERN-���� character
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//OCV pattern data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-NO DATA");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : �μ�ǰ��-NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//OCV pattern data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : �μ�ǰ��-NO DATA"));
						strCaption.Format("CHIP #%d NG : �μ�ǰ��-NO DATA", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -54:
		/*
		//Character quality
		
		  if(main_view)
		  {
		  //Char defect NG
		  set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
		  RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 45);
		  disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
		  
			line = line + step;
			set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			set_tposition(m_dlgImgRect[cam_idx]->GetView(), line, 1600);
			
			  write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-CHARS. DEFECT");
			  
				if(auto_mode)
				{
				sprintf(temp, "CAM #%d[%02d:%02d:%02d] : �μ�ǰ��-CHARS. DEFECT", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
				WriteStatusList(temp);
				
				  //Log
				  //������
				  area_center(*RsltRegion, &Area, &CRow, &CCol);
				  smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
				  rslt_num = (Hlong)Row0.Num();
				  for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVVarNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
				  {
				  rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
				  //cx, cy, sizex, sizey
				  ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height());
				  ng_idx++;
				  }
				  }
				  }
			*/
			break;
		case -55:
			//No character quality data
			/*
			if(main_view)
			{
			//OCV total search(pattern)
			set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
			disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
			
			  line = line + step;
			  set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			  set_tposition(m_dlgImgRect[cam_idx]->GetView(), line, 1600);
			  
				write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : �μ�ǰ��-NO DATA");
				
				  if(auto_mode)
				  {
				  sprintf(temp, "CAM #%d[%02d:%02d:%02d] : �μ�ǰ��-NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
				  WriteStatusList(temp);
				  
					//OCV variation data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0.);
					ng_idx++;
					}
					}
			*/
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//SR/////////////////////////////////////
		//SR bubble
		switch(ins_ng_rslt.m_InsRslt.m_iRsltSRBub[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
				}
			}
			break;
		case -60:
			//No SR pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Film data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : SR NO DATA");
					
					if(auto_mode)
					{					
						//Film data fail
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : SR NO DATA"));
						strCaption.Format("CHIP #%d NG : SR NO DATA", i + 1);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -61:
			//����
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = BUBD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iSRBubNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BUBD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 51);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : PI ����/��ũĨ");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					long test1, test2; 
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),3);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iSRBubNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BUBD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iSRBubNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//����
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 51);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iSRBubNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							//20150715 by
							//��ũĨ �˻�� Green  �̹����� �ϱ� ������ Green ���� ���
							//��ũĨ ���� ������ ǥ��
							if(VISION_CAM_BOTTOM == m_VisMod->GetVisIndex() && m_bIsColor)
							{
								Hobject red, green, blue;
								hrr = decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), &red, &green, &blue);

								disp_obj(green, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							}
							else
								disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : PI ����/��ũĨ");
							strCaption.Format("CHIP #%d NG : PI ����/��ũĨ", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
										HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
										HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));	
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		case -71:
			//����
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = FPCD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//������
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, FPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					/*	20191205 by �˻翵���� �׸��� �ʴ´�
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 52);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					*/
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 53);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : PI ����/������");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
	
					long test1, test2;
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),3);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					
					if(auto_mode)
					{
						//Log
						//������
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, FPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width(), rect.Height(), save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
						}
					}
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//����
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 53);
					
					
					//������
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							//write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : PI ����/������");
							strCaption.Format("CHIP #%d NG : PI ����/������", i + 1);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple(strCaption));

							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		
		
	}
}




//
//USER FUNCTION END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Message Map Fucntions Begin
//
void CFilmInsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	typeInspectPara ins_para;
	typeSystemPara sys_para;
	int i;
	double dist, sy, pos;
	CString strFrameRate;
	Hobject RegionRect;
	long StartTime, Endtime;

	switch(nIDEvent)
	{
	case 0:

#ifdef _CAM	
		StartTime = GetTickCount();

		if(m_bLineCamOn)
		{
			//m_cLineCam.Grab(0, m_iBufIdx[0], m_iMapIdx[0], m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_bIsColor, 
			//	m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]));
		}
		else
		{
			if(m_bIsColor)
				m_cCam.Grab(0, m_iBufIdx[0], m_iMapIdx[0], m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_bIsColor, 
					m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]));
			else
					m_cCam.Grab(0, m_iBufIdx[0], m_iMapIdx[0], m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]));
		}

		Endtime = GetTickCount();

		strFrameRate.Format("%3.3f",1000 / (double)(Endtime - StartTime));
		GetDlgItem(IDC_STATIC_FRAME_RATE)->SetWindowText(strFrameRate);
#endif
		if(m_bIsColor)
		{
			switch(m_lColorMode)
			{
			case 1:
				disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 2:
				disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 3:
				disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 0:	
				disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;	
			}
		}
		else
			disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());

		set_color(m_dlgImgRect[0]->GetView(), HTuple("green"));
		disp_line(m_dlgImgRect[0]->GetView(), m_lHeight[0] * 0.5, 0, m_lHeight[0] * 0.5, m_lWidth[0]);
		disp_line(m_dlgImgRect[0]->GetView(), 0, m_lWidth[0] * 0.5, m_lHeight[0], m_lWidth[0] * 0.5);
		//���� ���� ���� Display (48mm����)
		set_color(m_dlgImgRect[0]->GetView(), HTuple("red"));

		//if(m_VisMod->GetPCNum(0) == 0)
		//{
			disp_line(m_dlgImgRect[0]->GetView(), 0, (m_lWidth[0] * 0.5)+(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							m_lHeight[0], (m_lWidth[0] * 0.5)+(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2));
			disp_line(m_dlgImgRect[0]->GetView(), 0, (m_lWidth[0] * 0.5)-(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							m_lHeight[0], (m_lWidth[0] * 0.5)-(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2));
		//}
	

		/////////////////////////////////////////
		//Display film region for start position
		//
		ins_para = m_VisMod->GetInspectPara(0);
		sys_para = m_VisMod->GetSystemPara(0);
	
		double dCofWidth;
		//COF pitch
		dCofWidth = ins_para.m_iParaBase[2] * ins_para.m_dParaBase[1]; //20110513 lee
		//Pixel size
		sys_para.m_dPara[0];

		if(0 < sys_para.m_dPara[0])
		{
			dist = ( (dCofWidth * 1000) / sys_para.m_dPara[0]) * 0.5; //20110513 lee
		}
		else
		{
			dist = 0;
		}

		sy = (m_lHeight[0] * 0.5) - (dist * m_VisMod->GetCOFNum(0));

		if( (0 > sy) || (m_lHeight[0] < sy) )
		{
			sy = 0;
		}

		pos = sy;

		set_color(m_dlgImgRect[0]->GetView(), HTuple("yellow"));
		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			//Up
			pos = pos + (dist * i * 2);
			if( (0 > pos) || (m_lHeight[0] < pos) )
			{
				pos = 0;
			}
			disp_line(m_dlgImgRect[0]->GetView(), pos, 0, pos, m_lWidth[0]);

			//Dn
			pos = pos + (dist * 2);
			if( (0 > pos) || (m_lHeight[0] < pos) )
			{
				pos = m_lHeight[0];
			}
			
			disp_line(m_dlgImgRect[0]->GetView(), pos, 0, pos, m_lWidth[0]);
			//�ٽ� �ʱ� ��ġ�� ���� �Ѵ�.
			pos = sy;

		}

		if(m_lProfileMode == 4)
		{
			CalculateGrayValues();
		}

		if(m_lProfileMode == 5)
		{
			CalculateContrastValues();
		}

		//
		//
		/////////////////////////////////////////

		//Align inspect region
		set_color(m_dlgImgRect[0]->GetView(), HTuple("green"));
		gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
		disp_obj(RegionRect, m_dlgImgRect[0]->GetView()); 
		gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
		disp_obj(RegionRect, m_dlgImgRect[0]->GetView());

		break;
	case 1:	// SOCKET Reconnect try
		{
			int portnum = 0;

			switch(m_VisMod->GetVisIndex())
			{
			case VISION_CAM_TOP1:
				portnum = SOCKET_PORT_NUM1;
				break;
			case VISION_CAM_TOP2:
				portnum = SOCKET_PORT_NUM2;
				break;
			case VISION_CAM_BOTTOM:
				portnum = SOCKET_PORT_NUM3;
				break;
			}

			m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, portnum);

			break;
		}
		break;
	//Monitoring
	case 10:
		break;
	}
	//2910000330 3012302030

	CDialog::OnTimer(nIDEvent);
}

void CFilmInsDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CFilmInsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
		if(!pScrollBar || !pScrollBar->m_hWnd)
        return;

    if(pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_NG)
    {
        UINT nCurPos = pScrollBar->GetScrollPos();
        switch(nSBCode)
        {
        case SB_BOTTOM:         // 7-Scroll to bottom.
            pScrollBar->SetScrollPos(0);
            break;

        case SB_ENDSCROLL:      // 8-End scroll.
            break;

        case SB_LINEDOWN:       // 1-Scroll one line down.
            nCurPos += 1;

			if((int)nCurPos >= m_iNGScrollLastNum[0])
                nCurPos = m_iNGScrollLastNum[0] - 1;
			
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_LINEUP:         // 0-Scroll one line up.
            nCurPos -= 1;
            if(nCurPos < 0)
                nCurPos = 0;
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_PAGEDOWN:       // 3-Scroll one page down.
            nCurPos += 5;
			
            if((int)nCurPos >= m_iNGScrollLastNum[0])
                nCurPos = m_iNGScrollLastNum[0] - 1;
			
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_PAGEUP:         // 2-Scroll one page up.
            nCurPos -= 5;
            if(nCurPos < 0)
                nCurPos = 0;
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_THUMBPOSITION:  // 4-Scroll to the absolute position. The current position is provided in nPos.
            pScrollBar->SetScrollPos(nPos);
            break;

        case SB_THUMBTRACK:     // 5-Drag scroll box to specified position. The current position is provided in nPos.
            pScrollBar->SetScrollPos(nPos);
            break;

        case SB_TOP:            // 6-Scroll to top.
            pScrollBar->SetScrollPos(0);
            break;
        }

		if(0 > nCurPos)
			nCurPos = 0;
		m_iNGScrollIdx[0] = nCurPos;

		//NG view
		ResultCheck_Display(0, 0, m_iNGViewIdx[0], FALSE, FALSE, FALSE ,TRUE ,FALSE); //20110509 lee
    }


	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CFilmInsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CFilmInsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)m_BrushBlack;
	
}

LRESULT CFilmInsDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
//m_Client->GetLastError
#ifdef _CAM
	KillTimer(1);

	m_label_online.SetCaption("ON-LINE");
	m_label_online.SetForeColor(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));

	WriteStatusList("Success : SOCKET connected !");
#endif
	return 0;
}

LRESULT CFilmInsDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
#ifdef _CAM
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

	WriteStatusList("Error : SOCKET disconnected !");
#endif
	return 0;
}


LRESULT CFilmInsDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	char temp[PACKET_MAX_NUM];
	CString rcv_dat, rcv_temp, snd_dat, path;
	CString strCamIdx, strCmd, strBufIdx, strDevIdx, strModel, strLotNo, strLotStarTime, strChipCnt, strTemp, status;
	int nCount = 0;
	typeLotInfo lot_info;
	
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	m_Client->Receive((LPSTR)temp, PACKET_MAX_NUM);
	rcv_temp = temp;

	nCount = count((LPCTSTR)rcv_temp, (LPCTSTR)rcv_temp+rcv_temp.GetLength(), PACKET_CHAR_ETX); // �ΰ��� ��Ŷ�� �پ �� ��� ETX �� �������� �߶� ���� ó���Ѵ�.
	
	for(int i = 0; i < nCount; i++)
	{
		AfxExtractSubString(rcv_dat, rcv_temp, i, PACKET_CHAR_ETX);
		rcv_dat.Delete(0, 1);

		WriteTCPList((LPCTSTR)rcv_dat, TRUE);
		
		AfxExtractSubString(strCmd, rcv_dat, 1, ',');	//Parsing Comman

		if(!strCmd.Compare("0001"))						//Grab Start
		{
			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');

			m_iGrbIdx[0] = atoi((LPCTSTR)strBufIdx);
			m_iGrbDevIdx[0] = atoi((LPCTSTR)strDevIdx);

			m_VisFlagData[0].m_bFlag[7] = TRUE;
		}
		else if(!strCmd.Compare("0003"))				//Inspect Start
		{
			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');
			AfxExtractSubString(strLotNo, rcv_dat, 4, ',');
			AfxExtractSubString(strLotStarTime, rcv_dat, 5, ',');
			AfxExtractSubString(strChipCnt, rcv_dat, 6, ',');
			
			if(!strChipCnt.IsEmpty())
			{
				//Ĩ �� ��ŭ ��Ī ����� ��´�.
				for(int i = 0; i < atoi((LPCTSTR)strChipCnt) && i < COF_MAX_NUM; i++)
				{
					AfxExtractSubString(strTemp, rcv_dat, 7+i, ',');
					ins_para.m_bIsPunchSkip[i] = atoi((LPCTSTR)strTemp);
				}
				
				m_VisMod->SetInspectPara(0, ins_para);
			}

			m_iPrcIdx[0] = atoi((LPCTSTR)strBufIdx);
			m_iBufIdx[0] = m_iPrcIdx[0];
			m_iPrcDevIdx[0] = atoi((LPCTSTR)strDevIdx);
			
			m_strLotNo = strLotNo;
			m_strStartTime = strLotStarTime;

			m_VisFlagData[0].m_bFlag[10] = TRUE;
		}
		else if(!strCmd.Compare("0005"))				//Find Start
		{
			m_VisFlagData[0].m_bFlag[0] = TRUE;
		}
		else if(!strCmd.Compare("0007"))				//Align Start
		{
			m_VisFlagData[0].m_bFlag[4] = TRUE;
		}
		else if(!strCmd.Compare("0009"))				//OCV Teach Start
		{

		}
		else if(!strCmd.Compare("0011"))				//Model Change
		{
			AfxExtractSubString(strModel, rcv_dat, 2, ',');

			if(RemoteControlModel(2, strModel))
			{
				//���� ������Ŷ ����
				snd_dat.Format("%cVIS%d,0012,%s%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), strModel, PACKET_CHAR_ETX);
				DoEvents();
				OnSend(snd_dat);
				DoEvents();
			}
			else
			{
				//���� ������Ŷ ����
				snd_dat.Format("%cVIS%d,0012%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
				DoEvents();
				OnSend(snd_dat);
				DoEvents();

			}
		}
		else if(!strCmd.Compare("0013"))				//Auto Run
		{
			if(!m_btnStartStop.GetValue())
			{
				m_btnStartStop.SetValue(TRUE);
				ClickBtnenhctrlRunstop();
			}

			SendMessage(USER_MSG_SOCKET, 0, 6);
		}
		else if(!strCmd.Compare("0015"))				//Grab&Display
		{
			m_VisFlagData[0].m_bFlag[3] = TRUE;

		}
		else if(!strCmd.Compare("0017"))				//Lot Start
		{
			//20150225 ngh Auto Delete
			m_bDeleteFolderStart = TRUE;

			m_pDeleSearchThread = AfxBeginThread(DeleteSearchThread, this, THREAD_PRIORITY_NORMAL);
			if (m_pDeleSearchThread) m_pDeleSearchThread->m_bAutoDelete = TRUE;

			while(TRUE)
			{
				if(m_bDeleteFolder)
				{
					m_bDeleteFolder = FALSE;
					m_bDeleteFolderStart = FALSE;
					AfxExtractSubString(strLotNo, rcv_dat, 2, ',');
					AfxExtractSubString(strLotStarTime, rcv_dat, 3, ',');

					m_strStartTime = strLotStarTime;
					m_strLotNo = strLotNo;

					m_iGrbIdx[0] = 0;
					m_iBufIdx[0] = 0;
					m_iPrcIdx[0] = 0;

					m_iRslt[0] = 1;

					//NG view
					m_iNGViewIdx[0] = 0;
					m_iNGSubViewIdx[0] = 0;

					m_bInspectStartNGView[0] = FALSE;

					m_VisMod->ResetLotInfo();
					lot_info = m_VisMod->GetLotInfo();

					sprintf(lot_info.m_sNo, "%s", strLotNo);
					sprintf(lot_info.m_sStart, "%s", strLotStarTime);

					m_VisMod->SetLotInfo(lot_info);
					LotDisp(lot_info);

					//NG view clear
					for(i = 0; i < IMAGE_NG_NUM; i++)
					{
						clear_window(m_lNGWindowID[i]);
					}
					
					//vision log file clear 20150619 by
					path.Format("%s\\%s\\%s", m_strLogPath, m_strStartTime, m_strLotNo);
							
					DeleteFolder(path);

					/*
					path.Format("%s\\%s\\%s\\%s", m_strLogPath, m_strStartTime, m_strLotNo, "Defect_File.csv");
					DeleteFile(path);

					//20150821 by
					path.Format("%s\\%s\\%s\\%s", m_strLogPath, m_strStartTime, m_strLotNo, "PotLog_File.csv");
					DeleteFile(path);
					*/

					//status.Format("******************************** %s Lot Start!! (%s)", lot_info.m_sNo, lot_info.m_sStart);
					//m_VisMod->WriteErrorList((LPCTSTR)status);
					
					snd_dat.Format("%cVIS%d,0018%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
					OnSend(snd_dat);

					ClickBtnenhctrlLabelNgview();
					break;
				}

				Sleep(1);
			}
				
			if(m_bLineCamOn)
			{
				m_dlgSetup->m_SetUpChipDlg.AxisZMovePos(AXIS_Z, ins_para.m_iParaChip[38]);

				//20191101 by NG ���� ����Ʈ ���� ���� �ʱ�ȭ
				m_nNGIndex = 0;
				m_bNGBegin = TRUE;
				m_VisMod->ClearNGRsltData();
			}
		}
		else if(!strCmd.Compare("0019"))				//Lot End
		{
			AfxExtractSubString(strLotNo, rcv_dat, 2, ',');

			//Lot info.
			lot_info = m_VisMod->GetLotInfo();

			SYSTEMTIME	lpSystemTime;
			GetLocalTime(&lpSystemTime);

			sprintf(lot_info.m_sEnd, "%04d-%02d-%02d,%02d:%02d:%02d",  
					lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
					lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
			
			m_VisMod->SetLotInfo(lot_info);
			LotDisp(lot_info);
			WriteLot(lot_info);

			snd_dat.Format("%cVIS%d,0020%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
			OnSend(snd_dat);

			if(m_VisMod->GetVisIndex() == VISION_CAM_TOP2)
			{
				if(m_bLineCamOn)
				{
					OnOffLight(FALSE, m_bLineCamOn);
				

					//20191230 by NG �̹��� ������ ����

					CString fromPath;
					CString toPath;
					
					fromPath.Format("%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, m_strLotNo);
					toPath.Format("z:\\IMAGE\\NG");	// ==> �¶��κ��������� ��� Ȯ���Ͽ� �Է�

					MakeDirectories(toPath);

					CopyDirectory(fromPath, toPath);
				}
			}
		}
		else if(!strCmd.Compare("0021"))				//NG Display
		{
			ClickBtnenhctrlLabelNgview();

			SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');
			AfxExtractSubString(strLotNo, rcv_dat, 4, ',');
			AfxExtractSubString(strLotStarTime, rcv_dat, 5, ',');
			AfxExtractSubString(strChipCnt, rcv_dat, 6, ',');
			
			if(!strChipCnt.IsEmpty())
			{
				//Ĩ �� ��ŭ ��Ī ����� ��´�.
				for(int i = 0; i < atoi((LPCTSTR)strChipCnt) && i < COF_MAX_NUM; i++)
				{
					AfxExtractSubString(strTemp, rcv_dat, 7+i, ',');
					ins_para.m_bIsPunchSkip[i] = atoi((LPCTSTR)strTemp);
				}
				
				m_VisMod->SetInspectPara(0, ins_para);
			}

			m_strLotNo = strLotNo;
			m_strStartTime = strLotStarTime;

			lot_info = m_VisMod->GetLotInfo();
			sprintf(m_sNGDate[0], "%s", (LPCTSTR)m_strStartTime);
			sprintf(m_sNGLotID[0], "%s", (LPCTSTR)m_strLotNo);

			m_iNGDeviceIdx[0] = atoi((LPCTSTR)strDevIdx);

			m_bInspectStartNGView[0] = TRUE;
		}
		else if(!strCmd.Compare("0023"))		// SetFocus Window
		{
			SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

			//���� ������Ŷ ����
			snd_dat.Format("%cVIS%d,0024%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
			OnSend(snd_dat);
		}
		else if(!strCmd.Compare("0025"))		//Auto Teach OCV
		{
			//OCV�� ���1(���)������ �ϴ� ������ ����.
			ins_para = m_VisMod->GetInspectPara(0);	

			//20150717 by �������� �׷����μ��� �ȿ���..
			//m_cCam.Grab(0, 0, 0, m_VisMod->GetHalImage(0, m_iBufIdx[0], ins_para.m_iParaMapOCV[0]));
			GrabProcess(0, m_iBufIdx[0], ins_para.m_iParaBase[1], m_bLineCamOn);

			if(AutoTeachOCV())
			{
				::SendMessage(m_hWnd, USER_MSG_SOCKET, 1, 4);	// Success
			}
			else
			{
				::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 4);	// Fail
			}
		}
		else if(!strCmd.Compare("0027"))		//Ʈ���� ��ġ �ʱ�ȭ
		{
			m_cTriggerBoard->SetTrigger_Trig_Pos_Reset();

			while(TRUE)
			{
				if(m_cTriggerBoard->m_bTriggerTrigPosReset)
				{
					snd_dat.Format("%cVIS%d,0028%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
					WriteStatusList("Trigger Reset!!");
					OnSend(snd_dat);
					break;
				}

				if(m_cTriggerBoard->m_bTrigger_TimeOut)
				{
					WriteStatusList("Trigger TimeOut!!");
					break;					
				}

				Sleep(1);
			}
		}
		else	//�߸��� ��Ŷ
		{
			
		}
	}

	return 0;
}

void CFilmInsDlg::DisplayView(int cam_idx, int buf_idx, int img_idx)
{
	if(m_bIsColor)
	{
		switch(m_lColorMode)
		{
		case 1:
			disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 2:
			disp_obj(*m_VisMod->GetHalRedImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalRedImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 3:
			disp_obj(*m_VisMod->GetHalGreenImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalGreenImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 0:	
			disp_obj(*m_VisMod->GetHalBlueImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalBlueImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;	
		}
	}
	else
	{
		disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
		disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
	}

}

void CFilmInsDlg::OnRectImgMap0()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 0;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img0_ch0);
		SetLight(m_iLightChIdx[1], m_img0_ch1);
		SetLight(m_iLightChIdx[2], m_img0_ch2);
		SetLight(m_iLightChIdx[3], m_img0_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		//�Լ�����(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, BOOL chk_ng_type, BOOL chk_ng_Disp, BOOL ng_view_data_save)
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 1;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img1_ch0);
		SetLight(m_iLightChIdx[1], m_img1_ch1);
		SetLight(m_iLightChIdx[2], m_img1_ch2);
		SetLight(m_iLightChIdx[3], m_img1_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 2;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img2_ch0);
		SetLight(m_iLightChIdx[1], m_img2_ch1);
		SetLight(m_iLightChIdx[2], m_img2_ch2);
		SetLight(m_iLightChIdx[3], m_img2_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 3;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img3_ch0);
		SetLight(m_iLightChIdx[1], m_img3_ch1);
		SetLight(m_iLightChIdx[2], m_img3_ch2);
		SetLight(m_iLightChIdx[3], m_img3_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 4;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img4_ch0);
		SetLight(m_iLightChIdx[1], m_img4_ch1);
		SetLight(m_iLightChIdx[2], m_img4_ch2);
		SetLight(m_iLightChIdx[3], m_img4_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 5;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img5_ch0);
		SetLight(m_iLightChIdx[1], m_img5_ch1);
		SetLight(m_iLightChIdx[2], m_img5_ch2);
		SetLight(m_iLightChIdx[3], m_img5_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 6;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     

	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img6_ch0);
		SetLight(m_iLightChIdx[1], m_img6_ch1);
		SetLight(m_iLightChIdx[2], m_img6_ch2);
		SetLight(m_iLightChIdx[3], m_img6_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 7;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img7_ch0);
		SetLight(m_iLightChIdx[1], m_img7_ch1);
		SetLight(m_iLightChIdx[2], m_img7_ch2);
		SetLight(m_iLightChIdx[3], m_img7_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap8()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 8;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img8_ch0);
		SetLight(m_iLightChIdx[1], m_img8_ch1);
		SetLight(m_iLightChIdx[2], m_img8_ch2);
		SetLight(m_iLightChIdx[3], m_img8_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap9()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 9;
	
	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img9_ch0);
		SetLight(m_iLightChIdx[1], m_img9_ch1);
		SetLight(m_iLightChIdx[2], m_img9_ch2);
		SetLight(m_iLightChIdx[3], m_img9_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 10;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
#ifdef _CAM
		SetLight(m_iLightChIdx[0], m_img10_ch0);
		SetLight(m_iLightChIdx[1], m_img10_ch1);
		SetLight(m_iLightChIdx[2], m_img10_ch2);
		SetLight(m_iLightChIdx[3], m_img10_ch3);
#endif
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap11()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iMapIdx[0] = 11;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
	UpdateMapButton(m_iMapIdx[0]);     

#ifdef _CAM
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img11_ch0);
		SetLight(m_iLightChIdx[1], m_img11_ch1);
		SetLight(m_iLightChIdx[2], m_img11_ch2);
		SetLight(m_iLightChIdx[3], m_img11_ch3);
	}
#endif

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}

}



BEGIN_EVENTSINK_MAP(CFilmInsDlg, CDialog)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_RUNSTOP, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlRunstop, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_MODEL, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlModel, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_SETUP, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlSetup, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_EXIT, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlExit, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelSelProfilemode, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_SEL_COLORMODE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelSelColormode, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_RESULT4, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelResult4, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_TEST, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapTest, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_SAVE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapSave, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_LOAD, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapLoad, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_LOAD3, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapLoad3, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP0, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap0, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP1, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap1, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP2, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap2, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP3, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap3, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP4, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap4, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP5, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap5, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP6, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap6, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP7, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap7, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP8, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap8, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP9, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap9, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP10, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap10, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP11, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap11, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL2, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrl2, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_NGVIEW, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelNgview, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_SEL_OCVAUTOTEACH, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelSelOcvautoteach, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_CAMERA, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelCamera, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAINVIEW, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMainview, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_ONLINE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelOnline, VTS_NONE)
END_EVENTSINK_MAP()

void CFilmInsDlg::ClickBtnenhctrlRunstop()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//if(m_btnStartStop.GetBCheck())

	if(m_btnStartStop.GetValue())
	{
		for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			//Stop live
			KillTimer(i);

			m_bLiveOverlayDisp = TRUE;
		}

		m_btnStartStop.SetCaption("STOP");

		m_label_mode.SetCaption("RUN MODE");
		m_label_mode.SetForeColor(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));

		m_VisMod->SetNGViewMode(FALSE, -1);

    	StartInspectThread();

		switch(m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP2:
			m_dlgAM_Reference->ShowWindow(SW_SHOW);

			for(int i = 0; i < VIEW_MAX_NUM; i++)
				m_dlgAM_Reference->DisplayReferenceImage(i, m_strFullPath);
			break;
		}

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

		switch(m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP2:
			m_dlgAM_Reference->ShowWindow(SW_HIDE);
			break;
		}

		WriteStatusList(">>>>STOP MODE(Inspection stopped!)");
	}
	
}

void CFilmInsDlg::ClickBtnenhctrlModel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int idx = 0;
 	CRect rect;
 	char temp[MAX_MOD_NAME];
 	CString str0, str1;	
 	CString Dir;
 
 	if(m_btnStartStop.GetValue())
 	{
 		MessageBox("Error : ���� �˻� ���� ���Դϴ�! �˻� ���� �� �� �õ� �ٶ��ϴ�.", NULL, MB_ICONEXCLAMATION);
 		return;
 	}
 
 	//Close setup dialog
 	m_dlgSetup->ShowWindow(SW_HIDE);
 
 	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
 	{
 		//Stop live
 		KillTimer(i);
 	}
 
 	CDialogModelMenu dlg;
 
 	GetDlgItem(IDC_BTNENHCTRL_MODEL)->GetWindowRect(rect);
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
 		//MakeDefault Param
 		m_VisMod->MakeDefaultInspPara();
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
 			m_dlgProcessing.ShowWindow(SW_SHOW);
 			HandleAllMessage();
 
 			//no need
 			UpdateLight(TRUE);

 			m_dlgProcessing.ShowWindow(SW_HIDE);
 		}
 
 
 		if(moddlg->DoModal(idx))
 		{
 			WriteStatusList("MODEL CHANGE-SELECT");
			m_dlgProcessing.ShowWindow(SW_SHOW);
 			HandleAllMessage();
	 
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
 				//�� �ε� �̹Ƿ� ���� ��� �� ���� �Ѵ�.?
 				//clear_all_variation_models(); //20110508 lee
 				clear_all_shape_models(); //20110508 lee
				//m_VisMod->LoadSystemPara(m_strDBPath);
 				//Load setup INI file
 				if( m_VisMod->LoadInspectPara(moddlg->GetPath()) )
 				{
					m_VisMod->LoadInspectSelectPara(moddlg->GetPath());

					ResetCount();

					UpdateLight(FALSE);
 				}
 				else
 				{
 					WriteStatusList("Error : can't load setup parameter file!");
	 
 					//Make directory
 					Dir.Format("%s", moddlg->GetPath());
 					MakeDirectories(moddlg->GetPath());
 				}
 			}

			m_strFullPath = moddlg->GetPath();//20160922 by

			m_dlgProcessing.ShowWindow(SW_HIDE);
 
 		}
		else
		{

		}
 		break;
 	//Convertion
 	case 3:
 		//moddlg->DoModal(idx);
 	
 		break;
 	default:
 		
 		break;
 	}
 
 	delete moddlg;
}

void CFilmInsDlg::ResetCount()
{
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	sys_rslt.m_lCnt[1] = 0;
	sys_rslt.m_lCnt[2] = 0;
	sys_rslt.m_lCnt[3] = 0;

	char temp[10];
	sprintf(temp, "%1d", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%1d ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%1d ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);

	m_VisMod->SetMainSysRslt(sys_rslt);
}

void CFilmInsDlg::ClickBtnenhctrlSetup()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CPasswordDlg dlg;
#ifdef _CAM
	if(dlg.DoModal() == IDOK)
	{
		//�н����� üũ
		typeSystemPara sys_para;
		sys_para = m_VisMod->GetSystemPara(0);

		if(0 == dlg.m_ctrlPassword.Compare(sys_para.m_strPara))
		{
			WriteStatusList("Ok password. Setup started!");
#endif
			//Set inspect data
			m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
			m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(0);

			//20150223 ngh	setup â ���÷��� ���� �ڷ� ���°� ����
			if(m_dlgSetup->IsWindowVisible())
			{
				m_dlgSetup->MoveWindow(&m_dlgSetup->m_FixeRectPos);
				return;
			}

			//Set inspection result
			for(int i = 0; (i < m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 0), &m_dlgSetup->m_HalInsRgn[i][0], 1, -1);//SR region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 1), &m_dlgSetup->m_HalInsRgn[i][1], 1, -1);//PF hall region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 2), &m_dlgSetup->m_HalInsRgn[i][2], 1, -1);//up lead region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 3), &m_dlgSetup->m_HalInsRgn[i][3], 1, -1);//dn lead region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 4), &m_dlgSetup->m_HalInsRgn[i][4], 1, -1);//user area region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 5), &m_dlgSetup->m_HalInsRgn[i][5], 1, -1);//SR out region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 6), &m_dlgSetup->m_HalInsRgn[i][6], 1, -1);//vaccum region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 7), &m_dlgSetup->m_HalInsRgn[i][7], 1, -1);//chip region
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 8), &m_dlgSetup->m_HalInsRgn[i][8], 1, -1);//AM0
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 9), &m_dlgSetup->m_HalInsRgn[i][9], 1, -1);//AM0
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 10), &m_dlgSetup->m_HalInsRgn[i][10], 1, -1);//AM1
				copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 11), &m_dlgSetup->m_HalInsRgn[i][11], 1, -1);//AM1
			}

			m_dlgSetup->ShowWindow(SW_SHOW);
			WriteStatusList("Setup started!");
#ifdef _CAM
		}
		else
		{
			WriteStatusList("Wrong password !");
			AfxMessageBox("��й�ȣ�� Ʋ�Ƚ��ϴ� !");
		}
	}
	else
	{

		return;
	}	
#endif
}

void CFilmInsDlg::ClickBtnenhctrlExit()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_btnStartStop.GetValue())
	{
		AfxMessageBox("Run ��� �Դϴ�. Stop ���� ������ �ּ���!");
		return;
	}

	m_BrushBlack.DeleteObject();

	StopInspectThread();	

	KillTimer(0);
	KillTimer(1);

#ifdef _CAM

	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
	case VISION_CAM_BOTTOM:
		OnOffLight(FALSE, m_bLineCamOn);

		COMI_UnloadDll();

		m_cCam.Close(m_bIsColor);
		break;
	case VISION_CAM_TOP2:
		m_cLineCam.Close_LineScanCam();
		
		if(NULL != m_cTriggerBoard)
		{
			m_cTriggerBoard->CloseTrigger();
			delete m_cTriggerBoard;
			m_cTriggerBoard = NULL;
		}
		
		if( m_cSerial.IsOpen() )
		{
			OnOffLight(FALSE, m_bLineCamOn);
			m_cSerial.Close();
		}

		if(NULL != m_ThreadNGImage)
		{
			delete m_ThreadNGImage;
			m_ThreadNGImage = NULL;
		}
		break;
	}
#endif	

	/* 20190926 IMSI
	//Model save
		//Model
	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	moddlg->Open();

	char temp[MAX_MOD_NAME];

	sprintf(temp, "%s", moddlg->GetName());
	m_label_model.SetCaption(temp);
	CString str0, str1;
	str0 = moddlg->GetName();
	str1 = moddlg->GetPath();
	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		MessageBox("Error : invalid model!", NULL, MB_ICONEXCLAMATION);
	}
	else
	{
		//Load system file
		//Load/save system parameter
		m_VisMod->SaveSystemPara(m_strDBPath);
		//Load/save system result
		m_VisMod->SaveSystemRslt(m_strDBPath);

		ModelSave();

		WriteStatusList(">>>>>Application exit!");
	}

	delete moddlg;
	*/

#ifdef _DEBUG
	_CrtDumpMemoryLeaks(); 
#endif

	EndDialog(0);
}



void CFilmInsDlg::ClickBtnenhctrlLabelSelProfilemode()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(m_lProfileMode == 0)
	{
		m_lProfileMode = 1;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("X Profile Mode");
	}
	else if(m_lProfileMode == 1)
	{
		m_lProfileMode = 2;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Y Profile Mode");
	}
	else if(m_lProfileMode == 2)
	{
		m_lProfileMode = 3;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("X/Y Profile Mode");
	}
	else if(m_lProfileMode == 3)
	{
		// 20110524 Dongwhee BEGIN
		m_lProfileMode = 4;
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Gray Value View");
	}
	else if(m_lProfileMode == 4)
	{
		// 20110603 Dongwhee BEGIN : Add Contrast View
		m_lProfileMode = 5;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Contrast View");
	}
	else if(m_lProfileMode == 5)
	{
		m_lProfileMode = 0;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Only Gray Mode");
	}
}

void CFilmInsDlg::ClickBtnenhctrlLabelSelColormode()
{
	//StartAutoDelete_Img();
	
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(m_bIsColor)
	{
		Hobject MultiChannelImage, GrayImage;
		//20110510 lee
		if(m_lColorMode == 0)
		{
			m_lColorMode = 1;
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("COLOR MODE");
			if(m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 1)
		{
			m_lColorMode = 2;	
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("RED MODE");
			if(m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 2)
		{
			m_lColorMode = 3;	
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("GREEN MODE");
			if(m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 3)
		{
			m_lColorMode = 0;
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("BLUE MODE");
			if(m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
	}
	
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapTest()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	/*
	if(!m_bInspectStart[0])
	{
		CString tmpFull_Name, tmpPath_Name, tmpFile_Name;
		BOOL bWorking;
		CFileFind finder;
		CString str;
		int leng;
		int i;
		Hobject image;
		Herror Herr;

		CString add_str;
		long pos;
		char temp[MAX_PATH];

		int chk_end = 0;

		//Read
		CFileDialog dlgFile(TRUE,NULL,NULL,
								OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
								"Image(*.*)|*.*|");

		if(dlgFile.DoModal() == IDOK)
		{
			tmpFull_Name = dlgFile.GetPathName(); 
			tmpPath_Name = dlgFile.GetPathName();
			tmpFile_Name = dlgFile.GetFileName();

			leng = tmpPath_Name.GetLength();
			for(i = (leng - 1); i > 0; i--)
			{
				if('\\' == tmpPath_Name[i])
				{
					tmpPath_Name.Delete(i, 1);
					break;
				}
				else
				{
					tmpPath_Name.Delete(i, 1);
				}
			}
			
			// build a string with wildcards
			CString strWildcard(tmpPath_Name), OldFName, path;
			strWildcard += _T("\\*]_0.*");
			
			// start working for files
			bWorking = finder.FindFile(strWildcard);
			
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				
				// skip . and .. files; otherwise, we'd
				// recur infinitely!
				if (finder.IsDots())
					continue;
				
				// if it's a directory, recursively search it
				if (finder.IsDirectory() != 0)
				{
					continue;
				}
				else
				{
					for(i = 0; i < IMAGE_NG_NUM; i++)
					{
						clear_window(m_lNGWindowID[i]);
					}
					str = finder.GetFilePath();

					pos = str.Find("]_", 0);
					memset(temp, NULL, sizeof(temp));
					for(i = 0; i < pos + 2; i++)
					{
						temp[i] = str.GetAt(i);
					}

					for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
					{	
						//20190909 by if�� �߰�
						switch(m_VisMod->GetVisIndex())
						{
							case VISION_CAM_TOP1:
							case VISION_CAM_BOTTOM:
								add_str.Format("%s%d.bmp", temp, i);
								break;
							case VISION_CAM_TOP2:
								add_str.Format("%s%d.jpg", temp, i);
								break;
						}
																	
						set_check("~give_error");
						Herr = read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], i), HTuple((LPCTSTR)add_str));
						
						
						Herror herr;
						CString status;
						
						set_check("~give_error");
						herr = copy_image(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_VisMod->GetHalNGImage(0, m_iBufIdx[0], i));
						set_check("give_error");
						if(H_MSG_TRUE == herr)
						{
						}
						else
						{
							status.Format("Halcon Error Occured at rgb1_to_gray_03!");
							m_VisMod->WriteErrorList((LPCTSTR)status);
							return;
						}
						
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);
							
						}
						else
						{
							gen_image_const(m_VisMod->GetHalImage(0, m_iBufIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);	
						}
					}

		
					if(Test())
					{

					}
					else
					{
						HandleAllMessage();
						add_str.Format("�ҷ� �߻�-�׽�Ʈ�� ��� �����Ͻðڽ��ϱ�?(%s%d.bmp)", temp, 0);
						if(IDYES != MessageBox(add_str, "Ȯ��", MB_YESNO) )
						{
							break;
						}					
					}

					chk_end++;

					if(10 < chk_end)
					{
						HandleAllMessage();
						if(IDYES != MessageBox("�׽�Ʈ�� ��� �����Ͻðڽ��ϱ�?", "Ȯ��", MB_YESNO) )
						{
							break;
						}
						else
						{
							chk_end = 0;
						}
					}

					continue;
				}

			}
		}
		
		delete dlgFile;	
	}
	else
	{
		WriteStatusList("WARRNING : �˻� ���� �� �Դϴ�!");
	}
	*/
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapSave()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	int i;
	Herror Herr;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	//image load
	CString tmpPath_Name, add_str;
	long pos;
	char temp[MAX_PATH];
				
	CFileDialog dlgFile(FALSE,"Image","*.*",
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		tmpPath_Name = dlgFile.GetPathName();
		
		//read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], 0), HTuple((LPCTSTR)tmpPath_Name));
		
		pos = tmpPath_Name.Find(".", 0);
		
		memset(temp, NULL, sizeof(temp));
		for(i = 0; i < pos; i++)
		{
			temp[i] = tmpPath_Name.GetAt(i);
		}

		for(i = 0; (i < ins_para.m_iParaBase[1]) && (i < IMAGE_MAP_MAX_NUM); i++)
		{
			set_check("~give_error");
			switch(m_VisMod->GetVisIndex())
			{
				case VISION_CAM_TOP1:
				case VISION_CAM_BOTTOM:
					add_str.Format("%s[Gray]_%d.bmp", temp, i);
					Herr = write_image(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), "bmp", 0, HTuple((LPCTSTR)add_str));
					break;
				case VISION_CAM_TOP2:
					add_str.Format("%s[Gray]_%d.bmp", temp, i);
					Herr = write_image(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), "bmp", 0, HTuple((LPCTSTR)add_str));
					break;
			}
						
			
			
			set_check("give_error");
			if(H_MSG_TRUE == Herr)
			{
				
			}
			else
			{
				gen_image_const(m_VisMod->GetHalNGImage(0, m_iNGViewIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
			}
		}	
	}
	else
	{
		//gen_image_const(m_VisMod->GetHalImage(m_iBufIdx[0], i, m_iMapIdx[0]),HTuple("byte"),2048,2048);
	}

	delete dlgFile;		
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapLoad()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.	
	int i;
	Herror Herr;
	HTuple wd, ht;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	//image load
	CString tmpPath_Name, add_str;
	long pos;
	char temp[MAX_PATH];

	CFileDialog dlgFile(TRUE,"Image","*.*",
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		tmpPath_Name = dlgFile.GetPathName();
		
		pos = tmpPath_Name.Find("]_", 0);
		
		memset(temp, NULL, sizeof(temp));

		for(i = 0; i < pos + 2; i++)
		{
			temp[i] = tmpPath_Name.GetAt(i);
		}
	
		get_image_pointer1(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), NULL, NULL, &wd, &ht);
		m_dlgImgRect[0]->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
		
		for(i = 0; (i < ins_para.m_iParaBase[1]) && (i < IMAGE_MAP_MAX_NUM); i++)
		{
			//20190909 by if�� �߰�
			switch(m_VisMod->GetVisIndex())
			{
				case VISION_CAM_TOP1:
				case VISION_CAM_BOTTOM:
					add_str.Format("%s%d.bmp", temp, i);
					break;
				case VISION_CAM_TOP2:
					add_str.Format("%s%d.jpg", temp, i);
					break;
			}

			set_check("~give_error");

			Herr = read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], i), HTuple((LPCTSTR)add_str));

			if(m_bIsColor)
				decompose3(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalRedImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], i));

			set_check("give_error");

			if(H_MSG_TRUE == Herr)
			{
				
			}
			else
			{
				gen_image_const(m_VisMod->GetHalImage(0, m_iBufIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
				CString status;
				status.Format("Halcon Error Occured at rgb1_to_gray_02!");
				m_VisMod->WriteErrorList((LPCTSTR)status);

								
			}

			disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);	
		}		

		disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	}
	else
	{
		
	}
	delete dlgFile;
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapLoad3()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int nCount = 0;
#ifdef _CAM
	ULONG StartTime, Endtime;
	CString strTime, status, strPacket;

	if(m_bLineCamOn)
	{
		if(!m_cTriggerBoard->m_bTriggerStatus)
		{
			status.Format("Error : Trigger Board Connection Error");
			WriteStatusList((LPCTSTR)status);	
			return;
		}
	}

	m_VisMod->SetNGViewMode(FALSE, -1);

	strPacket.Format("%cVIS%d,0030%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
	OnSend(strPacket);

	status.Format("Grab Start");
	WriteStatusList((LPCTSTR)status);

	Sleep(1000);

	StartTime = GetTickCount();
	
	if(m_bLineCamOn)
		GrabProcess(0, m_iBufIdx[0], USE_MAP_CNT_VIS_LINE2, m_bLineCamOn, TRUE);
	else
		GrabProcess(0, m_iBufIdx[0], IMAGE_MAP_MAX_NUM, m_bLineCamOn);

	Endtime = GetTickCount();

	strTime.Format("%d",Endtime - StartTime);
	m_label_gtime.SetWindowText(strTime);
	
	for(int i = 0; (i < IMAGE_MAP_MAX_NUM); i++)
	{
		disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);
	}


	if(m_bLineCamOn)
		disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	else
		disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());

	m_iPrcIdx[0] = m_iGrbIdx[0];
	m_iBufIdx[0] = m_iPrcIdx[0];

	m_bCheckGrab = TRUE;

	strPacket.Format("%cVIS%d,0031%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
	OnSend(strPacket);

	status.Format("Grab End");
	WriteStatusList((LPCTSTR)status);
#endif
}

void CFilmInsDlg::ClickBtnenhctrlLabelResult4()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(GetDlgItem(IDC_RECT_IMG_MAP0)->IsWindowVisible())
	{
		GetDlgItem(IDC_BTNENHCTRL_LABEL_RESULT4)->SetWindowText("INS.ITEM");
		GetDlgItem(IDC_RECT_IMG_MAP0)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP7)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_BTNENHCTRL_LABEL_RESULT4)->SetWindowText("MAP DATA");
		GetDlgItem(IDC_RECT_IMG_MAP0)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP7)->ShowWindow(SW_SHOW);
	}
}




void CFilmInsDlg::ClickBtnenhctrlLabelMap0()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap0();
#endif

	dlg.m_img0_ch0 = m_img0_ch0;
	dlg.m_img0_ch1 = m_img0_ch1;
	dlg.m_img0_ch2 = m_img0_ch2;
	dlg.m_img0_ch3 = m_img0_ch3;

	//if(m_bLineCamOn)
	//	OnOffLineLight(TRUE);

	if(dlg.DoModal()==IDOK)
	{
		m_img0_ch0 = dlg.m_img0_ch0;
		m_img0_ch1 = dlg.m_img0_ch1;
		m_img0_ch2 = dlg.m_img0_ch2;
		m_img0_ch3 = dlg.m_img0_ch3;
	
		if(m_bLineCamOn)
			OnOffLight(FALSE, m_bLineCamOn);
	}
	else
	{
		OnOffLight(FALSE, m_bLineCamOn);
	}

	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap1()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap1();
#endif

	dlg.m_img0_ch0 = m_img1_ch0;
	dlg.m_img0_ch1 = m_img1_ch1;
	dlg.m_img0_ch2 = m_img1_ch2;
	dlg.m_img0_ch3 = m_img1_ch3;

	if(dlg.DoModal()==IDOK)
	{
		m_img1_ch0 = dlg.m_img0_ch0 ;
		m_img1_ch1 = dlg.m_img0_ch1 ;
		m_img1_ch2 = dlg.m_img0_ch2 ;
		m_img1_ch3 = dlg.m_img0_ch3 ;

		if(m_bLineCamOn)
			OnOffLight(FALSE, m_bLineCamOn);
	}
	else
	{
		OnOffLight(FALSE, m_bLineCamOn);
	}

	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap2();
#endif

	dlg.m_img0_ch0 = m_img2_ch0;
	dlg.m_img0_ch1 = m_img2_ch1;
	dlg.m_img0_ch2 = m_img2_ch2;
	dlg.m_img0_ch3 = m_img2_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img2_ch0 = dlg.m_img0_ch0 ;
		m_img2_ch1 = dlg.m_img0_ch1 ;
		m_img2_ch2 = dlg.m_img0_ch2 ;
		m_img2_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap3()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap3();
#endif

	dlg.m_img0_ch0 = m_img3_ch0;
	dlg.m_img0_ch1 = m_img3_ch1;
	dlg.m_img0_ch2 = m_img3_ch2;
	dlg.m_img0_ch3 = m_img3_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img3_ch0 = dlg.m_img0_ch0 ;
		m_img3_ch1 = dlg.m_img0_ch1 ;
		m_img3_ch2 = dlg.m_img0_ch2 ;
		m_img3_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap4()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap4();
#endif

	dlg.m_img0_ch0 = m_img4_ch0;
	dlg.m_img0_ch1 = m_img4_ch1;
	dlg.m_img0_ch2 = m_img4_ch2;
	dlg.m_img0_ch3 = m_img4_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img4_ch0 = dlg.m_img0_ch0 ;
		m_img4_ch1 = dlg.m_img0_ch1 ;
		m_img4_ch2 = dlg.m_img0_ch2 ;
		m_img4_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap5()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);


	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap5();
#endif
	dlg.m_img0_ch0 = m_img5_ch0;
	dlg.m_img0_ch1 = m_img5_ch1;
	dlg.m_img0_ch2 = m_img5_ch2;
	dlg.m_img0_ch3 = m_img5_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img5_ch0 = dlg.m_img0_ch0 ;
		m_img5_ch1 = dlg.m_img0_ch1 ;
		m_img5_ch2 = dlg.m_img0_ch2 ;
		m_img5_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap6()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM
	OnRectImgMap6();
#endif
	dlg.m_img0_ch0 = m_img6_ch0;
	dlg.m_img0_ch1 = m_img6_ch1;
	dlg.m_img0_ch2 = m_img6_ch2;
	dlg.m_img0_ch3 = m_img6_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img6_ch0 = dlg.m_img0_ch0 ;
		m_img6_ch1 = dlg.m_img0_ch1 ;
		m_img6_ch2 = dlg.m_img0_ch2 ;
		m_img6_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap7()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�..
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap7();
#endif

	dlg.m_img0_ch0 = m_img7_ch0;
	dlg.m_img0_ch1 = m_img7_ch1;
	dlg.m_img0_ch2 = m_img7_ch2;
	dlg.m_img0_ch3 = m_img7_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img7_ch0 = dlg.m_img0_ch0 ;
		m_img7_ch1 = dlg.m_img0_ch1 ;
		m_img7_ch2 = dlg.m_img0_ch2 ;
		m_img7_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap8()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap8();
#endif

	dlg.m_img0_ch0 = m_img8_ch0;
	dlg.m_img0_ch1 = m_img8_ch1;
	dlg.m_img0_ch2 = m_img8_ch2;
	dlg.m_img0_ch3 = m_img8_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img8_ch0 = dlg.m_img0_ch0 ;
		m_img8_ch1 = dlg.m_img0_ch1 ;
		m_img8_ch2 = dlg.m_img0_ch2 ;
		m_img8_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap9()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap9();
#endif
	
	dlg.m_img0_ch0 = m_img9_ch0;
	dlg.m_img0_ch1 = m_img9_ch1;
	dlg.m_img0_ch2 = m_img9_ch2;
	dlg.m_img0_ch3 = m_img9_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img9_ch0 = dlg.m_img0_ch0 ;
		m_img9_ch1 = dlg.m_img0_ch1 ;
		m_img9_ch2 = dlg.m_img0_ch2 ;
		m_img9_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap10()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap10();
#endif
	
	dlg.m_img0_ch0 = m_img10_ch0;
	dlg.m_img0_ch1 = m_img10_ch1;
	dlg.m_img0_ch2 = m_img10_ch2;
	dlg.m_img0_ch3 = m_img10_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img10_ch0 = dlg.m_img0_ch0 ;
		m_img10_ch1 = dlg.m_img0_ch1 ;
		m_img10_ch2 = dlg.m_img0_ch2 ;
		m_img10_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
	
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap11()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

#ifdef _CAM	
	OnRectImgMap11();
#endif
	
	dlg.m_img0_ch0 = m_img11_ch0;
	dlg.m_img0_ch1 = m_img11_ch1;
	dlg.m_img0_ch2 = m_img11_ch2;
	dlg.m_img0_ch3 = m_img11_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img11_ch0 = dlg.m_img0_ch0 ;
		m_img11_ch1 = dlg.m_img0_ch1 ;
		m_img11_ch2 = dlg.m_img0_ch2 ;
		m_img11_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::SetStaticMapType(int vis_idx, BOOL isLineScan)
{
	switch(vis_idx)
	{
	case VISION_CAM_TOP1:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP0_TYPE1)->SetWindowText("Punch�ҷ�");
		GetDlgItem(IDC_STATIC_MAP0_TYPE2)->SetWindowText("PF�ҷ�");
		GetDlgItem(IDC_STATIC_MAP0_TYPE3)->SetWindowText("Film����");

		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Potting");
		GetDlgItem(IDC_STATIC_MAP1_TYPE1)->SetWindowText("No Bond");
		//GetDlgItem(IDC_STATIC_MAP1_TYPE2)->SetWindowText("Chip��������/Broken");		
		//GetDlgItem(IDC_STATIC_MAP1_TYPE3)->SetWindowText("Pot part(��)");

		GetDlgItem(IDC_STATIC_MAP2_TYPE0)->SetWindowText("SR ����(��/��)");
		GetDlgItem(IDC_STATIC_MAP2_TYPE1)->SetWindowText("SR ����");
		GetDlgItem(IDC_STATIC_MAP2_TYPE2)->SetWindowText("SR �ݼӼ�");
		GetDlgItem(IDC_STATIC_MAP2_TYPE3)->SetWindowText("M/K�ҷ�");

		GetDlgItem(IDC_STATIC_MAP3_TYPE0)->SetWindowText("Lead ����(�켱)");
		GetDlgItem(IDC_STATIC_MAP3_TYPE1)->SetWindowText("Lead ����/����");
		//GetDlgItem(IDC_STATIC_MAP3_TYPE2)->SetWindowText("Chip Out");
		//GetDlgItem(IDC_STATIC_MAP3_TYPE2)->SetWindowText("Pot part(��)");
		break;
	case VISION_CAM_TOP2:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP0_TYPE1)->SetWindowText("A/M Scratch");
		GetDlgItem(IDC_STATIC_MAP0_TYPE2)->SetWindowText("Chip Out/Broken");
		GetDlgItem(IDC_STATIC_MAP0_TYPE3)->SetWindowText("Chip �̸��������");

		//GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Chip Out/Broken");					//IMSI by �Ի� �� ����
		//GetDlgItem(IDC_STATIC_MAP2_TYPE1)->SetWindowText("Chip �̸��������");
		break;
	case VISION_CAM_BOTTOM:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");

		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Ink Chip");
		GetDlgItem(IDC_STATIC_MAP1_TYPE1)->SetWindowText("����");

		GetDlgItem(IDC_STATIC_MAP2_TYPE0)->SetWindowText("����/������(��)");
		//GetDlgItem(IDC_STATIC_MAP2_TYPE1)->SetWindowText("PI Scratch");

		//GetDlgItem(IDC_STATIC_MAP3_TYPE0)->SetWindowText("�����ҷ�");

		GetDlgItem(IDC_STATIC_M8)->SetWindowText(" ");
		GetDlgItem(IDC_STATIC_M9)->SetWindowText(" ");
		GetDlgItem(IDC_STATIC_M10)->SetWindowText(" ");
		GetDlgItem(IDC_STATIC_M11)->SetWindowText(" ");
		break;
	}
	
}


void CFilmInsDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}


//Message Map Fucntions END
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//�� ��� �ڵ�
//
/*

void CFilmInsDlg::ScreenCapture(CString strPath)
{
	CRect rect;
	HDC h_screen_dc;
	m_hWnd = GetSafeHwnd();
	h_screen_dc = ::GetDC(m_hWnd);

	GetWindowRect(&rect);

	int width = 1280;
	int height = 1024;
	
	// DIB�� ������ �����Ѵ�. 
	BITMAPINFO dib_define;
	dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib_define.bmiHeader.biWidth = width;
	dib_define.bmiHeader.biHeight = height;
	dib_define.bmiHeader.biPlanes = 1;
	dib_define.bmiHeader.biBitCount = 24;
	dib_define.bmiHeader.biCompression = BI_RGB;
	dib_define.bmiHeader.biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
	dib_define.bmiHeader.biXPelsPerMeter = 0;
	dib_define.bmiHeader.biYPelsPerMeter = 0;
	dib_define.bmiHeader.biClrImportant = 0;
	dib_define.bmiHeader.biClrUsed = 0;
	
	// DIB�� ���� �̹��� ��Ʈ ������ ������ ������ ����
	BYTE *p_image_data = NULL;
	// dib_define�� ���ǵ� �������� DIB�� �����Ѵ�.
	HBITMAP h_bitmap = ::CreateDIBSection(h_screen_dc, &dib_define, DIB_RGB_COLORS, (void **)&p_image_data, 0, 0);
	// �̹����� �����ϱ� ���ؼ� ���� DC�� �����Ѵ�. ���� DC������ ���������� ��Ʈ�ʿ� �����Ͽ�
	// �̹��� ������ ���� �� ���� �����̴�.
	HDC h_memory_dc = ::CreateCompatibleDC(h_screen_dc);
	// ���� DC�� �̹����� ������ ��Ʈ���� �����Ѵ�.
	HBITMAP h_old_bitmap = (HBITMAP)::SelectObject(h_memory_dc, h_bitmap);
	// ���� ��ũ�� ȭ���� ĸ���Ѵ�.
	::BitBlt(h_memory_dc, 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);
	// ������ ��Ʈ������ �����Ѵ�.
	::SelectObject(h_memory_dc, h_old_bitmap); 
	// ���� DC�� �����Ѵ�.
	DeleteDC(h_memory_dc);
	
	// DIB ������ ������ �����Ѵ�.
	BITMAPFILEHEADER dib_format_layout;
	ZeroMemory(&dib_format_layout, sizeof(BITMAPFILEHEADER));
	dib_format_layout.bfType = *(WORD*)"BM";
	dib_format_layout.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dib_define.bmiHeader.biSizeImage;
	dib_format_layout.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	// DIB ������ �����Ѵ�.
	FILE *p_file = fopen("E:\\ScreenCapture.bmp", "wb");
	AfxMessageBox(strPath);
	if(p_file != NULL){
		fwrite(&dib_format_layout, 1, sizeof(BITMAPFILEHEADER), p_file);
		fwrite(&dib_define, 1, sizeof(BITMAPINFOHEADER), p_file);
		fwrite(p_image_data, 1, dib_define.bmiHeader.biSizeImage, p_file);
		fclose(p_file);
	}
	
	if(NULL != h_bitmap) DeleteObject(h_bitmap);
	// if(NULL != h_screen_dc) ::ReleaseDC(NULL, h_screen_dc); 
	if(NULL != h_screen_dc) ::ReleaseDC(m_hWnd, h_screen_dc); 
	// ĸ�ĸ� �ϱ� ���ؼ� ���� ȭ���� �ٽ� �����ش�.
	fclose(p_file);
	//ShowWindow(SW_SHOW);
}


void CFilmInsDlg::ChangeStateLightValueEditBox()
{
	BOOL state;
	state = m_dlgSetup->m_SysPara.m_bPara[34];

	GetDlgItem(IDC_EDIT_IMG0_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH3)->EnableWindow(state);
}

*/

BOOL CFilmInsDlg::StartAutoDelete_Img()
{
	//���� ������ 0 �� ��� �ƹ��͵� ���� �ʴ´�. 20150821 by
	if(0 >= m_dlgSetup->m_SysPara.m_iPara[0])
		return TRUE;

	CString Delete_Img_dir,Delete_Data_dir,Delete_Ng_Data_dir,Delete_Tab_Data_dir;
	CString Delete_Product_dir,Delete_Spc_dir;
	CTime now_time = CTime::GetCurrentTime();
	BOOL bDelFolder[31];
	
	CheckDeleteDate();

	int nYear = m_iDel_Year;
	int nDelYear = now_time.GetYear();

	if(1 == now_time.GetMonth() && now_time.GetDay() < m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year;
	}
	else if(1 == now_time.GetMonth() && now_time.GetDay() > m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year - 1;
	}

	for(int nYear = m_iDel_Year; nYear >= nDelYear; nYear--)		// 110930 ytlee
	{
		if(nYear != now_time.GetYear())
		{
			m_iDel_Month = 12;
			m_iDel_Day = m_iDel_RecentDay;		// 120104 ytlee
		}

		for(int nMonth = m_iDel_Month; nMonth > 0; nMonth--)
		{
			if(nMonth == 1 || nMonth == 3 || nMonth == 5 ||
				nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12)		// 120104 ytlee
			{
				m_iDel_Day = 31;
			}
			else if(nMonth == 4 || nMonth == 6 || nMonth == 9 || nMonth == 11)
			{
				m_iDel_Day = 30;
			}
			else if(nMonth == 2)
			{
				m_iDel_Day = 28;
			}
			if(nYear == now_time.GetYear() && (nMonth == now_time.GetMonth() || m_dlgSetup->m_SysPara.m_iPara[0] > now_time.GetDay()))		// 110901 ytlee
			{
				m_iDel_Day = m_iDel_RecentDay;
			}
			else
			{
				m_iDel_Day = 31;
			}

			for(int i = 0; i < 31; i++)
			{
				bDelFolder[i] = FALSE;
			}


			for(int nDay = 1; nDay <= m_iDel_Day; nDay++)
			{
				if(nMonth>9 && nDay>9)
					Delete_Img_dir.Format("%s\\%d%d%d",m_strLogPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay>9)
					Delete_Img_dir.Format("%s\\%d0%d%d",m_strLogPath, nYear, nMonth, nDay);
				else if(nMonth>9 && nDay<10)
					Delete_Img_dir.Format("%s\\%d%d0%d",m_strLogPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay<10)
					Delete_Img_dir.Format("%s\\%d0%d0%d",m_strLogPath, nYear, nMonth, nDay);

				CFileFind dbfile;

				if(dbfile.FindFile(Delete_Img_dir,0))
				{
					if(DeleteFolder(Delete_Img_dir))
						RemoveDirectory(Delete_Img_dir);
				}

				if(DeleteFolderSearch(Delete_Img_dir))
				{
					bDelFolder[nDay - 1] = TRUE;
				}
				else
				{
					bDelFolder[nDay - 1] = FALSE;
				}
			}

			for(int i = 0; i < m_iDel_Day; i++)
			{
				if(!bDelFolder[i])
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void CFilmInsDlg::OnNMRClickListTcp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	
	m_list_tcp.DeleteAllItems();
	
}


void CFilmInsDlg::ClickBtnenhctrl2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�

	m_label_tot.SetCaption("0");
	m_label_good.SetCaption("0");
	m_label_ng.SetCaption("0");

}

void CFilmInsDlg::ClickBtnenhctrlLabelNgview()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRect sys_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);

	MoveWindow(sys_rect.Width() - NGVIEW_MOVE_OFFSET, 0, NGVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMainview()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRect sys_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);
	MoveWindow(sys_rect.Width() - MAINVIEW_MOVE_OFFSET, 0, MAINVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);
}

void CFilmInsDlg::ClickBtnenhctrlLabelSelOcvautoteach()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(IDYES == AfxMessageBox("�ڵ� Ȱ�� ƼĪ�� �����ұ��?", MB_YESNO))
	{
	
		if(m_VisMod->GetVisIndex() != VISION_CAM_TOP1)
			return;

		typeInspectPara ins_para;	
		ins_para = m_VisMod->GetInspectPara(0);

		//Grab
		OnOffLight(TRUE, m_bLineCamOn);

		SetLight(m_iLightChIdx[0], m_img2_ch0);
		SetLight(m_iLightChIdx[1], m_img2_ch1);
		SetLight(m_iLightChIdx[2], m_img2_ch2);
		SetLight(m_iLightChIdx[3], m_img2_ch3);
		
		Sleep(100);

		m_cCam.Grab(0, 0, 0, m_VisMod->GetHalImage(0, m_iBufIdx[0], ins_para.m_iParaMapOCV[0]));

		Sleep(100);

		OnOffLight(FALSE, m_bLineCamOn);

		if(AutoTeachOCV())
		{
			::SendMessage(m_hWnd, USER_MSG_SOCKET, 1, 4);	// Success
		}
		else
		{
			::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 4);	// Fail
		}
	}
}


//�˻翵���� NG view �� ǥ���Ѵ�. 20150807 by
void CFilmInsDlg::DisplayDeviceGood(Hobject *RsltRegion, int cur_scroll_idx, int ng_view_idx, int disp_map_num)
{
	CRect rect;
	long sx, sy, ex, ey;
	HTuple Num, CRow, CCol, Row0, Col0, Row1, Col1, Area, Mean;
	Hobject SelectRgn, obj_red, obj_green, obj_blue;
	double interval;

	area_center(*RsltRegion, &Area, &CRow, &CCol);
	smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);

	if(ScrollCheck(cur_scroll_idx))
	{
		rect = CRect((Hlong)Row0[0], (Hlong)Col0[0], (Hlong)Row1[0], (Hlong)Col1[0]);
		//cx, cy, sizex, sizey
		
		sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
		sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
		ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
		ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;

		interval = (double)(ey - sy) / (double)NG_VIEW_TEXT_GAP;
		
		set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);

		decompose3(*m_VisMod->GetHalNGImage(0, ng_view_idx, disp_map_num), &obj_red, &obj_green, &obj_blue);		//20191203_2 by �׽�Ʈ ��
		
		//disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, disp_map_num), m_lNGWindowID[m_iNGSubViewIdx[0]]);
		disp_obj(obj_red, m_lNGWindowID[m_iNGSubViewIdx[0]]);

		set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("blue"));
		
		select_obj(*RsltRegion, &SelectRgn, 1);
		disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);

		set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx);
		write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "GOOD : A/M");

		//20160303 by
		//AM ��ǰ�� ��� ��չ�� ���
		set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("green"));
		set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], (Hlong)(sy + (15*interval)), sx);
		intensity(SelectRgn, *m_VisMod->GetHalNGImage(0, ng_view_idx, disp_map_num), &Mean, _);

		write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Average : ") + 	HTuple((Hlong)Mean[0]) ));
																							

		ScrollCheck();
	}

}
void CFilmInsDlg::ClickBtnenhctrlLabelCamera()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//CString from(_T("E:\\Insp_Result_1\\20191227\\IMAGE\\NG"));
/*
	HTuple StartSecond, EndSecond;
	long lGrabTime;
	


	CString from(_T("D:\\Images"));
	CString to;



	to.Format("z:\\IMAGE\\NG");

	MakeDirectories(to);



	count_seconds(&StartSecond);

	CopyDirectory(from, to);

	EndSecond = EndSecond - StartSecond;
*/	


	//int a = m_VisMod->GetListSize();

	//sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, m_strLotNo);
	//CString path;

	//path.Format("E:\\Insp_Result_1\\20191024\\IMAGE");
			
	//DeleteFolder(path);
	/*
	if(m_cSerial.IsOpen())
	{
		
		char data[128];
		memset(data, NULL, sizeof(char) * 128);

		if(g_LightOn)
			sprintf(data, ":1W%d%d%d%d%d\r\n", m_iLightChIdx[0] + 70, 1, 1, 1, 1);
		else
			sprintf(data, ":1W%d%d%d%d%d\r\n", m_iLightChIdx[0] + 70, 0, 0, 0, 0);


		g_LightOn = !g_LightOn;
*/
		//20190910 by �����ڿ��� ���� �����ߴ� ���� ��������
		/*
		if(LightOn)
			sprintf(data, "setonex ffffffff%c", 0x0D);
		else
			sprintf(data, "setonex 00000000%c", 0x0D);
	
		
		//m_cSerial.Write(data, strlen(data));
	}
	*/
}

void CFilmInsDlg::ClickBtnenhctrlLabelOnline()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, SOCKET_PORT_NUM1);
		break;
	case VISION_CAM_TOP2:
		m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, SOCKET_PORT_NUM2);
		break;
	case VISION_CAM_BOTTOM:
		m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, SOCKET_PORT_NUM3);
		break;
	}
}

void CFilmInsDlg::CopyDirectory(CString defPath, CString copyTo)
{
	CFileFind cFileFinder;
	CString fName;
	BOOL bRlt = cFileFinder.FindFile(defPath + "\\*.*");

	while (bRlt) {
		bRlt = cFileFinder.FindNextFile();
		
		if (cFileFinder.IsDots()) continue;

		fName = cFileFinder.GetFileName();
	
		if (cFileFinder.IsDirectory()) {
			CreateDirectory(copyTo + "\\" + fName, NULL);
			CopyDirectory(defPath + "\\" + fName, copyTo + "\\" + fName); // ���� ���� Ȯ��
		}
		else if (cFileFinder.IsArchived()) // ���Ϻ���
			::CopyFile(defPath + "\\" + fName, copyTo + "\\" + fName, FALSE);
	}
	cFileFinder.Close();

}