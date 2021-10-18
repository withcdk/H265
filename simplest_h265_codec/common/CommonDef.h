/**
******************************************************************************
*brief: Defines version information, constants and small in-line functions
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/



#ifndef __COMMONDEF__
#define __COMMONDEF__
#include "TypeDef.h"

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
#define NVM_COMPILEDBY  "[VS %d]", _MSC_VER          //VC version _MSC_VER      VS version
                                                     //VC14.0 ---> 1900 ----> VS2015
                                                    //VC15.0 -> [1910,1920] -> VS2017
                                                    //VC16.0 -> (1920,  -> VS2019
                                                    
#endif

#define NVM_BITS          "[%d bit] ", (sizeof(Void*) == 8 ? 64 : 32) // used for checking 64-bit O/S

#endif // __COMMONDEF__

