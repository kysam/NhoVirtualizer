#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include "PEMaster.h"

int _tmain(int argc, _TCHAR* argv[])
{
	PEMaster peMaster;
	peMaster.Load("conda2.exe");
	peMaster.Rebuild("HERE.exe");
	return 0;
}

