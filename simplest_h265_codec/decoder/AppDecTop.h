/**
******************************************************************************
*brief: Decoder application class (header)
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#ifndef _APP_DEC_TOP_H_
#define _APP_DEC_TOP_H_

#include "../common/LibTypeDef.h"

#include "AppDecCfg.h"


// decoder application class
class AppDecTop : public AppDecCfg
{


public:
	
	Void Create(); // Create internal members.
	Void Destroy(); // destroy internal members
	Void Decode();// main decoding function

private:

};



#endif // _APP_DEC_TOP_H_
