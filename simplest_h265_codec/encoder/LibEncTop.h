



#ifndef _LIB_ENC_TOP_H_
#define _LIB_ENC_TOP_H_


#include "../common/LibComPicYuv.h"
#include "../common/LibComList.h"
#include "../common/LibAccessUnit.h"
#include "../common/LibComSlice.h"


#include "LibEncCfg.h"
#include "LibEncGOP.h"




// ====================================================================================================================
// Class definition
// ====================================================================================================================

// Encoder class.
class LibEncTop : public LibEncCfg
{

public:
	LibEncTop();
	virtual ~LibEncTop();

	Void Create();
	Void Destroy();
	Void Init();



	LibEncSlice* Get_slice_encoder() { return &m_cSliceEncoder; }
	LibEncCu* Get_cu_encoder() { return &m_cCuEncoder; }


	LibEncSbac*** Get_rd_sbac_coder() { return m_pppcRDSbacCoder; }



	// Encode several number of pictures until end-of-sequence.
	Void Encode(Bool flush,
				LibComPicYuv* picYuvOrg,
				LibComList<LibComPicYuv*>& lisPicYuvRecOut,
				std::list<LibAccessUnit>& accessUnitsOut,
				Int& numEncoded);

	LibEncAnalyze::OutputLogControl Get_output_log_control() const
	{
		LibEncAnalyze::OutputLogControl outputLogCtrl;
		outputLogCtrl.m_bPrintFrameMSE = m_bPrintFrameMSE;
		outputLogCtrl.m_bPrintMSEBasedSequencePSNR = m_bPrintMSEBasedSequencePSNR;
		outputLogCtrl.m_bPrintSequenceMSE = m_bPrintSequenceMSE;
		outputLogCtrl.m_bPrintMSSSIM = m_bPrintMSSSIM;


		return outputLogCtrl;


	}



protected:
	Void get_new_pic_buffer(LibComPic*& pic);

	Void init_SPS(LibComSPS& sps);// Initialize SPS from encoder options.
	Void init_PPS(LibComPPS& pps, const LibComSPS& sps);// Initialize PPS from encoder options.

	Void init_PPS_for_tiles(LibComPPS& pps);

private:
	Int m_iPOCLast;// Time index(POC).
	Int m_iNumPicRcvd;// Number of received pictures.
	UInt m_uiNumAllPicCoded;// Number of coded pictures.
	LibComList<LibComPic*> m_cListPic;// Dynamic list of pictures.

	// Processing unit.
	LibEncGOP m_cGOPEncoder;// GOP encoder.
	LibEncSlice m_cSliceEncoder;// Slice encoder.
	LibEncCu m_cCuEncoder;// CU encoder.


	// SPS
	ParameterSetMap<LibComSPS> m_cSPSMap;// SPS. This is the base value.

	// PPS
	ParameterSetMap<LibComPPS> m_cPPSMap;// PPS. This is the base value.

	// RD cost computation.
	LibEncSbac*** m_pppcRDSbacCoder;// Temporal storage for RD computation.





};





#endif// _LIB_ENC_TOP_H_



