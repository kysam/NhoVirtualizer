#include "PEMaster.h"


PEMaster::PEMaster()
{
}


PEMaster::~PEMaster()
{
}

bool PEMaster::Load(const char* fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	unsigned char* fileView = (unsigned char*)MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (fileView == nullptr)
		return false;
	CloseHandle(hFile);

	memcpy(&m_dosHeader, CASTOFFSET(IMAGE_DOS_HEADER, fileView, 0), sizeof(IMAGE_DOS_HEADER));
	if (m_dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		return false;	//not PE

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
		unsigned char* data = new unsigned char[m_sectionHeaders[iSec].SizeOfRawData];
		m_sections[iSec].data = data;
		m_sections[iSec].size = m_sectionHeaders[iSec].SizeOfRawData;
	}

}

bool PEMaster::Rebuild(const char* fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD written;
	WriteFile(hFile, &m_dosHeader, sizeof(m_dosHeader), &written, NULL);
	WriteFile(hFile, &m_ntHeaders, sizeof(m_ntHeaders), &written, NULL);
	
	for (int iSec = 0; iSec < m_sectionCount; iSec++)
	{
		WriteFile(hFile, &m_sectionHeaders[iSec], sizeof(IMAGE_SECTION_HEADER), &written, NULL);
	}

	for (int iSec = 0; iSec < m_sectionCount; iSec++)
	{
		WriteFile(hFile, m_sections[iSec].data, m_sections[iSec].size, &written, NULL);
	}

	SetEndOfFile(hFile);
	CloseHandle(hFile);
}
