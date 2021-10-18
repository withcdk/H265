

#include "AnnexBread.h"




/*************************************************
Description:
Parse an AVC AnnexB Bytestream bs to extract a
single nalUnit while accmulating statistics into
stats.
If EOF occurs while trying to extract a NALunit,
an exception of std::ios_base::failure is thrown.
The sontents of stats will be correct at this point.
Input para:
Return value:
Return false if EOF was reached(NB, nalUnit data
may be valid), otherwise true
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*************************************************/
static Void _bytestream_nalunit(InputByteStream &bs, std::vector<uint8_t> &nalUnit, AnnexBStats &stats)
{

	// At the beginning of the decoding process, the decoder initializes its
	// current position in the byte stream to the beginning of the byte stream.
	// It then extracts and discards each leading_zero_8bits syntax element (if
	// present), moving the current position in the byte stream forward one
	// byte at a time, until the current position in the byte stream is such
	// that the next four bytes in the bitstream form the four-byte sequence
	// 0x00000001.	
	while ((bs.EOF_before_n_bytes(24 / 8) || bs.Peek_bytes(24 / 8) != 0x000001)
		&& (bs.EOF_before_n_bytes(32 / 8) || bs.Peek_bytes(32 / 8) != 0x00000001))
	{
		uint8_t leadingZero8Bits = bs.Read_byte();
		assert(leadingZero8Bits == 0);
		stats.m_NumLeadingZero8BitsBytes++;
	}

	// 1. When the next four bytes in the bitstream form the four-byte sequence
	// 0x00000001, the next byte in the byte stream (which is a zero_byte syntax element)
	// is extracted and discarded and the current position in the byte stream is set equal
	// to the position of the byte following this discarded byte.
	// NB(note well), the previous step guarantees this will succeed -- if EOF was encountered, an
	// exception will stop execution getting this far.
	if (bs.Peek_bytes(24 / 8) != 0x000001)
	{
		uint8_t zeroByte = bs.Read_byte();
		assert(zeroByte == 0);
		stats.m_NumZeroByteBytes++;
	}


	// 2. The next three-byte sequence in the byte stream (which is a start_code_prefix_one_3bytes)
	// is extracted and discarded and the current position in the byte stream is set equal to the 
	// position of the byte following this three-byte sequence.
	// NB, (1) guarantees that the next three bytes are 0x00 00 01
	uint32_t startCodePrefixOne3Bytes = bs.Read_bytes(24 / 8);
	assert(startCodePrefixOne3Bytes == 0x000001);
	stats.m_NumStartCodePrefixBytes += 3;


	// 3. NumBytesInNALunit is set equal to the number of bytes starting with the byte at the current
	// position in the byte stream up to and including the last byte that precedes the location of any
	// of the following conditions:
	// a. A subsequent byte-aligned three-byte sequence equal to 0x000000, or
	// b. A subsequent byte-aligned three-byte sequence equal to 0x000001, or
	// c. The end of the byte stream, as determined by unspecified means.

	// 4. NumByteInNALunit bytes are removed from the bitstream and the current position in the byte stream
	// is advanced by NumByteInNALunit bytes. This sequence of bytes is nal_unit(NumBytesInNALunit) and is 
	// decoded using the NAL unit decoding process.
	// NB, (unsigned)x > 2 implies x != 0 && x != 1
	while (bs.EOF_before_n_bytes(24 / 8) || bs.Peek_bytes(24 / 8) > 2)
	{
		nalUnit.push_back(bs.Read_byte());
	}


	// 5. When the current position in the byte stream is:
	// - not at the end of the byte stream (as determined by unspecified means)
	// - and the next bytes in the byte stream do not start with a three-byte
	//   sequence equal to 0x000001
	// - and the next bytes in the byte stream do not start with a four byte
	//   sequence equal to 0x00000001,
	// the decoder extracts and discards each trailing_zero_8bits syntax element,
	// moving the current position in the byte stream forward one byte at a time, 
	// until the current position in the byte stream is such that:
	// - the next bytes in the byte stream form the four-byte sequence 0x00000001 or
	// - the end of the byte stream has been encountered (as determined by unspecified means).
	// NB, (3) guarantees there are at least three bytes available or none
	while ((bs.EOF_before_n_bytes(24 / 8) || bs.Peek_bytes(24 / 8) != 0x000001)
		&& (bs.EOF_before_n_bytes(32 / 8) || bs.Peek_bytes(32 / 8) != 0x00000001))
	{
		uint8_t trailingZero8Bits = bs.Read_byte();
		assert(trailingZero8Bits == 0);
		stats.m_NumTrailingZero8BitsBytes++;
	}

}



/*************************************************
Description:
Parse an AVC AnnexB Bytestream bs to extract a 
single nalUnit while accmulating statistics into
stats.
Input para:
Return value:
Return false if EOF was reached(NB, nalUnit data 
may be valid), otherwise true
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*************************************************/
Bool bytestream_nalunit(InputByteStream &bs, std::vector<uint8_t> &nalUnit, AnnexBStats &stats)
{

	Bool eof = false;

	try
	{
		_bytestream_nalunit(bs, nalUnit, stats);
	}
	catch (...)
	{
		eof = true;
	}

	stats.m_NumBytesInNALUnit = UInt(nalUnit.size());

	return eof;
}





