/**
******************************************************************************
*brief: Define macros, basic types, new types and enumerations
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

// ====================================================================================================================
// Basic type redefinition
// ====================================================================================================================

typedef       void                Void;
typedef       bool                Bool;
typedef       char                TChar; // Used for text/characters
typedef       int                 Int;
typedef       double              Double;
typedef       unsigned int	      UInt;
typedef       unsigned char       UChar;
typedef       short               Short;



// ====================================================================================================================
// Named numerical types
// ====================================================================================================================
typedef       Short                Pel;// pixel type



// ====================================================================================================================
// Enumeration
// ====================================================================================================================


enum ChannelType
{
	CHANNEL_TYPE_LUMA = 0,
	CHANNEL_TYPE_CHROMA = 1,
	MAX_NUM_CHANNEL_TYPE = 2
};



namespace Profile
{
	enum Name
	{
		NONE = 0,
		MAIN = 1,
		MAIN10 = 2,
		MAINREXT = 3
			
	};
}



namespace Level
{
	enum Tier
	{
		MAIN = 0,
		HIGH = 1,
		NUMBER_OF_TIERS = 2

	};

	enum Name
	{
		NONE = 0,
		LEVEL1 = 30,
		LEVEL2 = 60,
		LEVEL2_1 = 63,
		LEVEL3 = 90,
		LEVEL3_1 = 93,
		LEVEL4 = 120,
		LEVEL4_1 = 123,
		LEVEL5 = 150,
		LEVEL5_1 = 153,
		LEVEL5_2 = 156,
		LEVEL6 = 180,
		LEVEL6_1 = 183,
		LEVEL6_2 = 186,
		LEVEL8_5 = 255,

	};

}


// Slice / Slice segment encoding modes.
enum SliceConstraint
{
	NO_SLICES = 0,// Don't use slices / slice segments.
	FIXED_NUMBER_OF_CTU = 1,// Limit maximum number of largest coding tree units in a slice / slice segments.
	FIXED_NUMBER_OF_BYTES = 2,// Limit maximum number of bytes in a slice / slice segment.
	FIXED_NUMBER_OF_TILES = 3,// Slices / Slice segments span an integer number of tiles.
	NUMBER_OF_SLICE_CONSTRAINT_MODES = 4
};



// chroma formats (according to semantics of chroma_format_idc)
enum ChromaFormat
{
	CHROMA_400 = 0,
	CHROMA_420 = 1,
	CHROMA_422 = 2,
	CHROMA_444 = 3,
	NUM_CHROMA_FORMAT = 4
};


enum ComponentID
{
	COMPONENT_Y = 0,
	COMPONENT_Cb = 1,
	COMPONENT_Cr = 2,
	MAX_NUM_COMPONENT = 3
};


enum WeightedPredictionMethod
{
	WP_PER_PICTURE_WITH_SIMPLE_DC_COMBINED_COMPONENT = 0,
	WP_PER_PICTURE_WITH_SIMPLE_DC_PER_COMPONENT = 1,
	WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT = 2,
	WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT_AND_CLIPPING = 3,
	WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT_AND_CLIPPING_AND_EXTENSION = 4
};


// Supported ME search methods.
enum MESearchMethod
{
	MESEARCH_FULL = 0,
	MESEARCH_DIAMOND = 1,
	MESEARCH_SELECTIVE = 2,
	MESEARCH_DIAMOND_ENHANCED = 3,
	MESEARCH_NUMBER_OF_METHODS = 4
};


enum FastInterSearchMode
{
	FASTINTERSEARCH_DISABLED = 0,
	FASTINTERSEARCH_MODE1 = 1, // TODO: assign better names to these.
	FASTINTERSEARCH_MODE2 = 2,
	FASTINTERSEARCH_MODE3 = 3
};


enum ScalingListMode
{
	SCALING_LIST_OFF,
	SCALING_LIST_DEFAULT,
	SCALING_LIST_FILE_READ
};


enum NalUnitType
{

	NAL_UNIT_CODED_SLICE_RASL_R,

	NAL_UNIT_CODED_SLICE_IDR_W_RADL,
	NAL_UNIT_CODED_SLICE_IDR_N_LP,

};

// Index for SBAC based RD optimization.
enum CI_IDX
{
	CI_CURR_BEST = 0,// Best mode index.
	CI_NEXT_BEST,// Next best index.
	CI_NUM,// Total number.

};


// Supported partition shape.
enum PartSize
{
	SIZE_2Nx2N = 0,// symmetric motion partition,  2Nx2N
	SIZE_2NxN = 1,// symmetric motion partition,  2Nx N
	SIZE_Nx2N = 2,// symmetric motion partition,   Nx2N
	SIZE_NxN = 3,// symmetric motion partition,   NxN
	SIZE_2NxnU = 4,// asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
	SIZE_2NxnD = 5,// asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
	SIZE_nLx2N = 6,// asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
	SIZE_nRx2N = 7,// asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
	NUMBER_OF_PART_SIZES = 8
};





#endif// _TYPE_DEF_H_
