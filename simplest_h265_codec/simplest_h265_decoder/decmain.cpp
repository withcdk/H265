
#include <stdlib.h>
#include <stdio.h>
#include "../simplest_h265_common/TypeDef.h"//注意包含路径。本地定义的.h，使用双引号。chendekai
#include "../simplest_h265_common/CommonDef.h"


int main(int argc, char* argv[])//学习指针符号的写法，chendekai
{
	Int returnCode = EXIT_SUCCESS;


	// print information
	fprintf(stdout, "\n");
	fprintf(stdout, "HM software: Decoder Version [%s] (including RExt)", NV_VERSION);
	fprintf(stdout, NVM_ONOS);
	fprintf(stdout, NVM_COMPILEDBY);
	fprintf(stdout, NVM_BITS);
	fprintf(stdout, "\n");


}


