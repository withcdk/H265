/**
******************************************************************************
*File: AppDecTop.cpp
*Descriptions: Decoder application class
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2021-12-01
           Huang Tao Tao 2021-12-20        
******************************************************************************
*/

#include <fstream>
#include <vector>


#include "AppDecTop.h"


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

	// cdk_coding
}






