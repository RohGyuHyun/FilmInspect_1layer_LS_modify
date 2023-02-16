#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define LINE_LIGHT_COM_PORT			8
#define LINE_LIGHT_COM_BAUDRATE		19200


//������ ���� ���ν�ĵ ����
//#define LINE_LIGHT_COM_PORT			8
//#define LINE_LIGHT_COM_BAUDRATE		38400


// CLightCtrlDlg ��ȭ �����Դϴ�.

class CLightCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightCtrlDlg)

public:
	CLightCtrlDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLightCtrlDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LIGHT_CONTROL_DIALOG };

public:
	CSliderCtrl	m_cCh1SliderCtrl;
	CSliderCtrl	m_cCh2SliderCtrl;
	CSliderCtrl	m_cCh3SliderCtrl;
	CSliderCtrl	m_cCh4SliderCtrl;
	CSliderCtrl	m_cCh5SliderCtrl;
	CSliderCtrl	m_cCh6SliderCtrl;
	CSliderCtrl	m_cCh7SliderCtrl;
	CSliderCtrl	m_cCh8SliderCtrl;

	UINT	m_img0_ch0;
	UINT	m_img0_ch1;
	UINT	m_img0_ch2;
	UINT	m_img0_ch3;
	UINT	m_img0_ch4;
	UINT	m_img0_ch5;
	UINT	m_img0_ch6;
	UINT	m_img0_ch7;

	CFilmInsDlg *pCFilmInsDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlLightOk();
	void ClickBtnenhctrlLightCancel();
	
	afx_msg void OnNMCustomdrawSlider5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider6(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider7(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider8(NMHDR *pNMHDR, LRESULT *pResult);
	
	BOOL m_bLineLightFan;
	afx_msg void OnBnClickedCheckLineFan();
	CButton m_ctrlLineLightFan;
};
