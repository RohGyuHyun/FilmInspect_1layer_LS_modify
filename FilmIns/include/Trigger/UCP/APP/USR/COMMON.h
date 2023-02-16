//#############################################################################
// Release Date: October 25, 2013
//#############################################################################

#ifndef	__COMMON_H__
#define	__COMMON_H__


#ifdef _MSC_VER

	typedef	signed _int8		int8;
	typedef	signed _int16		int16;
	typedef	signed _int32		int32;
	typedef	signed _int64		int64;
	typedef	unsigned _int8		Uint8;
	typedef	unsigned _int16		Uint16;
	typedef	unsigned _int32		Uint32;
	typedef	unsigned _int64		Uint64;
	typedef	float				float32;
	typedef	double				float64;

	#define	INT8				int8
	#define	INT16				int16
	#define	INT32				int32
	#define	INT64				int64
	#define	UINT8				Uint8
	#define	UINT16				Uint16
	#define	UINT32				UINT32
	#define	UINT64				Uint64
	#define	FLOAT32				float32
	#define	FLOAT64				float64

#elif defined(__TMS320C28346__)

	typedef	char				INT8;
	typedef	int					INT16;
	typedef	long				INT32;
	typedef	long long			INT64;

	typedef	unsigned char		UINT8;
	typedef	unsigned int		UINT16;
	typedef	unsigned long		UINT32;
	typedef	unsigned long long	UINT64;

	typedef	float				FLOAT32;
	typedef	long double			FLOAT64;


	#if (!defined(FALSE) && !defined(TRUE))
	typedef enum
	{
		FALSE = 0,
		TRUE
	}	EBOOL;

	typedef	UINT16			BOOL;
	#endif

	#define	WORDSIZEOF(x)	(sizeof(x))
	#define	BYTESIZEOF(x)	(sizeof(x)<<1)
	#define	LOBYTE(w)		((w) & 0x00FF)
	#define	HIBYTE(w)		(LOBYTE((w)>>8))
	#define MAKEWORD(h, l)	((UINT16)(((h) & 0x00ff)<<8) | (UINT16)((l) & 0x00ff))
	#define MAKEBYTE(h, l)	((UINT16)(((h) & 0x000f)<<4) | (UINT16)((l) & 0x000f))

	#define	GET_BIT(x, n)	((x >> n) & 0x1)

	#define	NTOHS(x)		(((x<<8) & 0xFF00) | ((x>>8) & 0x00FF))
	#define	HTONS(x)		NTOHS(x)
	#define	NTOHL(x)		(((x<<24) & 0xFF000000L) | ((x<<8) & 0x00FF0000L) | ((x>>8) & 0x0000FF00L) | ((x>>24) & 0x000000FFL))
	#define	HTONL(x)		NTOHL(x)

	#define	PI				(3.141592653589793)
	#define	SIN60			(0.866025403784439)
	#define	SIN30			(0.5)
	#define	COS60			(0.5)
	#define	COS30			(0.866025403784439)
	#define	SQRT2			(1.414213562373095)
	#define	SQRT3			(1.732050807568877)
	#define	ISQRT2			(0.707106781186548)
	#define	ISQRT3			(0.577350269189626)

	#define	ABS(x)			(((x) >= 0) ? (x) : -(x))
	#define	MAX(x,y)		(((x) >= (y)) ? (x) : (y))
	#define	MIN(x,y)		(((x) <= (y)) ? (x) : (y))
	#define	SQUARE(x)		((x)*(x))
	#define	DEG2RAD(x)		((x)*0.01745329252)
	#define	RAD2DEG(x)		((x)*57.2957795131)
	#define	RPM2FRQ(x)		((x)/60.)
	#define	RPM2ANGFRQ(x)	((x)*0.10471975512)		//((x)*2.*PI/60.) RPM -> rad/sec

	#define	SIN(degree)		(sin(DEG2RAD(degree)))
	#define	COS(degree)		(cos(DEG2RAD(degree)))

	#define	MSEC(sec)		((sec)/1000.)
	#define	USEC(sec)		((sec)/1000000.)

	#define	ROUND(x)		((Int32)(x + 0.5))

#elif defined(CORTEX_ARM)          
    typedef	unsigned char           UINT8;
    typedef	signed char             INT8;
    typedef	unsigned short          UINT16;
    typedef	signed short            INT16;
    typedef	unsigned long            UINT32;
    typedef	signed long              INT32;        
    typedef	unsigned long long	UINT64;
    typedef	signed long long          INT64;     
#endif
	

typedef	struct
{
	UINT16		bit0	: 1;		// LSB
	UINT16		bit1	: 1;
	UINT16		bit2	: 1;
	UINT16		bit3	: 1;
	UINT16		bit4	: 1;
	UINT16		bit5	: 1;
	UINT16		bit6	: 1;
	UINT16		bit7	: 1;
	UINT16		bit8	: 1;
	UINT16		bit9	: 1;
	UINT16		bit10	: 1;
	UINT16		bit11	: 1;
	UINT16		bit12	: 1;
	UINT16		bit13	: 1;
	UINT16		bit14	: 1;
	UINT16		bit15	: 1;		// MSB
}	BIT16;

typedef	struct
{
	UINT16		bit0	: 1;		// LSB
	UINT16		bit1	: 1;
	UINT16		bit2	: 1;
	UINT16		bit3	: 1;
	UINT16		bit4	: 1;
	UINT16		bit5	: 1;
	UINT16		bit6	: 1;
	UINT16		bit7	: 1;
	UINT16		bit8	: 1;
	UINT16		bit9	: 1;
	UINT16		bit10	: 1;
	UINT16		bit11	: 1;
	UINT16		bit12	: 1;
	UINT16		bit13	: 1;
	UINT16		bit14	: 1;
	UINT16		bit15	: 1;
	UINT16		bit16	: 1;
	UINT16		bit17	: 1;
	UINT16		bit18	: 1;
	UINT16		bit19	: 1;
	UINT16		bit20	: 1;
	UINT16		bit21	: 1;
	UINT16		bit22	: 1;
	UINT16		bit23	: 1;
	UINT16		bit24	: 1;
	UINT16		bit25	: 1;
	UINT16		bit26	: 1;
	UINT16		bit27	: 1;
	UINT16		bit28	: 1;
	UINT16		bit29	: 1;
	UINT16		bit30	: 1;
	UINT16		bit31	: 1;		// MSB
}	BIT32;

typedef	struct
{
	UINT16		byte0	: 8;
	UINT16		byte1	: 8;
}	BYTE16;

typedef	struct
{
	UINT16		byte0	: 8;
	UINT16		byte1	: 8;
	UINT16		byte2	: 8;
	UINT16		byte3	: 8;
}	BYTE32;

typedef	struct
{
	UINT16		word0;
	UINT16		word1;
}	WORD32;

typedef	union
{
	BIT16		bits;
	BYTE16		bytes;
	UINT16		all;
}	Data16U;

typedef	union
{
	BIT32		bits;
	BYTE32		bytes;
	WORD32		words;
	UINT32		all;
}	Data32U;

typedef struct
{
	float			Min, Max;
}	SfMinMax;


#endif

