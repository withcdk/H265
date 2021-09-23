/**
******************************************************************************
*brief: Decoder application class
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/



#include "AppDecTop.h"


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

}






