#include "PEImage.h"

PEImage::PEImage()
{
}


PEImage::~PEImage()
{
}

bool PEImage::Load(const char* fileName)
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

	int sectionCount = m_ntHeaders.FileHeader.NumberOfSections;
	uchar* headerMem = CASTOFFSET(uchar, fileView, m_dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS));
	IMAGE_SECTION_HEADER header;
	Section section;

	for (int i = 0; i < sectionCount; i++) {
		
		memcpy(&header, headerMem, sizeof(IMAGE_SECTION_HEADER));
		m_sectionHeaders.push_back(header);

		headerMem += sizeof(IMAGE_SECTION_HEADER);
	}

	for (int iSec = 0; iSec < sectionCount; iSec++) {
		
		uchar* data = new uchar[m_sectionHeaders[iSec].SizeOfRawData];
		memcpy(data, CASTOFFSET(IMAGE_DOS_HEADER, fileView, m_sectionHeaders[iSec].PointerToRawData),
			   m_sectionHeaders[iSec].SizeOfRawData);

		section.data = data;
		section.size = m_sectionHeaders[iSec].SizeOfRawData;
		m_sections.push_back(section);
	}

	return true;
}

bool PEImage::Rebuild(const char* fileName)
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
	int sectionCount = m_ntHeaders.FileHeader.NumberOfSections;

	for (int iSec = 0; iSec < sectionCount; iSec++) {
		WriteFile(hFile, &m_sectionHeaders[iSec], sizeof(IMAGE_SECTION_HEADER), &written, NULL);
	}

	for (int iSec = 0; iSec < sectionCount; iSec++) {
		SetFilePointer(hFile, m_sectionHeaders[iSec].PointerToRawData, 0, FILE_BEGIN);
		WriteFile(hFile, m_sections[iSec].data, m_sectionHeaders[iSec].SizeOfRawData, &written, NULL);
	}

	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return true;
}

void PEImage::AddSection(const char* name, uchar* data, int dataSize, DWORD charac)
{
	IMAGE_SECTION_HEADER& lastHeader = m_sectionHeaders[m_ntHeaders.FileHeader.NumberOfSections - 1];
	IMAGE_SECTION_HEADER header;

	memset(&header, 0, sizeof(header));
	memcpy(&header.Name, name, sizeof(header.Name));

	header.Misc.PhysicalAddress = dataSize;
	header.Misc.VirtualSize = dataSize;
	header.NumberOfLinenumbers = 0;

	header.NumberOfRelocations = 0;
	header.Characteristics = charac;
	header.PointerToRawData = Align(lastHeader.PointerToRawData + lastHeader.SizeOfRawData, m_ntHeaders.OptionalHeader.FileAlignment);

	header.SizeOfRawData = Align(dataSize, m_ntHeaders.OptionalHeader.FileAlignment);
	header.VirtualAddress = Align(lastHeader.VirtualAddress + lastHeader.Misc.VirtualSize, m_ntHeaders.OptionalHeader.SectionAlignment);

	Section section;
	section.data = data;
	section.size = dataSize;

	m_sectionHeaders.push_back(header);
	m_sections.push_back(section);
	m_ntHeaders.FileHeader.NumberOfSections++;

	m_ntHeaders.OptionalHeader.SizeOfImage = Align(header.VirtualAddress + header.Misc.VirtualSize, m_ntHeaders.OptionalHeader.SectionAlignment);

	//not needed?
//	m_ntHeaders.OptionalHeader.SizeOfInitializedData += header.Misc.VirtualSize;
//	m_ntHeaders.OptionalHeader.SizeOfHeaders = Align(m_ntHeaders.OptionalHeader.SizeOfHeaders + sizeof(IMAGE_SECTION_HEADER), m_ntHeaders.OptionalHeader.FileAlignment);
}

DWORD PEImage::Align(DWORD value, DWORD align)
{
	DWORD surplus = (value % align);
	if (surplus == 0)
		return value;

	return (value - surplus + align);
}
