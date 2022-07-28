
#include <time.h>


#include "LibEncGOP.h"
#include "LibEncTop.h"



// ====================================================================================================================
// Constructor / destructor / initialization / create / destroy
// ====================================================================================================================

LibEncGOP::LibEncGOP()
{
	m_iLastIDR = 0;
	m_iGOPSize = 0;
	m_pcCfg = NULL;
	m_pcSliceEncoder = NULL;



}


LibEncGOP::~LibEncGOP()
{

}




Void LibEncGOP::Init(LibEncTop* libEncTop)
{
	m_pcCfg = libEncTop;
	m_pcSliceEncoder = libEncTop->Get_slice_encoder();
}



// ====================================================================================================================
// Public member functions
// ====================================================================================================================



/****************************************************************************************
Description: Compress one GOPSize. eg. If GOPSize = 4, 
0 | 1 2 3 4 | 5 6 7 8 | 9 10 11 12 | ...
  |         |         |            |
the POC = 0 is the first GOP, "1 2 3 4" is one GOP, "5 6 7 8" is another one GOP...
Input para: 
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*****************************************************************************************/

Void LibEncGOP::CompressGOP(Int POCLast, Int numPicRcvd, LibComList<LibComPic*>& listPic, LibComList<LibComPicYuv*>& listPicYuvRecOut, 
	                        std::list<LibAccessUnit>& accessUnitsInGOP, const LibEncAnalyze::OutputLogControl& outputLogCtrl)
{
	LibComPic* pic = NULL;
	LibComPicYuv* picYuvRecOut;
	LibComSlice* slice;


	init_GOP(POCLast, numPicRcvd);


	for (Int GOPid = 0; GOPid < m_iGOPSize; GOPid++)
	{
		m_pcCfg->Set_encoded_flag(GOPid, false);
	}


	// Compress each frame in GOPSize.
	for (Int GOPid = 0; GOPid < m_iGOPSize; GOPid++)
	{

		// For time output for each slice.
		clock_t beforeTime = clock();


		Int timeOffset;
		Int pocCurr;

		if (POCLast == 0)
		{
			pocCurr = 0;
			timeOffset = 1;
		} 
		else
		{
			pocCurr = POCLast - numPicRcvd + m_pcCfg->Get_GOP_entry(GOPid).m_iPOC;
			timeOffset = m_pcCfg->Get_GOP_entry(GOPid).m_iPOC;
		}

		if (pocCurr >= m_pcCfg->Get_frames_to_be_encoded())
		{
			continue;
		}

		if (Get_nal_unit_type(pocCurr, m_iLastIDR) == NAL_UNIT_CODED_SLICE_IDR_W_RADL)
		{
			m_iLastIDR = pocCurr;
		}

		// Start a new access unit: create an entry in the list of output access units.
		accessUnitsInGOP.push_back(LibAccessUnit());
		LibAccessUnit& accessUnit = accessUnitsInGOP.back();
		get_buffer(listPic, listPicYuvRecOut, numPicRcvd, timeOffset, pic, picYuvRecOut, pocCurr);


		pic->Prepare_for_reconstruction();

		// Slice data initialization
		pic->Clear_slice_buffer();
		pic->Allocate_new_slice();

		m_pcSliceEncoder->Set_slice_idx(0);
		pic->Set_curr_slice_idx(0);

		m_pcSliceEncoder->Init_enc_slice(pic, POCLast, pocCurr, GOPid, slice);


		slice->Set_nal_unit_type(Get_nal_unit_type(pocCurr, m_iLastIDR));


		// Now compress (trial encode) the various slice segments (slices, and dependent slices).
		{

			const UInt numberOfCtusInFrame = pic->Get_pic_sym()->Get_number_of_ctus_in_frame();
			slice->Set_slice_cur_start_ctu_ts_addr(0);
			slice->Set_slice_segment_cur_start_ctu_ts_addr(0);

			for (UInt nextCtuTsAddr = 0; nextCtuTsAddr < numberOfCtusInFrame;)
			{
				m_pcSliceEncoder->Precompress_slice(pic);
				m_pcSliceEncoder->Compress_slice(pic, false, false);


			}


		}
		



	}// GOPid loop



	
}



/****************************************************************************************
Description: 
Function for deciding the nal_unit_type.
Input para:
\param pocCurr  POC of the current picture
\param lastIDR  POC of the last IDR picture
Return value:
the NAL unit type of the picture
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*****************************************************************************************/
NalUnitType LibEncGOP::Get_nal_unit_type(Int pocCurr, Int lastIDR)
{
	if (pocCurr == 0)
	{
		return NAL_UNIT_CODED_SLICE_IDR_W_RADL;
	}


	return NAL_UNIT_CODED_SLICE_RASL_R;
}





// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

Void LibEncGOP::init_GOP(Int POCLast, Int numPicRcvd)
{
	assert(numPicRcvd > 0);

	if (POCLast == 0)
	{
		m_iGOPSize = 1;
	} 
	else
	{
		m_iGOPSize = m_pcCfg->Get_GOP_size();
	}

	assert(m_iGOPSize > 0);

	return;

}



Void LibEncGOP::get_buffer(LibComList<LibComPic*>& listPic, LibComList<LibComPicYuv*>& listPicYuvRecOut, Int numPicRcvd, Int timeOffset, 
	                       LibComPic*& pic, LibComPicYuv*& picYuvRecOut, Int pocCurr)
{
	Int i;

	// Rec. output.
	LibComList<LibComPicYuv*>::iterator iterPicYuvRec = listPicYuvRecOut.end();

	for (i = 0; i < (numPicRcvd - timeOffset + 1); i++)
	{
		iterPicYuvRec--;
	}

	picYuvRecOut = *(iterPicYuvRec);

	LibComList<LibComPic*>::iterator iterPic = listPic.begin();
	while (iterPic != listPic.end())
	{
		pic = *(iterPic);
		pic->Set_curr_slice_idx(0);
		if (pic->Get_POC() == pocCurr)
		{
			break;
		}
		iterPic++;
	}

	assert(pic != NULL);
	assert(pic->Get_POC() == pocCurr);

	return;


}

