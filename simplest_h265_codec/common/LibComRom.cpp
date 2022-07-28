


#include "LibComRom.h"




// ====================================================================================================================
// Data structure related table & variable
// ====================================================================================================================

UInt g_auiZscanToRaster[MAX_NUM_PART_IDXS_IN_CTU_WIDTH*MAX_NUM_PART_IDXS_IN_CTU_WIDTH] = { 0, };
UInt g_auiRasterToZscan[MAX_NUM_PART_IDXS_IN_CTU_WIDTH*MAX_NUM_PART_IDXS_IN_CTU_WIDTH] = { 0, };




Void Init_zscan_to_raster(Int maxDepth, Int depth, UInt startVal, UInt*& currIdx)
{
	Int stride = 1 << (maxDepth - 1);

	if (depth == maxDepth)
	{
		currIdx[0] = startVal;
		currIdx++;
	} 
	else
	{
		Int step = stride >> depth;
		Init_zscan_to_raster(maxDepth, depth + 1, startVal, currIdx);
		Init_zscan_to_raster(maxDepth, depth + 1, startVal + step, currIdx);
		Init_zscan_to_raster(maxDepth, depth + 1, startVal + step * stride, currIdx);
		Init_zscan_to_raster(maxDepth, depth + 1, startVal + step * stride + step, currIdx);

	}

}

Void Init_raster_to_zscan(Int maxCuWidth, UInt maxCUHeight, UInt maxDepth)
{
	UInt minCuWidth = maxCuWidth >> (maxDepth - 1);
	UInt minCuHeight = maxCUHeight >> (maxDepth - 1);

	UInt numPartInWidth = (UInt)maxCuWidth / minCuWidth;
	UInt numPartInHeight = (UInt)maxCUHeight / minCuHeight;

	for (UInt i = 0; i < numPartInWidth * numPartInHeight; i++)
	{
		g_auiRasterToZscan[g_auiZscanToRaster[i]] = i;
	}


}

