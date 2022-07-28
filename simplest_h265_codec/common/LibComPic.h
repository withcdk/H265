/******************************************************************************
*File: LibComPic.h
*Descriptions: Picture class (header).
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-02-14

******************************************************************************/



#ifndef _LIB_COM_PIC_H_
#define _LIB_COM_PIC_H_

#include "LibCommonDef.h"
#include "LibComPicYuv.h"
#include "LibComPicSym.h"


// Picture class (symbol + YUV buffers)
class LibComPic
{
public:
	typedef enum PicYuv { PIC_YUV_ORG = 0, PIC_YUV_REC = 1, NUM_PIC_YUV = 2 } PicYuv_t;


	Void Create(const LibComSPS& sps, const LibComPPS& pps, const Bool bCreateEncoderSourcePicYuv, const Bool bCreateForImmediateReconstruction);
	Void Release_all_reconstruction_data();
	Void Prepare_for_encoder_source_pic_yuv();
	Void Prepare_for_reconstruction();

	virtual Void Destroy();


	UInt Get_curr_slice_idx() const { return m_uiCurrSliceIdx; }
	Void Set_curr_slice_idx(UInt i) { m_uiCurrSliceIdx = i; }
	Int Get_POC() const { return m_cPicSym.Get_slice(m_uiCurrSliceIdx)->Get_POC(); }
	LibComSlice* Get_slice(Int i) { return m_cPicSym.Get_slice(i); }
	LibComPicSym* Get_pic_sym() { return &m_cPicSym; }
	LibComDataCU* Get_ctu(UInt ctuRsAddr) { return m_cPicSym.Get_ctu(ctuRsAddr); }


	Void Set_recon_mark(Bool b) { m_bReconstructed = b; }


	LibComPicYuv* Get_pic_yuv_org() { return m_apcPicYuv[PIC_YUV_ORG]; }

	Void Clear_slice_buffer() { m_cPicSym.Clear_slice_buffer(); }
	Void Allocate_new_slice() { m_cPicSym.Allocate_new_slice(); }

	UInt Get_number_of_Ctus_in_frame() const { return m_cPicSym.Get_number_of_ctus_in_frame(); }


protected:
private:
	

	LibComPicSym m_cPicSym;// Symbol
	LibComPicYuv* m_apcPicYuv[NUM_PIC_YUV];



	Bool m_bReconstructed;

	UInt m_uiCurrSliceIdx;// Index of current slice.




};





#endif// _LIB_COM_PIC_H_



