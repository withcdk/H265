



#include "LibEncSlice.h"

#include "LibEncTop.h"

using namespace std;

// ====================================================================================================================
// Constructor / destructor / create / destroy / init
// ====================================================================================================================


LibEncSlice::LibEncSlice()
{
	
}

LibEncSlice::~LibEncSlice()
{

}



Void LibEncSlice::Init(LibEncTop* encTop)
{
	m_pcCfg = encTop;
	m_pppcRDSbacCoder = encTop->Get_rd_sbac_coder();


	m_pcCuEncoder = encTop->Get_cu_encoder();


}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================




Void LibEncSlice::Init_enc_slice(LibComPic* pic, const Int pocLast, const Int pocCurr, const Int GOPid, LibComSlice*& slice)
{
    Int QP;
	slice = pic->Get_slice(0);

    QP = m_pcCfg->Get_QP_for_picture(GOPid, slice);

    slice->Set_slice_QP(QP);

}




/****************************************************************************************
Description: Multi-loop slice encoding for different slice QP
Input para:
\param pic Picture class
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*****************************************************************************************/
Void LibEncSlice::Precompress_slice(LibComPic* pic)
{
	return;
}



/****************************************************************************************
Description: Analysis stage of slice.
Input para:
\param pic Picture class
\param compressEntireSlice: If compressEntireSlice is true, 
                            then the entire slice (not slice segment) is compressed,
                            effectively disabling the slice-segment-mode.
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*****************************************************************************************/
Void LibEncSlice::Compress_slice(LibComPic* pic, const Bool compressEntireSlice, const Bool fastDeltaQP)
{
	UInt startCtuTsAddr;
	UInt boundingCtuTsAddr;
	const UInt frameWidthInCtus = pic->Get_pic_sym()->Get_frame_width_in_ctus();

	LibComSlice* const slice = pic->Get_slice(Get_slice_idx());
	slice->Set_slice_segment_bits(0);
	Determine_start_and_bounding_ctu_ts_addr(startCtuTsAddr, boundingCtuTsAddr, pic);
	
	if (compressEntireSlice)
	{
		boundingCtuTsAddr = slice->Get_slice_cur_end_ctu_ts_addr();
		slice->Set_slice_segment_cur_end_ctu_ts_addr(boundingCtuTsAddr);
	}

	//m_pcEntropyCoder->Set_entropy_coder(m_pppcRDSbacCoder[0][CI_CURR_BEST]);
	//m_pcEntropyCoder->Reset_entropy(slice);



	// For every CTU in the slice segment (may terminate sooner if there is a byte limit on the slice-segment).
	for (UInt ctuTsAddr = startCtuTsAddr; ctuTsAddr < boundingCtuTsAddr; ++ctuTsAddr)
	{
		const UInt ctuRsAddr = pic->Get_pic_sym()->Get_ctu_ts_to_rs_addr_map(ctuTsAddr);

		// Initialize CTU encoder.
		LibComDataCU* ctu = pic->Get_ctu(ctuRsAddr);
		ctu->Init_ctu(pic, ctuRsAddr);

		const UInt firstCtuRsAddrOfTile = pic->Get_pic_sym()->Get_lib_com_tile(pic->Get_pic_sym()->Get_tile_idx_map(ctuRsAddr))
			->Get_first_ctu_rs_addr();
		const UInt tileXPosInCtus = firstCtuRsAddrOfTile % frameWidthInCtus;
		const UInt ctuXPosInCtus = ctuRsAddr % frameWidthInCtus;
		

		// Run CTU trial encoder.
		m_pcCuEncoder->Compress_ctu(ctu);




	}



}



/**************************************************************************************************
Description: Determines the starting and bounding CTU address of current slice / dependent slice.
Parameter:
\param [out] startCtuTsAddr
\param [out] boundingCtuTsAddr
\param [in]  pic
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
****************************************************************************************************/
Void LibEncSlice::Determine_start_and_bounding_ctu_ts_addr(UInt& startCtuTsAddr, UInt& boundingCtuTsAddr, LibComPic* pic)
{
	LibComSlice* slice = pic->Get_slice(Get_slice_idx());

	// Non-dependent slice.
	UInt startCtuTsAddrSlice = slice->Get_slice_cur_start_ctu_ts_addr();
	Bool haveReachedTileBoundarySlice = false;
	UInt boundingCtuTsAddrSlice;
	calculate_bounding_ctu_ts_addr_for_slice(startCtuTsAddrSlice, boundingCtuTsAddrSlice, haveReachedTileBoundarySlice, pic,
	                                         m_pcCfg->Get_slice_mode(), m_pcCfg->Get_slice_argument());
	slice->Set_slice_cur_end_ctu_ts_addr(boundingCtuTsAddrSlice);
	slice->Set_slice_cur_start_ctu_ts_addr(startCtuTsAddrSlice);

	// Dependent slice.
	UInt startCtuTsAddrSliceSegment = slice->Get_slice_segment_cur_start_ctu_ts_addr();
	Bool haveReachedTileBoundarySliceSegment = false;
	UInt boundingCtuTsAddrSliceSegment;
	calculate_bounding_ctu_ts_addr_for_slice(startCtuTsAddrSliceSegment, boundingCtuTsAddrSliceSegment, haveReachedTileBoundarySliceSegment,
		pic, m_pcCfg->Get_slice_segment_mode(), m_pcCfg->Get_slice_segment_argument());
	if (boundingCtuTsAddrSliceSegment > boundingCtuTsAddrSlice)
	{
		boundingCtuTsAddrSliceSegment = boundingCtuTsAddrSlice;
	}
	slice->Set_slice_segment_cur_start_ctu_ts_addr(startCtuTsAddrSliceSegment);
	slice->Set_slice_segment_cur_end_ctu_ts_addr(boundingCtuTsAddrSliceSegment);

	// Make a joint decision based on reconstruction and dependent slice bounds.
	startCtuTsAddr = max(startCtuTsAddrSlice, startCtuTsAddrSliceSegment);
	boundingCtuTsAddr = boundingCtuTsAddrSliceSegment;

}




// ====================================================================================================================
// Protected member functions
// ====================================================================================================================






// ====================================================================================================================
// Private member functions
// ====================================================================================================================

/**************************************************************************************************
Description: calculate bounding ctu ts addr for current slice.
Parameter:
\param [in] startCtuTsAddrSlice: Start ctu tile scan address in current slice.
\param [out] boundingCtuTsAddrSlice: Bounding_ctu ts addr for current slice.
\param [out] haveReachedTileBoundary: Have reached tile boundary or not.

Author£ºDe-Kai Chen    <cdk5@foxmail.com>
****************************************************************************************************/
Void LibEncSlice::calculate_bounding_ctu_ts_addr_for_slice(UInt& startCtuTsAddrSlice, UInt& boundingCtuTsAddrSlice, 
	                                                       Bool& haveReachedTileBoundary, LibComPic* pic, 
	                                                       const Int sliceMode, const Int sliceArgument)
{
	LibComSlice* slice = pic->Get_slice(Get_slice_idx());
	const UInt numberOfCtusInFrame = pic->Get_number_of_Ctus_in_frame();
	const LibComPPS& pps = *(slice->Get_pps());
	boundingCtuTsAddrSlice = 0;
	haveReachedTileBoundary = false;
	
	switch (sliceMode)
	{
	case FIXED_NUMBER_OF_CTU:
	  {
		UInt ctuAddrIncrement = sliceArgument;
		boundingCtuTsAddrSlice = ((startCtuTsAddrSlice + ctuAddrIncrement) < numberOfCtusInFrame) ?
			(startCtuTsAddrSlice + ctuAddrIncrement) : numberOfCtusInFrame;
	  }
	  break;
	case FIXED_NUMBER_OF_BYTES:
		boundingCtuTsAddrSlice = numberOfCtusInFrame;// This will be adjusted later if required.
		break;
	case FIXED_NUMBER_OF_TILES:
	  {
		const UInt tileIdx = pic->Get_pic_sym()->Get_tile_idx_map(pic->Get_pic_sym()->Get_ctu_ts_to_rs_addr_map(startCtuTsAddrSlice));
		const UInt tileTotalCount = (pic->Get_pic_sym()->Get_num_tile_columns_minus1() + 1) * 
			(pic->Get_pic_sym()->Get_num_tile_rows_minus1() + 1);
		UInt ctuAddrIncrement = 0;

		for (UInt tileIdxIncrement = 0; tileIdxIncrement < sliceArgument; tileIdxIncrement++)
		{
			if ((tileIdx + tileIdxIncrement) < tileTotalCount)
			{
				UInt tileWidthInCtus = pic->Get_pic_sym()->Get_lib_com_tile(tileIdx + tileIdxIncrement)->Get_tile_width_in_ctus();
				UInt tileHeightInCtus = pic->Get_pic_sym()->Get_lib_com_tile(tileIdx + tileIdxIncrement)->Get_tile_height_in_ctus();
				ctuAddrIncrement += (tileWidthInCtus * tileHeightInCtus);

			}
		}
		boundingCtuTsAddrSlice = ((startCtuTsAddrSlice + ctuAddrIncrement) < numberOfCtusInFrame) ? 
			(startCtuTsAddrSlice + ctuAddrIncrement) : numberOfCtusInFrame;

	  }
	  break;
	default:
		boundingCtuTsAddrSlice = numberOfCtusInFrame;
		break;
	}

	if ((sliceMode == FIXED_NUMBER_OF_CTU || sliceMode == FIXED_NUMBER_OF_BYTES) &&
		(pps.Get_num_tile_rows_minus1() > 0 || pps.Get_num_tile_columns_minus1() > 0))
	{
		const UInt ctuRsAddr = pic->Get_pic_sym()->Get_ctu_ts_to_rs_addr_map(startCtuTsAddrSlice);
		const UInt startTileIdx = pic->Get_pic_sym()->Get_tile_idx_map(ctuRsAddr);

		const LibComTile* startingTile = pic->Get_pic_sym()->Get_lib_com_tile(startTileIdx);
		const UInt tileStartTsAddr = pic->Get_pic_sym()->Get_ctu_rs_to_ts_addr_map(startingTile->Get_first_ctu_rs_addr());
		const UInt tileStartWidth = startingTile->Get_tile_width_in_ctus();
		const UInt tileStartHeight = startingTile->Get_tile_height_in_ctus();
		const UInt tileBoundingCtuTsAddrSlice = tileStartTsAddr + tileStartWidth * tileStartHeight;
		const UInt ctuColumnOfStartingTile = ((startCtuTsAddrSlice - tileStartTsAddr) % tileStartWidth);

		if (tileBoundingCtuTsAddrSlice < boundingCtuTsAddrSlice)
		{
			boundingCtuTsAddrSlice = tileBoundingCtuTsAddrSlice;
			haveReachedTileBoundary = true;
		}

	}
}







