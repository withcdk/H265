
#include <cassert>


#include "../common/AppProgramOptionsLite.h"
#include "../common/LibComSlice.h"
#include "../common/LibComChromaFormat.h"


#include "AppEncCfg.h"


using namespace std;// string
namespace po = df::program_options_lite;



// ====================================================================================================================
// Constructor / destructor / initialization / destroy
// ====================================================================================================================

Void AppEncCfg::Create()
{

}
Void AppEncCfg::Destroy()
{

}




// ====================================================================================================================
// Enum¡¢struct
// ====================================================================================================================
enum UIProfileName
{
	UI_NONE = 0,
	UI_MAIN = 1,
	UI_MAIN10 = 2,
	UI_MAINREXT = 3
};


static const struct MapStrToUIProfileName
{
	const TChar* str;
	UIProfileName value;
}
strToUIProfileName[] =
{

	{ "none",                      UI_NONE },
	{ "main",                      UI_MAIN },
	{ "main10",                    UI_MAIN10 },
	{ "main-RExt",                 UI_MAINREXT }
};


static const struct MapStrToProfile
{
	const TChar* str;
	Profile::Name value;
}
strToProfile[] =
{
	{ "none",                 Profile::NONE },
	{ "main",                 Profile::MAIN },
	{ "main10",               Profile::MAIN10 },
	{ "main-RExt",            Profile::MAINREXT }

};


static const struct MapStrToLevel
{
	const TChar* str;
	Level::Name value;
}
strToLevel[] =
{
	{ "none",Level::NONE },
	{ "1",   Level::LEVEL1 },
	{ "2",   Level::LEVEL2 },
	{ "2.1", Level::LEVEL2_1 },
	{ "3",   Level::LEVEL3 },
	{ "3.1", Level::LEVEL3_1 },
	{ "4",   Level::LEVEL4 },
	{ "4.1", Level::LEVEL4_1 },
	{ "5",   Level::LEVEL5 },
	{ "5.1", Level::LEVEL5_1 },
	{ "5.2", Level::LEVEL5_2 },
	{ "6",   Level::LEVEL6 },
	{ "6.1", Level::LEVEL6_1 },
	{ "6.2", Level::LEVEL6_2 },
	{ "8.5", Level::LEVEL8_5 }
};


static const struct MapStrToScalingListMode
{
	const TChar* str;
	ScalingListMode value;
}
strToScalingListMode[] =
{
	{ "0",       SCALING_LIST_OFF },
	{ "1",       SCALING_LIST_DEFAULT },
	{ "2",       SCALING_LIST_FILE_READ },
	{ "off",     SCALING_LIST_OFF },
	{ "default", SCALING_LIST_DEFAULT },
	{ "file",    SCALING_LIST_FILE_READ }
};






// ====================================================================================================================
// The definition of the function to be called by this .cpp file
// ====================================================================================================================

static inline ChromaFormat Number_to_chroma_format(const Int val)
{
	switch (val)
	{
	case 400: return CHROMA_400; break;
	case 420: return CHROMA_420; break;
	case 422: return CHROMA_422; break;
	case 444: return CHROMA_444; break;
	default:  return NUM_CHROMA_FORMAT;
	}
}

Bool Confirm_para(Bool bFlag, const TChar* message)
{
	if (!bFlag)
	{
		return false;
	}

	printf("Error: %s\n", message);
	return true;
}


const TChar* Profile_to_string(const Profile::Name profile)
{
	static const UInt numberOfProfiles = sizeof(strToProfile) / sizeof(*strToProfile);
	
	for (UInt profileIndex = 0; profileIndex < numberOfProfiles; profileIndex++)
	{
		if (strToProfile[profileIndex].value == profile)
		{
			return strToProfile[profileIndex].str;
		}
	}
	

	// If we get here, we didn't find this profile in the list - so there is an error.
	std::cerr << "ERROR: Unknown profile \"" << profile << "\" in Profile_to_string" << std::endl;
	assert(false);
	exit(1);
	return "";

}





// ====================================================================================================================
// The definition of the function to be called by other .cpp file
// ====================================================================================================================


template<typename T, typename P>
static istream& Read_str_to_enum(P map[], UInt mapLen, istream& in, T& val)
{
	string str;
	in >> str;

	UInt i = 0;
	for (; i < mapLen && str != map[i].str; i++);

	if (i < mapLen)
	{
		val = map[i].value;
	}
	else
	{
		in.setstate(ios::failbit);
	}
	return in;
}

// Inline to prevent compiler warnings for "unused static function".
// CdkNote: learn "static inline" the way to write.
static inline istream& operator>>(istream& in, UIProfileName& profile)
{
	return Read_str_to_enum(strToUIProfileName, sizeof(strToUIProfileName) / sizeof(*strToUIProfileName), in, profile);
}

// "namespace Level" limit scope.
namespace Level
{
	static inline istream& operator>>(istream& in, Name& level)
	{
		return Read_str_to_enum(strToLevel, sizeof(strToLevel) / sizeof(*strToLevel), in, level);
	}
}



template <class T>
struct SMultiValueInput
{
	const T m_MinValIncl;
	const T m_MaxValIncl;
	const std::size_t m_MinNumValuesIncl;
	const std::size_t m_MaxNumValuesIncl;
	std::vector<T> values;

	SMultiValueInput() : m_MinValIncl(0), m_MaxValIncl(0), m_MinNumValuesIncl(0), m_MaxNumValuesIncl(0), values() { }
	SMultiValueInput(const T &minValue, const T &maxValue, std::size_t minNumberValues = 0, std::size_t maxNumberValues = 0)
		: m_MinValIncl(minValue), m_MaxValIncl(maxValue), m_MinNumValuesIncl(minNumberValues), m_MaxNumValuesIncl(maxNumberValues), values() { }
	
	SMultiValueInput<T>& operator=(const SMultiValueInput<T>& userValues) { values = userValues.values; return *this; }

	T Read_value(const TChar*& pStr, Bool& bSuccess);// "*" and "&" priority: same level, and right to left

	istream& Read_values(std::istream& in);
};

template<>
UInt SMultiValueInput<UInt>::Read_value(const TChar*& pStr, Bool& bSuccess)
{
	TChar *endPtr;
	UInt val = strtol(pStr, &endPtr, 0);
	pStr = endPtr;
	bSuccess = !(*endPtr != 0 && !isspace(*endPtr) && *endPtr != ',') && !(val < m_MinValIncl || val > m_MaxValIncl);
	return val;

}

template <class T>
istream& SMultiValueInput<T>::Read_values(std::istream& in)
{
	values.clear();
	string str;
	while (!in.eof())
	{
		string tmp; in >> tmp; str += " " + tmp;
	}
	if (!str.empty())
	{
		const TChar *pStr = str.c_str();// c_str() return string address.
		// Soak up any whitespace.
		for (; isspace(*pStr); pStr++);

		while (*pStr != 0)
		{
			Bool bSuccess = true;
			T val = Read_value(pStr, bSuccess);
			if (!bSuccess)
			{
				in.setstate(ios::failbit);
				break;
			}
			if (m_MaxNumValuesIncl != 0 && values.size() >= m_MaxNumValuesIncl)
			{
				in.setstate(ios::failbit);
				break;
			}
			values.push_back(val);

			// Soak up any whitespace and up to 1 comma.
			for (; isspace(*pStr); pStr++);
			if (*pStr == ',')
			{
				pStr++;
			}
			for (; isspace(*pStr); pStr++);
		}

	}
	if (values.size() < m_MinNumValuesIncl)
	{
		in.setstate(ios::failbit);
	}

	return in;
}

template <class T>
static inline istream& operator>>(std::istream& in, SMultiValueInput<T>& values)
{
	return values.Read_values(in);
}



static inline istream& operator>>(istream& in, ScalingListMode& mode)
{
	return Read_str_to_enum(strToScalingListMode, sizeof(strToScalingListMode) / sizeof(*strToScalingListMode), in, mode);
}

/******************************************************************************
Description: 
Deal with "opts.Add_options()(oss.str(), m_GOPList[i - 1], GOPEntry());" 
Input para:
Return value: 
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
std::istringstream& operator>>(std::istringstream& in, GOPEntry& entry)
{
	in >> entry.m_iPOC;
	return in;
}


// ====================================================================================================================
// Public member functions
// ====================================================================================================================


/******************************************************************************
Description: Parse configuration file to fill member variables.
Input para:
-argc: number of arguments
-argv: array of arguments
Return value: True when success.
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Bool AppEncCfg::Parse_cfg(Int argc, TChar* argv[])
{

	Bool doHelp = false;
	Int tmpInputChromaFormat;
	Int tmpSliceMode;
	Int tmpSliceSegmentMode;
	Int tmpChromaFormat;
	UIProfileName UIProfile;

	// Multi-value input fields: minval, maxval (incl), min_entries, max_entries (incl) [, default values, number of default values]
	SMultiValueInput<UInt> cfgColumnWidth(0, std::numeric_limits<UInt>::max(), 0, std::numeric_limits<UInt>::max());
	SMultiValueInput<UInt> cfgRowHeight(0, std::numeric_limits<UInt>::max(), 0, std::numeric_limits<UInt>::max());



	Int warnUnknownParameter = 0;
	po::Options opts;
	opts.Add_options()
	("help", doHelp, false, "this help text")
	("c", po::Parse_config_file, "configuration file name")
	("WarnUnknowParameter,w", warnUnknownParameter, 0, "warn for unknown configuration parameters instead of failing")

	// File, I/O and source parameters
	("InputFile,i", m_szInputFileName, string(""), "Original YUV input file name")
	("BitstreamFile,b", m_szBitstreamFileName, string(""), "Bitstream output file name")
	("ReconFile,o", m_szReconFileName, string(""), "Reconstructed YUV output file name")

	// Source specification
	("SourceWidth,-wdt", m_iSourceWidth, 0, "Source picture width")
	("SourceHeight,-hgt", m_iSourceHeight, 0, "Source picture height")
	("InputBitDepth", m_aInputBitDepth[CHANNEL_TYPE_LUMA], 8, "Bit-depth of input file")
	("InputBitDepthC", m_aInputBitDepth[CHANNEL_TYPE_CHROMA], 0, "As per InputBitDepth but for chroma component. (default:InputBitDepth)")
	("OutputBitDepth", m_aOutputBitDepth[CHANNEL_TYPE_LUMA], 0, "Bit-depth of output file (default:InternalBitDepth)")
	("OutputBitDepthC", m_aOutputBitDepth[CHANNEL_TYPE_CHROMA], 0, "As per OutputBitDepth but for chroma component. (default:InternalBitDepthC)")
	("InputChromaFormat", tmpInputChromaFormat, 420, "InputChromaFormatIDC")
	("ChromaFormatIDC,-cf", tmpChromaFormat, 0, "ChromaFormatIDC (400|420|422|444 or set 0 (default) for same as InputChromaFormat)")
	("FrameRate,-fr", m_iFrameRate, 0, "Frame rate")
	("FramesToBeEncoded,f", m_iFramesToBeEncoded, 0, "Number of frames to be encoded (default=all)")
	("MSEBasedSequencePSNR", m_bPrintMSEBasedSequencePSNR, false, "0 (default) emit sequence PSNR only as a linear average of the frame PSNRs, 1 = also emit a sequence PSNR based on an average of the frame MSEs")
	("PrintFrameMSE", m_bPrintFrameMSE, false, "0 (default) emit only bit count and PSNRs for each frame, 1 = also emit MSE values")
	("PrintSequenceMSE", m_bPrintSequenceMSE, false, "0 (default) emit only bit rate and PSNRs for the whole sequence, 1 = also emit MSE values")
	("PrintMSSSIM", m_bPrintMSSSIM, false, "0 (default) do not print MS-SSIM scores, 1 = print MS-SSIM scores for each frame and for the whole sequence")
	("CabacZeroWordPaddingEnabled", m_bCabacZeroWordPaddingEnabled, true, "0 do not add conforming cabac-zero-words to bit streams, 1 (default) = add cabac-zero-words as required")



	// Profile and Level
	("Profile", UIProfile, UI_NONE, "Profile name to use for encoding. Use main (for main), main10 (for main10), main-still-picture, main-RExt (for Range Extensions profile), any of the RExt specific profile names, or none")
	("Level", m_Level, Level::NONE, "Level limit to be used, eg 5.1, or none")


	// Tiles
	("TileUniformSpacing", m_bTileUniformSpacingFlag, false, "Indicates that tile columns and rows are distributed uniformly")
	("NumTileColumnsMinus1", m_iNumTileColumnsMinus1, 0, "Number of tile columns in a picture minus 1")
	("NumTileRowsMinus1", m_iNumTileRowsMinus1, 0, "Number of rows in a picture minus 1")
	("TileColumnWidthArray", cfgColumnWidth, cfgColumnWidth, "Array containing tile column width values in units of CTU")
	("TileRowHeightArray", cfgRowHeight, cfgRowHeight, "Array containing tile row height values in units of CTU")
	("LFCrossTileBoundaryFlag", m_bLFCrossTileBoundaryFlag, true, "1: cross-tile-boundary loop filtering. 0:non-cross-tile-boundary loop filtering")

	
	// Slices
	("SliceMode", tmpSliceMode, Int(NO_SLICES), "0: Disable all Recon slice limits, 1: Enforce max # of CTUs, 2: Enforce max # of bytes, 3:specify tiles per dependent slice")
	("SliceArgument", m_iSliceArgument, 0, "Depending on SliceMode being:"
										  "\t1: max number of CTUs per slice"
										  "\t2: max number of bytes per slice"
										  "\t3: max number of tiles per slice")
	("SliceSegmentMode", tmpSliceSegmentMode, Int(NO_SLICES), "0: Disable all slice segment limits, 1: Enforce max # of CTUs, 2: Enforce max # of bytes, 3:specify tiles per dependent slice")
	("SliceSegmentArgument", m_iSliceSegmentArgument, 0, "Depending on SliceSegmentMode being:"
										  "\t1: max number of CTUs per slice segment"
										  "\t2: max number of bytes per slice segment"
										  "\t3: max number of tiles per slice segment")
	("LFCrossSliceBoundaryFlag", m_bLFCrossSliceBoundaryFlag, true)


	// Unit definition parameters
	("MaxCUWidth", m_uiMaxCUWidth, 64u)
	("MaxCUHeight", m_uiMaxCUHeight, 64u)
	("MaxPartitionDepth,h", m_uiMaxCUDepth, 4u, "CU depth")
	("QuadtreeTULog2MaxSize", m_uiQuadtreeTULog2MaxSize, 6u, "Maximum TU size in logarithm base 2")
	("QuadtreeTULog2MinSize", m_uiQuadtreeTULog2MinSize, 2u, "Minimum TU size in logarithm base 2")
	("QuadtreeTUMaxDepthIntra", m_uiQuadtreeTUMaxDepthIntra, 1u, "Depth of TU tree for intra CUs")


	// Coding structure paramters
	("IntraPeriod,-ip", m_iIntraPeriod, -1, "Intra period in frames, (-1: only first frame)")
	("GOPSize,g", m_iGOPSize, 1, "GOP size of temporal structure")



	// Transform
	("TransformSkip", m_bUseTransformSkip, false, "Intra transform skipping")
	("TransformSkipFast", m_bUseTransformSkipFast, false, "Fast intra transform skipping")

	
	// Quantization parameters
	("QP,q", m_iQP, 30, "Qp value")
	("MaxDeltaQP,d", m_iMaxDeltaQP, 0, "max dQp offset for block")
	("MaxCuDQPDepth,-dqd", m_iMaxCuDQPDepth, 0, "max depth for a minimum CuDQP")
	("DeltaQpRD,-dqr", m_uiDeltaQpRD, 0u, "max dQp offset for slice")
	("RDOQ", m_bUseRDOQ, true)
	("RDOQTS", m_bUseRDOQTS, true)


	// Quantization Matrix
	("ScalingList", m_useScalingListId, SCALING_LIST_OFF, "0/off: no scaling list, 1/default: default scaling lists, 2/file: scaling lists specified in ScalingListFile")
	("ScalingListFile", m_scalingListFileName, string(""), "Scaling list file name. Use an empty string to produce help.")


	// Deblock Filter
	("LoopFilterDisable", m_bLoopFilterDisable, false)
	("LoopFilterOffsetInPPS", m_bLoopFilterOffsetInPPS, true)
	("LoopFilterBetaOffset_div2", m_iLoopFilterBetaOffsetDiv2, 0)
	("LoopFilterTcOffset_div2", m_iLoopFilterTcOffsetDiv2, 0)
	("DeblockingFilterMetric", m_iDeblockingFilterMetric, 0)

	// SAO
	("SAO", m_bUseSAO, true, "Enable Sample Adaptive Offset")
	("SAOLcuBoundary", m_bSaoCtuBoundary, false, "0: right/bottom CTU boundary areas skipped from SAO parameter estimation, 1: non-deblocked pixels are used for those areas")

	// PCM
	("PCMEnabledFlag", m_bUsePCM, false)
	("PCMLog2MaxSize", m_uiPCMLog2MaxSize, 5u)
	("PCMLog2MinSize", m_uiPCMLog2MinSize, 3u)
	("PCMInputBitDepthFlag", m_bPCMInputBitDepthFlag, true)
	("PCMFilterDisableFlag", m_bPCMFilterDisableFlag, false)


	// WaveFront
	("WaveFrontSynchro", m_bEntropyCodingSyncEnabledFlag, false, "0: entropy coding sync disabled; 1 entropy coding sync enabled")


	// Lossless
	("TransquantBypassEnable", m_bTransquantBypassEnabledFlag, false, "transquant_bypass_enabled_flag indicator in PPS")
	("CUTransquantBypassFlagForce", m_bCUTransquantBypassFlagForce, false, "Force transquant bypass mode, when transquant_bypass_enabled_flag is enabled")


	// Coding tools
	("IntraReferenceSmoothing", m_bEnableIntraReferenceSmoothing, true, "0: Disable use of intra reference smoothing (not valid in V1 profiles). 1: Enable use of intra reference smoothing (same as V1)")


	// Misc.
	("InternalBitDepth", m_aInternalBitDepth[CHANNEL_TYPE_LUMA], 0, "Bit-depth the codec operates at. (default:MSBExtendedBitDepth). If different to MSBExtendedBitDepth, source data will be converted")


	;


	for (Int i = 1; i < MAX_GOP + 1; i++)
	{
		std::ostringstream oss;
		oss << "Frame" << i;
		opts.Add_options()(oss.str(), m_GOPList[i - 1], GOPEntry());
	}



	po::Set_defaults(opts);
	po::ErrorReporter err;
	const list<const TChar*>& argvUnhandled = po::Scan_argv(opts, argc, (const TChar**)argv, err);
	for (list<const TChar*>::const_iterator it = argvUnhandled.begin(); it != argvUnhandled.end(); it++)
	{
		fprintf(stderr, "Unhandled argument ignored: '%s'\n", *it);
	}

	if (argc == 1 || doHelp)
	{
		// argc == 1: no options have been specified.
		po::Do_help(cout, opts);
		return false;
	}

	if (err.m_bIsErrored)
	{
		if (!warnUnknownParameter)
		{
			// Error report has already been printed on stderr.
			return false;
		}
	}

	// Set any derived parameters.
	m_iInputFileWidth = m_iSourceWidth;
	m_iInputFileHeight = m_iSourceHeight;



	if (!m_bTileUniformSpacingFlag && m_iNumTileColumnsMinus1)
	{
		if (cfgColumnWidth.values.size() > m_iNumTileColumnsMinus1)
		{
			printf("The number of columns whose width are defined is larger than the allowed number of columns.\n");
			exit(EXIT_FAILURE);
		}
		else if (cfgColumnWidth.values.size() < m_iNumTileColumnsMinus1)
		{
			printf("The width of some columns is not defined.\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			m_iTileColumnWidth.resize(m_iNumTileColumnsMinus1);
			for (UInt i = 0; i < cfgColumnWidth.values.size(); i++)
			{
				m_iTileColumnWidth[i] = cfgColumnWidth.values[i];
			}
		}
	}
	else
	{
		m_iTileColumnWidth.clear();
	}


	if (!m_bTileUniformSpacingFlag && m_iNumTileRowsMinus1)
	{
		if (cfgRowHeight.values.size() > m_iNumTileRowsMinus1)
		{
			printf("The number of rows whose height are defined is larger than the allowed number of rows.\n");
			exit(EXIT_FAILURE);
		}
		else if (cfgRowHeight.values.size() < m_iNumTileRowsMinus1)
		{
			printf("The height of some rows is not defined.\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			m_iTileRowHeight.resize(m_iNumTileRowsMinus1);
			for (UInt i = 0; i < cfgRowHeight.values.size(); i++)
			{
				m_iTileRowHeight[i] = cfgRowHeight.values[i];
			}
		}
	}
	else
	{
		m_iTileRowHeight.clear();
	}


	// Rules for input, output and internal bitdepths as per help text.
	
	if (m_aInternalBitDepth[CHANNEL_TYPE_LUMA] == 0)
	{
		m_aInternalBitDepth[CHANNEL_TYPE_LUMA] = m_aInputBitDepth[CHANNEL_TYPE_LUMA];
	}
	if (m_aInternalBitDepth[CHANNEL_TYPE_CHROMA] == 0)
	{
		m_aInternalBitDepth[CHANNEL_TYPE_CHROMA] = m_aInternalBitDepth[CHANNEL_TYPE_LUMA];
	}
	if (m_aInputBitDepth[CHANNEL_TYPE_CHROMA] == 0)
	{
		m_aInputBitDepth[CHANNEL_TYPE_CHROMA] = m_aInputBitDepth[CHANNEL_TYPE_LUMA];
	}
	if (m_aOutputBitDepth[CHANNEL_TYPE_LUMA] == 0)
	{
		m_aOutputBitDepth[CHANNEL_TYPE_LUMA] = m_aInternalBitDepth[CHANNEL_TYPE_LUMA];
	}
	if (m_aOutputBitDepth[CHANNEL_TYPE_CHROMA] == 0)
	{
		m_aOutputBitDepth[CHANNEL_TYPE_CHROMA] = m_aInternalBitDepth[CHANNEL_TYPE_CHROMA];
	}



	
	m_InputChromaFormatIDC = Number_to_chroma_format(tmpInputChromaFormat);
	m_ChromaFormatIDC = (tmpChromaFormat == 0) ? (m_InputChromaFormatIDC) : (Number_to_chroma_format(tmpChromaFormat));

	
	switch (UIProfile)
	{
	case UI_NONE:
		m_Profile = Profile::NONE;
		break;
	case UI_MAIN:
		m_Profile = Profile::MAIN;
		break;
	case UI_MAIN10:
		m_Profile = Profile::MAIN10;
		break;
	case UI_MAINREXT:
		m_Profile = Profile::MAINREXT;
		break;
	default:
		fprintf(stderr, "Error: Unprocessed UI profile\n");
		assert(0);
		exit(EXIT_FAILURE);
		break;
		
	}

	switch (m_Profile)
	{
	case Profile::MAINREXT:
	case Profile::MAIN:
	case Profile::MAIN10:
	case Profile::NONE:
		break;
	default:
		fprintf(stderr, "Unknown profile selected\n");
		exit(EXIT_FAILURE);
		break;

	}

	if (tmpSliceMode < 0 || tmpSliceMode >= Int(NUMBER_OF_SLICE_CONSTRAINT_MODES))
	{
		fprintf(stderr, "Error: bad slice mode\n");
		exit(EXIT_FAILURE);
	}
	m_eSliceMode = SliceConstraint(tmpSliceMode);


	if (tmpSliceSegmentMode < 0 || tmpSliceSegmentMode >= Int(NUMBER_OF_SLICE_CONSTRAINT_MODES))
	{
		fprintf(stderr, "Error: bad slice segment mode\n");
		exit(EXIT_FAILURE);
	}
	m_eSliceSegmentMode = SliceConstraint(tmpSliceSegmentMode);


	// Allocate slice-based dQP values.
	m_aidQP = new Int[m_iFramesToBeEncoded + m_iGOPSize + 1];
	::memset(m_aidQP, 0, sizeof(Int) * (m_iFramesToBeEncoded + m_iGOPSize + 1));// ¡°::¡± Global scope

	// Check validity of input parameters
	check_parameter();

	// Compute actual CU depth with respect to config depth and max transform size.
	UInt uiAddCUDepth = 0;
	while ((m_uiMaxCUWidth >> m_uiMaxCUDepth) > (1 << (m_uiQuadtreeTULog2MinSize + uiAddCUDepth)))
	{
		uiAddCUDepth++;
	}

	m_uiMaxTotalCUDepth = m_uiMaxCUDepth + uiAddCUDepth + get_max_CUdepth_offset(m_ChromaFormatIDC, m_uiQuadtreeTULog2MinSize);// If minimum TU larger than 4x4, allow for additional part indices for 4:2:2 SubTUs.
	m_uiLog2DiffMaxMinCodingBlockSize = m_uiMaxCUDepth - 1;

	
	// Print-out parameters. 
	print_parameter();

	return true;

}



// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

Void AppEncCfg::check_parameter()
{
	if (m_Profile == Profile::NONE)
	{
		fprintf(stderr, "***************************************************************************\n");
		fprintf(stderr, "** WARNING: For conforming bitstreams a valid Profile value must be set! **\n");
		fprintf(stderr, "***************************************************************************\n");
	}
	if (m_Level == Level::NONE)
	{
		fprintf(stderr, "***************************************************************************\n");
		fprintf(stderr, "** WARNING: For conforming bitstreams a valid Level value must be set!   **\n");
		fprintf(stderr, "***************************************************************************\n");
	}

	Bool checkFailed = false;// Abort if there is a fatal configuration problem.
#define CONFIRM_PARA(a, b) checkFailed |= Confirm_para(a, b)
	CONFIRM_PARA(m_szBitstreamFileName.empty(), "A bitstream file name must be specified (BitstreamFile)");

	switch (m_Profile)
	{
	case Profile::MAIN:
	case Profile::MAIN10:
	{
		CONFIRM_PARA(m_bEnableIntraReferenceSmoothing == false, "EnableIntraReferenceSmoothing must be enabled for non main-RExt profiles.");
	}
	break;
	case Profile::NONE:
		// Non-conforming configuration, so all settings are valid.
		break;
	default:
		CONFIRM_PARA(1, "Unknown profile selected.");
		break;

	}


	// Check range of parameters.
	CONFIRM_PARA(m_aInputBitDepth[CHANNEL_TYPE_LUMA] < 8, "InputBitDepth must be at least 8");
	CONFIRM_PARA(m_aInputBitDepth[CHANNEL_TYPE_CHROMA] < 8, "InputBitDepthC must be at least 8");

	for (UInt channelType = 0; channelType < MAX_NUM_CHANNEL_TYPE; channelType++)
	{
		CONFIRM_PARA((m_aInternalBitDepth[channelType] > 12), "Model is not configured to support high enough internal accuracies - enable RExt__HIGH_BIT_DEPTH_SUPPORT to use increased precision internal data types etc...");
	}

	CONFIRM_PARA(m_ChromaFormatIDC >= NUM_CHROMA_FORMAT, "ChromaFormatIDC must be either 400, 420, 422 or 444");
	CONFIRM_PARA(m_InputChromaFormatIDC >= NUM_CHROMA_FORMAT, "InputChromaFormatIDC must be either 400, 420, 422 or 444");
	CONFIRM_PARA(m_iFrameRate <= 0, "Frame rate must be more than 1");
	CONFIRM_PARA(m_iFramesToBeEncoded <= 0, "Total Number Of Frames encoded must be more than 0");
	CONFIRM_PARA(m_iGOPSize < 1, "GOP Size must be greater or equal to 1");
	CONFIRM_PARA(m_iGOPSize > 1 && m_iGOPSize % 2, "GOP Size must be a multiple of 2, if GOP Size is greater than 1");
	CONFIRM_PARA((m_iIntraPeriod > 0 && m_iIntraPeriod < m_iGOPSize) || m_iIntraPeriod == 0, "Intra period must be more than GOP size, or -1 , not 0");


	CONFIRM_PARA(m_iDeblockingFilterMetric != 0 && (m_bLoopFilterDisable || m_bLoopFilterOffsetInPPS), "If DeblockingFilterMetric is non-zero then both LoopFilterDisable and LoopFilterOffsetInPPS must be 0");
	CONFIRM_PARA(m_iLoopFilterBetaOffsetDiv2 < -6 || m_iLoopFilterBetaOffsetDiv2 > 6, "Loop Filter Beta Offset div. 2 exceeds supported range (-6 to 6)");
	CONFIRM_PARA(m_iLoopFilterTcOffsetDiv2 < -6 || m_iLoopFilterTcOffsetDiv2 > 6, "Loop Filter Tc Offset div. 2 exceeds supported range (-6 to 6)");

	CONFIRM_PARA(m_iQP < -6 * (m_aInternalBitDepth[CHANNEL_TYPE_LUMA] - 8) || m_iQP > 51, "QP exceeds supported range (-QpBDOffsety to 51)");
	CONFIRM_PARA(m_iMaxDeltaQP > 7, "Absolute Delta QP exceeds supported range (0 to 7)");
	CONFIRM_PARA(m_uiMaxCUDepth < 1, "MaxPartitionDepth must be greater than zero");
	CONFIRM_PARA(m_iMaxCuDQPDepth > m_uiMaxCUDepth - 1, "Absolute depth for a minimum CuDQP exceeds maximum coding unit depth");

	CONFIRM_PARA((m_uiMaxCUWidth >> m_uiMaxCUDepth) < 4, "Minimum partition width size should be larger than or equal to 8");
	CONFIRM_PARA((m_uiMaxCUHeight >> m_uiMaxCUDepth) < 4, "Minimum partition height size should be larger than or equal to 8");
	CONFIRM_PARA(m_uiMaxCUWidth < 16, "Maximum partition width size should be larger than or equal to 16");
	CONFIRM_PARA(m_uiMaxCUHeight < 16, "Maximum partition height size should be larger than or equal to 16");
	CONFIRM_PARA((m_iSourceWidth % (m_uiMaxCUWidth >> (m_uiMaxCUDepth - 1))) != 0, "Resulting coded frame width must be a multiple of the minimum CU size");
	CONFIRM_PARA((m_iSourceHeight % (m_uiMaxCUHeight >> (m_uiMaxCUDepth - 1))) != 0, "Resulting coded frame height must be a multiple of the minimum CU size");
	CONFIRM_PARA(m_uiQuadtreeTULog2MinSize < 2, "QuadtreeTULog2MinSize must be 2 or greater.");
	CONFIRM_PARA(m_uiQuadtreeTULog2MaxSize > 5, "QuadtreeTULog2MaxSize must be 5 or smaller.");
	CONFIRM_PARA(m_uiQuadtreeTULog2MaxSize < m_uiQuadtreeTULog2MinSize, "QuadtreeTULog2MaxSize must be greater than or equal to m_uiQuadtreeTULog2MinSize.");
	CONFIRM_PARA((1 << m_uiQuadtreeTULog2MaxSize) > m_uiMaxCUWidth, "QuadtreeTULog2MaxSize must be log2(maxCUSize) or smaller.");
	CONFIRM_PARA((1 << m_uiQuadtreeTULog2MinSize) >= (m_uiMaxCUWidth >> (m_uiMaxCUDepth - 1)), "QuadtreeTULog2MinSize must not be greater than or equal to minimum CU size");
	CONFIRM_PARA(m_uiQuadtreeTUMaxDepthIntra < 1, "QuadtreeTUMaxDepthIntra must be greater than or equal to 1");
	CONFIRM_PARA(m_uiMaxCUWidth < (1 << (m_uiQuadtreeTULog2MinSize + m_uiQuadtreeTUMaxDepthIntra - 1)), "QuadtreeTUMaxDepthIntra must be less than or equal to the difference between log2(maxCUSize) and QuadtreeTULog2MinSize plus 1");


	if (m_bUsePCM)
	{
		
		CONFIRM_PARA(m_uiPCMLog2MinSize < 3, "PCMLog2MinSize must be 3 or greater.");
		CONFIRM_PARA(m_uiPCMLog2MinSize > 5, "PCMLog2MinSize must be 5 or smaller.");
		CONFIRM_PARA(m_uiPCMLog2MaxSize > 5, "PCMLog2MaxSize must be 5 or smaller.");
		CONFIRM_PARA(m_uiPCMLog2MaxSize < m_uiPCMLog2MinSize, "PCMLog2MaxSize must be equal to or greater than m_uiPCMLog2MinSize.");
	}


	if (m_eSliceMode != NO_SLICES)
	{
		CONFIRM_PARA(m_iSliceArgument < 1, "SliceArgument should be larger than or equal to 1");
	}
	if (m_eSliceSegmentMode != NO_SLICES)
	{
		CONFIRM_PARA(m_iSliceSegmentArgument < 1, "SliceSegmentArgument should be larger than or equal to 1");
	}


	Bool tileFlag = (m_iNumTileColumnsMinus1 > 0 || m_iNumTileRowsMinus1 > 0);
	
	CONFIRM_PARA(tileFlag && m_bEntropyCodingSyncEnabledFlag, "Tiles and entropy-coding-sync (Wavefronts) can not be applied together");
	

	CONFIRM_PARA(m_iSourceWidth  % LibComSPS::Get_win_unitX(m_ChromaFormatIDC) != 0, "Picture width must be an integer multiple of the specified chroma subsampling");
	CONFIRM_PARA(m_iSourceHeight % LibComSPS::Get_win_unitX(m_ChromaFormatIDC) != 0, "Picture height must be an integer multiple of the specified chroma subsampling");
	

	// Max CU width and height should be power of 2.
	UInt ui = m_uiMaxCUWidth;
	while (ui)
	{
		ui >>= 1;
		if ((ui & 1) == 1)
		{
			CONFIRM_PARA(ui != 1, "Width should be 2^n");
		}
	}
	ui = m_uiMaxCUHeight;
	while (ui)
	{
		ui >>= 1;
		if ((ui & 1) == 1)
		{
			CONFIRM_PARA(ui != 1, "Height should be 2^n");
		}
	}

	if (m_iIntraPeriod == 1 && m_GOPList[0].m_iPOC == -1)
	{
		m_GOPList[0] = GOPEntry();
		m_GOPList[0].m_iPOC = 1;
	}



	CONFIRM_PARA(m_iIntraPeriod >= 0 && (m_iIntraPeriod % m_iGOPSize != 0), "Intra period must be a multiple of GOPSize, or -1");
	

	for (Int i = 0; i < MAX_TLAYER; i++)
	{
		m_iMaxDecPicBuffering[i] = 1;
	}




	
	CONFIRM_PARA(!m_bTransquantBypassEnabledFlag && m_bCUTransquantBypassFlagForce, "CUTransquantBypassFlagForce cannot be 1 when TransquantBypassEnableFlag is 0");

	
#undef CONFIRM_PARA
	if (checkFailed)
	{
		exit(EXIT_FAILURE);
	}

}




Void AppEncCfg::print_parameter()
{

	printf("\n");
	printf("--------------GENERAL CFG----------------\n");
	printf("Input File: %s\n", m_szInputFileName.c_str());
	printf("Bitstream File: %s\n", m_szBitstreamFileName.c_str());
	printf("Reconstruction File: %s\n", m_szReconFileName.c_str());
	printf("Sequence PSNR output: %s\n", (m_bPrintMSEBasedSequencePSNR ? "Linear average, MSE-based" : "Linear average only"));
	printf("Sequence MSE output: %s\n", (m_bPrintSequenceMSE ? "Enabled" : "Disabled"));
	printf("Frame MSE output: %s\n", (m_bPrintFrameMSE ? "Enabled" : "Disabled"));
	printf("MS-SSIM output: %s\n", (m_bPrintMSSSIM ? "Enabled" : "Disabled"));
	printf("Cabac-zero-word-padding : %s\n", (m_bCabacZeroWordPaddingEnabled ? "Enabled" : "Disabled"));
	printf("Frame index: %u - %d (%d frames)\n", 0, m_iFramesToBeEncoded - 1, m_iFramesToBeEncoded);
	printf("Profile: %s\n", Profile_to_string(m_Profile));
	printf("CU size / depth / total-depth: %d / %d / %d\n", m_uiMaxCUWidth, m_uiMaxCUDepth, m_uiMaxTotalCUDepth);
	printf("RQT trans. size (min / max): %d / %d\n", 1 << m_uiQuadtreeTULog2MinSize, 1 << m_uiQuadtreeTULog2MaxSize);// RQT: Recursion QuadTree
	printf("Max RQT depth intra: %d\n", m_uiQuadtreeTUMaxDepthIntra);
	printf("Intra period: %d\n", m_iIntraPeriod);
	printf("QP: %d\n", m_iQP);
	printf("Max dQP signaling depth: %d\n", m_iMaxCuDQPDepth);
	printf("GOP size: %d\n", m_iGOPSize);
	printf("Input bit depth: (Y:%d, C:%d)\n", m_aInputBitDepth[CHANNEL_TYPE_LUMA], m_aInputBitDepth[CHANNEL_TYPE_CHROMA]);
	printf("Internal bit depth: (Y:%d, C:%d)\n", m_aInternalBitDepth[CHANNEL_TYPE_LUMA], m_aInternalBitDepth[CHANNEL_TYPE_CHROMA]);


	printf("\n\n");
	printf("--------------PERFORMANCE CFG----------------\n");
	printf("Min PCM size: %d\n", 1 << m_uiPCMLog2MinSize);
	printf("PCM sample bit depth: (Y:%d, C:%d)\n", m_bPCMInputBitDepthFlag ? m_aInputBitDepth[CHANNEL_TYPE_LUMA] : m_aInternalBitDepth[CHANNEL_TYPE_LUMA],
		                                           m_bPCMInputBitDepthFlag ? m_aInputBitDepth[CHANNEL_TYPE_CHROMA] : m_aInternalBitDepth[CHANNEL_TYPE_CHROMA]);
	printf("Intra reference smoothing: %s\n", (m_bEnableIntraReferenceSmoothing ? "Enabled" : "Disabled"));
	printf("RDQ: %d\n", m_bUseRDOQ);
	printf("RDQTS: %d\n", m_bUseRDOQTS);
	printf("SQP: %d\n", m_uiDeltaQpRD);
	printf("SAO: %d\n", (m_bUseSAO) ? (1) : (0));
	printf("PCM: %d\n", (m_bUsePCM && (1 << m_uiPCMLog2MinSize) <= m_uiMaxCUWidth) ? 1 : 0);
	printf("TransformSkip: %d\n", m_bUseTransformSkip);
	printf("TransformSkipFast: %d\n", m_bUseTransformSkipFast);


	printf("\n\n");
	printf("--------------FUNCTION CFG----------------\n");
	printf("RQT: %d\n", 1);// RQT: Residual Quad-tree Transform
	printf("Slice: M=%d\n", Int(m_eSliceMode));
	if (m_eSliceMode != NO_SLICES)
	{
		printf("A=%d\n", m_iSliceArgument);
	}
	printf("SliceSegment: M=%d\n", m_eSliceSegmentMode);
	if (m_eSliceSegmentMode != NO_SLICES)
	{
		printf("A=%d\n", m_iSliceSegmentArgument);
	}


	const Int iWaveFrontSubstreams = m_bEntropyCodingSyncEnabledFlag ? (m_iSourceHeight + m_uiMaxCUHeight - 1) / m_uiMaxCUHeight : 1;
	printf("WaveFrontSynchro: %d  WaveFrontSubstreams: %d\n", m_bEntropyCodingSyncEnabledFlag ? 1 : 0, iWaveFrontSubstreams);


	printf("\n\n");
	fflush(stdout);
	

}



