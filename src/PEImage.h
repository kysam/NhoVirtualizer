#pragma once
#include "common/common.h"
#include <vector>

class PEImage
{
public:
	PEImage();
	~PEImage();

	struct Section {
		uchar* data;
		unsigned int size;
	};

	IMAGE_DOS_HEADER m_dosHeader;
	uchar* m_subData; int m_subDataSize;

	IMAGE_NT_HEADERS m_ntHeaders;
	std::vector<IMAGE_SECTION_HEADER> m_sectionHeaders;
	std::vector<Section> m_sections;

	bool Load(const char* fileName);
	bool Rebuild(const char* fileName);
	void AddSection(const char* name, uchar* data, int dataSize, DWORD charac);
	static DWORD Align(DWORD value, DWORD align);
};

