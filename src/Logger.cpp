#include "Logger.h"
#include <stdio.h>

Logger::Logger()
{
	
}

Logger::~Logger()
{
	if (m_file)
		fclose(m_file);
}

bool Logger::New(const char* fileName)
{
	m_file = fopen(fileName, "a+");
	if (m_file == nullptr)
		return false;
}
