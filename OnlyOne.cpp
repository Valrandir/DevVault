#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>

#define WAVE_FILE_PATH TEXT("C:\\Users\\Valrandir\\Desktop\\OneFile\\Alarm05.wav")
typedef unsigned short ushort;
typedef unsigned int uint;

void fail()
{
	auto text = TEXT("Fail");
	MessageBox(0, text, text, MB_OK);
	ExitProcess(1);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	IDirectSound8* lpDirectSound = NULL;
	HRESULT hResult;
	BOOL result;

	//Init DirectSound
	hResult = DirectSoundCreate8(NULL, &lpDirectSound, NULL);
	if(hResult != DS_OK) fail();

	hResult = lpDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL);
	if(hResult != DS_OK) fail();

	//Load Wave File
	struct RuffWaveType
	{
		uint chunk_id, chunk_size, format;
		uint chunk_fmt_id, chunk_fmt_size;
		ushort audio_format, num_channel;
		uint sample_rate, byte_rate;
		ushort block_align, bits_per_sample;
		uint chunk_data_id, chunk_data_size;
		void* data;
	}ruff_wave;

	HANDLE hFile = CreateFile(WAVE_FILE_PATH, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile == INVALID_HANDLE_VALUE) fail();

	DWORD bytes_to_read = sizeof(ruff_wave) - sizeof(void*);
	DWORD bytes_read;
	result = ReadFile(hFile, &ruff_wave, bytes_to_read, &bytes_read, NULL);
	if(!result && bytes_read != bytes_to_read) fail();

	ruff_wave.data = malloc(ruff_wave.chunk_data_size);
	if(ruff_wave.data == 0) fail();

	bytes_to_read = ruff_wave.chunk_data_size;
	result = ReadFile(hFile, &ruff_wave.data, bytes_to_read, &bytes_read, NULL);
	if(!result && bytes_read != bytes_to_read) fail();

	CloseHandle(hFile);
	hFile = NULL;

	free(ruff_wave.data);
	ruff_wave.data = NULL;

	return 0;
}
