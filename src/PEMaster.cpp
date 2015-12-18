#include "PEMaster.h"
#include <stdio.h>

PEMaster::PEMaster()
{
}


PEMaster::~PEMaster()
{
}

bool PEMaster::Load(const char* fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	uchar* fileView = (uchar*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (fileView == nullptr)
		return false;

	CloseHandle(hFile);
	memcpy(&m_dosHeader, CASTOFFSET(IMAGE_DOS_HEADER, fileView, 0), sizeof(IMAGE_DOS_HEADER));
	if (m_dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		return false;	//not PE

	m_subDataSize = m_dosHeader.e_lfanew - sizeof(IMAGE_DOS_HEADER);
	m_subData = new uchar[m_subDataSize];
	memcpy(m_subData, CASTOFFSET(IMAGE_NT_HEADERS, fileView, sizeof(IMAGE_DOS_HEADER)), m_subDataSize);

	memcpy(&m_ntHeaders, CASTOFFSET(IMAGE_NT_HEADERS, fileView, m_dosHeader.e_lfanew), sizeof(IMAGE_NT_HEADERS));
	if (m_ntHeaders.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return false;	//not PE x32

	m_sectionCount = m_ntHeaders.FileHeader.NumberOfSections;
	m_sectionHeaders = new IMAGE_SECTION_HEADER[m_sectionCount];
	memcpy(m_sectionHeaders, CASTOFFSET(IMAGE_DOS_HEADER, fileView, m_dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS)),
		sizeof(IMAGE_SECTION_HEADER)* m_ntHeaders.FileHeader.NumberOfSections);

	
	m_sections = new Section[m_sectionCount];
	for (int iSec = 0; iSec < m_sectionCount; iSec++)
	{
		uchar* data = new uchar[m_sectionHeaders[iSec].SizeOfRawData];
		memcpy(data, CASTOFFSET(IMAGE_DOS_HEADER, fileView, m_sectionHeaders[iSec].PointerToRawData),
			m_sectionHeaders[iSec].SizeOfRawData);
		m_sections[iSec].data = data;
		m_sections[iSec].size = m_sectionHeaders[iSec].SizeOfRawData;
	}

	return true;
}

bool PEMaster::Rebuild(const char* fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	DWORD written;
	WriteFile(hFile, &m_dosHeader, sizeof(m_dosHeader), &written, NULL);
	WriteFile(hFile, m_subData, m_subDataSize, &written, NULL);
	WriteFile(hFile, &m_ntHeaders, sizeof(m_ntHeaders), &written, NULL);
	
	for (int iSec = 0; iSec < m_sectionCount; iSec++)
	{
		WriteFile(hFile, &m_sectionHeaders[iSec], sizeof(IMAGE_SECTION_HEADER), &written, NULL);
	}

	for (int iSec = 0; iSec < m_sectionCount; iSec++)
	{
		SetFilePointer(hFile, m_sectionHeaders[iSec].PointerToRawData, 0, FILE_BEGIN);
		WriteFile(hFile, m_sections[iSec].data, m_sections[iSec].size, &written, NULL);
	}

	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return true;
}
