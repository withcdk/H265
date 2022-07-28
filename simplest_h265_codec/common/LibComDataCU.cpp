

#include "LibComDataCU.h"
#include "LibComPic.h"



LibComDataCU::LibComDataCU()
{
	m_pcPic = NULL;
	m_pcSlice = NULL;
}



LibComDataCU::~LibComDataCU()
{

}




Void LibComDataCU::Create(ChromaFormat chromaFormatIDC, UInt numPartition, UInt width, UInt height, Bool decSubCu, Int unitSize)
{

}

Void LibComDataCU::Destroy()
{

}




/******************************************************************************
Description:
Initialize top-level CU: create internal buffers and 
set initial values before encoding the CTU.
\param  pic       Picture (LibComPic) class pointer.
\param  ctuRsAddr   CTU address in raster scan order.
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/

Void LibComDataCU::Init_ctu(LibComPic* pic, UInt ctuRsAddr)
{
	const UInt maxCUWidth = pic->Get_pic_sym()->Get_SPS().Get_max_cu_width();
	const UInt maxCUHeight = pic->Get_pic_sym()->Get_SPS().Get_max_cu_height();
	m_pcPic = pic;
	m_pcSlice = pic->Get_slice(pic->Get_curr_slice_idx());

	m_uiCtuRsAddr = ctuRsAddr;


	m_uiAbsZIdxInCtu = 0;



}
