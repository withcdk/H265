

#ifndef _LIB_ACCESS_UNIT_H_
#define _LIB_ACCESS_UNIT_H_




#include <list>


#include "LibNAL.h"



// An AccessUnit is a list of one or more NAL units, according to the
// working draft.  All NAL units within the object belong to the same
// access unit.

// NALUnits held in the AccessUnit list are in EBSP format.  Attempting
// to insert an OutputNALUnit into the access unit will automatically cause
// the nalunit to have its headers written and anti-emulation performed.

// The AccessUnit owns all pointers stored within.  Destroying the
// AccessUnit will delete all contained objects.
class LibAccessUnit : public std::list<NALUnitEBSP*>// NOTE: Should not inherit from STL.
{

public:
	~LibAccessUnit()
	{
		for (LibAccessUnit::iterator it = this->begin(); it != this->end(); it++)
		{
			delete *it;
		}

	}



};




#endif// _LIB_ACCESS_UNIT_H_


