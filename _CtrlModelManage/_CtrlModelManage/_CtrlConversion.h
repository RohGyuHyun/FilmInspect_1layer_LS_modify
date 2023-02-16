#pragma once
#include "afxcmn.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// C_CtrlConversionDlg dialog
//#define IDD_CTRLCONVERSION_DIALOG       10002
#define MAX_COV_NAME					100
#define MAX_COV_DESC					100
#define MAX_CON_LIST_NUM				30

#define MOD_CONV_PATH					"D:\\DATABASE\\MODEL"
#define MOD_CONV_REGI   				"SOFTWARE\\CANINS\\CONVERSION"


// C_CtrlConversionDlg ��ȭ �����Դϴ�.

class C_CtrlConversionDlg : public CDialog
{
	DECLARE_DYNAMIC(C_CtrlConversionDlg)

public:
	C_CtrlConversionDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~C_CtrlConversionDlg();

	void Open();
	char *GetConversionName(int idx);
	//Conversion ������
	char *GetConversionDesc(int idx);
	//Conversion ��θ���
	char *GetConversionPath(int idx);

// ��ȭ ���� �������Դϴ�.
	//enum { IDD = IDD_CTRLCONVERSION_DIALOG };

	
private:
	char m_szMainPath[MAX_PATH];
	char m_szMainRegi[MAX_PATH];
	BOOL GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue);
	BOOL SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, LPCTSTR lpintValue);
	int RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey);
	BOOL AddList(typeModel mod, int idx);
	
	int m_iListNum;

	typeModel m_ConversionModel[MAX_CON_LIST_NUM];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_conv;
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlconvOk();
	void ClickBtnenhctrlconvCancel();
	afx_msg void OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
