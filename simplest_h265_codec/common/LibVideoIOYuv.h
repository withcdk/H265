
#ifndef _LIB_VIDEO_IO_YUV_H_
#define _LIB_VIDEO_IO_YUV_H_


#include <fstream>// <fstream> include <iostream> 


#include "LibCommonDef.h"
#include "LibComPicYuv.h"


using namespace std;


// ====================================================================================================================
// Class definition
// ====================================================================================================================

// YUV file I/O class.
class LibVideoIOYuv
{
public:

	Void Open(const std::string& fileName, Bool writeMode, const Int fileBitDepth[MAX_NUM_CHANNEL_TYPE], const Int internalBitDepth[MAX_NUM_CHANNEL_TYPE]); // Open or create file.


	Bool Read(LibComPicYuv* picYuvOrg, ChromaFormat fileFormat = NUM_CHROMA_FORMAT);// Read one frame.
	Bool Is_eof();// Check for end-of-file.


protected:
private:
	fstream m_cHandle;// File handle.
	Int m_iFileBitDepth[MAX_NUM_CHANNEL_TYPE];// Bitdepth of input/output video file.
	Int m_iBitDepthShift[MAX_NUM_CHANNEL_TYPE];// Number of bits to increase or decrease image by before/after write/read.

};






#endif


