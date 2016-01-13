#include "NhoProcessor.h"
#include "global.h"
#include "common/common.h"

NhoProcessor::NhoProcessor(int keyBegin, int keyEnd)
{
	m_markerKeyBegin = keyBegin;
	m_markerKeyEnd = keyEnd;
}

NhoProcessor::~NhoProcessor()
{
}

bool NhoProcessor::Load(const char* fileName)
{
	if (!m_pe.Load(fileName))
		return false;
}

PREPROCESS_ERROR NhoProcessor::PreProcess()
{
	auto & headers = m_pe.m_sectionHeaders;
	auto & sections = m_pe.m_sections;
	VMTargetGroup tgroup;
	VMTarget target;
	m_targetGroups.clear();

	for (int i = 0; i < headers.size(); i++) 
	{
		if ( !(headers[i].Characteristics & IMAGE_SCN_CNT_CODE) )
			continue;

		uchar* code = sections[i].data;
		while (true) {
			if ((unsigned int)(code + MARKER_SIZE) > sections[i].size)
				break;

			WORD cWord = *(WORD*)(code);
			DWORD cDword = *(DWORD*)(code + sizeof(WORD));
			
			if (cWord != MARKER_JMP_SHORT)
				continue;

			if (cDword == m_markerKeyBegin) {
				if (target.beginOffset == -1)
					target.beginOffset = cWord;
				else {
					LOG("Mismatch markers found at offset: %X", (DWORD)code);
					return MISMATCH_FOUND;
				}
			}
			else if (cWord == m_markerKeyEnd) {
				if (target.endOffset == -1)
					target.endOffset = cWord;
				else {
					LOG("Mismatch markers found at offset: %X", (DWORD)code);
					return MISMATCH_FOUND;
				}
			}

			tgroup.targets.push_back(target);
			target.beginOffset = -1;
			target.endOffset = -1;

			code += MARKER_SIZE;
		}

		tgroup.section = i;
		m_targetGroups.push_back(tgroup);
		tgroup.targets.clear();
	}
	
	return SUCCESS;
}


void NhoProcessor::Process()
{
	m_pe.AddSection(".nho", nullptr, 0, IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_INITIALIZED_DATA); //empty section
	PEImage::Section* newSection = &m_pe.m_sections[m_pe.m_sections.size() - 1];

	for (int iG = 0; iG < m_targetGroups.size(); iG++)
	{
		auto & targets = m_targetGroups[iG].targets;
		std::vector<NhoInstr> nhos;
		for (int iT = 0; iT < targets.size(); iT++)
		{
			uchar* codeBegin = (uchar*)(targets[iT].beginOffset + MARKER_SIZE);
			int codeSize = targets[iT].endOffset - targets[iT].beginOffset - MARKER_SIZE;
			m_translator.Translate(codeBegin, codeSize, &nhos);
			memcpy(codeBegin, &nhos[0], nhos.size() * sizeof(NhoInstr));
		}
		
	}
}

