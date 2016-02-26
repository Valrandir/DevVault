#ifndef _UNICODE
	#define _UNICODE
#endif

#ifndef UNICODE
	#define UNICODE
#endif

#include <stdlib.h>
#include <math.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <gl/GL.h>
#include "libs/OpenGL/glcorearb.h"
#include "libs/OpenGL/glext.h"
#include "libs/OpenGL/wglext.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")

typedef struct Matrix4_t
{
	GLfloat v[16];
}Matrix4;

float degree_to_radian(float degree);
Matrix4 mat4_mul(Matrix4 a, Matrix4 b);
Matrix4 mat4_make_identity();
Matrix4 mat4_make_translation(float x, float y, float z);
Matrix4 mat4_make_scale(float x, float y, float z);
Matrix4 mat4_make_rotation_x(float angle_degree);
Matrix4 mat4_make_rotation_y(float angle_degree);
Matrix4 mat4_make_rotation_z(float angle_degree);

LRESULT CALLBACK window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init_window_class();
void init_gl_extensions();
void init_gl_window();
void destroy_gl_window();
BOOL process_window_messages();
GLuint create_shader(GLenum type, const char* shader_code);
void destroy_shader(GLuint shader_id);
GLuint create_program(const GLuint* vec_shader_id, int count);
void destroy_program(GLuint program_id);
void shaders_init();
void shaders_destroy();
void scene_init();
void scene_destroy();
void scene_render();
void scene_present();

unsigned char* get_texture_from_png_file(int* width, int* height);
float* get_texture_data(int* width, int* height);

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 640;

WNDCLASSEX _window_class;

//Initialization
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

//Vertex Buffers
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

//Shaders
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

HWND _hwnd;
HDC _hdc;
HGLRC _context;
GLuint _vertex_buffer;
GLuint _texture_data;
GLuint _program_id;

Matrix4 mat4_make_identity()
{
	Matrix4 mat =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return mat;
}

Matrix4 mat4_make_translation(float x, float y, float z)
{
	Matrix4 mat =
	{
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1,
	};
	return mat;
}

Matrix4 mat4_make_scale(float x, float y, float z)
{
	Matrix4 mat =
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1,
	};
	return mat;
}

Matrix4 mat4_make_rotation_x(float angle_degree)
{
	float rad = degree_to_radian(angle_degree);
	float s = sinf(rad);
	float c = cosf(rad);
	Matrix4 mat =
	{
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1,
	};
	return mat;
}

Matrix4 mat4_make_rotation_y(float angle_degree) 
{
	float rad = degree_to_radian(angle_degree);
	float s = sinf(rad);
	float c = cosf(rad);
	Matrix4 mat =
	{
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1,
	};
	return mat;
}

Matrix4 mat4_make_rotation_z(float angle_degree) 
{
	float rad = degree_to_radian(angle_degree);
	float s = sinf(rad);
	float c = cosf(rad);
	Matrix4 mat =
	{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return mat;
}

float degree_to_radian(float degree)
{
	return degree * 0.01745329251994329576923690768489f;
}

Matrix4 mat4_mul(Matrix4 transform, Matrix4 current)
{
	Matrix4 mat;
	const int size = 4;
	for(auto y = 0; y < size; ++y)
		for(auto x = 0; x < size; ++x)
		{
			float sum = 0;
			for(int i = 0; i < size; ++i)
				sum += transform.v[y * size + i] * current.v[i * size + x];
			mat.v[y * size + x] = sum;
		}

	return mat;
}

LRESULT CALLBACK window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_KEYDOWN:
			if(LOWORD(wParam) != VK_ESCAPE)
				break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void init_window_class()
{
	WNDCLASSEX* wc = &_window_class;
	wc->cbSize = sizeof(_window_class);
	wc->style = CS_OWNDC;
	wc->lpfnWndProc = window_proc;
	wc->hInstance = GetModuleHandle(NULL);
	wc->hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc->hCursor = LoadCursor(NULL, IDC_ARROW);
	wc->hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wc->lpszClassName = L"VoidGl";
	RegisterClassEx(wc);
}

void init_gl_extensions()
{
	HWND hwnd = CreateWindow(_window_class.lpszClassName, NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, _window_class.hInstance, NULL);
	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pixel_format_descriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24, 8, 0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	int pixel_format = ChoosePixelFormat(hdc, &pixel_format_descriptor);
	SetPixelFormat(hdc, pixel_format, NULL);
	HGLRC wgl_context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, wgl_context);

	#define WGL_GETPROC(type, name) name = (type)(wglGetProcAddress(#name))
	WGL_GETPROC(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
	WGL_GETPROC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);

	WGL_GETPROC(PFNGLGENBUFFERSPROC, glGenBuffers);
	WGL_GETPROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	WGL_GETPROC(PFNGLBINDBUFFERPROC, glBindBuffer);
	WGL_GETPROC(PFNGLBUFFERDATAPROC, glBufferData);
	WGL_GETPROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
	WGL_GETPROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
	WGL_GETPROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

	WGL_GETPROC(PFNGLCREATESHADERPROC, glCreateShader);
	WGL_GETPROC(PFNGLDELETESHADERPROC, glDeleteShader);
	WGL_GETPROC(PFNGLSHADERSOURCEPROC, glShaderSource);
	WGL_GETPROC(PFNGLCOMPILESHADERPROC, glCompileShader);
	WGL_GETPROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
	WGL_GETPROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
	WGL_GETPROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
	WGL_GETPROC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
	WGL_GETPROC(PFNGLATTACHSHADERPROC, glAttachShader);
	WGL_GETPROC(PFNGLDETACHSHADERPROC, glDetachShader);
	WGL_GETPROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
	WGL_GETPROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
	WGL_GETPROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
	WGL_GETPROC(PFNGLUSEPROGRAMPROC, glUseProgram);
	WGL_GETPROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
	WGL_GETPROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(wgl_context);
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

	MSG msg;
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE));
}

void init_gl_window()
{
	const DWORD style = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
	RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect(&rect, style, FALSE);
	const int w = rect.right - rect.left;
	const int h = rect.bottom - rect.top;
	const int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	const int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	_hwnd = CreateWindow(_window_class.lpszClassName, NULL, style, x, y, w, h, HWND_DESKTOP, NULL, _window_class.hInstance, NULL);
	_hdc = GetDC(_hwnd);

	int pf_attrib[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, 1,
		WGL_DOUBLE_BUFFER_ARB, 1,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	int pixel_format, num;
	wglChoosePixelFormatARB(_hdc, pf_attrib, NULL, 1, &pixel_format, &num);
	SetPixelFormat(_hdc, pixel_format, NULL);

	int c_attrib = 0;
	_context = wglCreateContextAttribsARB(_hdc, NULL, &c_attrib);
	wglMakeCurrent(_hdc, _context);

	SetWindowTextA(_hwnd, (char*)glGetString(GL_VERSION));
	ShowWindow(_hwnd, SW_SHOW);
}

void destroy_gl_window()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(_context);
	ReleaseDC(_hwnd, _hdc);
	DestroyWindow(_hwnd);
}

BOOL process_window_messages()
{
	MSG msg;
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		if(msg.message == WM_QUIT)
			return FALSE;
	}

	return TRUE;
}

GLuint create_shader(GLenum type, const char* shader_code)
{
	GLuint shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &shader_code, NULL);
	glCompileShader(shader_id);

	GLint info_log_length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if(info_log_length > 1)
	{
		GLsizei length;
		GLchar* buffer = (GLchar*)malloc(info_log_length);
		glGetShaderInfoLog(shader_id, info_log_length, &length, buffer);
		MessageBoxA(_hwnd, buffer, "Shader info log", MB_OK | MB_ICONEXCLAMATION);
		free(buffer);
		exit(1);
	}

	return shader_id;
}

void destroy_shader(GLuint shader_id)
{
	glDeleteShader(shader_id);
}

GLuint create_program(const GLuint* vec_shader_id, int count)
{
	const GLuint* it, *end = vec_shader_id + count;

	GLuint program_id = glCreateProgram();

	it = vec_shader_id;
	while(it < end)
		glAttachShader(program_id, *it++);

	glLinkProgram(program_id);
	
	GLint info_log_length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if(info_log_length > 1)
	{
		GLsizei length;
		GLchar* buffer = (GLchar*)malloc(info_log_length);
		glGetProgramInfoLog(program_id, info_log_length, &length, buffer);
		MessageBoxA(_hwnd, buffer, "Program info log", MB_OK | MB_ICONEXCLAMATION);
		free(buffer);
		exit(1);
	}

	it = vec_shader_id;
	while(it < end)
		glDetachShader(program_id, *it++);

	return program_id;
}

void destroy_program(GLuint program_id)
{
	glDeleteProgram(program_id);
}

void shaders_init()
{
	GLuint idv[] =
	{
		create_shader
		(
			GL_VERTEX_SHADER,
			"#version 330 core\n"
			"in vec3 in_vec;"
			"in vec3 in_color;"
			"in vec2 in_tex_coord;"
			"out vec3 color;"
			"out vec2 tex_coord;"
			"uniform mat4 mat_transformation;"
			"void main()"
			"{"
			"	gl_Position = vec4(in_vec, 1) * mat_transformation;"
			"	color = in_color;"
			"	tex_coord = in_tex_coord;"
			"}"
		),
		create_shader
		(
			GL_FRAGMENT_SHADER,
			"#version 330 core\n"
			"in vec3 color;"
			"in vec2 tex_coord;"
			"out vec4 out_color;"
			"uniform sampler2D tex;"
			"void main()"
			"{"
			"	out_color = texture(tex, tex_coord) * vec4(color, 1.0);"
			"}"
		)
	};

	_program_id = create_program(idv, 2);

	for(GLuint* it = idv, *end = idv + 2; it < end; ++it)
		destroy_shader(*it);

	glUseProgram(_program_id);
}

void shaders_destroy()
{
	glDeleteProgram(_program_id);
}

void scene_init()
{
	glClearColor(.2f, .2f, .2f, 1.f);
	//glEnable(GL_DEPTH_TEST);

	const float texx = 1.f;
	const float texy = 1.f;

	const float rectangle[] =
	{
		//  x      y     z,    r,    g,    b    tx,   ty
		-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, texx, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, texx, texy,
		-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, texy,
	};

	glGenBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);

	glGenTextures(1, &_texture_data);
	glBindTexture(GL_TEXTURE_2D, _texture_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* rgba_data = get_texture_from_png_file(&width, &height);
	if(rgba_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgba_data);
		free(rgba_data);
	}
	else
	{
		float* float_data = get_texture_data(&width, &height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, float_data);
	}

	const GLsizei stride = sizeof(float) * 8;
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void scene_destroy()
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteTextures(1, &_texture_data);
	glDeleteBuffers(1, &_vertex_buffer);
}

void scene_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint uniform = glGetUniformLocation(_program_id, "mat_transformation");

	static float degree = 0.f;
	degree += 0.5f;

	Matrix4 final;
	Matrix4 view = mat4_make_identity();
	view = mat4_mul(mat4_make_rotation_z(degree), view);
	view = mat4_mul(mat4_make_scale(0.5f, 0.5f, 0.0f), view);

	Matrix4 model;

	model = mat4_make_scale(2.0f, 2.0f, 0.0f);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, model.v);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

	model = mat4_make_identity();
	model = mat4_mul(mat4_make_rotation_x(-degree), model);
	model = mat4_mul(mat4_make_rotation_y(-degree), model);
	model = mat4_mul(mat4_make_translation(-0.5f, 0.5f, 0.0f), model);
	final = mat4_mul(view, model);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, final.v);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

	model = mat4_make_identity();
	model = mat4_mul(mat4_make_rotation_x(-degree), model);
	model = mat4_mul(mat4_make_rotation_y(-degree + 180), model);
	model = mat4_mul(mat4_make_translation(0.5f, 0.5f, 0.0f), model);
	final = mat4_mul(view, model);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, final.v);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

	model = mat4_make_identity();
	model = mat4_mul(mat4_make_rotation_x(degree + 180), model);
	model = mat4_mul(mat4_make_rotation_y(degree + 180), model);
	model = mat4_mul(mat4_make_translation(0.5f, -0.5f, 0.0f), model);
	final = mat4_mul(view, model);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, final.v);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

	model = mat4_make_identity();
	model = mat4_mul(mat4_make_rotation_x(degree + 180), model);
	model = mat4_mul(mat4_make_rotation_y(degree), model);
	model = mat4_mul(mat4_make_translation(-0.5f, -0.5f, 0.0f), model);
	final = mat4_mul(view, model);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, final.v);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
	
	Sleep(5);
}

void scene_present()
{
	glFlush();
	SwapBuffers(_hdc);
}

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR params, INT param_count)
{
	init_window_class();
	init_gl_extensions();
	init_gl_window();

	shaders_init();
	scene_init();

	while(process_window_messages())
	{
		scene_render();
		scene_present();
	}

	scene_destroy();
	shaders_destroy();
	destroy_gl_window();

	return 0;
}

#include "libs/libpng/png.h"

#ifdef _M_X64
	#pragma comment(lib, "libs/libpng/x64/zlib.lib")
	#pragma comment(lib, "libs/libpng/x64/libpng16.lib")
#else
	#pragma comment(lib, "libs/libpng/x86/zlib.lib")
	#pragma comment(lib, "libs/libpng/x86/libpng16.lib")
#endif

struct Buffer
{
	void* data;
	unsigned char* iterator;
};

//Return 0 on success
int read_file(const char* filename, void** out_buffer, size_t* out_size)
{
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if(hFile == INVALID_HANDLE_VALUE)
		return 1;

	*out_size = (size_t)GetFileSize(hFile, 0);
	*out_buffer = malloc(*out_size);
	ReadFile(hFile, *out_buffer, *out_size, 0, 0);
	CloseHandle(hFile);

	return 0;
}

void libpng_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
	MessageBoxA(0, error_msg, "Fatal error from libpng", MB_OK | MB_ICONERROR);
	exit(1);
}

void libpng_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
	//MessageBoxA(0, warning_msg, "Warning from libpng", MB_OK | MB_ICONWARNING);
}

void libpng_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct Buffer* buffer = png_get_io_ptr(png_ptr);
	memcpy(data, buffer->iterator, length);
	buffer->iterator += length;
}

unsigned char* get_texture_from_png_file(int* width, int* height)
{
	//http://libpng.org/pub/png/libpng-1.2.5-manual.html
	struct Buffer buffer;
	size_t size;

	if(read_file("lunar.png", &buffer.data, &size))
		return 0;

	buffer.iterator = buffer.data;

	if(size < 8 || png_sig_cmp(buffer.data, 0, 8))
		return 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, libpng_error_fn, libpng_warning_fn);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	png_set_read_fn(png_ptr, &buffer, libpng_read_data);

	const int transforms = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_GRAY_TO_RGB;
	png_read_png(png_ptr, info_ptr, transforms, NULL);

	*width = png_get_image_width(png_ptr, info_ptr);
	*height = png_get_image_height(png_ptr, info_ptr);
	const png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	const png_size_t bytes = rowbytes * *height;
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	unsigned char* rgba_data = (unsigned char*)malloc(bytes);
	unsigned char* it = rgba_data;
	for(unsigned int y = 0; y < *height; ++y)
		for(unsigned int x = 0; x < rowbytes; ++x)
			*(it++) = *(row_pointers[y] + x);

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	free(buffer.data);

	return rgba_data;
}

float* get_texture_data(int* width, int* height)
{
	*width = 32;
	*height = 32;

	//Grayscale Clouds Mosaic - 32x32 RGB
	static float texture_data[] =
	{
		0.53f,0.53f,0.53f, 0.53f,0.53f,0.53f, 0.44f,0.44f,0.44f, 0.31f,0.31f,0.31f, 0.21f,0.21f,0.21f, 0.17f,0.17f,0.17f, 0.18f,0.18f,0.18f, 0.22f,0.22f,0.22f, 0.32f,0.32f,0.32f, 0.41f,0.41f,0.41f, 0.41f,0.41f,0.41f, 0.36f,0.36f,0.36f, 0.33f,0.33f,0.33f, 0.35f,0.35f,0.35f, 0.31f,0.31f,0.31f, 0.26f,0.26f,0.26f, 0.26f,0.26f,0.26f, 0.31f,0.31f,0.31f, 0.35f,0.35f,0.35f, 0.33f,0.33f,0.33f, 0.36f,0.36f,0.36f, 0.41f,0.41f,0.41f, 0.41f,0.41f,0.41f, 0.32f,0.32f,0.32f, 0.22f,0.22f,0.22f, 0.18f,0.18f,0.18f, 0.17f,0.17f,0.17f, 0.21f,0.21f,0.21f, 0.31f,0.31f,0.31f, 0.44f,0.44f,0.44f, 0.53f,0.53f,0.53f, 0.53f,0.53f,0.53f,
		0.52f,0.52f,0.52f, 0.49f,0.49f,0.49f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.21f,0.21f,0.21f, 0.15f,0.15f,0.15f, 0.13f,0.13f,0.13f, 0.17f,0.17f,0.17f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.37f,0.37f,0.37f, 0.28f,0.28f,0.28f, 0.26f,0.26f,0.26f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.22f,0.22f,0.22f, 0.22f,0.22f,0.22f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.26f,0.26f,0.26f, 0.28f,0.28f,0.28f, 0.37f,0.37f,0.37f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.17f,0.17f,0.17f, 0.13f,0.13f,0.13f, 0.15f,0.15f,0.15f, 0.21f,0.21f,0.21f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.49f,0.49f,0.49f, 0.52f,0.52f,0.52f,
		0.52f,0.52f,0.52f, 0.45f,0.45f,0.45f, 0.37f,0.37f,0.37f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.19f,0.19f,0.19f, 0.17f,0.17f,0.17f, 0.21f,0.21f,0.21f, 0.31f,0.31f,0.31f, 0.33f,0.33f,0.33f, 0.33f,0.33f,0.33f, 0.26f,0.26f,0.26f, 0.24f,0.24f,0.24f, 0.24f,0.24f,0.24f, 0.23f,0.23f,0.23f, 0.18f,0.18f,0.18f, 0.18f,0.18f,0.18f, 0.23f,0.23f,0.23f, 0.24f,0.24f,0.24f, 0.24f,0.24f,0.24f, 0.26f,0.26f,0.26f, 0.33f,0.33f,0.33f, 0.33f,0.33f,0.33f, 0.31f,0.31f,0.31f, 0.21f,0.21f,0.21f, 0.17f,0.17f,0.17f, 0.19f,0.19f,0.19f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.37f,0.37f,0.37f, 0.45f,0.45f,0.45f, 0.52f,0.52f,0.52f,
		0.57f,0.57f,0.57f, 0.53f,0.53f,0.53f, 0.44f,0.44f,0.44f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.23f,0.23f,0.23f, 0.22f,0.22f,0.22f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.29f,0.29f,0.29f, 0.33f,0.33f,0.33f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.21f,0.21f,0.21f, 0.21f,0.21f,0.21f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.33f,0.33f,0.33f, 0.29f,0.29f,0.29f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.22f,0.22f,0.22f, 0.23f,0.23f,0.23f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.44f,0.44f,0.44f, 0.53f,0.53f,0.53f, 0.57f,0.57f,0.57f,
		0.61f,0.61f,0.61f, 0.57f,0.57f,0.57f, 0.55f,0.55f,0.55f, 0.45f,0.45f,0.45f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.27f,0.27f,0.27f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.16f,0.16f,0.16f, 0.17f,0.17f,0.17f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.17f,0.17f,0.17f, 0.16f,0.16f,0.16f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.27f,0.27f,0.27f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.45f,0.45f,0.45f, 0.55f,0.55f,0.55f, 0.57f,0.57f,0.57f, 0.61f,0.61f,0.61f,
		0.61f,0.61f,0.61f, 0.60f,0.60f,0.60f, 0.62f,0.62f,0.62f, 0.50f,0.50f,0.50f, 0.37f,0.37f,0.37f, 0.30f,0.30f,0.30f, 0.31f,0.31f,0.31f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.33f,0.33f,0.33f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.27f,0.27f,0.27f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.17f,0.17f,0.17f, 0.17f,0.17f,0.17f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.27f,0.27f,0.27f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.33f,0.33f,0.33f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.31f,0.31f,0.31f, 0.30f,0.30f,0.30f, 0.37f,0.37f,0.37f, 0.50f,0.50f,0.50f, 0.62f,0.62f,0.62f, 0.60f,0.60f,0.60f, 0.61f,0.61f,0.61f,
		0.64f,0.64f,0.64f, 0.59f,0.59f,0.59f, 0.62f,0.62f,0.62f, 0.51f,0.51f,0.51f, 0.40f,0.40f,0.40f, 0.34f,0.34f,0.34f, 0.38f,0.38f,0.38f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.42f,0.42f,0.42f, 0.37f,0.37f,0.37f, 0.38f,0.38f,0.38f, 0.32f,0.32f,0.32f, 0.24f,0.24f,0.24f, 0.20f,0.20f,0.20f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.20f,0.20f,0.20f, 0.24f,0.24f,0.24f, 0.32f,0.32f,0.32f, 0.38f,0.38f,0.38f, 0.37f,0.37f,0.37f, 0.42f,0.42f,0.42f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.38f,0.38f,0.38f, 0.34f,0.34f,0.34f, 0.40f,0.40f,0.40f, 0.51f,0.51f,0.51f, 0.62f,0.62f,0.62f, 0.59f,0.59f,0.59f, 0.64f,0.64f,0.64f,
		0.65f,0.65f,0.65f, 0.62f,0.62f,0.62f, 0.61f,0.61f,0.61f, 0.52f,0.52f,0.52f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.46f,0.46f,0.46f, 0.50f,0.50f,0.50f, 0.51f,0.51f,0.51f, 0.47f,0.47f,0.47f, 0.47f,0.47f,0.47f, 0.41f,0.41f,0.41f, 0.32f,0.32f,0.32f, 0.31f,0.31f,0.31f, 0.30f,0.30f,0.30f, 0.30f,0.30f,0.30f, 0.31f,0.31f,0.31f, 0.32f,0.32f,0.32f, 0.41f,0.41f,0.41f, 0.47f,0.47f,0.47f, 0.47f,0.47f,0.47f, 0.51f,0.51f,0.51f, 0.50f,0.50f,0.50f, 0.46f,0.46f,0.46f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.52f,0.52f,0.52f, 0.61f,0.61f,0.61f, 0.62f,0.62f,0.62f, 0.65f,0.65f,0.65f,
		0.63f,0.63f,0.63f, 0.60f,0.60f,0.60f, 0.58f,0.58f,0.58f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.49f,0.49f,0.49f, 0.48f,0.48f,0.48f, 0.52f,0.52f,0.52f, 0.51f,0.51f,0.51f, 0.53f,0.53f,0.53f, 0.57f,0.57f,0.57f, 0.54f,0.54f,0.54f, 0.45f,0.45f,0.45f, 0.41f,0.41f,0.41f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.41f,0.41f,0.41f, 0.45f,0.45f,0.45f, 0.54f,0.54f,0.54f, 0.57f,0.57f,0.57f, 0.53f,0.53f,0.53f, 0.51f,0.51f,0.51f, 0.52f,0.52f,0.52f, 0.48f,0.48f,0.48f, 0.49f,0.49f,0.49f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.58f,0.58f,0.58f, 0.60f,0.60f,0.60f, 0.63f,0.63f,0.63f,
		0.62f,0.62f,0.62f, 0.58f,0.58f,0.58f, 0.52f,0.52f,0.52f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.54f,0.54f,0.54f, 0.54f,0.54f,0.54f, 0.50f,0.50f,0.50f, 0.54f,0.54f,0.54f, 0.58f,0.58f,0.58f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.55f,0.55f,0.55f, 0.52f,0.52f,0.52f, 0.44f,0.44f,0.44f, 0.44f,0.44f,0.44f, 0.52f,0.52f,0.52f, 0.55f,0.55f,0.55f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.58f,0.58f,0.58f, 0.54f,0.54f,0.54f, 0.50f,0.50f,0.50f, 0.54f,0.54f,0.54f, 0.54f,0.54f,0.54f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.52f,0.52f,0.52f, 0.58f,0.58f,0.58f, 0.62f,0.62f,0.62f,
		0.60f,0.60f,0.60f, 0.57f,0.57f,0.57f, 0.52f,0.52f,0.52f, 0.49f,0.49f,0.49f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.62f,0.62f,0.62f, 0.66f,0.66f,0.66f, 0.70f,0.70f,0.70f, 0.68f,0.68f,0.68f, 0.65f,0.65f,0.65f, 0.60f,0.60f,0.60f, 0.56f,0.56f,0.56f, 0.46f,0.46f,0.46f, 0.46f,0.46f,0.46f, 0.56f,0.56f,0.56f, 0.60f,0.60f,0.60f, 0.65f,0.65f,0.65f, 0.68f,0.68f,0.68f, 0.70f,0.70f,0.70f, 0.66f,0.66f,0.66f, 0.62f,0.62f,0.62f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.49f,0.49f,0.49f, 0.52f,0.52f,0.52f, 0.57f,0.57f,0.57f, 0.60f,0.60f,0.60f,
		0.67f,0.67f,0.67f, 0.60f,0.60f,0.60f, 0.55f,0.55f,0.55f, 0.51f,0.51f,0.51f, 0.59f,0.59f,0.59f, 0.66f,0.66f,0.66f, 0.75f,0.75f,0.75f, 0.77f,0.77f,0.77f, 0.74f,0.74f,0.74f, 0.72f,0.72f,0.72f, 0.73f,0.73f,0.73f, 0.70f,0.70f,0.70f, 0.64f,0.64f,0.64f, 0.59f,0.59f,0.59f, 0.53f,0.53f,0.53f, 0.45f,0.45f,0.45f, 0.45f,0.45f,0.45f, 0.53f,0.53f,0.53f, 0.59f,0.59f,0.59f, 0.64f,0.64f,0.64f, 0.70f,0.70f,0.70f, 0.73f,0.73f,0.73f, 0.72f,0.72f,0.72f, 0.74f,0.74f,0.74f, 0.77f,0.77f,0.77f, 0.75f,0.75f,0.75f, 0.66f,0.66f,0.66f, 0.59f,0.59f,0.59f, 0.51f,0.51f,0.51f, 0.55f,0.55f,0.55f, 0.60f,0.60f,0.60f, 0.67f,0.67f,0.67f,
		0.72f,0.72f,0.72f, 0.69f,0.69f,0.69f, 0.64f,0.64f,0.64f, 0.60f,0.60f,0.60f, 0.64f,0.64f,0.64f, 0.71f,0.71f,0.71f, 0.76f,0.76f,0.76f, 0.78f,0.78f,0.78f, 0.78f,0.78f,0.78f, 0.76f,0.76f,0.76f, 0.72f,0.72f,0.72f, 0.68f,0.68f,0.68f, 0.62f,0.62f,0.62f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.44f,0.44f,0.44f, 0.44f,0.44f,0.44f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.62f,0.62f,0.62f, 0.68f,0.68f,0.68f, 0.72f,0.72f,0.72f, 0.76f,0.76f,0.76f, 0.78f,0.78f,0.78f, 0.78f,0.78f,0.78f, 0.76f,0.76f,0.76f, 0.71f,0.71f,0.71f, 0.64f,0.64f,0.64f, 0.60f,0.60f,0.60f, 0.64f,0.64f,0.64f, 0.69f,0.69f,0.69f, 0.72f,0.72f,0.72f,
		0.75f,0.75f,0.75f, 0.73f,0.73f,0.73f, 0.70f,0.70f,0.70f, 0.66f,0.66f,0.66f, 0.66f,0.66f,0.66f, 0.67f,0.67f,0.67f, 0.71f,0.71f,0.71f, 0.74f,0.74f,0.74f, 0.76f,0.76f,0.76f, 0.77f,0.77f,0.77f, 0.70f,0.70f,0.70f, 0.65f,0.65f,0.65f, 0.62f,0.62f,0.62f, 0.55f,0.55f,0.55f, 0.44f,0.44f,0.44f, 0.39f,0.39f,0.39f, 0.39f,0.39f,0.39f, 0.44f,0.44f,0.44f, 0.55f,0.55f,0.55f, 0.62f,0.62f,0.62f, 0.65f,0.65f,0.65f, 0.70f,0.70f,0.70f, 0.77f,0.77f,0.77f, 0.76f,0.76f,0.76f, 0.74f,0.74f,0.74f, 0.71f,0.71f,0.71f, 0.67f,0.67f,0.67f, 0.66f,0.66f,0.66f, 0.66f,0.66f,0.66f, 0.70f,0.70f,0.70f, 0.73f,0.73f,0.73f, 0.75f,0.75f,0.75f,
		0.80f,0.80f,0.80f, 0.78f,0.78f,0.78f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.70f,0.70f,0.70f, 0.68f,0.68f,0.68f, 0.71f,0.71f,0.71f, 0.74f,0.74f,0.74f, 0.77f,0.77f,0.77f, 0.77f,0.77f,0.77f, 0.71f,0.71f,0.71f, 0.65f,0.65f,0.65f, 0.58f,0.58f,0.58f, 0.50f,0.50f,0.50f, 0.38f,0.38f,0.38f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.38f,0.38f,0.38f, 0.50f,0.50f,0.50f, 0.58f,0.58f,0.58f, 0.65f,0.65f,0.65f, 0.71f,0.71f,0.71f, 0.77f,0.77f,0.77f, 0.77f,0.77f,0.77f, 0.74f,0.74f,0.74f, 0.71f,0.71f,0.71f, 0.68f,0.68f,0.68f, 0.70f,0.70f,0.70f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.78f,0.78f,0.78f, 0.80f,0.80f,0.80f,
		0.81f,0.81f,0.81f, 0.75f,0.75f,0.75f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.70f,0.70f,0.70f, 0.69f,0.69f,0.69f, 0.69f,0.69f,0.69f, 0.67f,0.67f,0.67f, 0.62f,0.62f,0.62f, 0.54f,0.54f,0.54f, 0.42f,0.42f,0.42f, 0.30f,0.30f,0.30f, 0.26f,0.26f,0.26f, 0.26f,0.26f,0.26f, 0.30f,0.30f,0.30f, 0.42f,0.42f,0.42f, 0.54f,0.54f,0.54f, 0.62f,0.62f,0.62f, 0.67f,0.67f,0.67f, 0.69f,0.69f,0.69f, 0.69f,0.69f,0.69f, 0.70f,0.70f,0.70f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.75f,0.75f,0.75f, 0.81f,0.81f,0.81f,
		0.81f,0.81f,0.81f, 0.75f,0.75f,0.75f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.70f,0.70f,0.70f, 0.69f,0.69f,0.69f, 0.69f,0.69f,0.69f, 0.67f,0.67f,0.67f, 0.62f,0.62f,0.62f, 0.54f,0.54f,0.54f, 0.42f,0.42f,0.42f, 0.30f,0.30f,0.30f, 0.26f,0.26f,0.26f, 0.26f,0.26f,0.26f, 0.30f,0.30f,0.30f, 0.42f,0.42f,0.42f, 0.54f,0.54f,0.54f, 0.62f,0.62f,0.62f, 0.67f,0.67f,0.67f, 0.69f,0.69f,0.69f, 0.69f,0.69f,0.69f, 0.70f,0.70f,0.70f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.71f,0.71f,0.71f, 0.75f,0.75f,0.75f, 0.81f,0.81f,0.81f,
		0.80f,0.80f,0.80f, 0.78f,0.78f,0.78f, 0.73f,0.73f,0.73f, 0.71f,0.71f,0.71f, 0.70f,0.70f,0.70f, 0.68f,0.68f,0.68f, 0.71f,0.71f,0.71f, 0.74f,0.74f,0.74f, 0.77f,0.77f,0.77f, 0.77f,0.77f,0.77f, 0.71f,0.71f,0.71f, 0.65f,0.65f,0.65f, 0.58f,0.58f,0.58f, 0.50f,0.50f,0.50f, 0.38f,0.38f,0.38f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.38f,0.38f,0.38f, 0.50f,0.50f,0.50f, 0.58f,0.58f,0.58f, 0.65f,0.65f,0.65f, 0.71f,0.71f,0.71f, 0.77f,0.77f,0.77f, 0.77f,0.77f,0.77f, 0.74f,0.74f,0.74f, 0.71f,0.71f,0.71f, 0.68f,0.68f,0.68f, 0.70f,0.70f,0.70f, 0.71f,0.71f,0.71f, 0.73f,0.73f,0.73f, 0.78f,0.78f,0.78f, 0.80f,0.80f,0.80f,
		0.75f,0.75f,0.75f, 0.73f,0.73f,0.73f, 0.70f,0.70f,0.70f, 0.66f,0.66f,0.66f, 0.66f,0.66f,0.66f, 0.67f,0.67f,0.67f, 0.71f,0.71f,0.71f, 0.74f,0.74f,0.74f, 0.76f,0.76f,0.76f, 0.77f,0.77f,0.77f, 0.70f,0.70f,0.70f, 0.65f,0.65f,0.65f, 0.62f,0.62f,0.62f, 0.55f,0.55f,0.55f, 0.44f,0.44f,0.44f, 0.39f,0.39f,0.39f, 0.39f,0.39f,0.39f, 0.44f,0.44f,0.44f, 0.55f,0.55f,0.55f, 0.62f,0.62f,0.62f, 0.65f,0.65f,0.65f, 0.70f,0.70f,0.70f, 0.77f,0.77f,0.77f, 0.76f,0.76f,0.76f, 0.74f,0.74f,0.74f, 0.71f,0.71f,0.71f, 0.67f,0.67f,0.67f, 0.66f,0.66f,0.66f, 0.66f,0.66f,0.66f, 0.70f,0.70f,0.70f, 0.73f,0.73f,0.73f, 0.75f,0.75f,0.75f,
		0.72f,0.72f,0.72f, 0.69f,0.69f,0.69f, 0.64f,0.64f,0.64f, 0.60f,0.60f,0.60f, 0.64f,0.64f,0.64f, 0.71f,0.71f,0.71f, 0.76f,0.76f,0.76f, 0.78f,0.78f,0.78f, 0.78f,0.78f,0.78f, 0.76f,0.76f,0.76f, 0.72f,0.72f,0.72f, 0.68f,0.68f,0.68f, 0.62f,0.62f,0.62f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.44f,0.44f,0.44f, 0.44f,0.44f,0.44f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.62f,0.62f,0.62f, 0.68f,0.68f,0.68f, 0.72f,0.72f,0.72f, 0.76f,0.76f,0.76f, 0.78f,0.78f,0.78f, 0.78f,0.78f,0.78f, 0.76f,0.76f,0.76f, 0.71f,0.71f,0.71f, 0.64f,0.64f,0.64f, 0.60f,0.60f,0.60f, 0.64f,0.64f,0.64f, 0.69f,0.69f,0.69f, 0.72f,0.72f,0.72f,
		0.67f,0.67f,0.67f, 0.60f,0.60f,0.60f, 0.55f,0.55f,0.55f, 0.51f,0.51f,0.51f, 0.59f,0.59f,0.59f, 0.66f,0.66f,0.66f, 0.75f,0.75f,0.75f, 0.77f,0.77f,0.77f, 0.74f,0.74f,0.74f, 0.72f,0.72f,0.72f, 0.73f,0.73f,0.73f, 0.70f,0.70f,0.70f, 0.64f,0.64f,0.64f, 0.59f,0.59f,0.59f, 0.53f,0.53f,0.53f, 0.45f,0.45f,0.45f, 0.45f,0.45f,0.45f, 0.53f,0.53f,0.53f, 0.59f,0.59f,0.59f, 0.64f,0.64f,0.64f, 0.70f,0.70f,0.70f, 0.73f,0.73f,0.73f, 0.72f,0.72f,0.72f, 0.74f,0.74f,0.74f, 0.77f,0.77f,0.77f, 0.75f,0.75f,0.75f, 0.66f,0.66f,0.66f, 0.59f,0.59f,0.59f, 0.51f,0.51f,0.51f, 0.55f,0.55f,0.55f, 0.60f,0.60f,0.60f, 0.67f,0.67f,0.67f,
		0.60f,0.60f,0.60f, 0.57f,0.57f,0.57f, 0.52f,0.52f,0.52f, 0.49f,0.49f,0.49f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.62f,0.62f,0.62f, 0.66f,0.66f,0.66f, 0.70f,0.70f,0.70f, 0.68f,0.68f,0.68f, 0.65f,0.65f,0.65f, 0.60f,0.60f,0.60f, 0.56f,0.56f,0.56f, 0.46f,0.46f,0.46f, 0.46f,0.46f,0.46f, 0.56f,0.56f,0.56f, 0.60f,0.60f,0.60f, 0.65f,0.65f,0.65f, 0.68f,0.68f,0.68f, 0.70f,0.70f,0.70f, 0.66f,0.66f,0.66f, 0.62f,0.62f,0.62f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.49f,0.49f,0.49f, 0.52f,0.52f,0.52f, 0.57f,0.57f,0.57f, 0.60f,0.60f,0.60f,
		0.62f,0.62f,0.62f, 0.58f,0.58f,0.58f, 0.52f,0.52f,0.52f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.54f,0.54f,0.54f, 0.54f,0.54f,0.54f, 0.50f,0.50f,0.50f, 0.54f,0.54f,0.54f, 0.58f,0.58f,0.58f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.55f,0.55f,0.55f, 0.52f,0.52f,0.52f, 0.44f,0.44f,0.44f, 0.44f,0.44f,0.44f, 0.52f,0.52f,0.52f, 0.55f,0.55f,0.55f, 0.62f,0.62f,0.62f, 0.64f,0.64f,0.64f, 0.62f,0.62f,0.62f, 0.58f,0.58f,0.58f, 0.54f,0.54f,0.54f, 0.50f,0.50f,0.50f, 0.54f,0.54f,0.54f, 0.54f,0.54f,0.54f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.52f,0.52f,0.52f, 0.58f,0.58f,0.58f, 0.62f,0.62f,0.62f,
		0.63f,0.63f,0.63f, 0.60f,0.60f,0.60f, 0.58f,0.58f,0.58f, 0.56f,0.56f,0.56f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.49f,0.49f,0.49f, 0.48f,0.48f,0.48f, 0.52f,0.52f,0.52f, 0.51f,0.51f,0.51f, 0.53f,0.53f,0.53f, 0.57f,0.57f,0.57f, 0.54f,0.54f,0.54f, 0.45f,0.45f,0.45f, 0.41f,0.41f,0.41f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.41f,0.41f,0.41f, 0.45f,0.45f,0.45f, 0.54f,0.54f,0.54f, 0.57f,0.57f,0.57f, 0.53f,0.53f,0.53f, 0.51f,0.51f,0.51f, 0.52f,0.52f,0.52f, 0.48f,0.48f,0.48f, 0.49f,0.49f,0.49f, 0.51f,0.51f,0.51f, 0.51f,0.51f,0.51f, 0.56f,0.56f,0.56f, 0.58f,0.58f,0.58f, 0.60f,0.60f,0.60f, 0.63f,0.63f,0.63f,
		0.65f,0.65f,0.65f, 0.62f,0.62f,0.62f, 0.61f,0.61f,0.61f, 0.52f,0.52f,0.52f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.46f,0.46f,0.46f, 0.50f,0.50f,0.50f, 0.51f,0.51f,0.51f, 0.47f,0.47f,0.47f, 0.47f,0.47f,0.47f, 0.41f,0.41f,0.41f, 0.32f,0.32f,0.32f, 0.31f,0.31f,0.31f, 0.30f,0.30f,0.30f, 0.30f,0.30f,0.30f, 0.31f,0.31f,0.31f, 0.32f,0.32f,0.32f, 0.41f,0.41f,0.41f, 0.47f,0.47f,0.47f, 0.47f,0.47f,0.47f, 0.51f,0.51f,0.51f, 0.50f,0.50f,0.50f, 0.46f,0.46f,0.46f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.52f,0.52f,0.52f, 0.61f,0.61f,0.61f, 0.62f,0.62f,0.62f, 0.65f,0.65f,0.65f,
		0.64f,0.64f,0.64f, 0.59f,0.59f,0.59f, 0.62f,0.62f,0.62f, 0.51f,0.51f,0.51f, 0.40f,0.40f,0.40f, 0.34f,0.34f,0.34f, 0.38f,0.38f,0.38f, 0.44f,0.44f,0.44f, 0.45f,0.45f,0.45f, 0.42f,0.42f,0.42f, 0.37f,0.37f,0.37f, 0.38f,0.38f,0.38f, 0.32f,0.32f,0.32f, 0.24f,0.24f,0.24f, 0.20f,0.20f,0.20f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.20f,0.20f,0.20f, 0.24f,0.24f,0.24f, 0.32f,0.32f,0.32f, 0.38f,0.38f,0.38f, 0.37f,0.37f,0.37f, 0.42f,0.42f,0.42f, 0.45f,0.45f,0.45f, 0.44f,0.44f,0.44f, 0.38f,0.38f,0.38f, 0.34f,0.34f,0.34f, 0.40f,0.40f,0.40f, 0.51f,0.51f,0.51f, 0.62f,0.62f,0.62f, 0.59f,0.59f,0.59f, 0.64f,0.64f,0.64f,
		0.61f,0.61f,0.61f, 0.60f,0.60f,0.60f, 0.62f,0.62f,0.62f, 0.50f,0.50f,0.50f, 0.37f,0.37f,0.37f, 0.30f,0.30f,0.30f, 0.31f,0.31f,0.31f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.33f,0.33f,0.33f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.27f,0.27f,0.27f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.17f,0.17f,0.17f, 0.17f,0.17f,0.17f, 0.19f,0.19f,0.19f, 0.19f,0.19f,0.19f, 0.27f,0.27f,0.27f, 0.32f,0.32f,0.32f, 0.32f,0.32f,0.32f, 0.33f,0.33f,0.33f, 0.35f,0.35f,0.35f, 0.35f,0.35f,0.35f, 0.31f,0.31f,0.31f, 0.30f,0.30f,0.30f, 0.37f,0.37f,0.37f, 0.50f,0.50f,0.50f, 0.62f,0.62f,0.62f, 0.60f,0.60f,0.60f, 0.61f,0.61f,0.61f,
		0.61f,0.61f,0.61f, 0.57f,0.57f,0.57f, 0.55f,0.55f,0.55f, 0.45f,0.45f,0.45f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.27f,0.27f,0.27f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.16f,0.16f,0.16f, 0.17f,0.17f,0.17f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.17f,0.17f,0.17f, 0.16f,0.16f,0.16f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.27f,0.27f,0.27f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.45f,0.45f,0.45f, 0.55f,0.55f,0.55f, 0.57f,0.57f,0.57f, 0.61f,0.61f,0.61f,
		0.57f,0.57f,0.57f, 0.53f,0.53f,0.53f, 0.44f,0.44f,0.44f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.23f,0.23f,0.23f, 0.22f,0.22f,0.22f, 0.24f,0.24f,0.24f, 0.25f,0.25f,0.25f, 0.29f,0.29f,0.29f, 0.33f,0.33f,0.33f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.21f,0.21f,0.21f, 0.21f,0.21f,0.21f, 0.20f,0.20f,0.20f, 0.20f,0.20f,0.20f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.33f,0.33f,0.33f, 0.29f,0.29f,0.29f, 0.25f,0.25f,0.25f, 0.24f,0.24f,0.24f, 0.22f,0.22f,0.22f, 0.23f,0.23f,0.23f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.44f,0.44f,0.44f, 0.53f,0.53f,0.53f, 0.57f,0.57f,0.57f,
		0.52f,0.52f,0.52f, 0.45f,0.45f,0.45f, 0.37f,0.37f,0.37f, 0.31f,0.31f,0.31f, 0.25f,0.25f,0.25f, 0.19f,0.19f,0.19f, 0.17f,0.17f,0.17f, 0.21f,0.21f,0.21f, 0.31f,0.31f,0.31f, 0.33f,0.33f,0.33f, 0.33f,0.33f,0.33f, 0.26f,0.26f,0.26f, 0.24f,0.24f,0.24f, 0.24f,0.24f,0.24f, 0.23f,0.23f,0.23f, 0.18f,0.18f,0.18f, 0.18f,0.18f,0.18f, 0.23f,0.23f,0.23f, 0.24f,0.24f,0.24f, 0.24f,0.24f,0.24f, 0.26f,0.26f,0.26f, 0.33f,0.33f,0.33f, 0.33f,0.33f,0.33f, 0.31f,0.31f,0.31f, 0.21f,0.21f,0.21f, 0.17f,0.17f,0.17f, 0.19f,0.19f,0.19f, 0.25f,0.25f,0.25f, 0.31f,0.31f,0.31f, 0.37f,0.37f,0.37f, 0.45f,0.45f,0.45f, 0.52f,0.52f,0.52f,
		0.52f,0.52f,0.52f, 0.49f,0.49f,0.49f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.21f,0.21f,0.21f, 0.15f,0.15f,0.15f, 0.13f,0.13f,0.13f, 0.17f,0.17f,0.17f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.37f,0.37f,0.37f, 0.28f,0.28f,0.28f, 0.26f,0.26f,0.26f, 0.28f,0.28f,0.28f, 0.25f,0.25f,0.25f, 0.22f,0.22f,0.22f, 0.22f,0.22f,0.22f, 0.25f,0.25f,0.25f, 0.28f,0.28f,0.28f, 0.26f,0.26f,0.26f, 0.28f,0.28f,0.28f, 0.37f,0.37f,0.37f, 0.38f,0.38f,0.38f, 0.29f,0.29f,0.29f, 0.17f,0.17f,0.17f, 0.13f,0.13f,0.13f, 0.15f,0.15f,0.15f, 0.21f,0.21f,0.21f, 0.29f,0.29f,0.29f, 0.38f,0.38f,0.38f, 0.49f,0.49f,0.49f, 0.52f,0.52f,0.52f,
		0.53f,0.53f,0.53f, 0.53f,0.53f,0.53f, 0.44f,0.44f,0.44f, 0.31f,0.31f,0.31f, 0.21f,0.21f,0.21f, 0.17f,0.17f,0.17f, 0.18f,0.18f,0.18f, 0.22f,0.22f,0.22f, 0.32f,0.32f,0.32f, 0.41f,0.41f,0.41f, 0.41f,0.41f,0.41f, 0.36f,0.36f,0.36f, 0.33f,0.33f,0.33f, 0.35f,0.35f,0.35f, 0.31f,0.31f,0.31f, 0.26f,0.26f,0.26f, 0.26f,0.26f,0.26f, 0.31f,0.31f,0.31f, 0.35f,0.35f,0.35f, 0.33f,0.33f,0.33f, 0.36f,0.36f,0.36f, 0.41f,0.41f,0.41f, 0.41f,0.41f,0.41f, 0.32f,0.32f,0.32f, 0.22f,0.22f,0.22f, 0.18f,0.18f,0.18f, 0.17f,0.17f,0.17f, 0.21f,0.21f,0.21f, 0.31f,0.31f,0.31f, 0.44f,0.44f,0.44f, 0.53f,0.53f,0.53f, 0.53f,0.53f,0.53f,
	};

	return texture_data;
}
