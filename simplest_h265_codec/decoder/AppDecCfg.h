/**
******************************************************************************
*brief: Decoder configuration class (header).
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#ifndef _APP_DEC_CFG_H_
#define _APP_DEC_CFG_H_

#include <string>

#include "../common/LibTypeDef.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================

class AppDecCfg
{
public:
	AppDecCfg()
	: m_szBitstreamFileName()
	, m_szReconFileName()
	{}// Initialize data members using the parameter initialization table.

	virtual ~AppDecCfg() {} // virtual destructor

	Bool  Parse_cfg(Int argc, TChar* argv[]);   // initialize option class from configuration

protected:
	std::string   m_szBitstreamFileName;                    // input bitstream file name
	                                                         //std::string ,using string template classes should contain a <string> header file,
	                                                        // as part of the standard library, string is defined in the namespace std

	std::string   m_szReconFileName;                        // output reconstruction file name
	

};


#endif // _APP_DEC_CFG_H_
