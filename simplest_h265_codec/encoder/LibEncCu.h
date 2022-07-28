
/**
******************************************************************************
*File: LibEncCu.h
*Descriptions: Coding Unit (CU) encoder class (header).
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-04-08

******************************************************************************
*/


#ifndef _LIB_ENC_CU_H_
#define _LIB_ENC_CU_H_


#include "../common/LibCommonDef.h"
#include "../common/LibComDataCU.h"
#include "../common/LibComYuv.h"


// ====================================================================================================================
// Class definition
// ====================================================================================================================


// CU encoder class.
class LibEncCu
{
public:


	// Create internal buffers.
	Void Create(UChar totalDepth, UInt maxWidth, UInt maxHeight, ChromaFormat chromaFormat);


	// Destroy internal buffers.
	Void Destroy();


	// CTU analysis function.
	Void Compress_ctu(LibComDataCU* ctu);




protected:

	Void compress_cu(LibComDataCU*& bestCU, LibComDataCU*& tempCU, const UInt depth, PartSize parentPartSize = NUMBER_OF_PART_SIZES);
	Int compute_QP(LibComDataCU* cu, UInt depth);

private:
	LibComDataCU** m_ppcBestCU;// Best CUs in each depth.
	LibComDataCU** m_ppcTempCU;// Temporary CUs in each depth.
	UChar m_uhTotalDepth;

	LibComYuv** m_ppcOrigYuv;// Original Yuv for each depth.




};







#endif// _LIB_ENC_CU_H_


