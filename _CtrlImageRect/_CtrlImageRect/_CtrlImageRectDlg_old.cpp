ï»?/ _CtrlImageRectDlg.cpp : êµ¬í˜„ ?Œì¼?…ë‹ˆ??
//

#include "stdafx.h"

#include "DialogMainView.h"

#include "_CtrlImageRect.h"
#include "afxdialogex.h"



//USB key ì½”ë“œ
unsigned char m_SafeNet_USB_vendor_code[] =
"6LCHxDCsAh5qK0OZUDIfbf4aLTYbXbz0bWitlU0/K24xtYoDMTgLEY6I+zwlqUqTW7gooSghysUj8YdV"
"NpgQUcDCBprT2tGa+DPInMWVq3aXeBcJMQyeumBk4JhXnB8WMWbc1QEF2q5S+WD7OtDpvLIk8EBEOChT"
"xRtyF+HbeEAcDFfJnmoBz95nzWI7GRwdLYHyIXXHqKPTczKzN3K03SmewwS/3JOyUa9dpfJIcwl362Sj"
"4IaCNQLAC/wycEGmJ4ADX/FlZA4cCNgHiK5Qllwl/0tl33J/L8wIP8VBh6xUk/bDckAOuB0X6f5ZQ4e/"
"4S6kyCIquM84+OmJHpjo670oH/kDGspjsQzhZR6YaMJuPGAhCaD11AcjW7gwgxnIvjwaL6oe3mVYpHbI"
"GKoKKbH1dWOmuwgEW61lv0a7DSrvaCHYQAYVOWIn8cZ8IeSyxvnCSr3jGFSnbN3bxWa7jDk/qFcCCma3"
"X+T4Ok49iQbrEMnatE/yWpcBEErGBGFSeMDoQbucdxyoZll1HucNJ/MdLzabon4qeyC9FcL7v+anKH+F"
"7pUy4XlFFFWVJ1itujg5HwqUK8zBgssGRPykGjxnclA1PrzgH8mrEaLyhuSWi0xejtJsIMdcovM1xjr6"
"TUSyyToTxCFA05+15CHGEfR5uaNDy8DTivtiv8Fg+UuGcW2aKTcRhzbsstwYUfcPl8tZ/hv8wP6J2I7y"
"qCO8GRfQTQU+L2jQfO2xVMn2gAm9n4zJTvRLXye2nP4+V7BhZrabOG75zwjJUf5E+u1xcqAlkxLea/40"
"ld2d68BkE8Cy1gdrlkh5t9TCS/EQYZNSQtA/jM5DnOfl1PycCGhOJqW2GbNjt4bTmmm5edbO60VWFRiP"
"kyWPhTW00uVQfaq+NVXSnxXAjzxCj+GnJEE4K0t80zB+KLG1ly0FotDfOaluef8t4xYBUr8VHoE6Vi9Q"
"zo/W0nFU91uV6k3yPr4nsA==";


// C_CtrlImageRect ?€???ìž?…ë‹ˆ??

IMPLEMENT_DYNAMIC(C_CtrlImageRect, CDialog)

C_CtrlImageRect::C_CtrlImageRect(CWnd* pParent /*=NULL*/)
	: CDialog(C_CtrlImageRect::IDD, pParent)
{
	m_iCamIndex = 0;
	
	m_lGrayX = 0;
	m_lGrayY = 0;
	m_iGray = 0;

	m_hnd = NULL;

	m_DlgMainView = NULL;
	m_lWindowID = 0;

	m_dZoomRatio = 20.;
#ifdef LOCK_KEY_USE
	if(!SafeNet_Lock_LogIn())
	{
		exit(0);
	}
#endif

	ZeroMemory(m_RectUI, sizeof(RECT)*C_CtrlImageRect_NUM_UI);
	ZeroMemory(m_bVisibleUI, sizeof(BOOL)*C_CtrlImageRect_NUM_UI);
}

C_CtrlImageRect::~C_CtrlImageRect()
{
#ifdef LOCK_KEY_USE
	SafeNet_Lock_LogOut();
#endif

	if (m_DlgMainView != NULL)
	{
		delete	m_DlgMainView;
		m_DlgMainView = NULL;
	}

	ZeroMemory(m_RectUI, sizeof(RECT)*C_CtrlImageRect_NUM_UI);
	ZeroMemory(m_bVisibleUI, sizeof(BOOL)*C_CtrlImageRect_NUM_UI);
}

void C_CtrlImageRect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_IMG_TITLE, m_Title);
	DDX_Control(pDX, IDC_BTNENHCTRL_COORD, m_Coord);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GRAY, m_Gray);
	DDX_Control(pDX, IDC_BTNENHCTRL_LIVE, m_btnLive);
	DDX_Control(pDX, IDC_BTNENHCTRL_GRAY, m_btnGray);
}

BEGIN_MESSAGE_MAP(C_CtrlImageRect, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_REGION_CONFIRM, &C_CtrlImageRect::OnRegionConfirm)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// C_CtrlImageRect ë©”ì‹œì§€ ì²˜ë¦¬ê¸°ìž…?ˆë‹¤.

void C_CtrlImageRect::SetActiveTracker(BOOL active)
{
	m_DlgMainView->SetActiveImageRect(active);
}

/*
void C_CtrlImageRect::SetActiveTracker(int idx, BOOL active)
{

}
*/

void C_CtrlImageRect::AddTracker(short type, long left, long top, long right, long bottom, long color, char *cap)
{
	CRect rect(left, top, right, bottom);
	//?¬ê°???ì—­ ì¶”ê?
	m_DlgMainView->AddTracker(type, rect, (COLORREF)color, cap);
}

short C_CtrlImageRect::GetNum()
{
	
	return m_DlgMainView->GetNum();
}

short C_CtrlImageRect::GetType(short idx)
{
	return m_DlgMainView->GetType(idx);
}

BOOL C_CtrlImageRect::SetTracker(short idx, long left, long top, long right, long bottom)
{
	CRect rect(left, top, right, bottom);
	//?¬ê°???ì—­ ?¤ì •(?ì„±???ˆì? ?Šìœ¼ë©?FALSE ë°˜í™˜)
	return m_DlgMainView->SetTracker(idx, rect);
}

BOOL C_CtrlImageRect::GetTracker(short idx, long *left, long *top, long *right, long *bottom)
{
	BOOL rslt = FALSE;
	CRect rect;

	//?¬ê°???ì—­ ?»ê¸°(?ì„±???ˆì? ?Šìœ¼ë©?FALSE ë°˜í™˜)
	rslt = m_DlgMainView->GetTracker(idx, &rect);
	if(rslt)
	{
		rect.NormalizeRect();
		*left = rect.left;
		*top = rect.top;
		*right = rect.right;
		*bottom = rect.bottom;
	}

	return rslt;
}

BOOL C_CtrlImageRect::DeleteTracker(short idx)
{
	//?ì—­ ?? œ(?ì„±???ˆì? ?Šìœ¼ë©?FALSE ë°˜í™˜)
	return m_DlgMainView->DeleteTracker(idx);
}

void C_CtrlImageRect::DeleteAll()
{
	//ëª¨ë“  ?ì—­ ?? œ
	m_DlgMainView->DeleteAll();
}

//Grid
void C_CtrlImageRect::SetActiveGrid(BOOL active)
{
	m_DlgMainView->SetActiveGrid(active);
}

BOOL C_CtrlImageRect::GetActiveGrid()
{
	return m_DlgMainView->GetActiveGrid();
}

void C_CtrlImageRect::SetGridPitch(double pitch)
{
	m_DlgMainView->SetGridPitch(pitch); 
}

double C_CtrlImageRect::GetGridPitch()
{
	return m_DlgMainView->GetGridPitch();
}	

//Gray
void C_CtrlImageRect::SetActiveGray(BOOL active)
{
	m_DlgMainView->SetActiveGray(active);
}

BOOL C_CtrlImageRect::GetActiveGray()
{
	return m_DlgMainView->GetActiveGray();
}

void C_CtrlImageRect::GetGrayPos(long *x, long *y)
{
	*x = m_lGrayX;
	*y = m_lGrayY;
}

void C_CtrlImageRect::SetGray(int gray)
{
	m_iGray = gray;
}

//Zoom
void C_CtrlImageRect::SetZoom(long zoom, long width, long height)
{
	Herror herr;
	double rat;
	double baseLength;

	if(m_lWindowID)
	{
		m_DlgMainView->GetClientRect(&m_DlgMainView->m_WinRect);

		if ( (double)m_DlgMainView->m_WinRect.Width() / (double)width > (double)m_DlgMainView->m_WinRect.Height() / (double)height )
		{
			rat = ((double)m_DlgMainView->m_WinRect.Height()) / ((double)height);
		}
		else
		{
			rat = ((double)m_DlgMainView->m_WinRect.Width()) / ((double)width);
		}

		set_check("~father");
		//Set window size
		herr = set_window_extents(m_lWindowID, (Hlong)0, (Hlong)0, (Hlong)m_DlgMainView->m_WinRect.Width(), (Hlong)m_DlgMainView->m_WinRect.Height());
//		herr = set_window_extents(m_lWindowID, (Hlong)0, (Hlong)0, (Hlong)(((double)width * rat)+0.5), (Hlong)(((double)height * rat)+0.5));
		set_check("father");

		m_DlgMainView->GetClientRect(&m_DlgMainView->m_WinRectOld);
		m_DlgMainView->SetZoom(zoom, width, height, rat);
	}
}

void C_CtrlImageRect::GetZoom(long *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y)
{
	m_DlgMainView->GetZoom(zoom, image_width, image_height, start_x, start_y, end_x, end_y);
}

//Display
void C_CtrlImageRect::Display()
{
	m_DlgMainView->Draw();
}	

//Set
void C_CtrlImageRect::SetCamIndex(int idx, CString title, HWND wnd)
{
	m_iCamIndex = idx;
	m_Title.SetCaption(title);
	m_Title.SetForeColor(RGB(255, 255, 255));
	m_Title.SetBackColor(RGB(64, 64, 64));
	m_hnd = wnd;

}

//Live
BOOL C_CtrlImageRect::GetActiveLive()
{
	return m_btnLive.GetValue();
}

void C_CtrlImageRect::SetActiveLive(BOOL active)
{

	m_btnLive.SetValue(active);
}

Hlong C_CtrlImageRect::GetView()
{
	return m_lWindowID;
}

void C_CtrlImageRect::Open()
{
	CRect rect;

	if(NULL == m_DlgMainView)
	{
		m_DlgMainView = new CDialogMainView;
		m_DlgMainView->Create(IDD_DIALOG_MAIN_VIEW, this);	
		m_DlgMainView->ShowWindow(SW_SHOW);
	}

	GetWindowRect(&rect);
	m_DlgMainView->MoveWindow(0, 60, rect.Width(), rect.Height() - 60);

	if(NULL == m_lWindowID)
	{
		m_DlgMainView->GetClientRect(&m_DlgMainView->m_WinRect);

		//Set display configuration
		set_window_attr("border_width",0);

		set_check("~father");
		open_window(0, 0, m_DlgMainView->m_WinRect.Width(), m_DlgMainView->m_WinRect.Height(), (long)m_DlgMainView->m_hWnd, "transparent", "", &m_lWindowID);
		m_DlgMainView->SetView(m_lWindowID);
		set_check("father");
	}
}

void C_CtrlImageRect::Invalidate()
{
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 0);
}

void C_CtrlImageRect::PixelGrayValue(long x, long y, short gray)
{
	CString temp;

	m_lGrayX = x;
	m_lGrayY = y;

	temp.Format(_T("(%d, %d)"), m_lGrayX, m_lGrayY);
	m_Coord.SetCaption(temp);

	temp.Format(_T("%d"), m_iGray);
	m_Gray.SetCaption(temp);

	if(0 > gray)
	{
		SetActiveGray(FALSE);
		m_btnGray.SetValue(FALSE);
	}
	
	//GetParent()->PostMessage(USER_MSG_IMG_RECT, m_iCamIndex, 5);
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 5);
}

void C_CtrlImageRect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	if(bShow)
	{
		//CRect rect;

		
		//GetClientRect(&rect);
		//GetDlgItem(IDC_IMAGERECTCTRL_VIEW)->MoveWindow(rect.left, rect.top + 60, rect.right, rect.bottom - 60);

		Open();
		RepositionUI();
	}
}

void C_CtrlImageRect::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	// TODO: Add your message handler code here
	if(nState)
	{
		m_Title.SetBackColor(0x00C0C0C0);
		//::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 9);
	}
	else
	{
		m_Title.SetBackColor(0x00534846);
	}
}

void C_CtrlImageRect::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	// ê·¸ë¦¬ê¸?ë©”ì‹œì§€???€?´ì„œ??CDialog::OnPaint()??ë¥? ?¸ì¶œ?˜ì? ë§ˆì‹­?œì˜¤.
	CRect rect;
	CPen pen,*oldPen;
	CBrush brush,*oldBrush;
	
	GetClientRect(&rect);
	
	//ë¹ˆê³µê°?ì±„ìš°ê¸?
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen=dc.SelectObject(&pen);
	brush.CreateSolidBrush(RGB(0, 0, 0));
	oldBrush=dc.SelectObject(&brush);
	
	rect.bottom = 60;
	dc.Rectangle(&rect);
	
	dc.SelectObject(oldPen);
	pen.DeleteObject();
	dc.SelectObject(oldBrush);
	brush.DeleteObject();
	
}

BEGIN_EVENTSINK_MAP(C_CtrlImageRect, CDialog)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_LIVE, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlLive, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_IMG_OPEN, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlImgOpen, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_IMG_SAVE, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlImgSave, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_ZOOMIN, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlZoomin, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_ZOOMOUT, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlZoomout, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_ZOOM11, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlZoom11, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_ZOOMFIT, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlZoomfit, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_GRAY, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlGray, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_GRID, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlGrid, VTS_NONE)
	ON_EVENT(C_CtrlImageRect, IDC_BTNENHCTRL_GENERAL, DISPID_CLICK, C_CtrlImageRect::ClickBtnenhctrlGeneral, VTS_NONE)
END_EVENTSINK_MAP()

void C_CtrlImageRect::ClickBtnenhctrlLive()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	if(m_btnLive.GetValue())
	{
		::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 1);
	}
	else
	{
		::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 2);
	}
}

void C_CtrlImageRect::ClickBtnenhctrlImgOpen()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 3);
}

void C_CtrlImageRect::ClickBtnenhctrlImgSave()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 4);
}

void C_CtrlImageRect::ClickBtnenhctrlZoomin()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	long zoom, img_w, img_h, s_x, s_y, e_x, e_y;

	GetZoom(&zoom, &img_w, &img_h, &s_x, &s_y, &e_x, &e_y);
	zoom = zoom + m_dZoomRatio;
	SetZoom(zoom, img_w, img_h);
	
	//GetParent()->PostMessage(USER_MSG_IMG_RECT, m_iCamIndex, 0);
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 0);

}

void C_CtrlImageRect::SetZoomRatio(double ratio)
{
	m_dZoomRatio = ratio;
	if(1. >= m_dZoomRatio)
	{
		m_dZoomRatio = 2.;
	}
}

void C_CtrlImageRect::ClickBtnenhctrlZoomout()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	long zoom, img_w, img_h, s_x, s_y, e_x, e_y;

	GetZoom(&zoom, &img_w, &img_h, &s_x, &s_y, &e_x, &e_y);
	zoom = zoom - m_dZoomRatio;
	if(0 > zoom)
	{
		zoom = 0;
	}
	SetZoom(zoom, img_w, img_h);
	
	//GetParent()->PostMessage(USER_MSG_IMG_RECT, m_iCamIndex, 0);	
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 0);
}

void C_CtrlImageRect::ClickBtnenhctrlZoom11()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	long zoom, img_w, img_h, s_x, s_y, e_x, e_y;

	GetZoom(&zoom, &img_w, &img_h, &s_x, &s_y, &e_x, &e_y);
	zoom = 1;
	SetZoom(zoom, img_w, img_h);
	
	//GetParent()->PostMessage(USER_MSG_IMG_RECT, m_iCamIndex, 0);
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 0);
}

void C_CtrlImageRect::ClickBtnenhctrlZoomfit()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	long zoom, img_w, img_h, s_x, s_y, e_x, e_y;

	GetZoom(&zoom, &img_w, &img_h, &s_x, &s_y, &e_x, &e_y);
	zoom = 0;
	SetZoom(zoom, img_w, img_h);
	
	//GetParent()->PostMessage(USER_MSG_IMG_RECT, m_iCamIndex, 0);
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 0);
}

void C_CtrlImageRect::ClickBtnenhctrlGray()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	if(m_btnGray.GetValue())
	{
		SetActiveGray(TRUE);
		//m_btnGray.SetValue(TRUE);
	}
	else
	{
		SetActiveGray(FALSE);
		//m_btnGray.SetValue(FALSE);
	}
}

//20160309 ngh
void C_CtrlImageRect::SetDragDrow(BOOL isDrag)
{
	m_DlgMainView->m_bDragSize = isDrag;
}

BOOL C_CtrlImageRect::GetDragDrow()
{
	return m_DlgMainView->m_bDragSize;
}

void C_CtrlImageRect::ClickBtnenhctrlGrid()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 8);
}

void C_CtrlImageRect::ClickBtnenhctrlGeneral()
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	//General purpose
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 7);
}

void C_CtrlImageRect::OnRegionConfirm()
{
	// TODO: ?¬ê¸°??ëª…ë ¹ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	::PostMessage(m_hnd, USER_MSG_IMG_RECT, m_iCamIndex, 6);
}

void C_CtrlImageRect::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??
	CMenu menu;
	
	if (menu.LoadMenu(IDR_MENU_IMAGERECT))
	{
		CMenu* psub = menu.GetSubMenu(0);

		psub->EnableMenuItem(ID_REGION_CONFIRM, MF_BYCOMMAND);
	
		if (psub)
		{
			psub->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	}	
}


void C_CtrlImageRect::SetPart(long sx, long sy, long ex, long ey)
{
	set_part(m_lWindowID, sy, sx, ey, ex);
}

BOOL C_CtrlImageRect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ?¬ê¸°??ì¶”ê? ì´ˆê¸°???‘ì—…??ì¶”ê??©ë‹ˆ??
	Open();

	int	UI_ID, MoveX, PivotX, PivotY;
	for (int i = 0; i < C_CtrlImageRect_NUM_UI; i++)
	{
		switch (i)
		{
			case	C_CtrlImageRect_UI_LIVE		:	UI_ID = IDC_BTNENHCTRL_LIVE			;	break	;
			case	C_CtrlImageRect_UI_LOADIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_OPEN		;	break	;
			case	C_CtrlImageRect_UI_SAVEIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_SAVE		;	break	;
			case	C_CtrlImageRect_UI_ZOOMIN	:	UI_ID = IDC_BTNENHCTRL_ZOOMIN		;	break	;
			case	C_CtrlImageRect_UI_ZOOMOUT	:	UI_ID = IDC_BTNENHCTRL_ZOOMOUT		;	break	;
			case	C_CtrlImageRect_UI_ZOOMORG	:	UI_ID = IDC_BTNENHCTRL_ZOOM11		;	break	;
			case	C_CtrlImageRect_UI_ZOOMFIT	:	UI_ID = IDC_BTNENHCTRL_ZOOMFIT		;	break	;
			case	C_CtrlImageRect_UI_COORD	:	UI_ID = IDC_BTNENHCTRL_COORD		;	break	;
			case	C_CtrlImageRect_UI_LABEL	:	UI_ID = IDC_BTNENHCTRL_LABEL_GRAY	;	break	;
			case	C_CtrlImageRect_UI_GRAY		:	UI_ID = IDC_BTNENHCTRL_GRAY			;	break	;
			case	C_CtrlImageRect_UI_GRID		:	UI_ID = IDC_BTNENHCTRL_GRID			;	break	;
			case	C_CtrlImageRect_UI_GENERAL	:	UI_ID = IDC_BTNENHCTRL_GENERAL		;	break	;
			default								:	continue							;	break	;
		}
		m_bVisibleUI[i] = TRUE;

		GetDlgItem(UI_ID)->GetWindowRect(&m_RectUI[i]);
		if (i > 0)
		{
			MoveX = m_RectUI[i].left - (m_RectUI[i - 1].left + (m_RectUI[i - 1].right - m_RectUI[i - 1].left) + 5);	//	5?½ì? ê°„ê²©?¼ë¡œ ?¬ë°°ì¹?
			m_RectUI[i].left -= MoveX;
			m_RectUI[i].right -= MoveX;
		}
		else
		{
			PivotX = 5 - m_RectUI[i].left;	//	?œìž‘ X ì¢Œí‘œ 5ë¡??‹íŒ…
			PivotY = 28 - m_RectUI[i].top;	//	?œìž‘ Y ì¢Œí‘œ 28ë¡??‹íŒ…
		}

		m_RectUI[i].left += PivotX;
		m_RectUI[i].right += PivotX;
		m_RectUI[i].top += PivotY;
		m_RectUI[i].bottom += PivotY;

		GetDlgItem(UI_ID)->MoveWindow(&m_RectUI[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ?ˆì™¸: OCX ?ì„± ?˜ì´ì§€??FALSEë¥?ë°˜í™˜?´ì•¼ ?©ë‹ˆ??
}

BOOL C_CtrlImageRect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ?¬ê¸°???¹ìˆ˜?”ëœ ì½”ë“œë¥?ì¶”ê? ë°??ëŠ” ê¸°ë³¸ ?´ëž˜?¤ë? ?¸ì¶œ?©ë‹ˆ??
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;

		if(pMsg->wParam == VK_CONTROL)
			m_DlgMainView->SetCtrlClick(TRUE);
	}

	if(pMsg->message ==WM_KEYUP)
	{
		m_DlgMainView->SetCtrlClick(FALSE);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

#ifdef LOCK_KEY_USE
//20140630 ngh SafeNet USB LockKey login
BOOL C_CtrlImageRect::SafeNet_Lock_LogIn()
{
	m_SafeNet_USB_feature = HASP_DEFAULT_FID;
	m_SafeNet_USB_handle = HASP_INVALID_HANDLE_VALUE;

	m_SafeNet_USB_status = hasp_login(m_SafeNet_USB_feature, m_SafeNet_USB_vendor_code, &m_SafeNet_USB_handle);

	if (m_SafeNet_USB_status != HASP_STATUS_OK)
	{
		AfxMessageBox(_T("USB LockKeyë¥??•ì¸?´ì£¼?¸ìš”"));
		return FALSE;
	}
	return TRUE;

}

//20140630 ngh SafeNet USB LockKey logout
void C_CtrlImageRect::SafeNet_Lock_LogOut()
{
	m_SafeNet_USB_status = hasp_logout(m_SafeNet_USB_handle);

	if (m_SafeNet_USB_status != HASP_STATUS_OK)
	{
		switch (m_SafeNet_USB_status)
		{
			case HASP_INV_HND:
				break;
			default:
				break;
		}
	}
}
#endif
void C_CtrlImageRect::SetPixelSize(double WpixelSize, double HpixelSize)
{
	m_dPixelSize[0] = WpixelSize;
	m_dPixelSize[1] = HpixelSize;
}

double *C_CtrlImageRect::GetPixelSize()
{
	return m_dPixelSize;
}

void C_CtrlImageRect::SetPos(LPRECT pCenterArea)
{
	m_DlgMainView->SetPos(pCenterArea);
}

BOOL C_CtrlImageRect::Create(CWnd* pParentWnd)
{
	// TODO: ?¬ê¸°???¹ìˆ˜?”ëœ ì½”ë“œë¥?ì¶”ê? ë°??ëŠ” ê¸°ë³¸ ?´ëž˜?¤ë? ?¸ì¶œ?©ë‹ˆ??
	return CDialog::Create(C_CtrlImageRect::IDD, pParentWnd);
}

void C_CtrlImageRect::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint /*TRUE*/)
{
	CDialog::MoveWindow(x, y, nWidth, nHeight, bRepaint);

	if (m_DlgMainView != NULL)
	{
		CRect rect;

		GetWindowRect(&rect);
		m_DlgMainView->MoveWindow(0, 60, rect.Width(), rect.Height() - 60);
	}
}

void C_CtrlImageRect::MoveWindow(LPCRECT lpRect, BOOL bRepaint /*TRUE*/)
{
	if (lpRect == NULL)
		return;

	MoveWindow(lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

void C_CtrlImageRect::EnableUI(C_CtrlImageRect_UI UI, BOOL bEnable/*TRUE*/)
{
	int	UI_ID;
	switch (UI)
	{
		case	C_CtrlImageRect_UI_LIVE		:	UI_ID = IDC_BTNENHCTRL_LIVE			;	break	;
		case	C_CtrlImageRect_UI_LOADIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_OPEN		;	break	;
		case	C_CtrlImageRect_UI_SAVEIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_SAVE		;	break	;
		case	C_CtrlImageRect_UI_ZOOMIN	:	UI_ID = IDC_BTNENHCTRL_ZOOMIN		;	break	;
		case	C_CtrlImageRect_UI_ZOOMOUT	:	UI_ID = IDC_BTNENHCTRL_ZOOMOUT		;	break	;
		case	C_CtrlImageRect_UI_ZOOMORG	:	UI_ID = IDC_BTNENHCTRL_ZOOM11		;	break	;
		case	C_CtrlImageRect_UI_ZOOMFIT	:	UI_ID = IDC_BTNENHCTRL_ZOOMFIT		;	break	;
		case	C_CtrlImageRect_UI_COORD	:	UI_ID = IDC_BTNENHCTRL_COORD		;	break	;
		case	C_CtrlImageRect_UI_LABEL	:	UI_ID = IDC_BTNENHCTRL_LABEL_GRAY	;	break	;
		case	C_CtrlImageRect_UI_GRAY		:	UI_ID = IDC_BTNENHCTRL_GRAY			;	break	;
		case	C_CtrlImageRect_UI_GRID		:	UI_ID = IDC_BTNENHCTRL_GRID			;	break	;
		case	C_CtrlImageRect_UI_GENERAL	:	UI_ID = IDC_BTNENHCTRL_GENERAL		;	break	;
	}
	GetDlgItem(UI_ID)->EnableWindow(bEnable);
}

void C_CtrlImageRect::ShowUI(C_CtrlImageRect_UI UI, BOOL bShow/*TRUE*/)
{
	int	UI_ID;
	switch (UI)
	{
		case	C_CtrlImageRect_UI_LIVE		:	UI_ID = IDC_BTNENHCTRL_LIVE			;	break	;
		case	C_CtrlImageRect_UI_LOADIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_OPEN		;	break	;
		case	C_CtrlImageRect_UI_SAVEIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_SAVE		;	break	;
		case	C_CtrlImageRect_UI_ZOOMIN	:	UI_ID = IDC_BTNENHCTRL_ZOOMIN		;	break	;
		case	C_CtrlImageRect_UI_ZOOMOUT	:	UI_ID = IDC_BTNENHCTRL_ZOOMOUT		;	break	;
		case	C_CtrlImageRect_UI_ZOOMORG	:	UI_ID = IDC_BTNENHCTRL_ZOOM11		;	break	;
		case	C_CtrlImageRect_UI_ZOOMFIT	:	UI_ID = IDC_BTNENHCTRL_ZOOMFIT		;	break	;
		case	C_CtrlImageRect_UI_COORD	:	UI_ID = IDC_BTNENHCTRL_COORD		;	break	;
		case	C_CtrlImageRect_UI_LABEL	:	UI_ID = IDC_BTNENHCTRL_LABEL_GRAY	;	break	;
		case	C_CtrlImageRect_UI_GRAY		:	UI_ID = IDC_BTNENHCTRL_GRAY			;	break	;
		case	C_CtrlImageRect_UI_GRID		:	UI_ID = IDC_BTNENHCTRL_GRID			;	break	;
		case	C_CtrlImageRect_UI_GENERAL	:	UI_ID = IDC_BTNENHCTRL_GENERAL		;	break	;
	}
	if (bShow)
	{
		GetDlgItem(UI_ID)->ShowWindow(SW_SHOW);
		m_bVisibleUI[UI] = TRUE;
	}
	else
	{
		GetDlgItem(UI_ID)->ShowWindow(SW_HIDE);
		m_bVisibleUI[UI] = FALSE;
	}

	RepositionUI();
}

void C_CtrlImageRect::RepositionUI()
{
	int		UI_ID;
	RECT	Pos;

	Pos.left = m_RectUI[0].left;
	for (int i = 0; i < C_CtrlImageRect_NUM_UI; i++)
	{
		switch (i)
		{
			case	C_CtrlImageRect_UI_LIVE		:	UI_ID = IDC_BTNENHCTRL_LIVE			;	break	;
			case	C_CtrlImageRect_UI_LOADIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_OPEN		;	break	;
			case	C_CtrlImageRect_UI_SAVEIMG	:	UI_ID = IDC_BTNENHCTRL_IMG_SAVE		;	break	;
			case	C_CtrlImageRect_UI_ZOOMIN	:	UI_ID = IDC_BTNENHCTRL_ZOOMIN		;	break	;
			case	C_CtrlImageRect_UI_ZOOMOUT	:	UI_ID = IDC_BTNENHCTRL_ZOOMOUT		;	break	;
			case	C_CtrlImageRect_UI_ZOOMORG	:	UI_ID = IDC_BTNENHCTRL_ZOOM11		;	break	;
			case	C_CtrlImageRect_UI_ZOOMFIT	:	UI_ID = IDC_BTNENHCTRL_ZOOMFIT		;	break	;
			case	C_CtrlImageRect_UI_COORD	:	UI_ID = IDC_BTNENHCTRL_COORD		;	break	;
			case	C_CtrlImageRect_UI_LABEL	:	UI_ID = IDC_BTNENHCTRL_LABEL_GRAY	;	break	;
			case	C_CtrlImageRect_UI_GRAY		:	UI_ID = IDC_BTNENHCTRL_GRAY			;	break	;
			case	C_CtrlImageRect_UI_GRID		:	UI_ID = IDC_BTNENHCTRL_GRID			;	break	;
			case	C_CtrlImageRect_UI_GENERAL	:	UI_ID = IDC_BTNENHCTRL_GENERAL		;	break	;
			default								:	continue							;	break	;
		}

		if (m_bVisibleUI[i])
		{
			Pos.right = Pos.left + (m_RectUI[i].right - m_RectUI[i].left);
			Pos.top = m_RectUI[i].top;
			Pos.bottom = m_RectUI[i].bottom;
			GetDlgItem(UI_ID)->MoveWindow(&Pos);
			Pos.left = Pos.right + 5;
		}
	}
}