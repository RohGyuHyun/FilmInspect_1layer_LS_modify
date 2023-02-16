#pragma once

#include "EthernetCtrl.h"
#include "IniEx.h"


typedef int(*CallBackCtrl)(void *pSrc, void *pData);

#define CMD_NULL 0
#define DEFAULT_ACC 100;


#define MAX_BL_SIZE			24000			// Bootloader firmware size
#define MAX_APP_SIZE		256000			// Application firmware size

typedef struct
{
	Uint16 Cmd;
	BYTE Group, Axis, No, retry;
	Uint16 Index, Total;
	DWORD time;
}SCmdMsgStatus;

class CUCP : public CEthernetCtrl
{
public:
	CUCP();
	~CUCP();

public:
	CIniEx m_ini;	
	CString m_sAppPath;
	
	SUCPSRC			m_Target;
	SProductInfo	m_ProductInfo;	
	SVisionTrigger	m_VisionTrigger;

	BOOL Initialize(CallBackCtrl pCallBack);
	void Terminate();

	void GetProductInfo();
	void GetTrigStatus();

	void SetLineTrigCntReset();	// add ky 2014.08.18

	void SetTrigPosReset();
	void SetTrigOneShot();
	void SetSaveTrig();

	void GetTrigConfig();
	void SetTrigSConfig(STrigConfig *pData);

	void GetPosTrig();
	void SetPosTrig(SPositionTrigger *pData);

	void GetLineTrig();
	void SetLineTrig(SLineScanTrigger *pData);

	void GetFusionTrig();
	void SetFusionTrig(SFusionTrigger *pData);

	void GetVisionTrig();
	void SetVisionTrig(SVisionTrigger *pData);

	static UINT16 ConvPulse(float time);
	static float ConvTime(UINT16 pulse);

public:
	virtual void OnDataReceived(SUCPSRC *pSrc, SUCPMsg* pMsg);

private:
	BOOL SendMsg(BYTE Axis, Uint16 MsgCmd, short MsgSize=0, BYTE *Buffer=NULL);

	HANDLE		m_hSendingThread;
	static UINT WINAPI SocketSendingThreadProc(LPVOID pParam);

};



