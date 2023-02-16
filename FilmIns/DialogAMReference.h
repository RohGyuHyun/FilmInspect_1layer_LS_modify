#pragma once


// CDialogAMReference ��ȭ �����Դϴ�.
#define		VIEW_MAX_NUM				2
#define		VIEW_AREA_OFFSET			25

class CDialogAMReference : public CDialog
{
	DECLARE_DYNAMIC(CDialogAMReference)

public:
	CDialogAMReference(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogAMReference();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_AM_REFERENCE };

protected:
	Hlong m_hWindow[VIEW_MAX_NUM];
	Hobject m_objImage[VIEW_MAX_NUM];

	CRect m_crView;

public:
	void DisplayReferenceImage(int idx, CString path);
	void SaveReferenceImage(int idx, CRect rect, Hobject img, CString path, int data_1 = 0, int data_2 = 0);
	void ResizeView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
