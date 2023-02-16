// LineScanCamControl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "LineScanCamControl.h"

CLineScanCamControl::CLineScanCamControl()
{
	m_SizeX = 0;
	m_SizeY = 0;

	m_SapAcq = NULL;
	m_SapXfer = NULL;
	m_SapAcqDevice = NULL;
	m_SapBuffer_Grab = NULL;
	m_SapBuffer_Red = NULL;
	m_SapBuffer_Green = NULL;
	m_SapBuffer_Blue = NULL;

	b_ThreadControl = FALSE;
}

CLineScanCamControl::~CLineScanCamControl()
{
	m_PtrRed = NULL;
	m_PtrGreen = NULL;
	m_PtrBlue = NULL;

	m_SapAcq = NULL;
	m_SapXfer = NULL;
	m_SapAcqDevice = NULL;
	m_SapBuffer_Grab = NULL;
	m_SapBuffer_Red = NULL;
	m_SapBuffer_Green = NULL;
	m_SapBuffer_Blue = NULL;
}

void CLineScanCamControl::XferCallback(SapXferCallbackInfo *pInfo)
{
	CLineScanCamControl *pDlg=(CLineScanCamControl*) pInfo->GetContext();
/*
	pDlg->m_SapBuffer_Split_Mono8->SplitComponents(pDlg->m_SapBuffer_Grab);
	pDlg->m_SapBuffer_Red->Copy(pDlg->m_SapBuffer_Split_Mono8, 0, 0);9  0
	pDlg->m_SapBuffer_Green->Copy(pDlg->m_SapBuffer_Split_Mono8, 1, 0);
	pDlg->m_SapBuffer_Blue->Copy(pDlg->m_SapBuffer_Split_Mono8, 2, 0);
*/
	pDlg->SetThreadControl(true);

	SendMessage(pDlg->m_pHwnd, USER_MSG_LINESCAN_MODULE, 0, 0);	//조명 OFF
}

BOOL CLineScanCamControl::Open_LineScanCam(HWND hwnd, BOOL isColor)
{
	BOOL rslt = TRUE;
	void *temp_Ptr = NULL;
	
	m_pHwnd = hwnd;

	// 보드 객체 생성
	m_SapAcq =new SapAcquisition(SapLocation("Xtium-CLHS_PX8_1", 1), "ccf\\trigger.ccf");		//경로 20190
	m_SapAcq->Create(); 

	// 보드 파라미터 읽어오기
	m_SapAcq->GetParameter(CORACQ_PRM_CROP_WIDTH, &m_SizeX);   // 버퍼 width 가져옴 -> SizeX 
	m_SapAcq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &m_SizeY);  // 버퍼 height 가져옴 -> SizeY

	 // 버퍼 객체 생성(PC메모리)
	m_SapBuffer_Grab = new SapBuffer(GRAB_FRAME_NUM, m_SizeX, m_SizeY, SapFormatRGB888/*ccf와 일치하는 포맷*/, SapBuffer::TypeScatterGather);// RGB888 타입 버퍼, '.ccf' 파일 설정과 일치하는 것 사용
	m_SapBuffer_Grab->Create();

//	m_SapBuffer_Split_Mono8 = new SapBuffer(3, m_SizeX, m_SizeY, SapFormatMono8, SapBuffer::TypeScatterGather);
//	m_SapBuffer_Split_Mono8->Create();

	m_SapBuffer_Red = new SapBuffer(GRAB_FRAME_NUM, m_SizeX, m_SizeY, SapFormatMono8, SapBuffer::TypeScatterGather);
	m_SapBuffer_Red->Create();
	//m_SapBuffer_Red->GetAddress(&temp_Ptr);
	//m_PtrRed = (BYTE*)temp_Ptr;

	m_SapBuffer_Green = new SapBuffer(GRAB_FRAME_NUM, m_SizeX, m_SizeY, SapFormatMono8, SapBuffer::TypeScatterGather);
	m_SapBuffer_Green->Create();
	//m_SapBuffer_Green->GetAddress(&temp_Ptr);
	//m_PtrGreen = (BYTE*)temp_Ptr;

	m_SapBuffer_Blue = new SapBuffer(GRAB_FRAME_NUM, m_SizeX, m_SizeY, SapFormatMono8, SapBuffer::TypeScatterGather);
	m_SapBuffer_Blue->Create();
	//m_SapBuffer_Blue->GetAddress(&temp_Ptr);
	//m_PtrBlue = (BYTE*)temp_Ptr;

	// 보드로 받은 데이터 -> 메모리 버퍼로 전송
	m_SapXfer=new SapAcqToBuf(m_SapAcq/*보드*/, m_SapBuffer_Grab/*버퍼*/, XferCallback/*콜백*/, this);
	m_SapXfer->Create();

	// 디바이스 컨트롤 객체 생성
	m_SapAcqDevice = new SapAcqDevice(SapLocation("Xtium-CLHS_PX8_1"), false); // 보드를 경유하여 컨트롤
	m_SapAcqDevice->Create();

	m_SapAcqDevice->SetFeatureValue("TriggerMode", "External");       // internal 트리거 모드 
	m_SapAcqDevice->SetFeatureValue("ExposureTime", 19.0); // expTime 설정
	
	m_SapAcqDevice->SetFeatureValue("GainSelector", "Red"); // GAIN 설정
	m_SapAcqDevice->SetFeatureValue("Gain", 2.5); // GAIN 설정

	m_SapAcqDevice->SetFeatureValue("GainSelector", "Green"); // GAIN 설정
	m_SapAcqDevice->SetFeatureValue("Gain", 1.0); // GAIN 설정
	
	m_SapAcqDevice->SetFeatureValue("SensorScanDirection","Forward"); // 스캔방향 설정	왕복촬상 시 수시로 변경해줘야함. by



	m_SapXfer->Abort();

	temp_Ptr = NULL;

	return rslt;
}

void CLineScanCamControl::Close_LineScanCam()
{	
	//정상적으로 해제를 하여도 메모리 릭으로 감지됨... 시간될 때 알아볼 것.
	m_SapXfer->Destroy();
	m_SapBuffer_Blue->Destroy();
	m_SapBuffer_Green->Destroy();
	m_SapBuffer_Red->Destroy();
	//m_SapBuffer_Split_Mono8->Destroy();
	m_SapBuffer_Grab->Destroy();
	m_SapAcq->Destroy();
	m_SapAcqDevice->Destroy();
}

void CLineScanCamControl::Grab(int cam_idx, int buf_idx, int img_idx, Hobject *rotate_halcon_object, Hobject *red_halcon_object, Hobject *green_halcon_object, Hobject *blue_halcon_object)
{
	Hobject MultiChannelImage;
	void *temp_Ptr = NULL;

	/*
	if(0 == (img_idx % 2))
		m_SapAcqDevice->SetFeatureValue("SensorScanDirection","Forward");
	else
		m_SapAcqDevice->SetFeatureValue("SensorScanDirection","Reverse");
	*/

	m_SapXfer->Snap();

	while(TRUE)
	{
		if(b_ThreadControl)	
		{
			b_ThreadControl = FALSE;
			break;
		}
		
		Sleep(1);
	}
/*
	m_SapBuffer_Grab->SetIndex(buf_idx);
	m_SapBuffer_Red->SetIndex(buf_idx);
	m_SapBuffer_Green->SetIndex(buf_idx);
	m_SapBuffer_Blue->SetIndex(buf_idx);
*/
	m_SapBuffer_Grab->SplitComponents(m_SapBuffer_Red, m_SapBuffer_Green, m_SapBuffer_Blue);

	m_SapBuffer_Red->GetAddress(&temp_Ptr);
	m_PtrRed = (BYTE*)temp_Ptr;

	m_SapBuffer_Green->GetAddress(&temp_Ptr);
	m_PtrGreen = (BYTE*)temp_Ptr;
	
	m_SapBuffer_Blue->GetAddress(&temp_Ptr);
	m_PtrBlue = (BYTE*)temp_Ptr;

	gen_image3_extern(&MultiChannelImage, "byte", m_SizeX, m_SizeY, (Hlong)m_PtrRed, (Hlong)m_PtrGreen, (Hlong)m_PtrBlue, (Hlong)0);

	//if(1 == (img_idx % 2))
		//mirror_image(MultiChannelImage, &MultiChannelImage, "row");	//180ms

	copy_image(MultiChannelImage, rotate_halcon_object);			//400ms

	decompose3(*rotate_halcon_object, red_halcon_object, green_halcon_object, blue_halcon_object);
}

void CLineScanCamControl::SetBufIndex()
{
	//m_SapBuffer_Grab->SetIndex(0);
}