/**
******************************************************************************
*brief: reading functions for Annex B byte streams
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/



#pragma once

#ifndef __ANNEXBREAD__
#define __ANNEXBREAD__ 


#include <vector>
#include <assert.h>
#include <istream>

#include "../common/TypeDef.h"


class InputByteStream
{

public:
	InputByteStream(std::istream &istream)
	: m_NumFutureBytes(0)
	, m_FutureBytes(0)
	, m_Input(istream)
	{
		istream.exceptions(std::istream::eofbit | std::istream::badbit);
	}



	// returns true if an EOF will be encountered within the next n bytes.
	// test next n bytes if there is an EOF.
	Bool EOF_before_n_bytes(UInt n)
	{
		assert(n <= 4);
		if (m_NumFutureBytes >= n)
		{
			return false;
		}
		n -= m_NumFutureBytes;
		try
		{
			for (UInt i = 0; i < n; i++)
			{
				m_FutureBytes = (m_FutureBytes << 8) | m_Input.get();// get a character from input stream, 
				                                                      // but represents it using ASCII in bytestream
				m_NumFutureBytes++;
			}
		}
		catch (...)
		{
			return true;
		}

		return false;
	}

	/***************************************************************************
	Description:
	return the next n bytes in the stream without advancing the stream pointer
	Input para:
	Return value:
	an unsigned integer representing an n byte bigendian word
	Author£ºDe-Kai Chen    <cdk5@foxmail.com>
	****************************************************************************/
	uint32_t Peek_bytes(UInt n)
	{
		EOF_before_n_bytes(n);
		return m_FutureBytes >> 8 * (m_NumFutureBytes - n);
	}


	/*******************************************************************************
	Description:
	Consume and return one byte from the input. If bytestream is already at EOF
	prior to a call to Read_byte(), an exception std::ios_base::failure is thrown
	Input para:
	Return value:
	Author£ºDe-Kai Chen    <cdk5@foxmail.com>
	********************************************************************************/
	uint8_t Read_byte()
	{
		if (!m_NumFutureBytes)
		{
			uint8_t byte = m_Input.get();
			return byte;
		}
		m_NumFutureBytes--;
		uint8_t wantedByte = m_FutureBytes >> 8 * m_NumFutureBytes;
		m_FutureBytes &= ~(0xff << 8 * m_NumFutureBytes);
		return wantedByte;

	}


	/*******************************************************************************
	Description:
	Consume and return n bytes from the input. N bytes from byte stream are interpreted
	as bigendian when assembling the return value.
	Input para:
	Return value:
	Author£ºDe-Kai Chen    <cdk5@foxmail.com>
	********************************************************************************/
	uint32_t Read_bytes(UInt n)
	{
		uint32_t val = 0;
		for (UInt i = 0; i < n; i++)
		{
			val = (val << 8) | Read_byte();
		}
		return val;
	}




private:
	UInt m_NumFutureBytes;// number of valid bytes in m_FutureBytes
	uint32_t m_FutureBytes;// bytes that have been peeked
	std::istream &m_Input;// Input stream to read from



};





// Statistics associated with AnnexB bytestreams
struct AnnexBStats 
{
	UInt m_NumLeadingZero8BitsBytes;
	UInt m_NumZeroByteBytes;
	UInt m_NumStartCodePrefixBytes;
	UInt m_NumTrailingZero8BitsBytes;
	UInt m_NumBytesInNALUnit;


};


Bool bytestream_nalunit(InputByteStream &bs, std::vector<uint8_t>& nalUnit, AnnexBStats &stats);
static Void _bytestream_nalunit(InputByteStream &bs, std::vector<uint8_t>& nalUnit, AnnexBStats &stats);


#endif



