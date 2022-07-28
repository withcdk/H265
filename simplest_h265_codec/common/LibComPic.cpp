

#include "LibComPic.h"




Void LibComPic::Create(const LibComSPS& sps, const LibComPPS& pps, const Bool bCreateEncoderSourcePicYuv, const Bool bCreateForImmediateReconstruction)
{
	Destroy();

	const ChromaFormat chromaFormatIDC = sps.Get_chroma_format_idc();
	const Int iWidth = sps.Get_pic_width_in_luma_samples();
	const Int iHeight = sps.Get_pic_height_in_luma_samples();
	const UInt uiMaxCuWidth = sps.Get_max_cu_width();
	const UInt uiMaxCuHeight = sps.Get_max_cu_height();
	const UInt uiMaxDepth = sps.Get_max_total_cu_depth();

	m_cPicSym.Create(sps, pps, uiMaxDepth, bCreateForImmediateReconstruction);

	if (bCreateEncoderSourcePicYuv)
	{
		m_apcPicYuv[PIC_YUV_ORG] = new LibComPicYuv;
		m_apcPicYuv[PIC_YUV_ORG]->Create(iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, uiMaxDepth, true);
	}
	
	if (bCreateForImmediateReconstruction)
	{
		m_apcPicYuv[PIC_YUV_REC] = new LibComPicYuv;
		m_apcPicYuv[PIC_YUV_REC]->Create(iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, uiMaxDepth, true);
	}

}

Void LibComPic::Release_all_reconstruction_data()
{
	if (m_apcPicYuv[PIC_YUV_REC])
	{
		m_apcPicYuv[PIC_YUV_REC]->Destroy();
		delete m_apcPicYuv[PIC_YUV_REC];
		m_apcPicYuv[PIC_YUV_REC] = NULL;
	}
	m_cPicSym.Release_all_reconstruction_data();
}

Void LibComPic::Prepare_for_encoder_source_pic_yuv()
{
	const LibComSPS& sps = m_cPicSym.Get_SPS();

	const ChromaFormat chromaFormatIDC = sps.Get_chroma_format_idc();
	const Int width = sps.Get_pic_width_in_luma_samples();
	const Int height = sps.Get_pic_height_in_luma_samples();
	const UInt maxCuWidth = sps.Get_max_cu_width();
	const UInt maxCuHeight = sps.Get_max_cu_height();
	const UInt maxDepth = sps.Get_max_total_cu_depth();

	if (m_apcPicYuv[PIC_YUV_ORG] == NULL)
	{
		m_apcPicYuv[PIC_YUV_ORG] = new LibComPicYuv;
		m_apcPicYuv[PIC_YUV_ORG]->Create(width, height, chromaFormatIDC, maxCuWidth, maxCuHeight, maxDepth, true);
	}

}


Void LibComPic::Prepare_for_reconstruction()
{
	if (m_apcPicYuv[PIC_YUV_REC] == NULL)
	{
		const LibComSPS& sps = m_cPicSym.Get_SPS();
		const ChromaFormat chromaFormatIDC = sps.Get_chroma_format_idc();
		const Int width = sps.Get_pic_width_in_luma_samples();
		const Int height = sps.Get_pic_height_in_luma_samples();
		const UInt maxCuWidth = sps.Get_max_cu_width();
		const UInt maxCuHeight = sps.Get_max_cu_height();
		const UInt maxDepth = sps.Get_max_total_cu_depth();

		m_apcPicYuv[PIC_YUV_REC] = new LibComPicYuv;
		m_apcPicYuv[PIC_YUV_REC]->Create(width, height, chromaFormatIDC, maxCuWidth, maxCuHeight, maxDepth, true);

	}

	m_cPicSym.Prepare_for_reconstruction();

}


Void LibComPic::Destroy()
{
	m_cPicSym.Destroy();

	for (UInt i = 0; i < NUM_PIC_YUV; i++)
	{
		if (m_apcPicYuv[i])
		{
			m_apcPicYuv[i]->Destroy();
			delete m_apcPicYuv[i];
			m_apcPicYuv[i] = NULL;
		}
	}

}





