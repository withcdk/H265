
#ifndef _LIB_COM_LIST_H_
#define _LIB_COM_LIST_H_

#include <list>


//List template.
template<class C>
class LibComList : public std::list<C> // NOTE: should not inherit from STL classes.
{
public:
	typedef typename std::list<C>::iterator LibComIterator;

	LibComList& operator+=(const LibComList& libComList)
	{
		if (!libComList.empty())
		{
			insert(this->end(), libComList.begin(), libComList.end());
		}
		return *this;
	}

	C Pop_back()
	{
		C cT = this->back();
		this->pop_back();
		return cT;
	}

	C Pop_front()
	{
		C cT = this->front();
		this->pop_front();
		return cT;
	}

	Void Push_back(const C& cT)
	{
		//assert(sizeof(C) == 4);// Because "C" is a pointer type.
		if (cT != NULL)
		{
			this->push_back(cT);
		}
	}

	Void Push_front(const C& cT)
	{
		// assert( sizeof(C) == 4);
		if (cT != NULL)
		{
			this->push_front(cT);
		}
	}

	LibComIterator Find(const C& cT)
	{

		return std::list<C>::find(this->begin(), this->end(), cT);
	}

};




#endif

