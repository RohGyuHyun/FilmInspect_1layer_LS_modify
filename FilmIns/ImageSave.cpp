#include "StdAfx.h"
#include "ImageSave.h"

CImageSave::CImageSave(void)
{
	m_bParaListAllSave = FALSE;
	//m_bSaveThreadEnd = TRUE;
	//m_pThreadSave = NULL;
	InitThread();
}

CImageSave::~CImageSave(void)
{
	ReleaseThread();
}

void CImageSave::SetSavePara(typeImageSavePara SavePara)
{
	typeImageSavePara sSavePara = SavePara;
	//copy_image(*Img, &SavePara.hSaveImage);
	//SavePara.hSaveImage = *Img;
	m_SavePara.push_back(sSavePara);

	//if(m_bSaveThreadEnd)
	//	InitThread();
}

void CImageSave::InitThread()
{
	//if(m_pThreadSave != NULL)
	//	ReleaseThread();

	m_pThreadSave = NULL;
	m_bThreadStart = TRUE;
	m_bThreadEnd = FALSE;
	//m_bSaveThreadEnd = FALSE;

	m_pThreadSave = AfxBeginThread(SaveThread, this, THREAD_PRIORITY_NORMAL);
	m_pThreadSave->m_bAutoDelete = FALSE;
	//m_pThreadSave->ResumeThread();
}

BOOL CImageSave::ReleaseThread()
{
	BOOL rslt = FALSE;
	
	m_bThreadStart = FALSE;
	m_bThreadEnd = TRUE;

	while(TRUE)
	{
		if(m_bParaListAllSave)
		{
			if(!m_bParaListAllSaveEnd)
				continue;
		}

		rslt = WaitThreadEnd();

		if(rslt)
		{
			m_SavePara.clear();

			if(m_pThreadSave)
				delete m_pThreadSave;

			m_pThreadSave = NULL;
			break;
		}
		Sleep(10);
	}

	return rslt;
}

BOOL CImageSave::WaitThreadEnd()
{
	BOOL rslt = TRUE;
	if(m_pThreadSave != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadSave->m_hThread, DEFAULT_TIMEOUT);

		if(dwRet == WAIT_FAILED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			rslt = FALSE;
		}
	}

	return rslt;
}

UINT CImageSave::SaveThread(LPVOID pParam)
{
	CImageSave *pdlg = (CImageSave *)pParam;

	pdlg->SaveThread();

	return 0;
}

void CImageSave::SaveThread()
{
	while(TRUE)
	{
		if(m_bThreadStart && !m_bThreadEnd)
		{
			if(m_SavePara.size() > 0)
			{
				SavePara();
			}
			/*
			else
			{
				m_bSaveThreadEnd = TRUE;
				break;
			}
			*/
		}

		if(!m_bThreadStart && m_bThreadEnd)
		{
			if(m_bParaListAllSave)
			{
				if(m_SavePara.size() > 0)
				{
					SavePara();
				}
				else
				{
					//m_bSaveThreadEnd = TRUE;
					m_bParaListAllSaveEnd = TRUE;
					break;
				}
			}
			else
			{
				break;
			}
		}
/*
		if(m_bSaveThreadEnd)
			break;
*/
		Sleep(1);
	}
}

void CImageSave::SavePara()
{
	//Herror herr;
	typeImageSavePara SavePara = m_SavePara.front();


	switch(SavePara.nImageType)
	{
	case 0:
		//SaveImage
		set_check("~give_error");

		write_image(SavePara.hSaveImage, "bmp", 0, SavePara.strPath);

		set_check("give_error");
		break;

	case 1:
		set_check("~give_error");

		write_image(SavePara.hSaveImage, "jpeg 50", 0, SavePara.strPath);

		set_check("give_error");
		break;
	}
	
	m_SavePara.pop_front();
}