// _VisionModule.h: interface for the C_VisionModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX__VISIONMODULE_H__C04D73F5_409A_4585_B7AD_8FC556FE83ED__INCLUDED_)
#define AFX__VISIONMODULE_H__C04D73F5_409A_4585_B7AD_8FC556FE83ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int CALLBACK sLock(UINT FuncCode, UINT Pass1, UINT Pass2, UINT InPut1, UINT InPut2, UINT *Retrun1, UINT *Return2);
#ifdef __cplusplus
};
#endif


#define	VISION_CAM_NONE				-1
#define	VISION_CAM_TOP1				0	
#define	VISION_CAM_TOP2				1
#define	VISION_CAM_BOTTOM			2
#define	VISION_CAM_PUNCHING			3

//#define PIXEL_SIZE_8K_LINESCAN                      9	//um   8K linescan
#define PIXEL_SIZE_8K_LINESCAN                     4	//um   16K linescan				20190823 by
#define PIXEL_SIZE_16M_AREA	                      22	//um   16M area
#define MICRO_METER					1000

#define USE_MAP_CNT					5
#define USE_MAP_CNT_BOTTOM			1
#define COF_PF_PITCH				4.75		//mm
#define COF_PF_WIDTH				3.3		//mm
#define COF_WIDTH					70.0		//mm
#define COF_MAX_NUM					8

#define IMG_RECT_MAX_NUM				1
#define INS_RGN_MAX_NUM					20
#define LIGHT_CH_MAX_NUM				8
#define RESULT_INS_MAX_NUM				100
#define CNT_MAX_NUM						20

#define SUB_RGN_MAX_NUM					8

#define RECT_PARA_MAX_NUM_BASE			10
#define INT_PARA_MAX_NUM_BASE			10
#define DBL_PARA_MAX_NUM_BASE			10
#define BOOL_PARA_MAX_NUM_BASE			10
#define INT_PARA_MAP_MAX_NUM_BASE		10

#define RECT_PARA_MAX_NUM_ALIGN			10
#define INT_PARA_MAX_NUM_ALIGN			10
#define DBL_PARA_MAX_NUM_ALIGN			10
#define BOOL_PARA_MAX_NUM_ALIGN			10
#define INT_PARA_MAP_MAX_NUM_ALIGN		10

#define RECT_PARA_MAX_NUM_CHIP			10
#define INT_PARA_MAX_NUM_CHIP			40
#define DBL_PARA_MAX_NUM_CHIP			10
#define BOOL_PARA_MAX_NUM_CHIP			10
#define INT_PARA_MAP_MAX_NUM_CHIP		10

#define RECT_PARA_MAX_NUM_LEAD			10
#define INT_PARA_MAX_NUM_LEAD				40
#define DBL_PARA_MAX_NUM_LEAD			10
#define BOOL_PARA_MAX_NUM_LEAD			10
#define INT_PARA_MAP_MAX_NUM_LEAD		10

#define RECT_PARA_MAX_NUM_FILM			10
#define INT_PARA_MAX_NUM_FILM			25
#define DBL_PARA_MAX_NUM_FILM			10
#define BOOL_PARA_MAX_NUM_FILM			10
#define INT_PARA_MAP_MAX_NUM_FILM		10

#define RECT_PARA_MAX_NUM_OCV			10
#define INT_PARA_MAX_NUM_OCV			10
#define DBL_PARA_MAX_NUM_OCV			10
#define BOOL_PARA_MAX_NUM_OCV			10
#define INT_PARA_MAP_MAX_NUM_OCV		10
#define OCV_CHAR_MAX_NUM				20

#define RECT_PARA_MAX_NUM_SR			10
#define INT_PARA_MAX_NUM_SR				15
#define DBL_PARA_MAX_NUM_SR				10
#define BOOL_PARA_MAX_NUM_SR			10
#define INT_PARA_MAP_MAX_NUM_SR			10

#define RECT_PARA_MAX_NUM_PPUNCH		10
#define INT_PARA_MAX_NUM_PPUNCH			10
#define DBL_PARA_MAX_NUM_PPUNCH			10
#define BOOL_PARA_MAX_NUM_PPUNCH		10
#define INT_PARA_MAP_MAX_NUM_PPUNCH		10


#define DBL_RSLT_MAX_NUM				10

#define IMAGE_BUF_NUM					3
#define IMAGE_MAP_MAX_NUM				12
#define LINE_IMAGE_MAP_MAX_NUM			2
#define IMAGE_NG_NUM					3

#define RECT_PARA_MAX_NUM_SYS			40
#define INT_PARA_MAX_NUM_SYS			10
#define DBL_PARA_MAX_NUM_SYS			10
#define BOOL_PARA_MAX_NUM_SYS			40

#define NG_VIEW_MAX_NUM					3

#define VISION_FACT_MIN_AREA			4
#define VISION_FACT_MIN_WIDTH			2
#define VISION_FACT_MIN_HEIGHT			2

#define VISION_FACT_MIN_AREA_SR			16
#define VISION_FACT_MIN_WIDTH_SR		4
#define VISION_FACT_MIN_HEIGHT_SR		4

#define LIGHT_CH_MAX_NUM				8

#define PATTERN_INS_NUM					2

#define SEARCH_DEVICE_IMAGE_MAP_IDX		7

//인식기 거리
#define RECOG_DIST						20

#define LENGTH_POINT_NUM			6
//Inspection parameter
typedef struct InspectPara
{
	//Base///////////////////////////////////////
	BOOL m_bParaBase[BOOL_PARA_MAX_NUM_BASE];
	int m_iParaBase[INT_PARA_MAX_NUM_BASE];//0 : COF number, 1 : Map last number
	double m_dParaBase[DBL_PARA_MAX_NUM_BASE];//0 : COF pitch, 1 : pf pitch
	int m_iParaMapBase[INT_PARA_MAP_MAX_NUM_BASE];

	//Align//////////////////////////////////////
	RECT m_InsRegionAlign[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_ALIGN];

	BOOL m_bParaAlign[BOOL_PARA_MAX_NUM_ALIGN];
	int m_iParaAlign[INT_PARA_MAX_NUM_ALIGN];//0 : pattern threshold, 1 : pattern score, 2 : pos. limit
	double m_dParaAlign[DBL_PARA_MAX_NUM_ALIGN];//0 : taught pattern x, 1 : Taught pattern y
	int m_iParaMapAlign[INT_PARA_MAP_MAX_NUM_ALIGN];//until 0

	typePatPara m_PatParaAlign;
	Hlong m_PatIDAlign, m_PatIDAlign2; // 20110531 Dongwhe*
	BOOL m_bCreatMdIDAlign;

	double m_dPatternPosXAlign[SUB_RGN_MAX_NUM][PATTERN_INS_NUM];
	double m_dPatternPosYAlign[SUB_RGN_MAX_NUM][PATTERN_INS_NUM];

	//Chip//////////////////////////////////////
	RECT m_InsRegionChip[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_CHIP];
	BOOL m_bParaChip[BOOL_PARA_MAX_NUM_CHIP];
	int m_iParaChip[INT_PARA_MAX_NUM_CHIP];//0 : out tol., 1 : out threshold, 2 : out high threshold
										   //3 : mold tol., 4 : mold threshold, 5 : mold high threshold
										   //6 : over pot ratio tol., 7 : pot threshold, 8 : no pot ratio tol.
										   //9 : pot part tol., 10 : pot part threshold, 11 : pot part high threshold
										   //12 : pot 등록영역, 13 : pot 도포범위 tol., 14 : broken threshold
										   //15 : pot2 threshold, 16 : Hide Pot Particle Tol
											//17 : 미세 out tol., 18 : 미세 out threshold, 19 : 미세 out high threshold
										   //20 : 미세 out 허용개수	: 미세 chip out 허용개수
											
											//38 : Z축 Position #1 39 : Z축 Position #2	//20190918 by 추가

	double m_dParaChip[DBL_PARA_MAX_NUM_CHIP];
	int m_iParaMapChip[INT_PARA_MAP_MAX_NUM_CHIP];//until 5
										          //0 : find chip, 1 : chip out, 2 : broken, 3 : chip이면결함
												  //4 : potting, 5 : pot particle 
	//Lead//////////////////////////////////////
	RECT m_InsRegionLead[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_LEAD];//0 : top(out), 1 : bottom(in), 2 : top(out 2 : 굵은것 왼쪽) 3 : top(out 3 : 굵은것 오른쪽)
	BOOL m_bParaLead[BOOL_PARA_MAX_NUM_LEAD];
	int m_iParaLead[INT_PARA_MAX_NUM_LEAD];//1 : top tol, 2 : top trsh 
										   //4 : bot tol, 5 : bot trsh
										   //6 : top-dirty tol, 7 : top-dirty trsh	
										   //8 : scratch tol, 9: scratch trsh
	double m_dParaLead[DBL_PARA_MAX_NUM_LEAD];//0 : top factor, 1 : bot factor, 

	int m_iParaMapLead[INT_PARA_MAP_MAX_NUM_LEAD];//until 5

	//Film//////////////////////////////////////
	RECT m_InsRegionFilm[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_FILM];
	BOOL m_bParaFilm[BOOL_PARA_MAX_NUM_FILM];
	int m_iParaFilm[INT_PARA_MAX_NUM_FILM];//0 : mold factor(mean_image), 1 : mold tol., 2 : mold thrsh.
										   //3 : PF tol. 4 : PF thrsh, 5 : PF high thrsh, 6 : mold light tol, 
										   //7 : mold user to, 8 : mold user threshold, 9 : pf edit tol
										   //10 : metal tol., 11 : metal thrsh.
										   //12 : burr tol, 13 : punch min area, 14 : punch max area
	
	double m_dParaFilm[DBL_PARA_MAX_NUM_FILM];
	int m_iParaMapFilm[INT_PARA_MAP_MAX_NUM_FILM];

	//Film var
	Hlong m_FilmVarID[SUB_RGN_MAX_NUM];
	BOOL m_bCreatFilmVarID[SUB_RGN_MAX_NUM];

	//OCV////////////////////////////////////////
	RECT m_InsRegionOCV[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_OCV];
	BOOL m_bParaOCV[BOOL_PARA_MAX_NUM_OCV];
	int m_iParaOCV[INT_PARA_MAX_NUM_OCV];//0 : pat threshold, 1 : pat score, 2 : Taught character num, 3 : var threshold, 4 : var tol
	double m_dParaOCV[DBL_PARA_MAX_NUM_OCV];
	int m_iParaMapOCV[INT_PARA_MAP_MAX_NUM_OCV];

	//Char pat
	RECT m_InsCharRegionOCV[OCV_CHAR_MAX_NUM];
	double m_dCharCenterX[OCV_CHAR_MAX_NUM];
	double m_dCharCenterY[OCV_CHAR_MAX_NUM];
	typePatPara m_PatParaOCV[OCV_CHAR_MAX_NUM];
	Hlong m_PatIDOCV[OCV_CHAR_MAX_NUM];
	BOOL m_bCreatMdIDOCV[OCV_CHAR_MAX_NUM];

	//20100702 lss
	//Punch(CAM #2)//////////////////////////////
	RECT m_InsRegionPPunch[RECT_PARA_MAX_NUM_PPUNCH];
	int m_iParaPPunch[INT_PARA_MAX_NUM_PPUNCH];//0 : min area, 1 : max area, 2 : tol

	//Char var
	Hlong m_CharVarID[OCV_CHAR_MAX_NUM];
	BOOL m_bCreatCharVarID[OCV_CHAR_MAX_NUM];

	//SR/////////////////////////////////////////
	RECT m_InsRegionSR[SUB_RGN_MAX_NUM][RECT_PARA_MAX_NUM_SR];
	//BOOL m_bParaSR[BOOL_PARA_MAX_NUM_SR];
	int m_iParaSR[INT_PARA_MAX_NUM_SR];//0 : sr factor, 1 : sr tol., 2 : sr trsh, 3 : sr cnt., 4 : sr max tol
									   //5 : pi hall tol, 6 : pi hall thresh, 7 : fold tol, 8 : fold thresh
	
	//double m_dParaSR[DBL_PARA_MAX_NUM_SR];
	int m_iParaMapSR[INT_PARA_MAP_MAX_NUM_SR];//until 1

	//Light
	int m_iLightVar[IMAGE_MAP_MAX_NUM][LIGHT_CH_MAX_NUM];

	//20141120 lby 펀칭일 때 검사 스킵
	BOOL m_bIsPunchSkip[COF_MAX_NUM];
}typeInspectPara;

//Inspection result
typedef struct InspectRslt
{
	int m_iRslt[SUB_RGN_MAX_NUM];

	//Align
	double m_dCXAlign[SUB_RGN_MAX_NUM][PATTERN_INS_NUM];
	double m_dCYAlign[SUB_RGN_MAX_NUM][PATTERN_INS_NUM];
	double m_dScoreAlign[SUB_RGN_MAX_NUM][PATTERN_INS_NUM];
	int m_iRsltAlign[SUB_RGN_MAX_NUM];
	double m_dOffsetX[SUB_RGN_MAX_NUM];
	double m_dOffsetY[SUB_RGN_MAX_NUM];

	//Chip
	Hlong m_iChipOutNum[SUB_RGN_MAX_NUM];
	Hlong m_iChipBrokenNum[SUB_RGN_MAX_NUM];
	Hlong m_iChipMoldNum[SUB_RGN_MAX_NUM];
	Hlong m_iChipPotNum[SUB_RGN_MAX_NUM];
	Hlong m_iChipPotArea[SUB_RGN_MAX_NUM];
	Hlong m_iChipPotPNum[SUB_RGN_MAX_NUM];
	Hlong m_iChipPotRNum[SUB_RGN_MAX_NUM];

	int m_iRsltChipFind[SUB_RGN_MAX_NUM];
	int m_iRsltChipOut[SUB_RGN_MAX_NUM];
	int m_iRsltChipBroken[SUB_RGN_MAX_NUM];
	int m_iRsltChipMold[SUB_RGN_MAX_NUM];
	int m_iRsltChipNPot[SUB_RGN_MAX_NUM];
	int m_iRsltChipDPot[SUB_RGN_MAX_NUM];
	int m_iRsltChipPotP[SUB_RGN_MAX_NUM];
	int m_iRsltChipPotR[SUB_RGN_MAX_NUM];

	//Film
	Hlong m_iFilmMoldNum[SUB_RGN_MAX_NUM];
	Hlong m_iFilmPFNum[SUB_RGN_MAX_NUM];
	Hlong m_iFilmMetalNum[SUB_RGN_MAX_NUM];

	int m_iRsltFilmMold[SUB_RGN_MAX_NUM];
	int m_iRsltFilmPF[SUB_RGN_MAX_NUM];
	int m_iRsltFilmMetal[SUB_RGN_MAX_NUM];

	//Lead
	Hlong m_iLeadTopNum[SUB_RGN_MAX_NUM];
	Hlong m_iLeadBotNum[SUB_RGN_MAX_NUM];
	Hlong m_iLeadTopNum2[SUB_RGN_MAX_NUM];
	Hlong m_iLeadBotNum2[SUB_RGN_MAX_NUM];
	Hlong m_iLeadTopNum3[SUB_RGN_MAX_NUM];
	Hlong m_iLeadBotNum3[SUB_RGN_MAX_NUM];
	Hlong m_iLeadAMNum[SUB_RGN_MAX_NUM];
	double m_dLeadBotWAVG[SUB_RGN_MAX_NUM];

	int m_iRsltLeadTop[SUB_RGN_MAX_NUM];
	int m_iRsltLeadBot[SUB_RGN_MAX_NUM];
	int m_iRsltAM[SUB_RGN_MAX_NUM];

	//OCV
	Hlong m_iOCVPatNum[SUB_RGN_MAX_NUM];

	int m_iRsltOCV[SUB_RGN_MAX_NUM];

	//SR
	Hlong m_iSRBubNum[SUB_RGN_MAX_NUM];
	int m_iRsltSRBub[SUB_RGN_MAX_NUM];

	Hlong m_iPIHallNum[SUB_RGN_MAX_NUM];

	//Error num
	int m_iRsltErrNum[SUB_RGN_MAX_NUM];

	//Region skip
	BOOL m_bRegionSkip[SUB_RGN_MAX_NUM];
}typeInspectRslt;

//System parameter
typedef struct SystemPara
{
	//int m_iPCIdx;//0 : PC0, 1 : PC1
	BOOL m_bPara[BOOL_PARA_MAX_NUM_SYS];
	int m_iPara[INT_PARA_MAX_NUM_SYS];
	double m_dPara[DBL_PARA_MAX_NUM_SYS];//0 : pixel size
	char m_strPara[MAX_PATH];
}typeSystemPara;

//Main result
typedef struct MainSystemRslt
{
	long m_lCnt[CNT_MAX_NUM];
}typeMainSystemRslt;

//Ng result
typedef struct InspectNGRslt
{
	int m_iRgnIdx;//몇번째 칩에 영역인지
	typeInspectRslt m_InsRslt;
}typeInspectNGRslt;

//Lot info.
typedef struct LotInfo
{
	char m_sNo[MAX_PATH];
	char m_sStart[100];
	char m_sEnd[100];
	char m_sTime[100];
	long m_lDeviceIdx;
	long m_lTot;
	long m_lGood;
	long m_lNG;
	float m_fRatio;
	long m_lPunch;
}typeLotInfo;


typedef struct VISIONRESULTDATA	//20191101 by IMSI TEST필요
{
	typeInspectRslt m_InsRslt;
	Hobject m_HalNGRsltRgn[SUB_RGN_MAX_NUM][RESULT_INS_MAX_NUM];		//불량 영역

}typeVisionResultData;

class AFX_EXT_CLASS C_VisionData  
{
public:
	C_VisionData();
	virtual ~C_VisionData();

	//Master image
	Hobject m_HalMasterImage;
	Hobject m_HalSRMasterImage;
	Hobject m_HalLeadMasterImage;
	Hobject m_HalSROutMasterImage;
	Hobject m_HalChipMasterImage;
	Hobject m_HalTaughtImage[IMAGE_MAP_MAX_NUM];

	//Halcon image
	Hobject m_HalImage[IMAGE_BUF_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalRedImage[IMAGE_BUF_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalGreenImage[IMAGE_BUF_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalBlueImage[IMAGE_BUF_NUM][IMAGE_MAP_MAX_NUM];
	
	//Halcon NG image
	Hobject m_HalNGImage[NG_VIEW_MAX_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalNGRedImage[NG_VIEW_MAX_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalNGGreenImage[NG_VIEW_MAX_NUM][IMAGE_MAP_MAX_NUM];
	Hobject m_HalNGBlueImage[NG_VIEW_MAX_NUM][IMAGE_MAP_MAX_NUM];

	//Inspect region
	Hobject m_HalInsRgn[SUB_RGN_MAX_NUM][INS_RGN_MAX_NUM];//0 : SR inspect region, 1 : PF hall region, 
														  //2 : up lead, 3 : dn lead, 4 : user area, 5 : SR-out
														  //6 : vaccum region, 7 : chip region, 8 : up left region, 9 : up right region

	//Inspection parameter
	typeInspectPara m_InsPara;

	//Inspection result
	typeInspectRslt m_InsRslt;

	//System parameter
	typeSystemPara m_SysPara;

	//Result region
	Hobject m_HalRsltRgn[SUB_RGN_MAX_NUM][RESULT_INS_MAX_NUM];
	//Disp result region
	Hobject m_HalNGRsltRgn[NG_VIEW_MAX_NUM][SUB_RGN_MAX_NUM][RESULT_INS_MAX_NUM];

	//Pot length 20150818 by 
	Hobject m_HalPotRsltRgn[SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];
	Hobject m_HalNGPotRsltRgn[NG_VIEW_MAX_NUM][SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];
	
	//Pot length 20150903 by 
	int  m_nPotRsltAverageLength[SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];
	int  m_nPotRsltCenterLength[SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];
	int  m_nNGPotRsltAverageLength[NG_VIEW_MAX_NUM][SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];
	int  m_nNGPotRsltCenterLength[NG_VIEW_MAX_NUM][SUB_RGN_MAX_NUM][LENGTH_POINT_NUM];

	//NG inspection result
	typeInspectNGRslt m_InsNGRslt[NG_VIEW_MAX_NUM];
};



class AFX_EXT_CLASS C_VisionModule  
{
public:
	//20191101 by IMSI TEST필요
	list<typeVisionResultData> m_listRsltData;
	void PushRsltData(int cam_idx);
	void ClearNGRsltData();
	void SetInspectNGRsltRgn2(int cam_idx, int ng_view_idx);
	typeInspectRslt GetInspRslt();
	int GetListSize();
	void NGListPopFront();
	////////////////////////////////////////////////////////

	double m_dPixelSizeData;
	void SetNGViewMode(BOOL mode, int ng_view_idx);
	BOOL SaveInspectNGRslt(int cam_idx, int ng_view_idx, int rgn_idx, CString path);
	BOOL LoadInspectNGRslt(int cam_idx, int ng_view_idx, CString path);
	int GetMapNum(int cam_idx);
	int GetCOFNum(int cam_idx);
	int Inspection(int cam_idx, int buf_idx);//상부검사
	int InspectionFirstAlign(int cam_idx, int buf_idx);
	int InspectionAlign(int cam_idx, int buf_idx);
	BOOL InspectionPunchCheck(int cam_idx, int buf_idx);//20150601 by
	int InspectionAlignForAutoOCVTeach(int cam_idx, int buf_idx, Hobject *mod_image, Hobject *shape, 
												   double *rx, double *ry, double *rscore, Hlong *rn,
												   double *offset_x, double *offset_y);

	int InspectionChip(int cam_idx, int buf_idx, BOOL find_chip_use, BOOL chip_out_use, BOOL chip_mold_use, BOOL chip_pot_use);
	int InspectionFilm(int cam_idx, int buf_idx, BOOL film_mold_use, BOOL film_pf_use, BOOL film_metal_use);
	int InspectionLead(int cam_idx, int buf_idx, BOOL out_lead_use, BOOL out_lead_dirt_use, BOOL in_lead_use,BOOL in_lead_dirt_use, BOOL out_lead2_use, BOOL am_use0, BOOL am_use1, BOOL lead_scat_hori);
	int InspectionOCV(int cam_idx, int buf_idx, BOOL ocv_use);

	//20141116 lby BOTTOM 추가
	int Inspection_Bottom(int cam_idx, int buf_idx);//하부검사
	int InspectionSR_Bottom(int cam_idx, int buf_idx);
	int InspectionAlign_Bottom(int cam_idx, int buf_idx);

	//20190916 by LINESCAN 추가
	int Inspection_Linescan(int cam_idx, int buf_idx);
	int InspectionAlign_Linescan(int cam_idx, int buf_idx);
	int InspectionChip_Linescan(int cam_idx, int buf_idx, BOOL find_chip_use, BOOL chip_out_use, BOOL chip_mold_use, BOOL chip_pot_use);
	int InspectionAlignMark(int cam_idx, int buf_idx, BOOL am_use0, BOOL am_use1);

	C_VisionModule();
	virtual ~C_VisionModule();

	//Get/Set data
	Hobject *GetHalImage(int cam_idx, int buf_idx, int img_idx);
	Hobject *GetHalRedImage(int cam_idx, int buf_idx, int img_idx);
	Hobject *GetHalGreenImage(int cam_idx, int buf_idx, int img_idx);
	Hobject *GetHalBlueImage(int cam_idx, int buf_idx, int img_idx);

	Hobject *GetHalMasterImage(int cam_idx);
	Hobject *GetHalSRMasterImage(int cam_idx);
	Hobject *GetHalLeadMasterImage(int cam_idx);
	Hobject *GetHalSROutMasterImage(int cam_idx);
	Hobject *GetHalChipMasterImage(int cam_idx);
	Hobject *GetHalTaughtImage(int cam_idx, int map_idx);
	
	void SetHalNGImage(int cam_idx, int buf_idx, int ng_view_idx);
	Hobject *GetHalNGImage(int cam_idx, int ng_view_idx, int img_idx);

	typeInspectPara GetInspectPara(int cam_idx);
	void SetInspectPara(int cam_idx, typeInspectPara para);
	void ResetInspectPara();

	//Inspect region
	Hobject *GetHalInspectRgn(int cam_idx, int rgn_idx, int ins_idx);
	void ResetInspectRgn(int cam_idx);

	

	//Inspect////////////////////////////////////
	//Result region
	Hobject *GetHalRsltRgn(int cam_idx, int rgn_idx, int ins_idx);
	void ResetInspectRsltRgn(int cam_idx);

	//Inspect result data
	typeInspectRslt GetInspectRslt(int cam_idx);
	void SetInspectRslt(int cam_idx, typeInspectRslt rslt);
	void ResetInspectRslt(int cam_idx);
	/////////////////////////////////////////////

	//NG/////////////////////////////////////////
	//NG result region
	Hobject *GetHalNGRsltRgn(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx);
	Hobject *GetHalNGPotRsltRgn(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx);//Pot length 20150818 by
	int GetHalNGPotRsltAverageLength(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx);//Pot length 20150903 by
	int GetHalNGPotRsltCenterLength(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx);//Pot length 20150903 by


	void ResetInspectNGRsltRgn(int cam_idx);
	void SetInspectNGRsltRgn(int cam_idx, int ng_view_idx);	
	void SaveInspectNGRslt(int cam_idx, int ng_view_idx, int rgn_idx);

	//NG result data
	typeInspectNGRslt GetInspectNGRslt(int cam_idx, int ng_view_idx);
	void SetInspectNGRslt(int cam_idx, int ng_view_idx, /*typeShiftRslt shift_rslt,*/ typeInspectRslt ins_rslt);
	void ResetInspectNGRslt(int cam_idx);
	/////////////////////////////////////////////

	typeSystemPara GetSystemPara(int cam_idx);
	void SetSystemPara(int cam_idx, typeSystemPara rslt);
	void ResetSystemPara();

	typeMainSystemRslt GetMainSysRslt();
	void SetMainSysRslt(typeMainSystemRslt rslt);
	void ResetMainSysRslt();

	typeLotInfo GetLotInfo();
	void SetLotInfo(typeLotInfo lot);
	void ResetLotInfo();

	//Load/save inspection parameter
	BOOL LoadInspectPara(CString path);
	BOOL SaveInspectPara(CString path);
	BOOL LoadInspectSelectPara(CString path);
	BOOL SaveInspectSelectPara(CString path);

	void MakeDefaultInspPara();
	void WriteErrorList(LPCTSTR lpszItem);	
	void WriteNotAgreeInspParam(CString ChangeParamName, int notagreepara);
	BOOL WriteErrorStatus(CString path, CString file, CString bak_file, CString log, long limit_line);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Load/save system parameter
	BOOL LoadSystemPara(CString path);
	BOOL SaveSystemPara(CString path);

	//Load/save system result
	BOOL LoadSystemRslt(CString path);
	BOOL SaveSystemRslt(CString path);

	//Vision index 
	void SetVisIndex(int vis_idx) { m_iVisIdx = vis_idx; }// 20141111 lby
	int GetVisIndex() { return m_iVisIdx; } // 20141111 lby

	BOOL FindPuchingChip(Hlong win, Hobject *image, Hobject sr_region, RECT ins_region, RECT ins_chip_region, RECT dont_care_region,
								 int pf_edit_thresh, int pf_edit_tol, int pf_pitch,
								 int min_punch_area, int max_punch_area, int bur_tol, int pos_error, int pos_error2, int edit_thresh, int edit_tol,
								 int pf_Count, BOOL *PuchErr, Hobject *rslt_region, Hlong *rslt_area); 

	BOOL FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region);

	BOOL _Hal_FilmMold(Hlong win, Hobject *mold_image, Hobject *ink_image, Hobject *user_image, Hobject *scratch_image,
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
							  Hlong var_id, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int dark_thresh, int dark_bright_tol, int white_thresh, int white_bright_tol, int mold_fact, int tolerance, int tolerance_light,
							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region,
							  Hlong min_area, Hlong min_width, Hlong min_height,
							  Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr);

	BOOL _Hal_FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region);

	BOOL _Hal_ChipPot(Hlong win, Hobject *image, RECT ins_region, Hobject chip_ins_region1, 
				 double offset_ox, double offset_oy, 
				 double offset_ox2, double offset_oy2, 
				 double offset_rx, double offset_ry,
				 double offset_rx2, double offset_ry2, Hlong pot_area, int thresh, int under_tol, int over_tol, int line_percent, 
				  int pot_width_1, int pot_width_2, int pot_width_3, int pot_width_4, int pot_width_5, int pot_width_6, int average_x, int average_y, Hobject *rslt_pot_region,
				 Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region, int *rslt_average_leng, int *rslt_center_leng);

	BOOL _Hal_FilmPF(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int black_thresh, int black_tolarance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr);

	BOOL _Hal_FilmPF2(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int area, int black_thresh, int black_tolerance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr);

	BOOL _Hal_InLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   double* line_width_avg, 
						   Hlong *rslt_num, Hobject *rslt_region,  
						   Hlong *rslt_num2, Hobject *rslt_region2,
						   Hobject *rslt_dn_region,
						   Hlong min_area, Hlong min_width, Hlong min_height);

	BOOL _Hal_InLead2(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, 
							Hobject *image4, Hobject *image5, Hobject *image6, Hobject *image7,
							Hobject *image8,
							RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
							int thres, double factor, int tolerance, 
							double* line_width_avg, 
							Hlong *rslt_num, Hobject *rslt_region,  
							Hlong *rslt_num2, Hobject *rslt_region2,
							Hobject *rslt_dn_region,
						    Hlong min_area, Hlong min_width, Hlong min_height);

	//Inspection
	BOOL  _Hal_OutLead(Hlong win, Hobject *image, Hobject *image2, Hobject *image3,
							RECT ins_region, RECT ins_region2, RECT ins_region3,
							Hobject not_sr_region, 
							Hobject up_region,
							Hobject up_region2,
							Hobject up_region3,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int thresh, double factor, int tolerance, 
						   int thresh_dirt, int tolerance_dirt,
						   BOOL out_lead_use, BOOL out_lead_dirt_use, BOOL use_out_hori,
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hlong *rslt_num2, Hobject *rslt_region2, 						   
						   Hobject *rslt_up_region,
						   Hlong min_area, Hlong min_width, Hlong min_height);

	//SR 기포검사
	BOOL SRBub(Hlong win, Hobject *image, Hobject *hall_image, Hobject *Green_image, Hobject *RefGreen_image,  Hobject *RefBlue_image, 
							Hobject *ink_chip_image,
							RECT ins_sr_region, RECT ins_chip_region, RECT chip_search_region,
						   double offset_ox, double offset_oy, double offset_rx, double offset_ry,
						   int abs_thresh, int var_thresh, int mold_fact, int tolerance, 
						   int tol_max, int tol_cnt,
						   int hall_thresh, int hall_tol,
						   int fold_thresh, int fold_tol,
						   int ink_chip_size, int ink_thresh, int ink_area_tol, int ink_factor,
			               Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region,
						   Hlong *rslt_hall_num, Hobject *rslt_hall_region, Hobject *rslt_hallins_region);

	BOOL PotPart(Hlong win, Hobject *image, Hobject *image2, Hobject *crack_image, Hobject chip_ins_region, Hobject pot_ins_region, Hobject sr_region,
		double offset_ox, double offset_oy, 
		double offset_ox2, double offset_oy2, 
		double offset_rx, double offset_ry,
		double offset_rx2, double offset_ry2,
		int part_thresh, int part2_thresh, int high_thresh, int tolerance, int range_tolerance, int black_pot_tol, int black_thresh,
		Hlong *rslt_num, Hobject *rslt_region, Hlong *range_rslt_num, Hobject *range_rslt_region, Hobject *recog_ins_rslt_region);

	//칩의 처음위치를 검색
	//20140513 lss
	BOOL FindFirstChip(Hlong win, Hobject *image, int thresh, Hlong *chip_offset);


	BOOL ChipOut(Hlong win, Hobject *image,
		Hobject chip_ins_region, Hobject chip_ins_region1, 
				 double offset_ox, double offset_oy, 
				 double offset_ox2, double offset_oy2, 
				 double offset_rx, double offset_ry,
				 double offset_rx2, double offset_ry2,
				 int thresh, int tolerance_x, int tolerance_y, int edgesize,
				 int mthresh, int mtolerance_x, int mtolerance_y,
				 int sthresh, int stolerance_x, int stolerance_y,
				 Hlong *rslt_num, Hobject *rslt_region);
	
	BOOL ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, Hobject chip_ins_region_cur,
				 double offset_ox, double offset_oy, 
				 double offset_ox2, double offset_oy2, 
				 double offset_rx, double offset_ry,
				 double offset_rx2, double offset_ry2, int high_thresh, int tolerance, int broken_thresh, double broken_sigma,
				 Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region);
	
	BOOL ChipPot(Hlong win, Hobject *image, RECT ins_region, Hobject chip_ins_region1, 
				 double offset_ox, double offset_oy, 
				 double offset_ox2, double offset_oy2, 
				 double offset_rx, double offset_ry,
				 double offset_rx2, double offset_ry2,
				 Hlong pot_area, int thresh, int under_tol, int over_tol, int line_percent, 
				 int pot_width_1, int pot_width_2, int pot_width_3, int pot_width_4, int pot_width_5, int pot_width_6, int average_x, int average_y, Hobject *rslt_pot_region,
				 Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region, int *rslt_average_leng, int *rslt_center_leng);
	
	BOOL FilmMold(Hlong win, Hobject *mold_image, 
		Hobject *ink_image, Hobject *pf_image, Hobject *scratch_image, 
				  RECT ins_region, RECT chip_region, RECT ocv_region, Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
				  Hlong var_id, 
				  double offset_ox, double offset_oy, 
				  double offset_ox2, double offset_oy2, 
				  double offset_rx, double offset_ry,
				  double offset_rx2, double offset_ry2,
				  int dark_thresh, int dark_bright_tol, int white_thresh, int white_bright_tol, int mold_fact, int tolerance, int tolerance_light,
				  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region);
	
	BOOL FilmPF(Hlong win, Hobject *white_image, Hobject *black_image,			   
							RECT ins_region, RECT chip_region, Hobject rf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_tx, double offset_ty,
						   double offset_tx2, double offset_ty2,
						   int thresh, int tolerance, int area, int black_thresh, int black_tolarance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region);
	BOOL InLead(Hlong win, Hobject *image, Hobject *crack_image, RECT ins_region, Hobject not_sr_region,	  
				Hobject dn_region,
				double offset_ox, double offset_oy, 
				double offset_ox2, double offset_oy2, 
				double offset_tx, double offset_ty,
				double offset_tx2, double offset_ty2,
				int threshold, double factor, int tolerance, 
				double* line_width_avg, 
				Hlong *rslt_num, Hobject *rslt_region, 
				Hlong *rslt_num2, Hobject *rslt_region2,
				Hobject *rslt_dn_region);

	BOOL OutLead(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, RECT ins_region, RECT ins_region2, RECT ins_region3,
							Hobject not_sr_region,	 
							Hobject up_region,
							Hobject dn_region,
							Hobject up_region2,
							Hobject up_region3,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   int threshold_dirt, int tolerance_dirt,
						   BOOL out_lead_use, BOOL out_lead_dirt_use, BOOL use_out_hori,
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hlong *rslt_num2, Hobject *rslt_region2, 
						   Hobject *rslt_up_region);

	//20160520 리턴형 BOOL -> int 변경
	int AMLead(Hlong win, Hobject *image,
							BOOL use_0,
							BOOL use_1,
							Hobject region_0_0,
							Hobject region_0_1,
							Hobject region_1_0,
							Hobject region_1_1,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold_1, int tolerance_1,
						   int teached_average_light_1, int insp_average_light_1,
						   int threshold_2, int tolerance_2,
						   int teached_average_light_2, int insp_average_light_2, 
						   int threshold_3, int tolerance_3,
						   int teached_average_light_3, int insp_average_light_3,
						   int threshold_4, int tolerance_4,
						   int teached_average_light_4, int insp_average_light_4, 
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hobject *rslt_ins_region_0_0, Hobject *rslt_ins_region_0_1, Hobject *rslt_ins_region_1_0, Hobject *rslt_ins_region_1_1);

	//20150511 by
	BOOL _Hal_RegionShift(Hobject region_1, Hobject region_2, Hobject *result_region, int steps);
	
	//Chip out 모서리
	BOOL _Hal_ChipOut(Hlong win, Hobject *image, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int tolerance_x, int tolerance_y, int edgesize, 
							 int mthresh, int mtolerance_x, int mtolerance_y,
							 int sthresh, int stolerance_x, int stolerance_y,
							 Hlong *rslt_num, Hobject *rslt_region);

	//20100416 lee
	BOOL _Hal_ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, Hobject chip_ins_region_cur,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int high_thresh, int tolerance, int broken_thresh, double broken_sigma,
							  Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region,
							  Hlong min_area, Hlong min_width, Hlong min_height);
private:

	//Vision data class
	C_VisionData *m_VDat[IMG_RECT_MAX_NUM];

	//Main
	typeMainSystemRslt m_MainSysRslt;

	//Window
	Hlong m_lWindow[IMG_RECT_MAX_NUM];

	//Lot
	typeLotInfo m_Lot;

	//NG view mode
	int m_iNGViewMode;

	//20141111 lbyVision index 
	int m_iVisIdx;
};

#endif // !defined(AFX__VISIONMODULE_H__C04D73F5_409A_4585_B7AD_8FC556FE83ED__INCLUDED_)