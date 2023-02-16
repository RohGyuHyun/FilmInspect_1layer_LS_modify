// LightCtrlDlg.cpp : ���� �����Դϴ�.
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

#include "LightCtrlDlg.h"


// CLightCtrlDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLightCtrlDlg, CDialog)

CLightCtrlDlg::CLightCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightCtrlDlg::IDD, pParent)
	, m_img0_ch4(0)
	, m_img0_ch5(0)
	, m_img0_ch6(0)
	, m_img0_ch7(0)
	, m_bLineLightFan(FALSE)
{
	pCFilmInsDlg = NULL;
}

CLightCtrlDlg::~CLightCtrlDlg()
{
}

void CLightCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_cCh1SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER2, m_cCh2SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER3, m_cCh3SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER4, m_cCh4SliderCtrl);
	DDX_Text(pDX, IDC_CH0_EDIT, m_img0_ch0);
	DDX_Text(pDX, IDC_CH1_EDIT, m_img0_ch1);
	DDX_Text(pDX, IDC_CH2_EDIT, m_img0_ch2);
	DDX_Text(pDX, IDC_CH3_EDIT, m_img0_ch3);
	DDX_Control(pDX, IDC_SLIDER5, m_cCh5SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER6, m_cCh6SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER7, m_cCh7SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER8, m_cCh8SliderCtrl);
	DDX_Text(pDX, IDC_CH4_EDIT, m_img0_ch4);
	DDX_Text(pDX, IDC_CH5_EDIT, m_img0_ch5);
	DDX_Text(pDX, IDC_CH6_EDIT, m_img0_ch6);
	DDX_Text(pDX, IDC_CH7_EDIT, m_img0_ch7);
	DDX_Check(pDX, IDC_CHECK_LINE_FAN, m_bLineLightFan);
	DDX_Control(pDX, IDC_CHECK_LINE_FAN, m_ctrlLineLightFan);
}


BEGIN_MESSAGE_MAP(CLightCtrlDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CLightCtrlDlg::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CLightCtrlDlg::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CLightCtrlDlg::OnNMCustomdrawSlider3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, &CLightCtrlDlg::OnNMCustomdrawSlider4)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER5, &CLightCtrlDlg::OnNMCustomdrawSlider5)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER6, &CLightCtrlDlg::OnNMCustomdrawSlider6)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER7, &CLightCtrlDlg::OnNMCustomdrawSlider7)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER8, &CLightCtrlDlg::OnNMCustomdrawSlider8)
	ON_BN_CLICKED(IDC_CHECK_LINE_FAN, &CLightCtrlDlg::OnBnClickedCheckLineFan)
END_MESSAGE_MAP()


// CLightCtrlDlg �޽��� ó�����Դϴ�.

void CLightCtrlDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh1SliderCtrl.GetPos());
	GetDlgItem(IDC_CH0_EDIT)->SetWindowText(tmp);

#ifdef _CAM
	if(pCFilmInsDlg->m_bLineCamOn)	
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[0], m_cCh1SliderCtrl.GetPos());
	else
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[0], m_cCh1SliderCtrl.GetPos());

	//pCFilmInsDlg->SetLineLight(0, m_cCh1SliderCtrl.GetPos());

#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh2SliderCtrl.GetPos());
	GetDlgItem(IDC_CH1_EDIT)->SetWindowText(tmp);

#ifdef _CAM
	if(pCFilmInsDlg->m_bLineCamOn)	
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[1], m_cCh2SliderCtrl.GetPos());
	else
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[1], m_cCh2SliderCtrl.GetPos());

	//pCFilmInsDlg->SetLineLight(1, m_cCh2SliderCtrl.GetPos());
#endif
	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh3SliderCtrl.GetPos());
	GetDlgItem(IDC_CH2_EDIT)->SetWindowText(tmp);

#ifdef _CAM
	if(pCFilmInsDlg->m_bLineCamOn)
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[2], m_cCh3SliderCtrl.GetPos());
	else
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[2], m_cCh3SliderCtrl.GetPos());
#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh4SliderCtrl.GetPos());
	GetDlgItem(IDC_CH3_EDIT)->SetWindowText(tmp);

#ifdef _CAM	
	if(pCFilmInsDlg->m_bLineCamOn)
	{	

	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[3], m_cCh4SliderCtrl.GetPos());
	}
#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh5SliderCtrl.GetPos());
	GetDlgItem(IDC_CH4_EDIT)->SetWindowText(tmp);

#ifdef _CAM	
	if(pCFilmInsDlg->m_bLineCamOn)
	{	
		
	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[0], m_cCh5SliderCtrl.GetPos());
	}
#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider6(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh6SliderCtrl.GetPos());
	GetDlgItem(IDC_CH5_EDIT)->SetWindowText(tmp);

#ifdef _CAM	
	if(pCFilmInsDlg->m_bLineCamOn)
	{	

	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[1], m_cCh6SliderCtrl.GetPos());
	}
#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider7(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh7SliderCtrl.GetPos());
	GetDlgItem(IDC_CH6_EDIT)->SetWindowText(tmp);

#ifdef _CAM	
	if(pCFilmInsDlg->m_bLineCamOn)
	{	

	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[2], m_cCh7SliderCtrl.GetPos());
	}
#endif

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider8(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh8SliderCtrl.GetPos());
	GetDlgItem(IDC_CH7_EDIT)->SetWindowText(tmp);

#ifdef _CAM	
	if(pCFilmInsDlg->m_bLineCamOn)
	{	

	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[3], m_cCh8SliderCtrl.GetPos());
	}
#endif

	*pResult = 0;
}


BOOL CLightCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	pCFilmInsDlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int nSetRange = 0;

	switch(pCFilmInsDlg->m_VisMod->GetVisIndex())
	{
		case VISION_CAM_TOP1:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("���̵�");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("�����簢");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("����");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("�����Ʈ");

			GetDlgItem(IDC_STATIC_LABEL_CH_5)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_6)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_7)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_8)->SetWindowText("-");

			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(TRUE);
			m_cCh4SliderCtrl.EnableWindow(TRUE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(TRUE);

			GetDlgItem(IDC_STATIC_LABEL_CH_5)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_6)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_7)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_8)->EnableWindow(FALSE);

			m_cCh5SliderCtrl.EnableWindow(FALSE);
			m_cCh6SliderCtrl.EnableWindow(FALSE);
			m_cCh7SliderCtrl.EnableWindow(FALSE);
			m_cCh8SliderCtrl.EnableWindow(FALSE);

			m_cCh5SliderCtrl.ShowWindow(FALSE);
			m_cCh6SliderCtrl.ShowWindow(FALSE);
			m_cCh7SliderCtrl.ShowWindow(FALSE);
			m_cCh8SliderCtrl.ShowWindow(FALSE);

			GetDlgItem(IDC_CH4_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH5_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH6_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH7_EDIT)->EnableWindow(FALSE);

			GetDlgItem(IDC_CH4_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH5_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH6_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH7_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CHECK_LINE_FAN)->EnableWindow(FALSE);

			break;
		case VISION_CAM_TOP2:
			if(pCFilmInsDlg->m_bLineCamOn)
			{
				GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("���̵�����(RED)#1");
				GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("���̵�����(RED)#2");
				GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("��������(BLUE)");

				GetDlgItem(IDC_STATIC_LABEL_CH_4)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_5)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_6)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_7)->ShowWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_8)->ShowWindow(FALSE);

				m_cCh4SliderCtrl.ShowWindow(FALSE);
				m_cCh5SliderCtrl.ShowWindow(FALSE);
				m_cCh6SliderCtrl.ShowWindow(FALSE);
				m_cCh7SliderCtrl.ShowWindow(FALSE);
				m_cCh8SliderCtrl.ShowWindow(FALSE);
			
				GetDlgItem(IDC_CH3_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH4_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH5_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH6_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH7_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CHECK_LINE_FAN)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("���̵�");
				GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("�����簢");
				GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("����");
				GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");

				GetDlgItem(IDC_STATIC_LABEL_CH_5)->SetWindowText("-");
				GetDlgItem(IDC_STATIC_LABEL_CH_6)->SetWindowText("-");
				GetDlgItem(IDC_STATIC_LABEL_CH_7)->SetWindowText("-");
				GetDlgItem(IDC_STATIC_LABEL_CH_8)->SetWindowText("-");

				GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(TRUE);
				m_cCh4SliderCtrl.EnableWindow(TRUE);
				GetDlgItem(IDC_CH3_EDIT)->EnableWindow(TRUE);

				GetDlgItem(IDC_STATIC_LABEL_CH_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_STATIC_LABEL_CH_8)->EnableWindow(FALSE);

				m_cCh5SliderCtrl.EnableWindow(FALSE);
				m_cCh6SliderCtrl.EnableWindow(FALSE);
				m_cCh7SliderCtrl.EnableWindow(FALSE);
				m_cCh8SliderCtrl.EnableWindow(FALSE);

				m_cCh5SliderCtrl.ShowWindow(FALSE);
				m_cCh6SliderCtrl.ShowWindow(FALSE);
				m_cCh7SliderCtrl.ShowWindow(FALSE);
				m_cCh8SliderCtrl.ShowWindow(FALSE);

				GetDlgItem(IDC_CH4_EDIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CH5_EDIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CH6_EDIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CH7_EDIT)->EnableWindow(FALSE);

				GetDlgItem(IDC_CH4_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH5_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH6_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CH7_EDIT)->ShowWindow(FALSE);
				GetDlgItem(IDC_CHECK_LINE_FAN)->EnableWindow(FALSE);
			}
			break;
		case VISION_CAM_BOTTOM:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("�����Ʈ");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("����#1");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("����#2");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");

			GetDlgItem(IDC_STATIC_LABEL_CH_5)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_6)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_7)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_8)->SetWindowText("-");

			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_5)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_6)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_7)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_LABEL_CH_8)->EnableWindow(FALSE);

			m_cCh4SliderCtrl.EnableWindow(FALSE);
			m_cCh5SliderCtrl.EnableWindow(FALSE);
			m_cCh6SliderCtrl.EnableWindow(FALSE);
			m_cCh7SliderCtrl.EnableWindow(FALSE);
			m_cCh8SliderCtrl.EnableWindow(FALSE);

			m_cCh4SliderCtrl.ShowWindow(FALSE);
			m_cCh5SliderCtrl.ShowWindow(FALSE);
			m_cCh6SliderCtrl.ShowWindow(FALSE);
			m_cCh7SliderCtrl.ShowWindow(FALSE);
			m_cCh8SliderCtrl.ShowWindow(FALSE);

			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH4_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH5_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH6_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CH7_EDIT)->EnableWindow(FALSE);

			GetDlgItem(IDC_CH3_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH4_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH5_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH6_EDIT)->ShowWindow(FALSE);
			GetDlgItem(IDC_CH7_EDIT)->ShowWindow(FALSE);

			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_LINE_FAN)->EnableWindow(FALSE);	
			break;
		default:

			break;
	}

	if(pCFilmInsDlg->m_bLineCamOn)
		nSetRange = 480;
	else
		nSetRange = 255;	

	m_cCh1SliderCtrl.SetRange(0,nSetRange);
	m_cCh2SliderCtrl.SetRange(0,nSetRange);
	m_cCh3SliderCtrl.SetRange(0,nSetRange);
	m_cCh4SliderCtrl.SetRange(0,nSetRange);

	m_cCh1SliderCtrl.SetPos(m_img0_ch0);
	m_cCh2SliderCtrl.SetPos(m_img0_ch1);
	m_cCh3SliderCtrl.SetPos(m_img0_ch2);
	m_cCh4SliderCtrl.SetPos(m_img0_ch3);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CLightCtrlDlg::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	EndDialog(0);
	CDialog::OnClose();
}
BEGIN_EVENTSINK_MAP(CLightCtrlDlg, CDialog)
	ON_EVENT(CLightCtrlDlg, IDC_BTNENHCTRL_LIGHT_OK, DISPID_CLICK, CLightCtrlDlg::ClickBtnenhctrlLightOk, VTS_NONE)
	ON_EVENT(CLightCtrlDlg, IDC_BTNENHCTRL_LIGHT_CANCEL, DISPID_CLICK, CLightCtrlDlg::ClickBtnenhctrlLightCancel, VTS_NONE)
END_EVENTSINK_MAP()

void CLightCtrlDlg::ClickBtnenhctrlLightOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_img0_ch0 = m_cCh1SliderCtrl.GetPos();
	m_img0_ch1 = m_cCh2SliderCtrl.GetPos();
	m_img0_ch2 = m_cCh3SliderCtrl.GetPos();
	m_img0_ch3 = m_cCh4SliderCtrl.GetPos();

	OnOK();
}

void CLightCtrlDlg::ClickBtnenhctrlLightCancel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

#ifdef _CAM
	if(pCFilmInsDlg->m_bLineCamOn)
	{
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[0], m_img0_ch0);
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[1], m_img0_ch1);
		pCFilmInsDlg->SetLineLight(pCFilmInsDlg->m_iLightChIdx[2], m_img0_ch2);

	}
	else
	{
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[0], m_img0_ch0);
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[1], m_img0_ch1);
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[2], m_img0_ch2);
		pCFilmInsDlg->SetLight(pCFilmInsDlg->m_iLightChIdx[3], m_img0_ch3);
	}
#endif

	OnCancel();
}

void CLightCtrlDlg::OnBnClickedCheckLineFan()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	/*
	if(m_bLineLightFan)
	{
		GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
		m_cCh4SliderCtrl.EnableWindow(FALSE);

		GetDlgItem(IDC_CH7_EDIT)->EnableWindow(FALSE);
		m_cCh8SliderCtrl.EnableWindow(FALSE);
		m_bLineLightFan = FALSE;
	}
	else
	{
		GetDlgItem(IDC_CH3_EDIT)->EnableWindow(TRUE);
		m_cCh4SliderCtrl.EnableWindow(TRUE);

		GetDlgItem(IDC_CH7_EDIT)->EnableWindow(TRUE);
		m_cCh8SliderCtrl.EnableWindow(TRUE);
		m_bLineLightFan = TRUE;
	}
	*/
}