/**
******************************************************************************
*brief: Decoder application class
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#include <fstream>


#include "AppDecTop.h"
#include "../common/AnnexBread.h"
#include "../common/NALread.h"




using namespace std;

// =======================================================================================================================
// Constructor / destructor / initialization / destroy
// =======================================================================================================================



Void AppDecTop::create()
{

}

Void AppDecTop::destroy()
{
	m_bitstreamFileName.clear();
	m_reconFileName.clear();
}


// =======================================================================================================================
// Public member functions
// =======================================================================================================================



/*************************************************
Description: 
-create internal class
-initialize internal class
-untill the end of the bitstream, call decoding 
 function in DecTop class
-delete allocated buffers
-destroy internal class
Input para: 
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
*************************************************/
void AppDecTop::decode()
{

	ifstream bistreamFile(m_bitstreamFileName.c_str(), ifstream::in | ifstream::binary);
	if (!bistreamFile)
	{
		fprintf(stderr, "\nfailed to open bitstream file %s for reading\n", m_bitstreamFileName.c_str());
		exit(EXIT_FAILURE);
	}

	InputByteStream bytestream(bistreamFile);


	while (bistreamFile)
	{

		streampos location = bistreamFile.tellg();
		AnnexBStats stats = AnnexBStats();
		InputNALUnit nalu;

		bytestream_nalunit(bytestream, nalu.Get_bitstream().Get_fifo(), stats);

		break;
	}


}






