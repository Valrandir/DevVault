#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>

#define WAVE_FILE_PATH TEXT("C:\\Users\\Valrandir\\Desktop\\OneFile\\Alarm05.wav")
typedef unsigned short ushort;
typedef unsigned int uint;

void fail()
{
	DWORD last_error = GetLastError();

	DWORD const buffer_size = 0x200;
	TCHAR buffer[buffer_size];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_error, 0, buffer, buffer_size, NULL);

	MessageBox(0, buffer, TEXT("Fail"), MB_OK);
	ExitProcess(1);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = TEXT("RUFF_WAVE");
	RegisterClassEx(&wc);
	return CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, hInstance, NULL);
}

void LoopWindow()
{
	MSG msg;

	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	HRESULT hResult;
	BOOL result;
	HWND hWnd;

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

	if(ruff_wave.chunk_id != 0x46464952 /*RIFF*/) fail();
	if(ruff_wave.format != 0x45564157 /*WAVE*/) fail();
	if(ruff_wave.chunk_fmt_id != 0x20746d66 /*fmt*/) fail();
	if(ruff_wave.chunk_data_id != 0x61746164 /*data*/) fail();

	ruff_wave.data = malloc(ruff_wave.chunk_data_size);
	if(ruff_wave.data == 0) fail();

	bytes_to_read = ruff_wave.chunk_data_size;
	result = ReadFile(hFile, ruff_wave.data, bytes_to_read, &bytes_read, NULL);
	if(!result && bytes_read != bytes_to_read) fail();

	CloseHandle(hFile);
	hFile = NULL;

	WAVEFORMATEX wave_format = {0};
	wave_format.wFormatTag = ruff_wave.audio_format;
	wave_format.nChannels = ruff_wave.num_channel;
	wave_format.nSamplesPerSec = ruff_wave.sample_rate;
	wave_format.nAvgBytesPerSec = ruff_wave.block_align * ruff_wave.sample_rate;
	wave_format.nBlockAlign = ruff_wave.block_align;
	wave_format.wBitsPerSample = ruff_wave.bits_per_sample;

	//Create Window
	hWnd = InitWindow(hInstance);

	//Init DirectSound
	IDirectSound8* lpDirectSound = NULL;
	hResult = DirectSoundCreate8(NULL, &lpDirectSound, NULL);
	if(hResult != DS_OK) fail();

	hResult = lpDirectSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
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
	LoopWindow();

	//Free
	lpDirectSoundBuffer->Release();
	lpDirectSound->Release();
	free(ruff_wave.data);
	ruff_wave.data = NULL;

	return 0;
}
