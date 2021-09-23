/**
******************************************************************************
*brief: Decoder configuration class (header)
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#ifndef __APPDECCFG__
#define __APPDECCFG__

#include <string>

#include "../common/TypeDef.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================

class AppDecCfg
{
protected:
	std::string   m_bitstreamFileName;                    // input bitstream file name
	                                                         //std::string ,using string template classes should contain a <string> header file,
	                                                        // as part of the standard library, string is defined in the namespace std

	std::string   m_reconFileName;                        // output reconstruction file name

public:
	AppDecCfg()
	: m_bitstreamFileName()
	, m_reconFileName()
	{}// initialize data members using the parameter initialization table

	virtual ~AppDecCfg() {} // virtual destructor

	Bool  Parse_cfg(Int argc, TChar* argv[]);   // initialize option class from configuration



};


#endif // __APPDECCFG__
