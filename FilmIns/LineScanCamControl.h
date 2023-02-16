#pragma once

#include "SapClassBasic.h"


#define		USER_MSG_LINESCAN_MODULE			WM_USER+100


#define		LINE_CAM_WIDTH				16384
#define		LINE_CAM_HEIGHT				21150	// 84.6mm 
#define		LINE_CAM_PIXELSIZE			4		// um			63mm/3.8um

#define		GRAB_FRAME_NUM				1

#define     LINE_CAM_MAX_BD_NUM			1
#define		LINE_CAM_MAX_CAM_NUM		1
#define     LINE_CAM_MAX_IMG_NUM		2
#define     LINE_CAM_MAX_BUF_NUM		2

class CLineScanCamControl
{
public:
	HWND m_pHwnd;

	SapAcquisition  *m_SapAcq;
	SapTransfer		*m_SapXfer;
	SapAcqDevice    *m_SapAcqDevice;

	SapBuffer		*m_SapBuffer_Grab;
	SapBuffer		*m_SapBuffer_Red;
	SapBuffer		*m_SapBuffer_Green;
	SapBuffer		*m_SapBuffer_Blue;
	SapBuffer		*m_SapBuffer_Split_Mono8;

	BYTE			*m_PtrRed;
	BYTE			*m_PtrGreen;
	BYTE			*m_PtrBlue;

	int				m_SizeX;
	int				m_SizeY;

public:
	bool b_ThreadControl;

public:
	CLineScanCamControl();
	~CLineScanCamControl();

	static void XferCallback(SapXferCallbackInfo *pInfo);

	BOOL Open_LineScanCam(HWND hwnd, BOOL isColor = FALSE);
	void Grab(int cam_idx, int buf_idx, int img_idx, Hobject *rotate_halcon_object, Hobject *red_halcon_object, Hobject *green_halcon_object, Hobject *blue_halcon_object);
	void SetBufIndex();

	void Close_LineScanCam();
	void SetThreadControl(bool bsts) { b_ThreadControl = bsts; }
	bool GetThreadContorl() { return b_ThreadControl; }
};