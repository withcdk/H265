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
	                                                         //std::string ,ʹ��stringģ����Ӧ�ð���<string>ͷ�ļ���
	                                                        //��Ϊ��׼���һ���֣�string�����������ռ�std��,chendekai

	std::string   m_reconFileName;                        // output reconstruction file name

public:
	TAppDecCfg()
	: m_bitstreamFileName()
	, m_reconFileName()
	{}//ʹ�ò�����ʼ��������ݳ�Ա��ʼ��,chendekai

	virtual ~TAppDecCfg() {} //����������,chendekai

	Bool  parseCfg(Int argc, TChar* argv[]);   // initialize option class from configuration



};




#endif // __TAPPDECCFG__
