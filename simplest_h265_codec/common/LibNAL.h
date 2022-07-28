

#ifndef _LIB_NAL_H_
#define _LIB_NAL_H_


// Represents a single NALunit header and the associated RBSPayload.
struct NALUnit 
{


};


// A single NALunit, with complete payload in EBSP format.
// NALU = NALUnit header + RBSP
// RBSP = SODB + rbsp_trailing_bits()
// EBSP = RBSP + 0x03
struct NALUnitEBSP : public NALUnit
{




};



#endif// _LIB_NAL_H_



