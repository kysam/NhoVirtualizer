#pragma once
#include <vector>
#include "common/common.h"
#include "PEImage.h"
#include "Translator.h"

#define MARKER_JMP_SHORT 0x09EB
#define MARKER_SIZE 0x9

enum PREPROCESS_ERROR {
	SUCCESS, MISMATCH_FOUND
};

class NhoProcessor
{
public:
	NhoProcessor(int keyBegin = 0x80808080, int keyEnd = 0x81818181);
	~NhoProcessor();

	struct VMTarget {
		unsigned long beginOffset;
		unsigned long endOffset;

		VMTarget() {
			beginOffset = -1;
			endOffset = -1;
		}
	};

	struct VMTargetGroup {
		int section;
		std::vector<VMTarget> targets;
	};

	unsigned int m_markerKeyBegin;
	unsigned int m_markerKeyEnd;
	PEImage m_pe;
	std::vector<VMTargetGroup> m_targetGroups;
	Translator m_translator;

	bool Load(const char* fileName);
	PREPROCESS_ERROR PreProcess();
	void Process();
};

