//#############################################################################
// Release Date: October 25, 2013
//#############################################################################

#ifndef	__IF_APPLICATION_H__
#define	__IF_APPLICATION_H__



#include "IF_UCP.h"
#include "IF_Engineer.h"


typedef	union
{
	UEngBody	Eng;
	UUCPBody	Usr;
}	UAppBody;

typedef struct
{
	SUCPHeader	Header;
	UAppBody	Body;
}	SAppMsg;


#endif

