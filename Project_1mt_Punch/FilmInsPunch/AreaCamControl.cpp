#include "StdAfx.h"
#include "AreaCamControl.h"

CAreaCamControl::CAreaCamControl(void)
{

}

CAreaCamControl::~CAreaCamControl(void)
{

}


BOOL CAreaCamControl::Open()
{
	CString filter;
	filter.Format("acA1300-30gm");

	m_sCamManager.SetRemoveTime( 5 ); //1분

	INT8 m_Error = 0;

	if(m_Error == m_sCamManager.Open_Camera(INFO_MODEL_NAME, filter))
	{
		m_sCamManager.Grab_Camera(TRUE);
		//성공
		return TRUE;
	}

	return FALSE;
}

BOOL CAreaCamControl::Close()
{
	m_sCamManager.Close_Camera();

	return TRUE;
}

BOOL CAreaCamControl::Grab(Hobject *obj_image)
{
	
	if( !m_sCamManager.IsOpen() ) 
		return FALSE;
	
	if( !m_sCamManager.IsGrabbing() )
		return FALSE;
	
	if( m_sCamManager.pImageBuffer == NULL )
		return FALSE;

	Herror herr;

	gen_empty_obj(&m_halGrabBuf);	// Hobject 변수 초기화. 이거 안해주면 망한다. 

	set_check("~give_error");
	herr = gen_image1_extern(&m_halGrabBuf, "byte", CAM_WIDTH, CAM_HEIGHT, (Hlong)m_sCamManager.pImageBuffer, (Hlong)0);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{
		copy_image(m_halGrabBuf, obj_image);
	}
	else
	{
		return FALSE;
	}
	

	return TRUE;
}