#pragma once
#include "common/common.h"
#include <vector>
#include "NhoVM.h"

class Translator
{
public:
	Translator();
	~Translator();

	void Translate(uchar* native, int nativeSize, std::vector<NhoInstr>* nhos);
};

