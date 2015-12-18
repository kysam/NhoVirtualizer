#pragma once
#include "common/common.h"

class PEMaster
{
public:
	PEMaster();
	~PEMaster();

	struct Section
	{
		unsigned char* data;
		unsigned int size;
	};

	IMAGE_DOS_HEADER m_dosHeader;
	int m_sectionCount;
	IMAGE_NT_HEADERS m_ntHeaders;
	IMAGE_SECTION_HEADER* m_sectionHeaders;
	Section* m_sections;

	bool Load(const char* fileName);
	bool Rebuild(const char* fileName);
};

