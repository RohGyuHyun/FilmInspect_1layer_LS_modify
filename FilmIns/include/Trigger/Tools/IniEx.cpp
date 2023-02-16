#include "stdafx.h"
#include "IniEx.h"


CIniEx::CIniEx()
{
	CString strPath;

	::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();

 	if (strPath.Find('\\') != -1)
	{
		for (int i = strPath.GetLength() - 1; i >= 0; i--)
		{
			TCHAR ch = strPath[i];
			strPath.Delete(i);
			if (ch == '\\')
				break;	
		}
	}

	m_ini.SetPathName(strPath + _T("\\VisitionTrigger.ini"));
}

CIniEx::~CIniEx()
{
}

CString CIniEx::GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault)		
{
	return m_ini.GetString(lpSection, lpKey, lpDefault);
}

BOOL CIniEx::WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue)		
{
	return m_ini.WriteString(lpSection, lpKey, lpValue);
}

int CIniEx::GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault)
{
	return m_ini.GetInt(lpSection, lpKey, nDefault);
}

BOOL CIniEx::WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue)
{
	return m_ini.WriteInt(lpSection, lpKey, nValue);
}

double CIniEx::GetDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fDefault)
{
	return m_ini.GetDouble(lpSection, lpKey, fDefault);
}

BOOL CIniEx::WriteDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fValue)
{
	return m_ini.WriteDouble(lpSection, lpKey, fValue);
}
