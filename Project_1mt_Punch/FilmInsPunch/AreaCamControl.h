#pragma once
#include "CameraManager.h"

#define CAM_WIDTH	1280
#define CAM_HEIGHT	960

class CAreaCamControl
{
public:
	CAreaCamControl(void);
	~CAreaCamControl(void);

protected:
	CCameraManager m_sCamManager;
	Hobject m_halGrabBuf;

public:
	BOOL Open();
	BOOL Close();
	BOOL Grab(Hobject *obj_image);

	void GrabStart() { m_sCamManager.Grab_Camera(TRUE);}
	void GrabEnd() { m_sCamManager.Grab_Camera(FALSE);}

};
