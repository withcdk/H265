/** \file     TAppDecCfg.cpp
    \brief    Decoder configuration class
*/

#include <string>


#include "../common/program_options_lite.h"


#include "TAppDecCfg.h"


using namespace std;
namespace po = df::program_options_lite;


// ====================================================================================================================
// Public member functions
// ====================================================================================================================

/** \param argc number of arguments
    \param argv array of arguments
*/
Bool TAppDecCfg::parseCfg(Int argc, TChar* argv[])
{
	Bool do_help = false;

	po::Options opts;
	opts.addOptions()//�������������()���أ�����һ����������û�ж����¶��������ã�chendekai
	("help",                         do_help,                        false,              "this help text")
	("BitstreamFile,b",              m_bitstreamFileName,            string(""),         "bitstream input file name")
	("ReconFile,o",                  m_reconFileName,                string(""),         "reconstructed YUV output file name\n"
		                                                                                 "YUV writing is skipped if omitted")
	;
	return true;
}


