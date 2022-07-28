
/**
******************************************************************************
*File: LibComRom.h
*Descriptions: global variables & functions (header)
*Author: De-Kai Chen  <cdk5@foxmail.com>
*Modifier: De-Kai Chen 2022-04-14

******************************************************************************
*/


#ifndef _LIB_COM_ROM_H_
#define _LIB_COM_ROM_H_


#include "LibCommonDef.h"


// Flexible conversion from relative to absolute index.

extern UInt g_auiZscanToRaster[MAX_NUM_PART_IDXS_IN_CTU_WIDTH*MAX_NUM_PART_IDXS_IN_CTU_WIDTH];
extern UInt g_auiRasterToZscan[MAX_NUM_PART_IDXS_IN_CTU_WIDTH*MAX_NUM_PART_IDXS_IN_CTU_WIDTH];


Void Init_zscan_to_raster(Int maxDepth, Int depth, UInt startVal, UInt*& currIdx);
Void Init_raster_to_zscan(Int maxCuWidth, UInt maxCUHeight, UInt maxDepth);



#endif// _LIB_COM_ROM_H_


