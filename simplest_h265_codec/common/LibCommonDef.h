/**
******************************************************************************
*brief: Defines version information, constants and small in-line functions
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/



#ifndef _COMMON_DEF_H_
#define _COMMON_DEF_H_

#include <algorithm>// _aligned_malloc()


#if _MSC_VER > 1000
// disable "signed and unsigned mismatch"
#pragma warning(disable : 4018)
// disable Bool coercion "performance warning"
#pragma warning(disable : 4800)

#endif



#include "LibTypeDef.h"


// ====================================================================================================================
// Version information
// ====================================================================================================================
#define NV_VERSION        "16.20"                 // Current software version


// ====================================================================================================================
// Platform information
// ====================================================================================================================
#ifdef _WIN32  //_WIN32: Predefined macro to determine whether Windows is used
#define NVM_ONOS        "[Windows]"
#elif  __linux
#define NVM_ONOS        "[Linux]"
#elif  __CYGWIN__
#define NVM_ONOS        "[Cygwin]"
#elif __APPLE__
#define NVM_ONOS        "[Mac OS X]"
#else
#define NVM_ONOS "[Unk-OS]"
#endif



#ifdef __INTEL_COMPILER
#define NVM_COMPILEDBY  "[ICC %d]", __INTEL_COMPILER
#elif  _MSC_VER
#define NVM_COMPILEDBY  "[VS %d]", _MSC_VER          //VC version    _MSC_VER      VS version
                                                      //VC14.0   ----> 1900     -----> VS2015
                                                       //VC15.0  ----> [1910,1920] --> VS2017
                                                       //VC16.0  ----> (1920,   -----> VS2019

#endif


#define NVM_BITS          "[%d bit] ", (sizeof(Void*) == 8 ? 64 : 32) // Used for checking 64-bit O/S.




// ====================================================================================================================
// Coding tool configuration
// ====================================================================================================================
static const Int MAX_GOP = 64;// Max. value of hierarchical GOP size.


static const Int MAX_NUM_SPS = 16;
static const Int MAX_NUM_PPS = 64;

static const Int MAX_QP = 51;


static const Int MAX_TLAYER = 7;// Explicit temporal layer QP offset - max number of temporal layer.

static const Int MAX_CU_SIZE = 64;
static const Int MIN_PU_SIZE = 4;
static const Int MAX_NUM_PART_IDXS_IN_CTU_WIDTH = MAX_CU_SIZE / MIN_PU_SIZE;// Maximum number of partition indices across the width of a CTU (or height of a CTU).





// ====================================================================================================================
// Macro functions
// ====================================================================================================================
template <typename T> inline T Clip3(const T minVal, const T maxVal, const T a) { return std::min<T>(std::max<T>(minVal, a), maxVal); }  // General min/max clip.





#define DATA_ALIGN 1  // Use 32-bit aligned malloc/free.
#if DATA_ALIGN && _WIN32 && (_MSC_VER > 1300)
#define xMalloc(type, len) _aligned_malloc(sizeof(type)*(len), 32)
#define xFree(ptr) _aligned_free(ptr)
#else
#define xMalloc(type, len) malloc(sizeof(type)*(len))
#define xFree(ptr) free(ptr)
#endif



#endif// _COMMON_DEF_H_
