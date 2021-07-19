/** \file     TAppDecCfg.h
    \brief    Decoder configuration class (header)
*/

#ifndef __TAPPDECCFG__
#define __TAPPDECCFG__

#include <string>

#include "../common/TypeDef.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================

class TAppDecCfg
{
protected:
	std::string   m_bitstreamFileName;                    // input bitstream file name
	                                                         //std::string ,使用string模板类应该包含<string>头文件，
	                                                        //作为标准库的一部分，string定义在命名空间std中,chendekai

	std::string   m_reconFileName;                        // output reconstruction file name

public:
	TAppDecCfg()
	: m_bitstreamFileName()
	, m_reconFileName()
	{}//使用参数初始化表对数据成员初始化,chendekai

	virtual ~TAppDecCfg() {} //虚析构函数,chendekai

	Bool  parseCfg(Int argc, TChar* argv[]);   // initialize option class from configuration



};




#endif // __TAPPDECCFG__
