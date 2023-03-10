#include "StdAfx.h"
#include "CameraManager.h"

CCameraManager::CCameraManager(void)
{
	Pylon::PylonAutoInitTerm autoInitTerm;  // PylonInitialize() will be called now

	//Initialization/Uninitialization of the pylon Runtime Library in MFC Applications
	AfxEnableMemoryTracking ( false );	//To suppress the dumping of memory leaks

	_tcscpy_s(m_stCameraInfo.szFriendlyName	,_T("N/A")	);	
	_tcscpy_s(m_stCameraInfo.szFullName		,_T("N/A")	);	
	_tcscpy_s(m_stCameraInfo.szVendorName	,_T("N/A")	);	
	_tcscpy_s(m_stCameraInfo.szDeviceClass	,_T("N/A")	);	
	//////////////////////////////////////////////////////////////////////////
	_tcscpy_s(m_stCameraInfo.szSerialNumber	,_T("N/A")	);	
	_tcscpy_s(m_stCameraInfo.szUserDefinedName,_T("N/A")	);
	_tcscpy_s(m_stCameraInfo.szModelName		,_T("N/A")	);
	_tcscpy_s(m_stCameraInfo.szDeviceVersion	,_T("N/A")	);
	_tcscpy_s(m_stCameraInfo.szDeviceFactory	,_T("N/A")	);
	_tcscpy_s(m_stCameraInfo.szXMLSource		,_T("N/A")	);

	m_bFirst = false;
	m_bFinded = false;
	m_bRemoved = false;

	m_iSkippiedFrame = 0;

	m_nLinkTime = 3;

	pImageBuffer = NULL;

	// Get the PC timer frequency.
	::QueryPerformanceFrequency(&freq);
	::QueryPerformanceCounter(&freqstart);// 시작 count를 구해놓고...

	m_pNodeMap = NULL;
}

CCameraManager::~CCameraManager(void)
{
}

//////////////////////////////////////////////////////////////////////////
void CCameraManager::SetRemoveTime( UINT16 nTime )
{
	m_nLinkTime = (int64_t)(4100*nTime);
}

INT8 CCameraManager::Open_Camera( UINT8 nFilterType, LPCTSTR szFilter  )
{
	try
	{
		if( m_camera.IsOpen() )
			return -1;

		m_bFinded = false;
		m_bRemoved = false;

		//////////////////////////////////////////////////////////////////////////
		if( !m_bFirst)
		{
			m_bFirst = true;
			//////////////////////////////////////////////////////////////////////////
			//연결된 모든 장치에 대한 고유한 이름을 얻기
			m_ptlFactory = &CTlFactory::GetInstance();			
		}

		//해당 장비만 연결하기위한 필터
		if( nFilterType == 0 )
		{
			if( m_ptlFactory->EnumerateDevices(m_DIList) > 0 )
				m_bFinded = true;
		}
		else
		{
			// 우리가 찾고있는 장치의 속성을 설명하는 CDeviceInfo 개체의 정보를 포함하는 필터를 만듭니다.
			DeviceInfoList_t filter;
			filter.clear ();

			switch(nFilterType)
			{
			case INFO_FRIENDLY_NAME		: filter.push_back( CDeviceInfo().SetFriendlyName( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_FULL_NAME			: filter.push_back( CDeviceInfo().SetFullName( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_VENDOR_NAME		: filter.push_back( CDeviceInfo().SetVendorName( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_DEVICE_CLASS		: filter.push_back( CDeviceInfo().SetDeviceClass( (char*)(LPCTSTR)szFilter ));
				break;

			case INFO_SERIAL_NUMBER		: filter.push_back( CDeviceInfo().SetSerialNumber( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_USER_DEFINED_NAME : filter.push_back( CDeviceInfo().SetUserDefinedName( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_MODEL_NAME		: filter.push_back( CDeviceInfo().SetModelName( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_DEVICE_VERSION	: filter.push_back( CDeviceInfo().SetDeviceVersion( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_DEVICE_FACTORY	: filter.push_back( CDeviceInfo().SetDeviceFactory( (char*)(LPCTSTR)szFilter ));
				break;
			case INFO_XML_SOURCE		: filter.push_back( CDeviceInfo().SetXMLSource( (char*)(LPCTSTR)szFilter ));				
				break;
			}
			//장비의 디바이스 정보 얻기
			if( m_ptlFactory->EnumerateDevices(m_DIList, filter) > 0 )
				m_bFinded = true;
		}

		if( m_bFinded ) //카메라를 찾았다면
		{
			m_camera.RegisterConfiguration( this, RegistrationMode_Append, Ownership_ExternalOwnership);	
			m_camera.RegisterImageEventHandler( this, RegistrationMode_Append, Ownership_ExternalOwnership);
			//카메라 이벤트 처리가 먼저 활성화되어야합니다, 기본값은 꺼짐입니다.
			m_camera.GrabCameraEvents = true;

			// 카메라가 발견되었습니다. 생성하고 인스턴트 카메라의 객체에 연결합니다.
			m_camera.Attach( m_ptlFactory->CreateDevice( m_DIList[0]));

			if ( m_camera.IsPylonDeviceAttached())
			{
#ifdef UNICODE
				//정보 얻기
				//////////////////////////////////////////////////////////////////////////
				_tcsncpy_s(m_stCameraInfo.szFriendlyName	,_countof(m_stCameraInfo.szFriendlyName)	,m_camera.GetDeviceInfo().GetFriendlyName().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szFullName		,_countof(m_stCameraInfo.szFullName)		,m_camera.GetDeviceInfo().GetFullName().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szVendorName		,_countof(m_stCameraInfo.szVendorName)		,m_camera.GetDeviceInfo().GetVendorName().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceClass		,_countof(m_stCameraInfo.szDeviceClass)		,m_camera.GetDeviceInfo().GetDeviceClass().w_str(), _TRUNCATE);

				//////////////////////////////////////////////////////////////////////////
				_tcsncpy_s(m_stCameraInfo.szSerialNumber	,_countof(m_stCameraInfo.szSerialNumber)	,m_camera.GetDeviceInfo().GetSerialNumber().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szUserDefinedName	,_countof(m_stCameraInfo.szUserDefinedName)	,m_camera.GetDeviceInfo().GetUserDefinedName().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szModelName		,_countof(m_stCameraInfo.szModelName)		,m_camera.GetDeviceInfo().GetModelName().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceVersion	,_countof(m_stCameraInfo.szDeviceVersion)	,m_camera.GetDeviceInfo().GetDeviceVersion().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceFactory	,_countof(m_stCameraInfo.szDeviceFactory)	,m_camera.GetDeviceInfo().GetDeviceFactory().w_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szXMLSource		,_countof(m_stCameraInfo.szXMLSource)		,m_camera.GetDeviceInfo().GetXMLSource().w_str(), _TRUNCATE);

#else
				//정보 얻기
				//////////////////////////////////////////////////////////////////////////
				_tcsncpy_s(m_stCameraInfo.szFriendlyName	,_countof(m_stCameraInfo.szFriendlyName)	,m_camera.GetDeviceInfo().GetFriendlyName().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szFullName		,_countof(m_stCameraInfo.szFullName)		,m_camera.GetDeviceInfo().GetFullName().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szVendorName		,_countof(m_stCameraInfo.szVendorName)		,m_camera.GetDeviceInfo().GetVendorName().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceClass		,_countof(m_stCameraInfo.szDeviceClass)		,m_camera.GetDeviceInfo().GetDeviceClass().c_str(), _TRUNCATE);

				//////////////////////////////////////////////////////////////////////////
				_tcsncpy_s(m_stCameraInfo.szSerialNumber	,_countof(m_stCameraInfo.szSerialNumber)	,m_camera.GetDeviceInfo().GetSerialNumber().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szUserDefinedName	,_countof(m_stCameraInfo.szUserDefinedName)	,m_camera.GetDeviceInfo().GetUserDefinedName().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szModelName		,_countof(m_stCameraInfo.szModelName)		,m_camera.GetDeviceInfo().GetModelName().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceVersion	,_countof(m_stCameraInfo.szDeviceVersion)	,m_camera.GetDeviceInfo().GetDeviceVersion().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szDeviceFactory	,_countof(m_stCameraInfo.szDeviceFactory)	,m_camera.GetDeviceInfo().GetDeviceFactory().c_str(), _TRUNCATE);
				_tcsncpy_s(m_stCameraInfo.szXMLSource		,_countof(m_stCameraInfo.szXMLSource)		,m_camera.GetDeviceInfo().GetXMLSource().c_str(), _TRUNCATE);

#endif
				//카메라 연결
				m_pNodeMap = &m_camera.GetNodeMap();


				m_camera.Open();
				
				CHearbeatHelper heartbeatHelper(m_camera);
				heartbeatHelper.SetValue(m_nLinkTime);  // 1000 ms timeout.
				return 0;	
			}
			return -1;
		}
		else
		{
			return -1;
		}
	}
	catch (GenICam::GenericException &e) //Error Handling
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("Open_Camera - GenericException : %s\n"), (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
}


INT8 CCameraManager::Close_Camera( )
{
	Grab_Camera( false );

	try
	{
		if( m_camera.IsOpen() )
		{
			m_pNodeMap = NULL;
			m_camera.Close();
			if( m_bFirst )
			{
				//카메라의 속성을 기억하기위한 장치 정보 개체를 만듭니다.
				CDeviceInfo m_csDevInfo;
				// 다시 같은 카메라를 감지 할 수 카메라의 속성을 기억하십시오.
				m_csDevInfo.SetDeviceClass( m_camera.GetDeviceInfo().GetDeviceClass());
				m_csDevInfo.SetSerialNumber( m_camera.GetDeviceInfo().GetSerialNumber());

				//분리 된 카메라 장치를 나타내는 Pylon 장치를 파괴한다.
				// 그것은 더 이상 사용할 수 없습니다.
				m_camera.DestroyDevice();
			}
			m_bRemoved = true;
		}
		return 0;
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		TRACE( _T("Close_Camera - GenericException : %s\n"), (CString)e.GetDescription() );
		return 0;
	}
}

INT8 CCameraManager::Grab_Camera( bool bGrab )
{
	if( bGrab )
	{
		if( !m_camera.IsOpen() )
			return -1;

		if( m_camera.IsGrabbing() )
			return -2;

		// 파라미터 MaxNumBuffer 잡는 위해 할당 된 버퍼의 수를 제어하는 데 사용될 수있다. 이 매개 변수의 기본값은 10입니다.
		m_camera.MaxNumBuffer = 1;
		m_camera.StartGrabbing( GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);
		
		/*
		열거 :
		GrabStrategy_OneByOne.			: 이미지 순서대로 처리
		GrabStrategy_LatestImageOnly.	: 마지막 이미지만 저장
		GrabStrategy_UpcomingImage.		: 입력버퍼에 계속 갱신처리
		*/
	}
	else
	{
		m_camera.StopGrabbing();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
int64_t CCameraManager::Adjust(int64_t val, int64_t minimum, int64_t maximum, int64_t inc)
{
	if (inc <= 0)	throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d", inc);
	if (minimum > maximum)	throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");
	if (val < minimum)	return minimum;
	if (val > maximum)	return maximum;
	if (inc == 1)	return val;
	return minimum + ( ((val - minimum) / inc) * inc );
}

short CCameraManager::GetParameter( TCHAR *szNodeName, int64_t *pValue )
{
	TCHAR szName[128]={0,};
	if( !_tcscmp(szNodeName, _T("GainAuto")) )
		GetEnumeration( szNodeName, pValue, szName, _countof(szName) );
	if( !_tcscmp(szNodeName, _T("GainRaw")) )
		GetInteger( szNodeName, pValue );
	if( !_tcscmp(szNodeName, _T("AutoTargetValue")) )
		GetInteger( szNodeName, pValue );
	
	


	return 0;
}
short CCameraManager::SetParameter( TCHAR *szNodeName, const int64_t nSetValue )
{
	TCHAR szName[128]={0,};
	if( !_tcscmp(szNodeName, _T("GainAuto")) )
		SetEnumeration( szNodeName, nSetValue );
	if( !_tcscmp(szNodeName, _T("GainRaw")) )
		SetInteger( szNodeName, nSetValue );

	return 0;
}
//////////////////////////////////////////////////////////////////////////
short CCameraManager::GetEnumeration( TCHAR *szNodeName, int64_t *pGetValue, TCHAR *szGetString, UINT16 nCount )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		INodeMap& nodemap = m_camera.GetNodeMap();
		CEnumerationPtr pEnumParam( nodemap.GetNode(cNodeName) );
		
		if( pEnumParam )
		{
			*pGetValue = pEnumParam->GetIntValue();
#ifdef UNICODE
			_tcsncpy_s(szGetString		,nCount		,pEnumParam->ToString().w_str(), _TRUNCATE);
#else
			_tcsncpy_s(szGetString		,nCount		,pEnumParam->ToString().c_str(), _TRUNCATE);		
#endif
			TRACE(_T("%s  : %s %d\n"), szNodeName, szGetString, *pGetValue );
			return 0;
		}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("GainAuto - GenericException : %s\n"), (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
short CCameraManager::SetEnumeration( TCHAR *szNodeName, const int64_t nSetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		TCHAR	szName[128]={0};
		INodeMap& nodemap = m_camera.GetNodeMap();
		CEnumerationPtr pEnumParam( nodemap.GetNode(cNodeName) );

		if( pEnumParam.IsValid() )	//Access the integer type node.
			if( IsWritable(pEnumParam) )//Access the enumeration type node GainAuto.
			{
				pEnumParam->SetIntValue( nSetValue );
#ifdef UNICODE
				_tcsncpy_s(szName ,_countof(szName)	 ,pEnumParam->ToString().w_str(), _TRUNCATE);
#else
				_tcsncpy_s(szName ,_countof(szName) ,pEnumParam->ToString().c_str(), _TRUNCATE);		
#endif
				TRACE(_T("%s  : %s %d\n"), szNodeName, szName, pEnumParam->GetIntValue() );
				return 0;
			}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("GainAuto - GenericException : %s\n"), (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////
int64_t CCameraManager::IntCheckMinMax(int64_t val, int64_t minimum, int64_t maximum, int64_t inc)
{
	if (inc <= 0)
		throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d", inc);
	if (minimum > maximum)
		throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");
	
	if (val < minimum)	return minimum;
	if (val > maximum)	return maximum;
	
	if (inc == 1)	return val;
	else			return minimum + ( ((val - minimum) / inc) * inc );
}
short CCameraManager::GetInteger( TCHAR *szNodeName, int64_t *pGetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		INodeMap& nodemap = m_camera.GetNodeMap();
		CIntegerPtr pIntegerParam( nodemap.GetNode(cNodeName) );

		if( pIntegerParam )
		{
			*pGetValue = pIntegerParam->GetValue();

			TRACE(_T("%s  : %d\n"), szNodeName, *pGetValue );
			return 0;
		}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
short CCameraManager::SetInteger( TCHAR *szNodeName, const int64_t nSetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		TCHAR	szName[128]={0};
		INodeMap& nodemap = m_camera.GetNodeMap();
		CIntegerPtr pIntegerParam( nodemap.GetNode(cNodeName) );

		if( pIntegerParam.IsValid() )//Access the integer type node.
			if( IsWritable(pIntegerParam) )//Access the enumeration type node GainAuto.
			{
				int64_t nCheckValue = IntCheckMinMax(nSetValue, pIntegerParam->GetMin(), pIntegerParam->GetMax(), pIntegerParam->GetInc());
				pIntegerParam->SetValue( nCheckValue );

				TRACE(_T("%s  : %d\n"), szNodeName, pIntegerParam->GetValue() );
				return 0;
			}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
float CCameraManager::FloatCheckMinMax(float val, float minimum, float maximum, float inc)
{
	if (inc <= 0)
		throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d", inc);
	if (minimum > maximum)
		throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");

	if (val < minimum)	return minimum;
	if (val > maximum)	return maximum;

	if (inc == 1)	return val;
	else			return minimum + ( ((val - minimum) / inc) * inc );
}
short CCameraManager::GetFloat( TCHAR *szNodeName, float *pGetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		INodeMap& nodemap = m_camera.GetNodeMap();
		CFloatPtr pFloatParam( nodemap.GetNode(cNodeName) );

		if( pFloatParam )
		{
			*pGetValue = (float)pFloatParam->GetValue();

			TRACE(_T("%s  : %f\n"), szNodeName, *pGetValue );
			return 0;
		}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
short CCameraManager::SetFloat( TCHAR *szNodeName, const float nSetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		TCHAR	szName[128]={0};
		INodeMap& nodemap = m_camera.GetNodeMap();
		CFloatPtr pFloatParam( nodemap.GetNode(cNodeName) );

		if( pFloatParam.IsValid() )//Access the integer type node.
			if( IsWritable(pFloatParam) )//Access the enumeration type node GainAuto.
			{
				float nCheckValue = FloatCheckMinMax(nSetValue, (float)pFloatParam->GetMin(), (float)pFloatParam->GetMax(), (float)pFloatParam->GetInc());
				pFloatParam->SetValue( nCheckValue );

				TRACE(_T("%s  : %f\n"), szNodeName, (float)pFloatParam->GetValue() );
				return 0;
			}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
short CCameraManager::GetBoolean( TCHAR *szNodeName, bool *pGetValue, TCHAR *szGetString, UINT16 nCount )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		INodeMap& nodemap = m_camera.GetNodeMap();
		CBooleanPtr pBooleanParam( nodemap.GetNode(cNodeName) );

		if( pBooleanParam )
		{
			*pGetValue = pBooleanParam->GetValue();

			_tcsncpy_s(szGetString	,nCount	,_T("FALSE"), _TRUNCATE);
			if( *pGetValue )
				_tcsncpy_s(szGetString	,nCount	,_T("TRUE"), _TRUNCATE);

			TRACE(_T("%s  : %f\n"), szNodeName, *pGetValue );
			return 0;
		}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}
short CCameraManager::SetBoolean( TCHAR *szNodeName, const bool bSetValue )
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		char         cNodeName[128]={0};
#ifdef _UNICODE                  
		:: WideCharToMultiByte (CP_ACP, 0, szNodeName , -1, (LPSTR) cNodeName, _countof(cNodeName), NULL , NULL  );
#else
		strncpy_s(cNodeName , _countof(cNodeName), (LPSTR)(LPCTSTR)szNodeName , _countof(cNodeName)-1);
#endif

		TCHAR	szName[128]={0};
		INodeMap& nodemap = m_camera.GetNodeMap();
		CBooleanPtr pBooleanParam( nodemap.GetNode(cNodeName) );

		if( pBooleanParam.IsValid() )//Access the integer type node.
			if( IsWritable(pBooleanParam) )//Access the enumeration type node GainAuto.
			{
				pBooleanParam->SetValue( bSetValue );

				if( pBooleanParam->GetValue() )	TRACE(_T("%s  : true\n"), szNodeName );
				else							TRACE(_T("%s  : false\n"), szNodeName );

				return 0;
			}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString strTrace;
		strTrace.Format( _T("%s - GenericException : %s\n"), szNodeName, (CString)e.GetDescription() );
		AfxMessageBox( strTrace );
		return -2;
	}
	return -1;
}

bool CCameraManager::IsGrabbing()
{
	return m_camera.IsGrabbing();
}
bool CCameraManager::IsOpen()
{
	return m_camera.IsOpen();
}
UINT16 CCameraManager::Ret_FrameRate()
{
	double dGrabTime = (double)freqdiff.QuadPart/(double)freq.QuadPart;
	UINT16 nRetFrameRate = (UINT16)(1.0/dGrabTime); 
	return nRetFrameRate; 
}

//////////////////////////////////////////////////////////////////////////
void CCameraManager::OnAttach( CInstantCamera& /*camera*/) 
{
	TRACE(_T("OnAttach event\n"));
}

void CCameraManager::OnAttached( CInstantCamera& camera) 
{
	TRACE( _T("OnAttached event for device %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnOpen( CInstantCamera& camera) 
{
	TRACE( _T("OnOpen event for device %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnOpened( CInstantCamera& camera) 
{
	TRACE( _T("OnOpened event for device %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnClose( CInstantCamera& camera) 
{
	TRACE( _T("OnClose event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnClosed( CInstantCamera& camera) 
{
	TRACE( _T("OnClosed event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnDestroy( CInstantCamera& camera) 
{
	TRACE( _T("OnDestroy event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnDestroyed( CInstantCamera& /*camera*/) 
{
	TRACE( _T("OnDestroyed event\n\n\n") );
}

void CCameraManager::OnDetach( CInstantCamera& camera) 
{
	TRACE( _T("OnDetach event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnDetached( CInstantCamera& camera) 
{
	TRACE( _T("OnDetached event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnCameraDeviceRemoved( CInstantCamera& camera)
{
	// 카메라 장치가 제거되었습니다.
	TRACE( _T("OnDetached event. The camera has been removed from the PC.\n") );
	m_bRemoved = true;
	Close_Camera( );

}

//////////////////////////////////////////////////////////////////////////
void CCameraManager::OnGrabStart( CInstantCamera& camera) 
{
	TRACE( _T("OnGrabStart event for device %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnGrabStarted( CInstantCamera& camera) 
{
	TRACE( _T("OnGrabStarted event for device %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnGrabStop( CInstantCamera& camera) 
{
	TRACE( _T("OnGrabStop event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnGrabStopped( CInstantCamera& camera) 
{
	TRACE( _T("OnGrabStopped event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
}

void CCameraManager::OnGrabError( CInstantCamera& camera, const String_t errorMessage) 
{
	TRACE( _T("OnGrabError event for device  %s\n"), camera.GetDeviceInfo().GetModelName().c_str() );
	TRACE( _T("Error Message :%s\n") ,errorMessage.c_str() );
}
void CCameraManager::OnImagesSkipped( CInstantCamera& camera, size_t countOfSkippedImages)
{   
	if( countOfSkippedImages <= 0)
		countOfSkippedImages = 0;
	m_iSkippiedFrame = countOfSkippedImages;

	TRACE( _T("OnImagesSkipped event for device %s \n%d images have been skipped.\n"), camera.GetDeviceInfo().GetModelName().c_str(), countOfSkippedImages );
}


void CCameraManager::OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{   
	CString strTrace;

	if (ptrGrabResult->GrabSucceeded())
	{
		::QueryPerformanceCounter(&freqend);
		freqdiff.QuadPart  = (LONGLONG)(freqend.QuadPart - freqstart.QuadPart);	
		
		// 캡처 한 이미지를 표시합니다.
		//m_PylonImage.AttachGrabResultBuffer( ptrGrabResult);
		//Pylon::DisplayImage(1, m_PylonImage);

		pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
		
		::QueryPerformanceCounter(&freqstart);// 끝 count를 구한다.
	}
	else
	{
		TRACE( _T("- Error: %d (%s) \n"), ptrGrabResult->GetErrorCode(), ptrGrabResult->GetErrorDescription().c_str() );
	}
	
}
