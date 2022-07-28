
#include "LibComPicSym.h"



// ====================================================================================================================
// Constructor / destructor / initialization / create / destroy
// ====================================================================================================================


LibComTile::LibComTile()
: m_uiTileWidthInCtus(0)
, m_uiTileHeightInCtus(0)
{

}




LibComTile::~LibComTile()
{

}



LibComPicSym::LibComPicSym()
:m_apSlices()
,m_uiNumCtusInFrame(0)
,m_puiCtuTsToRsAddrMap(NULL)
,m_puiTileIdxMap(NULL)
,m_puiCtuRsToTsAddrMap(NULL)
{

}


LibComPicSym::~LibComPicSym()
{
	Destroy();
}



Void LibComPicSym::Create(const LibComSPS& sps, const LibComPPS& pps, UInt uiMaxDepth, const Bool bAllocateCtuArray)
{
	Destroy();

	m_cSPS = sps;
	m_cPPS = pps;

	const Int picWidth = sps.Get_pic_width_in_luma_samples();
	const Int picHeight = sps.Get_pic_height_in_luma_samples();
	const UInt maxCuWidth = sps.Get_max_cu_width();
	const UInt maxCuHeight = sps.Get_max_cu_height();

	m_uiFrameWidthInCtus = (picWidth % maxCuWidth) ? picWidth / maxCuWidth + 1 : picWidth / maxCuWidth;
	m_uiFrameHeightInCtus = (picHeight % maxCuHeight) ? picHeight / maxCuHeight + 1 : picHeight / maxCuHeight;

	m_uiNumCtusInFrame = m_uiFrameWidthInCtus * m_uiFrameHeightInCtus;


	Clear_slice_buffer();
	Allocate_new_slice();
	
	if (bAllocateCtuArray)
	{

		Prepare_for_reconstruction();
	}

	m_puiCtuTsToRsAddrMap = new UInt[m_uiNumCtusInFrame + 1];
	m_puiTileIdxMap = new UInt[m_uiNumCtusInFrame];
	m_puiCtuRsToTsAddrMap = new UInt[m_uiNumCtusInFrame + 1];


	for (UInt i = 0; i < m_uiNumCtusInFrame; i++)
	{
		m_puiCtuTsToRsAddrMap[i] = i;
		m_puiCtuRsToTsAddrMap[i] = i;
	}



	init_tiles();
	init_ctu_ts_rs_addr_maps();


}





Void LibComPicSym::Release_reconstruction_intermediate_data()
{
	if (m_ppPictureCtuArray)
	{
		for (Int i = 0; i < m_uiNumCtusInFrame; i++)
		{
			if (m_ppPictureCtuArray[i])
			{
				m_ppPictureCtuArray[i]->Destroy();
				delete m_ppPictureCtuArray[i];
				m_ppPictureCtuArray[i] = NULL;
			}
		}
		delete[] m_ppPictureCtuArray;
		m_ppPictureCtuArray = NULL;

	}
}

Void LibComPicSym::Release_all_reconstruction_data()
{
	Release_reconstruction_intermediate_data();
}



Void LibComPicSym::Destroy()
{
	Clear_slice_buffer();
	Release_all_reconstruction_data();

}


// ====================================================================================================================
// Public member functions
// ====================================================================================================================




Void LibComPicSym::Clear_slice_buffer()
{
	for (UInt i = 0; i < UInt(m_apSlices.size()); i++)
	{
		delete m_apSlices[i];
	}
	m_apSlices.clear();
}

Void LibComPicSym::Allocate_new_slice()
{
	m_apSlices.push_back(new LibComSlice);
	m_apSlices.back()->Set_PPS(&m_cPPS);
	m_apSlices.back()->Set_SPS(&m_cSPS);

	if (m_apSlices.size() >= 2)
	{
		m_apSlices.back()->Copy_slice_info(m_apSlices[m_apSlices.size() - 2]);
		m_apSlices.back()->Init_slice();
	}

}


Void LibComPicSym::Prepare_for_reconstruction()
{
	if (m_ppPictureCtuArray == NULL)
	{
		m_ppPictureCtuArray = new LibComDataCU* [m_uiNumCtusInFrame];
		for (UInt i = 0; i < m_uiNumCtusInFrame; i++)
		{
			m_ppPictureCtuArray[i] = new LibComDataCU;
		}
	}

}


// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

UInt LibComPicSym::calculate_next_ctu_rs_addr(UInt currCtuRsAddr)
{
	UInt nextCtuRsAddr;

	//Get the tile index for the current CTU.
	const UInt tileIdx = Get_tile_idx_map(currCtuRsAddr);

	// Get the raster scan address for the next CTU.
	if (currCtuRsAddr % m_uiFrameWidthInCtus == Get_lib_com_tile(tileIdx)->Get_right_edge_pos_in_ctus() && 
		currCtuRsAddr / m_uiFrameWidthInCtus == Get_lib_com_tile(tileIdx)->Get_bottom_edge_pos_in_ctus())
	{// The current CTU is the last CTU of the tile.
		if (tileIdx + 1 == Get_num_tiles())
		{
			nextCtuRsAddr = m_uiNumCtusInFrame;
		} 
		else
		{
			nextCtuRsAddr = Get_lib_com_tile(tileIdx + 1)->Get_first_ctu_rs_addr();
		}
	} 
	else
	{// The current CTU is not the last CTU of the tile.
		if (currCtuRsAddr % m_uiFrameWidthInCtus == Get_lib_com_tile(tileIdx)->Get_right_edge_pos_in_ctus())
		{
			nextCtuRsAddr = currCtuRsAddr + m_uiFrameWidthInCtus - Get_lib_com_tile(tileIdx)->Get_tile_width_in_ctus() + 1;
		} 
		else
		{
			nextCtuRsAddr = currCtuRsAddr + 1;
		}
	}

	return nextCtuRsAddr;
}










// ====================================================================================================================
// Private member functions
// ====================================================================================================================

Void LibComPicSym::init_tiles()
{
	set_num_tile_columns_minus1(m_cPPS.Get_num_tile_columns_minus1());
	set_num_tile_rows_minus1(m_cPPS.Get_num_tile_rows_minus1());


	const Int numCols = m_cPPS.Get_num_tile_columns_minus1() + 1;
	const Int numRows = m_cPPS.Get_num_tile_rows_minus1() + 1;
	const Int numTiles = numRows * numCols;

	m_acTileParameters.resize(numTiles);

	if (m_cPPS.Get_tile_uniform_spacing_flag())
	{
		// Set width and height for each (uniform) tile.
		for (Int row = 0; row < numRows; row++)
		{
			for (Int col = 0; col < numCols; col++)
			{
				const Int tileIdx = row * numCols + col;
				m_acTileParameters[tileIdx].Set_tile_width_in_ctus((col + 1) * Get_frame_width_in_ctus() / numCols - (col * Get_frame_width_in_ctus()) / numCols);
				m_acTileParameters[tileIdx].Set_tile_height_in_ctus((row + 1) * Get_frame_height_in_ctus() / numRows - (row * Get_frame_height_in_ctus()) / numRows);

			}
		}
	}
	else
	{
		// Set the width for each tile.
		for (Int row = 0; row < numRows; row++)
		{
			Int cumulativeTileWidth = 0;
			for (Int col = 0; col < Get_num_tile_columns_minus1(); col++)
			{
				m_acTileParameters[row * numCols + col].Set_tile_width_in_ctus(m_cPPS.Get_tile_column_width(col));
				cumulativeTileWidth += m_cPPS.Get_tile_column_width(col);
			}
			m_acTileParameters[row * numCols + Get_num_tile_columns_minus1()].Set_tile_width_in_ctus(Get_frame_width_in_ctus() - cumulativeTileWidth);

		}

		// Set the height for each tile.
		for (Int col = 0; col < numCols; col++)
		{
			Int cumulativeTileHeight = 0;
			for (Int row = 0; row < Get_num_tile_rows_minus1(); row++)
			{
				m_acTileParameters[row * numCols + col].Set_tile_height_in_ctus(m_cPPS.Get_tile_row_height(row));
				cumulativeTileHeight += m_cPPS.Get_tile_row_height(row);
			}
			m_acTileParameters[Get_num_tile_rows_minus1() * numCols + col].Set_tile_height_in_ctus(Get_frame_height_in_ctus() - cumulativeTileHeight);
		}

	}


	// Tile size check.
	Int minWidth = 1;
	Int minHeight = 1;
	const Int profileIdc = m_cSPS.Get_PTL()->Get_general_PTL()->Get_profile_idc();
	if (profileIdc == Profile::MAIN || profileIdc == Profile::MAIN10)
	{
		if (m_cPPS.Get_tiles_enabled_flag())
		{
			minHeight = 64 / m_cSPS.Get_max_cu_height();// cdk: The minimum number of CTUs that each Tile must contain vertically.
			minWidth = 256 / m_cSPS.Get_max_cu_width();// cdk: The minimum number of CTUs that each Tile must contain horizontally.
		}
	}

	for (Int row = 0; row < numRows; row++)
	{
		for (Int col = 0; col < numCols; col++)
		{
			const Int tileIdx = row * numCols + col;
			assert(m_acTileParameters[tileIdx].Get_tile_width_in_ctus() >= minWidth);
			assert(m_acTileParameters[tileIdx].Get_tile_height_in_ctus() >= minHeight);
		}
	}

	
	// Initialize each tile of the current picture.
	for (Int row = 0; row < numRows; row++)
	{
		for (Int col = 0; col < numCols; col++)
		{
			const Int tileIdx = row * numCols + col;

			// Initialize the RightEdgePosInCTU for each tile.
			Int rightEdgePosInCTU = 0;
			for (Int i = 0; i <= col; i++)
			{
				rightEdgePosInCTU += m_acTileParameters[row * numCols + i].Get_tile_width_in_ctus();
			}
			m_acTileParameters[tileIdx].Set_right_edge_pos_in_ctus(rightEdgePosInCTU - 1);

			// Initialize the BottomEdgePosInCTU for each tile.
			Int bottomEdgePosInCTU = 0;
			for (Int i = 0; i <= row; i++)
			{
				bottomEdgePosInCTU += m_acTileParameters[i * numCols + col].Get_tile_height_in_ctus();
			}
			m_acTileParameters[tileIdx].Set_bottom_edge_pos_in_ctus(bottomEdgePosInCTU - 1);

			// initialize the FirstCUAddr for each tile
			m_acTileParameters[tileIdx].Set_first_ctu_rs_addr((m_acTileParameters[tileIdx].Get_bottom_edge_pos_in_ctus() -
				m_acTileParameters[tileIdx].Get_tile_height_in_ctus() + 1) * Get_frame_width_in_ctus() +
				m_acTileParameters[tileIdx].Get_right_edge_pos_in_ctus() -
				m_acTileParameters[tileIdx].Get_tile_width_in_ctus() + 1);

		}
	}

	Int columnIdx = 0;
	Int rowIdx = 0;

	// Initialize the TileIdxMap.
	for (Int i = 0; i < m_uiNumCtusInFrame; i++)
	{
		for (Int col = 0; col < numCols; col++)
		{
			if (i % Get_frame_width_in_ctus() <= m_acTileParameters[col].Get_right_edge_pos_in_ctus())
			{
				columnIdx = col;
				break;
			}
		}
		for (Int row = 0; row < numRows; row++)
		{
			if (i / Get_frame_width_in_ctus() <= m_acTileParameters[row * numCols].Get_bottom_edge_pos_in_ctus())
			{
				rowIdx = row;
				break;
			}
		}
		m_puiTileIdxMap[i] = rowIdx * numCols + columnIdx;
	}
}

Void LibComPicSym::init_ctu_ts_rs_addr_maps()
{
	for (Int ctuTsAddr = 0, ctuRsAddr = 0; ctuTsAddr < Get_number_of_ctus_in_frame(); ctuTsAddr++, 
		ctuRsAddr = calculate_next_ctu_rs_addr(ctuRsAddr))
	{
		set_ctu_ts_to_rs_addr_map(ctuTsAddr, ctuRsAddr);
		set_ctu_rs_to_ts_addr_map(ctuRsAddr, ctuTsAddr);
	}
	set_ctu_ts_to_rs_addr_map(Get_number_of_ctus_in_frame(), Get_number_of_ctus_in_frame());
	set_ctu_rs_to_ts_addr_map(Get_number_of_ctus_in_frame(), Get_number_of_ctus_in_frame());

}
