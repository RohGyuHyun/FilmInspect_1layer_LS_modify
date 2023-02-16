
// FilmIns.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "FilmIns.h"

#include "UserSockClient.h"
#include "include/_FuncModuleHal.h"
#include "include/_FuncModuleBasic.h"
#include "include/_VisionModuleFilmIns.h"
#include "include/_CtrlImageRect.h"
#include "include/_CtrlModelManage.h"
#include "DialogModelMenu.h"

#include "SetUpBsaeDlg.h"
#include "DialogProcessing.h"

#include "FilmInsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilmInsApp

BEGIN_MESSAGE_MAP(CFilmInsApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFilmInsApp ����

CFilmInsApp::CFilmInsApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	
	//m_hMutex = NULL;
}


// ������ CFilmInsApp ��ü�Դϴ�.

CFilmInsApp theApp;


// CFilmInsApp �ʱ�ȭ

BOOL CFilmInsApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	CWinApp *App = AfxGetApp();
	int cmdLine = atoi(App->m_lpCmdLine);
	
	if(1 == cmdLine)
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);	//20191120 by ���μ��� �켱������ �ǽð����� ����. (T/T ������ ��� ����.. CPU �ڿ��� �ִ��� ����� ����)
/*
	//���α׷� �ߺ� ���� ����
	m_hMutex = CreateMutexA(NULL, FALSE, "FilmIns");
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		AfxMessageBox("dafsfasfaff");
		return FALSE;
	}
*/
	if (!AfxSocketInit())
	{
		AfxMessageBox("AfxSocketInit() Failed..");
		return FALSE;
	}

#ifdef _DEBUG
	//_CrtSetBreakAlloc();	//20190828 by �޸𸮸� ���� �����
#endif

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CFilmInsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}
