
#ifndef _APP_ENC_CFG_H_
#define _APP_ENC_CFG_H_

#include <sstream>// Declare stringstream type.
#include <istream>
#include <vector>

#include "../common/LibCommonDef.h"


#include "LibEncCfg.h"



// Encoder configuration class.
class AppEncCfg
{
public:

	Void Create();// Create option handling class.
	Void Destroy();// Destroy option handling class.
	Bool Parse_cfg(Int argc, TChar* argv[]);// Parse configuration file to fill member variables.


protected:
	// File I/O
	std::string m_szInputFileName;// Source file name.
	std::string m_szBitstreamFileName;// Output bitstream file.
	std::string m_szReconFileName;// Output reconstruction file.

	// Source specification
	Int m_iSourceWidth;// Source width in pixel.
	Int m_iSourceHeight;// Source height in pixel.
	Int m_iInputFileWidth;// Width of image in input file (this is equivalent to sourceWidth, if sourceWidth is not subsequently altered due to padding).
	Int m_iInputFileHeight;// Height of image in input file (this is equivalent to sourceHeight, if sourceHeight is not subsequently altered due to padding).
	Int m_aInputBitDepth[MAX_NUM_CHANNEL_TYPE];// Bit-depth of input file.
	Int m_aOutputBitDepth[MAX_NUM_CHANNEL_TYPE];// Bit-depth of output file.
	ChromaFormat m_InputChromaFormatIDC;
	ChromaFormat m_ChromaFormatIDC;// Internal chroma format.
	Int m_iFrameRate;// Source frame-rates (Hz).
	Int  m_iFramesToBeEncoded;// Number of encoded frames.
	Bool m_bCabacZeroWordPaddingEnabled;



	// Profile and Level
	Profile::Name m_Profile;
	Level::Name   m_Level;

	// Tiles
	Bool m_bTileUniformSpacingFlag;
	Int m_iNumTileColumnsMinus1;
	Int m_iNumTileRowsMinus1;
	std::vector<Int> m_iTileColumnWidth;
	std::vector<Int> m_iTileRowHeight;
	Bool m_bLFCrossTileBoundaryFlag;

	// Slices
	SliceConstraint m_eSliceMode;
	Int m_iSliceArgument;// Argument according to selected slice mode.
	SliceConstraint m_eSliceSegmentMode;
	Int m_iSliceSegmentArgument;
	Bool m_bLFCrossSliceBoundaryFlag;// 1: Filter across slice boundaries 0: Do not filter across slice boundaries



	// Unit definition parameters
	UInt m_uiMaxCUWidth;
	UInt m_uiMaxCUHeight;
	UInt m_uiMaxCUDepth;
	UInt m_uiMaxTotalCUDepth;// Max. total CU depth - includes depth of transform-block structure.
	UInt m_uiLog2DiffMaxMinCodingBlockSize;// Difference between largest and smallest CU depth.
	UInt m_uiQuadtreeTULog2MaxSize;
	UInt m_uiQuadtreeTULog2MinSize;
	UInt m_uiQuadtreeTUMaxDepthIntra;

	
	// Coding structure paramters
	Int m_iIntraPeriod;// Period of I-slice (random access period).
	Int m_iGOPSize;// GOP size of hierarchical structure.
	GOPEntry m_GOPList[MAX_GOP];// The coding structure entries from the config file.
	Int m_iMaxDecPicBuffering[MAX_TLAYER];// Total number of pictures in the decoded picture buffer


	// Transform
	Bool m_bUseTransformSkip;// Flag for enabling intra transform skipping
	Bool m_bUseTransformSkipFast;// Flag for enabling fast intra transform skipping


	// Quantization parameters
	Int m_iQP;// QP value of key-picture (integer)
	Int m_iMaxDeltaQP;// max. |delta QP|
	Int m_iMaxCuDQPDepth;// Max. depth for a minimum CuDQPSize (0:default)
	UInt m_uiDeltaQpRD;// dQP range for multi-pass slice QP optimization
	Bool m_bUseRDOQ;// Flag for using RD optimized quantization
	Bool m_bUseRDOQTS;// Flag for using RD optimized quantization for transform skip
	Int* m_aidQP;// Array of slice QP values


	// Quantization Matrix
	ScalingListMode m_useScalingListId;// Using quantization matrix
	std::string m_scalingListFileName;// Quantization matrix file name


	// Deblock Filter
	Bool m_bLoopFilterDisable;// Flag for using deblocking filter
	Bool m_bLoopFilterOffsetInPPS;// Offset for deblocking filter in 0 = slice header, 1 = PPS
	Int m_iLoopFilterBetaOffsetDiv2;// Beta offset for deblocking filter
	Int m_iLoopFilterTcOffsetDiv2;// Tc offset for deblocking filter
	Int m_iDeblockingFilterMetric;// Blockiness metric in encoder


	// SAO
	Bool m_bUseSAO;
	Bool m_bSaoCtuBoundary;// SAO parameter estimation using non-deblocked pixels for CTU bottom and right boundary areas


	// PCM
	Bool m_bUsePCM;// Flag for using IPCM
	UInt m_uiPCMLog2MaxSize;// Log2 of maximum PCM block size
	UInt m_uiPCMLog2MinSize;// Log2 of minimum PCM block size
	Bool m_bPCMInputBitDepthFlag;// 0: PCM bit-depth is internal bit-depth. 1: PCM bit-depth is input bit-depth.
	Bool m_bPCMFilterDisableFlag;// PCM filter disable flag


	// WaveFront
	Bool m_bEntropyCodingSyncEnabledFlag;


	// Lossless
	Bool m_bTransquantBypassEnabledFlag;// transquant_bypass_enabled_flag setting in PPS.
	Bool m_bCUTransquantBypassFlagForce;// If transquant_bypass_enabled_flag, then, if true, all CU transquant bypass flags will be set to true.


	// Coding tools
	Bool m_bEnableIntraReferenceSmoothing; // Flag for enabling(default)/disabling intra reference smoothing/filtering
	Bool m_bPrintMSEBasedSequencePSNR;
	Bool m_bPrintFrameMSE;
	Bool m_bPrintSequenceMSE;
	Bool m_bPrintMSSSIM;


	// Misc.
	Int m_aInternalBitDepth[MAX_NUM_CHANNEL_TYPE];// Bit-depth codec operates at (input/output files will be converted).



	// Internal member functions
	Void check_parameter();// Check validity of configuration values.
	Void print_parameter();// Print configuration values.

private:
};// END CLASS DEFINITION TAppEncCfg



#endif// _APP_ENC_CFG_H_



