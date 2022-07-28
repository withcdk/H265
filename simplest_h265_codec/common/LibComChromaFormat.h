
#ifndef _COM_CHROMA_FORMAT_H_
#define _COM_CHROMA_FORMAT_H_




#include "LibCommonDef.h"


static inline ChannelType to_channel_type(const ComponentID id) { return (id == COMPONENT_Y) ? CHANNEL_TYPE_LUMA : CHANNEL_TYPE_CHROMA; }
static inline Bool is_luma(const ChannelType id) { return (id == CHANNEL_TYPE_LUMA); }


static inline UInt get_number_valid_components(const ChromaFormat fmt) { return (fmt == CHROMA_400) ? 1 : MAX_NUM_COMPONENT; }
static inline UInt get_channel_type_scaleX(const ChannelType id, const ChromaFormat fmt) { return (is_luma(id) || (fmt == CHROMA_444)) ? 0 : 1; }
static inline UInt get_channel_type_scaleY(const ChannelType id, const ChromaFormat fmt) { return (is_luma(id) || (fmt != CHROMA_420)) ? 0 : 1; }

static inline UInt get_component_scaleX(const ComponentID id, const ChromaFormat fmt) { return get_channel_type_scaleX(to_channel_type(id), fmt); }
static inline UInt get_component_scaleY(const ComponentID id, const ChromaFormat fmt) { return get_channel_type_scaleY(to_channel_type(id), fmt); }




// If chroma format is 4:2:2 and a chroma-square-sub-tu is possible for the smallest TU, then increase the depth by 1 to allow for more parts.
static inline UInt get_max_CUdepth_offset(const ChromaFormat chFmt, const UInt quadtreeTULog2MinSize)
{
	return (chFmt == CHROMA_422 && quadtreeTULog2MinSize > 2) ? 1 : 0;
}



#endif//_COM_CHROMA_FORMAT_H_



