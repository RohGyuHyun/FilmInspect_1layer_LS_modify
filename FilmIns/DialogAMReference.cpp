// DialogAMReference.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "DialogAMReference.h"


// CDialogAMReference 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogAMReference, CDialog)

CDialogAMReference::CDialogAMReference(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAMReference::IDD, pParent)
{

}

CDialogAMReference::~CDialogAMReference()
{
}

void CDialogAMReference::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogAMReference, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDialogAMReference 메시지 처리기입니다.
void CDialogAMReference::ResizeView()
{
	
	CRect dlg_rect, static_rect;
	Herror hrr;

	//VIEW #1
	GetWindowRect(&dlg_rect);
	ScreenToClient(&dlg_rect);

	GetDlgItem(IDC_STATIC_REFERENCE_VIEW)->MoveWindow(dlg_rect.left, dlg_rect.top, dlg_rect.Width(), dlg_rect.Width());
	GetDlgItem(IDC_STATIC_REFERENCE_VIEW)->GetClientRect(&static_rect);

	set_check("~father");
	hrr = open_window(0, 0, static_rect.Width(), static_rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_REFERENCE_VIEW)->m_hWnd, "transparent", "", &m_hWindow[0]);
	set_check("father");

	//VIEW #2
	GetDlgItem(IDC_STATIC_REFERENCE_VIEW2)->MoveWindow( static_rect.left, static_rect.bottom + VIEW_AREA_OFFSET, static_rect.Width(), static_rect.Height());
	GetDlgItem(IDC_STATIC_REFERENCE_VIEW2)->GetClientRect(&static_rect);

	set_check("~father");
	hrr = open_window(0, 0, static_rect.Width(), static_rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_REFERENCE_VIEW2)->m_hWnd, "transparent", "", &m_hWindow[1]);
	set_check("father");
}

void CDialogAMReference::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}

BOOL CDialogAMReference::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	::SetWindowPos(GetSafeHwnd(),  HWND_TOPMOST , 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogAMReference::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	CPen pen,*oldPen;
	CBrush brush,*oldBrush;

	GetClientRect(&rect);

	//빈공간 채우기
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


void CDialogAMReference::DisplayReferenceImage(int idx, CString path)
{
	//file 유/무
	CRect rect;
	Hlong width, height;
	CFileFind find;
	CString full_path;
	//full_path.Format("%s\\AM_Reference_%d.bmp", path, idx + 1);	//20190909 by 주석처리
	full_path.Format("%s\\AM_Reference_%d.jpg", path, idx + 1);
	
	clear_window(m_hWindow[idx]);
	GetDlgItem(IDC_STATIC_REFERENCE_VIEW)->GetClientRect(&rect);
	
	if( find.FindFile(full_path) )
	{	
		//set font
		set_color(m_hWindow[idx], "green");
		read_image(&m_objImage[idx], full_path);

		//get_image_pointer1(m_objImage[idx], NULL, NULL, &width, &height);
		get_image_size(m_objImage[idx], &width, &height);
		set_part(m_hWindow[idx], 0, 0, height, width);
		disp_obj(m_objImage[idx], m_hWindow[idx]);		
	}
	else
	{
		set_part(m_hWindow[idx], 0, 0, rect.Height(), rect.Width());
		set_color(m_hWindow[idx], "red");
		set_tposition(m_hWindow[idx], rect.Height() / 2 , rect.Width()/3);
		write_string(m_hWindow[idx], "No Image !!");
	}

	//line TOP
	disp_line(m_hWindow[idx], rect.top, rect.left, rect.top, rect.right);
	//line RIGHT
	disp_line(m_hWindow[idx], rect.top, rect.right, rect.bottom-1, rect.right-1);
	//line BOTTOM
	disp_line(m_hWindow[idx], rect.bottom, rect.left, rect.bottom-1, rect.right-1);
	//line LEFT
	disp_line(m_hWindow[idx], rect.top, rect.left, rect.bottom, rect.left);
}

void CDialogAMReference::SaveReferenceImage(int idx, CRect rect, Hobject img, CString path, int data_1, int data_2)
{
	CString full_path, Reference_Info;
	m_objImage[idx] = img;
	CRect _Rect;
	Hlong width, height;

	GetDlgItem(IDC_STATIC_REFERENCE_VIEW2)->GetClientRect(&_Rect);
	

	crop_rectangle1(m_objImage[idx], &m_objImage[idx], rect.top, rect.left, rect.bottom, rect.right);
	get_image_size(m_objImage[idx], &width, &height);
	set_part(m_hWindow[idx], 0, 0, height, width);

	//full_path.Format("%s\\AM_Reference_%d.bmp", path, idx + 1);	//20190909 by 주석처리 이하 코드 동일
	full_path.Format("%s\\AM_Reference_%d.jpg", path, idx + 1);
	disp_obj(m_objImage[idx],m_hWindow[idx]);
	
	
	set_color(m_hWindow[idx],"green");
	set_tposition(m_hWindow[idx], 0, 0);
	Reference_Info.Format("Area : %d  /  Mean : %d", data_1, data_2);
	write_string(m_hWindow[idx],(HTuple)Reference_Info);
	
	//write_image(m_objImage[idx], "bmp", 0, full_path);
	//dump_window(m_hWindow[idx],"bmp", (HTuple)full_path);
	dump_window(m_hWindow[idx],"jpg", (HTuple)full_path);
}