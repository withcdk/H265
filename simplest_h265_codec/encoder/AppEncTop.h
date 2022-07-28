#ifndef _APP_ENC_TOP_H_
#define _APP_ENC_TOP_H_


#include "AppEncCfg.h"
#include "LibEncTop.h"



#include "../common/LibComPicYuv.h"
#include "../common/LibComList.h"
#include "../common/LibVideoIOYuv.h"

// Encoder application class.
class AppEncTop : public AppEncCfg
{
public:
	Void Encode();// Main encoding function.


protected:
	// Initialization.
	Void init_lib_cfg();// Initialize internal variables.
	Void create_lib();// Create files & encoder class.
	Void init_lib();// Initialize encoder class.


	// File I/O.
	Void print_chroma_format();


	// Obtain required buffers.
	Void get_buffer(LibComPicYuv*& picYuvRec);


private:
	// Class interface.
	LibEncTop m_cLibEncTop;


	LibComList<LibComPicYuv*> m_cListPicYuvRec;// List of reconstruction YUV files(frames).

	LibVideoIOYuv m_cVideoIOYuvInputFile;// Input YUV file.
	LibVideoIOYuv m_cVideoIOYuvReconFile;// Output reconstruction file.

	Int m_iFrameRcvd;// Number of received frames.


};


#endif// _APP_ENC_TOP_H_
