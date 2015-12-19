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
	
	uchar* data = new uchar[1000];
	peMaster.AddSection(".sam", data, 1000, IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA);
	peMaster.Rebuild("HERE.exe");
	system("pause");
	return 0;
}

