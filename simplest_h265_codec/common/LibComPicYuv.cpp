
#include <assert.h>


#include "LibComPicYuv.h"


LibComPicYuv::LibComPicYuv()
{
	for (UInt i = 0; i < MAX_NUM_COMPONENT; i++)
	{
		m_apiPicBuf[i] = NULL;
		m_apiPicOrg[i] = NULL;
	}

	for (UInt i = 0; i < MAX_NUM_CHANNEL_TYPE; i++)
	{
		m_apiCtuOffsetInBuffer[i] = 0;
		m_apiSubCuOffsetInBuffer[i] = 0;
	}

	m_bIsBorderExtended = false;

}



Void LibComPicYuv::Destroy()
{
	for (Int comp = 0; comp < MAX_NUM_COMPONENT; comp++)
	{
		m_apiPicOrg[comp] = NULL;
		if (m_apiPicBuf[comp])
		{
			xFree(m_apiPicBuf[comp]);
			m_apiPicBuf[comp] = NULL;
		}
	}

	for (UInt chan = 0; chan < MAX_NUM_CHANNEL_TYPE; chan++)
	{
		if (m_apiCtuOffsetInBuffer[chan])
		{
			delete[] m_apiCtuOffsetInBuffer[chan];
			m_apiCtuOffsetInBuffer[chan] = NULL;
		}

		if (m_apiSubCuOffsetInBuffer[chan])
		{
			delete[] m_apiSubCuOffsetInBuffer[chan];
			m_apiSubCuOffsetInBuffer[chan] = NULL;
		}


	}



}



Void LibComPicYuv::Create_without_CU_info(const Int picWidth,
	                        const Int picHeight,
	                        const ChromaFormat chromaFormatIDC,
	                        const Bool useMargin,
	                        const UInt maxCUWidth,
	                        const UInt maxCUHeight)
{
	Destroy();

	m_iPicWidth = picWidth;
	m_iPicHeight = picHeight;
	m_cChromaFormatIDC = chromaFormatIDC;

	m_iMarginX = (useMargin ? maxCUWidth : 0) + 16;// For 16-byte alignment.
	m_iMarginY = (useMargin ? maxCUHeight : 0) + 16;// Margin for 8-tap filter and infinite padding.
	m_bIsBorderExtended = false;

	// Assign the picture arrays and set up the ptr to the top left of the original picture.
	for (UInt comp = 0; comp < Get_number_valid_components(); comp++)
	{
		const ComponentID ch = ComponentID(comp);

		/**************************************************
		
		            *  *  *  *
					*  +  +  *         "+"！！>Org
					*  +  +  *         "*"！！>Buf
					*  *  *  *         "+ *"！！>Margin
		
		*********************************************************/

		m_apiPicBuf[comp] = (Pel*)xMalloc(Pel, Get_stride(ch) * Get_total_height(ch));
		m_apiPicOrg[comp] = m_apiPicBuf[comp] + (m_iMarginY >> Get_component_scaleY(ch)) * Get_stride(ch) + (m_iMarginX >> Get_component_scaleX(ch));
	}

	// Initialize pointers for unused components to NULL.
	for (UInt comp = Get_number_valid_components(); comp < MAX_NUM_COMPONENT; comp++)
	{
		m_apiPicBuf[comp] = NULL;
		m_apiPicOrg[comp] = NULL;
	}

	for (Int chan = 0; chan < MAX_NUM_CHANNEL_TYPE; chan++)
	{
		m_apiCtuOffsetInBuffer[chan] = NULL;
		m_apiSubCuOffsetInBuffer[chan] = NULL;
	}


}



Void LibComPicYuv::Create(const Int picWidth,
						  const Int picHeight,
	                      const ChromaFormat chromaFormatIDC,
	                      const UInt maxCUWidth,
	                      const UInt maxCUHeight,
	                      const UInt maxCUdepth,
	                      const Bool useMargin)
{

	Create_without_CU_info(picWidth, picHeight, chromaFormatIDC, useMargin, maxCUWidth, maxCUHeight);

	const Int numCuInWidth = m_iPicWidth / maxCUWidth + (m_iPicWidth % maxCUWidth != 0);
	const Int numCuInHeight = m_iPicHeight / maxCUHeight + (m_iPicHeight % maxCUHeight != 0);

	for (Int chan = 0; chan < MAX_NUM_CHANNEL_TYPE; chan++)
	{
		const ChannelType ch = ChannelType(chan);
		const Int ctuHeight = maxCUHeight >> Get_channel_type_scaleY(ch);
		const Int ctuWidth = maxCUWidth >> Get_channel_type_scaleX(ch);
		const Int stride = Get_stride(ch);

		m_apiCtuOffsetInBuffer[chan] = new Int[numCuInWidth * numCuInHeight];

		for (Int cuRow = 0; cuRow < numCuInHeight; cuRow++)
		{
			for (Int cuCol = 0; cuCol < numCuInWidth; cuCol++)
			{
				m_apiCtuOffsetInBuffer[chan][cuRow * numCuInWidth + cuCol] = stride * cuRow * ctuHeight + cuCol * ctuWidth;
			}
		}

		m_apiSubCuOffsetInBuffer[chan] = new Int[(size_t)1 << (2 * maxCUdepth)];
		const Int numSubBlockPartitions = (1 << maxCUdepth);
		const Int minSubBlockHeight = (ctuHeight >> maxCUdepth);
		const Int minSubBlockWidth = (ctuWidth >> maxCUdepth);
		for (Int buRow = 0; buRow < numSubBlockPartitions; buRow++)
		{
			for (Int buCol = 0; buCol < numSubBlockPartitions; buCol++)
			{
				m_apiSubCuOffsetInBuffer[chan][(buRow << maxCUdepth) + buCol] = stride * buRow * minSubBlockHeight + buCol * minSubBlockWidth;
			}
		}

	}

}





/******************************************************************************
Description: Copy picYuvOrg YUV buffer to pcPicYuvDst YUV buffer.
Input para: 
Return value:
pcPicYuvDst->m_apiPicOrg[]
Author��De-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void LibComPicYuv::Copy_to_pic(LibComPicYuv* pcPicYuvDst) const
{
	assert(m_cChromaFormatIDC == pcPicYuvDst->Get_chroma_format());

	for (Int comp = 0; comp < Get_number_valid_components(); comp++)
	{
		const ComponentID compId = ComponentID(comp);
		const Int width = Get_width(compId);
		const Int height = Get_height(compId);
		const Int strideSrc = Get_stride(compId);
		
		assert(pcPicYuvDst->Get_width(compId) == width);
		assert(pcPicYuvDst->Get_height(compId) == height);
		if (strideSrc == pcPicYuvDst->Get_stride(compId))
		{
			::memcpy(pcPicYuvDst->Get_buf(compId), Get_buf(compId), sizeof(Pel) * strideSrc * Get_total_height(compId));
		} 
		else
		{
			const Pel* pSrc = Get_addr(compId);
			Pel* pDest = pcPicYuvDst->Get_addr(compId);
			const UInt strideDest = pcPicYuvDst->Get_stride(compId);

			for (Int y = 0; y < height; y++, pSrc += strideSrc, pDest += strideDest)
			{
				::memcpy(pDest, pSrc, width * sizeof(Pel));
			}
		}
	}
}



