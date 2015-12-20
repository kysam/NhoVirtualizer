#pragma once
#include <Windows.h>
#include "NhoProcessor.h"

class ProfileManager
{
public:
	ProfileManager();
	~ProfileManager();

	char m_inFile[MAX_PATH + 1];
	char m_outFile[MAX_PATH + 1];

	bool Load(const char* fileName);
	bool Save(const char* fileName);
	void Execute(NhoProcessor* processor);
};

