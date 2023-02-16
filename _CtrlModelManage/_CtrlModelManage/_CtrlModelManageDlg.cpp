// _CtrlModelManageDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "_CtrlModelManage.h"

#include "include/_FuncModuleBasic.h"


// C_CtrlModelManageDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(C_CtrlModelManageDlg, CDialog)

C_CtrlModelManageDlg::C_CtrlModelManageDlg(CWnd* pParent/*=NULL*/, CString path/*=NULL*/, CString regi/*=NULL*/)
	//: CDialog(C_CtrlModelManageDlg::IDD, pParent)
	:CDialog(IDD_CTRLMODELMANAGE_DIALOG, pParent)
	, m_edit_name(_T(""))
	, m_edit_desc(_T(""))
{
	m_edit_name = _T("");
	m_edit_desc = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_iDlgType = 0;
	m_iModelIdx = -1;


	memset(m_szMainPath, NULL, sizeof(m_szMainPath));
	if(!path.IsEmpty())
		sprintf(m_szMainPath, (LPCTSTR)path);
	else
		sprintf(m_szMainPath, MOD_MODEL_PATH);

	memset(m_szMainRegi, NULL, sizeof(m_szMainRegi));
	if(!regi.IsEmpty())
		sprintf(m_szMainRegi, (LPCTSTR)regi);
	else
		sprintf(m_szMainRegi, MOD_MODEL_REGI);

	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	memset(&m_ConversionModel, NULL, sizeof(m_ConversionModel));
}

C_CtrlModelManageDlg::~C_CtrlModelManageDlg()
{
}

void C_CtrlModelManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_MODEL, m_list_model);
	DDX_Control(pDX, IDC_BTNENHCTRL_NAME, m_Name);
	DDX_Control(pDX, IDC_BTNENHCTRL_DESC, m_Desc);
	DDX_Text(pDX, IDC_EDIT_NAME, m_edit_name);
	DDX_Text(pDX, IDC_EDIT_DESC, m_edit_desc);
}


BEGIN_MESSAGE_MAP(C_CtrlModelManageDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_NAME, &C_CtrlModelManageDlg::OnEnChangeEditName)
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL, &C_CtrlModelManageDlg::OnNMDblclkListModel)
END_MESSAGE_MAP()


// C_CtrlModelManageDlg �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(C_CtrlModelManageDlg, CDialog)
	ON_EVENT(C_CtrlModelManageDlg, IDC_BTNENHCTRL_NEW, DISPID_CLICK, C_CtrlModelManageDlg::ClickBtnenhctrlNew, VTS_NONE)
	ON_EVENT(C_CtrlModelManageDlg, IDC_BTNENHCTRL_SELECT, DISPID_CLICK, C_CtrlModelManageDlg::ClickBtnenhctrlSelect, VTS_NONE)
	ON_EVENT(C_CtrlModelManageDlg, IDC_BTNENHCTRL_DEL, DISPID_CLICK, C_CtrlModelManageDlg::ClickBtnenhctrlDel, VTS_NONE)
	ON_EVENT(C_CtrlModelManageDlg, IDC_BTNENHCTRL_OK, DISPID_CLICK, C_CtrlModelManageDlg::ClickBtnenhctrlOk, VTS_NONE)
	ON_EVENT(C_CtrlModelManageDlg, IDC_BTNENHCTRL_CANCEL, DISPID_CLICK, C_CtrlModelManageDlg::ClickBtnenhctrlCancel, VTS_NONE)
END_EVENTSINK_MAP()

void C_CtrlModelManageDlg::ClickBtnenhctrlNew()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(IDYES == MessageBox("���ο� ���� �����ϰڽ��ϱ�?", NULL, MB_YESNO))
	{
		Model();
	}
}

void C_CtrlModelManageDlg::ClickBtnenhctrlSelect()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(IDYES == MessageBox("���õ� �𵨷� �����ϰڽ��ϱ�?", NULL, MB_YESNO))
	{
		Model();
	}
}

void C_CtrlModelManageDlg::ClickBtnenhctrlDel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(IDYES == MessageBox("���õ� ���� ����ڽ��ϱ�?", NULL, MB_YESNO | MB_ICONEXCLAMATION))
	{
		Model();
	}
}

void C_CtrlModelManageDlg::ClickBtnenhctrlOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CString path, str0, str1;
	int list_cnt, dupl_check;
	int i, idx;
	
	RegDeleteSubKey(HKEY_LOCAL_MACHINE, m_szMainRegi);

	//������Ʈ�� �����ϱ�
	//����Ʈ ����Ÿ
	dupl_check = 0;
	list_cnt = 0;
	list_cnt = m_list_model.GetItemCount();

	idx = list_cnt;
	for(i = 0; i < list_cnt; i++)
	{
		idx--;
		str0.Empty();
		str0.Format("%d", i);

		//��ǰ��
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		str1.Empty();
		str1 = m_list_model.GetItemText(idx,0);
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}

		//����
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		str1.Empty();
		str1 = m_list_model.GetItemText(idx,1);
		SetRegString(path, str0, str1);

		//��θ�
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		str1.Empty();
		str1 = m_list_model.GetItemText(idx,2);
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}
	}



	//������굥��Ÿ(null, ����Ʈ�� �����ϴ���)
	str0.Empty();
	str0 = m_CurrentModel.szName;
	BOOL chk = FALSE;
	if(!str0.IsEmpty())
	{
		list_cnt = 0;
		list_cnt = m_list_model.GetItemCount();
		for(i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_list_model.GetItemText(i, 0);
			if(0 == str0.Compare(str1))
			{
				str0.Empty();
				str0 = m_list_model.GetItemText(i, 1);
				sprintf(m_CurrentModel.szDesc, "%s", (LPCTSTR)str0);
				str0.Empty();
				str0 = m_list_model.GetItemText(i, 2);
				sprintf(m_CurrentModel.szPath, "%s", (LPCTSTR)str0);
				SelectList(i, TRUE);
				chk = TRUE;
				break;
			}
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}
	if(!chk)
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));		
	}




	//�������(�ε��� = -1)
	//��ǰ��
	path.Empty();
	path.Format("%s\\CURRENT", m_szMainRegi);
	str1.Empty();
	str1 = m_CurrentModel.szName;
	SetRegString(path, "NAME", str1);

	//����
	str1.Empty();
	str1 = m_CurrentModel.szDesc;
	SetRegString(path, "DESCRIPTION", str1);

	//��θ�
	str1.Empty();
	str1 = m_CurrentModel.szPath;
	SetRegString(path, "PATH", str1);

	switch(m_iDlgType)
	{
		//�ű�
		case 0:
			SetWindowText("Model-New");

			break;
		//���� 
		case 1:
			SetWindowText("Model-Delete");

			break;
		//����
		case 2:
			SetWindowText("Model-Select");
			m_Name.SetCaption(m_CurrentModel.szName);
			m_Desc.SetCaption(m_CurrentModel.szDesc);
			break;
		//Conversion
		case 3:
			SetWindowText("Conversion Model-Select");
			m_Name.SetCaption(m_CurrentModel.szName);
			m_Desc.SetCaption(m_CurrentModel.szDesc);
			break;
		default:
			AfxMessageBox("Error : invalid index!");
	}

	EndDialog(1);	
}

void C_CtrlModelManageDlg::ClickBtnenhctrlCancel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	EndDialog(0);
}


void C_CtrlModelManageDlg::Open()
{
	CString path;

	//������굥��Ÿ �ҷ�����
	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));

	//��ǰ��
	path.Empty();
	path.Format("%s\\CURRENT", m_szMainRegi);
	if(GetRegString(path, "NAME", (LPBYTE)m_CurrentModel.szName))
	{
		Sleep(0);
		//����
		if(!GetRegString(path, "DESCRIPTION", (LPBYTE)m_CurrentModel.szDesc))
		{
			memset(m_CurrentModel.szDesc, NULL, sizeof(m_CurrentModel.szDesc));
		}
		Sleep(0);
		//��θ�
		if(!GetRegString(path, "PATH", (LPBYTE)m_CurrentModel.szPath))
		{
			memset(m_CurrentModel.szPath, NULL, sizeof(m_CurrentModel.szPath));
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}
}

void C_CtrlModelManageDlg::Model()
{
	switch(m_iDlgType)
	{
		//�ű�
		case 0:
			Add();
			break;
		//���� 
		case 1:
			Remove();
			break;
		//����
		case 2:
			Select();
			break;
		//Conversion
		case 3:
			Conversion();
			break;
		default:
			AfxMessageBox("Error : invalid index!");
	}
}

//�⺻��θ� ����
void C_CtrlModelManageDlg::SetMainFolder(char* path)
{
	sprintf(m_szMainPath, "%s", path);
}

//�⺻�������Ͱ�� ����
void C_CtrlModelManageDlg::SetMainRegister(char* path)
{
	sprintf(m_szMainRegi, "%s", path);
}


//���������ǰ ��ǰ����
char *C_CtrlModelManageDlg::GetName()
{

	return m_CurrentModel.szName;
}

//���������ǰ ������
char *C_CtrlModelManageDlg::GetDesc()
{

	return m_CurrentModel.szDesc;
}

//���������ǰ ��θ���
char *C_CtrlModelManageDlg::GetPath()
{
	return m_CurrentModel.szPath;
}

//Conversion ��ǰ����
char *C_CtrlModelManageDlg::GetConversionName()
{

	return m_ConversionModel.szName;
}

//Conversion ������
char *C_CtrlModelManageDlg::GetConversionDesc()
{

	return m_ConversionModel.szDesc;
}

//Conversion ��θ���
char *C_CtrlModelManageDlg::GetConversionPath()
{
	return m_ConversionModel.szPath;
}

BOOL C_CtrlModelManageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int i;
	long setstyle;
	LV_COLUMN column;
	CImageList *img_list_slct;
	CBitmap bitmap;

	SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE); 

	img_list_slct = new CImageList;
	img_list_slct->Create(16, 16, ILC_COLOR4, 3, 3);

	bitmap.LoadBitmap(IDB_BITMAP_SELECTED);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_NOTSELECTED);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	m_list_model.SetImageList(img_list_slct, LVSIL_SMALL);

	setstyle = GetWindowLong(m_list_model.m_hWnd, GWL_STYLE);
	setstyle |= LVS_EDITLABELS;
	SetWindowLong(m_list_model.m_hWnd, GWL_STYLE, setstyle);

	m_list_model.RedrawWindow();


	char* list[4]={"��ǰ��", "����", "��θ�"};
	int width[4]={133, 300, 500};
	for(i=0; i<3; i++)
	{
		column.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		column.fmt=LVCFMT_LEFT;
		column.pszText=list[i];
		column.iSubItem=i;
		column.cx=width[i];
		m_list_model.InsertColumn(i, &column);
	}
	m_list_model.DeleteAllItems();

	//����Ÿ �ҷ�����////////////////////////////
	CString str0, str1, path;
	typeModel mod;
	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;
	int list_cnt, dupl_check;

	path.Empty();
	path.Format("%s\\NAME", m_szMainRegi);

	//��絥��Ÿ �ҷ�����
	int idx = 0;
	while(1)
	{
		str0.Empty();
		str0.Format("%d", idx);

		memset(&mod, NULL, sizeof(mod));

		//��ǰ��
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szName))
		{
			break;
		}

		//����
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szDesc))
		{
			break;
		}

		//��θ�
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szPath))
		{
			break;
		}

		//Null �� �ߺ� Ȯ��
		str0.Empty();
		str0 = mod.szName;
		str1.Empty();
		str1 = mod.szPath;
		if(!(str0.IsEmpty() || str1.IsEmpty()))
		{
			name_not_dupl = TRUE;
			//����Ʈ�� ���� �̸��� �����ϴ��� Ȯ��
			dupl_check = 0;
			list_cnt = 0;
			list_cnt = m_list_model.GetItemCount();
			for(i = 0; i < list_cnt; i++)
			{
				str1.Empty();
				str1 = m_list_model.GetItemText(i,0);
				dupl_check = str0.Compare(str1);
				if(0 == dupl_check)
				{
					name_not_dupl = FALSE;
					break;
				}
			}
			if(name_not_dupl)
			{
				//����Ʈ ����
				AddList(mod);
			}
		}

		idx++;
	}

	//������굥��Ÿ �ҷ�����
	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));


	//��ǰ��
	path.Empty();
	path.Format("%s\\CURRENT", m_szMainRegi);
	if(GetRegString(path, "NAME", (LPBYTE)m_CurrentModel.szName))
	{
		//����
		if(!GetRegString(path, "DESCRIPTION", (LPBYTE)m_CurrentModel.szDesc))
		{
			memset(m_CurrentModel.szDesc, NULL, sizeof(m_CurrentModel.szDesc));
		}

		//��θ�
		if(!GetRegString(path, "PATH", (LPBYTE)m_CurrentModel.szPath))
		{
			memset(m_CurrentModel.szPath, NULL, sizeof(m_CurrentModel.szPath));
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}

	//������굥��Ÿ(null, ����Ʈ�� �����ϴ���)
	str0.Empty();
	str0 = m_CurrentModel.szName;
	if(!str0.IsEmpty())
	{
		list_cnt = 0;
		list_cnt = m_list_model.GetItemCount();
		for(i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_list_model.GetItemText(i, 0);
			if(0 == str0.Compare(str1))
			{
				str0.Empty();
				str0 = m_list_model.GetItemText(i, 1);
				sprintf(m_CurrentModel.szDesc, "%s", (LPCTSTR)str0);
				str0.Empty();
				str0 = m_list_model.GetItemText(i, 2);
				sprintf(m_CurrentModel.szPath, "%s", (LPCTSTR)str0);
				SelectList(i, TRUE);
				break;
			}
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}

	/////////////////////////////////////////////
	int list_num = 0;
	char szTime[30]; 
	SYSTEMTIME	lpSystemTime;

	GetLocalTime(&lpSystemTime);
	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%04d/%02d/%02d[%02d:%02d:%02d]", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);

	switch(m_iDlgType)
	{
		//�ű�
		case 0:
			SetWindowText("Model-New");
			GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_NAME)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_DESC)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BTNENHCTRL_NEW)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_DEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_SELECT)->ShowWindow(SW_HIDE);
			break;
		//���� 
		case 1:
			SetWindowText("Model-Delete");
			GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_NAME)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_DESC)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_BTNENHCTRL_NEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_DEL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_SELECT)->ShowWindow(SW_HIDE);
			break;
		//����
		case 2:
			SetWindowText("Model-Select");
			GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_NAME)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_DESC)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_BTNENHCTRL_NEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_DEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_SELECT)->ShowWindow(SW_SHOW);

			m_Name.SetCaption(m_CurrentModel.szName);
			m_Desc.SetCaption(m_CurrentModel.szDesc);
			break;
		//Conversion
		case 3:
			SetWindowText("Conversion Model-Select");
			GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_NAME)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTNENHCTRL_DESC)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_BTNENHCTRL_NEW)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_DEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_SELECT)->ShowWindow(SW_SHOW);

			m_Name.SetCaption(m_CurrentModel.szName);
			m_Desc.SetCaption(m_CurrentModel.szDesc);
			break;
		default:
			AfxMessageBox("Error : invalid index!");
	}
	


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

INT_PTR C_CtrlModelManageDlg::DoModal(int type)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_iDlgType = type;

	return CDialog::DoModal();
}

void C_CtrlModelManageDlg::OnEnChangeEditName()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int invalid_cnt;
	invalid_cnt = 0;
	CString name;

	name = m_Name.GetCaption();

	UpdateData(TRUE);

	if(0 <= name.Find('\\', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('/', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find(':', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('*', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('?', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('"', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('<', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('>', 1))
	{
		invalid_cnt++;
	}

	if(0 <= name.Find('|', 1))
	{
		invalid_cnt++;
	}


	int leng = 0;
	if(0 < invalid_cnt)
	{
		MessageBox("Error : ��ǰ�� ���� ���ڸ� ����� �� �����ϴ�! \n \\ / : * ? \" < > |", "���", MB_OK | MB_ICONEXCLAMATION);
		leng = name.GetLength();
		name.Delete(leng - 1, 1);
		UpdateData(FALSE);
	}
}

void C_CtrlModelManageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rect;
	CPen pen,*oldPen;
	CBrush brush,*oldBrush;

	GetClientRect(&rect);
	
	//����� ä���
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


//Conversion
void C_CtrlModelManageDlg::Conversion()
{
	typeModel mod;
	CString str0, str1;
	int list_cnt, dupl_check;
	int i;

	UpdateData(TRUE);

	sprintf(mod.szName, "%s", (LPCTSTR)m_Name.GetCaption());
	sprintf(mod.szDesc, "%s", (LPCTSTR)m_Desc.GetCaption());
	sprintf(mod.szPath, "%s\\%s", m_szMainPath, mod.szName);

	//�̸��� Null���� Ȯ�� 
	str0 = mod.szName;
	if(str0.IsEmpty())
	{
		MessageBox("Error : ��ǰ���� �Էµ��� �ʾҽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	//����Ʈ�� ���� �̸� ã��
	BOOL find_check = FALSE;
	dupl_check = 0;
	list_cnt = 0;
	list_cnt = m_list_model.GetItemCount();
	for(i = 0; i < list_cnt; i++)
	{
		str1.Empty();
		str1 = m_list_model.GetItemText(i,0);
		dupl_check = str0.Compare(str1);
		if( (0 == dupl_check) && !find_check)
		{
			//��ǰ��
			sprintf(m_ConversionModel.szName, "%s", m_list_model.GetItemText(i,0));
			//����
			sprintf(m_ConversionModel.szDesc, "%s", m_list_model.GetItemText(i,1));
			//���
			sprintf(m_ConversionModel.szPath, "%s", m_list_model.GetItemText(i,2));

			SelectList(i, TRUE);

			find_check = TRUE;
		}
		else
		{
			SelectList(i, FALSE);
		}
	}

	if(!find_check)
	{
		MessageBox("Error : ��ǰ���� �������� �ʽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
	}
}


void C_CtrlModelManageDlg::RemoteSelect(CString modelname)
{

}

//��ǰ����
void C_CtrlModelManageDlg::Select()
{
	typeModel mod;
	CString str0, str1;
	int list_cnt, dupl_check;
	int i;

	UpdateData(TRUE);

	sprintf(mod.szName, "%s", (LPCTSTR)m_Name.GetCaption());
	sprintf(mod.szDesc, "%s", (LPCTSTR)m_Desc.GetCaption());
	sprintf(mod.szPath, "%s\\%s", m_szMainPath, mod.szName);

	//�̸��� Null���� Ȯ�� 
	str0 = mod.szName;
	if(str0.IsEmpty())
	{
		MessageBox("Error : ��ǰ���� �Էµ��� �ʾҽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	//����Ʈ�� ���� �̸� ã��
	BOOL find_check = FALSE;
	dupl_check = 0;
	list_cnt = 0;
	list_cnt = m_list_model.GetItemCount();
	for(i = 0; i < list_cnt; i++)
	{
		str1.Empty();
		str1 = m_list_model.GetItemText(i,0);
		dupl_check = str0.Compare(str1);
		if( (0 == dupl_check) && !find_check)
		{
			//��ǰ��
			sprintf(m_CurrentModel.szName, "%s", m_list_model.GetItemText(i,0));
			//����
			sprintf(m_CurrentModel.szDesc, "%s", m_list_model.GetItemText(i,1));
			//���
			sprintf(m_CurrentModel.szPath, "%s", m_list_model.GetItemText(i,2));

			SelectList(i, TRUE);

			find_check = TRUE;
		}
		else
		{
			SelectList(i, FALSE);
		}
	}

	if(!find_check)
	{
		MessageBox("Error : ������ ��ǰ���� �������� �ʽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
	}
}


BOOL C_CtrlModelManageDlg::SelectList(int idx, BOOL on_off)
{
	CString str;
	LV_ITEM lvitem;

	str.Empty();
	str = m_list_model.GetItemText(idx, 0);

	lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem=idx;
	lvitem.iSubItem=0;
	lvitem.iImage = !(on_off);
	lvitem.pszText=(char*)((LPCTSTR)str);
	m_list_model.SetItem(&lvitem);

	return TRUE;
}



BOOL C_CtrlModelManageDlg::SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, LPCTSTR lpintValue)
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

//Get registry
BOOL C_CtrlModelManageDlg::GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue)
{
    HKEY hkResult;
    DWORD dwLength = MAX_PATH;	//20191217 by Release Mode ������ �ݵ�� �ʱ�ȭ�� ���־�� �Ѵ�... �׷��� ������.....
    DWORD dwType = 0;
 
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


//������Ʈ�� ���λ���(���� ������Ʈ������ ����)
int C_CtrlModelManageDlg::RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey)
{
    // RegDeleteKey() �Լ��� NT���� ����Ű�� ������ ������ �ȵȴ�.
    // �׷��� ����Ű���� ��� �������ִ� �Լ��� �������.

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
        // ��� Ű�� ���� �ǹǷ� dwIndex�� �׻� 0�� �־��־�� �Ѵ�.
        // ���� for������ i�� �����ø� ����ϸ� �ϳ������ �ϳ� �پ�Ѿ� ���� ���´�.
        Result = RegEnumKeyEx(newKey, 0, newSubKey, &Size, NULL, NULL, NULL, &FileTime);
        if (Result != ERROR_SUCCESS) break;
        Result = RegDeleteSubKey(newKey, newSubKey);
        if (Result  != ERROR_SUCCESS) break;
    }
    RegCloseKey(newKey);

    return RegDeleteKey(hKey, lpSubKey);
}


//�űԵ��
void C_CtrlModelManageDlg::Add()
{
	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;
	typeModel mod;
	CString str0, str1;
	int list_cnt, dupl_check;
	int i;

	UpdateData(TRUE);

	sprintf(mod.szName, "%s", (LPCTSTR)m_edit_name);
	sprintf(mod.szDesc, "%s", (LPCTSTR)m_edit_desc);
	sprintf(mod.szPath, "%s\\%s", m_szMainPath, mod.szName);

	//�̸��� Null���� Ȯ�� 
	str0 = mod.szName;
	if(str0.IsEmpty())
	{
		name_not_null = FALSE;
	}

	//����Ʈ�� ���� �̸��� �����ϴ��� Ȯ��
	dupl_check = 0;
	list_cnt = 0;
	list_cnt = m_list_model.GetItemCount();
	for(i = 0; i < list_cnt; i++)
	{
		str1.Empty();
		str1 = m_list_model.GetItemText(i,0);
		dupl_check = str0.Compare(str1);
		if(0 == dupl_check)
		{
			name_not_dupl = FALSE;
			break;
		}
	}


	if(name_not_null && name_not_dupl)
	{
		RemoveFileSRecurse((LPCTSTR)mod.szPath, -1);
		RemoveDirectory((LPCTSTR)mod.szPath);
		AddList(mod);//1. Null�̸� ����, 2. ���� �̸� �ִ��� Ȯ�� �� ������ ��� ����
	}

	if(!name_not_null)
	{
		MessageBox("Error : ��ǰ���� �Էµ��� �ʾҽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
	}

	if(!name_not_dupl)
	{
		MessageBox("Error : �űԵ�� ��ǰ���� �̹� ���� �մϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
	}

}


//��ǰ����
void C_CtrlModelManageDlg::Remove()
{
	if(0 > m_iModelIdx)
	{	
		MessageBox("Error : ��ǰ�� ���õ��� �ʾҽ��ϴ�!", "���", MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		m_list_model.DeleteItem(m_iModelIdx);
	}
}


//�𵨸���Ʈ�� �߰�
BOOL C_CtrlModelManageDlg::AddList(typeModel mod)
{
	LV_ITEM lvitem;

	lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem=0;
	lvitem.iSubItem=0;
	lvitem.iImage = 1;
	lvitem.pszText=mod.szName;
	m_list_model.InsertItem(&lvitem);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=1;
	lvitem.pszText=mod.szDesc;
	m_list_model.SetItem(&lvitem);

	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=0;
	lvitem.iSubItem=2;
	lvitem.pszText=mod.szPath;
	m_list_model.SetItem(&lvitem);

	return TRUE;
}

void C_CtrlModelManageDlg::OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iModelIdx = 0;
	CString str;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_iModelIdx = pNMListView->iItem;

	str.Empty();

	//��ǰ��
	str = m_list_model.GetItemText(m_iModelIdx, 0);
	m_Name.SetCaption(str);

	//����
	str = m_list_model.GetItemText(m_iModelIdx, 1);
	m_Desc.SetCaption(str);

	*pResult = 0;
}


//�������� ���� �𵨺���
BOOL C_CtrlModelManageDlg::RemoteModel(CString name, CString desc)
{
	BOOL rslt = TRUE;

	CString path, str0, str1, mod_path;
	int dupl_check;
	int idx;


	//����Ÿ �ҷ�����////////////////////////////
	typeModel mod;
	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;

	path.Empty();
	path.Format("%s\\NAME", m_szMainRegi);

	//��絥��Ÿ �ҷ�����
	idx = 0;
	while(1)
	{
		str0.Empty();
		str0.Format("%d", idx);

		memset(&mod, NULL, sizeof(mod));

		//��ǰ��
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szName))
		{
			break;
		}

		//����
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szDesc))
		{
			break;
		}

		//��θ�
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		if(!GetRegString(path, str0, (LPBYTE)mod.szPath))
		{
			break;
		}

		//Null �� �ߺ� Ȯ��
		str0.Empty();
		str0 = mod.szName;
		str1.Empty();
		str1 = mod.szPath;
		if(!(str0.IsEmpty() || str1.IsEmpty()))
		{
			name_not_dupl = TRUE;
			//����Ʈ�� ���� �̸��� �����ϴ��� Ȯ��
			dupl_check = 0;


			str1.Empty();
			str1 = name;
			dupl_check = str0.Compare(str1);
			if(0 == dupl_check)
			{
				name_not_dupl = FALSE;
				break;
			}
		}

		idx++;
	}


	mod_path.Format("%s\\%s", m_szMainPath, name);

	sprintf(m_CurrentModel.szName, "%s", (LPCTSTR)name);
	sprintf(m_CurrentModel.szDesc, "%s", (LPCTSTR)desc);
	sprintf(m_CurrentModel.szPath, "%s", (LPCTSTR)mod_path);

	if(name_not_dupl)
	{
		str0.Empty();
		str0.Format("%d", idx);

		//��ǰ��
		path.Empty();
		path.Format("%s\\NAME", m_szMainRegi);
		str1.Empty();
		str1 = name;
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}

		//����
		path.Empty();
		path.Format("%s\\DESCRIPTION", m_szMainRegi);
		str1.Empty();
		str1 = desc;
		SetRegString(path, str0, str1);

		//��θ�
		path.Empty();
		path.Format("%s\\PATH", m_szMainRegi);
		str1.Empty();
		str1 = mod_path;
		if(!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}
	}


	//�������(�ε��� = -1)
	//��ǰ��
	path.Empty();
	path.Format("%s\\CURRENT", m_szMainRegi);
	str1.Empty();
	str1 = m_CurrentModel.szName;
	SetRegString(path, "NAME", str1);

	//����
	str1.Empty();
	str1 = m_CurrentModel.szDesc;
	SetRegString(path, "DESCRIPTION", str1);

	//��θ�
	str1.Empty();
	str1 = m_CurrentModel.szPath;
	SetRegString(path, "PATH", str1);

	return rslt;
}

