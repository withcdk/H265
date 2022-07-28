

#ifndef _LIB_ENC_SBAC_H_
#define _LIB_ENC_SBAC_H_

#include "../common/LibComSlice.h"

#include "LibEncEntropy.h"




// SBAC encoder class.
class LibEncSbac : public LibEncEntropyIf
{
public:
	LibEncSbac();
	virtual ~LibEncSbac();

	Void Reset_entropy(const LibComSlice* slice);


protected:
private:
};





#endif// _LIB_ENC_SBAC_H_

