/**
******************************************************************************
*File: LibEncTop.h
*Descriptions: Encoder class.
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-02-14
******************************************************************************
*/

#include "../common/LibComSlice.h"



#include "LibEncTop.h"




// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

// cdkNote: Note the use of parameter initialization tables to initialize class template objects.
LibEncTop::LibEncTop()
: m_cSPSMap(MAX_NUM_SPS)
, m_cPPSMap(MAX_NUM_PPS)
{
	m_iPOCLast = -1;
	m_iNumPicRcvd = 0;
	m_uiNumAllPicCoded = 0;
	m_pppcRDSbacCoder = NULL;

}


LibEncTop::~LibEncTop()
{

}



Void LibEncTop::Create()
{

	m_pppcRDSbacCoder = new LibEncSbac** [m_uiMaxTotalCUDepth + 1];

	m_cCuEncoder.Create(m_uiMaxTotalCUDepth, m_uiMaxCUWidth, m_uiMaxCUHeight, m_ChromaFormatIDC);


	for (Int depth = 0; depth < m_uiMaxTotalCUDepth + 1; depth++)
	{
		m_pppcRDSbacCoder[depth] = new LibEncSbac* [CI_NUM];

		for (Int CIIdx = 0; CIIdx < CI_NUM; CIIdx++)
		{
			m_pppcRDSbacCoder[depth][CIIdx] = new LibEncSbac;
		}

	}


}



Void LibEncTop::Destroy()
{
	Int depth;
	for (depth = 0; depth < m_uiMaxTotalCUDepth + 1; depth++)
	{
		for (Int CIIdx = 0; CIIdx < CI_NUM; CIIdx++)
		{
			delete m_pppcRDSbacCoder[depth][CIIdx];
		}
	}

	for (depth; depth < m_uiMaxTotalCUDepth + 1; depth++)
	{
		delete[] m_pppcRDSbacCoder[depth];
	}

	delete [] m_pppcRDSbacCoder;

}



Void LibEncTop::Init()
{
	LibComSPS& sps0 = *(m_cSPSMap.AllocatePS(0));// NOTE: Implementations that use more than 1 SPS need to be aware of activation issues.
	LibComPPS& pps0 = *(m_cPPSMap.AllocatePS(0));

	init_SPS(sps0);
	init_PPS(pps0, sps0);


	// Initialize processing unit classes.
	m_cGOPEncoder.Init(this);
	m_cSliceEncoder.Init(this);


}



// ====================================================================================================================
// Public member functions
// ====================================================================================================================

/******************************************************************************
Description: 
- Application has picture buffer list with size of GOP + 1
- Picture buffer list acts like as ring buffer
- End of the list has the latest picture
Input para:
\param   flush               cause encoder to encode a partial GOP
\param   picYuvOrg           original YUV picture
Return value:
\retval  listPicYuvRecOut    list of reconstruction YUV pictures
\retval  accessUnitsOut      list of output access units
\retval  numEncoded          number of encoded pictures
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void LibEncTop::Encode(Bool flush, LibComPicYuv* picYuvOrg, LibComList<LibComPicYuv*>& lisPicYuvRecOut, std::list<LibAccessUnit>& accessUnitsOut, Int& numEncoded)
{
	if (picYuvOrg != NULL)
	{
		// Get original YUV.
		LibComPic* picCurr = NULL;

		get_new_pic_buffer(picCurr);
		picYuvOrg->Copy_to_pic(picCurr->Get_pic_yuv_org());

	}


	if ((m_iNumPicRcvd == 0) || (!flush && (m_iPOCLast != 0) && (m_iNumPicRcvd != m_iGOPSize) && (m_iGOPSize != 0)))
	{
		numEncoded = 0;
		return;
	}

	// Compress one GOP.
	m_cGOPEncoder.CompressGOP(m_iPOCLast, m_iNumPicRcvd, m_cListPic, lisPicYuvRecOut, accessUnitsOut, Get_output_log_control());

	numEncoded = m_iNumPicRcvd;
	m_iNumPicRcvd = 0;
	m_uiNumAllPicCoded += numEncoded;


}





// ====================================================================================================================
// Protected member functions
// ====================================================================================================================


/******************************************************************************
Description:
- Application has picture buffer list with size of GOP + 1
- Picture buffer list acts like as ring buffer
- End of the list has the latest picture
Input para:

Return value:
\retval rpcPic     Obtained picture buffer. "rpc": r->return, p->pointer, c->class
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void LibEncTop::get_new_pic_buffer(LibComPic*& rpcPic)
{
	rpcPic = 0;
	
	const LibComPPS* pPPS = m_cPPSMap.Get_first_PS();
	assert(pPPS != 0);
	const LibComPPS& pps = *pPPS;

	const LibComSPS* pSPS = m_cSPSMap.Get_PS(pps.Get_SPS_id());
	assert(pSPS != 0);
	const LibComSPS& sps = *pSPS;


	LibComSlice::sort_pic_list(m_cListPic);

	// Use an entry in the buffered list if the maximum number that need buffering has been reached.
	if (m_cListPic.size() >= (UInt)(m_iGOPSize + Get_max_dec_pic_buffering(MAX_TLAYER - 1) + 2))
	{
		LibComList<LibComPic*>::iterator iterPic = m_cListPic.begin();
		Int size = Int(m_cListPic.size());
		for (Int i = 0; i < size; i++)
		{
			rpcPic = *iterPic;
			if (rpcPic->Get_slice(0)->Is_referenced() == false)
			{
				break;
			}
			iterPic++;
		}

		// If PPS ID is the same, we will assume that it has not changed since it was last used
		// and return the old object.
		if (pps.Get_PPS_id() == rpcPic->Get_pic_sym()->Get_PPS().Get_PPS_id())
		{
			rpcPic->Release_all_reconstruction_data();
			rpcPic->Prepare_for_encoder_source_pic_yuv();
		} 
		else
		{
			// The IDs differ - free up an entry in the list, and then create a new one, as with the case where the max buffering state has not been reached.
			delete rpcPic;
			m_cListPic.erase(iterPic);
			rpcPic = 0;
		}

	}


	if (rpcPic == 0)
	{
		rpcPic = new LibComPic;
		rpcPic->Create(sps, pps, true, false);
		m_cListPic.Push_back(rpcPic);

	}

	rpcPic->Set_recon_mark(false);

	m_iPOCLast++;
	m_iNumPicRcvd++;
	rpcPic->Get_slice(0)->Set_POC(m_iPOCLast);

}




Void LibEncTop::init_SPS(LibComSPS& sps)
{
	ProfileTierLevel& profileTierLevel = *sps.Get_PTL()->Get_general_PTL();
	profileTierLevel.Set_profile_idc(m_eProfile);
	sps.Set_chroma_format_idc(m_ChromaFormatIDC);
	sps.Set_pic_width_in_luma_samples(m_iSourceWidth);
	sps.Set_pic_height_in_luma_samples(m_iSourceHeight);
	sps.Set_max_cu_width(m_uiMaxCUWidth);
	sps.Set_max_cu_height(m_uiMaxCUHeight);
	sps.Set_max_total_cu_depth(m_uiMaxTotalCUDepth);

}

Void LibEncTop::init_PPS(LibComPPS& pps, const LibComSPS& sps)
{

	pps.Set_tiles_enabled_flag((m_iNumColumnsMinus1 > 0 || m_iNumRowsMinus1 > 0));

	init_PPS_for_tiles(pps);

}

Void LibEncTop::init_PPS_for_tiles(LibComPPS& pps)
{
	pps.Set_tile_uniform_spacing_flag(m_bTileUniformSpacingFlag);
	pps.Set_num_tile_columns_minus1(m_iNumColumnsMinus1);
	pps.Set_num_tile_rows_minus1(m_iNumRowsMinus1);
	if (!m_bTileUniformSpacingFlag)
	{
		pps.Set_tile_column_width(m_aiTileColumnWidth);
		pps.Set_tile_row_height(m_aiTileRowHeight);
	}


}

// ====================================================================================================================
// Private member functions
// ====================================================================================================================










// ====================================================================================================================
// Member functions defined in other header files.
// ====================================================================================================================



Int LibEncCfg::Get_QP_for_picture(const UInt gopIndex, const LibComSlice* slice) const
{
    Int qp;
    
    qp = get_base_QP();

    qp = Clip3(0, MAX_QP, qp);

    
	return qp;
}



