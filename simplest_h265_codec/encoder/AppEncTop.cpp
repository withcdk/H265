
#include <cassert>
#include <fstream>
#include <iostream>
#include <iomanip>// setw()


#include "../common/LibComPicYuv.h"
#include "../common/LibTypeDef.h"


#include "AppEncTop.h"





using namespace std;




// ====================================================================================================================
// Public member functions
// ====================================================================================================================


Void AppEncTop::Encode()
{
	fstream bitstreamFile(m_szBitstreamFileName.c_str(), fstream::binary | fstream::out);
	if (!bitstreamFile)
	{
		fprintf(stderr, "\nfailed to open bitstream file `%s' for writing\n", m_szBitstreamFileName.c_str());// `%s' ¡ª¡ª>  '%s'
		exit(EXIT_FAILURE);
	}

	LibComPicYuv* picYuvOrg = new LibComPicYuv;
	LibComPicYuv* picYuvRec = NULL;


	init_lib_cfg();

	create_lib();

	init_lib();


	print_chroma_format();



	// Main encoder loop.
	Int   numEncoded = 0;
	Bool  eos = false;

	list<LibAccessUnit> outputAccessUnits;// List of access units to write out. is populated by the encoding process.

	// Allocate original YUV buffer.
	picYuvOrg->Create(m_iSourceWidth, m_iSourceHeight, m_ChromaFormatIDC, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxTotalCUDepth, true);


	while (!eos)
	{

		// Allocate reconstructed YUV buffer.
		get_buffer(picYuvRec);


		// Read input YUV file.
		m_cVideoIOYuvInputFile.Read(picYuvOrg, m_InputChromaFormatIDC);

		m_iFrameRcvd++;

		eos = (m_iFrameRcvd == m_iFramesToBeEncoded);

		Bool flush = 0;

		// If end of file (which is only detected on a read failure), flush the encoder of any queued pictures.
		if (m_cVideoIOYuvInputFile.Is_eof())
		{
			flush = true;
			eos = true;
			m_iFrameRcvd--;
			m_cLibEncTop.Set_frames_to_be_encoded(m_iFrameRcvd);
		}


		m_cLibEncTop.Encode(eos, flush ? 0 : picYuvOrg, m_cListPicYuvRec, outputAccessUnits, numEncoded);


		if (numEncoded > 0)
		{
			//write_output();
		}

	}

	//Print_summary();



	
}






// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

/******************************************************************************
Description:
Initialize internal class & member variables.
Input para:
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void AppEncTop::init_lib_cfg()
{

	//==== Source specification ========
	m_cLibEncTop.Set_chroma_format_idc(m_ChromaFormatIDC);
	m_cLibEncTop.Set_source_width(m_iSourceWidth);
	m_cLibEncTop.Set_source_height(m_iSourceHeight);
	m_cLibEncTop.Set_max_CU_width(m_uiMaxCUWidth);
	m_cLibEncTop.Set_max_CU_height(m_uiMaxCUHeight);
	m_cLibEncTop.Set_max_total_CU_depth(m_uiMaxTotalCUDepth);
	m_cLibEncTop.Set_frames_to_be_encoded(m_iFramesToBeEncoded);


	//====== Profile and Level ========
	m_cLibEncTop.Set_profile(m_Profile);


	//====== Slices ========
	m_cLibEncTop.Set_tile_uniform_spacing_flag(m_bTileUniformSpacingFlag);
	m_cLibEncTop.Set_slice_mode(m_eSliceMode);
	m_cLibEncTop.Set_slice_argument(m_iSliceArgument);
	m_cLibEncTop.Set_slice_segment_mode(m_eSliceSegmentMode);
	m_cLibEncTop.Set_slice_segment_argument(m_iSliceSegmentArgument);

	//====== Tiles ========
	m_cLibEncTop.Set_tile_uniform_spacing_flag(m_bTileUniformSpacingFlag);
	m_cLibEncTop.Set_num_columns_minus1(m_iNumTileColumnsMinus1);
	m_cLibEncTop.Set_num_rows_minus1(m_iNumTileRowsMinus1);
	if (!m_bTileUniformSpacingFlag)
	{
		m_cLibEncTop.Set_column_width(m_iTileColumnWidth);
		m_cLibEncTop.Set_row_height(m_iTileRowHeight);
	}


	//====== Coding Structure ========
	m_cLibEncTop.Set_GOP_size(m_iGOPSize);
	m_cLibEncTop.Set_GOP_list(m_GOPList);

	for (Int i = 0; i < MAX_TLAYER; i++)
	{
		m_cLibEncTop.Set_max_dec_pic_buffering(m_iMaxDecPicBuffering[i], i);
	}


    //====== Quantization ========
    m_cLibEncTop.Set_QP(m_iQP);


	//==== Coding Tools ========
	m_cLibEncTop.Set_print_msssim(m_bPrintMSSSIM);
	m_cLibEncTop.Set_print_frame_mse(m_bPrintFrameMSE);
	m_cLibEncTop.Set_print_sequence_mse(m_bPrintSequenceMSE);
	m_cLibEncTop.Set_print_mse_based_sequence_psnr(m_bPrintMSEBasedSequencePSNR);


	




}

/******************************************************************************
Description:
Open original yuv file, including src yuv and rec yuv.
Input para:
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void AppEncTop::create_lib()
{
	// Video I/O
	m_cVideoIOYuvInputFile.Open(m_szInputFileName, false, m_aInputBitDepth, m_aInternalBitDepth);// Read mode.

	if (!m_szReconFileName.empty())
	{
		m_cVideoIOYuvReconFile.Open(m_szReconFileName, true, m_aOutputBitDepth, m_aInternalBitDepth);// Write mode.
	}

	m_cLibEncTop.Create();

}


/******************************************************************************
Description:
Initialize VPS¡¢SPS¡¢PPS and so on.
Input para:
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void AppEncTop::init_lib()
{
	m_cLibEncTop.Init();
}

Void AppEncTop::print_chroma_format()
{
	cout << setw(43) << "Input ChromaFormatIDC = ";
	switch (m_InputChromaFormatIDC)
	{
	case CHROMA_400:  cout << "  4:0:0"; break;
	case CHROMA_420:  cout << "  4:2:0"; break;
	case CHROMA_422:  cout << "  4:2:2"; break;
	case CHROMA_444:  cout << "  4:4:4"; break;
	default:
		cerr << "Invalid";
		exit(1);
	}
	cout << endl;

	cout << setw(43) << "Output (internal) ChromaFormatIDC = ";	
	switch (m_cLibEncTop.Get_chroma_format_idc())
	{
	case CHROMA_400:  cout << "  4:0:0"; break;
	case CHROMA_420:  cout << "  4:2:0"; break;
	case CHROMA_422:  cout << "  4:2:2"; break;
	case CHROMA_444:  cout << "  4:4:4"; break;
	default:
		cerr << "Invalid";
		exit(1);
	}

	cout << "\n" << endl;
}


/******************************************************************************
Description: 
- Application has picture buffer list with size of GOP.
- Picture buffer list acts as ring buffer.
- End of the list has the latest picture.
Input para: 
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void AppEncTop::get_buffer(LibComPicYuv*& picYuvRec)
{
	assert(m_iGOPSize > 0);

	// Org(origin) buffer
	if (m_cListPicYuvRec.size() >= (UInt)m_iGOPSize)
	{
		picYuvRec = m_cListPicYuvRec.Pop_front();
	}
	else
	{
		picYuvRec = new LibComPicYuv;
		picYuvRec->Create(m_iSourceWidth, m_iSourceHeight, m_ChromaFormatIDC, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxTotalCUDepth, true);
	}

	m_cListPicYuvRec.Push_back(picYuvRec);
	

}





// ====================================================================================================================
// Private member functions
// ====================================================================================================================



