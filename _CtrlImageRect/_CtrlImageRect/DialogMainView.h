#pragma once

// CDialogMainView ��ȭ �����Դϴ�.
#include "resource.h"
#include "_CtrlTracker.h"

//#define IDD_DIALOG_MAIN_VIEW 10000

class CDialogMainView : public CDialog
{
	DECLARE_DYNAMIC(CDialogMainView)

public:
	CDialogMainView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogMainView();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MAIN_VIEW };

private:
	CList<C_CtrlTracker, C_CtrlTracker &> m_Tracker;

	BOOL m_bActive;
	int m_iActiveIdx;
	//���� ��
	COLORREF m_curColor;

	//bjs
	double	m_dblWndImgRate;

	//Zoom
	double m_dblZoomFact;
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
	

public:
	CRect m_WinRect;
	CRect m_WinRectOld;

	BOOL m_bDragSize;
	void SetView(Hlong window);

	void Draw();
	//�簢�� ���� �߰�
	void AddTracker(int type, CRect rect, COLORREF color, char *cap);
	//��� ���� ����
	void DeleteAll();
	//���� ����(������ ���� ������ FALSE ��ȯ)
	BOOL DeleteTracker(int idx);
	//�簢�� ���� ���(������ ���� ������ FALSE ��ȯ)
	BOOL GetTracker(int idx, CRect *rect);
	//�簢�� ���� ����(������ ���� ������ FALSE ��ȯ)
	BOOL SetTracker(int idx, CRect rect);
	BOOL SetGroup(int idx, BOOL grp);
	//Tracker ���� ���
	int GetNum();
	//Tracker ���� ���
	int GetType(int idx);
	//���� Ʈ��Ŀ ȭ�� Ȱ��ȭ ����
	void SetActiveImageRect(BOOL active);
	void SetActiveImageRect(int idx, BOOL active);

//	void SetZoom(long zoom, long image_width, long image_height);
	//	20191028 bjs Fit ������ ����, ª���� ������ �� �ֵ��� �ϰ�, Fit �Ǵ� 1:1 Zoom ���Ŀ��� Ȯ��/��� �̻��۵� ���ϵ��� ����
	void SetZoom(double zoom, long image_width, long image_height, BOOL bIsFitToLongSide = TRUE);
	void SetActiveGrid(BOOL active);
	void SetGridPitch(double pitch); 
	void DrawGrid(); 
	void GetZoom(double *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y);   
	BOOL GetActiveGrid(); 
	double GetGridPitch(); 
	void SetActiveGray(BOOL active); 
	BOOL GetActiveGray();

	//force position move
	void SetPos(LPRECT pCenterArea);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
