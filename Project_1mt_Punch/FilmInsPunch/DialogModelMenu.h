#pragma once


// CDialogModelMenu ��ȭ �����Դϴ�.

class CDialogModelMenu : public CDialog
{
	DECLARE_DYNAMIC(CDialogModelMenu)

public:
	CDialogModelMenu(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogModelMenu();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MODEL_MENU };

	CRect m_Rect;
	int m_Ret;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlModelNew();
	void ClickBtnenhctrlModelDelete();
	void ClickBtnenhctrlModelSelect();
};
