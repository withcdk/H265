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
	Int warnUnknowParameter = 0;

	po::Options opts;
	opts.addOptions()//函数调用运算符()重载，建立一个匿名对象（没有定义新对象）来调用，chendekai
	("help",                         do_help,                        false,              "this help text")
	("BitstreamFile,b",              m_bitstreamFileName,            string(""),         "bitstream input file name")
	("ReconFile,o",                  m_reconFileName,                string(""),         "reconstructed YUV output file name\n"
		                                                                                 "YUV writing is skipped if omitted")
	;

	po::setDefaults(opts);
	po::ErrorReporter err;
	const list<const TChar*>& argv_unhandled = po::scanArgv(opts, argc, (const TChar**)argv, err);
	for (list<const TChar*>::const_iterator it = argv_unhandled.begin(); it != argv_unhandled.end(); it++)
	{
		fprintf(stderr, "Unhandled argument ignored: `%s'\n", *it);
	}

	if (argc == 1 || do_help)
	{
		po::doHelp(cout, opts);
		return false;
	}

	if (err.is_errored)
	{
		if (!warnUnknowParameter)
		{
			/* errors have already been reported to stderr */
			return false;
		}
	}

	if (m_bitstreamFileName.empty())
	{
		fprintf(stderr, "No input file specified, aborting\n");
		return false;
	}


	return true;
}


