#include <iostream>// std::cerr
#include <time.h>// clock_t


#include "../common/AppProgramOptionsLite.h"


#include "AppEncTop.h"



int main(int argc, char* argv[])
{
	
	AppEncTop appEncTop;


	// Print information.
	fprintf(stdout, "\n");
	fprintf(stdout, "HM software: Encoder Version [%s] (including RExt)", NV_VERSION);
	fprintf(stdout, NVM_ONOS);// OS version.
	fprintf(stdout, NVM_COMPILEDBY);// Compiler
	fprintf(stdout, NVM_BITS);// 64 bit or 32 bit OS.
	fprintf(stdout, "\n\n");

	appEncTop.Create();


	// Parse configuration.
	try
	{
		if (!appEncTop.Parse_cfg(argc, argv))
		{
			appEncTop.Destroy();
			return 1;
		}
	}
	catch (df::program_options_lite::ParseFailure& e)
	{
		std::cerr << "Error parsing option \"" << e.m_szArg << "\" with argument \"" << e.m_szArg << "\"." << std::endl;// \" is escape character.
		return 1;
	}

	// Starting time.
	Double result;
	clock_t before = clock();

	// Call encoding function.
	appEncTop.Encode();

	// Ending time.
	result = (Double)(clock() - before) / CLOCKS_PER_SEC;
	printf("\n Total Time: %12.3f sec.\n", result);

	// Destroy application encoder class.
	appEncTop.Destroy();

	
	return 0;
}



