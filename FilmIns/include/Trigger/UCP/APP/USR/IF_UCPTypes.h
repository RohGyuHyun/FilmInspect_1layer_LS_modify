//#############################################################################
// Release Date: October 25, 2013
//#############################################################################

#ifndef	__IF_UCPTYPES_H__
#define	__IF_UCPTYPES_H__



#include "COMMON.h"


#ifndef MTU_SIZE
#define	MTU_SIZE	(1500)
#endif

#ifndef TCP_MSS
#define TCP_MSS		(1460)		// TCP Maximum Segment Size = (MTU_SIZE - IPHDR_SIZE - TCPHDR_SIZE)
#endif

#ifndef UDP_MSS
#define UDP_MSS		(1472)		// UDP Maximum Segment Size = (MTU_SIZE - IPHDR_SIZE - UDPHDR_SIZE)
#endif

#define UCP_MBS		(1464)		// [Byte], UCP Maximum Body Size = (UDP_MSS - UCPHDR_SIZE) = 1472 - 8
#define	NO_RESPONSE	(0xFFFF)


typedef	struct
{
	UINT16			MAC0 : 8;
	UINT16			MAC1 : 8;
	UINT16			MAC2 : 8;
	UINT16			MAC3 : 8;
	UINT16			MAC4 : 8;
	UINT16			MAC5 : 8;
}	SMACAddr;

typedef	struct
{
	UINT16			IP0 : 8;
	UINT16			IP1 : 8;
	UINT16			IP2 : 8;
	UINT16			IP3 : 8;
}	SIPv4;

#define UCPHDR_SIZE		(8)
typedef struct
{
	UINT32			Version;
	UINT16			TransactionID;
	UINT16			Cmd : 14;
	UINT16			Status : 1;
	UINT16			VerError : 1;
}	SUCPHeader;

typedef struct 
{
	UINT16			Index;
	UINT16			Total;
}	SFragmentInfo;

#define UCP_FRAGMENT_SIZE (UCP_MBS-4)
typedef struct
{
	SFragmentInfo	        Info;
	UINT16			Data[UCP_FRAGMENT_SIZE/2];
}	SFragmentData;

typedef struct
{
	SIPv4		IpAddr;	
	UINT16		IpPort;
	UINT16		UcpSize;
}	SUCPSRC;


#endif
