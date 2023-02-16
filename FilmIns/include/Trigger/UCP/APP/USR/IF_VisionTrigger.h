#ifndef	__IF_TRIGGER_H__
#define	__IF_TRIGGER_H__

#define ETH_IP_ADDR0		192
#define ETH_IP_ADDR1		168
#define ETH_IP_ADDR2		0
#define ETH_IP_ADDR3		108

#define ETH_MASK0		255
#define ETH_MASK1		255
#define ETH_MASK2		240
#define ETH_MASK3		0

#define USTIME_GAIN		0.5333333333333333333333333333f // (us/pulse)

typedef union
{
  struct
  {
      UINT16 AxisCounterDir     : 1;
      UINT16 ModeCH0            : 2;
	  UINT16 ModeCH1			: 2;
	  UINT16 ModeCH2			: 2;
	  UINT16 Rsvd				: 1;
	  UINT16 EnableCH0			: 1;
	  UINT16 EnableCH1			: 1;
	  UINT16 EnableCH2			: 1;
	  UINT16 LogicCH0			: 1;
	  UINT16 LogicCH1			: 1;
	  UINT16 LogicCH2			: 1;
  }bits;
  UINT16 Word;
}STrigConfig;

typedef union
{
  struct
  {
      UINT32 Low            :16;
      UINT32 High           :16; 
  }bits;
  INT32 DWord;
}FPGA_POSITION;



typedef struct
{
  union
  {
	struct
	{
		UINT16 Low : 8;
		UINT16 High : 8;
	}bits;
	UINT16 Word;
  }FPGA_Version;

  union
  {
	  struct
	  {
		  UINT16 PosTrigEnable		: 1;
		  UINT16 LineTrigEnable		: 1;
		  UINT16 FusionTrigEnable	: 1;
		  UINT16 Rsvd				: 13;
	  }bits;
	  UINT16 Word;
  }FPGA_ActMode;

  union
  {
	  struct
	  {
		  UINT16 PosTrigNums		: 8;
		  UINT16 FusionTrigNums		: 8;
	  }bits;
	  UINT16 Word;
  }FPGA_MemAlloc;

  INT32			CurPosition;
  INT32			LScanTrigOut_Counter;
} STrigStatus;

#define MAX_POS_TRIG		100
typedef struct
{
  UINT16       Nums;
  UINT16       TrigPulseWidth;
  INT32        Position[MAX_POS_TRIG];
} SPositionTrigger;

typedef struct
{
  UINT16       TrigPulseWidth;
  UINT16       Rsvd;
  INT32        StartPos, EndPos;
  INT32        GapSpace;
  INT32        MotorSpeed;
} SLineScanTrigger;

#define MAX_FUSION_TRIG		100
typedef struct
{
  UINT16       Nums, Rsvd;
  INT32        StartPos[MAX_FUSION_TRIG];
  INT32        EndPos[MAX_FUSION_TRIG];
} SFusionTrigger;

typedef struct
{
  STrigStatus             TrigStatus;
  STrigConfig             TrigConfig;
  
  SPositionTrigger        PosTrig;
  SLineScanTrigger        LineTrig;
  SFusionTrigger          FusionTrig;
} SVisionTrigger;

#endif

