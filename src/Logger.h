#pragma once
class Logger
{
public:
	Logger();
	~Logger();

	FILE* m_file;

	bool New(const char* fileName);
	template<typename ARG& ... args>
	void Log(const char* format, args);
};

template<typename ... ARGS>
void Logger::Log(const char* format, ARGS& ... args)
{
	fprintf(m_file, format, args);
	fprintf("\n");
}

