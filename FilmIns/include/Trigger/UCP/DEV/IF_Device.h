//#############################################################################
// Release Date: October 28, 2013
//#############################################################################

#ifndef	__IF_DEVICE_H__
#define	__IF_DEVICE_H__



#include "IF_UCPTypes.h"
#include "IF_UCP.h"


#define	DEV_PORTNUM				(50000)

#define	DEV_VER_MAJOR			(1)
#define	DEV_VER_MINOR			(0)
#define	DEV_VER_BUILD_YEAR		(13)
#define	DEV_VER_BUILD_MONTH		(10)
#define	DEV_VER_BUILD_DAY		(28)

/////////////////////////////////////////////////////////////////////
// Command
typedef	enum
{
	START_OF_DEV_CMD		= 0x2000,

	RESET_DEVICE			= 0x2010,
	RESET_FACTORY_DEFAULT,

	GET_ETHERNET_CFG		= 0x2020,
	SET_ETHERNET_CFG,

	GET_FW_UPDATE_STATUS	= 0x2030,
	SET_FW_UPDATE_START,
	SET_FW_UPDATE_DATA,
	SET_FW_UPDATE_FINISH,

	ERASE_FLASH				= 0x2040,

	GET_DEVICE_INFO			= 0x2050,

	BROADCAST_IP_SEARCH		= 0x2999,

	END_OF_DEV_CMD			= 0x2FFF
}	EDEV_CMD;

/////////////////////////////////////////////////////////////////////
// Structure
typedef struct
{
	UINT16			PortNum;
	UINT16			Rsvd;
    SIPv4			IP;
    SIPv4			SubnetMask;
    SIPv4			GateWay;
    SMACAddr		Mac;
}	SNWCfg;

typedef	enum
{
	FW_APPLICATION			= 0,
	FW_BOOTLOADER,
        FW_AUXILIARY
}	EFW_TYPE;

typedef	enum
{
	FW_UPDATE_READY			= 0,
	FW_FLASH_ERASE_START,
	FW_FLASH_ERASE_DONE,
	FW_FLASH_WRITE,
	FW_FLASH_WRITE_FAIL,
	FW_UPDATE_DONE,
	FW_UPDATE_FAIL,
	FW_UPDATE_GOTO_BOOTLOADER,
	FW_MODEL_IDENTY_ERROR
}	EFW_PROGRESS;

typedef struct
{
        UINT16			Progress;        
	UINT16			RunningMode     : 8;
        UINT16			Target          : 8;

	SFragmentInfo	        DataInfo;
}	SFWStatus;

typedef struct
{
	UINT16			Type;
	UINT16			Rsvd;
	UINT32			DataSize;
}	SFWStart;

typedef struct
{
	SFragmentInfo	Info;
	UINT32			CheckSum;
}	SFWFinish;

typedef	enum
{
	FLASH_PAGE		= 0,
	FLASH_SECTOR,
	FLASH_BLOCK,
	FLASH_CHIP
}	EFLASH_TYPE;

typedef struct
{
	UINT16			Type;
	UINT16			Rsvd;

	UINT32			StartAddr;
	UINT32			EndAddr;
	UINT32			Step;
}	SEraseFlash;

/////////////////////////////////////////////////////////////////////
// Message
typedef	union
{
	SNWCfg			NWCfg;

	SFWStatus		FWStatus;
	SFWStart		FWStart;
	SFragmentData	FWData;
	SFWFinish		FWFinish;

	SEraseFlash		EraseFlash;

	SProductInfo	ProductInfo;

	UINT16			Dummy[UCP_MBS/2];
}	UDevBody;

typedef struct
{
	SUCPHeader		Header;
	UDevBody		Body;
}	SDevMsg;


#endif

