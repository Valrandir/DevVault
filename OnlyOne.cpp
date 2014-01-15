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
	HRESULT hResult;
	BOOL result;

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

	if(ruff_wave.chunk_id != 0x52494646 /*RIFF*/) fail();
	if(ruff_wave.format != 0x57415645 /*WAVE*/) fail();
	if(ruff_wave.chunk_fmt_id != 0x666d7420 /*fmt*/) fail();

	ruff_wave.data = malloc(ruff_wave.chunk_data_size);
	if(ruff_wave.data == 0) fail();

	bytes_to_read = ruff_wave.chunk_data_size;
	result = ReadFile(hFile, &ruff_wave.data, bytes_to_read, &bytes_read, NULL);
	if(!result && bytes_read != bytes_to_read) fail();

	CloseHandle(hFile);
	hFile = NULL;

	WAVEFORMATEX wave_format = {0};
	wave_format.wFormatTag = ruff_wave.audio_format;
	wave_format.nChannels = ruff_wave.num_channel;
	wave_format.nSamplesPerSec = ruff_wave.sample_rate;
	wave_format.nBlockAlign = ruff_wave.block_align;
	wave_format.nAvgBytesPerSec = wave_format.nBlockAlign * wave_format.nSamplesPerSec;

	//Init DirectSound
	IDirectSound8* lpDirectSound = NULL;
	hResult = DirectSoundCreate8(NULL, &lpDirectSound, NULL);
	if(hResult != DS_OK) fail();

	hResult = lpDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL);
	if(hResult != DS_OK) fail();

	DSBUFFERDESC ds_buffer_desc = {0};
	ds_buffer_desc.dwSize = sizeof(DSBUFFERDESC);
	ds_buffer_desc.dwFlags = DSBCAPS_STATIC;
	ds_buffer_desc.dwBufferBytes = ruff_wave.chunk_data_size;
	ds_buffer_desc.lpwfxFormat = &wave_format;

	IDirectSoundBuffer* lpDirectSoundBuffer;
	hResult = lpDirectSound->CreateSoundBuffer(&ds_buffer_desc, &lpDirectSoundBuffer, NULL);
	if(hResult != DS_OK) fail();

	//Load Sound Data into DirectSoundBuffer
	void* locked_buffer;
	DWORD locked_buffer_size;

	hResult = lpDirectSoundBuffer->Lock(0, 0, &locked_buffer, &locked_buffer_size, 0, 0, DSBLOCK_ENTIREBUFFER);
	if(hResult != DS_OK) fail();
	memcpy(locked_buffer, ruff_wave.data, locked_buffer_size);
	hResult = lpDirectSoundBuffer->Unlock(locked_buffer, locked_buffer_size, 0, 0);
	if(hResult != DS_OK) fail();

	//Play
	hResult = lpDirectSoundBuffer->Play(NULL, 0, 0);
	if(hResult != DS_OK) fail();

	//Wait
	Sleep(12000);

	//Free
	lpDirectSoundBuffer->Release();
	lpDirectSound->Release();
	free(ruff_wave.data);
	ruff_wave.data = NULL;

	return 0;
}
