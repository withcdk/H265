
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "../common/test1.h"
#include "../common/TypeDef.h"
#include "../common/CommonDef.h"


#include "AppDecTop.h"


int main(int argc, char* argv[])
{
	
	Int returnCode = EXIT_SUCCESS;
	AppDecTop  appDecTop;

	// print information
	fprintf(stdout, "\n");
	fprintf(stdout, "HM software: Decoder Version [%s] (including RExt)", NV_VERSION);
	fprintf(stdout, NVM_ONOS);// check whether the operating system is Windows or Linux
	fprintf(stdout, NVM_COMPILEDBY);// check whether the compiler ICC(Intel) or VS
	fprintf(stdout, NVM_BITS);// check whether the compilation environment is x86 or x64
	fprintf(stdout, "\n");


	// create application decoder class
	appDecTop.create();

	// parse configuration
	if (!appDecTop.Parse_cfg(argc, argv))
	{
		appDecTop.destroy();
		returnCode = EXIT_FAILURE;
		return returnCode;
	}

	// starting time
	Double dResult;
	clock_t lBefore = clock();


	appDecTop.decode();



	dResult = (Double)(clock() - lBefore) / CLOCKS_PER_SEC;//(clock() - lBefore) forced to be converted to Double, and then divided by CLOCKS_PER_SEC
	printf("\n Total Time: %12.3f sec.\n", dResult);

	// destroy application decoder class
	appDecTop.destroy();

	show_code(5);
	printf("main_decoder\n");
	return returnCode;
}


