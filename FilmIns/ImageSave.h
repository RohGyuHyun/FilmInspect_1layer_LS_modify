#pragma once

#define DEFAULT_TIMEOUT				5000

typedef struct typeImageSavePara
{
	Hobject hSaveImage;
	char strPath[512];
	int nImageType;		// 0: bmp 1: jpg

}TypeImageSavePara;


class CImageSave
{
public:
	CImageSave(void);
	~CImageSave(void);

protected:
	list<typeImageSavePara> m_SavePara;
	CWinThread* m_pThreadSave;
	BOOL m_bThreadStart;
	BOOL m_bThreadEnd;
	BOOL m_bParaListAllSave;
	BOOL m_bParaListAllSaveEnd;
	BOOL m_bSaveThreadEnd;
public:
	void SetSavePara(typeImageSavePara SavePara);
	void SetParaListAllSave(BOOL isSave){m_bParaListAllSave = isSave;};
	BOOL GetParaListAllSave(){return m_bParaListAllSave;};
private:
	void InitThread();
	BOOL ReleaseThread();
	BOOL WaitThreadEnd();
	UINT static SaveThread(LPVOID pParam);
	void SaveThread();
	void SavePara();
};
