/******************************************************************************
*File: LibEncGOP.h
*Descriptions: Encoder class (header).
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-02-14

******************************************************************************/


#ifndef _LIB_ENC_GOP_H_
#define _LIB_ENC_GOP_H_

#include "../common/LibComPic.h"
#include "../common/LibComList.h"
#include "../common/LibAccessUnit.h"


#include "LibEncAnalyze.h"
#include "LibEncCfg.h"
#include "LibEncSlice.h"



class LibEncTop;


// ====================================================================================================================
// Class definition
// ====================================================================================================================

class LibEncGOP
{
public:
	LibEncGOP();
	virtual ~LibEncGOP();


	Void Init(LibEncTop* libEncTop);





	Void CompressGOP(Int POCLast, Int numPicRcvd, LibComList<LibComPic*>& listPic, LibComList<LibComPicYuv*>& listPicYuvRecOut,
		             std::list<LibAccessUnit>& accessUnitsInGOP, const LibEncAnalyze::OutputLogControl& outputLogCtrl);


	NalUnitType Get_nal_unit_type(Int pocCurr, Int lastIDR);



protected:
	Void init_GOP(Int POCLast, Int numPicRcvd);

	Void get_buffer(LibComList<LibComPic*>& listPic, LibComList<LibComPicYuv*>& listPicYuvRecOut, Int numPicRcvd, Int timeOffset, 
		            LibComPic*& pic, LibComPicYuv*& picYuvRecOut, Int pocCurr);




private:

	Int m_iLastIDR;

	Int m_iGOPSize;


	LibEncCfg* m_pcCfg;
	LibEncSlice* m_pcSliceEncoder;

	

};



#endif// _LIB_ENC_GOP_H_




