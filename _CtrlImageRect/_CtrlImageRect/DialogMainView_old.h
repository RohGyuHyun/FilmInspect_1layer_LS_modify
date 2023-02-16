#pragma once

// CDialogMainView 대화 상자입니다.
#include "resource.h"
#include "_CtrlTracker.h"

//#define IDD_DIALOG_MAIN_VIEW 10000

class CDialogMainView : public CDialog
{
	DECLARE_DYNAMIC(CDialogMainView)

public:
	CDialogMainView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogMainView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAIN_VIEW };

private:
	CList<C_CtrlTracker, C_CtrlTracker &> m_Tracker;

	BOOL m_bActive;
	int m_iActiveIdx;
	//현재 색
	COLORREF m_curColor;

	//bjs
	double	m_dblWndImgRate;

	//Zoom
	long m_lZoomFact;
	long m_lImageWidth;
	long m_lImageHeight;
	long m_lZoomStartX;
	long m_lZoomStartY;
	long m_lZoomEndX;
	long m_lZoomEndY;

	//Grid
	BOOL m_bActiveGrid;
	double m_dGridPitch;

	//Gray
	BOOL m_bActiveGray;
	long m_lPixelX;
	long m_lPixelY;
	short m_sPixelGray;

	BOOL m_bZoom1;

	Hlong m_lWindowID;
	BOOL m_bMouseMove;
	CRect main_temp;
	double MainRectRat1_x;
	double MainRectRat1_y;
	double MainRectRat2_x;
	double MainRectRat2_y;

	double m_nZoomRat;
	CPoint m_poImageSize;
	CPoint m_poCenterPoint;
	CPoint m_poStartPoint;
	CPoint m_poEndPoint;
	CRect m_rZoomRect;
	CPoint m_poStartTemp;
	BOOL m_bImageMove;

	BOOL m_bImageNGSize;
	CPoint m_poDrawStartPoint;
	CPoint m_poDrawEndPoint;
	CPoint m_poDrawStartTemp;

	CPoint m_poStartSizePoint;
	CPoint m_poEndSizePoint;

	void ImageDrawLine();
	void ImageLineSize();
	
	BOOL m_bCtrlClick;

public:
	CRect m_WinRect;
	CRect m_WinRectOld;

	BOOL m_bDragSize;
	void SetView(Hlong window);

	void Draw();
	//사각형 영역 추가
	void AddTracker(int type, CRect rect, COLORREF color, char *cap);
	//모든 영역 삭제
	void DeleteAll();
	//영역 삭제(생성해 있지 않으면 FALSE 반환)
	BOOL DeleteTracker(int idx);
	//사각형 영역 얻기(생성해 있지 않으면 FALSE 반환)
	BOOL GetTracker(int idx, CRect *rect);
	//사각형 영역 설정(생성해 있지 않으면 FALSE 반환)
	BOOL SetTracker(int idx, CRect rect);
	BOOL SetGroup(int idx, BOOL grp);
	//Tracker 개수 얻기
	int GetNum();
	//Tracker 종류 얻기
	int GetType(int idx);
	//현재 트랙커 화면 활성화 여부
	void SetActiveImageRect(BOOL active);
	void SetActiveImageRect(int idx, BOOL active);

//	void SetZoom(long zoom, long image_width, long image_height);
	void SetZoom(long zoom, long image_width, long image_height, double ExWndZoomRate = 0.0);	//	bjs, 외부 윈도우와 이미지간의 줌비율 추가(0이면 무시)
	void SetActiveGrid(BOOL active);
	void SetGridPitch(double pitch); 
	void DrawGrid(); 
	void GetZoom(long *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y);   
	BOOL GetActiveGrid(); 
	double GetGridPitch(); 
	void SetActiveGray(BOOL active); 
	BOOL GetActiveGray();

	//force position move
	void SetPos(LPRECT pCenterArea);

	void SetCtrlClick(BOOL bClick){m_bCtrlClick = bClick;};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
