/**
******************************************************************************
*File: LibComSlice.h
*Descriptions: slice header and SPS class (header)
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-01-06

******************************************************************************
*/


#ifndef _COM_SLICE_H_
#define _COM_SLICE_H_

#include <cassert>
#include <vector>
#include <map>


#include "LibCommonDef.h"
#include "LibComList.h"


// ====================================================================================================================
// Prior declaration
// ====================================================================================================================
class LibComPic;




// ====================================================================================================================
// Class definition
// ====================================================================================================================


class ProfileTierLevel
{
public:
	Profile::Name Get_profile_idc() const { return m_eProfileIdc; }
	Void Set_profile_idc(Profile::Name x) { m_eProfileIdc = x; }


protected:
private:
	Profile::Name m_eProfileIdc;


};


class LibComPTL
{
public:

	ProfileTierLevel* Get_general_PTL() { return &m_cGeneralPTL; }




protected:
private:
	ProfileTierLevel m_cGeneralPTL;
};





// SPS class.
class LibComSPS
{

public:
	LibComSPS();
	virtual ~LibComSPS();


	Void Set_SPS_id(Int i) { m_iSPSId = i; }


	static Int Get_win_unitX(Int chromaFormatIdc) { assert(chromaFormatIdc >= 0 && chromaFormatIdc < NUM_CHROMA_FORMAT); return m_aiWinUnitX[chromaFormatIdc]; }
	static Int Get_win_unitY(Int chromaFormatIdc) { assert(chromaFormatIdc >= 0 && chromaFormatIdc < NUM_CHROMA_FORMAT); return m_aiWinUnitY[chromaFormatIdc]; }

	Void Set_chroma_format_idc(ChromaFormat i) { m_ChromaFormatIdc = i; }
	ChromaFormat Get_chroma_format_idc() const { return m_ChromaFormatIdc; }

	Void Set_pic_width_in_luma_samples(UInt u) { m_uiPicWidthInLumaSamples = u; }
	UInt Get_pic_width_in_luma_samples() const { return m_uiPicWidthInLumaSamples; }
	Void Set_pic_height_in_luma_samples(UInt u) { m_uiPicHeightInLumaSamples = u; }
	UInt Get_pic_height_in_luma_samples() const { return m_uiPicHeightInLumaSamples; }
	Void Set_max_cu_width(UInt u) { m_uiMaxCUWidth = u; }
	UInt Get_max_cu_width() const { return m_uiMaxCUWidth; }
	Void Set_max_cu_height(UInt u) { m_uiMaxCUHeight = u; }
	UInt Get_max_cu_height() const { return m_uiMaxCUHeight; }
	Void Set_max_total_cu_depth(UInt u) { m_uiMaxTotalCUDepth = u; }
	UInt Get_max_total_cu_depth() const { return m_uiMaxTotalCUDepth; }


	LibComPTL* Get_PTL() { return &m_cPTL; }





private:
	Int m_iSPSId;

	ChromaFormat m_ChromaFormatIdc;

	// Structure.
	UInt m_uiPicWidthInLumaSamples;
	UInt m_uiPicHeightInLumaSamples;
	UInt m_uiMaxCUWidth;
	UInt m_uiMaxCUHeight;
	UInt m_uiMaxTotalCUDepth;// Total CU depth, relative to the smallest possible transform block size.


	static const Int m_aiWinUnitX[NUM_CHROMA_FORMAT];
	static const Int m_aiWinUnitY[NUM_CHROMA_FORMAT];
	LibComPTL m_cPTL;

};


// PPS class
class LibComPPS
{
public:
	LibComPPS();
	virtual ~LibComPPS();


	Void Set_PPS_id(Int i) { m_iPPSId = i; }

	Int Get_SPS_id() const { return m_iSPSId; }
	Int Get_PPS_id() const { return m_iPPSId; }


	Void Set_tiles_enabled_flag(Bool val) { m_bTilesEnabledFlag = val; }
	Bool Get_tiles_enabled_flag() const { return m_bTilesEnabledFlag; }
	Void Set_tile_uniform_spacing_flag(Bool b) { m_bUniformSpacingFlag = b; }
	Bool Get_tile_uniform_spacing_flag() const { return m_bUniformSpacingFlag; }
	Void Set_num_tile_columns_minus1(Int i) { m_iNumTileColumnsMinus1 = i; }
	Int Get_num_tile_columns_minus1() const { return m_iNumTileColumnsMinus1; }
	Void Set_num_tile_rows_minus1(Int i) { m_iNumTileRowsMinus1 = i; }
	Int Get_num_tile_rows_minus1() const { return m_iNumTileRowsMinus1; }
	Void Set_tile_column_width(const std::vector<Int>& columnWidth) { m_aiTileColumnWidth = columnWidth; }
	UInt Get_tile_column_width(UInt columnIdx) const { return m_aiTileColumnWidth[columnIdx]; }
	Void Set_tile_row_height(const std::vector<Int>& rowHeight) { m_aiTileRowHeight = rowHeight; }
	UInt Get_tile_row_height(UInt rowIdx) const { return m_aiTileRowHeight[rowIdx]; }

protected:
private:
	Int m_iPPSId;// pic_parameter_set_id
	Int m_iSPSId;// seq_parameter_set_id

	Bool m_bTilesEnabledFlag;// Indicates the presence of tiles.


	Bool m_bUniformSpacingFlag;
	Int m_iNumTileColumnsMinus1;
	Int m_iNumTileRowsMinus1;
	std::vector<Int> m_aiTileColumnWidth;
	std::vector<Int> m_aiTileRowHeight;


};





// Slice header class.
class LibComSlice
{
public:
	LibComSlice();
	virtual ~LibComSlice();

	Void Init_slice();


	Void Set_PPS(const LibComPPS* pcPPS) { m_pcPPS = pcPPS;  m_iPPSId = (pcPPS) ? pcPPS->Get_PPS_id() : -1;}
	const LibComPPS* Get_pps() const { return m_pcPPS; }
	Void Set_SPS(const LibComSPS* pcSPS) { m_pcSPS = pcSPS; }


	Int Get_POC() const { return m_iPOC; }
	Void Set_POC(Int i) { m_iPOC = i; }
	Int Get_slice_QP() const { return m_iSliceQP; }


	static Void sort_pic_list(LibComList<LibComPic*>& listPic);

	Bool Is_referenced() const { return m_bRefenced; }


    Void Set_slice_QP(Int i) { m_iSliceQP = i; }


	Void Copy_slice_info(LibComSlice* pcSliceSrc);


	Void Set_lambdas(const Double lambdas[MAX_NUM_COMPONENT])
	{
		for (Int component = 0; component < MAX_NUM_COMPONENT; component++)
		{
			m_dLambdas[component] = lambdas[component];
		}
	}

	Void Set_nal_unit_type(NalUnitType e) { m_eNalUnitType = e; }


	Void Set_slice_cur_start_ctu_ts_addr(UInt ctuTsAddr) { m_uiSliceCurStartCtuTsAddr = ctuTsAddr; }
	UInt Get_slice_cur_start_ctu_ts_addr() const { return m_uiSliceCurStartCtuTsAddr; }
	Void Set_slice_cur_end_ctu_ts_addr(UInt ctuTsAddr) { m_uiSliceCurEndCtuTsAddr = ctuTsAddr; }
	UInt Get_slice_cur_end_ctu_ts_addr() const { return m_uiSliceCurEndCtuTsAddr; }

	Void Set_slice_segment_bits(UInt val) { m_uiSliceSegmentBits = val; }

	Void Set_slice_segment_cur_start_ctu_ts_addr(UInt ctuTsAddr) { m_uiSliceSegmentCurStartCtuTsAddr = ctuTsAddr; }
	UInt Get_slice_segment_cur_start_ctu_ts_addr() const { return m_uiSliceSegmentCurStartCtuTsAddr; }
	Void Set_slice_segment_cur_end_ctu_ts_addr(UInt ctuTsAddr) { m_uiSliceSegmentCurEndCtuTsAddr = ctuTsAddr; }
	UInt Get_slice_segment_cur_end_ctu_ts_addr() const { return m_uiSliceSegmentCurEndCtuTsAddr; }


protected:
private:

	Int m_iPPSId;// Picture parameter set ID


	Int m_iPOC;

	Int m_iSliceQP;


	// Referenced slice flag.
	Bool m_bRefenced;

	const LibComPPS* m_pcPPS;
	const LibComSPS* m_pcSPS;


	Double m_dLambdas[MAX_NUM_COMPONENT];

	NalUnitType m_eNalUnitType;// Nal unit type for the slice


	UInt m_uiSliceCurStartCtuTsAddr;
	UInt m_uiSliceCurEndCtuTsAddr;

	UInt m_uiSliceSegmentCurStartCtuTsAddr;
	UInt m_uiSliceSegmentCurEndCtuTsAddr;

	UInt m_uiSliceSegmentBits;


};




template <class T>
class ParameterSetMap
{

public:

	ParameterSetMap(Int maxId):m_iMaxId(maxId){}

	template <class Tm>
	struct MapData 
	{
		Bool m_bChanged;
		std::vector<UChar>* m_pNaluData;
		Tm* m_pParameterSet;
	};

	

	T* AllocatePS(const Int psId)
	{
		assert(psId < m_iMaxId);
		if (m_ParamSetMap.find(psId) == m_ParamSetMap.end())
		{
			m_ParamSetMap[psId].m_bChanged = true;
			m_ParamSetMap[psId].m_pNaluData = 0;
			m_ParamSetMap[psId].m_pParameterSet = new T;
			set_ID(m_ParamSetMap[psId].m_pParameterSet, psId);
		}
		return m_ParamSetMap[psId].m_pParameterSet;
	}
	


	T* Get_first_PS()
	{
		return (m_ParamSetMap.begin() == m_ParamSetMap.end()) ? NULL : m_ParamSetMap.begin()->second.m_pParameterSet;
	}

	T* Get_PS(Int psId)
	{
		typename std::map<Int, MapData<T>>::iterator it = m_ParamSetMap.find(psId);
		return (it == m_ParamSetMap.end()) ? NULL : (it)->second.m_pParameterSet;
	}




private:
	std::map<Int, MapData<T>> m_ParamSetMap;
	Int m_iMaxId;

	static Void set_ID(T* parameterSet, const Int psId);

};




#endif// _COM_SLICE_H_


