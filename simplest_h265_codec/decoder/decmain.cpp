
#include <stdio.h>
#include <stdlib.h>


#include "../common/test1.h"
#include "../common/TypeDef.h"
#include "../common/CommonDef.h"


#include "TAppDecTop.h"    //包含的头文件，按查找的距离，由远及近，顺序书写，chendekai


int main(int argc, char* argv[])
{
	//变量的定义写在函数的前头，chendekai
	Int returnCode = EXIT_SUCCESS;
	TAppDecTop  cTAppDecTop;

	// print information
	fprintf(stdout, "\n");
	fprintf(stdout, "HM software: Decoder Version [%s] (including RExt)", NV_VERSION);
	fprintf(stdout, NVM_ONOS);//判断系统windows还是linux，chendekai
	fprintf(stdout, NVM_COMPILEDBY);//判断编译器ICC（intel）还是VS,chendekai
	fprintf(stdout, NVM_BITS);//判断编译环境是x86还是x64，chendekai
	fprintf(stdout, "\n");


	// create application decoder class
	cTAppDecTop.create();//什么都没做,chendekai

	// parse configuration
	if (!cTAppDecTop.parseCfg(argc, argv))
	{
		cTAppDecTop.destroy();
		returnCode = EXIT_FAILURE;
		return returnCode;
	}



	show_code(5);
	printf("main_decoder\n");
	return returnCode;
}


