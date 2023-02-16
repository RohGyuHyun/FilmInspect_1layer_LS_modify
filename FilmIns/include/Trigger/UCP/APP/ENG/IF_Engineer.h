//#############################################################################
// Release Date: October 25, 2013
//#############################################################################

#ifndef	__IF_ENGINEER_H__
#define	__IF_ENGINEER_H__



#include "IF_UCPTypes.h"


/////////////////////////////////////////////////////////////////////
// Command
typedef	enum
{
	START_OF_ENG_CMD		= 0x1100,


	END_OF_ENG_CMD			= 0x1FFF
}	EENG_CMD;

/////////////////////////////////////////////////////////////////////
// Structure


/////////////////////////////////////////////////////////////////////
// Message
typedef	union
{
	UINT16			Dummy[UCP_MBS/2];
}	UEngBody;

typedef struct
{
	SUCPHeader		Header;
	UEngBody		Body;
}	SEngMsg;



#endif

