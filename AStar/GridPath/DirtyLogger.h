#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef DirtyLogger
	#define LOG_OPEN(Text) Logger::Open(TEXT(Text))
	#define LOG_CLOSE Logger::Close()
	#define LOG_BEGIN Logger::Start()
	#define LOG_END(Text) 	Logger::Stop(); 	Logger::LogTick(Text)
#else
	#define LOG_OPEN
	#define LOG_CLOSE
	#define LOG_BEGIN
	#define LOG_END
#endif

namespace Logger
{
	void Start();
	void Stop();
	void LogTick(LPCSTR Message);
	void Log(LPCSTR Message);
	void Open(LPCTSTR Filename);
	void Close();
}
