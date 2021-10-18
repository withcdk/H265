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

	Void Create(); // create internal members
	Void Destroy(); // destroy internal members
	void Decode();// main decoding function



};



#endif // __APPDECTOP__
