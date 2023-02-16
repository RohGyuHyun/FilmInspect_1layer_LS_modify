
// FilmInsPunchDlg.h : 헤더 파일
//

#pragma once
#include "BtnEnh.h"
#include "afxcmn.h"

#define	IMAGE_RECT_DIALOG	10000
#define DIALOG_MOVE_OFFSET	390


#define SOCKET_PORT_NUM		3003

#define DUMMY_FILM_DETECT_LIMIT 180
// CFilmInsPunchDlg 대화 상자
class CFilmInsPunchDlg : public CDialog
{
// 생성입니다.
public:
	CFilmInsPunchDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILMINSPUNCH_DIALOG };

	short m_iVisIndex;				//Vision Index

	cnComm	m_cComm;				//Comm 
	CAreaCamControl m_cCam;			//Camera
	C_CtrlImageRect *m_dlgImgRect;	//View
	CClientSock *m_Client;			//Socket Client
	CDialogMainSetup *m_dlgSetup;	//Child Dialog

	CWinThread *m_pInspectThread;	//Inspect Thread
	CString m_strDBPath;			//데이터베이스 경로
	CString m_strRegiPath;			//레지스트리 경로
	CString m_strLogPath;			//저장 경로
	CBrush m_BrushBlack;
	CListCtrl m_list_tcp;
	CListCtrl m_list_rslt;

	typeInspectPara m_InsPara;
	typeSystemPara m_SysPara;
	typeMainSystemRslt m_MainSysRslt;

	BOOL m_bInspectStart;//Auto inspection flag
	BOOL m_bInspectFlag;
	BOOL m_bInspectResult;

	Hobject m_HalGrabImage;				//Main Image buffer
	Hobject m_HalProcImage;

	CBtnEnh m_label_model;
	CBtnEnh m_ctrlMainTitle;
	CBtnEnh m_label_mode;
	CBtnEnh m_btnStartStop;
	CBtnEnh m_btnSetup;
	CBtnEnh m_label_online;
	CBtnEnh m_label_tot;
	CBtnEnh m_label_good;
	CBtnEnh m_label_ng;

	BOOL WriteLogStatus(CString path, CString file, CString bak_file, CString log, long limit_line);
	int HandleAllMessage();
	int writeSCL(char *str, bool needResponse);
	BOOL SaveImage();
	BOOL WriteErrorStatus(CString path, CString file, CString bak_file, CString log, long limit_line);
	void WriteStatusList(LPCTSTR lpszItem);
	void ModelSave();
	BOOL RemoteControlModel(short menu_select, CString model_name);
	void SetLight(int ch, int val);
	void OnSend(CString data);
	void WriteErrorList(LPCTSTR lpszItem);	
	void WriteTCPList(LPCTSTR lpszItem, BOOL IsRCVorSND);
	

	BOOL StartInspectThread();
	BOOL StopInspectThread();

	BOOL SaveInspectPara(CString path);
	BOOL LoadInspectPara(CString path);
	BOOL SaveSystemPara(CString path);
	BOOL LoadSystemPara(CString path);
	BOOL SaveSystemRslt(CString path);
	BOOL LoadSystemRslt(CString path);

	UINT static InspectThread(LPVOID pParam);


	BOOL Inspection();
	BOOL InspectionAlign();
	BOOL FindPPuchingChip(Hlong win, Hobject image, CRect ins_region, Hobject *rslt_region);
	void InspectResult();

	int GetHalRegionMean(Hobject img, CRect rect);	//20161025 by 이미지 내 특정영역의 평균 밝기 값을 리턴

	//GET/SET Func.
	void SetHalImage(Hobject img) { m_HalGrabImage = img; }
	Hobject *GetHalImage() { return &m_HalGrabImage; }
	Hobject *GetHalProcImage() { return &m_HalProcImage; }

	void SetVisIndex(short vis_idx) { m_iVisIndex = vis_idx; }
	short GetVisIndex() { return m_iVisIndex; }
	
	typeInspectPara GetInspectPara() { return m_InsPara; }
	typeSystemPara GetSystemPara() { return m_SysPara; }

	void SetInspectPara(typeInspectPara para) { m_InsPara = para; }
	void SetSystemPara(typeSystemPara para) { m_SysPara = para; }

	void ResetInspectPara()	{ memset(&m_InsPara, NULL, sizeof(m_InsPara));}
	void ResetSystemPara()	{ memset(&m_SysPara, NULL, sizeof(m_SysPara));}
	
	typeMainSystemRslt GetMainSysRslt(){ return m_MainSysRslt; }
	void SetMainSysRslt(typeMainSystemRslt rslt){ m_MainSysRslt = rslt; }
	void ResetMainSysRslt(){ memset(&m_MainSysRslt, NULL, sizeof(m_MainSysRslt)); }

	LRESULT OnImageRectMessage(WPARAM para0, LPARAM para1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnConnect(WPARAM, LPARAM);
	afx_msg LRESULT OnReceive(WPARAM, LPARAM);
	afx_msg LRESULT OnClose(WPARAM, LPARAM);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlMainRunstop();
	void ClickBtnenhctrlMainSetup();
	void ClickBtnenhctrlMainModel();
	void ClickBtnenhctrlMainExit();
	afx_msg void OnBnClickedButton1();
};
