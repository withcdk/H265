/** \file     TAppDecTop.h
    \brief    Decoder application class (header)
*/

#ifndef __TAPPDECTOP__
#define __TAPPDECTOP__

#include "../common/TypeDef.h"

#include "TAppDecCfg.h"

// decoder application class
class TAppDecTop : public TAppDecCfg
{


public:
	Void  create(); // create internal members
	Void  destroy(); // destroy internal members
};



#endif // __TAPPDECTOP__
