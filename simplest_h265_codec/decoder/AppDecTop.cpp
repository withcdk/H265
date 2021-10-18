/**
******************************************************************************
*brief: Decoder application class
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/

#include <fstream>
#include <vector>


#include "AppDecTop.h"
#include "AnnexBread.h"
#include "NALread.h"




using namespace std;

// =======================================================================================================================
// Constructor / destructor / initialization / destroy
// =======================================================================================================================



Void AppDecTop::Create()
{

}

Void AppDecTop::Destroy()
{
	m_szBitstreamFileName.clear();
	m_szReconFileName.clear();
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
void AppDecTop::Decode()
{

	ifstream bistreamFile(m_szBitstreamFileName.c_str(), ifstream::in | ifstream::binary);
	if (!bistreamFile)
	{
		fprintf(stderr, "\nfailed to open bitstream file %s for reading\n", m_szBitstreamFileName.c_str());
		exit(EXIT_FAILURE);
	}

	InputByteStream bytestream(bistreamFile);


	while (bistreamFile)
	{

		streampos location = bistreamFile.tellg();
		AnnexBStats stats = AnnexBStats();
		InputNALUnit nalu;

		bytestream_nalunit(bytestream, nalu.Get_bitstream().Get_fifo(), stats);

		if (nalu.Get_bitstream().Get_fifo().empty())
		{
			// This can happen if the following occur:
			// - empty input file
			// - two back-to-back startCodePrefixes
			// - startCodePrefix immediately followed by EOF
			fprintf(stderr, "Warning: Attempt to decode an empty NAL unit\n");
		} 
		else
		{
			//read(nalu);
		}


	}


}






