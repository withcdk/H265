


#include "../common/LibComPic.h"


#include "LibEncCu.h"






Void LibEncCu::Create(UChar totalDepth, UInt maxWidth, UInt maxHeight, ChromaFormat chromaFormat)
{
	Int i;
	m_uhTotalDepth = totalDepth + 1;
	m_ppcBestCU = new LibComDataCU* [m_uhTotalDepth - 1];
	m_ppcTempCU = new LibComDataCU* [m_uhTotalDepth - 1];

	m_ppcOrigYuv = new LibComYuv* [m_uhTotalDepth - 1];


	UInt numPartitions;
	for (i = 0; i < m_uhTotalDepth - 1; i++)
	{
		numPartitions = 1 << ((m_uhTotalDepth - i - 1) << 1);
		UInt width = maxWidth >> i;
		UInt height = maxHeight >> i;

		m_ppcBestCU[i] = new LibComDataCU;
		m_ppcBestCU[i]->Create(chromaFormat, numPartitions, width, height, false, maxWidth >> (m_uhTotalDepth - 1));
		m_ppcTempCU[i] = new LibComDataCU;
		m_ppcTempCU[i]->Create(chromaFormat, numPartitions, width, height, false, maxWidth >> (m_uhTotalDepth - 1));

		m_ppcOrigYuv[i] = new LibComYuv;
		m_ppcOrigYuv[i]->Create(width, height, chromaFormat);

	}

	// Initialize partition order.
	UInt* tmp = &g_auiZscanToRaster[0];
	Init_zscan_to_raster(m_uhTotalDepth, 1, 0, tmp);
	Init_raster_to_zscan(maxWidth, maxHeight, m_uhTotalDepth);

}




Void LibEncCu::Destroy()
{
	Int i;
	for (i = 0; i < m_uhTotalDepth - 1; i++)
	{
		if (m_ppcBestCU[i])
		{
			m_ppcBestCU[i]->Destroy();
			delete m_ppcBestCU[i];
			m_ppcBestCU[i] = NULL;
		}
		if (m_ppcTempCU[i])
		{
			m_ppcTempCU[i]->Destroy();
			delete m_ppcTempCU[i];
			m_ppcTempCU[i] = NULL;
		}
	}

	if (m_ppcBestCU)
	{
		delete [] m_ppcBestCU;
		m_ppcBestCU = NULL;
	}
	if (m_ppcTempCU)
	{
		delete [] m_ppcTempCU;
		m_ppcTempCU = NULL;
	}

}

Void LibEncCu::Compress_ctu(LibComDataCU* ctu)
{
	// Initialize CU data.
	m_ppcBestCU[0]->Init_ctu(ctu->Get_pic(), ctu->Get_ctu_rs_addr());
	m_ppcTempCU[0]->Init_ctu(ctu->Get_pic(), ctu->Get_ctu_rs_addr());


	compress_cu(m_ppcBestCU[0], m_ppcTempCU[0], 0);

}




// ====================================================================================================================
// Protected member functions
// ====================================================================================================================



Void LibEncCu::compress_cu(LibComDataCU*& bestCU, LibComDataCU*& tempCU, const UInt depth, PartSize parentPartSize /*= NUMBER_OF_PART_SIZES*/)
{

	LibComPic* pic = bestCU->Get_pic();


	// Get original YUV data from picture.
	m_ppcOrigYuv[depth]->Copy_from_pic_yuv(pic->Get_pic_yuv_org(), bestCU->Get_ctu_rs_addr(), bestCU->Get_zorder_idx_in_ctu());


	Int baseQP = compute_QP(bestCU, depth);

    // cdk_coding
	

}



/******************************************************************************
Description: Compute QP for each CU.
Parameter:
\param cu Target CU
\param depth CU depth
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Int LibEncCu::compute_QP(LibComDataCU* cu, UInt depth)
{
	Int baseQP = cu->Get_slice()->Get_slice_QP();


	return Clip3(0, MAX_QP, baseQP);
}
