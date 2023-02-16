#pragma once

#include "ini.h"

class CIniEx
{
public:
	CIniEx();
	~CIniEx();

	CIni m_ini;

	CString GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault = NULL);
	BOOL WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue);

	int GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault);
	BOOL WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue);

	double GetDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fDefault);
	BOOL WriteDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fValue);
};
