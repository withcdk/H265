
/**
******************************************************************************
*File: LibComPicYuv.h
*Descriptions: picture YUV buffer class (header)
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-02-07

******************************************************************************
*/

#ifndef _LIB_COM_PIC_YUV_H_
#define _LIB_COM_PIC_YUV_H_


#include "LibCommonDef.h"
#include "LibComChromaFormat.h"
#include "LibComRom.h"


// Picture YUV buffer class.
class LibComPicYuv
{
public:

	LibComPicYuv();



	// ------------------------------------------------------------------------------------------------
	//  Memory management
	// ------------------------------------------------------------------------------------------------
	Void Create(const Int picWidth,
		        const Int picHeight,
				const ChromaFormat chromaFormatIDC,
				const UInt maxCUWidth,// Used for generating offsets to CUs.
				const UInt maxCUHeight,// Used for generating offsets to CUs.
				const UInt maxCUdepth,// Used for generating offsets to CUs.
				const Bool useMargin);// If true, then a margin of maxCUWidth+16 and maxCUHeight+16 is created around the image.

	Void Create_without_CU_info(const Int picWidth,
		                        const Int picHeight,
		                        const ChromaFormat chromaFormatIDC,
		                        const Bool useMargin = false,
		                        const UInt maxCUWidth = 0,// Used for margin only.
		                        const UInt maxCUHeight = 0);// Used for margin only.
	Void Destroy();


	// ------------------------------------------------------------------------------------------------
	//  Get information of picture
	// ------------------------------------------------------------------------------------------------

	ChromaFormat Get_chroma_format() const { return m_cChromaFormatIDC; }

	Int Get_width(const ComponentID id) const { return m_iPicWidth >> Get_component_scaleX(id); }
	Int Get_height(const ComponentID id) const { return m_iPicHeight >> Get_component_scaleY(id); }


	UInt Get_number_valid_components() const { return ::get_number_valid_components(m_cChromaFormatIDC); }

	// Get the stride of picture after scale in X direction.
	Int Get_stride(const ComponentID id) const { return ((m_iPicWidth) + (m_iMarginX << 1)) >> Get_component_scaleX(id); }

	// Get the total height of picture after scale in Y direction.
	Int Get_total_height(const ComponentID id) const { return ((m_iPicHeight)+(m_iMarginY << 1)) >> Get_component_scaleY(id); }

	UInt Get_component_scaleX(const ComponentID id) const { return ::get_component_scaleX(id, m_cChromaFormatIDC); }
	UInt Get_component_scaleY(const ComponentID id) const { return ::get_component_scaleY(id, m_cChromaFormatIDC); }

	UInt Get_channel_type_scaleX(const ChannelType id) const { return ::get_channel_type_scaleX(id, m_cChromaFormatIDC); }
	UInt Get_channel_type_scaleY(const ChannelType id) const { return ::get_channel_type_scaleY(id, m_cChromaFormatIDC); }



	


	// ------------------------------------------------------------------------------------------------
	//  Access function for picture buffer
	// ------------------------------------------------------------------------------------------------

	// Access starting position of original picture.
	Pel* Get_addr(const ComponentID ch) { return m_apiPicOrg[ch]; }
	const Pel* Get_addr(const ComponentID ch) const { return m_apiPicOrg[ch]; }

	const Pel* Get_addr(const ComponentID ch, const Int ctuRsAddr, const Int absZorderIdx) const
	{
		return m_apiPicOrg[ch] + m_apiCtuOffsetInBuffer[ch == 0 ? 0 : 1][ctuRsAddr] + 
			m_apiSubCuOffsetInBuffer[ch == 0 ? 0 : 1][g_auiZscanToRaster[absZorderIdx]];
	}


	// Access starting position of picture buffer with margin.
	Pel* Get_buf(const ComponentID ch) { return m_apiPicBuf[ch]; }
	const Pel* Get_buf(const ComponentID ch) const { return m_apiPicBuf[ch]; }



	// ------------------------------------------------------------------------------------------------
	//  Miscellaneous.
	// ------------------------------------------------------------------------------------------------
	Void Copy_to_pic(LibComPicYuv* pcPicYuvDst) const;





protected:



private:
	// ------------------------------------------------------------------------------------------------
	//  Parameter for general YUV buffer usage
	// ------------------------------------------------------------------------------------------------
	Int m_iPicWidth;// Width of picture in pixels.
	Int m_iPicHeight;// Height of picture in pixels.
	ChromaFormat m_cChromaFormatIDC;// Chroma Format.

	Int m_iMarginX;// Margin of Luma channel (chroma's may be smaller, depending on ratio).
	Int m_iMarginY;// Margin of Luma channel (chroma's may be smaller, depending on ratio).
	Bool m_bIsBorderExtended;

	Int* m_apiCtuOffsetInBuffer[MAX_NUM_CHANNEL_TYPE];// Gives an offset in the buffer for a given CTU (and channel).
	Int* m_apiSubCuOffsetInBuffer[MAX_NUM_CHANNEL_TYPE];// Gives an offset in the buffer for a given sub-CU (and channel), 
	                                                      // relative to start of CTU.


	// ------------------------------------------------------------------------------------------------
	//  YUV buffer
	// ------------------------------------------------------------------------------------------------
	Pel* m_apiPicBuf[MAX_NUM_COMPONENT];// Buffer (including margin). "api": array pointer inter
	Pel* m_apiPicOrg[MAX_NUM_COMPONENT];// m_apiPicBufY + m_iMarginLuma*Get_stride() + m_iMarginLuma, eg.
										/**************************************************

										*  *  *  *
										*  +  +  *         "+"！！>Org
										*  +  +  *         "*"！！>Buf
										*  *  *  *         "+ *"！！>Margin

										*********************************************************/



	Int Get_stride(const ChannelType id) const { return ((m_iPicWidth)+(m_iMarginX << 1)) >> Get_channel_type_scaleX(id); }



};




#endif

