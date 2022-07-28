

#ifndef _LIB_COM_PIC_SYM_H_
#define _LIB_COM_PIC_SYM_H_


#include <deque>


#include "LibCommonDef.h"
#include "LibComSlice.h"
#include "LibComDataCU.h"


// ====================================================================================================================
// Class definition
// ====================================================================================================================
class LibComTile
{
public:
	LibComTile();
	virtual ~LibComTile();

	Void Set_tile_width_in_ctus(UInt i) { m_uiTileWidthInCtus = i; }
	UInt Get_tile_width_in_ctus() const { return m_uiTileWidthInCtus; }
	Void Set_tile_height_in_ctus(UInt i) { m_uiTileHeightInCtus = i; }
	UInt Get_tile_height_in_ctus() const { return m_uiTileHeightInCtus; }
	Void Set_right_edge_pos_in_ctus(UInt i) { m_uiRightEdgePosInCtus = i; }
	UInt Get_right_edge_pos_in_ctus() const { return m_uiRightEdgePosInCtus; }
	Void Set_bottom_edge_pos_in_ctus(UInt i) { m_uiBottomEdgePosInCtus = i; }
	UInt Get_bottom_edge_pos_in_ctus() const { return m_uiBottomEdgePosInCtus; }
	Void Set_first_ctu_rs_addr(UInt i) { m_uiFirstCtuRsAddr = i; }
	UInt Get_first_ctu_rs_addr() const { return m_uiFirstCtuRsAddr; }

protected:
private:
	UInt m_uiTileWidthInCtus;
	UInt m_uiTileHeightInCtus;
	UInt m_uiRightEdgePosInCtus;
	UInt m_uiBottomEdgePosInCtus;
	UInt m_uiFirstCtuRsAddr;



};



// Picture symbol class.
class LibComPicSym
{
public:

	LibComPicSym();
	~LibComPicSym();

	Void Create(const LibComSPS& sps, const LibComPPS& pps, UInt uiMaxDepth, const Bool bAllocateCtuArray);

	Void Release_reconstruction_intermediate_data();
	Void Release_all_reconstruction_data();

	Void Destroy();



	
	LibComSlice* Get_slice(UInt i) { return m_apSlices[i]; }
	const LibComSlice* Get_slice(UInt i) const { return m_apSlices[i]; }
	UInt Get_frame_width_in_ctus() const { return m_uiFrameWidthInCtus; }
	UInt Get_frame_height_in_ctus() const { return m_uiFrameHeightInCtus; }
	LibComDataCU* Get_ctu(UInt ctuRsAddr) { return m_ppPictureCtuArray[ctuRsAddr]; }

	const LibComSPS& Get_SPS() const { return m_cSPS; }
	const LibComPPS& Get_PPS() const { return m_cPPS; }


	Void Clear_slice_buffer();
	Void Allocate_new_slice();
	Void Prepare_for_reconstruction();

	UInt Get_number_of_ctus_in_frame() const { return m_uiNumCtusInFrame; }
	Int Get_num_tile_columns_minus1() const { return m_iNumTileColumnsMinus1; }
	Int Get_num_tile_rows_minus1() const { return m_iNumTileRowsMinus1; }

	// Map CTU TS(Tile scan) addr to RS(Raster scan) addr.
	UInt Get_ctu_ts_to_rs_addr_map(Int ctuTsAddr) const 
	{ return *(m_puiCtuTsToRsAddrMap + (ctuTsAddr >= m_uiNumCtusInFrame ? m_uiNumCtusInFrame : ctuTsAddr)); }
	UInt Get_ctu_rs_to_ts_addr_map(Int ctuRsAddr) const
	{ return *(m_puiCtuRsToTsAddrMap + (ctuRsAddr >= m_uiNumCtusInFrame ? m_uiNumCtusInFrame : ctuRsAddr)); }
	UInt Get_tile_idx_map(Int ctuRsAddr) const { return *(m_puiTileIdxMap + ctuRsAddr); }
	LibComTile* Get_lib_com_tile(UInt tileIdx) { return &(m_acTileParameters[tileIdx]); }
	Int Get_num_tiles() const { return (m_iNumTileRowsMinus1 + 1) * (m_iNumTileColumnsMinus1 + 1); }


protected:
	UInt calculate_next_ctu_rs_addr(UInt currCtuRsAddr);

private:
	std::deque<LibComSlice*> m_apSlices;
	LibComSPS m_cSPS;
	LibComPPS m_cPPS;
	LibComDataCU** m_ppPictureCtuArray;// Array of CU data.


	
	UInt m_uiFrameWidthInCtus;
	UInt m_uiFrameHeightInCtus;
	UInt m_uiNumCtusInFrame;

	Int m_iNumTileColumnsMinus1;
	Int m_iNumTileRowsMinus1;
	std::vector<LibComTile> m_acTileParameters;
	UInt* m_puiCtuTsToRsAddrMap;// For a given TS (Tile-Scan; coding order) address, returns the RS (Raster-Scan) address. 
	                            // cf CtbAddrTsToRs in specification.
	UInt* m_puiTileIdxMap;// The map of the tile index relative to CTU raster scan address.
	UInt* m_puiCtuRsToTsAddrMap;// For a given RS (Raster-Scan) address, returns the TS (Tile-Scan; coding order) address. 
	                            // cf CtbAddrRsToTs in specification.


	Void init_tiles();
	Void init_ctu_ts_rs_addr_maps();
	Void set_num_tile_columns_minus1(Int i) { m_iNumTileColumnsMinus1 = i; }
	Void set_num_tile_rows_minus1(Int i) { m_iNumTileRowsMinus1 = i; }
	Void set_ctu_ts_to_rs_addr_map(Int ctuTsAddr, Int ctuRsAddr) { *(m_puiCtuTsToRsAddrMap + ctuTsAddr) = ctuRsAddr; }
	Void set_ctu_rs_to_ts_addr_map(Int ctuRsAddr, Int ctuTsAddr) { *(m_puiCtuRsToTsAddrMap + ctuRsAddr) = ctuTsAddr; }


};






#endif


