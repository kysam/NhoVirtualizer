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
			if ((unsigned int)(code + 6) > sections[i].size)
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

			code += 6;
		}

		tgroup.section = i;
		m_targetGroups.push_back(tgroup);
		tgroup.targets.clear();
	}
	
	return SUCCESS;
}

