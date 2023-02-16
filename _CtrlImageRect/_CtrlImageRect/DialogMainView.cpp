// DialogMainView.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "DialogMainView.h"
#include "_CtrlImageRect.h"

#include "afxdialogex.h"


// CDialogMainView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogMainView, CDialogEx)

CDialogMainView::CDialogMainView(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMainView::IDD, pParent)
{
	m_bActive = FALSE;
	m_iActiveIdx = -1;
	m_curColor = RGB(0, 255, 0);

	//bjs
	m_dblWndImgRate = 1.0;

	//Zoom
	m_dblZoomFact = 100.0;
	m_lImageWidth = 640;
	m_lImageHeight = 480;
	m_lZoomStartX = 0;
	m_lZoomStartY = 0;
	m_lZoomEndX = 0;
	m_lZoomEndY = 0;

	//Grid
	m_bActiveGrid = FALSE;
	m_dGridPitch = 0;

	//Gray
	m_bActiveGray = FALSE;
	m_lPixelX = 0;
	m_lPixelY = 0;
	m_sPixelGray = 0;

	m_bZoom1 = TRUE;
	m_bDragSize = FALSE;
	m_bMouseMove = FALSE;
}

CDialogMainView::~CDialogMainView()
{
	m_Tracker.RemoveAll();
}

void CDialogMainView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMainView, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CDialogMainView 메시지 처리기입니다.

void CDialogMainView::Draw()
{
	int i, tracker_num;
	C_CtrlTracker* tracker;

	//검사용
	if(m_bActive)
	{
		tracker_num = m_Tracker.GetCount();
		for(i = 0; (i < tracker_num); i++)
		{
			POSITION pos = m_Tracker.FindIndex(i);
			if(NULL != pos)
			{
				tracker = &m_Tracker.GetAt(pos);
				switch(tracker->GetType())
				{
					case 0:
					case 1:
						tracker->Draw(i);
						break;
					default:
						AfxMessageBox(_T("Error : Invalid index!"));
						break;
				}
			}
		}
	}

	CRect rect;
	CDC *pDC;
	pDC=GetDC();

	CPen pen,*oldPen;
	CBrush brush,*oldBrush;

	GetClientRect(&rect);

	//빈공간 채우기
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen=pDC->SelectObject(&pen);
	brush.CreateSolidBrush(RGB(0, 0, 0));
	oldBrush=pDC->SelectObject(&brush);
	
	rect.left = m_WinRect.right;
	rect.top = m_WinRect.top;

	pDC->Rectangle(&rect);
	
	pDC->SelectObject(oldPen);
	pen.DeleteObject();
	pDC->SelectObject(oldBrush);
	brush.DeleteObject();

	ReleaseDC(pDC);
}


//사각형 영역 추가
void CDialogMainView::AddTracker(int type, CRect rect, COLORREF color, char *cap)
{

	C_CtrlTracker tracker;
	int idx;
	double rect_l, rect_t, rect_r, rect_b;

	if(m_bActive)
	{
		m_curColor = color;
		switch(type)
		{
			case 0:
			case 1:
				tracker = C_CtrlTracker(this, type, 4, 2);
				tracker.SetColor(m_curColor);
				
				rect_l = rect.left;
				rect_t = rect.top;
				rect_r = rect.right;
				rect_b = rect.bottom;

//				rect_l = rect.left / ((double)m_WinRect.Width() / (m_lZoomEndX - m_lZoomStartX));
//				rect_t = rect.top / ((double)m_WinRect.Height() / (m_lZoomEndY - m_lZoomStartY));
//				rect_r = rect.right / ((double)m_WinRect.Width() / (m_lZoomEndX - m_lZoomStartX));
//				rect_b = rect.bottom / ((double)m_WinRect.Height() / (m_lZoomEndY - m_lZoomStartY));

				rect_l = (rect_l - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_t = (rect_t - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				rect_r = (rect_r - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_b = (rect_b - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				tracker.SetRect(rect_l, rect_t, rect_r, rect_b);
				tracker.SetZoom(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);
				tracker.SetCaption(cap);
				idx = m_Tracker.GetCount();
				m_Tracker.AddTail(tracker);
				break;
			default:
				AfxMessageBox(_T("Error : invalid index!"));
				break;
		}

	}

}


//모든 영역 삭제
void CDialogMainView::DeleteAll()
{
	if(m_bActive && ( !m_Tracker.IsEmpty() ) )
	{
		m_Tracker.RemoveAll();
		m_iActiveIdx = -1;
	}
}

//영역 삭제(생성해 있지 않으면 FALSE 반환)
BOOL CDialogMainView::DeleteTracker(int idx)
{
	C_CtrlTracker tracker;

	if(m_bActive && ( !m_Tracker.IsEmpty()) )
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			m_Tracker.RemoveAt(pos);
			m_iActiveIdx = -1;

			return TRUE;
		}
	}

	return FALSE;
}

//사각형 영역 얻기(생성해 있지 않으면 FALSE 반환)
BOOL CDialogMainView::GetTracker(int idx, CRect *rect)
{
	C_CtrlTracker tracker;
	double rect_l, rect_t, rect_r, rect_b;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			switch(tracker.GetType())
			{
				case 0:
				case 1:
					tracker.GetRect(&rect_l, &rect_t, &rect_r, &rect_b);

					/*
					rect_l = (rect_l - m_lZoomStartX) / ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
					rect_t = (rect_t - m_lZoomStartY) / ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

					rect_r = (rect_r - m_lZoomStartX) / ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
					rect_b = (rect_b - m_lZoomStartY) / ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));
					*/

					//2090924
					rect_l = m_lZoomStartX + ( ((double)rect_l)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
					rect_t = m_lZoomStartY + ( ((double)rect_t)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));
					rect_r = m_lZoomStartX + ( ((double)rect_r)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
					rect_b = m_lZoomStartY + ( ((double)rect_b)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));


					rect_l = rect_l + 0.5;
					rect_t = rect_t + 0.5;
					rect_r = rect_r + 0.5;
					rect_b = rect_b + 0.5;

					*rect = CRect(rect_l, rect_t, rect_r, rect_b);
					break;
				default:
					AfxMessageBox(_T("Error : invalid index!"));
					break;
			}
			
			return TRUE;
		}
	}

	return FALSE;
}


//사각형 영역 설정(생성해 있지 않으면 FALSE 반환)
BOOL CDialogMainView::SetTracker(int idx, CRect rect)
{
	C_CtrlTracker tracker;

	double rect_l, rect_t, rect_r, rect_b;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			switch(tracker.GetType())
			{
				case 0:
				case 1:
				rect_l = rect.left;
				rect_t = rect.top;
				rect_r = rect.right;
				rect_b = rect.bottom;

				rect_l = (rect_l - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_t = (rect_t - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				rect_r = (rect_r - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_b = (rect_b - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				tracker.SetRect(rect_l, rect_t, rect_r, rect_b);
				tracker.SetZoom(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);

				idx = m_Tracker.GetCount();
				m_Tracker.AddTail(tracker);
					break;
				default:
					AfxMessageBox(_T("Error : invalid index!"));
					break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

//Tracker 개수 얻기
int CDialogMainView::GetNum()
{
	return m_Tracker.GetCount();
}

//Tracker 종류 얻기
int CDialogMainView::GetType(int idx)
{
	C_CtrlTracker tracker;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			return tracker.GetType();
		}
	}
	
	return -1;
}

//현재 트랙커 화면 활성화 여부
void CDialogMainView::SetActiveImageRect(BOOL active)
{
	m_bActive = active;
}

void CDialogMainView::SetActiveImageRect(int idx, BOOL active)
{
	C_CtrlTracker tracker;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			tracker.SetActive(active);
		}
	}
}

void CDialogMainView::SetZoom(double zoom, long image_width, long image_height, BOOL bIsFitToLongSide/*TRUE*/)
{
	// TODO: Add your dispatch handler code here
	CRect rect;
	double zoom_rat;
	double	LongSideFitZoom, ShortSideFitZoom;
	long zoom_part_x, zoom_part_y;
	long zoom_diff;
	long s_x, s_y, e_x, e_y;
	long ScrollRangeX, ScrollRangeY;
	s_x = m_lZoomStartX;
	s_y = m_lZoomStartY;
	e_x = m_lZoomEndX;
	e_y = m_lZoomEndY;

	m_lImageWidth = image_width;
	m_lImageHeight = image_height;

	//외부 지정(ExWndZoomRate) 비율대로 화면에 출력
	if ((double)m_WinRect.Width() / (double)image_width < (double)m_WinRect.Height() / (double)image_height)
	{
		LongSideFitZoom = ((double)m_WinRect.Width() / (double)image_width);
		ShortSideFitZoom = ((double)m_WinRect.Height() / (double)image_height);
	}
	else
	{
		LongSideFitZoom = ((double)m_WinRect.Height() / (double)image_height);
		ShortSideFitZoom = ((double)m_WinRect.Width() / (double)image_width);
	}

	if (0 >= zoom)
	{
		if (bIsFitToLongSide)
			zoom = LongSideFitZoom;
		else
			zoom = ShortSideFitZoom;
	}
	else
	{
		if (zoom < LongSideFitZoom)
			zoom = LongSideFitZoom;
	}

	//Zoom
	//ShowScrollBar(SB_BOTH, TRUE);
	zoom_part_x = (long)(m_WinRect.Width() / zoom + 0.5);
	zoom_part_y = (long)(m_WinRect.Height() / zoom + 0.5);

	if (zoom_part_x > m_lImageWidth)
	{
		m_lZoomStartX = -((zoom_part_x - m_lImageWidth) / 2);
		m_lZoomEndX = m_lZoomStartX + zoom_part_x;
	}
	else
	{
		m_lZoomStartX = ((m_lZoomEndX + m_lZoomStartX) / 2) - (zoom_part_x / 2);
		if (m_lZoomStartX < 0)
			m_lZoomStartX = 0;
		m_lZoomEndX = m_lZoomStartX + zoom_part_x;
		if (m_lZoomEndX > m_lImageWidth)
		{
			m_lZoomStartX = m_lImageWidth - zoom_part_x;
			m_lZoomEndX = m_lImageWidth;
		}
	}

	if (zoom_part_y > m_lImageHeight)
	{
		m_lZoomStartY = -((zoom_part_y - m_lImageHeight) / 2);
		m_lZoomEndY = m_lZoomStartY + zoom_part_y;
	}
	else
	{
		m_lZoomStartY = ((m_lZoomEndY + m_lZoomStartY) / 2) - (zoom_part_y / 2);
		if (m_lZoomStartY < 0)
			m_lZoomStartY = 0;
		m_lZoomEndY = m_lZoomStartY + zoom_part_y;
		if (m_lZoomEndY > m_lImageHeight)
		{
			m_lZoomStartY = m_lImageHeight - zoom_part_y;
			m_lZoomEndY = m_lImageHeight;
		}
	}

	//이미지 최상하좌우 제한
/*
	if(0 > m_lZoomStartX)
	{
		m_lZoomEndX = m_lZoomEndX - m_lZoomStartX;
		m_lZoomStartX = 0;
	}
	if(0 > m_lZoomStartY)
	{
		m_lZoomEndY = m_lZoomEndY - m_lZoomStartY;
		m_lZoomStartY = 0;
	}
	if(m_lImageWidth < m_lZoomEndX)
	{
		m_lZoomStartX = m_lZoomStartX + (m_lImageWidth - m_lZoomEndX);
		m_lZoomEndX = m_lImageWidth;
	}
	if(m_lImageHeight < m_lZoomEndY)
	{
		m_lZoomStartY = m_lZoomStartY + (m_lImageHeight - m_lZoomEndY);
		m_lZoomEndY = m_lImageHeight;
	}
*/
	//zoom범위 초과 감시
	if( (m_lZoomStartX < m_lZoomEndX) && (m_lZoomStartY < m_lZoomEndY) )
	{
		m_dblZoomFact = zoom;

		((C_CtrlImageRect *)GetParent())->SetPart(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);
	}
	else
	{
		m_lZoomStartX = s_x;
		m_lZoomStartY = s_y;
		m_lZoomEndX = e_x;
		m_lZoomEndY = e_y;
	}

	//Scroll
	ScrollRangeX = image_width - (m_lZoomEndX - m_lZoomStartX);
	if (ScrollRangeX < 0)
		ScrollRangeX = 0;
	ScrollRangeY = image_height - (m_lZoomEndY - m_lZoomStartY);
	if (ScrollRangeY < 0)
		ScrollRangeY = 0;

//	if(1 > zoom)
//	{
		SetScrollRange(SB_HORZ, 0, ScrollRangeX, TRUE);
		SetScrollRange(SB_VERT, 0, ScrollRangeY, TRUE);
		SetScrollPos(SB_HORZ, m_lZoomStartX, TRUE);
		SetScrollPos(SB_VERT, m_lZoomStartY, TRUE);
		ShowScrollBar(SB_BOTH, TRUE);
//	}
//	else if(1 == zoom)
//	{
//		SetScrollRange(SB_HORZ, 0, image_width, TRUE);
//		SetScrollRange(SB_VERT, 0, image_height, TRUE);
//		SetScrollPos(SB_HORZ, m_lZoomStartX, TRUE);
//		SetScrollPos(SB_VERT, m_lZoomStartY, TRUE);
//	}
//	else
//	{
//		SetScrollRange(SB_HORZ, 0, image_width, TRUE);
//		SetScrollRange(SB_VERT, 0, image_height, TRUE);
//		SetScrollPos(SB_HORZ, m_lZoomStartX, TRUE);
//		SetScrollPos(SB_VERT, m_lZoomStartY, TRUE);
//	}

	//Zoom 적용
	int i, tracker_num;
	C_CtrlTracker tracker;
	double rect_l, rect_t, rect_r, rect_b;

	
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num); i++)
	{
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			switch(tracker.GetType())
			{
				case 0:
				case 1:
					tracker.GetRect(&rect_l, &rect_t, &rect_r, &rect_b);

					//Zoom 적용
					tracker.GetZoom(&s_x, &s_y, &e_x, &e_y);

					rect_l =  s_x + rect_l / (((double)m_WinRect.Width()) / ((double)(e_x - s_x)));
					rect_t =  s_y + rect_t / (((double)m_WinRect.Height()) / ((double)(e_y - s_y)));
					rect_r =  s_x + rect_r / (((double)m_WinRect.Width()) / ((double)(e_x - s_x)));
					rect_b =  s_y + rect_b / (((double)m_WinRect.Height()) / ((double)(e_y - s_y)));
									
					rect_l = (rect_l - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
					rect_t = (rect_t - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));
					rect_r = (rect_r - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
					rect_b = (rect_b - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));
					
					tracker.SetZoom(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);
					tracker.SetRect(rect_l, rect_t, rect_r, rect_b);
					m_Tracker.SetAt(pos, tracker);
					break;
				default:
					AfxMessageBox(_T("Error : invalid index!"));
					break;
			}
		}
	}

	((C_CtrlImageRect *)GetParent())->Invalidate();
}

void CDialogMainView::GetZoom(double *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y)  
{
	// TODO: Add your dispatch handler code here
	*zoom = m_dblZoomFact;
	*image_width = m_lImageWidth;
	*image_height = m_lImageHeight;
	*start_x = m_lZoomStartX;
	*start_y = m_lZoomStartY;
	*end_x = m_lZoomEndX;
	*end_y = m_lZoomEndY;
}

void CDialogMainView::SetActiveGrid(BOOL active) 
{
	// TODO: Add your dispatch handler code here
	m_bActiveGrid = active;
}

void CDialogMainView::SetGridPitch(double pitch) 
{
	// TODO: Add your dispatch handler code here
	m_dGridPitch = pitch;
}

void CDialogMainView::DrawGrid() 
{
	// TODO: Add your dispatch handler code here
	if(m_bActiveGrid)
	{

	}
}

BOOL CDialogMainView::GetActiveGrid() 
{
	// TODO: Add your dispatch handler code here

	return m_bActiveGrid;
}

double CDialogMainView::GetGridPitch() 
{
	// TODO: Add your dispatch handler code here

	return m_dGridPitch;
}

void CDialogMainView::SetActiveGray(BOOL active) 
{
	// TODO: Add your dispatch handler code here
	m_bActiveGray = active;
}

BOOL CDialogMainView::GetActiveGray() 
{
	// TODO: Add your dispatch handler code here

	return m_bActiveGray;
}

void CDialogMainView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int i, tracker_num;
	C_CtrlTracker *tracker;

	//검사용
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num) && m_bActive; i++)
	{
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = &m_Tracker.GetAt(pos);
			switch(tracker->GetType())
			{
				case 0:
				case 1:
					tracker->OnLButtonDown(nFlags, point);
					m_Tracker.SetAt(pos, *tracker);
					break;
				default:
					AfxMessageBox(_T("Error : Invalid index!"));
					break;
			}
		}
	}

	long s_x, s_y, e_x, e_y;

	s_x = m_lZoomStartX;
	s_y = m_lZoomStartY;
	e_x = m_lZoomEndX;
	e_y = m_lZoomEndY;
	//tracker->GetZoom(&s_x, &s_y, &e_x, &e_y);

	//Pixel gray
	if(m_bActiveGray)
	{
		m_lPixelX = s_x + ( ((double)point.x)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
		m_lPixelY = s_y + ( ((double)point.y)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));

		if( (0 < m_lPixelX) && (0 < m_lPixelY) && (m_lImageWidth > m_lPixelX) && (m_lImageHeight > m_lPixelY) )
		{
		
			((C_CtrlImageRect *)GetParent())->PixelGrayValue(m_lPixelX, m_lPixelY, -1);
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CDialogMainView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int i, tracker_num;
	C_CtrlTracker *tracker;

	//검사용
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num) && m_bActive; i++)
	{
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = &m_Tracker.GetAt(pos);
			switch(tracker->GetType())
			{
				case 0:
				case 1:
					tracker->OnLButtonUp(nFlags, point);
					m_Tracker.SetAt(pos, *tracker);
					break;
				default:
					AfxMessageBox(_T("Error : invalid index!"));
					break;
			}
		}
	}

	if(m_bActive)
	{
	
		((C_CtrlImageRect *)GetParent())->Invalidate();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDialogMainView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int i, tracker_num;
	C_CtrlTracker *tracker;
	CRect rect;
	BOOL check = FALSE;

	//20080803
	if(m_bActive)
	{
		m_iActiveIdx = -1;
	}

	//검사용
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num) && m_bActive; i++)
	{
		
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = &m_Tracker.GetAt(pos);
			switch(tracker->GetType())
			{
				case 0:
				case 1:
					if( ((!check) && tracker->OnLButtonDblClk(nFlags, point)) || (tracker->GetGroup()) )
					{
						check = TRUE;
						m_iActiveIdx = i;
						tracker->SetEnable(TRUE);
					}
					else
					{
						tracker->SetEnable(FALSE);
					}
					m_Tracker.SetAt(pos, *tracker);
					break;
				default:
					AfxMessageBox(_T("Error : Invalid index!"));
					break;
			}
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDialogMainView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int i, tracker_num;
	C_CtrlTracker *tracker;
	long s_x, s_y, e_x, e_y;
	double r_l, r_t, r_r, r_b;

	//Limit
	if(m_WinRect.right < point.x)
	{
		point.x = m_WinRect.right;
	}

	//검사용
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num) && m_bActive; i++)
	{
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = &m_Tracker.GetAt(pos);
			switch(tracker->GetType())
			{
				case 0:
				case 1:
					

					//Limit
					tracker->GetRect(&r_l, &r_t, &r_r, &r_b);
					tracker->GetZoom(&s_x, &s_y, &e_x, &e_y);
					if( m_lImageWidth < ( s_x + r_r * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()) ) )
					{
						r_r = ((double)(m_lImageWidth - s_x)) / ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()) - 2;
					}
					tracker->SetRect(r_l, r_t, r_r, r_b);

					tracker->OnMouseMove(nFlags, point);
					m_Tracker.SetAt(pos, *tracker);
					break;
				default:
					AfxMessageBox(_T("Error : Invalid index!"));
					break;
			}
		}
	}

	s_x = m_lZoomStartX;
	s_y = m_lZoomStartY;
	e_x = m_lZoomEndX;
	e_y = m_lZoomEndY;

	//Pixel gray
	if(m_bActiveGray)
	{
		m_lPixelX = s_x + ( ((double)point.x)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
		m_lPixelY = s_y + ( ((double)point.y)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));

		if( (0 < m_lPixelX) && (0 < m_lPixelY) && (m_lImageWidth > m_lPixelX) && (m_lImageHeight > m_lPixelY) )
		{
			((C_CtrlImageRect *)GetParent())->PixelGrayValue(m_lPixelX, m_lPixelY, m_sPixelGray);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDialogMainView::SetView(Hlong window)
{
	m_lWindowID = window;
}

void CDialogMainView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
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

	
	int i, tracker_num;
	C_CtrlTracker* tracker;

	//검사용
	tracker_num = m_Tracker.GetCount();
	for(i = 0; (i < tracker_num); i++)
	{
		POSITION pos = m_Tracker.FindIndex(i);
		if(NULL != pos)
		{
			tracker = &m_Tracker.GetAt(pos);
			switch(tracker->GetType())
			{
				case 0:
				case 1:
					tracker->Draw(i);
					break;
				default:
					AfxMessageBox(_T("Error : Invalid index!"));
					break;
			}
		}
	}
}

void CDialogMainView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	long diff;

	diff = m_lZoomEndX - m_lZoomStartX;
	
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	
	GetScrollInfo(SB_HORZ, &scrollInfo);
	
	switch(nSBCode)
	{
	case SB_LINELEFT:
		m_lZoomStartX -= 2;
		
		break;
	case SB_LINERIGHT:
		m_lZoomStartX += 2;
		
		break;
	case SB_PAGELEFT:
		
		break;
	case SB_PAGERIGHT:
		
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(nSBCode == SB_THUMBPOSITION)
			m_lZoomStartX = scrollInfo.nPos;
		else
			m_lZoomStartX = scrollInfo.nTrackPos;
		break;
		
	case SB_ENDSCROLL:
		return;
	}
	
	m_lZoomEndX = m_lZoomStartX + diff;
	
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = m_lZoomStartX;
	SetScrollInfo(SB_HORZ, &scrollInfo);

	m_bZoom1 = FALSE;
	SetZoom(m_dblZoomFact, m_lImageWidth, m_lImageHeight);
	m_bZoom1 = TRUE;


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDialogMainView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	long diff;

	diff = m_lZoomEndY - m_lZoomStartY;
	
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	
	GetScrollInfo(SB_VERT, &scrollInfo);
	
	switch(nSBCode)
	{
	case SB_LINELEFT:
		m_lZoomStartY -= 2;
		
		break;
	case SB_LINERIGHT:
		m_lZoomStartY += 2;
		
		break;
	case SB_PAGELEFT:
		
		break;
	case SB_PAGERIGHT:
		
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(nSBCode == SB_THUMBPOSITION)
			m_lZoomStartY = scrollInfo.nPos;
		else
			m_lZoomStartY = scrollInfo.nTrackPos;
		break;
		
	case SB_ENDSCROLL:
		return;
	}
	
	m_lZoomEndY = m_lZoomStartY + diff;
	
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = m_lZoomStartY;
	SetScrollInfo(SB_VERT, &scrollInfo);

	m_bZoom1 = FALSE;
	SetZoom(m_dblZoomFact, m_lImageWidth, m_lImageHeight); 
	m_bZoom1 = TRUE;

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


//라인 그리기
void CDialogMainView::ImageDrawLine()
{
	C_CtrlImageRect * pdlg = (C_CtrlImageRect *)AfxGetApp()->m_pMainWnd;
	Hobject line;

	double	zoom;
	long sx,sy,ex,ey,width,height;
	GetZoom(&zoom,&width,&height,&sx,&sy,&ex,&ey);
	if(zoom == 0)
	{
		zoom = 1;
	}

	set_color(m_lWindowID, "red");
	set_draw(m_lWindowID, HTuple("margin"));
	set_line_width(m_lWindowID, 2);

	gen_region_line(&line, (Hlong)((m_poDrawStartPoint.y / zoom) + sy), (Hlong)((m_poDrawStartPoint.x / zoom) + sx), 
							(Hlong)((m_poDrawEndPoint.y / zoom) + sy), (Hlong)((m_poDrawEndPoint.x / zoom) + sx));

	disp_obj(line, m_lWindowID);
	ImageLineSize();
}

//라인 사이즈
void CDialogMainView::ImageLineSize()
{
	C_CtrlImageRect * pdlg = (C_CtrlImageRect *)AfxGetApp()->m_pMainWnd;
	CString temp;
	CPoint start, end, start_tposition, end_tposition;

	CRect rect, win_rect;
	this->GetClientRect(rect);
	this->GetWindowRect(win_rect);

	double	zoom;
	long sx,sy,ex,ey,width,height;
	GetZoom(&zoom,&width,&height,&sx,&sy,&ex,&ey);

	start.x = (int)(sx + (m_poStartSizePoint.x * m_nZoomRat) + (m_rZoomRect.left / ((double)m_lImageWidth / (double)ex-sx)));
	start.y = (int)(sy + (m_poStartSizePoint.y * m_nZoomRat) + (m_rZoomRect.top / ((double)m_lImageHeight / (double)ey-sy)));
	end.x = (int)(ex + (m_poEndSizePoint.x * m_nZoomRat) + (m_rZoomRect.left / ((double)m_lImageWidth / (double)ex-sx)));
	end.y = (int)(ey + (m_poEndSizePoint.y * m_nZoomRat) + (m_rZoomRect.top / ((double)m_lImageHeight / (double)ey-sy)));

	start_tposition.x = (int)((m_poDrawStartPoint.x * m_nZoomRat) + m_rZoomRect.left);
	start_tposition.y = (int)((m_poDrawStartPoint.y * m_nZoomRat) + m_rZoomRect.top);

	if((start_tposition.x) >= ((m_rZoomRect.Width() * (rect.Width() / win_rect.Width())) + m_rZoomRect.left))
	{
		start_tposition.x = (int)((m_rZoomRect.Width() * (rect.Width() / win_rect.Width())) + (m_rZoomRect.left));
	}
	if((start_tposition.y) >= ((m_rZoomRect.Height() * (rect.Height() / win_rect.Height())) + m_rZoomRect.top))
	{
		start_tposition.y = (int)((m_rZoomRect.Height() * (rect.Height() / win_rect.Height())) + (m_rZoomRect.top));
	}

	end_tposition.x = (int)((m_poDrawEndPoint.x * m_nZoomRat) + m_rZoomRect.left);
	end_tposition.y = (int)((m_poDrawEndPoint.y * m_nZoomRat) + m_rZoomRect.top);

	if((end_tposition.x) >= ((m_rZoomRect.Width() * (rect.Width() / win_rect.Width())) + m_rZoomRect.left))
	{
		end_tposition.x = (int)((m_rZoomRect.Width() * (rect.Width() / win_rect.Width())) + (m_rZoomRect.left));
	}
	if((end_tposition.y) >= ((m_rZoomRect.Height() * (rect.Height() / win_rect.Height())) + m_rZoomRect.top))
	{
		end_tposition.y = (int)((m_rZoomRect.Height() * (rect.Height() / win_rect.Height())) + (m_rZoomRect.top));
	}

	set_color(m_lWindowID, "red");
	set_tposition(m_lWindowID, (Hlong)(start_tposition.y), (Hlong)(start_tposition.x));
	
	char temp1[512];
	sprintf(temp1, "Start x : %d, Start y : %d", start.x, start.y);
	write_string(m_lWindowID, (HTuple)temp1);

	set_color(m_lWindowID, "red");
	set_tposition(m_lWindowID, (Hlong)(end_tposition.y), (Hlong)(end_tposition.x));
	sprintf(temp1, "End x : %d, End y : %d", end.x, end.y);
	write_string(m_lWindowID, (HTuple)temp1);

	set_color(m_lWindowID, "red");
	set_tposition(m_lWindowID, (Hlong)(end_tposition.y + 10), (Hlong)(end_tposition.x));
	sprintf(temp1, "Size x : %.2f um, Size y : %.2f um", (double)(end.x - start.x) * pdlg->GetPixelSize()[0],
													(double)(end.y - start.y) * pdlg->GetPixelSize()[1]);
	write_string(m_lWindowID, (HTuple)temp1);
}

BOOL CDialogMainView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	CRect main_rect, zoom_rect;
	this->GetWindowRect(main_rect);
	
	if(m_bDragSize)
	{
		//특정 위치를 벋어날 시 이미지 이동 or 사이즈 기능 종료
		if(pMsg->message != WM_LBUTTONDOWN || pMsg->message != WM_LBUTTONUP || pMsg->message != WM_MOUSEMOVE || 
			pMsg->message != WM_RBUTTONDOWN || pMsg->message != WM_RBUTTONUP)
		{
			if(main_rect.left >= pMsg->pt.x && pMsg->pt.x >= main_rect.right && main_rect.top >= pMsg->pt.y && pMsg->pt.y >= main_rect.bottom)
			{
				m_bImageNGSize = FALSE;
				return TRUE;
			}
		}

		CPoint point, temp;
		//WM_RBUTTONUP, WM_RBUTTONDOWN 사이즈
		//WM_LBUTTONUP, WM_LBUTTONDOWN 이미지 이동
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			if(main_rect.left <= pMsg->pt.x && pMsg->pt.x <= main_rect.right && main_rect.top <= pMsg->pt.y && pMsg->pt.y <= main_rect.bottom)
			{
				m_bImageNGSize = TRUE;

				temp.x = (int)((pMsg->pt.x - main_rect.left) / MainRectRat1_x);
				temp.y = (int)((pMsg->pt.y - main_rect.top) / MainRectRat1_y);

				m_poDrawStartPoint = temp;

				temp.x = (int)((pMsg->pt.x - main_rect.left) * MainRectRat2_x);
				temp.y = (int)((pMsg->pt.y - main_rect.top) * MainRectRat2_y);

				m_poStartSizePoint = temp;
				m_bMouseMove = TRUE;
			}
		}
		else if(pMsg->message == WM_LBUTTONUP)
		{
			if(main_rect.left <= pMsg->pt.x && pMsg->pt.x <= main_rect.right && main_rect.top <= pMsg->pt.y && pMsg->pt.y <= main_rect.bottom)
			{
				m_bMouseMove = FALSE;
				m_bImageNGSize = FALSE;
			}
		}
		else if(pMsg->message == WM_MOUSEMOVE && m_bMouseMove)
		{
			if(m_bImageNGSize && main_rect.left <= pMsg->pt.x && pMsg->pt.x <= main_rect.right && main_rect.top <= pMsg->pt.y && pMsg->pt.y <= main_rect.bottom)
			{
				temp.x = (int)((pMsg->pt.x - main_rect.left) / MainRectRat1_x);
				temp.y = (int)((pMsg->pt.y - main_rect.top) / MainRectRat1_y);

				m_poDrawEndPoint = temp;

				temp.x = (int)((pMsg->pt.x - main_rect.left) * MainRectRat2_x);
				temp.y = (int)((pMsg->pt.y - main_rect.top) * MainRectRat2_y);

				m_poEndSizePoint = temp;

				ImageDrawLine();
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogMainView::SetPos(LPRECT pCenterArea)
{
	if (pCenterArea == NULL)
		return;

	long	CenterX, CenterY, ZoomWidth, ZoomHeight;

	CenterX = (pCenterArea->left + pCenterArea->right) / 2;
	CenterY = (pCenterArea->top + pCenterArea->bottom) / 2;
	ZoomWidth	= m_lZoomEndX - m_lZoomStartX;
	ZoomHeight	= m_lZoomEndY - m_lZoomStartY;

	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);

	if (ZoomWidth < m_lImageWidth)
	{
		m_lZoomStartX = CenterX - (ZoomWidth / 2);
		m_lZoomEndX = m_lZoomStartX + ZoomWidth;

		GetScrollInfo(SB_HORZ, &scrollInfo);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = m_lZoomStartX;
		SetScrollInfo(SB_HORZ, &scrollInfo);
	}

	if (ZoomHeight < m_lImageHeight)
	{
		m_lZoomStartY = CenterY - (ZoomHeight / 2);
		m_lZoomEndY = m_lZoomStartY + ZoomHeight;

		GetScrollInfo(SB_VERT, &scrollInfo);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = m_lZoomStartY;
		SetScrollInfo(SB_VERT, &scrollInfo);
	}

	m_bZoom1 = FALSE;
	SetZoom(m_dblZoomFact, m_lImageWidth, m_lImageHeight);
	m_bZoom1 = TRUE;
}
