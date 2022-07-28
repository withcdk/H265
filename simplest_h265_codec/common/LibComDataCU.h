

#ifndef _LIB_COM_DATA_CU_H_
#define _LIB_COM_DATA_CU_H_

#include "LibCommonDef.h"
#include "LibComSlice.h"


// ====================================================================================================================
// Class definition
// ====================================================================================================================

// CU data structure class.
class LibComDataCU
{
public:
	LibComDataCU();
	virtual ~LibComDataCU();

	// -------------------------------------------------------------------------------------------------------------------
	// Create / Destroy / Initialize / Copy
	// -------------------------------------------------------------------------------------------------------------------
	Void Create(ChromaFormat chromaFormatIDC, UInt numPartition, UInt width, UInt height, Bool decSubCu, Int unitSize);


	Void Destroy();

	Void Init_ctu(LibComPic* pic, UInt ctuRsAddr);


	// -------------------------------------------------------------------------------------------------------------------
	// Member functions for CU description.
	// -------------------------------------------------------------------------------------------------------------------
	LibComPic* Get_pic() { return m_pcPic; }
	LibComSlice* Get_slice() { return m_pcSlice; }

	UInt& Get_ctu_rs_addr() { return m_uiCtuRsAddr; }

	UInt Get_zorder_idx_in_ctu() const { return m_uiAbsZIdxInCtu; }




protected:
private:
	LibComPic* m_pcPic;// Picture class pointer.
	LibComSlice* m_pcSlice;// Slice header pointer.

	// -------------------------------------------------------------------------------------------------------------------
	// CU description
	// -------------------------------------------------------------------------------------------------------------------
	UInt m_uiCtuRsAddr;// CTU (also known as LCU) address in a slice (Raster-scan address, as opposed to tile-scan/encoding order).
	UInt m_uiAbsZIdxInCtu;// Absolute address in a CTU. It's Z scan order.

};


#endif// _LIB_COM_DATA_CU_H_

