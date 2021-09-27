#pragma once

#ifndef __COMBITSTREAM__
#define __COMBITSTREAM__

#include <vector>


class ComInputBitstream
{

public:
	std::vector<uint8_t> &Get_fifo()       { return m_fifo; }


protected:
	std::vector<uint8_t> m_fifo;

};



#endif


