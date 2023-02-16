// _CtrlConversionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_CtrlModelManage.h"
#include "_CtrlConversion.h"

#include "include/_FuncModuleBasic.h"


// C_CtrlConversionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(C_CtrlConversionDlg, CDialog)

C_CtrlConversionDlg::C_CtrlConversionDlg(CWnd* pParent /*=NULL*/)
	//: CDialog(C_CtrlConversionDlg::IDD, pParent)
	:CDialog(IDD_CTRLCONVERSION_DIALOG, pParent)
{
	memset(m_szMainPath, NULL, sizeof(m_szMainPath));
	sprintf(m_szMainPath, MOD_CONV_PATH);

	memset(m_szMainRegi, NULL, sizeof(m_szMainRegi));
	sprintf(m_szMainRegi, MOD_CONV_REGI);

	m_iListNum = 0;

	memset(&m_ConversionModel, NULL, sizeof(m_ConversionModel));
}

C_CtrlConversionDlg::~C_CtrlConversionDlg()
{
}

void C_CtrlConversionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_list_conv);
}


BEGIN_MESSAGE_MAP(C_CtrlConversionDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL, &C_CtrlConversionDlg::OnNMDblclkListModel)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// C_CtrlConversionDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(C_CtrlConversionDlg, CDialog)
	ON_EVENT(C_CtrlConversionDlg, IDC_BTNENHCTRLCONV_OK, DISPID_CLICK, C_CtrlConversionDlg::ClickBtnenhctrlconvOk, VTS_NONE)
	ON_EVENT(C_CtrlConversionDlg, IDC_BTNENHCTRLCONV_CANCEL, DISPID_CLICK, C_CtrlConversionDlg::ClickBtnenhctrlconvCancel, VTS_NONE)
END_EVENTSINK_MAP()

void C_CtrlConversionDlg::ClickBtnenhctrlconvOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString path, str0, str1;
	int i;
	
	RegDeleteSubKey(HKEY_LOCAL_MACHINE, m_szMainRegi);

	//레지스트리 적용하기
	//리스트 데이타
	for(i = 0; i < MAX_CON_LIST_NUM; i++)
	{
		str0.Empty();
		str0.Format("%d", i);

		//제품명
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		str1.Empty();
		str1 = m_list_conv.GetItemText(i,1);
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}

		//설명
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		str1.Empty();
		str1 = m_list_conv.GetItemText(i,2);
		SetRegString(path, str0, str1);

		//경로명
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		str1.Empty();
		str1 = m_list_conv.GetItemText(i,3);
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}
	}

	EndDialog(1);
}

void C_CtrlConversionDlg::ClickBtnenhctrlconvCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	EndDialog(0);
}

void C_CtrlConversionDlg::OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iListNum = pNMListView->iItem;

	if( (0 <= m_iListNum) && (MAX_CON_LIST_NUM >=m_iListNum) )
	{

		//Model
		C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg();

		if(moddlg->DoModal(3))
		{
			m_list_conv.SetItemText(m_iListNum, 1, moddlg->GetConversionName());
			m_list_conv.SetItemText(m_iListNum, 2, moddlg->GetConversionDesc());
			m_list_conv.SetItemText(m_iListNum, 3, moddlg->GetConversionPath());
		}
		
		delete moddlg;

	}

	*pResult = 0;
}

BOOL C_CtrlConversionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	int i;
	LV_COLUMN column;

	SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE); 

	char* list[5]={"번호", "제품명", "설명", "경로명"};
	int width[5]={50, 200, 550, 700};
	for(i=0; i<5; i++)
	{
		column.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		column.fmt=LVCFMT_LEFT;
		column.pszText=list[i];
		column.iSubItem=i;
		column.cx=width[i];
		m_list_conv.InsertColumn(i, &column);
	}

	//
	//
	//
	//
	//

	//데이타 불러오기////////////////////////////
	CString str0, str1, path;
	typeModel mod;
	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;

	path.Empty();
	path.Format("%s\\NAME", m_szMainRegi);

	memset(&mod, NULL, sizeof(mod));

	//모든데이타 불러오기
	for(i = (MAX_CON_LIST_NUM - 1); i >= 0; i--)
	{
		str0.Empty();
		str0.Format("%d", i);

		memset(&mod, NULL, sizeof(mod));

		//제품명
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szName))
		{
			//break;
		}

		//설명
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szDesc))
		{
			//break;
		}

		//경로명
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szPath))
		{
			//break;
		}

		//리스트 구성
		AddList(mod, i);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void C_CtrlConversionDlg::OnPaint()
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

}


//Get registry
BOOL C_CtrlConversionDlg::GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue)
{
    HKEY hkResult;
    DWORD dwLength;// = DAT_BUF_LEN;
    DWORD dwType;
 
    // Get Registry Key handle
    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                                        lpstrKey,
                                        0, 
                                        KEY_ALL_ACCESS,
                                        &hkResult)) {

        return FALSE;
    }
	else
	{
 
		// Get Entry Value
		if (ERROR_SUCCESS != RegQueryValueEx(hkResult,
											  lpstrEntry,                                           
											   NULL,
											   &dwType,                                        
											   (LPBYTE)lpintValue,
											   &dwLength)) {

			RegCloseKey(hkResult);
			return FALSE;
		}

   
		RegCloseKey(hkResult);
	}
    return TRUE;
}

BOOL C_CtrlConversionDlg::SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, LPCTSTR lpintValue)
{

    TCHAR ClassName[MAX_PATH]; // Buffer for class name.
    HKEY hkey;
    DWORD dwDesc;

    // Set Registry Key & Value
    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
                                        lpstrKey, 
                                          0,
                                          ClassName, 
                                          REG_OPTION_NON_VOLATILE, 
                                          KEY_ALL_ACCESS, 
                                          NULL, 
                                          &hkey,
                                          &dwDesc)) {
 
        RegSetValueEx(hkey, lpstrEntry, 0, REG_SZ, (BYTE*)lpintValue, strlen(lpintValue));
        RegCloseKey(hkey);
 
        return TRUE;
    }
 
    return FALSE;
}

//레지스트리 전부삭제(하위 레지스트리까지 삭제)
int C_CtrlConversionDlg::RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey)
{
    // RegDeleteKey() 함수는 NT이후 서브키가 있으면 삭제가 안된다.
    // 그래서 서브키까지 모두 삭제해주는 함수를 만들었다.

    HKEY newKey;
    char newSubKey[MAX_PATH];
    LONG Result;
    DWORD Size;
    FILETIME FileTime;

    RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &newKey);
    Result = ERROR_SUCCESS;

    while(TRUE) 
	{
        Size = MAX_PATH;
        // 계속 키가 삭제 되므로 dwIndex는 항상 0을 넣어주어야 한다.
        // 만약 for문으로 i를 증가시며 사용하면 하나지우고 하나 뛰어넘어 반이 남는다.
        Result = RegEnumKeyEx(newKey, 0, newSubKey, &Size, NULL, NULL, NULL, &FileTime);
        if (Result != ERROR_SUCCESS) break;
        Result = RegDeleteSubKey(newKey, newSubKey);
        if (Result  != ERROR_SUCCESS) break;
    }
    RegCloseKey(newKey);

    return RegDeleteKey(hKey, lpSubKey);
}

//모델리스트에 추가
BOOL C_CtrlConversionDlg::AddList(typeModel mod, int idx)
{
	LV_ITEM lvitem;
	char temp[MAX_COV_NAME];

	sprintf(temp, "%d", idx + 1);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=0;
	lvitem.pszText=temp;
	m_list_conv.InsertItem(&lvitem);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=1;
	lvitem.pszText=mod.szName;
	m_list_conv.SetItem(&lvitem);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=2;
	lvitem.pszText=mod.szDesc;
	m_list_conv.SetItem(&lvitem);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=3;
	lvitem.pszText=mod.szPath;
	m_list_conv.SetItem(&lvitem);

	return TRUE;
}


//Conversion 제품명얻기
char *C_CtrlConversionDlg::GetConversionName(int idx)
{
	return m_ConversionModel[idx].szName;
}

//Conversion 설명얻기
char *C_CtrlConversionDlg::GetConversionDesc(int idx)
{
	return m_ConversionModel[idx].szDesc;
}

//Conversion 경로명얻기
char *C_CtrlConversionDlg::GetConversionPath(int idx)
{
	return m_ConversionModel[idx].szPath;
}

void C_CtrlConversionDlg::Open()
{
	//데이타 불러오기////////////////////////////
	CString str0, str1, path;
	typeModel mod;
	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;

	path.Empty();
	path.Format("%s\\NAME", m_szMainRegi);

	memset(&mod, NULL, sizeof(mod));

	//모든데이타 불러오기
	for(int i = (MAX_CON_LIST_NUM - 1); i >= 0; i--)
	{
		str0.Empty();
		str0.Format("%d", i);

		memset(&mod, NULL, sizeof(mod));

		//제품명
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szName))
		{
			//break;
		}

		//설명
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szDesc))
		{
			//break;
		}

		//경로명
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szPath))
		{
			//break;
		}

		sprintf(m_ConversionModel[i].szName, "%s", mod.szName);
		sprintf(m_ConversionModel[i].szDesc, "%s", mod.szDesc);
		sprintf(m_ConversionModel[i].szPath, "%s", mod.szPath);
	}

}
