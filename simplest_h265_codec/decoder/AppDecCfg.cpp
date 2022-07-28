/**
******************************************************************************
*brief: Decoder configuration class
*author: De-Kai Chen  <cdk5@foxmail.com>
******************************************************************************
*/



#include <string>


#include "../common/AppProgramOptionsLite.h"


#include "AppDecCfg.h"


using namespace std;
namespace po = df::program_options_lite;


// ====================================================================================================================
// Public member functions
// ====================================================================================================================

// param argc number of arguments
// param argv array of arguments
Bool AppDecCfg::Parse_cfg(Int argc, TChar *argv[])
{
	Bool doHelp = false;
	Int warnUnknowParameter = 0;

	po::Options opts;


	opts.Add_options()// The function call operator () is overloaded, creating an anonymous object (with no new object defined) to call
	("help",                         doHelp,                        false,              "this help text")
	("BitstreamFile,b",              m_szBitstreamFileName,           string(""),         "bitstream input file name")
	("ReconFile,o",                  m_szReconFileName,               string(""),         "reconstructed YUV output file name\n"
		                                                                                "YUV writing is skipped if omitted")
	("WarnUnknowParameter,w",        warnUnknowParameter,           0,                  "warn for unknown configuration parameters instead of failing")
	;

	po::Set_defaults(opts);
	po::ErrorReporter err;
	const list<const TChar*>& argvUnhandled = po::Scan_argv(opts, argc, (const TChar**)argv, err);
	for (list<const TChar*>::const_iterator it = argvUnhandled.begin(); it != argvUnhandled.end(); it++)
	{
		fprintf(stderr, "Unhandled argument ignored: `%s'\n", *it);
	}

	if (argc == 1 || doHelp)
	{
		po::Do_help(cout, opts);
		return false;
	}

	if (err.m_bIsErrored)
	{
		if (!warnUnknowParameter)
		{
			// errors have already been reported to stderr 
			return false;
		}
	}

	if (m_szBitstreamFileName.empty())
	{
		fprintf(stderr, "No input file specified, aborting\n");
		return false;
	}



	return true;
}


