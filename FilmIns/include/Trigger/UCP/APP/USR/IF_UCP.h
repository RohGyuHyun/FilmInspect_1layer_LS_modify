//#############################################################################
// Release Date: October 28, 2013
//#############################################################################

#ifndef	__IF_UCP_H__
#define	__IF_UCP_H__


#include "IF_UCPTypes.h"

#define	UCP_PORTNUM			(50010)

#define	UCP_VER_MAJOR			(1)
#define	UCP_VER_MINOR			(0)
#define	UCP_VER_BUILD_YEAR		(14)
#define	UCP_VER_BUILD_MONTH		(04)
#define	UCP_VER_BUILD_DAY		(02)

/////////////////////////////////////////////////////////////////////
// Command
typedef	enum
{
	START_OF_CMD		= 0x0000,

	// System
	GET_PRODUCT_INFO	= 0x0001,
        
        //Trigger Command
        GET_TRIG_STATUS         = 0x0010,
        SET_TRIG_POS_RESET,
        SET_TRIG_ONE_SHOT,
        SET_TRIG_SAVE,
        
        GET_TRIG_CONFIG         = 0x0020,
        SET_TRIG_CONFIG,

        GET_POS_TRIG            = 0x0030,
        SET_POS_TRIG,
        
        GET_LINE_TRIG           = 0x0040,
        SET_LINE_TRIG,
        
        GET_FUSION_TRIG         = 0x0050,
        SET_FUSION_TRIG,
        
        GET_VISION_TRIGGER      = 0x0060,
        SET_VISION_TRIGGER,
        
		SET_LINE_TRIG_CNT_RESET      = 0x0070,

	END_OF_CMD		= 0x0FFF
}	EUCP_CMD;

/////////////////////////////////////////////////////////////////////
// Structure
typedef union
{
	struct
	{
		UINT32	Major		: 8;
		UINT32	Minor		: 8;
		UINT32	BuildYear	: 7;
		UINT32	BuildMonth	: 4;
		UINT32	BuildDay	: 5;
	}	bit;

	UINT32	all;
}	UVersion;

typedef struct
{
	UVersion		HW;
	UVersion		FW;
	UVersion		UCP;
}	SVersion;

#define	MAX_NAME_SIZE	20
typedef struct
{
	SVersion		Version;
        UINT16		ProductName[MAX_NAME_SIZE];
}	SProductInfo;


#include "IF_VisionTrigger.h"

/////////////////////////////////////////////////////////////////////
// Message
typedef	union
{
	SProductInfo	        ProductInfo;
        
	STrigStatus             TrigStatus;
	STrigConfig             TrigConfig;
        
	SPositionTrigger        PosTrig;
	SLineScanTrigger        LineTrig;
	SFusionTrigger          FusionTrig;
        
	SVisionTrigger          VisionTrig;
	
	UINT16		        Dummy[UCP_MBS/2];
}	UUCPBody;

typedef struct
{
	SUCPHeader		Header;
	UUCPBody		Body;
}	SUCPMsg;



#endif

