/******************************************************************************
*File: LibEncSlice.h
*Descriptions: Slice encoder class (header).
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-03-07

******************************************************************************/


#ifndef _LIB_ENC_SLICE_H_
#define _LIB_ENC_SLICE_H_



#include "../common/LibCommonDef.h"
#include "../common/LibComPic.h"

#include "LibEncCfg.h"
#include "LibEncEntropy.h"
#include "LibEncSbac.h"
#include "LibEncCu.h"



class LibEncTop;


// ====================================================================================================================
// Class definition
// ====================================================================================================================


class LibEncSlice
{
public:
	LibEncSlice();
	virtual ~LibEncSlice();

	Void Init(LibEncTop* encTop);


	Void Set_slice_idx(UInt i) { m_uiSliceIdx = i; }
	UInt Get_slice_idx() { return m_uiSliceIdx; }

	Void Init_enc_slice(LibComPic* pic, const Int pocLast, const Int pocCurr, const Int GOPid, LibComSlice*& slice);



	Void Precompress_slice(LibComPic* pic);// Precompress slice for multi-loop slice-level QP opt.
	Void Compress_slice(LibComPic* pic, const Bool compressEntireSlice, const Bool fastDeltaQP);// Analysis stage of slice.

	Void Determine_start_and_bounding_ctu_ts_addr(UInt& startCtuTsAddr, UInt& boundingCtuTsAddr, LibComPic* pic);




protected:
private:
	// Encoder configuration.
	LibEncCfg* m_pcCfg;// Encoder configuration class.

	// Processing units.
	LibEncCu* m_pcCuEncoder;// CU encoder.



	// Coding tools.
	LibEncEntropy* m_pcEntropyCoder;// Entropy encoder.


	// RD optimization.
	LibEncSbac*** m_pppcRDSbacCoder;// Storage for SBAC-based RD optimization.
	UInt m_uiSliceIdx;








	Void calculate_bounding_ctu_ts_addr_for_slice(UInt& startCtuTsAddrSlice, UInt& boundingCtuTsAddrSlice, 
		                                          Bool& haveReachedTileBoundary, LibComPic* pic, 
		                                          const Int sliceMode, const Int sliceArgument);



};





#endif


