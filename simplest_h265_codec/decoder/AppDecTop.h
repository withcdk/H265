/**
******************************************************************************
*brief: Decoder application class (header)
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#ifndef __APPDECTOP__
#define __APPDECTOP__

#include "../common/TypeDef.h"

#include "AppDecCfg.h"

// decoder application class
class AppDecTop : public AppDecCfg
{


public:
	//AppDecTop();
	//virtual ~AppDecTop(){}

	Void create(); // create internal members
	Void destroy(); // destroy internal members
	void decode();// main decoding function



};



#endif // __APPDECTOP__
