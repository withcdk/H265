/********************************************************************************
Brief: 
Reading functionality for NAL units
Author: De-Kai Chen  <cdk5@foxmail.com>
*********************************************************************************/



#pragma once

#ifndef __NALREAD__
#define __NALREAD__

#include "../common/NAL.h"
#include "../common/ComBitStream.h"




class InputNALUnit : public NALUnit
{
public:
	ComInputBitstream &Get_bitstream()       { return m_Bitstream; }

private:
	ComInputBitstream m_Bitstream;



};

Void read(InputNALUnit &nalu);



#endif



