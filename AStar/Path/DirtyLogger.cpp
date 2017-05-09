#include "DirtyLogger.h"

namespace Logger
{
	HANDLE hFile = NULL;
	DWORD LastTick, Tick;
	DWORD ID = 0;

	void Start()
	{
		LastTick = GetTickCount();
	}

	void Stop()
	{
		Tick = GetTickCount() - LastTick;
	}

	void LogTick(LPCSTR Message)
	{
		char Buffer[128];
		wsprintfA(Buffer, "%u\t%s\t%u\r\n", ++ID, Message, Tick);
		Log(Buffer);
	}

	void Log(LPCSTR Message)
	{
		DWORD BytesToWrite, BytesWritten;

		if(!hFile)
			return;

		BytesToWrite = lstrlenA(Message);
		WriteFile(hFile, Message, BytesToWrite, &BytesWritten, NULL);
	}

	void Open(LPCTSTR Filename)
	{
		if(hFile)	
			Close();
		hFile = CreateFile(Filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	void Close()
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
}
