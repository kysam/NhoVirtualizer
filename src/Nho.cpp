#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include "PEMaster.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//test
	PEMaster peMaster;
	if (!peMaster.Load("conda2.exe"))
		printf("load failed");

	peMaster.Rebuild("HERE.exe");
	system("pause");
	return 0;
}

