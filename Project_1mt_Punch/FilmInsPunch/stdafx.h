
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����



#include "halconcpp.h"
#include <mmsystem.h>
#include <afxsock.h> 
#include <algorithm>

using namespace std;
using namespace Halcon;

#include "include/_FuncModuleHal.h"
#include "include/_FuncModuleBasic.h"
#include "include/_CtrlImageRect.h"
#include "include/_CtrlModelManage.h"

#define _FILMINS_1METAL

#define VISION_CAM_PUNCHING 3
#define	LIGHT_OFF			0
#define LIGHT_CH_INDEX		1

#define CONTROL_PC_IP_ADDRESS "1.1.1.1"
#define CONTROL_PC_PORT_NUM		3003

#define COF_MAX_NUM		1
#define COF_PF_COUNT	8
#define COF_PF_PITCH 4.75
#define COF_WIDTH    70.0
#define PIXEL_SIZE		60		//um

#define MICRO_METER		1000

#define LIST_MONI_NUM		50

#define CNT_MAX_NUM					10
#define BOOL_PARA_MAX_NUM			10
#define INT_PARA_MAX_NUM			10
#define DBL_PARA_MAX_NUM			10
#define RECT_PARA_MAX_NUM_ALIGN		2
#define RECT_PARA_MAX_NUM_CHIP		2
#define RECT_PARA_MAX_NUM_PUNCH		2

typedef struct InspectPara{
	BOOL m_bCreatMdIDAlign;
	Hlong m_PatIDAlign;
	typePatPara m_PatParaAlign;

	double m_dPatternPosXAlign;
	double m_dPatternPosYAlign;

	long m_iParaChip[INT_PARA_MAX_NUM];	//0: Threshold
	double m_dParaChip[DBL_PARA_MAX_NUM];	// 0: Area 1: cx 2: cy 3: width(mm) 4: height(mm)

	long m_iParaPunch[INT_PARA_MAX_NUM];	//0: Punch area min, 1: Punch area max, 2: Punch burr tolerance, 3: Punch center x, 4: Punch center y, 5: Punch Position limit,
	double m_dParaPunch[DBL_PARA_MAX_NUM];	// 0: Punch Width, 1: Punch height 2: Punch size limit
	int m_iParaBase[INT_PARA_MAX_NUM];		//0: Light var

	int m_iParaAlign[INT_PARA_MAX_NUM];		//0: Align threshold, 1: Align Score, 2: Align Position limit
	double m_dParaAlign[DBL_PARA_MAX_NUM];	//0: Teaching center x, 1: Teaching center y, 2: Inspection center x, 3:Inspection center y

	CRect m_InsRegionAlign[RECT_PARA_MAX_NUM_ALIGN]; // 0: Align search, 1: //Align mark
	CRect m_InsRegionChip[RECT_PARA_MAX_NUM_CHIP];	//0 :Chip search  
	CRect m_InsRegionPunch[RECT_PARA_MAX_NUM_PUNCH];	//0: Punch Search

	//BOOL m_iSystemPara[INT_PARA_MAX_NUM];	
}typeInspectPara;

typedef struct SystemPara{
	BOOL m_bParaSys[BOOL_PARA_MAX_NUM];	//0 : NG image 1: GOOD image
	int m_iParaSys[INT_PARA_MAX_NUM];	// 0: serial port
}typeSystemPara;

//Main result
typedef struct MainSystemRslt
{
	long m_lCnt[CNT_MAX_NUM];
}typeMainSystemRslt;



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


