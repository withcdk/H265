
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "../common/test1.h"
#include "../common/TypeDef.h"
#include "../common/CommonDef.h"


#include "TAppDecTop.h"    //������ͷ�ļ��������ҵľ��룬��Զ������˳����д��chendekai


int main(int argc, char* argv[])
{
	//�����Ķ���д�ں�����ǰͷ��chendekai
	Int returnCode = EXIT_SUCCESS;
	TAppDecTop  cTAppDecTop;

	// print information
	fprintf(stdout, "\n");
	fprintf(stdout, "HM software: Decoder Version [%s] (including RExt)", NV_VERSION);
	fprintf(stdout, NVM_ONOS);//�ж�ϵͳwindows����linux��chendekai
	fprintf(stdout, NVM_COMPILEDBY);//�жϱ�����ICC��intel������VS,chendekai
	fprintf(stdout, NVM_BITS);//�жϱ��뻷����x86����x64��chendekai
	fprintf(stdout, "\n");


	// create application decoder class
	cTAppDecTop.create();//ʲô��û��,chendekai

	// parse configuration
	if (!cTAppDecTop.parseCfg(argc, argv))
	{
		cTAppDecTop.destroy();
		returnCode = EXIT_FAILURE;
		return returnCode;
	}

	// starting time
	Double dResult;
	clock_t lBefore = clock();





	dResult = (Double)(clock() - lBefore) / CLOCKS_PER_SEC;//(clock() - lBefore)�Ƚ���ǿ������ת����Double��Ȼ������CLOCKS_PER_SEC�����chendekai
	printf("\n Total Time: %12.3f sec.\n", dResult);

	// destroy application decoder class
	cTAppDecTop.destroy();

	show_code(5);
	printf("main_decoder\n");
	return returnCode;
}


