// _VisionModule.cpp: implementation of the C_VisionModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "include/_FuncModuleBasic.h"
#include "include/_FuncModuleHal.h"
#include "_VisionModuleFilmIns.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Vision Data
//////////////////////////////////////////////////////////////////////
C_VisionData::C_VisionData()
{

}

C_VisionData::~C_VisionData()
{

}

/////////////////////////////////////////////////
//Inspection function
//
BOOL C_VisionModule::FindPuchingChip(Hlong win, Hobject *image, Hobject sr_region, RECT ins_region, RECT ins_chip_region, RECT dont_care_region,
									 int pf_edit_thresh, int pf_edit_tol, int pf_pitch,
									 int min_punch_area, int max_punch_area, int bur_tol, int pos_error, int pos_error2, int edit_thresh, int edit_tol,
									 int pf_Count, BOOL *PuchErr, Hobject *rslt_region, Hlong *rslt_area)
{	//20100115LSS
	Hobject ConvxRegion2, TransRegion, ConvxRegion, Region, ImageReduced, ConnectedRegions, SelectedRegions, CircleRegion, DontCareRegion;
	HTuple AreaCheck1, AreaCheck2, Num, CRow0,CCol0,CRow1,CCol1, Row, Col, Row2, Col2, Dist; 
	Hlong DiffCheckArea = 0;
	Herror herr;
	Hlong chip_cen_posx, chip_cen_posy, Area;
	Hlong wd, ht;

	HTuple _area, _row, _col;

	int thresh;
	double CRow,CCol;

	CString status;

	BOOL isCircle = FALSE;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_21!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	gen_rectangle1(&DontCareRegion, dont_care_region.top,
														dont_care_region.left,
														dont_care_region.bottom,
														dont_care_region.right);

	thresh = pf_edit_thresh;
	if(0 > thresh)
	{
		thresh = 0;
	}
	else if(254 < thresh)
	{
		thresh = 254;
	}

	*PuchErr = FALSE;

	//20170912 by 
	reduce_domain(*image, DontCareRegion, &ImageReduced);

	threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);

	//원형 펀치 검사 //20100725 lee
	select_shape(ConnectedRegions,&SelectedRegions, HTuple("width").Concat("height"), HTuple("and"), HTuple(150).Concat(150), HTuple(300).Concat(300));
	select_shape(SelectedRegions,&CircleRegion,HTuple("circularity"),HTuple("and"),HTuple(0.7),HTuple(1.0)); //20110530 lee 0.9->0.7
	count_obj(CircleRegion, &Num);

	if(1 == (Hlong)Num[0])  //한개 이상 있다? 그럼 원형 펀치!! 이건 위치 않보고 그냥 펀치!
	{
		return TRUE;
	}
	/////////////////////////////////////////////

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);


	//Punching inspection////////////////////////
	erosion_rectangle1(Region, &Region, 100, 3);

	reduce_domain(*image,Region,&ImageReduced);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	set_check("~give_error");

	//20141111 lby
	switch(m_iVisIdx)
	{
	case VISION_CAM_TOP1:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));
		break;
	case VISION_CAM_TOP2:
		herr = threshold(ImageReduced,&Region,HTuple(0), HTuple(thresh));
		break;
	case VISION_CAM_BOTTOM:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));
		break;
	default:
		AfxMessageBox("C_VisionModule::FindPuchingChip() Failed ! Unknown Vision Index.");
		return FALSE;
		break;
	}

	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_19!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);

	//원형 펀치 검사 //20100725 lee
	select_shape(ConnectedRegions,&SelectedRegions, HTuple("width").Concat("height"), HTuple("and"), HTuple(150).Concat(150), HTuple(500).Concat(500));
	select_shape(SelectedRegions,&CircleRegion,HTuple("circularity"),HTuple("and"),HTuple(0.7),HTuple(1.0)); //20110530 lee 0.9->0.7
	count_obj(CircleRegion, &Num);
	if(1 == (Hlong)Num[0])  //한개 이상 있다? 그럼 원형 펀치!! 이건 위치 않보고 그냥 펀치!
	{
		//원형 Punch가 있는데 사각 Punch가 없으면 우선 불량..향후 원형만 있을 경우 조치 필요함. //20100811 lee
		//사각 펀치 검사 
 		select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area));
		count_obj(SelectedRegions, &Num);

		if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
		{
			*PuchErr = TRUE;  //원형 Punch긴 하나 사각 Punch가 없으므로 편집 불량으로 간다. //20100811 lee
			return TRUE;  //Punch 면 TRUE
		}
		else
		{
			isCircle = TRUE;
		}
		//Circle Punching
		//return TRUE;    //Punch 면 TRUE
	}
	//원형 펀치 검사 //20100725 lee

	//사각 펀치 검사 
	//select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area));
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple("max"));


	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(SelectedRegions, win);
	}

	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		return FALSE;  //Punch 아니며 FALSE
	}

	//20150702 by
	//3PF 검사 시 오류로 인하여 임시 막아둠.
	//////////////////////////////////////////////////////////////////////////////////////////
	//PF위치기준으로 편집 불량 검출
	//
	/*
	int high_threshold, low_threshold;
	Hobject UP_PFRegion, DN_PFRegion, UP_ImageReduced, DN_ImageReduced, UP_RsltRegion, DN_RsltRegion;
	Hobject UP_DN_DiffRegion;
	HTuple UP_Row0 ,UP_Col0, UP_Row1, UP_Col1;
	HTuple DN_Row0 ,DN_Col0, DN_Row1, DN_Col1;
	Hlong num;

	gen_rectangle1(&UP_DN_DiffRegion, ins_region.top, 
								 ins_chip_region.left - ((ins_chip_region.right - ins_chip_region.left) * 0.5), 
								 ins_region.bottom, 
								 ins_chip_region.right + ((ins_chip_region.right - ins_chip_region.left) * 0.5));

	gen_rectangle1(&UP_PFRegion, ins_region.top, 
								 ins_region.left, 
								 ins_region.top + 200, 
								 ins_region.right);

	gen_rectangle1(&DN_PFRegion, ins_region.bottom - 200, 
								 ins_region.left, 
								 ins_region.bottom, 
								 ins_region.right);
	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(UP_PFRegion, win);
		disp_obj(DN_PFRegion, win);
	}
	
	difference(UP_PFRegion, UP_DN_DiffRegion, &UP_PFRegion);
	difference(DN_PFRegion, UP_DN_DiffRegion, &DN_PFRegion);
	reduce_domain(*image, UP_PFRegion, &UP_ImageReduced);
	reduce_domain(*image, DN_PFRegion, &DN_ImageReduced);

	high_threshold = edit_thresh;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 1;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 1;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}

	threshold(UP_ImageReduced, &UP_RsltRegion, HTuple(pf_edit_thresh), HTuple(255));
	closing_circle(UP_RsltRegion, &UP_RsltRegion,HTuple(3.5));
	connection(UP_RsltRegion, &UP_RsltRegion);
	select_shape(UP_RsltRegion, &UP_RsltRegion, HTuple("width"), HTuple("and"), HTuple(30), HTuple("max"));
	union1(UP_RsltRegion, &UP_RsltRegion);
	closing_circle(UP_RsltRegion, &UP_RsltRegion, HTuple(10.5));

	threshold(DN_ImageReduced, &DN_RsltRegion, HTuple(pf_edit_thresh), HTuple(255));
	closing_circle(DN_RsltRegion, &DN_RsltRegion,HTuple(3.5));
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("width"), HTuple("and"), HTuple(30), HTuple("max"));
	union1(DN_RsltRegion, &DN_RsltRegion);
	closing_circle(DN_RsltRegion, &DN_RsltRegion, HTuple(10.5));

	smallest_rectangle1(UP_RsltRegion, &UP_Row0, &UP_Col0, &UP_Row1, &UP_Col1);
	smallest_rectangle1(DN_RsltRegion, &DN_Row0, &DN_Col0, &DN_Row1, &DN_Col1);

	num = 0;
	num = (Hlong)UP_Row0.Num();
	if(0 < num)
	{
		gen_rectangle1(&UP_PFRegion, (Hlong)UP_Row1[0] + 15,//25, 
									 ins_region.left + 475,
									 (Hlong)UP_Row1[0] + 17,//35, 
									 ins_region.right - 475);
	}
	else
	{
		union2(UP_PFRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}
	
	num = 0;
	num = (Hlong)DN_Row0.Num();
	if(0 < num)
	{
		gen_rectangle1(&DN_PFRegion, (Hlong)DN_Row0[0] - 17,//35, 
								 ins_region.left + 475,
								 (Hlong)DN_Row0[0] - 15,//25, 
								 ins_region.right - 475);
	}
	else
	{
		union2(DN_PFRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}
	
	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(UP_PFRegion, win);
		disp_obj(DN_PFRegion, win);
	}

	reduce_domain(*image, UP_PFRegion, &UP_ImageReduced);
	reduce_domain(*image, DN_PFRegion, &DN_ImageReduced);

	hysteresis_threshold(UP_ImageReduced, &UP_RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));
	hysteresis_threshold(DN_ImageReduced, &DN_RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));

	closing_circle(UP_RsltRegion, &UP_RsltRegion, HTuple(3.5));
	connection(UP_RsltRegion, &UP_RsltRegion);
	select_shape(UP_RsltRegion ,&UP_RsltRegion, HTuple("height"), HTuple("and"), HTuple(2), HTuple("max"));
	select_shape(UP_RsltRegion ,&UP_RsltRegion, HTuple("width"), HTuple("and"), HTuple(edit_tol), HTuple("max"));

	num = 0;
	count_obj(UP_RsltRegion, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(UP_RsltRegion, &UP_RsltRegion);
		union2(UP_RsltRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}

	closing_circle(DN_RsltRegion, &DN_RsltRegion, HTuple(3.5));
	connection(DN_RsltRegion, &DN_RsltRegion);
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("height"), HTuple("and"), HTuple(2), HTuple("max"));
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("width"), HTuple("and"), HTuple(edit_tol), HTuple("max"));

	count_obj(DN_RsltRegion, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(DN_RsltRegion, &DN_RsltRegion);
		union2(DN_RsltRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}

	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(UP_RsltRegion, win);
		disp_obj(DN_RsltRegion, win);
	}
	*/
	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////


	//이하 Punch가 있는 경우 
	//punch 중심 위치와 Chip 중심 위치를 확인하여 편차발생 시 불량 

	//20150210 by 펀칭이 2개 이상일 경우 Halcon error 수정
	//area_center(SelectedRegions, &Area,&CRow,&CCol);
	area_center(SelectedRegions, &_area, &_row, &_col);	
	Area = (Hlong)_area[0];
	CRow = (double)_row[0];
	CCol = (double)_col[0];

	*rslt_area = (Hlong)Area;

	chip_cen_posx = ins_chip_region.left + abs(ins_chip_region.right - ins_chip_region.left) / 2;

	if(abs(CCol - chip_cen_posx) > pos_error2) 
	{	
		copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}

	chip_cen_posy = ins_chip_region.top + abs(ins_chip_region.bottom - ins_chip_region.top) / 2; 

	if(abs(CRow - chip_cen_posy) > pos_error)
	{	
		copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}

	if((Hlong)Area > max_punch_area) 
	{	
		copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}

	//Punch burr검사/////////////////////////////
	union1(SelectedRegions, &SelectedRegions);	
	shape_trans(SelectedRegions, &ConvxRegion, "convex");
	shape_trans(SelectedRegions, &TransRegion, "rectangle1");
	erosion_circle(ConvxRegion, &ConvxRegion2, 5.5);
	difference(ConvxRegion2, SelectedRegions, &SelectedRegions);
	connection(SelectedRegions, &SelectedRegions);
	select_shape(SelectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(bur_tol),HTuple("max"));
	
	union1(SelectedRegions, &SelectedRegions);
	count_obj(SelectedRegions, &Num);
	
	if(0 < (Hlong)Num[0]) 
	{
		area_center(ConvxRegion, &AreaCheck1,_,_);
		area_center(TransRegion, &AreaCheck2,_,_);
		DiffCheckArea = (Hlong)AreaCheck2[0] - (Hlong)AreaCheck1[0];
		
		if(1000 < DiffCheckArea) 
		{
			//편집 불량으로 처리 
			smallest_rectangle1(SelectedRegions,&CRow0,&CCol0,&CRow1,&CCol1);
			
			if(((Hlong)CRow1[0]-(Hlong)CRow0[0])> 50)
			{
				copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
				*PuchErr = TRUE;
				return TRUE;  //Punch 면 TRUE
			}
		}
		else
		{
			//편집 불량으로 처리 
			copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
			*PuchErr = TRUE;
			return TRUE;  //Punch 면 TRUE
		}	
	}

	Hobject CheckDoublePunchRgn, FillupDoublePunchRgn, ThreshDoublePunch, ConnectedRegionsDoublePunch;
	Hobject SelectedRegionsDoublePunch;
	int nCheckDoublePunch;

	//20150313 SR 티칭 영역으로 펀치 영역이 끊어질 경우 대비. by
	shape_trans(sr_region, &FillupDoublePunchRgn, "convex");
	fill_up(FillupDoublePunchRgn,&FillupDoublePunchRgn);
	reduce_domain(*image,FillupDoublePunchRgn,&CheckDoublePunchRgn);
	threshold(CheckDoublePunchRgn,&ThreshDoublePunch,HTuple(210),HTuple(255));

	if(isCircle)
	{
		intersection(DontCareRegion, CircleRegion, &CircleRegion);
		dilation_circle(CircleRegion, &CircleRegion, HTuple(10));
		difference(ThreshDoublePunch, CircleRegion, &ThreshDoublePunch);//사각펀치가 있으면 원형영역은 제거
	}

	connection(ThreshDoublePunch, &ConnectedRegionsDoublePunch);
	nCheckDoublePunch = bur_tol-50;
	if((bur_tol-50)<1)
	{
		nCheckDoublePunch = 50;
	}
	select_shape(ConnectedRegionsDoublePunch,&SelectedRegionsDoublePunch,HTuple("area"),HTuple("and"),HTuple(nCheckDoublePunch),HTuple("max"));
	count_obj(SelectedRegionsDoublePunch, &Num);
	
	if(1 < (Hlong)Num[0]) 
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(SelectedRegions, win);
		}

		//편집 불량으로 처리 
		copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
			
	}
	

	return TRUE;
}

BOOL C_VisionModule::FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region)
{
	BOOL rslt = TRUE;

	rslt = _Hal_FindChip(win, image, ins_region, rslt_chip_region);

	return rslt;
}

BOOL C_VisionModule::FindFirstChip(Hlong win, Hobject *image, int thresh, Hlong *chip_offset)
{
	BOOL find;
	int i, j;
	HTuple Width, Height, Pointer, Type;
	BYTE *pointer;
	Hlong idx, pos;
	int width, height;

	//Find chip(위에서 아래로 검사 밝은 부분이 있는지 검사)
	get_image_pointer1(*image, &Pointer, &Type, &Width, &Height);
	pointer = (BYTE*)((Hlong)Pointer[0]);
	pos = 0;
	width = (Hlong)Width[0];
	height = (Hlong)Height[0];

	for(i = (width - 1) ; i > 0; i--)
	{	
		find = TRUE;

		for(j = 0; j < 2; j++)
		{
			idx = i + ((400 + (j * 100)) * width);
			if( (*(pointer + idx) < thresh) && 
				(*(pointer + idx + (10 * width)) < thresh) && 
				(*(pointer + idx + (20 * width)) < thresh) && 
				(*(pointer + idx + (30 * width)) < thresh) && 
				(*(pointer + idx + (40 * width)) < thresh) && 
				(*(pointer + idx + (50 * width)) < thresh) && 
				(*(pointer + idx + (60 * width)) < thresh) && 
				(*(pointer + idx + (70 * width)) < thresh) && 
				(*(pointer + idx + (80 * width)) < thresh) && 
				(*(pointer + idx + (90 * width)) < thresh) && 
				(*(pointer + idx + (100 * width)) < thresh) )
			{
				find = FALSE;
				break;
			}
		}

		if(find)
		{
			//찾았음
			pos = i;
			break;
		}
	}	
	if(find)
	{
		if(win)
		{
			//찾았음
			set_color(win, HTuple("green"));
			for(i = 0 ; i < 2; i++)
			{
				disp_line(win, (400 + (i * 100)), 0, (400 + (i * 100)), pos);
			}

		}
	}
	else
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			for(i = 0 ; i < 2; i++)
			{
				disp_line(win, (400 + (i * 100)), 0, (400 + (i * 100)), width);
			}

		}
	}

	*chip_offset = pos;

	return find;
}

BOOL C_VisionModule::ChipOut(Hlong win, Hobject *image,
							 Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int thresh, int tolerance_x, int tolerance_y, int edgesize, 
							  int mthresh, int mtolerance_x, int mtolerance_y,
							  int sthresh, int stolerance_x, int stolerance_y,
							  Hlong *rslt_num, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	HTuple Num;
	Hlong count1 = 0, count2 = 0, count3 = 0;
	Hobject rgn1, rgn2, rgn3;


	rslt = _Hal_ChipOut(win, image, chip_ins_region, chip_ins_region1,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							 thresh, tolerance_x, tolerance_y, edgesize,
							 mthresh, mtolerance_x, mtolerance_y,
							 sthresh, stolerance_x, stolerance_y,
							 rslt_num, rslt_region);

	return rslt;
}


BOOL C_VisionModule::_Hal_ChipOut(Hlong win, Hobject *image, Hobject chip_ins_region/*Chip region현재 찾은 영역*/, Hobject chip_ins_region1/*chip region(셋업시 등록된 영역)*/,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int thresh, int tolerance_x, int tolerance_y, int edgesize, 
							  int mthresh, int mtolerance_x, int mtolerance_y,
							  int sthresh, int stolerance_x, int stolerance_y,
							  Hlong *rslt_num, Hobject *rslt_region)
{
	Herror herr;
	CString status;
	BOOL result_out = TRUE;
	Hlong cnt = 0;

	Hobject ImageReduced, Region, RegionClosing, RegionIntersecEdge, InsRgn;//, ChipRgn1, SelectRgn, RsltRgn1;
	Hobject RegionCorner, RegionShortEdge, RegionLongEdge, SelectRgn, RegionTemp1, RegionTemp2, RegionEdge1, RegionEdge2;
	Hobject RsltRgn, RsltRgn1, RsltRgn2, RsltRgn3, RsltTempRgn;
	HTuple /*Row1, Row2, Col1, Col2,*/ Num, Area, Mean;//, , CenterRow, CenterCol, SelRow1, SelRow2, SelCol1, SelCol2;
	HTuple Row11, Col11, Phi, Leng1, Leng2;
	
	/*
	HTuple DiffAng, HomMat2D;
	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);
	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));
	*/


	//모서리, 단변, 장변 검사영역 생성

	gen_empty_obj(&RsltRgn);
	gen_empty_obj(&RsltRgn1);
	gen_empty_obj(&RsltRgn2);
	gen_empty_obj(&RsltRgn3);
	gen_empty_obj(&RsltTempRgn);


	//현재 칩 영역 기준으로 영역 설정
	//smallest_rectangle1(chip_ins_region, &Row1, &Col1, &Row2, &Col2);
	

	//erosion_rectangle1(chip_ins_region, &InsRgn, 2, 2); 
	//erosion_rectangle1(chip_ins_region, &InsRgn, 1, 1); 
	smallest_rectangle2(chip_ins_region, &Row11, &Col11, &Phi, &Leng1, &Leng2);

	if(edgesize < 10)
		edgesize = 10;

	if(edgesize >= 80)
		edgesize = 80;

	////////////////////////////////////////////////////////////////////////////////////////////////
	//코너 검사 시작
	//좌상
	//gen_rectangle1(&RegionTemp1, (Hlong)Row1[0], (Hlong)Col1[0], (Hlong)Row1[0] + edgesize, (Hlong)Col1[0] + edgesize);
	herr = gen_rectangle2(&RegionTemp1, Row11 - Leng2, Col11 - Leng1, Phi, HTuple(edgesize), HTuple(edgesize));

	//좌하
	//gen_rectangle1(&RegionTemp2, (Hlong)Row2[0] - edgesize, (Hlong)Col1[0], (Hlong)Row2[0], (Hlong)Col1[0] + edgesize);
	herr = gen_rectangle2(&RegionTemp2, Row11 + Leng2, Col11 - Leng1, Phi, HTuple(edgesize), HTuple(edgesize));

	union2(RegionTemp1, RegionTemp2, &RegionCorner);

	//우상
	//gen_rectangle1(&RegionTemp1, (Hlong)Row1[0], (Hlong)Col2[0] - edgesize, (Hlong)Row1[0] + edgesize, (Hlong)Col2[0]);
	herr = gen_rectangle2(&RegionTemp1, Row11 - Leng2, Col11 + Leng1, Phi, HTuple(edgesize), HTuple(edgesize));

	union2(RegionCorner, RegionTemp1, &RegionCorner);
	
	//우하
	//gen_rectangle1(&RegionTemp1, (Hlong)Row2[0] - edgesize, (Hlong)Col2[0] - edgesize, (Hlong)Row2[0], (Hlong)Col2[0]);
	herr = gen_rectangle2(&RegionTemp1, Row11 + Leng2, Col11 + Leng1, Phi, HTuple(edgesize), HTuple(edgesize));

	union2(RegionCorner, RegionTemp1, &RegionCorner);

	intersection(chip_ins_region, RegionCorner, &RegionCorner);

	if(win)
	{
		set_color(win, "gold");
		disp_obj(RegionCorner, win);
	}

	reduce_domain(*image, RegionCorner, &ImageReduced);

	threshold(ImageReduced, &Region, HTuple(0),HTuple(thresh));
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn1, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance_x).Concat(tolerance_y), HTuple(400).Concat(400));
	count_obj(RsltRgn1, &Num);
/*
	if(0 < (Hlong)Num[0])
	{
		//threshold 100% 으로 1차 검출한 이후에 threshold 60% 으로 2차 걸러낸다.
		for(int i = 0; i < (Hlong)Num[0]; i++)
		{
			select_obj(RsltRgn1, &SelectRgn, i + 1);

			intensity(SelectRgn, ImageReduced, &Mean, _);

			if((thresh * 0.6) >= (Hlong)Mean[0])
				union2(SelectRgn, RsltTempRgn, &RsltTempRgn);
		}

		connection(RsltTempRgn, &RsltRgn1);
		count_obj(RsltRgn1, &Num);
	}
*/
	//코너 검사 끝
	////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////
	//단변 검사 시작
	//좌
	//gen_rectangle1(&RegionTemp1, (Hlong)Row1[0] + edgesize, (Hlong)Col1[0], (Hlong)Row2[0] - edgesize, (Hlong)Col1[0] + edgesize);
	herr = gen_rectangle2(&RegionTemp1, Row11, Col11 - Leng1, Phi, HTuple(edgesize), HTuple((Hlong)Leng2[0] - edgesize));

	//우
	//gen_rectangle1(&RegionTemp2, (Hlong)Row1[0] + edgesize, (Hlong)Col2[0] - edgesize, (Hlong)Row2[0] - edgesize, (Hlong)Col2[0]);
	herr = gen_rectangle2(&RegionTemp2, Row11, Col11 + Leng1, Phi, HTuple(edgesize), HTuple((Hlong)Leng2[0] - edgesize));
	
	union2(RegionTemp1, RegionTemp2, &RegionShortEdge);

	intersection(chip_ins_region, RegionShortEdge, &RegionShortEdge);

	
	if(win)
	{
		set_color(win, "coral");
		disp_obj(RegionShortEdge, win);
	}


	reduce_domain(*image, RegionShortEdge, &ImageReduced);

	threshold(ImageReduced, &Region, HTuple(0),HTuple(sthresh));
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn2, HTuple("width").Concat("height"), HTuple("and"), HTuple(stolerance_x).Concat(stolerance_y), HTuple(400).Concat(400));
	count_obj(RsltRgn2, &Num);

	//칩 엣지에 닿아있지 않은 경우 제외
	if(0 < (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)
		{
			select_obj(RsltRgn2, &SelectRgn, i + 1);

			//dilation_rectangle1(SelectRgn, &SelectRgn, 3, 3);
			
			//gen_rectangle1(&RegionEdge1, (Hlong)Row1[0], (Hlong)Col1[0], (Hlong)Row2[0], (Hlong)Col1[0]+5);
			//gen_rectangle1(&RegionEdge2, (Hlong)Row1[0], (Hlong)Col2[0]-5, (Hlong)Row2[0], (Hlong)Col2[0]);
			gen_rectangle2(&RegionEdge1, Row11, Col11 - Leng1, Phi, HTuple(5), HTuple((Hlong)Leng2[0] - edgesize));
			gen_rectangle2(&RegionEdge2, Row11, Col11 + Leng1, Phi, HTuple(5), HTuple((Hlong)Leng2[0] - edgesize));

			intersection(RegionEdge1, SelectRgn, &RegionIntersecEdge);
			area_center(RegionIntersecEdge, &Area, _, _);

			if(1 > (Hlong)Area[0])
			{
				intersection(RegionEdge2, SelectRgn, &RegionIntersecEdge);
				area_center(RegionIntersecEdge, &Area, _, _);

				if(1 > (Hlong)Area[0])
				{
					//양쪽 엣지에 닿아 있지 않기 때문에 불량이 아님.
				}
				else
				{
					//dilation_circle(SelectRgn, &SelectRgn, 1.2);

					union2(SelectRgn, RsltTempRgn, &RsltTempRgn);
				}
			}
			else
			{
				//dilation_circle(SelectRgn, &SelectRgn, 1.2);

				union2(SelectRgn, RsltTempRgn, &RsltTempRgn);
			}
		}

		connection(RsltTempRgn, &RsltRgn2);
		count_obj(RsltRgn2, &Num);
	}
	//단변 검사 끝
	////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////
	//장변 검사 시작
	//상
	//gen_rectangle1(&RegionTemp1, (Hlong)Row1[0], (Hlong)Col1[0] + edgesize, (Hlong)Row1[0] + edgesize, (Hlong)Col2[0] - edgesize);
	herr = gen_rectangle2(&RegionTemp1, Row11 - Leng2, Col11, Phi, HTuple((Hlong)Leng1[0] - edgesize), edgesize);
	
	//하
	//gen_rectangle1(&RegionTemp2, (Hlong)Row2[0] - edgesize, (Hlong)Col1[0] + edgesize, (Hlong)Row2[0], (Hlong)Col2[0] - edgesize);
	herr = gen_rectangle2(&RegionTemp2, Row11 + Leng2, Col11, Phi, HTuple((Hlong)Leng1[0] - edgesize), edgesize);

	union2(RegionTemp1, RegionTemp2, &RegionLongEdge);

	intersection(chip_ins_region, RegionLongEdge, &RegionLongEdge);

	
	if(win)
	{
		set_color(win, "pink");
		disp_obj(RegionLongEdge, win);
	}
	

	reduce_domain(*image, RegionLongEdge, &ImageReduced);

	threshold(ImageReduced, &Region, HTuple(0),HTuple(mthresh));
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing,&RsltRgn3, HTuple("width").Concat("height"), HTuple("and"), HTuple(mtolerance_x).Concat(mtolerance_y), HTuple(400).Concat(400));
	count_obj(RsltRgn3, &Num);

	//칩 엣지에 닿아있지 않은 경우 제외
	if(0 < (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)
		{
			select_obj(RsltRgn3, &SelectRgn, i + 1);

			//dilation_rectangle1(SelectRgn, &SelectRgn, 3, 3);
			
			//gen_rectangle1(&RegionEdge1, (Hlong)Row1[0], (Hlong)Col1[0], (Hlong)Row1[0]+15, (Hlong)Col2[0]);
			//gen_rectangle1(&RegionEdge2, (Hlong)Row2[0]-15, (Hlong)Col1[0], (Hlong)Row2[0], (Hlong)Col2[0]);
			gen_rectangle2(&RegionEdge1, Row11 - Leng2, Col11, Phi, HTuple((Hlong)Leng1[0] - edgesize), HTuple(1));	//upper edge
			gen_rectangle2(&RegionEdge2, Row11 + Leng2, Col11, Phi, HTuple((Hlong)Leng1[0] - edgesize), HTuple(1));	//lower edge
			

			intersection(RegionEdge1, SelectRgn, &RegionIntersecEdge);
			area_center(RegionIntersecEdge, &Area, _, _);

			if(1 > (Hlong)Area[0])
			{
				intersection(RegionEdge2, SelectRgn, &RegionIntersecEdge);
				area_center(RegionIntersecEdge, &Area, _, _);

				if(1 > (Hlong)Area[0])
				{
					//양쪽 엣지에 닿아 있지 않기 때문에 불량이 아님.
				}
				else
				{
					move_region(RegionEdge2, &RegionEdge2, 2, 0);
					difference(SelectRgn, RegionEdge2, &SelectRgn);			//칩 하단 엣지에서 검출되는 칩아웃은 엣지부분이 포함되어 과검출이 있기 때문에 엣지영역을 제외시켜준다.

					//최종 사이즈를 한번 더 보고 포함시킨다. 너무 작은 애들이 포함될 수 있음

					//dilation_rectangle1(SelectRgn, &SelectRgn, 1, 2);
					//dilation_circle(SelectRgn, &SelectRgn, 1.2);
					union2(SelectRgn, RsltTempRgn, &RsltTempRgn);
				}
			}
			else
			{
				//move_region(RegionEdge2, &RegionEdge2, -2, 0);
				//difference(SelectRgn, RegionEdge1, &SelectRgn);			//칩 하단 엣지에서 검출되는 칩아웃은 엣지부분이 포함되어 과검출이 있기 때문에 엣지영역을 제외시켜준다.
				//dilation_rectangle1(SelectRgn, &SelectRgn, 1, 1.2);
				union2(SelectRgn, RsltTempRgn, &RsltTempRgn);
			}
		}

		connection(RsltTempRgn, &RsltRgn3);
		count_obj(RsltRgn3, &Num);
	}

	//장변 검사 끝
	////////////////////////////////////////////////////////////////////////////////////////////////

	union2(RsltRgn, RsltRgn1, &RsltRgn);
	union2(RsltRgn, RsltRgn2, &RsltRgn);
	union2(RsltRgn, RsltRgn3, &RsltRgn);
	connection(RsltRgn, &RsltRgn);

	count_obj(RsltRgn, &Num);

	cnt = (Hlong)Num[0];

	if(0 < cnt)
	{
		union2(RsltRgn, *rslt_region, rslt_region);

		*rslt_num = (Hlong)Num[0];

		result_out = FALSE;
	}

	if(win)
	{
		disp_obj(RsltRgn, win);
	}
	
	return result_out;
}

BOOL C_VisionModule::ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, Hobject chip_ins_region_cur,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int high_thresh, int tolerance, int broken_thresh, double broken_sigma,
							  Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;


	if( (3 > broken_sigma) || (20 < broken_sigma) )
		broken_sigma = 3;

	if( (1 > broken_thresh) || (255 < broken_thresh) )
		broken_thresh = 50;

	rslt = _Hal_ChipMold(win, image, broken_image, chip_ins_region, chip_ins_region_cur,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							  high_thresh, tolerance, broken_thresh, broken_sigma,
							  rslt_num, rslt_region, rslt_broken_num, rslt_broken_region,
							  min_area, min_width, min_height);

	return rslt;	



}

BOOL  C_VisionModule::_Hal_ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, Hobject chip_ins_region_cur,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int high_thresh, int tolerance, int broken_thresh, double broken_sigma,
							  Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region,
							  Hlong min_area, Hlong min_width, Hlong min_height)
{
	BOOL rslt = TRUE;
	Hobject ImageReduced, Region, RegionClosing, InsRegion, Line, Image;
	HTuple Num, Area;
	Hlong chip_area, tol_area;
	//HTuple RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond, ColumnEdgeSecond, AmplitudeSecond, MeasureHandle, IntraDistance, InterDistance;
	Hobject RsltRgn, RsltBrokenRgn, ChipRgn1;

	HTuple DiffAng, HomMat2D;
	//double offset_x, offset_y;

	Herror herr;
	CString status;

	gen_empty_obj(&RsltRgn);
	gen_empty_obj(&RsltBrokenRgn);

	
	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);
	
	//offset_x = offset_rx - offset_ox;
	//offset_y = offset_ry - offset_oy;
	
	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region, &ChipRgn1, HomMat2D, HTuple("false"));
	
	
	//Mold칩 이면 결함. //////////////////////////////////////
	erosion_rectangle1(ChipRgn1, &InsRegion, 1, 1);
	
	reduce_domain(*image, InsRegion, &ImageReduced);

	set_check("~give_error");

	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(high_thresh));

	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_32!... %ld, %ld", (Hlong)0, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);

	area_center(InsRegion, &Area, _, _);
	chip_area = (Hlong)Area[0];

	if(0 == tolerance)
		tol_area = min_area;
	else
		tol_area = (Hlong)( ((double)chip_area) * (tolerance * 0.01) );

	select_shape(Region, &RsltRgn, HTuple("area"), HTuple("and"), HTuple(tol_area),  HTuple("max"));
	
	connection(RsltRgn, &RsltRgn);

	count_obj(RsltRgn, &Num);

	if(0 < (Hlong)Num[0])
	{
		union1(RsltRgn, &RsltRgn);
		area_center(RsltRgn, &Area, _, _);

		if(tol_area < (Hlong)Area[0])
		{
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(RsltRgn, win);
			}

			
			*rslt_num = 1;

			rslt = FALSE;
		}
		else
		{
			*rslt_num = 0;
		}
	}

	copy_obj(RsltRgn, rslt_region, 1, -1);
	//이면수지 검사 끝///////////////////////////


	//Broken/////////////////////////////////////
	Hobject broken_lines, RsltRgnBroken;

	Hlong broken_mask = (Hlong)broken_sigma;
	double dBroken_thresh = (double)broken_thresh;
	
	int nChipHeight = 0;
	HTuple Row1, Col1, Phi1, halfWidth, halfHeight;

	erosion_rectangle1(chip_ins_region_cur, &chip_ins_region_cur, 7, 1);
	smallest_rectangle2(chip_ins_region_cur, &Row1, &Col1, &Phi1, &halfWidth, &halfHeight);

	nChipHeight = (Hlong)halfHeight[0] * 2;

	reduce_domain(*broken_image, chip_ins_region_cur, &ImageReduced);

	emphasize(ImageReduced, &ImageReduced, 10, 10, 10);

	lines_gauss(ImageReduced, &broken_lines, broken_sigma, 1, (double)broken_thresh, "dark", "false", "gaussian", "false");	//true / true
	//lines_facet(ImageReduced, &broken_lines, broken_mask, 1.0, dBroken_thresh, "dark");
	

	gen_region_contour_xld (broken_lines, &RsltBrokenRgn, "filled");

	connection(RsltBrokenRgn, &RsltBrokenRgn);
	dilation_circle(RsltBrokenRgn, &RsltBrokenRgn, 5);
	union1(RsltBrokenRgn, &RsltBrokenRgn);
	erosion_circle(RsltBrokenRgn, &RsltBrokenRgn, 5);
	connection(RsltBrokenRgn, &RsltBrokenRgn);

	select_shape(RsltBrokenRgn, &RsltBrokenRgn, HTuple("height"), HTuple("and"), HTuple(nChipHeight * 0.85), HTuple("max"));

	connection(RsltBrokenRgn, &RsltBrokenRgn);

	count_obj(RsltBrokenRgn, &Num);

	if( (0 < (Hlong)Num[0]) && (3 >= (Hlong)Num[0]) ) //너무 많으면 broken이 아니라 chip grind가 잘못된것->양품처리 
	{
		union2(RsltBrokenRgn, RsltRgnBroken, &RsltRgnBroken);

		*rslt_broken_num = (Hlong)Num[0];

		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRgnBroken, win);
		}
		rslt = FALSE;
	}

	copy_obj(RsltRgnBroken, rslt_broken_region, 1, -1);


	return rslt;
}

BOOL C_VisionModule::PotPart(Hlong win, Hobject *image, Hobject *image2, Hobject *crack_image, Hobject chip_ins_region, Hobject pot_ins_region, Hobject sr_region,
							 double offset_ox, double offset_oy, 
							 double offset_ox2, double offset_oy2, 
							 double offset_rx, double offset_ry,
							 double offset_rx2, double offset_ry2,
							 int part_thresh, int part2_thresh, int high_thresh, int tolerance, int range_tolerance, int black_pot_tol, int black_thresh,
							 Hlong *rslt_num, Hobject *rslt_region, Hlong *range_rslt_num, Hobject *range_rslt_region, Hobject *recog_ins_rslt_region)
{
	//BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;

	BOOL rslt = TRUE;
	Hobject ImageReduced, Region, RegionClosing, DilationRegion, ImageReduced2, Region2;
	HTuple Num;
	Hobject FillRegion, RangeRegion, ConvRegion;
	HTuple Row0, Col0, Row1, Col1;
	Hobject RecogRgn;
	Hobject PotBlackPart, RlstPotBlackPart;

	Hobject OutRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean;
	int diff_var;
	Hobject ComplRgn;
	Hobject PosChkRgnTop, PosChkRgnRight, PosChkRgnBottom, PosChkRgnLeft;
	HTuple AreaTop, AreaBottom, AreaLeft, AreaRight;

	Hobject SRRegion, RecogRegion, RsltRegion, RangeRsltRegion, RecogRsltRegion;

	Hobject Pot2InsRgn, Pot2RsltRgn, Pot2ImageReduced, Pot2FillRegion, Pot2DilationRegion;

	Hlong num, i;

	HTuple HomMat2D, DiffAng, Area;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(sr_region, &SRRegion, HomMat2D, HTuple("false"));

	//Particle///////////////////////////////////
	fill_up(pot_ins_region, &FillRegion);

	//튀어나온 부분을 없에기
	closing_circle(FillRegion, &FillRegion, 3.5);
	erosion_circle(FillRegion, &FillRegion, 6.5);
	//작게 분리된 영역 없에기
	connection(FillRegion, &FillRegion);
	select_shape(FillRegion, &FillRegion, HTuple("area"), HTuple("and"), HTuple(100), HTuple("max"));
	union1(FillRegion, &FillRegion);
	fill_up(FillRegion, &FillRegion);

	dilation_circle(chip_ins_region, &DilationRegion, 8.5);
	
	difference(FillRegion, DilationRegion, &Region);//Region = Potting 영역 (섬세한 부분은 제거) 

	//Pot2검사용
	dilation_rectangle1(chip_ins_region, &Pot2DilationRegion, 20.5, 20.5);
	fill_up(pot_ins_region, &Pot2FillRegion);
	closing_circle(Pot2FillRegion, &Pot2FillRegion, 3.5);
	difference(Pot2FillRegion, Pot2DilationRegion, &Pot2InsRgn); //Pot2InsRgn = Potting 영역
	

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&Pot2RsltRgn);
	gen_empty_obj(&RangeRsltRegion);
	gen_empty_obj(&RecogRsltRegion);

	//Don't care region 좌상, 좌하, 우상, 우하(인식기 부분)
	gen_empty_obj(&RecogRgn);
	smallest_rectangle1(chip_ins_region,&Row0,&Col0,&Row1,&Col1);

	gen_rectangle1(&RecogRgn,Row0 - recog_dist, Col0 - recog_dist, Row0, Col0);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row1, Col0 - recog_dist, Row1 + recog_dist, Col0);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row0 - recog_dist, Col1, Row0, Col1 + recog_dist);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row1, Col1, Row1 + recog_dist, Col1 + recog_dist);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);
	difference(Region, RecogRsltRegion, &Region);

	difference(Pot2InsRgn, RecogRsltRegion, &Pot2InsRgn);


	copy_obj(RecogRsltRegion, recog_ins_rslt_region, 1, -1);

	//Check position region
	//Top
	gen_rectangle1(&PosChkRgnTop, Row0 - 150, Col0, Row0 - 5, Col1);
	//Bottom
	gen_rectangle1(&PosChkRgnBottom, Row1 + 5, Col0, Row1 + 150, Col1);
	//Left
	gen_rectangle1(&PosChkRgnLeft, Row0, Col0 - 150, Row1, Col0 - 5);
	//Right
	gen_rectangle1(&PosChkRgnRight, Row0, Col1 + 5, Row1, Col1 + 150);

	select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(100), HTuple("max"));
	count_obj(Region, &Num);
	Hlong test;
	test = ((Hlong)Num[0]);
	if(1 <= ((Hlong)Num[0]) )
	{
		union1(Region, &Region);
	}
	else
	{
		return TRUE;
	}

	reduce_domain(*image, Region, &ImageReduced);			//Potting 영역 #1
	reduce_domain(*image2, Pot2InsRgn, &Pot2ImageReduced); // Pottin 영역 #2

	dilation_circle(chip_ins_region, &Region2, 13);
	difference(Region, Region2, &Region2);//원래 첫번째 검사영역에서...
	erosion_circle(SRRegion, &SRRegion, 3.5);
	intersection(Region2, SRRegion, &Region2);//SR region영역에서 찾는다.(두번째 이미지에서는->pot부분에 흰색 띠 형성때문(조명반사))

	if(win)
	{
		set_color(win, HTuple("cyan"));
		disp_obj(SRRegion, win);
	}	
		
	reduce_domain(*crack_image, Region2, &ImageReduced2); // Potting 영역 #3
	
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}	
		
	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(Region2, win);
	}

	if( (part_thresh > 255) || (high_thresh > 255) )
	{
		part_thresh = 230;
		high_thresh = 255;
	}
	if( (part_thresh < 0) || (high_thresh < 0) )
	{
		part_thresh = 0;
		high_thresh = 20;
	}
	high_thresh = part_thresh + 20;
	if(high_thresh > 255)
	{
		part_thresh = 235;
		high_thresh = 255;
	}
	hysteresis_threshold(ImageReduced,&Region,HTuple(part_thresh),HTuple(high_thresh),HTuple(10));
	hysteresis_threshold(ImageReduced2,&Region2,HTuple(part_thresh),HTuple(high_thresh),HTuple(10));
	union2(Region, Region2, &Region);
	

	//Pot중에서 작은것은 dark 수지묻음으로 인정//
	connection(pot_ins_region, &PotBlackPart);
	select_shape(PotBlackPart, &PotBlackPart, HTuple("area"), HTuple("and"), HTuple(0), HTuple(5000));
	//Port particle에서 제외하고 그것들을 경계값 비교한다. 즉 pot영역외 작은것은 경계값 비교하여 불량은 선별
	difference(Region, PotBlackPart, &RsltRegion);
	select_shape(PotBlackPart, &PotBlackPart, HTuple("width").Concat("height"), HTuple("and"), HTuple(black_pot_tol).Concat(black_pot_tol), HTuple("max").Concat("max"));
	//경계값과 비교
	count_obj(PotBlackPart, &Num);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(PotBlackPart, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 2);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *image, &Mean, _);
		intensity(OutRgn, *image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 < diff_var)
		{
			if((black_thresh + 10) < diff_var)
			{
				concat_obj(RlstPotBlackPart, SelectRgn, &RlstPotBlackPart);
			}
		}
	}
	union2(RsltRegion, RlstPotBlackPart, &RsltRegion);
	/////////////////////////////////////////////

	
	connection(RsltRegion, &RsltRegion);
	closing_circle(RsltRegion,&RsltRegion,HTuple(3.5));
	
	select_shape(RsltRegion, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(RsltRegion, &RsltRegion, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(RsltRegion, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	//영상이 약간 밀려 오버킬 발생 됨->폭이 칩정도 되면 양품처리(300 : 일반적인 칩에 폭에 2/3수준)
	select_shape(RsltRegion, &RsltRegion, HTuple("width"), HTuple("and"), HTuple("min"), HTuple(80));
	//길쭉한것은 무시(장축단축비)
	select_shape(RsltRegion, &RsltRegion, HTuple("anisometry"), HTuple("and"), HTuple(0), HTuple(10));


	//LSS 201040406
	//Pot#2//////////////////////////////////////
	threshold(Pot2ImageReduced,&Pot2RsltRgn,HTuple(0),HTuple(part2_thresh));
	
	connection(Pot2RsltRgn, &Pot2RsltRgn);
	//closing_circle(Pot2RsltRgn,&Pot2RsltRgn,HTuple(3.5));
	
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	union2(RsltRegion, Pot2RsltRgn, &RsltRegion);
	connection(RsltRegion, &RsltRegion);

	Hobject select_object;
	HTuple Row2, Column2;
	
	count_obj(RsltRegion, &Num);
	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
		}
		
		rslt = FALSE;
	}

	copy_obj(RsltRegion, rslt_region, 1, -1);
	

	//도포불량///////////////////////////////////
	connection(pot_ins_region, &RangeRegion);
	select_shape(RangeRegion, &RangeRegion, HTuple("area"), HTuple("and"), HTuple(100),  HTuple("max"));
	select_shape(RangeRegion, &RangeRegion, HTuple("width"), HTuple("and"), HTuple(100),  HTuple("max"));
	select_shape(RangeRegion, &RangeRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(range_tolerance).Concat(range_tolerance), HTuple("max").Concat("max"));

	union1(RangeRegion, &RangeRegion);

	//20100310 lss
	opening_circle(RangeRegion, &RangeRegion, 3.5);

	shape_trans(RangeRegion, &ConvRegion, "convex");
	fill_up(pot_ins_region, &FillRegion);
	difference(ConvRegion, FillRegion, &Region);
	//Don't care region 좌상, 좌하, 우상, 우하(인식기 부분)
	dilation_rectangle1(*recog_ins_rslt_region, &RecogRegion, 20, 20);
	difference(Region, RecogRegion, &Region);



	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100315 lss
	select_shape(RegionClosing, &RegionClosing, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple(20000));
	select_shape(RegionClosing, &RegionClosing, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(RegionClosing, &RangeRsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(range_tolerance).Concat(range_tolerance), HTuple("max").Concat("max"));

	count_obj(RangeRsltRegion, &Num);

	Hlong top, bot, left, right;

	if(0 < (Hlong)Num[0])
	{
		//불량발생 위치별 조건
		//Top
		intersection(PosChkRgnTop, RangeRsltRegion, &PosChkRgnTop);
		area_center(PosChkRgnTop, &AreaTop, _, _);

		//Bottom
		intersection(PosChkRgnBottom, RangeRsltRegion, &PosChkRgnBottom);
		area_center(PosChkRgnBottom, &AreaBottom, _, _);

		//Left
		intersection(PosChkRgnLeft, RangeRsltRegion, &PosChkRgnLeft);
		area_center(PosChkRgnLeft, &AreaLeft, _, _);

		//Right
		intersection(PosChkRgnRight, RangeRsltRegion, &PosChkRgnRight);
		area_center(PosChkRgnRight, &AreaRight, _, _);

		top = 0;
		bot = 0;
		left = 0;
		right = 0;
		top = (Hlong)AreaTop[0];
		bot = (Hlong)AreaBottom[0];
		left = (Hlong)AreaLeft[0];
		right = (Hlong)AreaRight[0];

		if(0 < (Hlong)AreaTop[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaBottom[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaLeft[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaRight[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));

		}

		count_obj(RangeRsltRegion, &Num);
		*range_rslt_num = (Hlong)Num[0];
		if(0 < *range_rslt_num)
		{

			if(win)
			{
				set_color(win, HTuple("orange"));
				disp_obj(RangeRsltRegion, win);
			}
			
			rslt = FALSE;
		}
	}

	copy_obj(RangeRsltRegion, range_rslt_region, 1, -1);

	return rslt;

}

BOOL C_VisionModule::ChipPot(Hlong win, Hobject *image, RECT ins_region, Hobject chip_ins_region1,
											  double offset_ox, double offset_oy, 
											  double offset_ox2, double offset_oy2, 
											  double offset_rx, double offset_ry,
											  double offset_rx2, double offset_ry2,Hlong pot_area, int thresh, int under_tol, int over_tol, int line_percent, 
											   int pot_width_1, int pot_width_2, int pot_width_3, int pot_width_4, int pot_width_5, int pot_width_6, int average_x, int average_y, Hobject *rslt_pot_region,
											   Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region, int *rslt_average_leng, int *rslt_center_leng)
{
	BOOL rslt = TRUE;
	
	rslt = _Hal_ChipPot(win, image, ins_region,  chip_ins_region1,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,pot_area, thresh, under_tol, over_tol, line_percent, 
							   pot_width_1, pot_width_2, pot_width_3, pot_width_4, pot_width_5, pot_width_6, average_x, average_y, rslt_pot_region, rslt_num, rslt_area, rslt_region, rslt_average_leng, rslt_center_leng);  


	return rslt;

}

//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
BOOL C_VisionModule::FilmMold(Hlong win, 
							  Hobject *mold_image, //map 1
							  Hobject *ink_image, //map 3
							  Hobject *user_image, //map 4
							  Hobject *scratch_image,//map 1
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
							  Hlong var_id, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int dark_thresh, int dark_bright_tol, int white_thresh, int white_bright_tol, int mold_fact, int tolerance, int tolerance_light,
							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;

	rslt = _Hal_FilmMold(win, mold_image, 
							  ink_image, user_image, scratch_image, 
							  ins_region, chip_region, ocv_region, 
							  ocv_chars_region, sr_region, user_region, sr_out_region,
							  var_id, 
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							  dark_thresh, dark_bright_tol, white_thresh, white_bright_tol, mold_fact, tolerance, tolerance_light,
							  rslt_num, rslt_region, rslt_sr_region, rslt_notsr_region, rslt_user_region, rslt_sr_out_region,
							  min_area, min_width, min_height,
							  min_area_sr, min_width_sr, min_height_sr);
	return rslt;
}

BOOL C_VisionModule::FilmPF(Hlong win, Hobject *image, Hobject *image2, 
							RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int area, int black_thresh, int black_tolarance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region) 
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;
	switch(GetVisIndex())
	{
	case VISION_CAM_TOP1:			
		rslt = _Hal_FilmPF(win, image, image2,
								ins_region, chip_region, pf_region, vacc_region,
							   offset_ox, offset_oy, 
							   offset_ox2, offset_oy2, 
							   offset_rx, offset_ry,
							   offset_rx2, offset_ry2,
							   thresh, tolerance, black_thresh, black_tolarance,
							   rslt_num, rslt_region, rslt_pf_region,
							   min_area, min_width, min_height,
							   min_area_sr, min_width_sr, min_height_sr); 
		break;
	case VISION_CAM_TOP2:
		rslt = _Hal_FilmPF2(win, image, image2,
								ins_region, chip_region, pf_region, vacc_region,
							   offset_ox, offset_oy, 
							   offset_ox2, offset_oy2, 
							   offset_rx, offset_ry,
							   offset_rx2, offset_ry2,
							   thresh, tolerance, area, black_thresh, black_tolarance,
							   rslt_num, rslt_region, rslt_pf_region,
							   min_area, min_width, min_height,
							   min_area_sr, min_width_sr, min_height_sr); 
		break;
	case VISION_CAM_BOTTOM:

		break;
	default:
		AfxMessageBox("Error : invalid index(C_VisionModule::Inspection)!)");
		break;
	}

	

	return rslt;
}

//하부결함(out lead)
BOOL C_VisionModule::InLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   double* line_width_avg, 
						   Hlong *rslt_num, Hobject *rslt_region,  
						   Hlong *rslt_num2, Hobject *rslt_region2,
						   Hobject *rslt_dn_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	rslt = _Hal_InLead(win, image, image2, ins_region, not_sr_region,	
							dn_region,
							offset_ox, offset_oy, 
							offset_ox2, offset_oy2, 
							offset_rx, offset_ry,
							offset_rx2, offset_ry2,
						   threshold, factor, tolerance, 
						   line_width_avg, 
						   rslt_num, rslt_region,  
						   rslt_num2, rslt_region2,
						   rslt_dn_region,
						   min_area, min_width, min_height);

	return rslt;


}

// Return value :
//	1 : GOOD	-1 : AMSC -2 : AMDI
//A/M scratch
int C_VisionModule::AMLead(Hlong win, Hobject *image,
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
						   int threshold_1, int tolerance_1,						//TOP
						   int teached_average_light_1, int insp_average_light_1, //TOP
						   int threshold_2, int tolerance_2,					//BOTTOM
						   int teached_average_light_2, int insp_average_light_2, //BOTTOM
						   int threshold_3, int tolerance_3,
						   int teached_average_light_3, int insp_average_light_3,
						   int threshold_4, int tolerance_4,
						   int teached_average_light_4, int insp_average_light_4, 
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hobject *rslt_ins_region_0_0, Hobject *rslt_ins_region_0_1, Hobject *rslt_ins_region_1_0, Hobject *rslt_ins_region_1_1)
{
	int rslt = 1;

	HTuple HomMat2D_Up, HomMat2D_Dn, HomMat2D_Shift;
	HTuple DiffAng;

	HTuple   Num, Mean;

	Hlong max, am_area, tol_area;
	HTuple Min, Max, Area;

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;


	//int nRslt;
	//int nMeanAverage = 0, MeanCnt = 0;

	Hobject ImageReduced, ResultRegion, TotRegion, TotRegion0_0, TotRegion0_1, TotRegion1_0, TotRegion1_1;

	gen_empty_obj(&ResultRegion);
	gen_empty_obj(&TotRegion);
	gen_empty_obj(&TotRegion0_0);
	gen_empty_obj(&TotRegion0_1);
	gen_empty_obj(&TotRegion1_0);
	gen_empty_obj(&TotRegion1_1);
	gen_empty_obj(rslt_ins_region_0_0);
	gen_empty_obj(rslt_ins_region_0_1);
	gen_empty_obj(rslt_ins_region_1_0);
	gen_empty_obj(rslt_ins_region_1_1);

	*rslt_num = 0;

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	//20150609 by 아래쪽 A/M 은 아래쪽 Align 따라가게끔 변경.
	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D_Up);
	vector_angle_to_rigid(offset_oy2, offset_ox2, HTuple(0), offset_ry2, offset_rx2, DiffAng, &HomMat2D_Dn);

	affine_trans_region(region_0_0, rslt_ins_region_0_0, HomMat2D_Up, HTuple("false"));
	affine_trans_region(region_1_0, rslt_ins_region_1_0, HomMat2D_Up, HTuple("false"));
	
	affine_trans_region(region_0_1, rslt_ins_region_0_1, HomMat2D_Dn, HTuple("false"));
	affine_trans_region(region_1_1, rslt_ins_region_1_1, HomMat2D_Dn, HTuple("false"));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//전체 변색 검사 시작
	//
	//※ 티칭 시 영역 내 평균 밝기와 검사 시 영역 내 평균 밝기의 차를 구하여 검사
	//
	if(use_0)
	{
		//if( (0 < insp_average_light_1) && (0 < teached_average_light_1) )		// 검사 파라미터가 0 이 아닐 경우만 검사 수행
		//{
			//TOP#1
			intensity(*rslt_ins_region_0_0, *image, &Mean, _);	// 영역 내 평균 밝기 구하기

			//if( (150 < (int)Mean[0]) && (240 > (int)Mean[0]) )	//20160303 by 조건식 추가.
			//{
				if( insp_average_light_1 <= abs((int)Mean[0] - teached_average_light_1) )
				{
					// 전체 변색 불량
					concat_obj(*rslt_region, *rslt_ins_region_0_0, rslt_region);
					rslt = -2;
				}
			//}
			//else
			//{
			//	concat_obj(*rslt_region, *rslt_ins_region_0_0, rslt_region);
			//	rslt = FALSE;
			//}

			//BOTTOM#1
			intensity(*rslt_ins_region_0_1, *image, &Mean, _);

			//if( (150 < (int)Mean[0]) && (240 > (int)Mean[0]) )	//20160303 by 조건식 추가.
			//{
				if( insp_average_light_2 <= abs((int)Mean[0] - teached_average_light_2) )
				{
					concat_obj(*rslt_region, *rslt_ins_region_0_1, rslt_region);
					rslt = -2;
				}
			//}
			//else
			//{
			//	concat_obj(*rslt_region, *rslt_ins_region_0_1, rslt_region);
			//	rslt = FALSE;
			//}
		//}

	}
	
	if(use_1)
	{
		//if( (0 < insp_average_light) && (0 < teached_average_light) )
		//{
			//TOP#2
			intensity(*rslt_ins_region_1_0, *image, &Mean, _);

			//if( (150 < (int)Mean[0]) && (240 > (int)Mean[0]) )	//20160303 by 조건식 추가.
			//{
				if( insp_average_light_3 <= abs((int)Mean[0] - teached_average_light_3) )
				{
					concat_obj(*rslt_region, *rslt_ins_region_1_0, rslt_region);
					rslt = -2;
				}
			//}
			//else
			//{
			//	concat_obj(*rslt_region, *rslt_ins_region_1_0, rslt_region);
			//	rslt = FALSE;
			//}

			//BOTTOM#2

			intensity(*rslt_ins_region_1_1, *image, &Mean, _);

			//if( (150 < (int)Mean[0]) && (240 > (int)Mean[0]) )	//20160303 by 조건식 추가.
			//{
				if( insp_average_light_4 <= abs((int)Mean[0] - teached_average_light_4) )
				{
					concat_obj(*rslt_region, *rslt_ins_region_1_1, rslt_region);

					rslt = -2;
				}
			//}
			//else
			//{
			//	concat_obj(*rslt_region, *rslt_ins_region_1_1, rslt_region);
			//	rslt = FALSE;
			//}

		//}
	}
	//
	//전체 변색 검사 종료
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//스크래치, 부분 변색 검사 시작
	//
	if(use_0)
	{
		if(0 >= tolerance_1)
			tolerance_1 = 1;

		//20151218 by 면적 % 검사로 변경.
		area_center(*rslt_ins_region_0_0, &Area, _, _);
		am_area = (Hlong)Area[0];

		tol_area = (Hlong)( ((double)am_area) * (tolerance_1 * 0.01) );	// 티칭 된 전체 영역에서 검사 기준 영역을 구함

		//0_0
		reduce_domain(*image, *rslt_ins_region_0_0, &ImageReduced);

		min_max_gray(*rslt_ins_region_0_0, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - threshold_1;

		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		_Hal_RegionShift(*rslt_ins_region_0_0, ResultRegion, &TotRegion0_0, 10);

		closing_circle(TotRegion0_0, &TotRegion0_0, 2.5);
		connection(TotRegion0_0, &TotRegion0_0);
		//select_shape(TotRegion0_0, &TotRegion0_0, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));
		
		select_shape(TotRegion0_0, &TotRegion0_0, HTuple("area"), HTuple("and"), HTuple(1),  HTuple("max"));
		select_shape(TotRegion0_0, &TotRegion0_0, HTuple("compactness"), HTuple("and"), HTuple(1),  HTuple(20));

		connection(TotRegion0_0, &TotRegion0_0);
		
		count_obj(TotRegion0_0, &Num);

		if(0 < (Hlong)Num[0])
		{
			union1(TotRegion0_0, &TotRegion0_0);
			area_center(TotRegion0_0, &Area, _, _);
			if(tol_area < (Hlong)Area[0])
			{
				concat_obj(*rslt_region, TotRegion0_0, rslt_region);	

				rslt = -1;
			}
		}

		//0_1
		if(0 >= tolerance_2)
			tolerance_2 = 1;

		area_center(*rslt_ins_region_0_1, &Area, _, _);
		am_area = (Hlong)Area[0];

		tol_area = (Hlong)( ((double)am_area) * (tolerance_2 * 0.01) );	// 티칭 된 전체 영역에서 검사 기준 영역을 구함

		reduce_domain(*image, *rslt_ins_region_0_1, &ImageReduced);

		min_max_gray(*rslt_ins_region_0_1, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - threshold_2;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		_Hal_RegionShift(*rslt_ins_region_0_1, ResultRegion, &TotRegion0_1, 10);

		closing_circle(TotRegion0_1, &TotRegion0_1, 2.5);
		connection(TotRegion0_1, &TotRegion0_1);
		//select_shape(TotRegion0_1, &TotRegion0_1, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

		select_shape(TotRegion0_1, &TotRegion0_1, HTuple("area"), HTuple("and"), HTuple(1),  HTuple("max"));
		select_shape(TotRegion0_1, &TotRegion0_1, HTuple("compactness"), HTuple("and"), HTuple(1),  HTuple(20));

		connection(TotRegion0_1, &TotRegion0_1);

		count_obj(TotRegion0_1, &Num);

		if(0 < (Hlong)Num[0])
		{
			union1(TotRegion0_1, &TotRegion0_1);
			area_center(TotRegion0_1, &Area, _, _);
			if(tol_area < (Hlong)Area[0])
			{
				concat_obj(*rslt_region, TotRegion0_1, rslt_region);
				rslt = -1;
			}
		}

		//concat_obj(*rslt_region, TotRegion0_1, rslt_region);
	}

	if(FALSE)	//20160528 LEAD용 영역은 스크래치 검사를 하지 않는다.
	{
		if(use_1)
		{
			//1_0
			if(0 >= tolerance_3)
				tolerance_3 = 1;

			//20151218 by 면적 % 검사로 변경.
			area_center(*rslt_ins_region_1_0, &Area, _, _);
			am_area = (Hlong)Area[0];

			tol_area = (Hlong)( ((double)am_area) * (tolerance_3 * 0.01) );	// 티칭 된 전체 영역에서 검사 기준 영역을 구함

			reduce_domain(*image, *rslt_ins_region_1_0, &ImageReduced);

			min_max_gray(*rslt_ins_region_1_0, *image, HTuple(20), &Min, &Max, _);

			max = (Hlong)Max[0];
			max = max - threshold_3;
			if( (0 > max) || (255 < max) )
			{
				max = 0;
			}
		
			threshold(ImageReduced, &ResultRegion, 0, max);

			_Hal_RegionShift(*rslt_ins_region_1_0, ResultRegion, &TotRegion1_0, 10);

			closing_circle(TotRegion1_0, &TotRegion1_0, 2.5);
			connection(TotRegion1_0, &TotRegion1_0);
			//select_shape(TotRegion1_0, &TotRegion1_0, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

			select_shape(TotRegion1_0, &TotRegion1_0, HTuple("area"), HTuple("and"), HTuple(1),  HTuple("max"));
			select_shape(TotRegion1_0, &TotRegion1_0, HTuple("compactness"), HTuple("and"), HTuple(1),  HTuple(20));

			connection(TotRegion1_0, &TotRegion1_0);

			count_obj(TotRegion1_0, &Num);

			if(0 < (Hlong)Num[0])
			{
				union1(TotRegion1_0, &TotRegion1_0);
				area_center(TotRegion1_0, &Area, _, _);
				if(tol_area < (Hlong)Area[0])
				{
					concat_obj(*rslt_region, TotRegion1_0, rslt_region);
					rslt = -1;
				}
			}

			//concat_obj(*rslt_region, TotRegion1_0, rslt_region);
		
			//1_1

			//20151218 by 면적 % 검사로 변경.
			if(0 >= tolerance_4)
				tolerance_4 = 1;

			area_center(*rslt_ins_region_1_1, &Area, _, _);
			am_area = (Hlong)Area[0];

			tol_area = (Hlong)( ((double)am_area) * (tolerance_4 * 0.01) );	// 티칭 된 전체 영역에서 검사 기준 영역을 구함

			reduce_domain(*image, *rslt_ins_region_1_1, &ImageReduced);

			min_max_gray(*rslt_ins_region_1_1, *image, HTuple(20), &Min, &Max, _);

			max = (Hlong)Max[0];
			max = max - threshold_4;
			if( (0 > max) || (255 < max) )
			{
				max = 0;
			}
		
			threshold(ImageReduced, &ResultRegion, 0, max);

			_Hal_RegionShift(*rslt_ins_region_1_1, ResultRegion, &TotRegion1_1, 10);

			closing_circle(TotRegion1_1, &TotRegion1_1, 2.5);
			connection(TotRegion1_1, &TotRegion1_1);
			//select_shape(TotRegion1_1, &TotRegion1_1, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

			select_shape(TotRegion1_1, &TotRegion1_1, HTuple("area"), HTuple("and"), HTuple(1),  HTuple("max"));
			select_shape(TotRegion1_1, &TotRegion1_1, HTuple("compactness"), HTuple("and"), HTuple(1),  HTuple(20));

			connection(TotRegion1_1, &TotRegion1_1);

			count_obj(TotRegion1_1, &Num);

			if(0 < (Hlong)Num[0])
			{
				union1(TotRegion1_1, &TotRegion1_1);
				area_center(TotRegion1_1, &Area, _, _);
				if(tol_area < (Hlong)Area[0])
				{
					concat_obj(*rslt_region, TotRegion1_1, rslt_region);
					rslt = -1;
				}
			}


			//concat_obj(*rslt_region, TotRegion1_1, rslt_region);
		}
	}
	//
	//스크래치, 부분 변색 검사 종료
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Image1
	count_obj(*rslt_region, &Num);

	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}
		//return rslt;
	}

	return rslt;
}

//20150511 by
//영역을 step 수 만캄 사방으로 이동 하여 중복되는 이물만 검출
BOOL C_VisionModule::_Hal_RegionShift(Hobject region_1, Hobject region_2, Hobject *result_region, int steps)
{
	BOOL rslt = TRUE;

	HTuple HomMat2D_Shift;
	HTuple DiffAng, Area;

	HTuple   Num;

	Hobject SelectRgn, InterRgn;
	Hlong num;

	Hobject Left_Shift_Region, Right_Shift_Region, Up_Shift_Region, Down_Shift_Region, ResultShiftRegion;
	gen_empty_obj(&Left_Shift_Region);
	gen_empty_obj(&Right_Shift_Region);
	gen_empty_obj(&Up_Shift_Region);
	gen_empty_obj(&Down_Shift_Region);
	gen_empty_obj(&ResultShiftRegion);

	//left
	vector_angle_to_rigid(HTuple(0), HTuple(0), HTuple(0), HTuple(0), HTuple(steps * -1), HTuple(0), &HomMat2D_Shift);
	affine_trans_region(region_1, &Left_Shift_Region, HomMat2D_Shift, HTuple("false"));

	//right
	vector_angle_to_rigid(HTuple(0), HTuple(0), HTuple(0), HTuple(0), HTuple(steps), HTuple(0), &HomMat2D_Shift);
	affine_trans_region(region_1, &Right_Shift_Region, HomMat2D_Shift, HTuple("false"));

	//up
	vector_angle_to_rigid(HTuple(0), HTuple(0), HTuple(0), HTuple(steps * -1), HTuple(0), HTuple(0), &HomMat2D_Shift);
	affine_trans_region(region_1, &Up_Shift_Region, HomMat2D_Shift, HTuple("false"));

	//down
	vector_angle_to_rigid(HTuple(0), HTuple(0), HTuple(0), HTuple(steps), HTuple(0), HTuple(0), &HomMat2D_Shift);
	affine_trans_region(region_1, &Down_Shift_Region, HomMat2D_Shift, HTuple("false"));

	intersection(Left_Shift_Region, region_2, &ResultShiftRegion);
	intersection(Right_Shift_Region, ResultShiftRegion, &ResultShiftRegion);
	intersection(Up_Shift_Region, ResultShiftRegion, &ResultShiftRegion);
	intersection(Down_Shift_Region, ResultShiftRegion, &ResultShiftRegion);

	connection(region_2, &region_2);
	count_obj(region_2, &Num);
	gen_empty_obj(&InterRgn);
	num = (Hlong)Num[0];
	for(int i = 0; i < num; i++)
	{
		select_obj(region_2, &SelectRgn, i + 1);
		intersection(ResultShiftRegion, SelectRgn, &InterRgn);
		area_center(InterRgn, &Area, _, _);
		if(1 < (Hlong)Area[0])
		{
			union2(*result_region, SelectRgn, result_region);
		}
	}

	/*
		//20150509 by AM shift inspection
		//left
		vector_angle_to_rigid(0, 0, HTuple(0), 0, -3, 0, &HomMat2D_Shift);
		affine_trans_region(*rslt_ins_region_0_1, &Left_Shift_Region, HomMat2D_Shift, HTuple("false"));

		//right
		vector_angle_to_rigid(0, 0, HTuple(0), 0, 3, 0, &HomMat2D_Shift);
		affine_trans_region(*rslt_ins_region_0_1, &Right_Shift_Region, HomMat2D_Shift, HTuple("false"));

		//up
		vector_angle_to_rigid(0, 0, HTuple(0), -3, 0, 0, &HomMat2D_Shift);
		affine_trans_region(*rslt_ins_region_0_1, &Up_Shift_Region, HomMat2D_Shift, HTuple("false"));

		//down
		vector_angle_to_rigid(0, 0, HTuple(0), 3, 0, 0, &HomMat2D_Shift);
		affine_trans_region(*rslt_ins_region_0_1, &Down_Shift_Region, HomMat2D_Shift, HTuple("false"));

		intersection(Left_Shift_Region, ResultRegion, &ResultShiftRegion);
		intersection(Right_Shift_Region, ResultShiftRegion, &ResultShiftRegion);
		intersection(Up_Shift_Region, ResultShiftRegion, &ResultShiftRegion);
		intersection(Down_Shift_Region, ResultShiftRegion, &ResultShiftRegion);

		Hobject SelectRgn, InterRgn;
		Hlong num;

		connection(ResultRegion, &ResultRegion);
		count_obj(ResultRegion, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(int i = 0; i < num; i++)
		{
			select_obj(ResultRegion, &SelectRgn, i + 1);
			intersection(ResultShiftRegion, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotRegion0_1, SelectRgn, &TotRegion0_1);
			}
		}
		*/

	return rslt;
}

//상부결함(in lead)
BOOL C_VisionModule::OutLead(Hlong win, Hobject *image, Hobject *image2,Hobject *image3,
							RECT ins_region, RECT ins_region2, RECT ins_region3,
							Hobject not_sr_region, 
							Hobject up_region,
							Hobject dn_region,
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
						   Hobject *rslt_up_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	//unsigned int CaseInOutLeadPosition=0;
	HTuple UpLeadRow, UpLeadCaseCol, DnLeadRow, DnLeadCaseCol;
	area_center(up_region, _, &UpLeadRow, &UpLeadCaseCol);
	area_center(dn_region, _, &DnLeadRow, &DnLeadCaseCol);

	/*
	if(UpLeadRow < DnLeadRow)//Out 리드가 위쪽에 위치
	{
		CaseInOutLeadPosition = 2;
		m_VDat[0]->m_InsRslt.m_iResultInOutPutDir[0] = 2;
	}
	else if(UpLeadRow > DnLeadRow)//In 리드가 위쪽에 위치
	{
		CaseInOutLeadPosition = 1;
		m_VDat[0]->m_InsRslt.m_iResultInOutPutDir[0] = 1;
	}
	*/

	rslt = _Hal_OutLead(win, image, image2,image3,
							ins_region,	ins_region2, ins_region3, 
							not_sr_region, 
							up_region,
							up_region2,
							up_region3,
							offset_ox, offset_oy, 
							offset_ox2, offset_oy2, 
							offset_rx, offset_ry,
							offset_rx2, offset_ry2,
						   thresh, factor, tolerance, 
						   thresh_dirt, tolerance_dirt,
						   out_lead_use, out_lead_dirt_use,use_out_hori,
						   rslt_num, rslt_region, 
						   rslt_num2, rslt_region2, 						   
						   rslt_up_region,
						   min_area, min_width, min_height);

	return rslt;
}

//20100618 lss
//image 검정, image 흰색
BOOL  C_VisionModule::_Hal_OutLead(Hlong win, Hobject *image, Hobject *image2, Hobject *image3,
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
						   Hlong min_area, Hlong min_width, Hlong min_height)
{
	BOOL rslt = TRUE;
	HTuple   Pointer, Type, Width, Height, WindowHandle, Row1;
	HTuple   Column1, Row2, Column2, Row, Sum, Column, Diff;
	HTuple   Length1, UntilHeight, Start, MeasureHandle0, MeasureHandle1;
	HTuple   RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond;
	HTuple   ColumnEdgeSecond, AmplitudeSecond, IntraDistance;
	HTuple   InterDistance, End, Time;
	Hobject  I, InsRegion, DontCare;
	Hobject  Line, TotLine, select_object;
	HTuple   Num;
	Hlong idx;
	HTuple HomMat2D;

	Hobject RegionDiff;
	Hobject OutRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean;
	Hobject ComplRgn, EmpImage;

	Hobject Region, SubImage;
	HTuple DiffAng;

	Hobject MeanImage;
	Hlong center;
	Hobject TotDirtyRgn, DirtyRgn, LineRgn, ImageReduced;
	Hlong ins_area, tol_area, max;
	HTuple Min, Max, Area;

	Hobject NotSRRgn, EmpImage2, InterRgn;

	Hlong i, num;

	get_image_pointer1(*image, &Pointer, &Type, &Width, &Height);

	Hlong row;
	Hlong col;
	Hlong leng1;
	Hlong height;

	//LSS new
	if(use_out_hori)
	{
		row = (ins_region.top + ins_region.bottom) * 0.5;
		col = ins_region.left + 1;
		leng1 = (ins_region.bottom - ins_region.top) * 0.5;
		height = (ins_region.right - ins_region.left) - 1;
	}
	else
	{
		row = ins_region.top + 1;
		col = (ins_region.left + ins_region.right) * 0.5;
		leng1 = (ins_region.right - ins_region.left) * 0.5;
		height = (ins_region.bottom - ins_region.top) - 1;
	}


	//20100621 lss
	Hlong row_l = (ins_region2.top + ins_region2.bottom) * 0.5;
	Hlong col_l = ins_region2.left + 1;
	Hlong leng1_l = (ins_region2.bottom - ins_region2.top) * 0.5;
	Hlong height_l = (ins_region2.right - ins_region2.left) - 1;

	//20100621 lss
	Hlong row_r = (ins_region3.top + ins_region3.bottom) * 0.5;
	Hlong col_r = ins_region3.left + 1;
	Hlong leng1_r = (ins_region3.bottom - ins_region3.top) * 0.5;
	Hlong height_r = (ins_region3.right - ins_region3.left) - 1;

	Hobject RsltRegion, RsltRegion2, RsltUPRegion;

	Hobject TotLine_l, TotLine_r;

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltRegion2);
	gen_empty_obj(&RsltUPRegion);

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	if(0.4 > factor)
	{
		factor = 0.4;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(up_region, &RsltUPRegion, HomMat2D, HTuple("false"));

	//sub_image(*image, *crack_image, &SubImage, 1, 0);
	//emphasize(*image, &EmpImage, 20, 20, 2);
	emphasize(*image, &EmpImage, 10, 10, 2);
	emphasize(*image2, &EmpImage2, 10, 10, 2);

	gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(InsRegion, win);
	}

	//Image1/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	//20100621 lss
	gen_empty_obj(&TotLine_l);
	gen_empty_obj(&TotLine_r);	

	if(use_out_hori)
	{
		gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(-1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}
	else
	{
		gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}

	for(idx=1; idx<=height; idx+=1)
	{
		if(use_out_hori)
		{
			translate_measure(MeasureHandle0, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle0, row + idx, col);
		}

		measure_pos(EmpImage, MeasureHandle0, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		if(use_out_hori)
		{
			translate_measure(MeasureHandle1, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle1, row + idx, col);
		}

		measure_pos(EmpImage, MeasureHandle1, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);

			union2(TotLine, Line, &TotLine);
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			union2(TotLine, Line, &TotLine);
		}

	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);


	/////////////////////////////////////////////

	connection(TotLine, &TotLine);

	erosion_rectangle1(RsltUPRegion, &RsltUPRegion, 2.5, 2.5);
	intersection(TotLine, RsltUPRegion, &TotLine);

	//20100621 lss
	union2(TotLine, TotLine_l, &TotLine);
	union2(TotLine, TotLine_r, &TotLine);
	connection(TotLine, &TotLine);

	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltUPRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}

	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(50.5));

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));

	if(out_lead_use)
	{
		select_shape(TotLine, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
	}



	//Image2/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	//20100621 lss
	gen_empty_obj(&TotLine_l);
	gen_empty_obj(&TotLine_r);	


	if(use_out_hori)
	{
		gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(-1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}
	else
	{
		gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}

	for(idx=1; idx<=height; idx+=1)
	{
		if(use_out_hori)
		{
			translate_measure(MeasureHandle0, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle0, row + idx, col);
		}

		measure_pos(EmpImage2, MeasureHandle0, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		if(use_out_hori)
		{
			translate_measure(MeasureHandle1, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle1, row + idx, col);
		}

		measure_pos(EmpImage2, MeasureHandle1, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);
		}

	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);


	/////////////////////////////////////////////

	connection(TotLine, &TotLine);

	erosion_rectangle1(RsltUPRegion, &RsltUPRegion, 5.5, 2.5);
	intersection(TotLine, RsltUPRegion, &TotLine);

	//20100621 lss
	union2(TotLine, TotLine_l, &TotLine);
	union2(TotLine, TotLine_r, &TotLine);
	connection(TotLine, &TotLine);

	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltUPRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}

	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(50.5));

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));

	if(out_lead_use)
	{
		select_shape(TotLine, &RsltRegion2, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
	}
	//Image1/////////////////////////////////////
	//오염///////////////////////////////////////
	mean_image(*image3, &MeanImage, 10, 2);

	center = (ins_region.top + ins_region.bottom) * 0.5;
	
	gen_empty_obj(&TotDirtyRgn);

	for(i = ins_region.left; i < ins_region.right; i++)
	{
		//All
		gen_region_line(&LineRgn, ins_region.top, i, ins_region.bottom, i);

		reduce_domain(MeanImage, LineRgn, &ImageReduced);
		
		//Find suitable threshold
		min_max_gray(LineRgn, MeanImage, HTuple(20), &Min, &Max, _);
		

		max = (Hlong)Max[0];
		max = max - thresh_dirt;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		/*
		intensity(LineRgn, MeanImage, &Mean, _);
		mean = (Hlong)Mean[0];
		mean = mean - thresh_dirt;
		if( (0 > mean) || (255 < mean) )
		{
			mean = 0;
		}
		max = mean;
		*/
		
		threshold(ImageReduced, &DirtyRgn, 0, max);

		//connection(DirtyRgn, &DirtyRgn);
		//select_shape(DirtyRgn, &DirtyRgn, HTuple("area"), HTuple("and"), HTuple(10000),  HTuple("max"));
		//concat_obj(TotDirtyRgn, DirtyRgn, &TotDirtyRgn);

		union2(TotDirtyRgn, DirtyRgn, &TotDirtyRgn);
	}

	//union1(TotDirtyRgn, &TotDirtyRgn);
	closing_circle(TotDirtyRgn, &TotDirtyRgn, 10.5);
	connection(TotDirtyRgn, &TotDirtyRgn);

	area_center(Region, &Area, _, _);
	ins_area = (Hlong)Area[0];
	tol_area = (Hlong)( ((double)ins_area) * (tolerance_dirt * 0.01) );

	select_shape(TotDirtyRgn, &TotDirtyRgn, HTuple("area"), HTuple("and"), HTuple(tol_area),  HTuple("max"));

	if(out_lead_dirt_use)
	{
		concat_obj(RsltRegion, TotDirtyRgn, &RsltRegion);
	}

	/////////////////////////////////////////////

	//Image1
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];

	if( (0 < *rslt_num) && out_lead_use)//LSS new
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}

	copy_obj(RsltRegion, rslt_region, 1, -1);

	//Image2
	count_obj(RsltRegion2, &Num);

	*rslt_num2 = (Hlong)Num[0];

	if( (0 < *rslt_num2) && out_lead_use)//LSS new
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}

	copy_obj(RsltRegion2, rslt_region2, 1, -1);

	copy_obj(RsltUPRegion, rslt_up_region, 1, -1);

	close_all_measures();

	return rslt;
}

//SR 기포검사 & Hall 검사
BOOL C_VisionModule::SRBub(Hlong win, Hobject *image, Hobject *hall_image, Hobject *Green_image, 
						   Hobject *RefGreen_image,  Hobject *RefBlue_image, 
						   Hobject *ink_chip_image,
						   RECT ins_sr_region, RECT ins_film_region, RECT chip_search_region,
						   double offset_ox, double offset_oy, double offset_rx, double offset_ry,
						   int abs_thresh, int var_thresh, int mold_fact, int tolerance, 
						   int tol_max, int tol_cnt,
						   int hall_thresh, int hall_tol,
						   int fold_thresh, int fold_tol, 
						   int ink_chip_size, int ink_thresh,int ink_area_tol, int ink_factor,
							Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region,
							Hlong *rslt_hall_num, Hobject *rslt_hall_region, Hobject *rslt_hallins_region)
{
	BOOL rslt = TRUE;
	Hobject Image, ImageTrans, RegionDiff, ImageReduced, RefImage, RefImageReduced, ImageSub, RegionHysteresis2, RegionErosion2, RegionDilation2, RegionDilation3;
	Hobject	RegionRect, RegionRectChip, ChipRect, OCVRect;
	HTuple HomMat2D, Num;
	Hobject select_object;
	Hobject RegionErosion, RegionDilation;
	Hobject ImageTrans2, TempRgn1, PunchReduce, PunchThresh, PunchRgn; 

	Hobject OutRgn, SelectRgn, ResultRgn, ChipRegionRect, ChipSearchRect; 
	HTuple Mean, OutMin, OutMax;
	Hobject ComplRgn; 

	Hobject MeanImage, MeanImageReduced, HallImageReduced, HallRegion, DilRegion;
	long offset_x, offset_y;

	Hobject RedImageEmpha, RefImageEmpha, ImageBubReduced, ThreshBub, SelectRgn2; 

	int i;
	int BubCount, BubCount2; 
	int thresh, high_threshold;
	HTuple start_t, end_t;

	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	gen_rectangle1(&RegionRect, (Hlong)(ins_sr_region.top), 
								(Hlong)(ins_sr_region.left), 
								(Hlong)(ins_sr_region.bottom), 
								(Hlong)(ins_sr_region.right));


	gen_rectangle1(&RegionRectChip, (Hlong)(ins_film_region.top), 
								(Hlong)(ins_film_region.left), 
								(Hlong)(ins_film_region.bottom), 
								(Hlong)(ins_film_region.right));

	vector_angle_to_rigid(offset_oy, offset_ox, 0, offset_ry, offset_rx, HTuple(0), &HomMat2D);

	reduce_domain(*RefGreen_image, RegionRectChip, &ImageTrans);
	reduce_domain(*RefBlue_image, RegionRectChip, &ImageTrans2);

	affine_trans_image(ImageTrans, &ImageTrans, HomMat2D, HTuple("constant"), HTuple("false"));
	affine_trans_image(ImageTrans2, &ImageTrans2, HomMat2D, HTuple("constant"), HTuple("false")); 
	affine_trans_region(RegionRect, &RegionRect, HomMat2D, HTuple("false")); 

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	//sprintf(temp,"[1:%f]", (double)(HTuple(end_t - start_t)[0]) );
	//TRACE(temp);
	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	reduce_domain(*Green_image, RegionRect, &ImageReduced);
	reduce_domain(ImageTrans, RegionRect, &RefImageReduced);

	Hobject RegionThresh, RegionThreshRef, DiffRgn2;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced, &RegionThresh, HTuple(abs_thresh-5), HTuple(255));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_16!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	set_check("~give_error");

	abs_thresh = 245;

	herr = threshold(RefImageReduced, &RegionThreshRef, HTuple(abs_thresh-15), HTuple(255)); 
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_17!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	difference(RegionThresh, RegionThreshRef, &DiffRgn2);
	connection(DiffRgn2, &DiffRgn2);
	
	erosion_rectangle1(DiffRgn2, &DiffRgn2, HTuple(1), HTuple(3)); 

	select_shape(DiffRgn2, &RegionDiff, HTuple("height"), HTuple("and"), HTuple(5), HTuple("max"));
	select_shape(RegionDiff, &RegionDiff, HTuple("width"), HTuple("and"), HTuple(2), HTuple("max"));  

	dilation_rectangle1(RegionDiff, &RegionDiff, HTuple(1), HTuple(1));
	connection(RegionDiff, &RegionDiff);

	//경계값 및 max tol 비교
	BOOL max_chk = FALSE;
	count_obj(RegionDiff, &Num);
	BubCount = (Hlong)Num[0]; 

	//기포 검사 
	Hobject BubRgnTemp, BubRgnTemp2; 
	long CheckBigBub, CheckSmallBub, CheckTotBub;

	gen_empty_obj(&BubRgnTemp);
	gen_empty_obj(&BubRgnTemp2);
	CheckBigBub = 0;
	CheckSmallBub = 0;
	CheckTotBub = 0;

	long CheckSize;
	HTuple CRow0, CCol0, CRow1, CCol1;//, CArea;
	
	HTuple deviInt, RefInt, AreaBub;
	if(0 < (Hlong)Num[0])
	{	
		for(i = 0; i < BubCount; i++) 
		{
			select_obj(RegionDiff, &SelectRgn, i + 1);
			smallest_rectangle1(SelectRgn, &CRow0, &CCol0, &CRow1, &CCol1);
			CheckSize = (Hlong)CRow1[0] - (Hlong)CRow0[0];
			
			if( CheckSize >= tolerance && CheckSize < tol_max )
			{
				union2(BubRgnTemp2, SelectRgn, &BubRgnTemp2);
				CheckSmallBub++;
			}
			else if(CheckSize >= tol_max)
			{
				union2(BubRgnTemp, SelectRgn, &BubRgnTemp);
				CheckBigBub++;
			}	
		}
		if(CheckSmallBub < tol_cnt)
		{
			gen_empty_obj(&BubRgnTemp2);
		}
		
		if( CheckSmallBub >= tol_cnt || CheckBigBub > 0 )
		{
			union2(BubRgnTemp, BubRgnTemp2, &BubRgnTemp);
			connection(BubRgnTemp, &BubRgnTemp);
			count_obj(BubRgnTemp, &Num);
			BubCount2 = (Hlong)Num[0]; 
			
			for(i = 0; i < BubCount2; i++) 
			{
				select_obj(BubRgnTemp, &SelectRgn2, i + 1);
				intensity(SelectRgn, ImageTrans, &RefInt, _);
				intensity(SelectRgn, *Green_image, &AreaBub, _);
				
				if((Hlong)RefInt[0] < 170)//(Hlong)deviInt[0] < 15 && (Hlong)RefInt[0] < 210 && (Hlong)AreaBub[0] >10 ) 
				{
					union2(SelectRgn2, *rslt_region, rslt_region );
					rslt = FALSE;
					
					if(win)
					{
						set_color(win, HTuple("red"));
						disp_obj(*rslt_region, win);
					}
				}
				else if((Hlong)AreaBub[0] > 250 && (Hlong)RefInt[0] < 210) 
				{
					union2(SelectRgn2, *rslt_region, rslt_region );
					rslt = FALSE;
					
					if(win)
					{
						set_color(win, HTuple("red"));
						disp_obj(*rslt_region, win);
					}
				}
			}
			if(!rslt)
			{
				connection(*rslt_region, rslt_region);
				count_obj(*rslt_region, &Num);
				*rslt_num = (Hlong)Num[0];
			}	
		}	
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//InkChip 검사 시작//검사결과는 기포/잉그칩
	//inkchip 샘플 불량 사이즈 14*14 pixel 최소 290um			20150715 by

	Hobject InkChipReduced, InkChipThresh, InkChipErosionRgn, InkChipSelectRgn, InkChipRgnTemp, InkChipInsRgn;
	long InkChipCount;
	HTuple InkChipMean, Row, Col;
	HTuple Row0, Col0, Row1, Col1;
	long XSize, YSize;

	area_center(RegionRect, _, &Row, &Col);		// SR-Search 영역 센터

	//20150131 by 잉크칩 검색범위 영역 설정
	if(ink_area_tol < 0)
		ink_area_tol = 1;
	else if(ink_area_tol > 100)
		ink_area_tol = 100;

	double mag = (double)ink_area_tol / 100;
	long x = (ins_sr_region.right - ins_sr_region.left) * mag;

	gen_rectangle1(&InkChipInsRgn, Row - x, 
									Col - x, 
									Row + x, 
									Col + x);

	reduce_domain(*ink_chip_image, InkChipInsRgn, &InkChipReduced);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(InkChipInsRgn, win);
	}

	//부각시킨다. 
	if(ink_factor < 0)
		ink_factor = 1;
	else if(ink_factor > 20)
		ink_factor = 20;

	emphasize(InkChipReduced, &InkChipReduced, HTuple(30), HTuple(3), HTuple(ink_factor));	//20150715 by

	set_check("~give_error");

	herr = threshold(InkChipReduced, &InkChipThresh, HTuple(0), HTuple(ink_thresh));
	
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_InkChip!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	//closing_circle(InkChipThresh, &InkChipThresh, 1);
	connection(InkChipThresh, &InkChipThresh);
	

	//20150715 by 원형에 가까운 것을 찾는다
	select_shape(InkChipThresh,&InkChipSelectRgn, HTuple("width").Concat("height"), HTuple("and"), HTuple(ink_chip_size).Concat(ink_chip_size), HTuple("max").Concat("max"));
	select_shape (InkChipSelectRgn, &InkChipSelectRgn, HTuple("circularity"), HTuple("and"), HTuple(0.5), HTuple(1));

	count_obj(InkChipSelectRgn, &Num);
	InkChipCount = (Hlong)Num[0]; 

	if(0 < InkChipCount)
	{	
		for(i = 0; i < InkChipCount; i++) 
		{
			select_obj(InkChipSelectRgn, &SelectRgn, i + 1);
			intensity(SelectRgn, *image, &InkChipMean, _);
			smallest_rectangle1(SelectRgn, &Row0, &Col0, &Row1, &Col1);
			XSize = (Hlong)Col1[0] - (Hlong)Col0[0];
			YSize = (Hlong)Row1[0] - (Hlong)Row0[0];
			if(YSize < 1)
			{
				YSize = 1;
			}
			if(XSize < 1) 
			{
				XSize = 1;
			}

			rslt = FALSE;

			//if((Hlong)InkChipMean[0] > 180 && (Hlong)(YSize/XSize) < 3)
			{
				union2(SelectRgn, *rslt_region, rslt_region );
				rslt = FALSE;
						
				if(win)
				{
					set_color(win, HTuple("red"));
					disp_obj(*rslt_region, win);
				}
			}
		}

		/*
		union2(SelectRgn, *rslt_region, rslt_region );
		rslt = FALSE;
				
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}
		*/

		if(!rslt)
		{
			connection(*rslt_region, rslt_region);
			count_obj(*rslt_region, &Num);
			*rslt_num = (Hlong)Num[0];
		}	
	}
	//Ink chip 검사 끝
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////
	//찍힘///////////////////////////////////////////
	/////////////////////////////////////////////////
	long CheckDentCnt(0);

	Hobject FoldInsRgnEmpha, FoldImageReduced,FoldImageEmpha,FoldImageInvert, FoldRegion;

	affine_trans_region(RegionRect,&RegionRect,HomMat2D,HTuple("false"));

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	// chip 검색 영역  
	gen_rectangle1(&ChipSearchRect, (Hlong)(chip_search_region.top + offset_y), 
								(Hlong)(chip_search_region.left + offset_x), 
								(Hlong)(chip_search_region.bottom + offset_y), 
								(Hlong)(chip_search_region.right + offset_x));
			
	gen_rectangle1(&ChipRegionRect, (Hlong)(ins_film_region.top + offset_y), 
								(Hlong)(ins_film_region.left + offset_x), 
								(Hlong)(ins_film_region.bottom + offset_y), 
								(Hlong)(ins_film_region.right + offset_x));


	difference(ChipRegionRect, ChipSearchRect, &TempRgn1);

	reduce_domain(ImageTrans2, ChipRegionRect, &PunchReduce);

	/*
	//펀치 찾기
	set_check("~give_error");
	herr = threshold(PunchReduce, &PunchThresh, HTuple(0), HTuple(1));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_18!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	dilation_circle(PunchThresh, &PunchRgn, HTuple(3));
	*/
	difference(TempRgn1, PunchRgn, rslt_hallins_region);


	///////////////////////////////////////////////////
	//찍힘 / 구겨짐 ///////////////////////////////////
	///////////////////////////////////////////////////
	Hobject HallImage, InsRgn, OutRgn2;

	Hobject RefImageReduced2, RegionThreshRef2, RegionEmpRef2;
	Hobject TempDilationRef1, TempDilationRef2, TempErosionRef1, TempErosionRef2, TempFillUp1, TempFillUp2;
	Hobject InterRefRgn;
	HTuple IntenInterRgn;

	//어두운 것
	fold_thresh = 255 - fold_thresh;//by 

 	high_threshold = fold_thresh + 20;
	thresh = fold_thresh;       

	//조명 180 / Blue : 1.92157
 	if(0 >= thresh) 
	{
		thresh = 10;  
		high_threshold = 30;
	}
	else if(255 <= high_threshold)
	{
		thresh = 230;  
		high_threshold = 255;
	}

	//어두운 것

	//Reduce Domain전에서 Emphaize를 수행 한다. 
	/*//LSS new
	emphasize(*hall_image,&FoldInsRgnEmpha,HTuple(20),HTuple(20),HTuple(2));
	dilation_rectangle1(*rslt_hallins_region, &InsRgn, 10, 10);
	reduce_domain(FoldInsRgnEmpha, InsRgn, &HallImage);

	reduce_domain(ImageTrans, *rslt_hallins_region, &RefImageReduced2);
	emphasize(RefImageReduced2, &RegionEmpRef2, 3, 3, 1); 

	set_check("~give_error");
	herr = threshold(RegionEmpRef2, &RegionThreshRef2, HTuple(100), HTuple(255));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_19!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	dilation_rectangle1(RegionThreshRef2, &TempDilationRef1, 2, 2);
	fill_up(TempDilationRef1, &TempFillUp1);
	erosion_rectangle1(TempFillUp1, &TempErosionRef1, 1, 5);
	dilation_rectangle1(TempErosionRef1, &TempDilationRef2, 1, 15);
	fill_up(TempDilationRef2, &TempFillUp2);
	erosion_rectangle1(TempFillUp2, &TempErosionRef2, 1, 10);
	*/

	erosion_rectangle1(*rslt_hallins_region, &InsRgn, 10, 10);
	
	emphasize(*hall_image, &FoldInsRgnEmpha,HTuple(25),HTuple(25),HTuple(2));
	//write_image(FoldInsRgnEmpha , "bmp", 0, "d:\\aaa.bmp");

	reduce_domain(FoldInsRgnEmpha, InsRgn, &HallImage);
	//write_image(HallImage , "bmp", 0, "d:\\bbb.bmp");
	
	invert_image(HallImage, &FoldImageInvert); //이미지 역상 
	//write_image(FoldImageInvert , "bmp", 0, "d:\\ccc.bmp");
	
	//mean_image(FoldImageInvert, &FoldImageInvert,10,10);
	mean_image(FoldImageInvert, &FoldImageInvert,5,5);
	//write_image(FoldImageInvert , "bmp", 0, "d:\\ddd.bmp");
	

	set_check("~give_error");
	herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(thresh),HTuple(high_threshold),HTuple(20));
	//hysteresis는 밝은색을 찾는 함수(현재는 검은색을 찾기위해 역상 이미지를 사용)
	//herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(60),HTuple(high_threshold),HTuple(20));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_00!... at %ld, %ld",(Hlong)thresh, (Hlong)high_threshold);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(FoldRegion, &FoldRegion);

    if(win)
	{
		
		disp_obj(FoldImageInvert, win);
		set_color(win, HTuple("green"));
		disp_obj(*rslt_hallins_region, win);
		set_color(win, HTuple("blue"));
		disp_obj(FoldRegion, win);
	}

	select_shape(FoldRegion, &FoldRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(fold_tol).Concat(fold_tol), HTuple("max").Concat("max"));
	select_shape(FoldRegion, &FoldRegion, HTuple("compactness"), HTuple("and"), HTuple(0), HTuple(20));
	count_obj(FoldRegion, &Num);

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	//sprintf(temp,"[2:%f]", (double)(HTuple(end_t - start_t)[0]) );
	//TRACE(temp);
	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	long LimitTopSide, LimitBtmSide;
	smallest_rectangle1(*rslt_hallins_region, &Row0, &Col0, &Row1, &Col1);

	LimitTopSide = (Hlong)Row0[0] + 200;
	LimitBtmSide = (Hlong)Row1[0] - 200;

	HTuple Area, SelectRow;
	HTuple Ra, Rb, nCompactness; 
	CheckDentCnt = (Hlong)Num[0];
	HTuple InterArea; 

	BOOL m_bCheckResult; 

	//LSS new
	m_bCheckResult = FALSE; 
	if(0 < CheckDentCnt)
	{
		concat_obj(*rslt_hall_region, FoldRegion, rslt_hall_region);
		*rslt_hall_num = CheckDentCnt;
		m_bCheckResult = TRUE; 
	}

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	//sprintf(temp,"[3:%f]", (double)(HTuple(end_t - start_t)[0]) );
	//TRACE(temp);

	if(0 < *rslt_hall_num)
	{
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_hall_region, win);
		}
		
		rslt = FALSE;
	}

	return rslt;
}


//////////////////////////////////////////////////////////////////////
// Vision module
//////////////////////////////////////////////////////////////////////

C_VisionModule::C_VisionModule()
{
	int i;

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_VDat[i] = new C_VisionData;
	}

	ResetInspectPara();

	ResetSystemPara();
	ResetMainSysRslt();



	ResetLotInfo();

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		ResetInspectRslt(i);
		ResetInspectRsltRgn(i);
		ResetInspectRgn(i);
		ResetInspectNGRslt(i);
		ResetInspectNGRsltRgn(i);

		m_lWindow[i] = 0;
	}

	m_iNGViewMode = -1;

	m_listRsltData.clear();			//20191101 by IMSI TEST필요
}

C_VisionModule::~C_VisionModule()
{
	clear_all_shape_models();

	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		delete m_VDat[i];
	}
}

//Get/Set data
Hobject *C_VisionModule::GetHalImage(int cam_idx, int buf_idx, int img_idx)
{
	
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGImage[m_iNGViewMode][img_idx];
	}

}

Hobject *C_VisionModule::GetHalRedImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalRedImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGRedImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalGreenImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalGreenImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGGreenImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalBlueImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalBlueImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGBlueImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalNGImage(int cam_idx, int ng_view_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalNGImage[ng_view_idx][img_idx];
}	


void C_VisionModule::SetHalNGImage(int cam_idx, int buf_idx, int ng_view_idx)
{
	int i;

	if(-1 == m_iNGViewMode)
	{
		for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[1]) && (i < IMAGE_MAP_MAX_NUM); i++)
		{
			//copy_image(m_VDat[cam_idx]->m_HalImage[buf_idx][i], &m_VDat[cam_idx]->m_HalNGImage[ng_view_idx][i]);
			m_VDat[cam_idx]->m_HalNGImage[ng_view_idx][i] = m_VDat[cam_idx]->m_HalImage[buf_idx][i];
		}
	}
}

Hobject *C_VisionModule::GetHalRsltRgn(int cam_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalRsltRgn[rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalInspectRgn(int cam_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalInsRgn[rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalNGRsltRgn(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalNGPotRsltRgn(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx)	//Pot length 20150818 by
{
	return &m_VDat[cam_idx]->m_HalNGPotRsltRgn[ng_view_idx][rgn_idx][ins_idx];
}

int C_VisionModule::GetHalNGPotRsltAverageLength(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx)	//Pot length 20150903 by
{
	return m_VDat[cam_idx]->m_nNGPotRsltAverageLength[ng_view_idx][rgn_idx][ins_idx];
}

int C_VisionModule::GetHalNGPotRsltCenterLength(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx)	//Pot length 20150903 by
{
	return m_VDat[cam_idx]->m_nNGPotRsltCenterLength[ng_view_idx][rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalMasterImage;
}

Hobject *C_VisionModule::GetHalSRMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalSRMasterImage;
}

Hobject *C_VisionModule::GetHalLeadMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalLeadMasterImage;
}

Hobject *C_VisionModule::GetHalSROutMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalSROutMasterImage;
}

Hobject *C_VisionModule::GetHalChipMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalChipMasterImage;
}

Hobject *C_VisionModule::GetHalTaughtImage(int cam_idx, int map_idx)
{
	return &m_VDat[cam_idx]->m_HalTaughtImage[map_idx];
}

typeInspectPara C_VisionModule::GetInspectPara(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara;
}

void C_VisionModule::SetInspectPara(int cam_idx, typeInspectPara para)
{
	m_VDat[cam_idx]->m_InsPara = para;
}

void C_VisionModule::ResetInspectPara()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		memset(&m_VDat[i]->m_InsPara, NULL, sizeof(m_VDat[i]->m_InsPara));
	}
}

typeInspectRslt C_VisionModule::GetInspectRslt(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsRslt;
}

void C_VisionModule::SetInspectRslt(int cam_idx, typeInspectRslt rslt)
{
	m_VDat[cam_idx]->m_InsRslt = rslt;
}

void C_VisionModule::ResetInspectRslt(int cam_idx)
{
	
	memset(&m_VDat[cam_idx]->m_InsRslt, NULL, sizeof(m_VDat[cam_idx]->m_InsRslt));
}

void C_VisionModule::ResetInspectRsltRgn(int cam_idx)
{
	int i, j, k;
	
	i = cam_idx;
	//Variable clear
	for(j = 0; j < SUB_RGN_MAX_NUM; j++)
	{
		for(k = 0; k < RESULT_INS_MAX_NUM; k++)
		{
			//clear_obj(m_VDat[i]->m_HalRsltRgn[j][k]); 
			gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[j][k]);
		}
	}
}

void C_VisionModule::ResetInspectRgn(int cam_idx)
{
	int i, j, k;

	i = cam_idx;
	
	//Variable clear
	for(j = 0; j < SUB_RGN_MAX_NUM; j++)
	{
		for(k = 0; k < INS_RGN_MAX_NUM; k++)
		{
			//clear_obj(m_VDat[i]->m_HalRsltRgn[j][k]); 
			gen_empty_obj(&m_VDat[i]->m_HalInsRgn[j][k]);
		}
	}
}

typeInspectNGRslt C_VisionModule::GetInspectNGRslt(int cam_idx, int ng_view_idx)
{
	return m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx];
}

void C_VisionModule::SetInspectNGRslt(int cam_idx, int ng_view_idx, /*typeShiftRslt shift_rslt,*/ typeInspectRslt ins_rslt)
{
	//m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_ShiftRslt = shift_rslt;
	m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt = ins_rslt;
}

BOOL C_VisionModule::SaveInspectNGRslt(int cam_idx, int ng_view_idx, int rgn_idx, CString path)
{
	BOOL rslt = TRUE;

	Hobject SubRgn, TotRgn, UnionSubRgn;
	HTuple Num;
	int num;

	CString temp;
	
	gen_empty_obj(&SubRgn);
	gen_empty_obj(&TotRgn);
	gen_empty_obj(&UnionSubRgn);
	
	//Region/////////////////////////////////////
	for(int i = 0; i < RESULT_INS_MAX_NUM; i++)
	{
		count_obj(*GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i), &Num);
		num = (Hlong)Num[0];
		if( 0 == num )
		{
			gen_empty_region(&UnionSubRgn);
		}
		else
		{
			//if XLD
			if( (i == 2) || (i = 43) )//2 : align pattern, 43 : OCV pattern
			{
				SubRgn = *GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i);
				//gen_region_contour_xld(SubRgn, &SubRgn, "fill");
				union1(SubRgn, &UnionSubRgn);
			}
			else
			{
				SubRgn = *GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i);
				union1(SubRgn, &UnionSubRgn);
			}
		}
		
		concat_obj(TotRgn, UnionSubRgn, &TotRgn);
	}
	
	//Write region
	temp = path + ".rgn";
	write_region(TotRgn, temp);

	//Data///////////////////////////////////////
	m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx = rgn_idx;

	temp = path + ".dat";
	//NG inspection result
	//typeInspectNGRslt m_InsNGRslt[NG_VIEW_MAX_NUM];
	HANDLE fd=CreateFile(   temp,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		//AfxMessageBox("Error : Can't save NG result file!" + path_sys);
		rslt = FALSE;
	}
	else
	{
		DWORD len;

		WriteFile(fd, &m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx], sizeof(m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx]), &len, NULL);

		CloseHandle(fd);
	}

	fd=0;
		
	return rslt;	
}

BOOL C_VisionModule::LoadInspectNGRslt(int cam_idx, int ng_view_idx, CString path)
{
	BOOL rslt = TRUE;

	Hobject SubRgn, TotRgn;
	HTuple Num;
	long num;
	CString temp;
	int rgn_idx, i;

	temp = path + ".dat";
	HANDLE fd = CreateFile( temp,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		//AfxMessageBox("Error : Can't load NG result file!" + path_sys);
		rslt = FALSE;
	}
	else
	{
		DWORD len;

		//Setup
		ReadFile(fd, &m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx], sizeof(m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx]), &len, NULL);

		CloseHandle(fd);
	}

	fd=0;

	if( (0 > m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx) || (NG_VIEW_MAX_NUM <= m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx) )
	{
		m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx = -1;
	}

	rgn_idx = m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx;

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		if(i == rgn_idx)
		{
			m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt.m_bRegionSkip[i] = TRUE;
		}
		else
		{
			m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt.m_bRegionSkip[i] = FALSE;			
		}
	}
	
	//Region/////////////////////////////////////
	//Read region
	temp = path + ".rgn";
	read_region(&TotRgn, temp);
	count_obj(TotRgn, &Num);
	num = (Hlong)Num[0];
	//Region union
	for(i = 0; (i < RESULT_INS_MAX_NUM) && (i < num); i++)
	{
		select_obj(TotRgn, GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i), i + 1);
		connection(*GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i), 
					GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i));
	}
	
	return rslt;
}

void C_VisionModule::ResetInspectNGRslt(int cam_idx)
{
	int i, j;
	i = cam_idx;
	for(j = 0; j < NG_VIEW_MAX_NUM; j++)
	{
		memset(&m_VDat[i]->m_InsNGRslt[j], NULL, sizeof(m_VDat[i]->m_InsNGRslt[j]));
	}
}

void C_VisionModule::ResetInspectNGRsltRgn(int cam_idx)
{
	int i, j, k, l;
	
	i = cam_idx;
	//Variable clear
	
	for(j = 0; j < NG_VIEW_MAX_NUM; j++)
	{
		for(k = 0; k < SUB_RGN_MAX_NUM; k++)
		{
			for(l = 0; l < RESULT_INS_MAX_NUM; l++)
			{
				gen_empty_obj(&m_VDat[i]->m_HalNGRsltRgn[j][k][l]);
			}
		}
	}
	
}

/////////////////////////////////////////////////////////////////
//20191101 by IMSI TEST필요
void C_VisionModule::ClearNGRsltData()
{
	m_listRsltData.clear();
}

int C_VisionModule::GetListSize() 
{ 
	return m_listRsltData.size();
}

void C_VisionModule::NGListPopFront()
{ 
	m_listRsltData.pop_front();
} 

void C_VisionModule::PushRsltData(int cam_idx)
{
	typeVisionResultData temp;

	int i, j, k;
	long num;
	HTuple Num;
	Hobject Region;
	
	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		for(j = 0; j < RESULT_INS_MAX_NUM; j++)
		{
			gen_empty_obj(&temp.m_HalNGRsltRgn[i][j]);

			count_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Num);
			num = (Hlong)Num[0];

			for(k = 0; k < num; k++)
			{
				copy_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Region, k + 1, 1); 
				concat_obj(temp.m_HalNGRsltRgn[i][j], Region, &temp.m_HalNGRsltRgn[i][j]);
			}
		}
	}

	temp.m_InsRslt = m_VDat[cam_idx]->m_InsRslt;

	m_listRsltData.push_back(temp);
}

typeInspectRslt C_VisionModule::GetInspRslt()
{
	typeVisionResultData temp;

	temp  = m_listRsltData.front();

	return temp.m_InsRslt;

}

void C_VisionModule::SetInspectNGRsltRgn2(int cam_idx, int ng_view_idx)
{
	typeVisionResultData temp;

	int i, j, k;
	long num;
	HTuple Num;
	Hobject Region;

	temp = m_listRsltData.front();
	
	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		for(j = 0; j < RESULT_INS_MAX_NUM; j++)
		{
			gen_empty_obj(&m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);

			count_obj(temp.m_HalNGRsltRgn[i][j], &Num);
			num = (Hlong)Num[0];

			for(k = 0; k < num; k++)
			{
				copy_obj(temp.m_HalNGRsltRgn[i][j], &Region, k + 1, 1); 

				concat_obj(m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j], 
						   Region, 
						   &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////


void C_VisionModule::SetInspectNGRsltRgn(int cam_idx, int ng_view_idx)
{
	int i, j, k;
	long num;
	HTuple Num;
	Hobject Region;
	
	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		for(j = 0; j < RESULT_INS_MAX_NUM; j++)
		{
			gen_empty_obj(&m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);

			count_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Num);
			num = (Hlong)Num[0];
			for(k = 0; k < num; k++)
			{
				copy_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Region, k + 1, 1); 
				concat_obj(m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j], 
						   Region, 
						   &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);
			}
		}
	}

	//Pot length 20150818 by
	if(VISION_CAM_TOP1 == m_iVisIdx)	//20191028 by if문 추가
	{
		for(i = 0; i < SUB_RGN_MAX_NUM; i++)
		{
			for(j = 0; j < LENGTH_POINT_NUM; j++)
			{
				gen_empty_obj(&m_VDat[cam_idx]->m_HalNGPotRsltRgn[ng_view_idx][i][j]);

				count_obj(m_VDat[cam_idx]->m_HalPotRsltRgn[i][j], &Num);
				num = (Hlong)Num[0];
				for(k = 0; k < num; k++)
				{
					copy_obj(m_VDat[cam_idx]->m_HalPotRsltRgn[i][j], &Region, k + 1, 1); 
					concat_obj(m_VDat[cam_idx]->m_HalNGPotRsltRgn[ng_view_idx][i][j], 
							   Region, 
							   &m_VDat[cam_idx]->m_HalNGPotRsltRgn[ng_view_idx][i][j]);
				}

				//20150903 by
				m_VDat[cam_idx]->m_nNGPotRsltAverageLength[ng_view_idx][i][j] = m_VDat[cam_idx]->m_nPotRsltAverageLength[i][j];
				m_VDat[cam_idx]->m_nNGPotRsltCenterLength[ng_view_idx][i][j] = m_VDat[cam_idx]->m_nPotRsltCenterLength[i][j];			
			}
		}
	}
}

typeSystemPara C_VisionModule::GetSystemPara(int cam_idx)
{
	return m_VDat[cam_idx]->m_SysPara;
}

void C_VisionModule::SetSystemPara(int cam_idx, typeSystemPara rslt)
{
	m_VDat[cam_idx]->m_SysPara = rslt;
}

void C_VisionModule::ResetSystemPara()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		memset(&m_VDat[i]->m_SysPara, NULL, sizeof(m_VDat[i]->m_SysPara));
	}
}

int C_VisionModule::GetCOFNum(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara.m_iParaBase[0];
}

int C_VisionModule::GetMapNum(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara.m_iParaBase[1];
}

typeMainSystemRslt C_VisionModule::GetMainSysRslt()
{
	return m_MainSysRslt;
}

void C_VisionModule::SetMainSysRslt(typeMainSystemRslt rslt)
{
	m_MainSysRslt = rslt;
}

void C_VisionModule::ResetMainSysRslt()
{
	memset(&m_MainSysRslt, NULL, sizeof(m_MainSysRslt));
}

typeLotInfo C_VisionModule::GetLotInfo()
{
	return m_Lot;
}

void C_VisionModule::SetLotInfo(typeLotInfo lot)
{
	m_Lot = lot;
}

void C_VisionModule::ResetLotInfo()
{
	memset(&m_Lot, NULL, sizeof(m_Lot));
}

//Set handle(for test)
/*
void C_VisionModule::SetWindow(int cam_idx, Hlong win)
{
	m_lWindow[cam_idx] = win;
}
*/

BOOL C_VisionModule::LoadInspectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	char temp[DAT_STR_LENG];
	int i, j, k;
	CString file_path, pat_path, extention, master_path;
	Herror Herr;

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				//Base///////////////////////////////
				for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_BASE%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaBase[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_BASE%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaBase[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_BASE%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaBase[j] = atof(temp);
				}
				
				//Align//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_ALIGN; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Align%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaAlign[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				//fixed
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_shType = VISIONHALFUNC_MOD_RECT1;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleStart = 0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleExtend = 0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleStep = 0;
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_Optimize, "none");
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_Matric, "ignore_global_polarity");
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_SupPix, "interpolation");
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_Overlap = 0;
				
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_MatchNum = 1;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_Greed = 0.5;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleMin = 1.0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleMax = 1.0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleStep = 0;
				
				//read shape region
				pat_path = path + "\\PatternAlign."; 
				extention.Format("m%d", i);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDAlign);
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
					m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;

					// 20110531 Dongwhe* BEGIN 2nd Align Mark Shape Load
					// 두개의 마크 정보가 모두 읽히면 TRUE가 되도록 하기 위해
					// 첫번째 마크 정보 로딩에 성공한 경우에 두번째 마크 정보 로딩 시도를 하도록 작성
					// read shape region2
					pat_path = path + "\\PatternAlign2."; 
					extention.Format("m%d", i);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDAlign2);
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;	
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
					}
					// 20110531 Dongwhe* END
				}
				else
				{
					m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < PATTERN_INS_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPosX_Align%d_Pos%d", i, j, k);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						m_VDat[i]->m_InsPara.m_dPatternPosXAlign[j][k] = atof(temp);
						
						key.Empty();
						key.Format("CAM%d_iPosY_Align%d_Pos%d", i, j, k);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						m_VDat[i]->m_InsPara.m_dPatternPosYAlign[j][k] = atof(temp);
					}
				}
				/////////////////////////////////////
				
				//Chip///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_CHIP; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if( m_VDat[i]->m_InsPara.m_iParaChip[j] <= 0 )
					{
						switch(j)
						{
						case 0:			//모서리면 tolerance x
						case 30:		//모서리면 tolerance y
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 5;
							break;
						case 1:			//모서리면 threshold
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 50;
							break;
						case 2:			//칩외곽 검사 면적률
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 80;
							break;
						case 17:		//장변 tolerance x
						case 31:		//장변 tolerance y
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 8;
							break;
						case 18:		//장변 threshold
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 50;
							break;
						case 32:		//모서리면 검사 길이(영역)
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 50;
							break;
						case 21:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 4000;
							break;
						case 22:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 23:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 24:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 25:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 26:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 27:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 1000;
							break;
						case 28:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 10;
							break;
						case 29:
							m_VDat[i]->m_InsPara.m_iParaChip[j] = 10;
							break;
						default:
							
							break;
						}
					}
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Chip%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaChip[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/////////////////////////////////////
				
				//Lead///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_LEAD; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom = 200;
						}
					}
				}				
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}

				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					//20160303 by
					//A/M 파라미터 0일 경우 디폴트 값 설정..
					if( 0 >= m_VDat[i]->m_InsPara.m_iParaLead[j])
					{
					switch(j)
					{
						case 8:	// TOP 긁힘/변색 허용면적률
						case 16:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 25;
							break;
						case 9:	//TOP 긁힘/변색 밝기차
						case 17:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 50;
							break;
						case 10://TOP 전체변색 기준평균밝기
						case 18:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 210;
							break;
						case 11://TOP 전체변색 허용밝기차
						case 19:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 50;
							break;
						case 12://BOTTOM 긁힘/변색 허용면적률
						case 20:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 25;
							break;
						case 13://BOTTOM 긁힘/변색 밝기차
						case 21:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 50;
							break;
						case 14://BOTTOM 전체변색 기준평균밝기
						case 22:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 210;
							break;
						case 15://BOTTOM 전체변색 허용밝기차
						case 23:
							m_VDat[i]->m_InsPara.m_iParaLead[j] = 50;
							break;
						}
					}
					///////////////////////////////////////////////
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaLead[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/////////////////////////////////////
				
				//Film///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_FILM; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Film%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					//if(j==20) 
					//{
					//	m_VDat[i]->m_InsPara.m_iParaFilm[j] = 100;
					//}
					//else 
					//{
						m_VDat[i]->m_InsPara.m_iParaFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
					//}
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Film%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaFilm[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Film%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
				/*
				//read variation region
				pat_path = path + "\\VarFilm."; 
				extention.Format("v%d_%d", i, j);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_FilmVarID[j]);
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = TRUE;	
				}
				else
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
					*/
					m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
				
				/////////////////////////////////////
				
				//OCV////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_OCV; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaOCV[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < OCV_CHAR_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Left", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Top", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Right", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Bottom", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					
					if( (0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom) )
					{
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left = 100;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top = 100;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right = 200;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom = 200;
					}
					
					key.Empty();
					key.Format("CAM%d_Char_CenterX_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dCharCenterX[j] = atof(temp);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterY_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dCharCenterY[j] = atof(temp);
					
					//fixed
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_shType = VISIONHALFUNC_MOD_RECT1;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleStart = 0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleExtend = 0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleStep = 0;
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Optimize, "none");
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Matric, "use_polarity");
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_SupPix, "none");
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Overlap = 0;
					
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_MatchNum = 1;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Greed = 0.5;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleMin = 1.0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleMax = 1.0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleStep = 0;
					
					//read shape region
					pat_path = path + "\\PatternOCV."; 
					extention.Format("m%d_%d", i, j);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDOCV[j]);
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = TRUE;	
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = FALSE;
					}
					
					//read variation region
// 					pat_path = path + "\\VarOCV."; 
// 					extention.Format("v%d_%d", i, j);
// 					pat_path = pat_path + extention;
// 					set_check("~give_error");
// 					Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_CharVarID[j]);
// 					set_check("give_error");
// 					if(H_MSG_TRUE == Herr)
// 					{
// 						m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = TRUE;	
// 					}
// 					else
// 					{
// 						m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = FALSE;
// 					}
				}
				
				
				/////////////////////////////////////
				
				//SR/////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_SR; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom = 200;
						}
					}
				}
				
				/*
				for(j = 0; j < BOOL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/*
				for(j = 0; j < DBL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_SR%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaSR[j] = atof(temp);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/*
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					//read variation region
					//pat_path = path + "\\VarSR."; 
					//extention.Format("v%d_%d", i, j);
					//pat_path = pat_path + extention;
					//set_check("~give_error");
					//Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_SRVarID[j]);
					//set_check("give_error");
					//if(H_MSG_TRUE == Herr)
					//{
					//	m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = TRUE;	
					//}
					//else
					//{
					//	m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = FALSE;
					//}
				}
				*/
				
				/////////////////////////////////////
				switch(i)
				{
				case 0:
					//Read master image
					master_path = path + "\\MasterImage.bmp"; 

					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read SR master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\SRMasterImage.bmp"; 

							set_check("~give_error");
							Herr= read_image(&m_VDat[i]->m_HalSRMasterImage, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							if(H_MSG_TRUE == Herr)
							{
								
							}
							else
							{
								gen_image_const(&m_VDat[i]->m_HalSRMasterImage,HTuple("byte"),2048,2048);
							}
									break;
						case VISION_CAM_TOP2:

							break;
					}
										
					//Read Lead master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\LeadMasterImage.bmp"; 

							set_check("~give_error");
							Herr= read_image(&m_VDat[i]->m_HalLeadMasterImage, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							if(H_MSG_TRUE == Herr)
							{
								
							}
							else
							{
								gen_image_const(&m_VDat[i]->m_HalLeadMasterImage,HTuple("byte"),2048,2048);
							}
							break;
						case VISION_CAM_TOP2:

							break;
					}
					
					//Read SROut master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\SROutMasterImage.bmp";

							set_check("~give_error");
							Herr= read_image(&m_VDat[i]->m_HalSROutMasterImage, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							if(H_MSG_TRUE == Herr)
							{
								
							}
							else
							{
								gen_image_const(&m_VDat[i]->m_HalSROutMasterImage,HTuple("byte"),2048,2048);
							}
							break;
						case VISION_CAM_TOP2:

							break;
					}

					//Read Chip master image
					master_path = path + "\\ChipMasterImage.bmp"; 

					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalChipMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalChipMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read Taught image
					for(j = 0; (j < GetMapNum(0)) && (j < IMAGE_MAP_MAX_NUM); j++)
					{
						master_path = path + "\\TaughtImage";
						extention.Format("%d.bmp", j);

						master_path = master_path + extention;
						set_check("~give_error");
						Herr= read_image(&m_VDat[i]->m_HalTaughtImage[j], HTuple((LPCTSTR)master_path));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							
						}
						else
						{
							gen_image_const(&m_VDat[i]->m_HalTaughtImage[j],HTuple("byte"),2048,2048);
						}
					}
					
					//Read inspection region
					for(j = 0; j < SUB_RGN_MAX_NUM; j++)
					{
						for(k = 0; k < INS_RGN_MAX_NUM; k++)
						{
							pat_path = path + "\\InsRegion_"; 
							extention.Format("%d_%d_%d", i, j, k);
							pat_path = pat_path + extention;
							set_check("~give_error");
							Herr = read_region(&m_VDat[i]->m_HalInsRgn[j][k], pat_path);
							set_check("give_error");
							if(H_MSG_TRUE == Herr)
							{
								
							}
							else
							{
								gen_empty_obj(&m_VDat[i]->m_HalInsRgn[j][k]);
							}
						}
					}

					break;
				case 1:
					//Read master image
					master_path = path + "\\MasterImage_bottom.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalMasterImage,HTuple("byte"),2048,2048);
					}
					break;
				}
				
				//Light
				for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				{
					for(k = 0; k < LIGHT_CH_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPara_Map%d_Light%d", i, j, k);
						m_VDat[i]->m_InsPara.m_iLightVar[j][k] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
					}
				}

				//20100702 lss
				//PPunch(펀칭부)/////////////////////
				for(j = 0; j < RECT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Left", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Top", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Right", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Bottom", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					
					if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom) )
					{
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left = 100;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top = 100;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right = 200;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom = 200;
					}
				}
				
				
				/*
				for(j = 0; j < BOOL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/*
				for(j = 0; j < DBL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_PPUNCH%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaPPunch[j] = atof(temp);
				}
				*/
				
				/*
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				*/
				break;
			}
		}
		
		
	}
	fd=0;
	
	//UpdateSRBubPrepare();
	
	return rslt;
}

 //20110513 lee
void C_VisionModule::MakeDefaultInspPara()
{
	int i,j;
	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
		{
				m_VDat[i]->m_InsPara.m_iParaBase[0] = 2;
				m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT;
				m_VDat[i]->m_InsPara.m_iParaBase[2] = 8;
				m_VDat[i]->m_InsPara.m_dParaBase[1] = COF_PF_PITCH;
				m_VDat[i]->m_InsPara.m_dParaBase[2] = COF_WIDTH;
		}
	}
}


BOOL C_VisionModule::SaveInspectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j, k;
	CString file_path, pat_path, extention, master_path;
	Herror Herr;

	//Make director
	MakeDirectories(path);

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//write INI

		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			switch(i)
			{
			case 0:
			case 1:
				//Base///////////////////////////////
				for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				//Align//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_ALIGN; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				
				//write shape region
				if(m_VDat[i]->m_InsPara.m_bCreatMdIDAlign)
				{
					pat_path = path + "\\PatternAlign."; 
					extention.Format("m%d", i);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDAlign, HTuple(((LPCTSTR)pat_path)));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						// 20110531 Dongwhe* BEGIN 2nd Mark shape Save
						// 두번째 Align Mark도 저장 하도록 작성
						// m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;
						pat_path = path + "\\PatternAlign2."; 
						extention.Format("m%d", i);
						pat_path = pat_path + extention;
						set_check("~give_error");
						Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDAlign2, HTuple(((LPCTSTR)pat_path)));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;	
						}
						else
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
						}
						// 20110531 Dongwhe* END
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
					}
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < PATTERN_INS_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPosX_Align%d_Pos%d", i, j, k);
						dat.Empty();
						dat.Format("%f", m_VDat[i]->m_InsPara.m_dPatternPosXAlign[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_iPosY_Align%d_Pos%d", i, j, k);
						dat.Empty();
						dat.Format("%f", m_VDat[i]->m_InsPara.m_dPatternPosYAlign[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				/////////////////////////////////////
				
				//Chip//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_CHIP; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				/////////////////////////////////////
				
				//Lead//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_LEAD; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				/////////////////////////////////////
				
				//Film//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_FILM; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
				/*
				//write variation region
				if(m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j])
				{
				pat_path = path + "\\VarFilm."; 
				extention.Format("v%d_%d", i, j);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = write_variation_model(m_VDat[i]->m_InsPara.m_FilmVarID[j], HTuple(((LPCTSTR)pat_path)));
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = TRUE;	
				}
				else
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
				}
					*/
				}
				/////////////////////////////////////
				
				//OCV////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_OCV; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < OCV_CHAR_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Left", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Top", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Right", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Bottom", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterX_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dCharCenterX[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterY_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dCharCenterY[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					//write shape region
					if(m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j])
					{
						pat_path = path + "\\PatternOCV."; 
						extention.Format("m%d_%d", i, j);
						pat_path = pat_path + extention;
						set_check("~give_error");
						Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDOCV[j], HTuple(((LPCTSTR)pat_path)));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = TRUE;	
						}
						else
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = FALSE;
						}
					}
					
					//write variation region
					//if(m_VDat[i]->m_InsPara.m_bCreatCharVarID[j])
					//{
					//	pat_path = path + "\\VarOCV."; 
					//	extention.Format("v%d_%d", i, j);
					//	pat_path = pat_path + extention;
					//	set_check("~give_error");
					//	Herr = write_variation_model(m_VDat[i]->m_InsPara.m_CharVarID[j], HTuple(((LPCTSTR)pat_path)));
					//	set_check("give_error");
					//	if(H_MSG_TRUE == Herr)
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = TRUE;	
					//	}
					//	else
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = FALSE;
					//	}
					//}
				}
				
				/////////////////////////////////////
				
				//SR//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_SR; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				/*
				for(j = 0; j < BOOL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				/*
				for(j = 0; j < DBL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				/*
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					//write variation region
					//if(m_VDat[i]->m_InsPara.m_bCreatSRVarID[j])
					//{
					//	pat_path = path + "\\VarSR."; 
					//	extention.Format("v%d_%d", i, j);
					//	pat_path = pat_path + extention;
					//	set_check("~give_error");
					//	Herr = write_variation_model(m_VDat[i]->m_InsPara.m_SRVarID[j], HTuple(((LPCTSTR)pat_path)));
					//	set_check("give_error");
					//	if(H_MSG_TRUE == Herr)
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = TRUE;	
					//	}
					//	else
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = FALSE;
					//	}
					//}
				}
				*/
				/////////////////////////////////////
				
				switch(i)
				{
				case 0:
					//Write master image
					master_path = path + "\\MasterImage.bmp"; 

					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					
					//Write SR master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\SRMasterImage.bmp";

							set_check("~give_error");
							Herr= write_image(m_VDat[i]->m_HalSRMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							break;
						case VISION_CAM_TOP2:

							break;
					}
					
					//Write Lead master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\LeadMasterImage.bmp"; 

							set_check("~give_error");
							Herr= write_image(m_VDat[i]->m_HalLeadMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							break;
						case VISION_CAM_TOP2:

							break;
					}
					
					//Write SROut master image
					switch(GetVisIndex())
					{
						case VISION_CAM_TOP1:
						case VISION_CAM_BOTTOM:
							master_path = path + "\\SROutMasterImage.bmp"; 
							
							set_check("~give_error");
							Herr= write_image(m_VDat[i]->m_HalSROutMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
							set_check("give_error");
							break;
						case VISION_CAM_TOP2:

							break;
					}

					//Write Chip master image
					master_path = path + "\\ChipMasterImage.bmp"; 

					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalChipMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");


					//Write Taught image			
					//for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
					for(j = 0; (j < GetMapNum(0)) && (j < IMAGE_MAP_MAX_NUM); j++)
					{
						master_path = path + "\\TaughtImage"; 
						extention.Format("%d.bmp", j);
						master_path = master_path + extention;

						set_check("~give_error");
						Herr= write_image(m_VDat[i]->m_HalTaughtImage[j], "bmp", 0, HTuple((LPCTSTR)master_path));
						set_check("give_error");
					}
					break;
				case 1:
					//Write master image
					master_path = path + "\\MasterImage_bottom.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					break;
				}
				
				//Write inspection region
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < INS_RGN_MAX_NUM; k++)
					{
						pat_path = path + "\\InsRegion_"; 
						extention.Format("%d_%d_%d", i, j, k);
						pat_path = pat_path + extention;
						set_check("~give_error");
						Herr = write_region(m_VDat[i]->m_HalInsRgn[j][k], HTuple((LPCTSTR)pat_path));
						set_check("give_error");
					}
				}
				
				//Light
				for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				{
					for(k = 0; k < LIGHT_CH_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPara_Map%d_Light%d", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_iLightVar[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				//20100702
				//PPunch/////////////////////////////
				for(j = 0; j < RECT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Left", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Top", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Right", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Bottom", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				
				/*
				for(j = 0; j < BOOL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				*/
				
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				/*
				for(j = 0; j < DBL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				*/
				
				/*
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				*/
				break;				
			}
		}
		
	}
	fd=0;
	
	return rslt;
}

//Load/save system parameter
BOOL C_VisionModule::LoadSystemPara(CString path)
{
	char temp[DAT_STR_LENG];
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;

	path = path + "\\System.par";
	//file 존재 확인 
	HANDLE fd = CreateFile( path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//Read INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			/*
			key.Empty();
			key.Format("PC_INDEX_CH%d", i);
			m_VDat[i]->m_SysPara.m_iPCIdx = GetPrivateProfileInt("SYSTEM_PARAMETER", key, 0, path);
			*/

			for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_bPara_Sys%d", i, j);
				m_VDat[i]->m_SysPara.m_bPara[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, path);
			}

			for(j = 0; j < INT_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_iPara_Sys%d", i, j);
				m_VDat[i]->m_SysPara.m_iPara[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, path);
			}

			for(j = 0; j < DBL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_dPara_Sys%d", i, j);
				GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), path);
				m_VDat[i]->m_SysPara.m_dPara[j] = atof(temp);
			}
		}

		m_dPixelSizeData = m_VDat[0]->m_SysPara.m_dPara[0]; 

		key.Empty();
		key.Format("User_Password");
		GetPrivateProfileString("USER_PASSWORD", key, "1234", temp, sizeof(temp), path);
		//m_VDat[0]->m_SysPara.m_strPara = temp;
		sprintf(m_VDat[0]->m_SysPara.m_strPara , "%s", temp);
	}
	fd=0;

	return rslt;
}

BOOL  C_VisionModule::SaveSystemPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;

	//Make director
	MakeDirectories(path);
	path = path + "\\System.par";
	//file 존재 확인 
	HANDLE fd=CreateFile(   path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//Write INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			/*
			key.Empty();
			key.Format("PC_INDEX_CH%d", i);
			dat.Empty();
			dat.Format("%d", m_VDat[i]->m_SysPara.m_iPCIdx);
			WritePrivateProfileString("SYSTEM_PARAMETER", key, dat, path);
			*/

			for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_bPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%d", m_VDat[i]->m_SysPara.m_bPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}

			for(j = 0; j < INT_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_iPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%d", m_VDat[i]->m_SysPara.m_iPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}

			for(j = 0; j < DBL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_dPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%f", m_VDat[i]->m_SysPara.m_dPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}
		}

		key.Empty();
		key.Format("User_Password");
		dat.Empty();
		dat.Format("%s", m_VDat[0]->m_SysPara.m_strPara);
		WritePrivateProfileString("USER_PASSWORD", key, dat, path);

	}
	fd=0;

	return rslt;
}

//Load/save system result
BOOL C_VisionModule::LoadSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString key;

	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd = CreateFile( path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			key.Format("CNT%d", i);
			m_MainSysRslt.m_lCnt[i] = GetPrivateProfileInt("MAINSYSTEM_RESULT", key, 0, path);
		}
	}
	fd=0;

	return rslt;
}

BOOL C_VisionModule::SaveSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString dat, key;

	//Make director
	MakeDirectories(path);
	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd=CreateFile(   path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			dat.Empty();
			dat.Format("%d", m_MainSysRslt.m_lCnt[i]);
			key.Format("CNT%d", i);
			WritePrivateProfileString("MAINSYSTEM_RESULT", key, dat, path);
		}

	}
	fd=0;

	return rslt;
}

//Inspection
int C_VisionModule::Inspection(int cam_idx, int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;
	int i;
	int pf_pitch = 0;

	CRect rect, rect_chip, rect_dont_care;
	BOOL punch_err;
	Hlong area;

	//Variable clear
	ResetInspectRslt(cam_idx);
	ResetInspectRsltRgn(cam_idx);
	
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화

	BOOL lead_scat_hori = TRUE, find_chip_use = TRUE;	//수평방향 스캔(IC가로 긴방향기준)

	//Check punching chip
	//Punching chip
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if( VISION_CAM_TOP1 == GetVisIndex() )	// 상부 1번 카메라 경우만 펀칭칩 수행
		{
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;
			
			//chip 중심 기준으로 편차를 찾기 위해 검사 위치 넘겨줌. 
			rect_chip.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			rect_chip.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			rect_chip.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			rect_chip.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;

			rect_dont_care.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].left;
			rect_dont_care.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].top;
			rect_dont_care.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].right;
			rect_dont_care.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].bottom;
			
			pf_pitch = (int)((m_VDat[cam_idx]->m_InsPara.m_dParaBase[1] * 1000.) / m_VDat[cam_idx]->m_SysPara.m_dPara[0]);

			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;

			//Punching chip
			if( FindPuchingChip(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[4]),           //펀칭 막힘  
									m_VDat[cam_idx]->m_HalInsRgn[i][0],																//SR region
									rect,
									rect_chip,
									rect_dont_care,
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[19],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[9],
									pf_pitch,
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[13],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[14],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[12],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[15],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[20],    
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[10],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[11], 
									m_VDat[cam_idx]->m_InsPara.m_iParaBase[2],     
									&punch_err, &m_VDat[cam_idx]->m_HalRsltRgn[i][19], &area))
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}

			//Punching chip이라고 할지라도 PF막히면 불량(편집불량)
			if(m_VDat[cam_idx]->m_SysPara.m_bPara[12])
			{
				if(punch_err)
				{
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					//편집 불량 ...Punch가 있을 수도 있고 없을 수도 있다. 
					//단 Punch가 안보이더라도 PF가 막히므로 여기서 처리 가능함. 예외 가능성 있음.
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 3;  
					gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][19], rect.top, rect.left, rect.bottom, rect.right);//20100413 lee
				}
			}
		}
		else
		{
			//20141120 lby
			if(TRUE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) //검사대상이 펀칭칩이라면 이하 모든 검사 스킵
			{	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}
		}
	}

	//Align 펀치칩 유무 상관없이 수행
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2] ||	// Chip이면수지/BROKEN
	  m_VDat[cam_idx]->m_SysPara.m_bPara[3] ||	// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량
	  m_VDat[cam_idx]->m_SysPara.m_bPara[10] ||	// FILM 수지/잉크 묻음/긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[11] || // FILM PF 찢김
	  m_VDat[cam_idx]->m_SysPara.m_bPara[12] ||	// PF 막힘/BURR(편집불량)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[13] ||	// SR면 금속성 이물
	  m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||	// PF 막힘/BURR(편집불량)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[16] ||	// SR면 금속성 이물
	  m_VDat[cam_idx]->m_SysPara.m_bPara[20] || // Our lead 긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[21] ||	// Out lead 변색
	  m_VDat[cam_idx]->m_SysPara.m_bPara[22] ||	// In 긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[23] ||	// In 변색
	  m_VDat[cam_idx]->m_SysPara.m_bPara[24] ||	// OUT LEAD2
	  m_VDat[cam_idx]->m_SysPara.m_bPara[25] )	// 인쇄불량
	{
		rslt = InspectionAlign(cam_idx, buf_idx);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}
	
	//Chip
	if((m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2] ||	// Chip이면수지/BROKEN
	  m_VDat[cam_idx]->m_SysPara.m_bPara[3] ))	// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량)
	{
		rslt = InspectionChip(cam_idx, buf_idx, find_chip_use, m_VDat[cam_idx]->m_SysPara.m_bPara[0], m_VDat[cam_idx]->m_SysPara.m_bPara[2], m_VDat[cam_idx]->m_SysPara.m_bPara[3]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//OCV
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[25])
	{
		rslt = InspectionOCV(cam_idx, buf_idx, m_VDat[cam_idx]->m_SysPara.m_bPara[25]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Film
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[10] ||	// FILM 수지/잉크 묻음/긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[11] ||	// FILM PF 막힘/FILM 찢김
	  m_VDat[cam_idx]->m_SysPara.m_bPara[13] )	// FILM SR면 금속성 이물
	{
		rslt = InspectionFilm(cam_idx, buf_idx, m_VDat[cam_idx]->m_SysPara.m_bPara[10], m_VDat[cam_idx]->m_SysPara.m_bPara[11], m_VDat[cam_idx]->m_SysPara.m_bPara[13]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Lead
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||   //AM0
	   m_VDat[cam_idx]->m_SysPara.m_bPara[16] ||   //AM1
	   m_VDat[cam_idx]->m_SysPara.m_bPara[20] ||   //Out긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[21] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[22] ||   //In긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[23] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[24])     //Out2
	{
		rslt = InspectionLead(cam_idx, buf_idx, 
			                  m_VDat[cam_idx]->m_SysPara.m_bPara[20], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[21], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[22], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[23], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[24],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[15],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[16], lead_scat_hori);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	return tot_rslt;
}

//20141116 lby BOTTOM 추가
int C_VisionModule::Inspection_Bottom(int cam_idx, int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;
	CRect rect,rect_chip;
	HTuple start_t, end_t;

	//Variable clear
	ResetInspectRslt(cam_idx);
	ResetInspectRsltRgn(cam_idx);

	//Align
		if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
		  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
		  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		//count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////
		rslt = InspectionAlign_Bottom(cam_idx, buf_idx);
		//count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
		//sprintf(temp,"[Align:%f]", (double)(HTuple(end_t - start_t)[0]) );
		//TRACE(temp);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
		  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
		  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		//count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////
		rslt = InspectionSR_Bottom(cam_idx, buf_idx);
		//count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
		//sprintf(temp,"[SRBub:%f]", (double)(HTuple(end_t - start_t)[0]) );
		//TRACE(temp);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	return tot_rslt;
}

//20141116 lby BOTTOM 추가
int C_VisionModule::InspectionAlign_Bottom(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
	BOOL bAlignCheck = TRUE;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;
	
	rn = 0;
	
	get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		bAlignCheck = TRUE;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 0;

		if(FALSE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i])
		{
			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);

			m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
			
			for(j = 0; j < PATTERN_INS_NUM; j++)
			{
				if(1 == j)
				{
					continue;
				}

				ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0];
				gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);

				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);

				if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
				{
					//Inspection fail
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

					continue;
				}
				 
				reduce_domain(*GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);

				emphasize(ImageReduced, &EmphaImage, 5, 5, 1);	//20191203_2 by 5, 5, 3 -> 5, 5, 1 변경
			
				Hal_ShapePatternFind(0, 
					&EmphaImage,  //20100715 lee
					m_VDat[cam_idx]->m_InsPara.m_PatIDAlign, 
					m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
					wd, ht,
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
				if(rn)
				{
					gen_region_line(&line, (long)ry, (long)rx - 10, (long)ry, (long)rx + 10);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					gen_region_line(&line, (long)ry - 10, (long)rx, (long)ry + 10, (long)rx);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
					
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
					m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
					m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
					
					//Pos. limit check
					offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
					offset_x = fabs(offset_x);
					if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) )
					{
						//Limit error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
						rslt = 0;
						bAlignCheck = FALSE;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
				else
				{
					//Can't find
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					bAlignCheck = FALSE;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//둘다 성공했으면
			if(TRUE == bAlignCheck)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
				//첫번째 가지고 x, y offset을 결정한다.
				offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
				offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
			}
		}
		else
		{
			if(TRUE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) //검사대상이 펀칭칩이라면 이하 모든 검사 스킵
			{	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}
		}
	}

	return rslt;	
}


//20141116 lby BOTTOM 추가
int C_VisionModule::InspectionSR_Bottom(int cam_idx, int buf_idx)
{
	int rslt = 1;
	
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hobject RefRedImage, RefGreenImage, RefBlueImage;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, film_rect, chip_search_rect;
	
	rn = 0;
	
	get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), NULL, NULL, &wd, &ht);

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] )
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = 0;

			//Find chip//////////////////////////////
			
			//Offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][10], rect.top, rect.left, rect.bottom, rect.right);

			if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] || //기포/잉크
				m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||//SR검사/찍힘 (빽라이트)
				m_VDat[cam_idx]->m_SysPara.m_bPara[29] )//SR검사/찍힘
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = -61;


				rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].left;
				rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].top;
				rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].right;
				rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].bottom;

				film_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
				film_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
				film_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
				film_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;

				//20100716 lee
				chip_search_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
				chip_search_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
				chip_search_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
				chip_search_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;

				decompose3(*GetHalTaughtImage(0,1), &RefRedImage, &RefGreenImage, &RefBlueImage);

				//0 : sr factor, 1 : sr tol., 2 : sr trsh, 3 : sr cnt., 4 : sr max tol, 5 : pi hall tol, 6 : pi hall thresh
				if( SRBub(0, GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[0]), 
							 GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[1]), 
							 GetHalGreenImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[0]),
							 &RefGreenImage, &RefBlueImage, 
							 //20150130 by InkChip Green으로 변경.
							 //GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[3]),
							 GetHalGreenImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[3]),
					rect, 
					film_rect,
					chip_search_rect,
					m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[2], m_VDat[cam_idx]->m_InsPara.m_iParaSR[2] * 0.4, 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[0], m_VDat[cam_idx]->m_InsPara.m_iParaSR[1], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[4], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[3], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[6], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[5], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[8], 
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[7], m_VDat[cam_idx]->m_InsPara.m_iParaSR[9], m_VDat[cam_idx]->m_InsPara.m_iParaSR[10],
					m_VDat[cam_idx]->m_InsPara.m_iParaSR[11],m_VDat[cam_idx]->m_InsPara.m_iParaSR[12],
					&m_VDat[cam_idx]->m_InsRslt.m_iSRBubNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][51],
					&m_VDat[cam_idx]->m_HalRsltRgn[i][50],
					&m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][53],
					&m_VDat[cam_idx]->m_HalRsltRgn[i][52]) )
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = 1;
				}
				else
				{
					//SR bubble
					if( (m_VDat[cam_idx]->m_SysPara.m_bPara[28]) && (0 < m_VDat[cam_idx]->m_InsRslt.m_iSRBubNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = -61;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iSRBubNum[i] = 0;
						//gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[i][51]);
					}

					//PI 찍힘
					if( (m_VDat[cam_idx]->m_SysPara.m_bPara[29]) && (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = -71;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i] = 0;
						//gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[i][53]);
					}

					if( (0 < m_VDat[cam_idx]->m_InsRslt.m_iSRBubNum[i]) || (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i]) )
					{
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						//OK
						m_VDat[cam_idx]->m_InsRslt.m_iRsltSRBub[i] = 1;
					}
				}

			}
			else
			{
				//OK
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 1;
			}
		}
	}
	
	return rslt;
}


int C_VisionModule::InspectionAlign_Linescan(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
	BOOL bAlignCheck = TRUE;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;

	offset_x = 0.0;
	offset_y = 0.0;
	pat_score = 0.0;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_27!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화
	}


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		bAlignCheck = TRUE;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 0;

		if(FALSE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i])
		{
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
			
			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
			
			for(j = 0; j < PATTERN_INS_NUM; j++)
			{
				ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
				gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);
				
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);
				
				if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
				{
					//Inspection fail
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					
					continue;
				}
				//20100715 lee
				reduce_domain(*GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);
				
				emphasize(ImageReduced, &EmphaImage, 5, 5, 1);	//20191203_2 by 5, 5, 3 -> 5, 5, 1 변경
				
				Hal_ShapePatternFind(0, 
					&EmphaImage,  //20100715 lee
					(j==0?m_VDat[cam_idx]->m_InsPara.m_PatIDAlign:m_VDat[cam_idx]->m_InsPara.m_PatIDAlign2), 
					m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
					//					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][(j==0?1:3)],//model 영역(고정)
					wd, ht,
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
				// 20110530 Dongwhe* END
				if(rn)
				{
					gen_region_line(&line, (Hlong)ry, (Hlong)rx - 10, (Hlong)ry, (Hlong)rx + 10);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					gen_region_line(&line, (Hlong)ry - 10, (Hlong)rx, (Hlong)ry + 10, (Hlong)rx);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
					
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
					m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
					m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
					
					//Pos. limit check
					offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
					offset_x = fabs(offset_x);
					offset_x *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					offset_y = ry - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][j];
					offset_y = fabs(offset_y);
					offset_y *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) || (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_y) )
					{
						//Limit error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
						rslt = 0;
						bAlignCheck = FALSE;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
				else
				{
					//Can't find
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					bAlignCheck = FALSE;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			//둘다 성공했으면
			if(1 == bAlignCheck)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;

				//첫번째 가지고 x, y offset을 결정한다.
				offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
				offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
			}
		}
		else
		{
			if(TRUE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) //검사대상이 펀칭칩이라면 이하 모든 검사 스킵
			{	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}
		}
	}

	return rslt;
}

int C_VisionModule::InspectionChip_Linescan(int cam_idx, int buf_idx, BOOL find_chip_use, BOOL chip_out_use, BOOL chip_mold_use, BOOL chip_pot_use)
{
	int rslt = 1;
	
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hobject RefRedImage, RefGreenImage, RefBlueImage;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, film_rect, chip_search_rect;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_29!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]&&		/*얼라인 성공 시*/ 
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i])&&	/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]))		/*편집불량이 아닐 때*/
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;

			//Offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][10], rect.top, rect.left, rect.bottom, rect.right);

			//Find chip
			if(find_chip_use)
			{
				if(FindChip(0, GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]),  
												 rect, 
												 &m_VDat[cam_idx]->m_HalRsltRgn[i][11]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 1;
				}
				else
				{
					//No chip
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = -20;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			//Chipout////////////////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 0;

			if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]&&		/*얼라인 성공 시*/ 
			  (1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i])&&	/*정상 칩 일때*/ 
				chip_out_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;

				if(ChipOut(0, GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]),
								m_VDat[cam_idx]->m_HalRsltRgn[i][11], //Chip region현재 찾은 영역
								m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[1], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[0],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[30],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[2], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[18], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[17],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[31],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[32], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[33],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[34],
								&m_VDat[cam_idx]->m_InsRslt.m_iChipOutNum[i], 
								&m_VDat[cam_idx]->m_HalRsltRgn[i][12]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 1;
				}
				else
				{
					//Chipout
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//Chip 이면 결함/////////////////////
			//Broken & Mold//////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 0;

			if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] &&		/*얼라인 성공 시*/ 
			  (1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*정상 칩 일때*/ 
				chip_mold_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;

				if( ChipMold(0, GetHalGreenImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), 
								GetHalGreenImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]),
															m_VDat[cam_idx]->m_HalInsRgn[i][7],
															m_VDat[cam_idx]->m_HalRsltRgn[i][11],
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[5], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[3], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[14],
															m_VDat[cam_idx]->m_InsPara.m_dParaChip[0],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][14],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][13])
 															)
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
				}
				else
				{
					//Broken
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
					}

					//Mold
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					}
				}
			}
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionAlignMark(int cam_idx, int buf_idx, BOOL am_use0, BOOL am_use1)
{
	int rslt = 1;
		
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect trect, brect, lrect, rrect, SrRect;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_31!");
		WriteErrorList((LPCTSTR)status);
		return 0; 
	}



	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = 0;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = 0;

		if( (1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
		  (1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) )
		{
			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			if(am_use0 || am_use1)
			{
				int rsltam = 0;

				m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;

				rsltam = AMLead(0,  GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[0]),
										   am_use0,
										   am_use1,
										   m_VDat[cam_idx]->m_HalInsRgn[i][8],
										   m_VDat[cam_idx]->m_HalInsRgn[i][9],
										   m_VDat[cam_idx]->m_HalInsRgn[i][10],
										   m_VDat[cam_idx]->m_HalInsRgn[i][11],
										   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
										   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
										   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
										   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[9], m_VDat[cam_idx]->m_InsPara.m_iParaLead[8],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[10], m_VDat[cam_idx]->m_InsPara.m_iParaLead[11],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[13], m_VDat[cam_idx]->m_InsPara.m_iParaLead[12],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[14], m_VDat[cam_idx]->m_InsPara.m_iParaLead[15], 
											m_VDat[cam_idx]->m_InsPara.m_iParaLead[17], m_VDat[cam_idx]->m_InsPara.m_iParaLead[16],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[18], m_VDat[cam_idx]->m_InsPara.m_iParaLead[19],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[21], m_VDat[cam_idx]->m_InsPara.m_iParaLead[20],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[22], m_VDat[cam_idx]->m_InsPara.m_iParaLead[23], 
										   &m_VDat[cam_idx]->m_InsRslt.m_iLeadAMNum[i], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][60], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][61], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][62], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][63],
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][64]);

				if(1 ==  rsltam)
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = 1;
				}
				else if(-1 == rsltam)		//AMSC
				{
					//Bottom error
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
				else if(-2 == rsltam)		//AMDI
				{
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -43;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}

			}
		}
	}
	
	return rslt;
}

int C_VisionModule::Inspection_Linescan(int cam_idx, int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;

	CRect rect,rect_chip;
	HTuple start_t, end_t;
	BOOL find_chip_use = TRUE;

	//HTuple StartSecond, EndSecond;
	//long ProcessTime;

	//Variable clear
	ResetInspectRslt(cam_idx);
	ResetInspectRsltRgn(cam_idx);

	//Align
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2] ||		// Chip이면수지/BROKEN
	 // m_VDat[cam_idx]->m_SysPara.m_bPara[3] ||		// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량
	  m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||		 //AM0
	  m_VDat[cam_idx]->m_SysPara.m_bPara[16])		//AM1
	{
		//count_seconds(&StartSecond);

		rslt = InspectionAlign_Linescan(cam_idx, buf_idx);

		//count_seconds(&EndSecond);
		//EndSecond = EndSecond - StartSecond;
		//ProcessTime = (long)(((double)EndSecond[0]) * 1000);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Chip
	if((m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2]	||		// Chip이면수지/BROKEN
	  m_VDat[cam_idx]->m_SysPara.m_bPara[3] ))	// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량)
	{
		//count_seconds(&StartSecond);

		rslt = InspectionChip_Linescan(cam_idx, buf_idx, find_chip_use, m_VDat[cam_idx]->m_SysPara.m_bPara[0], m_VDat[cam_idx]->m_SysPara.m_bPara[2], m_VDat[cam_idx]->m_SysPara.m_bPara[3]);

		//count_seconds(&EndSecond);
		//EndSecond = EndSecond - StartSecond;
		//ProcessTime = (long)(((double)EndSecond[0]) * 1000);
	}
	/*
	else
	{
		rslt = 1;
	}
	*/

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Lead
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||   //AM0
	   m_VDat[cam_idx]->m_SysPara.m_bPara[16] )   //AM1
	{
		rslt = InspectionAlignMark(cam_idx, buf_idx, m_VDat[cam_idx]->m_SysPara.m_bPara[15], m_VDat[cam_idx]->m_SysPara.m_bPara[16]);
	}
	/*
	else
	{
		rslt = 1;
	}
	*/

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	return tot_rslt;
}

//20150527 by
BOOL C_VisionModule::InspectionPunchCheck(int cam_idx, int buf_idx)
{	
	BOOL rslt = FALSE;
	int i;
	CRect rect, rect_chip, rect_dont_care;
	BOOL punch_err;
	Hlong area;
	int pf_pitch;
	
	//Check punching chip
	//Punching chip
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//활자 자동 티칭 용으로 첫번째 칩만 체크한다.
		if(i >= 1)
			continue;

		rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
		rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
		rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
		rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;
		
		//chip 중심 기준으로 편차를 찾기 위해 검사 위치 넘겨줌. 
		rect_chip.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
		rect_chip.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
		rect_chip.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
		rect_chip.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;

		rect_dont_care.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].left;
		rect_dont_care.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].top;
		rect_dont_care.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].right;
		rect_dont_care.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].bottom;
		
		pf_pitch = (int)((m_VDat[cam_idx]->m_InsPara.m_dParaBase[1] * 1000.) / m_VDat[cam_idx]->m_SysPara.m_dPara[0]);

		//Punching chip
		if( FindPuchingChip(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[4]),           //펀칭 막힘  
								m_VDat[cam_idx]->m_HalInsRgn[i][0],																//SR region
								rect,
								rect_chip,
								rect_dont_care,
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[19],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[9],
								pf_pitch,
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[13],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[14],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[12],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[15],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[20],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[10],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[11], 
								m_VDat[cam_idx]->m_InsPara.m_iParaBase[2],     
								&punch_err, &m_VDat[cam_idx]->m_HalRsltRgn[i][19], &area))
		{
			rslt = TRUE;
		}
	}

	return rslt;
}

int C_VisionModule::InspectionFirstAlign(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;

	offset_x = 0.0;
	offset_y = 0.0;
	pat_score = 0.0;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_27!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
		rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
		rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
		rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
		
		
		pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
		
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
		
		for(j = 0; j < PATTERN_INS_NUM; j++)
		{
		
			//대칭으로 만들어 2번째 align 검사
			// 20110530 Dongwhe* BEGIN
			// 반대쪽 Align Mark도 따로 지정해주도록 변경
			// -> 대칭으로 만들어내지 않음
			
			ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
			gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);
			
			concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);
			
			if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
			{
				//Inspection fail
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
				m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
				rslt = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				
				continue;
			}
			//20100715 lee
			reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);
			emphasize(ImageReduced, &EmphaImage, 5, 5, 3); //20100715 lee
			
			//reduce_domain(EmphaImage,InsRgn,&reudce_image);
			
			Hal_ShapePatternFind(0, 
				&EmphaImage,  //20100715 lee
				(j==0?m_VDat[cam_idx]->m_InsPara.m_PatIDAlign:m_VDat[cam_idx]->m_InsPara.m_PatIDAlign2), 
				m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
				//					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
				m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][(j==0?1:3)],//model 영역(고정)
				wd, ht,
				//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
				//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
				m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
				m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
				pat_score,//Score
				&modimage, 
				&shape, 
				&rx, 
				&ry, 
				&ra, 
				&rs, 
				&rscore, 
				&rn);
			// 20110530 Dongwhe* END
			if(rn)
			{
				gen_region_line(&line, (Hlong)ry, (Hlong)rx - 10, (Hlong)ry, (Hlong)rx + 10);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				gen_region_line(&line, (Hlong)ry - 10, (Hlong)rx, (Hlong)ry + 10, (Hlong)rx);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
				
				m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
				m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
				m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
				
				//Pos. limit check
				offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
				offset_x = fabs(offset_x);
				if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) )
				{
					//Limit error
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
			else
			{
				//Can't find
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
				m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
				rslt = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
			}
		}
		
		//둘다 성공했으면
		if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i])
		{
			//첫번째 가지고 x, y offset을 결정한다.
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
			m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
		}
	}
	
	return rslt;
}


int C_VisionModule::InspectionAlign(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
	BOOL bAlignCheck = TRUE;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;

	offset_x = 0.0;
	offset_y = 0.0;
	pat_score = 0.0;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_27!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		bAlignCheck = TRUE;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 0;
		
		//20150617 얼라인 진입조건 추가. 정상칩일 경우. by
		if( (FALSE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) &&	/*펀치 스킵이 아닐 때*/ 
			(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*펀칭 칩이 아닐 때*/ 
			(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) )		/*펀칭 불량이 아닐 때*/
			
		{
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
			
			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
			
			for(j = 0; j < PATTERN_INS_NUM; j++)
			{
				ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
				gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);
				
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);
				
				if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
				{
					//Inspection fail
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					
					continue;
				}
				//20100715 lee
				reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);

				emphasize(ImageReduced, &EmphaImage, 5, 5, 1);	//20191203_2 by 5, 5, 3 -> 5, 5, 1 변경
				
				Hal_ShapePatternFind(0, 
					&EmphaImage,  //20100715 lee
					(j==0?m_VDat[cam_idx]->m_InsPara.m_PatIDAlign:m_VDat[cam_idx]->m_InsPara.m_PatIDAlign2), 
					m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
					//					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][(j==0?1:3)],//model 영역(고정)
					wd, ht,
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
				// 20110530 Dongwhe* END
				if(rn)
				{
					gen_region_line(&line, (Hlong)ry, (Hlong)rx - 10, (Hlong)ry, (Hlong)rx + 10);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					gen_region_line(&line, (Hlong)ry - 10, (Hlong)rx, (Hlong)ry + 10, (Hlong)rx);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
					
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
					m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
					m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
					
					//Pos. limit check
					offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
					offset_x = fabs(offset_x);
					offset_x *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					offset_y = ry - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][j];
					offset_y = fabs(offset_y);
					offset_y *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) || (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_y) )
					{
						//Limit error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
						rslt = 0;
						bAlignCheck = FALSE;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
				else
				{
					//Can't find
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					bAlignCheck = FALSE;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			//둘다 성공했으면
			if(1 == bAlignCheck)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;

				//첫번째 가지고 x, y offset을 결정한다.
				offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
				offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
			}
		}
	}

	return rslt;	
}

int C_VisionModule::InspectionAlignForAutoOCVTeach(int cam_idx, int buf_idx, Hobject *mod_image, Hobject *shape, 
												   double *rx, double *ry, double *rscore, Hlong *rn,
												   double *offset_x, double *offset_y)
{
	int rslt = 1;

	CRect rect,rect_chip;
		
	Hobject reudce_image, InsRgn;  //20100715 lee
	Hobject line;

	Hlong wd, ht;
	double ra, rs;   
	int i;
	double pat_score;

	//BOOL punch_rslt;

	Hobject EmphaImage;
	
	*rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_28!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}
	
	for(i = 0; i < 1; i++)//첫번째것만
	{
			gen_rectangle1(&InsRgn, m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].top, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].left, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].bottom, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].right);
		
		if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
		{
			//Inspection fail
			rslt = 0;
		}

		reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&reudce_image); //20100715 lee
		emphasize(reudce_image, &EmphaImage, 5, 5, 3); //20100715 lee

		pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
		
		Hal_ShapePatternFind(0, 
			&EmphaImage,  //20100715 lee
			m_VDat[cam_idx]->m_InsPara.m_PatIDAlign, 
			m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
			wd, ht,
			m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
			m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
			pat_score,//Score
			mod_image, 
			shape, 
			rx, 
			ry, 
			&ra, 
			&rs, 
			rscore, 
			rn);
		if(*rn)
		{
			rslt = 1;
			
			*offset_x = *rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			*offset_y = *ry - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];

			//Check punching chip
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left + *offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top + *offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + *offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + *offset_y; //20100715 lee

			//chip 중심에서 편차를 구하기 위해 ..
			rect_chip.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + *offset_x;
			rect_chip.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + *offset_y;
			rect_chip.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + *offset_x;
			rect_chip.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + *offset_y;
		}
		else
		{
			rslt = 0;
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionChip(int cam_idx, int buf_idx, BOOL find_chip_use, BOOL chip_out_use, BOOL chip_mold_use, BOOL chip_pot_use)
{
	int rslt = 1;
	
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hobject RefRedImage, RefGreenImage, RefBlueImage;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, film_rect, chip_search_rect;

	Hlong over_area, under_area;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_29!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]&&		/*얼라인 성공 시*/ 
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i])&&	/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]))		/*편집불량이 아닐 때*/
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;

			//Offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][10], rect.top, rect.left, rect.bottom, rect.right);

			//20100111LSS
			//Find chip
			if(find_chip_use)
			{
				if(FindChip(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]),  
												 rect, 
												 &m_VDat[cam_idx]->m_HalRsltRgn[i][11]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 1;
				}
				else
				{
					//No chip
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = -20;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			/*
			m_map0 = 1;//Chip인식/포팅
			m_map1 = 2;//Chipout(IC표면 밝게)
			m_map2 = 3;//Broken/(IC표면의 어두운것이 잘보이게)
			m_map3 = 1;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
			m_map4 = 1;//Potting
			m_map5 = 1;//Potting위 밝은이물/도포불량
			m_map6 = 0;//Potting위 어두운이물
			*/
		
			//Chipout////////////////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 0;

			if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] &&	//칩을 찾았으면..	//20141211 상부#2에서는 수행하지 않음
				chip_out_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;

				//0 : out tol., 1 : out threshold, 2 : out high threshold
				//20110530 lee green에서 Chip Out이 가장 밝음. 
				if(ChipOut(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[1]),  
								m_VDat[cam_idx]->m_HalRsltRgn[i][11], 
								m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[1], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[0],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[30],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[2], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[18], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[17],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[31],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[32], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[33],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[34],
								&m_VDat[cam_idx]->m_InsRslt.m_iChipOutNum[i], 
								&m_VDat[cam_idx]->m_HalRsltRgn[i][12]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 1;
				}
				else
				{
					//Chipout
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//Chip 이면 결함/////////////////////
			//Broken & Mold//////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 0;

			if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] &&				 //칩을 찾았으면..
							chip_mold_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;

				//3 : mold tol., 4 : mold threshold, 5 : mold high threshold, 14 : broken threshold

				/*
				m_map0 = 1;//Chip인식/포팅
				m_map1 = 2;//Chipout(IC표면 밝게)
				m_map2 = 3;//Broken/(IC표면의 어두운것이 잘보이게)
				m_map3 = 1;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
				m_map4 = 1;//Potting
				m_map5 = 1;//Potting위 밝은이물/도포불량
				m_map6 = 0;//Potting위 어두운이물
				*/

				if( ChipMold(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[3]), 
								GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[2]),
															m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
															m_VDat[cam_idx]->m_HalRsltRgn[i][11],
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[5], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[3], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[14],
															m_VDat[cam_idx]->m_InsPara.m_dParaChip[0],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][14],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][13])
 															)
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
				}
				else
				{
					//Broken
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
					}

					//Mold
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					}
				}
			}

			//Pot////////////////////////////////////	
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 0;//
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 0;
			
			if(	1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] && //칩을 찾았으면..
							chip_pot_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = -24;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = -25;

				over_area = m_VDat[cam_idx]->m_InsPara.m_iParaChip[12] * (m_VDat[cam_idx]->m_InsPara.m_iParaChip[6] * 0.01);
				under_area = m_VDat[cam_idx]->m_InsPara.m_iParaChip[12] * (m_VDat[cam_idx]->m_InsPara.m_iParaChip[8] * 0.01);

				//6 : over pot tol., 7 : pot threshold, 8 : no pot tol. 12 : pot area
				if(ChipPot(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[4]), 
															rect, 															m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[12],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[7], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[8], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[6], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[21], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[22], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[23], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[24], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[25], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[26], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[27],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[28],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[29],
															&m_VDat[cam_idx]->m_HalPotRsltRgn[i][0],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipPotNum[i],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i],
															&m_VDat[cam_idx]->m_HalRsltRgn[i][15],
															&m_VDat[cam_idx]->m_nPotRsltAverageLength[i][0], 
															&m_VDat[cam_idx]->m_nPotRsltCenterLength[i][0]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 1;
				}
				else
				{
					if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
					{
						//NO POTTING
						if(under_area > m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i])
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = -24;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 1;
						}
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 0;
					}
					
					if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
					{
						//DOUBLE POTTING
						if(over_area < m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i])
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = -25;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 1;
						}
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 0;
					}
				}
			}

			//Chip 이면 결함/////////////////////
			//Pot particle///////////////////////
			
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 0;
			
			if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] && // 칩을 찾았으면..
							chip_pot_use)								
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = -26;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = -27;

				//9 : pot part tol., 10 : pot part threshold, 11 : pot part high threshold
				//13 : pot 도포범위 tol.
				
				if(PotPart(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[7]), 
					GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[6]),
					GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[7]),
					m_VDat[cam_idx]->m_HalRsltRgn[i][11],//chip region 
					m_VDat[cam_idx]->m_HalRsltRgn[i][15],//pot region
					m_VDat[cam_idx]->m_HalInsRgn[i][0],//SR region
					
					m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
					m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
					
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[10], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[15],
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[11], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[9], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[13], 
					m_VDat[cam_idx]->m_InsPara.m_iParaFilm[1],//Film 수지 tol.
					m_VDat[cam_idx]->m_InsPara.m_iParaFilm[2],//Film 수지 thresh.
					&m_VDat[cam_idx]->m_InsRslt.m_iChipPotPNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][16],
					&m_VDat[cam_idx]->m_InsRslt.m_iChipPotRNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][17],	
					&m_VDat[cam_idx]->m_HalRsltRgn[i][18]//인식기 영역
					))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 1;
				}
				else
				{
					//Particle
					if( m_VDat[cam_idx]->m_SysPara.m_bPara[3] && (0 < m_VDat[cam_idx]->m_InsRslt.m_iChipPotPNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = -26;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						 if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 1;
						 }
						 else
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 0;
						 }
						
					}

					//Pot range
					if( m_VDat[cam_idx]->m_SysPara.m_bPara[3] && (0 < m_VDat[cam_idx]->m_InsRslt.m_iChipPotRNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = -27;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 1;
						 }
						 else
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 0;
						 }
					}
				}
			}			
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionFilm(int cam_idx, int buf_idx, BOOL film_mold_use, BOOL film_pf_use, BOOL film_metal_use)
{
	int rslt = 1;
		
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, chip_rect, ocv_rect, chip_rect_insp, ocv_rect_insp;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_30!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&			//얼라인이 성공 했을 때
		   //(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*펀칭 칩이 아닐 때*/ 
		   //(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]))			/*편집불량이 아닐 때*/
		   1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i])
		{
			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//For display
			//Film-mold inspection region
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom + offset_y;
			
			//Chip inspection region
			chip_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			chip_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			chip_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			chip_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			//20100720 lee
			chip_rect_insp = chip_rect;

			//OCV inspection region
			ocv_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left + offset_x;
			ocv_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top + offset_y;
			ocv_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right + offset_x;
			ocv_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom + offset_y;

			//20100720 lee
			ocv_rect_insp = ocv_rect;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][20], rect.top, rect.left, rect.bottom, rect.right);
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][21], chip_rect.top, chip_rect.left, chip_rect.bottom, chip_rect.right);
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][22], ocv_rect.top, ocv_rect.left, ocv_rect.bottom, ocv_rect.right);		

			//For inspection
			//Film-mold inspection region
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;
			
			//Chip inspection region
			chip_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			chip_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			chip_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			chip_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;	

			 //20100720 lee  //영상 보정된 영역을 사용하기 위해 
			//OCV inspection region
			ocv_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left;
			ocv_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top;
			ocv_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right;
			ocv_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom;

			m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = 0;

			if(film_mold_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = -31;

				//0 : mold factor(mean_image), 1 : mold tol., 2 : mold thrsh.
				if(FilmMold(0,
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]),	//Mold image
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[2]),//Ink image
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[1]),//User image
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[3]),//SR-OUT Scratch image
						   rect, chip_rect, ocv_rect, m_VDat[cam_idx]->m_HalRsltRgn[i][41],//OCV chars region
						   m_VDat[cam_idx]->m_HalInsRgn[i][0],//SR region
						   m_VDat[cam_idx]->m_HalInsRgn[i][4],//user region
						   m_VDat[cam_idx]->m_HalInsRgn[i][5],//sr out region
								m_VDat[cam_idx]->m_InsPara.m_FilmVarID[i], 
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[22], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[2], 
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[23], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[21], 
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[0], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[1], 
								m_VDat[cam_idx]->m_InsPara.m_iParaFilm[6],
								&m_VDat[cam_idx]->m_InsRslt.m_iFilmMoldNum[i], 
								&m_VDat[cam_idx]->m_HalRsltRgn[i][23],
								&m_VDat[cam_idx]->m_HalRsltRgn[i][24],
								&m_VDat[cam_idx]->m_HalRsltRgn[i][27],
								&m_VDat[cam_idx]->m_HalRsltRgn[i][28],
								&m_VDat[cam_idx]->m_HalRsltRgn[i][29]) )
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = 1;
				}
				else
				{
					//Film mold
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = -31;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//PF hall////////////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = 0;
			/*
			if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
				(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&
				(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&
				film_pf_use)
			*/
			if(film_pf_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = -32;

				//3 : pf tol., 4 : pf thresh., 5 : pf high thresh.
				if( FilmPF(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[4]), 
						    GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[5]),
							rect, chip_rect, 
							m_VDat[cam_idx]->m_HalInsRgn[i][1],//PF region
							m_VDat[cam_idx]->m_HalInsRgn[i][6],//vaccum region

							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0],
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1],
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],							
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],

							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[4], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[3], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[5],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[19],  
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[9],  

							&m_VDat[cam_idx]->m_InsRslt.m_iFilmPFNum[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][25],   //막힘/찢어짐
							&m_VDat[cam_idx]->m_HalRsltRgn[i][26])  )//PF영역 
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = 1;
				}
				else
				{
					//Film mold
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = -32;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			if(film_metal_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = 0;

			}
		}		
	}
	
	return rslt;
}

int C_VisionModule::InspectionLead(int cam_idx, int buf_idx, 
								   BOOL out_lead_use, 
								   BOOL out_lead_dirt_use, 
								   BOOL in_lead_use,
								   BOOL in_lead_dirt_use, 
								   BOOL out_lead2_use,
								   BOOL am_use0, BOOL am_use1, BOOL lead_scat_hori)
{
	int rslt = 1;
		
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect trect, brect, lrect, rrect, SrRect;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_31!");
		WriteErrorList((LPCTSTR)status);
		return 0; 
	}



	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = 0;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = 0;

		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&//if not no chip and puching chip
			//(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) && 
			//(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&
			(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) )
		{
			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//Top Lead inspection region
			trect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].left + offset_x;
			trect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].top + offset_y;
			trect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].right + offset_x;
			trect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].bottom + offset_y;

			//Bottom Lead inspection region
			brect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].left + offset_x;
			brect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].top + offset_y;
			brect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].right + offset_x;
			brect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].bottom + offset_y;

			if(out_lead2_use)
			{
				//Top Left Lead inspection region
				lrect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].left + offset_x;
				lrect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].top + offset_y;
				lrect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].right + offset_x;
				lrect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].bottom + offset_y;

				//Top Right Lead inspection region
				rrect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].left + offset_x;
				rrect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].top + offset_y;
				rrect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].right + offset_x;
				rrect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].bottom + offset_y;
			}
			else
			{
				//Top Left Lead inspection region
				lrect.left = 0;
				lrect.top = 0;
				lrect.right = 0;
				lrect.bottom = 0;

				//Top Right Lead inspection region
				rrect.left = 0;
				rrect.top = 0;
				rrect.right = 0;
				rrect.bottom = 0;
			}
			
			//Top Lead///////////////////////////
			//double 0 : top factor, int 1 : top tol, 2 : top trsh, 6 : top-dirty tol, 7 : top-dirty trsh
			if(out_lead_use || out_lead_dirt_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = -40;

				if(10 < trect.top)
				{
					if(OutLead(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[0]),
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[1]), 
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[5]), 
						   trect, lrect, rrect,
						   m_VDat[cam_idx]->m_HalRsltRgn[i][27],//not sr black mold result
						   m_VDat[cam_idx]->m_HalInsRgn[i][2],//up lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][3],//dn lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][8],//up left lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][9],//up right lead region
						   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
						   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
						   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
						   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[2],
						   m_VDat[cam_idx]->m_InsPara.m_dParaLead[0], 
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[1],
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[7], 
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[6],
						   out_lead_use, out_lead_dirt_use, lead_scat_hori,
						   &m_VDat[cam_idx]->m_InsRslt.m_iLeadTopNum[i], 
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][32],
						   &m_VDat[cam_idx]->m_InsRslt.m_iLeadTopNum2[i], 
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][34],
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][30]) )
					{
						//OK
						m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = 1;
					}
					else
					{
						//Top error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = -40;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
			}

			//Bot Lead///////////////////////////
			if(in_lead_use || in_lead_dirt_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = -41;

				//20110626 lee
				//검사 영역이 SR 검사 영역 내부에 있지 않으면 검사 PASS한다.
				SrRect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
				SrRect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
				SrRect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
				SrRect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;

				if(10 < brect.top)
				{
					//20110626 lee
					//검사 영역이 SR 검사 영역 내부에 있지 않으면 검사 PASS한다.
					if(brect.left > SrRect.left && brect.right < SrRect.right)
					{	
						//double 1 : bot factor, int 4 : bot tol, 5 : bot trsh
						if(InLead(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[2]),
							GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[3]),
							brect,	
							m_VDat[cam_idx]->m_HalRsltRgn[i][27],
							m_VDat[cam_idx]->m_HalInsRgn[i][3],//dn lead region
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
							m_VDat[cam_idx]->m_InsPara.m_iParaLead[5],
							m_VDat[cam_idx]->m_InsPara.m_dParaLead[1], 
							m_VDat[cam_idx]->m_InsPara.m_iParaLead[4],
							&m_VDat[cam_idx]->m_InsRslt.m_dLeadBotWAVG[i], 
							&m_VDat[cam_idx]->m_InsRslt.m_iLeadBotNum[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][33],
							&m_VDat[cam_idx]->m_InsRslt.m_iLeadBotNum2[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][35],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][31]))
						{
							//OK
							m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = 1;
						}
						else
						{
							//Bottom error
							m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = -41;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
					}
				}
			}

			if(am_use0 || am_use1)
			{
				int rsltam = 0;

				m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;

				rsltam = AMLead(0,  GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[4]),
										   am_use0,
										   am_use1,
										   m_VDat[cam_idx]->m_HalInsRgn[i][8],
										   m_VDat[cam_idx]->m_HalInsRgn[i][9],
										   m_VDat[cam_idx]->m_HalInsRgn[i][10],
										   m_VDat[cam_idx]->m_HalInsRgn[i][11],
										   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
										   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
										   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
										   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[9], m_VDat[cam_idx]->m_InsPara.m_iParaLead[8],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[10], m_VDat[cam_idx]->m_InsPara.m_iParaLead[11],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[13], m_VDat[cam_idx]->m_InsPara.m_iParaLead[12],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[14], m_VDat[cam_idx]->m_InsPara.m_iParaLead[15], 
											m_VDat[cam_idx]->m_InsPara.m_iParaLead[17], m_VDat[cam_idx]->m_InsPara.m_iParaLead[16],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[18], m_VDat[cam_idx]->m_InsPara.m_iParaLead[19],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[21], m_VDat[cam_idx]->m_InsPara.m_iParaLead[20],
										   m_VDat[cam_idx]->m_InsPara.m_iParaLead[22], m_VDat[cam_idx]->m_InsPara.m_iParaLead[23], 
										   &m_VDat[cam_idx]->m_InsRslt.m_iLeadAMNum[i], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][60], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][61], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][62], 
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][63],
										   &m_VDat[cam_idx]->m_HalRsltRgn[i][64]);

				if(1 ==  rsltam)
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = 1;
				}
				else if(-1 == rsltam)		//AMSC
				{
					//Bottom error
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
				else if(-2 == rsltam)		//AMDI
				{
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -43;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}

			}
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionOCV(int cam_idx, int buf_idx, BOOL ocv_use)
{
	int rslt = 1;

	Hobject modimage, shape, Region, ReducedImage, RegionDiff, TransImage, CropImage;
	HTuple HomMat2D;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore, dx, dy, char_dx[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM], char_dy[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];   
	Hlong rn;
	int i, j;
	int idx;
	double offset_x, offset_y;
	CRect rect;
	HTuple Num;
	Hobject PatternRegion[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];

	double pat_score;

	BOOL good_chk, sub_good_chk;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_32!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}
	
	rn = 0;

	good_chk = TRUE;
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = 0;

		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*편집불량이 아닐 때*/
		  ocv_use)//if not no chip and puching chip
		{			
			m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -50;

			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaOCV[1]) * 0.01);

			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//For display
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom + offset_y;
			
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right); 
			
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][40], rect.top, rect.left, rect.bottom, rect.right);
			
			reduce_domain(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), Region, &ReducedImage);
						
			dx = 0;
			dy = 0;
			if(0 < m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2])
			{
				//Total//////////////////////////////
				idx = m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2];
				
				modimage.Reset();
				shape.Reset();
				good_chk = TRUE;
				
				//0 : pat threshold, 1 : pat score, 2 : Taught character num, 3 : var threshold, 4 : var tol
				if(m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDOCV[idx])
				{
					Hal_ShapePatternFind(0, 
						&ReducedImage, 
						m_VDat[cam_idx]->m_InsPara.m_PatIDOCV[idx], 
						m_VDat[cam_idx]->m_InsPara.m_PatParaOCV[idx], 
						m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[idx], 
						wd, ht,
						m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[idx],
						m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[idx],
						pat_score,//Score
						&m_VDat[cam_idx]->m_HalRsltRgn[i][41], 
						&shape, 
						&rx, 
						&ry, 
						&ra, 
						&rs, 
						&rscore, 
						&rn);
					if(rn)
					{
						//OK
						dx = rx - m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[idx];
						dy = ry - m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[idx];
					}
					else
					{
						rslt = 0;
						
						//Can't find total pattern
						m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -50;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

						good_chk = FALSE;
					}
				}
				else
				{
					rslt = 0;
					
					//No pattern
					m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -51;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

					good_chk = FALSE;
				}

				m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i] = 0;

				sub_good_chk = TRUE;
				for(j = 0; j < m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2]; j++)
				{
					modimage.Reset();
					shape.Reset();
				
					if(good_chk)
					{
						//Sub////////////////////////////////
						//세부영역조정
						gen_rectangle1(&Region, m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].top + dy, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].left + dx, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].bottom + dy, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].right + dx);
						reduce_domain(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), Region, &ReducedImage);
						

						if(m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDOCV[j])
						{
							modimage.Reset();
							shape.Reset();
							//0 : pat threshold, 1 : pat score, 2 : Taught character num, 3 : var threshold, 4 : var tol
							Hal_ShapePatternFind(0, 
								&ReducedImage, 
								m_VDat[cam_idx]->m_InsPara.m_PatIDOCV[j], 
								m_VDat[cam_idx]->m_InsPara.m_PatParaOCV[j], 
								m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j], 
								wd, ht,
								m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[j],
								m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[j],
								pat_score,//Score
								&modimage, 
								&shape, 
								&rx, 
								&ry, 
								&ra, 
								&rs, 
								&rscore, 
								&rn);
							if(rn)
							{
								char_dx[i][j] = rx - m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[j];
								char_dy[i][j] = ry - m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[j];
								
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][42], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][42]);
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][43], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][43]);

								copy_obj(shape, &PatternRegion[i][j], 1, -1);
							}
							else
							{
								char_dx[i][j] = 0;
								char_dy[i][j] = 0;
								
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][44], Region, &m_VDat[cam_idx]->m_HalRsltRgn[i][44]);
								m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i]++;
								rslt = 0;
						
								//No can't find char pattern
								m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -52;
								rslt = 0;
								m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
								m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

								sub_good_chk = FALSE;
							}
						}
						else
						{
							char_dx[i][j] = 0;
							char_dy[i][j] = 0;
							
							concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][44], Region, &m_VDat[cam_idx]->m_HalRsltRgn[i][44]);
							m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i]++;

							rslt = 0;
							
							//No char pattern
							m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -53;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

							sub_good_chk = FALSE;
						}

						if(sub_good_chk)
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = 1;
						}
					}
				}
			}
		}
	}
	
	return rslt;
}

void C_VisionModule::SetNGViewMode(BOOL mode, int ng_view_idx)
{
	if(mode)
	{
		m_iNGViewMode = ng_view_idx;
		if( (0 > m_iNGViewMode) && (NG_VIEW_MAX_NUM <= m_iNGViewMode) )
		{
			m_iNGViewMode = -1;
		}
	}
	else
	{
		m_iNGViewMode = -1;
	}
}


//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
BOOL C_VisionModule::_Hal_FilmMold(Hlong win, Hobject *mold_image, Hobject *ink_image, Hobject *user_image, Hobject *scratch_image,
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
							  Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr)
{
	Hobject Image, ImageLight, ImageTrans, RegionDiff, ImageReduced, ImageReducedFine, ImageReducedLight, ImageReducedLight2;
	Hobject	RegionRect, ChipRect, OCVRect;
	HTuple HomMat2D, Num;
	Hobject select_object;
	Hobject ins_rgn, ins_rgn_fine, OCVRegion, ins_rgn_srout;
	HTuple DiffAng;

	Hobject RsltSRRegion, RsltNotSRRegion, RsltUserRegion, RsltSROutRegion;

	gen_empty_obj(&RsltSRRegion);
	gen_empty_obj(&RsltNotSRRegion);
	gen_empty_obj(&RsltUserRegion);
	gen_empty_obj(&RsltSROutRegion);

	int i;

	Hobject OutRgn, IntRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean, IntMean;
	int diff_var;
	Hobject ComplRgn;
	
	Hobject Dark, DarkFine, DarkSROut, UserDark, Light, MaskImage, MaskRegion, MaskRegion1, MaskRegion2;
	Hobject ImageReducedNotSR, ImageNotSR, ImageReducedUser, ImageReducedSROut;

	double offset_x, offset_y;

	Hlong max, num;
	HTuple Max;

	Hobject MeanImage, ImageLight2, SROutImage;

	HTuple Area;

	HTuple CircleVar;

	HTuple LightMean;

	Hlong lightvar;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;
	ins_region.top = ins_region.top + offset_y;
	ins_region.left = ins_region.left + offset_x;	
	ins_region.bottom = ins_region.bottom + offset_y;
	ins_region.right = ins_region.right + offset_x;

	gen_rectangle1(&RegionRect, (Hlong)(ins_region.top), 
								(Hlong)(ins_region.left), 
								(Hlong)(ins_region.bottom), 
								(Hlong)(ins_region.right));

	gen_rectangle1(&ChipRect, (Hlong)(chip_region.top + offset_y), 
							  (Hlong)(chip_region.left + offset_x), 
							  (Hlong)(chip_region.bottom + offset_y), 
							  (Hlong)(chip_region.right + offset_x));

	gen_rectangle1(&OCVRect, (Hlong)(ocv_region.top + offset_y), 
							  (Hlong)(ocv_region.left + offset_x), 
							  (Hlong)(ocv_region.bottom + offset_y), 
							  (Hlong)(ocv_region.right + offset_x));

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(sr_region, &RsltSRRegion, HomMat2D, HTuple("false"));
	affine_trans_region(user_region, &RsltUserRegion, HomMat2D, HTuple("false"));
	affine_trans_region(sr_out_region, &RsltSROutRegion, HomMat2D, HTuple("false"));	

	//Dark image/////////////////////////////////
	//Mask image(테두리 둔감하게)
	gen_image_proto(*mold_image, &MaskImage, HTuple(0));
	for(i = 0 ; i < 10; i++)
	{
		if(0 == i)
		{
			erosion_circle(RsltSRRegion, &MaskRegion1, i + 1);
			difference(RsltSRRegion, MaskRegion1, &MaskRegion);
		}
		else if(i % 2)
		{
			erosion_circle(RsltSRRegion, &MaskRegion2, i + 1);
			difference(MaskRegion1, MaskRegion2, &MaskRegion);
		}
		else
		{
			erosion_circle(RsltSRRegion, &MaskRegion1, i + 1);
			difference(MaskRegion2, MaskRegion1, &MaskRegion);
		}
		paint_region(MaskRegion, MaskImage, &MaskImage, (10 - i) * 1 + 5, HTuple("fill"));
	}

	//SR/////////////////////////////////////////
	add_image(*mold_image, *mold_image, &Image, HTuple(0.6), HTuple(0));
	add_image(Image, MaskImage, &Image, HTuple(1.0), HTuple(0));
	mean_image(Image, &Image, HTuple(mold_fact), HTuple(mold_fact));

	//Not SR(for lead////////////////////////////
	add_image(*mold_image, *mold_image, &ImageNotSR, HTuple(0.7), HTuple(0));

	//Light image////////////////////////////////
	mean_image(*ink_image, &ImageLight, HTuple(mold_fact), HTuple(mold_fact));

	if(win)
	{
		//disp_obj(Image, win);
	}
	
	//SR 전영역 검사영역, 어두운 불량////////////
	erosion_circle(RsltSRRegion, &ins_rgn, 1.5);
	difference(ins_rgn, ChipRect, &ins_rgn);
	dilation_circle(ocv_chars_region, &OCVRegion, 7.5);
	difference(ins_rgn, OCVRegion, &ins_rgn);
	reduce_domain(Image, ins_rgn, &ImageReduced);

	//SR 정밀영역////////////////////////////////
	//SR 영역에서 테두리 제외 하고 검사(더 세밀히게)영역, 어두운 불량
	fill_up(ins_rgn, &ins_rgn_fine);
	erosion_circle(ins_rgn_fine, &ins_rgn_fine, 55.5);
	intersection(ins_rgn, ins_rgn_fine, &ins_rgn_fine);
	reduce_domain(Image, ins_rgn_fine, &ImageReducedFine);
	
	//Lead검사용 영역(여기서는 사용않는다 나중에 lead검사부에서 이영역을 사용한다.
	reduce_domain(ImageNotSR, RegionRect, &ImageReducedNotSR);//Lead 검사영역

	//밝은 불량//////////////////////////////////
	//ink image : map 3
	reduce_domain(ImageLight, ins_rgn, &ImageReducedLight);//긁힘같은 밝은불량  검사영역

	//밝은 불량2/////////////////////////////////
	//mold image : map 1
	reduce_domain(*mold_image, ins_rgn, &ImageReducedLight2);//밝은불량 검사영역

	//User area검사//////////////////////////////
	reduce_domain(*user_image, RsltUserRegion, &ImageReducedUser);//user area검사영역

	difference(RsltSROutRegion, ChipRect, &RsltSROutRegion);
	difference(RsltSROutRegion, OCVRegion, &ins_rgn_srout);

	reduce_domain(*scratch_image, RegionRect, &ImageReducedSROut);//SR out검사영역
	//reduce_domain(*mold_image, RegionRect, &ImageReducedSROut);//SR out검사영역

	emphasize(ImageReducedSROut, &ImageReducedSROut, 7, 7, 6);
	gray_erosion_rect(ImageReducedSROut, &ImageReducedSROut, 3.5, 3.5);
	reduce_domain(ImageReducedSROut, ins_rgn_srout, &ImageReducedSROut);//SR out검사영역


	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(ins_rgn, win);
		set_color(win, HTuple("blue"));
		disp_obj(RsltSROutRegion, win);
	}

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(RsltSRRegion, win);
	}

	gen_empty_obj(&ResultRgn);

	/////////////////////////////////////////////
	//Dark///////////////////////////////////////
	//
	//For SR/////////////////////////////////////
	min_max_gray(ins_rgn, Image, HTuple(5), _, &Max, _);
	max = (Hlong)Max[0];
	max = max - dark_thresh;
	if(0 > max)
	{
		max = 230;
	}
	if(255 < max)
	{
		max = 255;
	}

	//max = 240;
	//max = 230;
	threshold(ImageReduced, &Dark, 0, max);

	//For Lead(여기 필름 검사하고는 상관없음 나중에 lead검사에 사용하려고 시행 한것임)
	threshold(ImageReducedNotSR, &RsltNotSRRegion, 0, max);

	//고정(전체 줄임 fine)
	//max = 254;
	//max = 230;
	threshold(ImageReducedFine, &DarkFine, 0, max);

	closing_circle(Dark, &Dark, 10.5);
	connection(Dark, &Dark);
	fill_up(Dark, &Dark);
	closing_circle(Dark, &Dark, 10.5);

	closing_circle(DarkFine, &DarkFine, 5.5);
	closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
	connection(DarkFine, &DarkFine);
	fill_up(DarkFine, &DarkFine);
	closing_circle(DarkFine, &DarkFine, 5.5);
	closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
	//shape_trans(Dark, &Dark,HTuple("convex"));

	select_shape(Dark, &Dark, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
	select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	select_shape(DarkFine, &DarkFine, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
	select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//주변값을 중간값으로 채운다.
	paint_region(ChipRect, Image, &Image, 255, HTuple("fill"));

	//경계값과 비교
	count_obj(Dark, &Num);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Dark, win);
	}

	concat_obj(RsltSRRegion, Dark, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Dark, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, Image, &Mean, _);
		intensity(OutRgn, Image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		//크기가 크면 섬세하게 검사
		area_center(SelectRgn, &Area, _, _);

		if(300 < (Hlong)Area[0])
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < (diff_var + 40))//LSS new 10->40
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
		else
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)

				diff_var = abs(diff_var);
				if(dark_bright_tol < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
	}

	//경계값과 비교
	count_obj(DarkFine, &Num);

	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(DarkFine, win);
	}

	concat_obj(RsltSRRegion, DarkFine, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(DarkFine, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *mold_image, &Mean, _);
		intensity(OutRgn, *mold_image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		//크기가 크면 그냥 불량으로 뺀다.
		area_center(SelectRgn, &Area, _, _);


		if(300 < (Hlong)Area[0])
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < (diff_var + 10))
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
		else
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
	}
	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(ResultRgn, win);
	}

	max = 255 - white_thresh;
	if((0 > max) || (255 < max))
	{
		max = 230;
	}

	threshold(ImageReducedLight, &Light, max, 255);

	closing_circle(Light, &Light, 5.5);
	connection(Light, &Light);
	fill_up(Light, &Light);

	select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

	//경계값과 비교
	count_obj(Light, &Num);

	if(win)
	{
		set_color(win, HTuple("black"));
		disp_obj(Light, win);
	}

	concat_obj(RsltSRRegion, Light, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Light, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, ImageLight, &Mean, _);
		intensity(OutRgn, ImageLight, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 >= diff_var)
		{
			//Light(-)
			diff_var = abs(diff_var);
			if( (white_bright_tol) < diff_var)
			{
				concat_obj(ResultRgn, SelectRgn, &ResultRgn);
			}
		}
	
	}
	//
	//
	/////////////////////////////////////////////

	/////////////////////////////////////////////
	//Light2/////////////////////////////////////
	//긁힘(ink image==crack image)<--crack image에서 밝은 긁힘, 밝은 이물
	hysteresis_threshold(ImageReducedLight2,&Light,HTuple(210),HTuple(230),HTuple(10));

	closing_circle(Light, &Light, 45.5);
	connection(Light, &Light);
	fill_up(Light, &Light);

	select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

	//경계값과 비교
	count_obj(Light, &Num);

	concat_obj(RsltSRRegion, Light, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Light, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *mold_image, &Mean, _);
		intensity(OutRgn, *mold_image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 >= diff_var)
		{
			//Light(-)
			diff_var = abs(diff_var);
			if( (white_bright_tol) < diff_var)
			{
				concat_obj(ResultRgn, SelectRgn, &ResultRgn);
			}
		}
	
	}
	//
	//
	/////////////////////////////////////////////

	/////////////////////////////////////////////
	//Dark-SR out////////////////////////////////
	//긁힘(밝은 back light image에서 검게 나온다. 검은 영역만 잡는다. SR out pattern에서만 검사, 안쪽 가는SR 구간 정밀검사)	

	intensity(ins_rgn_srout, ImageReducedSROut, &Mean, _);
	int chk_cnt = Mean.Num();
	
	if(0 < chk_cnt)
	{
		max = (Hlong)Mean[0];
		max = 255 - dark_thresh;
		if(0 > max)
		{
			max = 30;
		}
		if(255 < max)
		{
			max = 255;
		}
		threshold(ImageReducedSROut,&DarkSROut,HTuple(0),HTuple(max));
		
		closing_circle(DarkSROut, &DarkSROut, 10.5);
		connection(DarkSROut, &DarkSROut);

		select_shape(DarkSROut, &DarkSROut, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
		select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
		select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
		
		//경계값과 비교
		count_obj(DarkSROut, &Num);
		
		concat_obj(RsltSRRegion, DarkSROut, &RsltSRRegion);
		num = (Hlong)Num[0];
		
		for(i = 0; i < num; i++)
		{
			select_obj(DarkSROut, &SelectRgn, i + 1);
			dilation_circle(SelectRgn, &OutRgn, 7.5);
			dilation_circle(SelectRgn, &IntRgn, 2.5);
			difference(OutRgn, IntRgn, &OutRgn);
			intensity(SelectRgn, ImageReducedSROut, &Mean, _);
			intensity(OutRgn, ImageReducedSROut, &OutMean, _);
			intensity(SelectRgn, ImageLight, &LightMean, _);
			diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);
			circularity(SelectRgn, &CircleVar);		
			//크기가 크면 섬세하게 검사
			area_center(SelectRgn, &Area, _, _);
			
			if(300 < (Hlong)Area[0])
			{
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)
					diff_var = abs(diff_var);
					if(dark_bright_tol < (diff_var + 10))
					{
						concat_obj(ResultRgn, SelectRgn, &ResultRgn);
					}
				}
			}
			else
			{
				//여기서만 백색 이물 제거 필요(강조를 하다 보니 흰색은 흐리게 나오는데 잡힌다.->오버킬)
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)
					
					diff_var = abs(diff_var);
					if(dark_bright_tol < diff_var)
					{
						//진원도가 0.5보다 크면서 백색 이물성이면 오버킬로 간주
						if(0.5 < (double)CircleVar[0] )
						{
							//잉크이미지(크랙이미지)에서 밝값이 밝으면 
							lightvar = (Hlong)LightMean[0];
							if(150 > lightvar)
							{
								concat_obj(ResultRgn, SelectRgn, &ResultRgn);
							}
						}
						else
						{
							concat_obj(ResultRgn, SelectRgn, &ResultRgn);
						}
					}
				}
			}
		}
	}
	//
	//
	/////////////////////////////////////////////
	copy_obj(RsltSRRegion, rslt_sr_region, 1, -1);
	copy_obj(RsltNotSRRegion, rslt_notsr_region, 1, -1);
	copy_obj(RsltUserRegion, rslt_user_region, 1, -1);
	copy_obj(RsltSROutRegion, rslt_sr_out_region, 1, -1);

	union1(ResultRgn, &ResultRgn);
	connection(ResultRgn, &ResultRgn);
	count_obj(ResultRgn, &Num);
	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{

		copy_obj(ResultRgn, rslt_region, 1, -1);
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		return FALSE;
	}

	return TRUE;
}


BOOL C_VisionModule::_Hal_FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region)
{
	HTuple Width, Height, Num, Area;
	Hobject Region, ImageReduced, RegionClosing1, RegionUnion, SelectedRegions, ConnectedRegions;
	HTuple Max;
	CRect rect;

	Hobject RsltChipRgn;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &Width, &Height);
	set_check("give_error");

	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_33!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	reduce_domain(*image,Region,&ImageReduced);

	set_check("~give_error");
	herr = threshold(ImageReduced, &ConnectedRegions, HTuple(200), HTuple(255)); 
	set_check("give_error");

	connection(ConnectedRegions, &ConnectedRegions);

	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(20000),HTuple("max"));

	union1(SelectedRegions, &SelectedRegions);

	shape_trans(SelectedRegions,&SelectedRegions,HTuple("rectangle2"));

	erosion_rectangle1(SelectedRegions, &RsltChipRgn, 3, 3);

	count_obj(RsltChipRgn, &Num);

	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RsltChipRgn, win);
	}

	area_center(RsltChipRgn, &Area, _, _);

	copy_obj(RsltChipRgn, rslt_chip_region, 1, -1);
	
	return TRUE;
}

BOOL C_VisionModule::_Hal_ChipPot(Hlong win, Hobject *image, RECT ins_region, Hobject chip_ins_region1, 
				 double offset_ox, double offset_oy, 
				 double offset_ox2, double offset_oy2, 
				 double offset_rx, double offset_ry,
				 double offset_rx2, double offset_ry2, Hlong pot_area, int thresh, int under_tol, int over_tol, int line_percent, 
				  int pot_width_1, int pot_width_2, int pot_width_3, int pot_width_4, int pot_width_5, int pot_width_6, int average_x, int average_y, Hobject *rslt_pot_region,
				 Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region, int *rslt_average_leng, int *rslt_center_leng)
{
	Hobject ImageReduced, Region, RegionClosing, InsRegion, RsltRegion, ChipRgn1, RegionLine;
	HTuple Num;
	Hlong over_area, under_area;
	double offset_x, offset_y;
	HTuple DiffAng, HomMat2D;
	HTuple ChipRow0, ChipColumn0, ChipRow1, ChipColumn1;
	HTuple ChipRow2, ChipColumn2, ChipRow3, ChipColumn3;
	CRect RectChip;

	//티칭 당시 칩영역 얼라인
	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));
	smallest_rectangle1(ChipRgn1, &ChipRow0, &ChipColumn0, &ChipRow1, &ChipColumn1);
	RectChip.left = (Hlong)ChipColumn0[0];
	RectChip.top = (Hlong)ChipRow0[0];
	RectChip.right = (Hlong)ChipColumn1[0];
	RectChip.bottom = (Hlong)ChipRow1[0];


	//칩 검사 영역
	over_area = pot_area * (over_tol * 0.01);
	under_area = pot_area * (under_tol * 0.01);

	gen_empty_obj(&RsltRegion);
	gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	reduce_domain(*image, InsRegion, &ImageReduced);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,0,HTuple(thresh));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_57!... %ld", (Hlong)thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	union1(Region, &Region);

	select_shape(Region, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(1), HTuple("max"));

	Hobject select_object, intersectionRegion;;
	HTuple Row1, Column1, Row2, Column2, Area;
	Hlong chip_width, chip_height;
	int dLine_percent;
	int rslt_length, nMin, cnt = 0;
	Hlong RowCenter, ColCenter;
	const int virtual_line = 66;

	*rslt_area = 0;
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		area_center(RsltRegion, &Area, _, _);
		*rslt_area = (Hlong)Area[0];
		if( (over_area > *rslt_area) && (under_area < *rslt_area))
		{
			
			//////////////////////////////////////////////			
			//20150817 by
			//포팅 부위별 길이 판정
			chip_width = RectChip.right - RectChip.left;
			chip_height = RectChip.bottom - RectChip.top;
			dLine_percent  = (line_percent / (int)m_dPixelSizeData);

			
			//도포 검출이 정상적으로 이루어진 후 각 길이 검사 시작

			if(average_y >= chip_width)
				average_y = (chip_width / 2) - 1;

			//#1 Y top
			if(0 < pot_width_1)
			{
				rslt_length = 0;
				nMin = (RectChip.right - (chip_width / 2)) - average_y;

				for(cnt = 0; cnt <= (average_y * 2); cnt++)
				{
					gen_region_line(&RegionLine, RectChip.top, nMin + cnt, (RectChip.top - virtual_line), nMin + cnt);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.right - (chip_width / 2)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[0], 1, -1);
						rslt_center_leng[0] = (((Hlong)ChipRow3[0] - (Hlong)ChipRow2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow2[0];
						ColCenter = ChipColumn2[0];
					}

					rslt_length += (((Hlong)ChipRow3[0] - (Hlong)ChipRow2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[0] = rslt_length / cnt;

				if(win)
				{
					set_color(win, HTuple("blue"));

					disp_obj(rslt_pot_region[0], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter-20, ColCenter);
					write_string(win, HTuple( HTuple( "#1 length : " ) + HTuple( (Hlong)rslt_average_leng[0])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[0]) + HTuple(")") ));
	
				}
			}

			//#2 Y bottom
			if(0 < pot_width_2)
			{
				rslt_length = 0;
				nMin = (RectChip.right - (chip_width / 2)) - average_y;

				//#2 Y bottom
				for(cnt = 0; cnt <= (average_y * 2); cnt++)
				{
					gen_region_line(&RegionLine, RectChip.bottom, nMin + cnt, (RectChip.bottom + virtual_line), nMin + cnt);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.right - (chip_width / 2)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[1], 1, -1);
						rslt_center_leng[1] = (((Hlong)ChipRow3[0] - (Hlong)ChipRow2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow3[0];
						ColCenter = ChipColumn3[0];
					}

					rslt_length += (((Hlong)ChipRow3[0] - (Hlong)ChipRow2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[1] = rslt_length / cnt;

				if(win)
				{
					disp_obj(rslt_pot_region[1], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter+20, ColCenter);
					write_string(win, HTuple( HTuple( "#2 length : " ) + HTuple( (Hlong)rslt_average_leng[1])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[1]) + HTuple(")") ));
				}
			}

			//#3 X Left Top
			if(0 < pot_width_3)
			{
				rslt_length = 0;
				nMin = (RectChip.top + (dLine_percent)) - average_x;
				
				for(cnt = 0; cnt <= (average_x * 2); cnt++)
				{
					gen_region_line(&RegionLine, nMin + cnt, RectChip.left, nMin + cnt, RectChip.left - virtual_line);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.top + (dLine_percent)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[2], 1, -1);
						rslt_center_leng[2] = (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow2[0];
						ColCenter = ChipColumn2[0];
					}

					rslt_length += (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[2] = rslt_length / cnt;

				if(win)
				{
					disp_obj(rslt_pot_region[2], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter-20, ColCenter);
					write_string(win, HTuple( HTuple( "#3 length : " ) + HTuple( (Hlong)rslt_average_leng[2])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[2]) + HTuple(")") ));
				}
			}

			//#4 X Left Bottom
			if(0 < pot_width_4)
			{
				rslt_length = 0;
				nMin = (RectChip.bottom - (dLine_percent)) - average_x;
				
				for(cnt = 0; cnt <= (average_x * 2); cnt++)
				{
					gen_region_line(&RegionLine, nMin + cnt, RectChip.left, nMin + cnt, RectChip.left - virtual_line);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.bottom - (dLine_percent)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[3], 1, -1);
						rslt_center_leng[3] = (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow2[0];
						ColCenter = ChipColumn2[0];
					}

					rslt_length += (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[3] = rslt_length / cnt;

				if(win)
				{
					disp_obj(rslt_pot_region[3], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter-20, ColCenter);
					write_string(win, HTuple( HTuple( "#3 length : " ) + HTuple( (Hlong)rslt_average_leng[3])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[3]) + HTuple(")") ));
				}
			}

			//#5 X Right Top
			if(0 < pot_width_5)
			{
				rslt_length = 0;
				nMin = (RectChip.top + (dLine_percent)) - average_x;
				
				for(cnt = 0; cnt <= (average_x * 2); cnt++)
				{
					gen_region_line(&RegionLine, nMin + cnt, RectChip.right, nMin + cnt, RectChip.right + virtual_line);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.top + (dLine_percent)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[4], 1, -1);
						rslt_center_leng[4] = (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow2[0];
						ColCenter = ChipColumn3[0];
					}

					rslt_length += (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[4] = rslt_length / cnt;

				if(win)
				{
					disp_obj(rslt_pot_region[4], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter+20, ColCenter);
					write_string(win, HTuple( HTuple( "#4 length : " ) + HTuple( (Hlong)rslt_average_leng[4])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[4]) + HTuple(")") ));
				}
			}

			//#6 X Right Bottom
			if(0 < pot_width_6)
			{
				rslt_length = 0;
				nMin = (RectChip.bottom - (dLine_percent)) - average_x;
				
				for(cnt = 0; cnt <= (average_x * 2); cnt++)
				{
					gen_region_line(&RegionLine, nMin + cnt, RectChip.right, nMin + cnt, RectChip.right + virtual_line);
					intersection(RegionLine, Region, &intersectionRegion);
					smallest_rectangle1(intersectionRegion, &ChipRow2, &ChipColumn2, &ChipRow3, &ChipColumn3);
					
					if( (nMin + cnt) == (RectChip.bottom - (dLine_percent)) )
					{
						copy_obj(intersectionRegion, &rslt_pot_region[5], 1, -1);
						rslt_center_leng[5] = (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
						RowCenter = ChipRow2[0];
						ColCenter = ChipColumn3[0];
					}

					rslt_length += (((Hlong)ChipColumn3[0] - (Hlong)ChipColumn2[0]) * m_dPixelSizeData);
				}
				
				rslt_average_leng[5] = rslt_length / cnt;

				if(win)
				{
					disp_obj(rslt_pot_region[5], win);
					disp_cross(win, RowCenter, ColCenter, 50, 0);

					set_tposition(win, RowCenter+20, ColCenter);
					write_string(win, HTuple( HTuple( "#6 length : " ) + HTuple( (Hlong)rslt_average_leng[5])  + HTuple( " um " ) + HTuple(" (Center : ") + HTuple((Hlong)rslt_center_leng[5]) + HTuple(")") ));
				}
			}

			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(RsltRegion, win);
			}

			*rslt_num = 0;

			copy_obj(RsltRegion, rslt_region, 1, -1);
		}
		else
		{
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(RsltRegion, win);
			}

			copy_obj(RsltRegion, rslt_region, 1, -1);
			return FALSE;
		}
	}
	else
	{
		//Chip region
		copy_obj(InsRegion, rslt_region, 1, -1);

		*rslt_num = 1;
		return FALSE;  
	}

	return TRUE;
}

BOOL C_VisionModule::_Hal_FilmPF(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int black_thresh, int black_tolerance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr) 
{
	BOOL rslt = TRUE;
	Hobject Region, SubImage, ImageReduced, RsltRegion, DontCare, ChipRgn, AffineRegion, InsRgn;
	Hobject ImageReduced2, Region2, InsRgn2, RsltRegion2;
	HTuple Num, HomMat2D;
	double offset_x, offset_y;
	HTuple DiffAng;

	int low_threshold, high_threshold;
	HTuple Mean;

	Hobject RsltPFRegion, RsltRegionTot;
	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltPFRegion);
	gen_empty_obj(&RsltRegionTot);

	*rslt_num = 0;
	Hlong rslt_num_hall, rslt_num_tear;
	rslt_num_hall = 0;
	rslt_num_tear = 0;

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	high_threshold = 255 - thresh;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}

	//Offset
	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	gen_rectangle1(&Region, (Hlong)(ins_region.top + offset_y), 
		                    (Hlong)(ins_region.left + offset_x), 
							(Hlong)(ins_region.bottom + offset_y), 
							(Hlong)(ins_region.right + offset_x));

	gen_rectangle1(&ChipRgn, (Hlong)(chip_region.top + offset_y), 
		                    (Hlong)(chip_region.left + offset_x), 
							(Hlong)(chip_region.bottom + offset_y), 
							(Hlong)(chip_region.right + offset_x));

	difference(Region, ChipRgn, &Region);

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(pf_region, &AffineRegion, HomMat2D, HTuple("false"));

	//PF for image///////////////////////////////
	//찢어짐 검사
	difference(Region, AffineRegion, &RsltPFRegion);
	erosion_circle(RsltPFRegion, &InsRgn, 7.5);
	erosion_rectangle1(InsRgn, &InsRgn, 15, 3.5);

	difference(InsRgn, vacc_region, &InsRgn);

	reduce_domain(*image, InsRgn, &ImageReduced);

	hysteresis_threshold(ImageReduced, &RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));
	closing_circle(RsltRegion,&RsltRegion,HTuple(3.5));

	/////////////////////////////////////////////////////////
	
	connection(RsltRegion, &RsltRegion);
	select_shape(RsltRegion, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));


	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	count_obj(RsltRegion, &Num);

	rslt_num_tear = (Hlong)Num[0];
	if(0 < rslt_num_tear)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			set_draw(win, HTuple("margin"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("blue"));
		set_draw(win, HTuple("margin"));
		disp_obj(RsltPFRegion, win);
	}

	//PF for image2//////////////////////////////////////
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	//막힘검사

	erosion_circle(AffineRegion, &Region2, 10.5);

	intersection(Region2, Region, &RsltPFRegion);

	reduce_domain(*image2, RsltPFRegion, &ImageReduced2);

	intensity(InsRgn2, *image2, &Mean, _);
	high_threshold = (Hlong)Mean[0] + black_thresh + 20;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	
	threshold(ImageReduced2, &RsltRegion2, 0, low_threshold);
	closing_circle(RsltRegion2, &RsltRegion2, HTuple(3.5));

	/////////////////////////////////////////////////////////
	
	connection(RsltRegion2, &RsltRegion2);
	select_shape(RsltRegion2, &RsltRegion2, HTuple("height"), HTuple("and"), HTuple(black_tolerance),  HTuple("max"));

	if(win)
	{
		set_color(win, HTuple("blue"));
		set_draw(win, HTuple("margin"));
		disp_obj(RsltPFRegion, win);
	}

	count_obj(RsltRegion2, &Num);
	rslt_num_hall = (Hlong)Num[0];
	if(0 < rslt_num_hall)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			set_draw(win, HTuple("margin"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}
	
	*rslt_num = rslt_num_tear + rslt_num_hall;
	
	concat_obj(RsltRegion, RsltRegion2, &RsltRegionTot);
	copy_obj(RsltRegionTot, rslt_region, 1, -1);
	copy_obj(RsltPFRegion, rslt_pf_region, 1, -1);
	return rslt;
}


BOOL C_VisionModule::_Hal_FilmPF2(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int area, int black_thresh, int black_tolerance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr) 
{
	BOOL rslt = TRUE;
	Hobject Region, SubImage, ImageReduced, RsltRegion, DontCare, ChipRgn, AffineRegion, InsRgn, ComplementRgn;
	Hobject ImageReduced2, Region2, InsRgn2, RsltRegion2;
	HTuple Num, HomMat2D;
	double offset_x, offset_y;
	HTuple DiffAng;

	int low_threshold, high_threshold;
	HTuple Mean;
	int pf_width;

	double insp_area;

	HTuple row0, col0, row1, col1;

	Hobject up_region, dn_region, updn_region;
	gen_empty_obj(&updn_region);

	Hobject RsltPFRegion, RsltRegionTot;
	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltPFRegion);
	gen_empty_obj(&RsltRegionTot);

	*rslt_num = 0;
	Hlong rslt_num_hall, rslt_num_tear;
	rslt_num_hall = 0;
	rslt_num_tear = 0;

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	high_threshold = 255 - thresh;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}

	//Offset
	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	//20150721 by
	//실제 검사할 영역을 계산한다
	if( (1 > area) || (100 < area) )
		area = 1;

	insp_area = (double)area * 0.01;
	pf_width = ((COF_PF_WIDTH * MICRO_METER) / m_dPixelSizeData) * insp_area;

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(pf_region, &RsltPFRegion, HomMat2D, HTuple("false"));
	smallest_rectangle1(pf_region, &row0, &col0, &row1, &col1);

	
	//20151019 by
	//VT-05 임시 코드
	/*
	gen_rectangle1(&up_region, (Hlong)((Hlong)row0[0] + offset_y), 
		                    (Hlong)((Hlong)col0[0] + offset_x), 
							(Hlong)((Hlong)row1[0] + offset_y),  //   PF 폭 3.3mm		
							(Hlong)((Hlong)col0[0] + pf_width + offset_x) );

	gen_rectangle1(&dn_region, (Hlong)((Hlong)row0[0] + offset_y), //135	// PF 폭 3.38mm
		                    (Hlong)((Hlong)col1[0] - pf_width + offset_x), 
							(Hlong)((Hlong)row1[0] + offset_y), 
							(Hlong)((Hlong)col1[0] + offset_x));
*/
	//20151019 by
	//정석 코드.

	gen_rectangle1(&up_region, (Hlong)((Hlong)row0[0] + offset_y), 
		                    (Hlong)((Hlong)col0[0] + offset_x), 
							(Hlong)((Hlong)row0[0] + offset_y + pf_width),  //   PF 폭 3.3mm		
							(Hlong)((Hlong)col1[0] + offset_x));

	gen_rectangle1(&dn_region, (Hlong)((Hlong)row1[0] + offset_y - pf_width), //135	// PF 폭 3.38mm
		                    (Hlong)((Hlong)col0[0] + offset_x), 
							(Hlong)((Hlong)row1[0] + offset_y), 
							(Hlong)((Hlong)col1[0] + offset_x));


	//PF for image///////////////////////////////
	erosion_circle(RsltPFRegion, &RsltPFRegion, 7.5);
	erosion_rectangle1(RsltPFRegion, &RsltPFRegion, 3, 15); // VT05 3, 15 그외 15, 3

	union2(up_region, dn_region, &updn_region);
	intersection(RsltPFRegion, updn_region, &InsRgn); 
	reduce_domain(*image, InsRgn, &ImageReduced);

	threshold(ImageReduced, &RsltRegion, 0, thresh);
	closing_circle(RsltRegion,&RsltRegion,HTuple(3.5));

	/////////////////////////////////////////////////////////
	
	//1차 후보군 검출
	connection(RsltRegion, &RsltRegion);
	
	select_shape(RsltRegion, &RsltRegion, HTuple("height"), HTuple("and"), HTuple(tolerance),  HTuple("max"));
	
	count_obj(RsltRegion, &Num);

	rslt_num_tear = (Hlong)Num[0];
	if(0 < rslt_num_tear)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			set_draw(win, HTuple("margin"));
			disp_obj(InsRgn, win);
		}

		rslt = FALSE;
	}


	if(win)
	{
		set_color(win, HTuple("blue"));
		set_draw(win, HTuple("margin"));
		disp_obj(InsRgn, win);
	}

	*rslt_num = rslt_num_tear;

	
	copy_obj(RsltRegion, rslt_region, 1, -1);
	copy_obj(InsRgn, rslt_pf_region, 1, -1);

	return rslt;
}

BOOL C_VisionModule::_Hal_InLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
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
						   Hlong min_area, Hlong min_width, Hlong min_height)
{
	HTuple   Pointer, Type, Width, Height, WindowHandle, Row1;
	HTuple   Column1, Row2, Column2, Row, Sum, Column, Diff;
	HTuple   Length1, UntilHeight, Start, MeasureHandle0, MeasureHandle1;
	HTuple   RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond;
	HTuple   ColumnEdgeSecond, AmplitudeSecond, IntraDistance;
	HTuple   InterDistance, End, Time;
	Hobject  I, InsRegion, Line, TotLine, TotLine_Vert;
	HTuple	 Distance;
	Hlong idx;
	BOOL rslt = TRUE;
	HTuple HomMat2D;

	HTuple Num;
	Hobject select_object, NotSRRgn;

	get_image_pointer1(*image,&Pointer,&Type,&Width,&Height);
	
	Hlong row = ins_region.top + 1;
	Hlong col = (ins_region.left + ins_region.right) * 0.5;
	Hlong leng1 = (ins_region.right - ins_region.left) * 0.5;
	Hlong height = (ins_region.bottom - ins_region.top) - 1;

	Hlong row_vert = (ins_region.top + ins_region.bottom) * 0.5;
	Hlong col_vert = ins_region.left + 1;
	Hlong leng1_vert = (ins_region.bottom - ins_region.top) * 0.5;
	Hlong height_vert = (ins_region.right - ins_region.left) - 1;

	HTuple DiffAng, Area;

	Hobject InterRgn, SelectRgn;

	Hlong i;

	Hobject RsltRegion, RsltRegion2, RsltDNRegion;

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltRegion2);
	gen_empty_obj(&RsltDNRegion);
	
	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);
	
	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(dn_region, &RsltDNRegion, HomMat2D, HTuple("false"));

	if(0.4 > factor)
	{
		factor = 0.4;
	}
	double factor_vert = 10;
	
    gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	
	//Display inspection region
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(InsRegion, win);
	}           

	gen_empty_obj(&TotLine);
	Hlong line_width = 0;
	Hlong line_idx = 0;
	double dist = 0;

	//Image//////////////////////////////////////
	gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=1)
	{
		translate_measure(MeasureHandle0, row + idx, col);
		measure_pos(*image, MeasureHandle0, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		translate_measure(MeasureHandle1, row + idx, col);
		measure_pos(*image, MeasureHandle1, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
			
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(Line, win);
			}
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("blue"));
				disp_obj(Line, win);
			}
		}
	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);

	HTuple Mean, TotMean;
	Hlong mean, totmean;
	Hobject Region;
	gen_empty_obj(&TotLine_Vert);
	

	intensity(RsltDNRegion, *image, &TotMean, _);
	totmean = (Hlong)TotMean[0];
	
	//connection(RsltDNRegion, &RsltDNRegion);

	erosion_rectangle1(RsltDNRegion, &RsltDNRegion, 5, 1);
	union1(RsltDNRegion, &RsltDNRegion);
	closing_rectangle1(RsltDNRegion, &RsltDNRegion, 20, 5);
	connection(RsltDNRegion, &Region);

	count_obj(Region, &Num);

	Hlong ResultCount;
	ResultCount = (Hlong)Num[0];

	BOOL chk;
	int j;
	chk = FALSE;

	for(i = 0; i < ResultCount; i++)
	{
		chk = FALSE;

		select_obj(Region, &SelectRgn, i + 1);
		intensity(SelectRgn, *image, &Mean, _);
		mean = (Hlong)Mean[0];

		if( (mean + threshold) <  totmean)
		{
			//불량이면 오버킬을 방지하기 위해 위아래로 몇픽셀 움직여서 재검사한다.

			for(j = -5; j <= 5; j++)
			{
				vector_angle_to_rigid(0, 0, HTuple(0), j, 0, 0, &HomMat2D);
				affine_trans_region(SelectRgn, &SelectRgn, HomMat2D, HTuple("false"));

				intensity(SelectRgn, *image, &Mean, _);
				mean = (Hlong)Mean[0];
				if( (mean + threshold) <  totmean)
				{
					//AfxMessageBox("test");
				}
				else
				{
					//양품처리
					chk = TRUE;
					break;
				}
			}
			if(!chk)
			{
				union2(TotLine_Vert, SelectRgn, &TotLine_Vert);
			}
		}
	}

	union2(TotLine, TotLine_Vert, &TotLine);

	connection(TotLine, &TotLine);

	//dilation_rectangle1(RsltDNRegion, &RsltDNRegion, 1.5, 1.5);
	//intersection(TotLine, RsltDNRegion, &TotLine);


	union1(TotLine, &TotLine);
	

	closing_circle(TotLine, &TotLine, HTuple(15.5));

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("or"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(TotLine, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//Image2/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=1)
	{
		translate_measure(MeasureHandle0, row + idx, col);
		measure_pos(*image2, MeasureHandle0, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		translate_measure(MeasureHandle1, row + idx, col);
		measure_pos(*image2, MeasureHandle1, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
			
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(Line, win);
			}
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("blue"));
				disp_obj(Line, win);
			}
		}
	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);

	connection(TotLine, &TotLine);

	intersection(TotLine, RsltDNRegion, &TotLine);

	union1(TotLine, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(15.5));
	
	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(TotLine, &RsltRegion2, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//Image//////////////////////////////////////
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}
	copy_obj(RsltRegion, rslt_region, 1, -1);

	//Image2/////////////////////////////////////
	count_obj(RsltRegion2, &Num);

	*rslt_num2 = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}
	copy_obj(RsltRegion2, rslt_region2, 1, -1);

	copy_obj(RsltDNRegion, rslt_dn_region, 1, -1);

	close_all_measures();

	return rslt;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 110720 ytlee
void C_VisionModule::WriteErrorList(LPCTSTR lpszItem)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);

	memset(name, NULL, sizeof(temp));
	sprintf(name, "Error_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "E:\\Insp_Result_%d\\%04d%02d%02d\\SYSTEM_LOG\\", GetVisIndex(), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteErrorStatus(path, name, "Error_Log.bak", temp, 0);
}

void C_VisionModule::WriteNotAgreeInspParam(CString ChangeParamName, int notagreepara) 
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	CString Dir, strFile;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d] : %s -> %d", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, ChangeParamName, notagreepara);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "D:\\DATABASE\\LOG\\%04d%02d%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteErrorStatus(path, "InspParamChageLog.dat", "InspParamChageLog.bak", temp, 1000);
}

BOOL C_VisionModule::WriteErrorStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			AfxMessageBox("Error : log file open fail!(WriteErrorStatus)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	110723 ytlee 체크 항목 모델별로 로드 위치 이동		
BOOL C_VisionModule::LoadInspectSelectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;
	CString file_path, pat_path, extention, master_path;
	
	file_path = path + "\\InspectionSel.par";
	
	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
				for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Sys%d", i, j);
					m_VDat[i]->m_SysPara.m_bPara[j] = GetPrivateProfileInt("INSPECTION_SELECT_PARAMETER", key, 0, file_path);
					

				}
			}
		}
	}
	fd=0;
	
	return rslt;
}

BOOL C_VisionModule::SaveInspectSelectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;
	CString file_path, pat_path, extention, master_path;
	
	//Make director
	MakeDirectories(path);
	
	file_path = path + "\\InspectionSel.par";
	
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		//write INI
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			switch(i)
			{
			case 0:
				for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Sys%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_SysPara.m_bPara[j]);
					WritePrivateProfileString("INSPECTION_SELECT_PARAMETER", key, dat, file_path);
				}
			}
		}
		
	}
	fd=0;
	
	return rslt;
}
