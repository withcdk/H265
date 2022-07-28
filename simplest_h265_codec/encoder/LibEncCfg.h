


#ifndef _LIB_ENC_CFG_H_
#define _LIB_ENC_CFG_H_

#include "../common/LibComSlice.h"


// ====================================================================================================================
// Struct definition
// ====================================================================================================================
struct GOPEntry 
{

	Int m_iPOC;// cdk: picture order conut in current GOPSize. Come from cfg configuration.
	

	Bool m_bIsEncoded;

	GOPEntry()
	: m_iPOC(-1)
	, m_bIsEncoded(false)
	{}


};


std::istringstream& operator>>(std::istringstream& in, GOPEntry& entry);



// ====================================================================================================================
// Class definition
// ====================================================================================================================

// Encoder configuration class.
class LibEncCfg
{
public:

	LibEncCfg()
	: m_aiTileColumnWidth()
	, m_aiTileRowHeight()
	{}
	virtual ~LibEncCfg()
	{}


	//==== Source specification ========
	Void Set_chroma_format_idc(ChromaFormat cf) { m_ChromaFormatIDC = cf; }
	ChromaFormat  Get_chroma_format_idc() { return m_ChromaFormatIDC; }
	Void Set_source_width(Int i) { m_iSourceWidth = i; }
	Void Set_source_height(Int i) { m_iSourceHeight = i; }
	Void Set_max_CU_width(UInt u) { m_uiMaxCUWidth = u; }
	Void Set_max_CU_height(UInt u) { m_uiMaxCUHeight = u; }
	Void Set_max_total_CU_depth(UInt u) { m_uiMaxTotalCUDepth = u; }
	Void Set_frames_to_be_encoded(Int i) { m_iFramesToBeEncoded = i; }
	Int Get_frames_to_be_encoded() { return m_iFramesToBeEncoded; }

	//====== Profile and Level ========
	Void Set_profile(Profile::Name profile) { m_eProfile = profile; }


	//====== Tiles ========
	Void Set_tile_uniform_spacing_flag(Bool b) { m_bTileUniformSpacingFlag = b; }
	Void Set_num_columns_minus1(Int i) { m_iNumColumnsMinus1 = i; }
	Void Set_num_rows_minus1(Int i) { m_iNumRowsMinus1 = i; }
	Void Set_column_width(const std::vector<Int>& columnWidth) { m_aiTileColumnWidth = columnWidth; }
	Void Set_row_height(const std::vector<Int> & rowHeight) { m_aiTileRowHeight = rowHeight; }

	//====== Slices ========
	Void Set_slice_mode(SliceConstraint i) { m_eSliceMode = i; }
	SliceConstraint Get_slice_mode() const { return m_eSliceMode; }
	Void Set_slice_argument(Int i) { m_iSliceArgument = i; }
	Int Get_slice_argument() { return m_iSliceArgument; }
	Void Set_slice_segment_mode(SliceConstraint i) { m_eSliceSegmentMode = i; }
	SliceConstraint Get_slice_segment_mode() const { return m_eSliceSegmentMode; }
	Void Set_slice_segment_argument(Int i) { m_iSliceSegmentArgument = i; }
	Int Get_slice_segment_argument() { return m_iSliceSegmentArgument; }

	

	
	//==== Coding Structure ========
	Void Set_max_dec_pic_buffering(UInt u, UInt tLayer) { m_iMaxDecPicBuffering[tLayer] = u; }
	Int Get_max_dec_pic_buffering(UInt tLayer) { return m_iMaxDecPicBuffering[tLayer]; }
	Void Set_GOP_size(Int i) { m_iGOPSize = i; }
	Int Get_GOP_size() { return m_iGOPSize; }
	Void Set_GOP_list(const GOPEntry GOPList[MAX_GOP]) { for (Int i = 0; i < MAX_GOP; i++) m_GOPList[i] = GOPList[i]; }
	const GOPEntry& Get_GOP_entry(Int i) const { return m_GOPList[i]; }
	Void Set_encoded_flag(Int i, Bool value) { m_GOPList[i].m_bIsEncoded = value; }


	//====== Quantization ========
	Int Get_QP_for_picture(const UInt gopIndex, const LibComSlice* slice) const;// Function actually defined in LibEncTop.cpp.
    Void Set_QP(Int i) { m_iQP = i; };


	//==== Coding Tools ========
	Void Set_print_msssim(Bool value) { m_bPrintMSSSIM = value; }
	Void Set_print_frame_mse(Bool value) { m_bPrintFrameMSE = value; }
	Void Set_print_sequence_mse(Bool value) { m_bPrintSequenceMSE = value; }
	Void Set_print_mse_based_sequence_psnr(Bool value) { m_bPrintMSEBasedSequencePSNR = value; }


	



protected:

    Int get_base_QP() const { return m_iQP; };


    

	//==== File I/O ========
	ChromaFormat m_ChromaFormatIDC;
	Int m_iFramesToBeEncoded;
	Int m_iSourceWidth;
	Int m_iSourceHeight;
	UInt m_uiMaxCUWidth;
	UInt m_uiMaxCUHeight;
	UInt m_uiMaxTotalCUDepth;


	//==== Profile and Level ========
	Profile::Name m_eProfile;





	//===== Source specification ==========
	Bool m_bPrintMSEBasedSequencePSNR;
	Bool m_bPrintFrameMSE;
	Bool m_bPrintSequenceMSE;
	Bool m_bPrintMSSSIM;

	//====== Tiles ========
	Bool m_bTileUniformSpacingFlag;
	Int m_iNumColumnsMinus1;
	Int m_iNumRowsMinus1;
	std::vector<Int> m_aiTileColumnWidth;
	std::vector<Int> m_aiTileRowHeight;


	//====== Slices ========
	SliceConstraint m_eSliceMode;
	Int m_iSliceArgument;
	SliceConstraint m_eSliceSegmentMode;
	Int m_iSliceSegmentArgument;



	

	//====== Coding Structure ========
	Int m_iGOPSize;
	GOPEntry m_GOPList[MAX_GOP];
	Int m_iMaxDecPicBuffering[MAX_TLAYER];


    //====== Quantization ========
    Int m_iQP;





private:





};




#endif// _LIB_ENC_CFG_H_

