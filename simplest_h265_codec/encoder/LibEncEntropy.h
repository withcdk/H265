

#ifndef _LIB_ENC_ENTROPY_H_
#define _LIB_ENC_ENTROPY_H_

#include "../common/LibTypeDef.h"
#include "../common/LibComSlice.h"


// Entropy encoder pure class.
class LibEncEntropyIf
{
public:
	virtual ~LibEncEntropyIf() {}

	virtual Void Reset_entropy(const LibComSlice* slice) = 0;


protected:
private:
};






// Entropy encoder class.
class LibEncEntropy
{
public:
	Void Set_entropy_coder(LibEncEntropyIf* e);
	Void Reset_entropy(const LibComSlice* slice) { m_pcLibEntropyCoderIf->Reset_entropy(slice); }

	LibEncEntropyIf* m_pcLibEntropyCoderIf;

protected:
private:
};






#endif// _LIB_ENC_ENTROPY_H_






