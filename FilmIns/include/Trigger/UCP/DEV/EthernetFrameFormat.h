//#############################################################################
// Release Date: October 25, 2013
//#############################################################################

#ifndef	__ETHERNET_FRAME_FORMAT_H__
#define	__ETHERNET_FRAME_FORMAT_H__



/********************************************************************

[Preamble] [SFD] [Destination] [Source] [Length/Type] [Data] [FCS]

Legend : SFD=Start Frame Delimeter, FCS=Frame Check Sequence(CRC)

<Description>
---------------------------------------------------------------------
Filed		Bytes		Description
=====================================================================
Preamble	7
SFD			1
Destination	6
Source		6
Length/Type	2
Data		46-1500		Ethernet MTU(Maximum Transmission Unit) = 1500
FCS			4
---------------------------------------------------------------------

********************************************************************/

#include "COMMON.h"


#define ETHHDR_SIZE		(14)
typedef	struct
{
	MACAddr		DstMAC;
	MACAddr		SrcMAC;
	UINT16		Type;
}	ETHHDR;

#define IPHDR_SIZE		(20)
typedef struct
{
	UINT8		Version : 4;
	UINT8		HeaderLength : 4;
	UINT16		TOS;
	UINT16		TotalLength;
	UINT16		Id;
	UINT16		Flag : 3;
	UINT16		FragmentOffset : 13;
	UINT8		TTL : 8;
	UINT8		Protocol : 8;
	UINT16		HeaderChecksum;
	IPv4		SrcIP;
	IPv4		DstIP;
}	IPHDR;

#define	TCPHDR_SIZE		(20)
typedef struct
{
	UINT16		SrcPort;
	UINT16		DstPort;
	UINT32		SeqNum;
	UINT32		AckNum;
	UINT8		Offset : 4;
	UINT8		RSVD : 6;
	UINT8		URG : 1;
	UINT8		ACK : 1;
	UINT8		PSH : 1;
	UINT8		RST : 1;
	UINT8		SYN : 1;
	UINT8		FIN : 1;
	UINT16		WindowSize;
	UINT16		Checksum;
	UINT16		UrgentPtr;
}	TCPHDR;

#define UDPHDR_SIZE		(8)
typedef struct
{
	UINT16		SrcPort;
	UINT16		DstPort;
	UINT16		Length;
	UINT16		Checksum;
}	UDPHDR;

#define PSEUDOHDR_SIZE	(12)
typedef struct
{
	IPv4		SrcIP;
	IPv4		DstIP;
	UINT16		Length;
	UINT8		RSVD : 8;
	UINT8		Protocol : 8;
}	PSEUDOHDR;


#endif
