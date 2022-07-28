/**
******************************************************************************
*File: ComSlice.cpp
*Descriptions: slice header and SPS class
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-01-06

******************************************************************************
*/


#include "LibComSlice.h"
#include "LibComPic.h"











// ------------------------------------------------------------------------------------------------
// Sequence parameter set (SPS) class
// ------------------------------------------------------------------------------------------------


LibComSPS::LibComSPS()
: m_iSPSId(0)
, m_ChromaFormatIdc(CHROMA_420)
, m_uiPicWidthInLumaSamples(352)
, m_uiPicHeightInLumaSamples(288)
, m_uiMaxCUWidth(32)
, m_uiMaxCUHeight(32)
, m_uiMaxTotalCUDepth(3)
{

}


LibComSPS::~LibComSPS()
{

}

const Int LibComSPS::m_aiWinUnitX[] = { 1,2,2,1 };// Static members can only be initialized outside the class.
const Int LibComSPS::m_aiWinUnitY[] = { 1,2,1,1 };

template <> 
Void ParameterSetMap<LibComSPS>::set_ID(LibComSPS* parameterSet, const Int psId)
{
	parameterSet->Set_SPS_id(psId);
}




// ------------------------------------------------------------------------------------------------
// Picture parameter set (PPS) class
// ------------------------------------------------------------------------------------------------
LibComPPS::LibComPPS()
: m_iPPSId(0)
, m_iSPSId(0)
, m_bTilesEnabledFlag(false)
{

}


LibComPPS::~LibComPPS()
{

}

template <>
Void ParameterSetMap<LibComPPS>::set_ID(LibComPPS* parameterSet, const Int psId)
{
	parameterSet->Set_PPS_id(psId);
}



// ------------------------------------------------------------------------------------------------
// Slice clss
// ------------------------------------------------------------------------------------------------

LibComSlice::LibComSlice()
: m_iPOC(0)
, m_bRefenced(false)
, m_uiSliceCurStartCtuTsAddr(0)
, m_uiSliceCurEndCtuTsAddr(0)
, m_uiSliceSegmentCurStartCtuTsAddr(0)
, m_uiSliceSegmentBits(0)
, m_iSliceQP(0)
{

}



LibComSlice::~LibComSlice()
{

}

Void LibComSlice::Init_slice()
{

}


/******************************************************************************
Description:
Sort the pic in the list by POC from smallest to largest.
Input para:
-listPic
Return value:
-sorted listPic
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void LibComSlice::sort_pic_list(LibComList<LibComPic*>& listPic)
{

	LibComPic* picExtract;
	LibComPic* picInsert;

	LibComList<LibComPic*>::iterator iterPicExtract;
	LibComList<LibComPic*>::iterator iterPicExtract1;
	LibComList<LibComPic*>::iterator iterPicInsert;

	for (Int i = 1; i < listPic.size(); i++)
	{
		iterPicExtract = listPic.begin();
		for (Int j = 0; j < i; j++)
		{
			iterPicExtract++;
		}
		picExtract = *(iterPicExtract);
		picExtract->Set_curr_slice_idx(0);// Because sort some frames, not slices in one frame, so set slice to 0. 

		iterPicInsert = listPic.begin();
		while (iterPicInsert != iterPicExtract)
		{
			picInsert = *(iterPicInsert);
			picInsert->Set_curr_slice_idx(0);

			if (picInsert->Get_POC() >= picExtract->Get_POC())
			{
				break;
			}
			iterPicInsert++;


		}
		iterPicExtract1 = iterPicExtract;
		iterPicExtract1++;

		listPic.insert(iterPicInsert, iterPicExtract, iterPicExtract1);
		listPic.erase(iterPicExtract);

	}

}


Void LibComSlice::Copy_slice_info(LibComSlice* pcSliceSrc)
{
	assert(pcSliceSrc != NULL);

	m_iSliceQP = pcSliceSrc->m_iSliceQP;


}


