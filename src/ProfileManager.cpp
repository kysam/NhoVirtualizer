#include "ProfileManager.h"
#include "global.h"
#include <iostream>

ProfileManager::ProfileManager()
{
}


ProfileManager::~ProfileManager()
{
}

bool ProfileManager::Load(const char * fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD read;
	ReadFile(file, m_inFile, MAX_PATH, &read, NULL);
	m_inFile[MAX_PATH] = '\0';

	ReadFile(file, m_outFile, MAX_PATH, &read, NULL);
	m_outFile[MAX_PATH] = '\0';
	CloseHandle(file);
}

bool ProfileManager::Save(const char* fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD written;
	WriteFile(file, m_inFile, MAX_PATH, &written, NULL);
	WriteFile(file, m_outFile, MAX_PATH, &written, NULL);
	CloseHandle(file);
}

void ProfileManager::Execute(NhoProcessor* processor)
{
	if (!processor->Load(m_inFile))
	{
		std::cout << "Could not load input file...\n";
		return;
	}
	PREPROCESS_ERROR error = processor->PreProcess();
	if (error == SUCCESS)
	{
		processor->Process();
		std::cout << "Processing finished successfully.\n";
	}
	else
	{
		std::cout << "Pre-process errors found, processing terminated...\n";
	}
}
