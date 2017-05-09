#include "LoadSave.h"

LoadSave::LoadSave()
{
}

void LoadSave::SaveWorld(LPCTSTR FileName, const World *lpWorld)
{
	HANDLE hFile;
	DWORD NumberOfBytesWritten;

	hFile = CreateFile(FileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	WriteFile(hFile, (void*)lpWorld->vGrid, lpWorld->nGridBytes, &NumberOfBytesWritten, NULL);
	CloseHandle(hFile);
}

void LoadSave::LoadWorld(LPCTSTR FileName, World *lpWorld)
{
	HANDLE hFile;
	DWORD NumberOfBytesRead;

	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	ReadFile(hFile, (void*)lpWorld->vGrid, lpWorld->nGridBytes, &NumberOfBytesRead, NULL);
	CloseHandle(hFile);
}
