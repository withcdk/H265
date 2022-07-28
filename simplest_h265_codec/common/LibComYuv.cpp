

#include "LibComYuv.h"





LibComYuv::LibComYuv()
{
	for (Int comp = 0; comp < MAX_NUM_COMPONENT; comp++)
	{
		m_apiBuf[comp] = NULL;
	}
}

LibComYuv::~LibComYuv()
{
	Destroy();
}




Void LibComYuv::Create(const UInt width, const UInt height, const ChromaFormat chromaFormatIDC)
{
	Destroy();

	// Set width and height.
	m_uiWidth = width;
	m_uiHeight = height;
	m_eChromaFormatIDC = chromaFormatIDC;
	for (Int comp = 0; comp < MAX_NUM_COMPONENT; comp++)
	{
		// Memory allocation.
		m_apiBuf[comp] = (Pel*)xMalloc(Pel, Get_width(ComponentID(comp)) * Get_height(ComponentID(comp)));
	}

}

Void LibComYuv::Destroy()
{
	// Memory free.
	for (Int comp = 0; comp < MAX_NUM_COMPONENT; comp++)
	{
		if (m_apiBuf[comp] != NULL)
		{
			xFree(m_apiBuf[comp]);
			m_apiBuf[comp] = NULL;
		}
	}
}

Void LibComYuv::Copy_from_pic_yuv(const LibComPicYuv* picYuvSrc, const UInt ctuRsAddr, const UInt absZorderIdx)
{
	for (Int comp = 0; comp < Get_number_valid_components(); comp++)
	{
		Copy_from_pic_component(ComponentID(comp), picYuvSrc, ctuRsAddr, absZorderIdx);
	}
}

Void LibComYuv::Copy_from_pic_component(const ComponentID id, const LibComPicYuv* picYuvSrc, const UInt ctuRsAddr, const UInt absZorderIdx)
{
	Pel* dst = Get_addr(id);
	const Pel* src = picYuvSrc->Get_addr(id, ctuRsAddr, absZorderIdx);
	const UInt dstStride = Get_stride(id);
	const UInt srcStride = picYuvSrc->Get_stride(id);
	const Int width = Get_width(id);
	const Int height = Get_height(id);

	for (Int y = height; y != 0; y--)
	{
		::memcpy(dst, src, sizeof(Pel) * width);
		dst += dstStride;
		src += srcStride;
	}



}

