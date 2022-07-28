
/******************************************************************************
*File: LibComYuv.h
*Descriptions: General YUV buffer class (header).
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-03-07

******************************************************************************/



#ifndef _LIB_COM_YUV_H_
#define _LIB_COM_YUV_H_

#include "LibCommonDef.h"
#include "LibComPicYuv.h"



// ====================================================================================================================
// Class definition
// ====================================================================================================================

// General YUV buffer class.
class LibComYuv
{
public:
	LibComYuv();
	virtual ~LibComYuv();



	// ------------------------------------------------------------------------------------------------------------------
	//  Memory management
	// ------------------------------------------------------------------------------------------------------------------
	Void Create(const UInt width, const UInt height, const ChromaFormat chromaFormatIDC);// Create YUV buffer.
	Void Destroy();// Destroy YUV buffer.



	UInt Get_width(const ComponentID id) const { return m_uiWidth >> Get_component_scaleX(id); }
	UInt Get_height(const ComponentID id) const { return m_uiHeight >> Get_component_scaleY(id); }

	UInt Get_component_scaleX(const ComponentID id) const { return ::get_component_scaleX(id, m_eChromaFormatIDC); }
	UInt Get_component_scaleY(const ComponentID id) const { return ::get_component_scaleY(id, m_eChromaFormatIDC); }
	UInt Get_number_valid_components() const { return ::get_number_valid_components(m_eChromaFormatIDC); }


	// ------------------------------------------------------------------------------------------------------------------
	//  Copy, load, store YUV buffer.
	// ------------------------------------------------------------------------------------------------------------------


	// Copy YUV buffer from picture buffer.
	Void Copy_from_pic_yuv(const LibComPicYuv* picYuvSrc, const UInt ctuRsAddr, const UInt absZorderIdx);
	Void Copy_from_pic_component(const ComponentID id, const LibComPicYuv* picYuvSrc, const UInt ctuRsAddr, const UInt absZorderIdx);


	// ------------------------------------------------------------------------------------------------------------------
	//  Access function for YUV buffer
	// ------------------------------------------------------------------------------------------------------------------
	Pel* Get_addr(const ComponentID id) { return m_apiBuf[id]; }


	// Get stride value of YUV buffer.
	UInt Get_stride(const ComponentID id) const { return m_uiWidth >> Get_component_scaleX(id); }




protected:
private:

	Pel* m_apiBuf[MAX_NUM_COMPONENT];// YUV buffer.



	// ------------------------------------------------------------------------------------------------------------------
	//  Parameter for general YUV buffer usage
	// ------------------------------------------------------------------------------------------------------------------
	UInt m_uiWidth;
	UInt m_uiHeight;
	ChromaFormat m_eChromaFormatIDC;// Chroma Format.




};





#endif// _LIB_COM_YUV_H_




