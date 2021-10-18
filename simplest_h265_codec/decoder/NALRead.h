#pragma once

#ifndef __NALREAD__
#define __NALREAD__

#include "NAL.h"
#include "ComBitStream.h"




class InputNALUnit : public NALUnit
{
public:
	ComInputBitstream &Get_bitstream()       { return m_Bitstream; }

private:
	ComInputBitstream m_Bitstream;



};





#endif



