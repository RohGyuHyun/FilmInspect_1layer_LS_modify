// SetUpMotAxisZ.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "SetUpMotAxisZ.h"


// CSetUpMotAxisZ 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpMotAxisZ, CDialog)

CSetUpMotAxisZ::CSetUpMotAxisZ(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpMotAxisZ::IDD, pParent)
	, m_edit_nStepVal(0)
{
	m_pAxis = NULL;
	m_pAxis = new CAxis();

	m_nRadioMoveType = 0;
	m_dJogSpeed	= 1.0;
	m_nProcessMoveIdx = 0;

	for(int i = 0; i < AXIS_POS_MAX_NUM; i++)
		m_lPos[i] = 0;
}

CSetUpMotAxisZ::~CSetUpMotAxisZ()
{
#ifdef _CAM
	m_pAxis->SetReadIOStart(FALSE);

	m_pAxis->Release();
#endif

	if(NULL != m_pAxis)
	{
		delete m_pAxis;
		m_pAxis = NULL;
	}
}

void CSetUpMotAxisZ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_AXIS_Z_JOG_SPEED, m_combo_move_speed);
	DDX_Text(pDX, IDC_EDIT_AXIS_Z_STEP_VALUE, m_edit_nStepVal);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVE_JOG, m_CBtnJog);

	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_LABEL_TITLE3, m_CBtnAxisName);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_CURRENT_POS, m_CBtnAxisCurPos);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_LIMIT_PLUS, m_CBtnAxisSensorP);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_LIMIT_MINUS, m_CBtnAxisSensorN);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_ORG, m_CBtnAxisHome);
	DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_PROGRESS, m_ctrl_progress);

	for(int i = 0; i < AXIS_POS_MAX_NUM; i++)
		DDX_Control(pDX, IDC_BTNENHCTRL_MOTION_AXIS_Z_TEACH_POS1 + i, m_CBtnAxisPos[i]);

	
}


BEGIN_MESSAGE_MAP(CSetUpMotAxisZ, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS_Z_JOG_SPEED, &CSetUpMotAxisZ::OnCbnSelchangeComboAxisZJogSpeed)
END_MESSAGE_MAP()

// CSetUpMotAxisZ 메시지 처리기입니다.

BOOL CSetUpMotAxisZ::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifdef _CAM

	if(!m_pAxis->Initialize(DEFAULT_PORT_NUMBER))
	{
		AfxMessageBox(_T("Error : Initialize AXIS set fail!"));
	}
#endif

	m_combo_move_speed.AddString(_T("LOW"));
	m_combo_move_speed.AddString(_T("MID"));
	m_combo_move_speed.AddString(_T("HIGH"));
	m_combo_move_speed.SetCurSel(0);

	m_ctrl_progress.SetProgressBarMin(0);
	m_ctrl_progress.SetProgressBarMax(100);
	m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
	m_ctrl_progress.SetCaption(_T(" "));
	m_ctrl_progress.ShowWindow(FALSE);

	m_CBtnJog.SetValue(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSetUpMotAxisZ::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

#ifdef _CAM
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEUP)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == AXIS_MOVE_TYPE_JOG)
				m_pAxis->JogMove(AXIS_Z, (DWORD)(AXIS_BASE_VEL * m_dJogSpeed), FALSE);

		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEDOWN)->GetSafeHwnd())	
		{
			if (m_nRadioMoveType == AXIS_MOVE_TYPE_JOG)
				m_pAxis->JogMove(AXIS_Z, (DWORD)(AXIS_BASE_VEL * m_dJogSpeed), TRUE);

			
		}
		break;
	case WM_LBUTTONUP:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEUP)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEDOWN)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == AXIS_MOVE_TYPE_JOG)
				m_pAxis->Stop(AXIS_Z);
		}
		break;
	}
#endif

	return CDialog::PreTranslateMessage(pMsg);
}

void CSetUpMotAxisZ::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
#ifdef _CAM
		m_pAxis->SetReadIOStart(TRUE);
		SetTimer(TIMER_IO_READ, 100, NULL);
#endif
	}
	else
	{
#ifdef _CAM
		m_pAxis->SetReadIOStart(FALSE);
		KillTimer(TIMER_IO_READ);
#endif
	}

	//UpdatePara(bShow);
}

void CSetUpMotAxisZ::OnCbnSelchangeComboAxisZJogSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch (m_combo_move_speed.GetCurSel())
	{
	case 0:
		m_dJogSpeed = 0.1;
		break;
	case 1:
		m_dJogSpeed = 1;
		break;
	case 2:
		m_dJogSpeed = 2;
		break;
	}
}

void CSetUpMotAxisZ::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int rslt = 0;

	switch(nIDEvent)
	{
	case TIMER_IO_READ:
		//DisplayReadIO();
		DispCurPos();
		DispAxisState();
		break;
	case TIMER_ORIGIN_DLG:
	
		if (!m_pAxis->AxisIsHomeFinished(AXIS_Z))
		{
			rslt = m_pAxis->MotReadOriginResult(AXIS_Z);

			//원점 완료
			if(ORIGIN_OK == rslt)
			{
				KillTimer(TIMER_ORIGIN_DLG);

				m_pAxis->SetAxisHomeFinished(AXIS_Z, TRUE);

				m_nProcessMoveIdx = 0;
				m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
				m_ctrl_progress.ShowWindow(SW_HIDE);
			}
			//원점 동작 중..
			else if(ORIGIN_SEARCHING == rslt)
			{
				if (m_nProcessMoveIdx >= 100)
					m_nProcessMoveIdx = 0;

				m_nProcessMoveIdx++;
				m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
			}
		}
		break;
	default:
		
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CSetUpMotAxisZ::DispCurPos()
{
	CString strPos = _T("");

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		strPos.Format(_T("%d"), m_pAxis->GetCurrentPos(i));

		if(strPos.Compare(m_CBtnAxisCurPos.GetCaption()))
			m_CBtnAxisCurPos.SetCaption(strPos);
	}
}

void CSetUpMotAxisZ::DispAxisState()
{
	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		//리미트 P
		if(m_pAxis->GetPosSensor(i))
		{
			if (m_CBtnAxisSensorP.GetBackColor() != RED)
				m_CBtnAxisSensorP.SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorP.GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorP.SetBackColor(LTYELLOW);
		}

		//리미트 N
		if(m_pAxis->GetNegSensor(i))
		{
			if (m_CBtnAxisSensorN.GetBackColor() != RED)
				m_CBtnAxisSensorN.SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorN.GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorN.SetBackColor(LTYELLOW);
		}

		//서보
		if(m_pAxis->GetAmpFaultState(i))
		{
			if (m_CBtnAxisName.GetBackColor() != GREEN)
			{
				m_CBtnAxisName.SetBackColor(GREEN);
			}
		}
		else
		{
			if (m_CBtnAxisName.GetBackColor() != LTYELLOW)
			{
				m_CBtnAxisName.SetBackColor(RED);
			}
		}

		//HOME
		if(m_pAxis->AxisIsHomeFinished(i))
		{
			if (m_CBtnAxisHome.GetBackColor() != GREEN)
			{
				m_CBtnAxisHome.SetBackColor(GREEN);
			}
		}
		else
		{
			if (m_CBtnAxisHome.GetBackColor() != RED)
			{
				m_CBtnAxisHome.SetBackColor(RED);
			}
		}
	}
}


void CSetUpMotAxisZ::AxisMove()
{
	UpdateData(TRUE);

	if (m_nRadioMoveType != AXIS_MOVE_TYPE_STEP)
		return;

	int nDialogID;

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	switch (nDialogID)
	{
	case IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEUP:
		if (m_pAxis->Incmove(AXIS_Z, (long)((m_edit_nStepVal) * -1), (DWORD)(AXIS_BASE_VEL * m_dJogSpeed)))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEDOWN:
		if(m_pAxis->Incmove(AXIS_Z, (long)(m_edit_nStepVal), (DWORD)(AXIS_BASE_VEL * m_dJogSpeed)))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	}
}

void CSetUpMotAxisZ::AxisMovePos(int nIdx, int nVal)
{
	m_pAxis->Absmove(AXIS_Z, nVal, AXIS_BASE_VEL);
}


void CSetUpMotAxisZ::DisplayReadIO()
{
	/*
	int i;
	LV_ITEM lvitem;

	CString str;
	BOOL state;

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	for(i = 0; i < MAX_OUTPUT_NUM; i++)
	{
		state = m_pAxis->GetOutput(i);

		if(m_bOutputState[i] == state)
			continue;

		m_bOutputState[i] = state;

		m_listctrl_IO_Output.GetItem(&lvitem);
		str.Format(_T("Y%03d"), i);

		lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem=i;
		lvitem.iSubItem=0;
		lvitem.iImage = (state) ? 1:0;
		lvitem.pszText=(LPWSTR)(LPCWSTR)str;
		m_listctrl_IO_Output.SetItem(&lvitem);
	}

	for(i = 0; i < MAX_INPUT_NUM; i++)
	{
		state = m_pAxis->GetInput(i);

		if(m_bInputState[i] == state)
			continue;

		m_bInputState[i] = state;

		m_listctrl_IO_Input.GetItem(&lvitem);
		str.Format(_T("X%03d"), i);

		lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem=i;
		lvitem.iSubItem=0;
		lvitem.iImage = (state) ? 1:0;
		lvitem.pszText=(LPWSTR)(LPCWSTR)str;
		m_listctrl_IO_Input.SetItem(&lvitem);
	}
	*/
}

BEGIN_EVENTSINK_MAP(CSetUpMotAxisZ, CDialog)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVE_JOG, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMoveJog, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVE_STEP, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMoveStep, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEUP, DISPID_CLICK, CSetUpMotAxisZ::AxisMove, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEDOWN, DISPID_CLICK, CSetUpMotAxisZ::AxisMove, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_HOME, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZHome, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_STOP, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZStop, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_SERVO_ON, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZServoOn, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_SERVO_OFF, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZServoOff, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_SETPOS1, DISPID_CLICK, CSetUpMotAxisZ::SetPosition, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_SETPOS2, DISPID_CLICK, CSetUpMotAxisZ::SetPosition, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_APPLY, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZApply, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEPOS1, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMovepos1, VTS_NONE)
ON_EVENT(CSetUpMotAxisZ, IDC_BTNENHCTRL_MOTION_AXIS_Z_MOVEPOS2, DISPID_CLICK, CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMovepos2, VTS_NONE)
END_EVENTSINK_MAP()


void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMoveJog()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_nRadioMoveType = AXIS_MOVE_TYPE_JOG;
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMoveStep()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioMoveType = AXIS_MOVE_TYPE_STEP;
}


void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZHome()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strMsg;


	//원점 설정
	if (AfxMessageBox(_T("Z축 원점작업을 실행합니까?"), MB_YESNO) == IDNO)
		return;

	if (!m_pAxis->GetAmpFaultState(AXIS_Z))
	{

		AfxMessageBox(_T("Z축 서보 상태를 점검하세요!"));
		return;
	}

	m_pAxis->SetAxisHomeFinished(AXIS_Z, FALSE);

	m_pAxis->SingleHome(AXIS_Z);

	//프로그레스 동작
	m_ctrl_progress.SetCaption(_T("원점 작업 진행 중.."));
	m_ctrl_progress.ShowWindow(SW_SHOW);

	SetTimer(TIMER_ORIGIN_DLG, 300, NULL);
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZStop()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pAxis->Stop(AXIS_Z);
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZServoOn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pAxis->ServoOn(AXIS_Z);
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZServoOff()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pAxis->ServoOff(AXIS_Z);
}


void CSetUpMotAxisZ::SetPosition()
{
	UpdateData(TRUE);
	
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	int nDialogID;
	nDialogID = pBtnEnh->GetDlgCtrlID();

	CString strPos;

	switch (nDialogID)
	{
	case IDC_BTNENHCTRL_MOTION_AXIS_Z_SETPOS1:
		strPos.Format(_T("%d"), m_pAxis->GetCurrentPos(AXIS_POS_0));
		m_CBtnAxisPos[AXIS_POS_0].SetCaption(strPos);
		break;
	case IDC_BTNENHCTRL_MOTION_AXIS_Z_SETPOS2:
		strPos.Format(_T("%d"), m_pAxis->GetCurrentPos(AXIS_POS_0));
		m_CBtnAxisPos[AXIS_POS_1].SetCaption(strPos);
		break;
	}
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	for(int i = 0; i < AXIS_POS_MAX_NUM; i++)
		//m_lPos[i] = _wtoi(m_CBtnAxisPos[i].GetCaption());
		m_lPos[i] = atoi(m_CBtnAxisPos[i].GetCaption());
}

void CSetUpMotAxisZ::UpdatePara(BOOL bstate)
{
	CString strPos;

	if(bstate)
	{
		for(int i = 0; i < AXIS_POS_MAX_NUM; i++)
		{
			strPos.Format(_T("%d"), m_lPos[i]);
			m_CBtnAxisPos[i].SetCaption(strPos);
		}
	}
	else
	{
		
	}
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMovepos1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pAxis->Absmove(AXIS_Z, m_lPos[AXIS_POS_0], (DWORD)(AXIS_BASE_VEL * m_dJogSpeed));
}

void CSetUpMotAxisZ::ClickBtnenhctrlMotionAxisZMovepos2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pAxis->Absmove(AXIS_Z, m_lPos[AXIS_POS_1], (DWORD)(AXIS_BASE_VEL * m_dJogSpeed));
}
