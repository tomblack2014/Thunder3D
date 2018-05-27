#include "cubemap.h"

#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#ifndef _MSC_VER
#define GL_BGR_EXT 0x80E0
#define GL_BGRA_EXT 0x80E1
#endif
#include "model.h"

CubeMap::CubeMap(float size) : 
_size(size)
{

}

CubeMap::~CubeMap()
{

}

void CubeMap::Render()
{
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBegin(GL_QUADS);
	glColor4f(1.f, 1.f, 1.f, 1.f);

	/*1*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(_size, _size, _size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(-_size, _size, _size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(-_size, _size, -_size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(_size, _size, -_size);

	/*2*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(_size, _size, _size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(_size, _size, -_size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(_size, -_size, -_size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(_size, -_size, _size);

	/*3*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(_size, _size, _size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(_size, -_size, _size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(-_size, -_size, _size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(-_size, _size, _size);

	/*4*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-_size, -_size, -_size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(-_size, _size, -_size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(_size, _size, -_size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(_size, -_size, -_size);

	/*5*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-_size, -_size, -_size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(-_size, -_size, _size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(-_size, _size, _size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(-_size, _size, -_size);

	/*6*/
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-_size, -_size, -_size);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(_size, -_size, -_size);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(_size, -_size, _size);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(-_size, -_size, _size);

	glEnd();
	glEnable(GL_LIGHTING);
}

bool CubeMap::Initialize(_In_ const char*  bmpFile)
{
	HANDLE hFile;
	DWORD dwBytes;
	BITMAPFILEHEADER fhead;
	BITMAPINFOHEADER ihead;
	GLubyte *buffer;

	hFile = CreateFileA(bmpFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	ReadFile(hFile, &fhead, sizeof(fhead), &dwBytes, NULL);
	if (dwBytes < sizeof(fhead) || fhead.bfType != 0x4D42)  //ASCII字符'BM'开头表示位图
	{
		CloseHandle(hFile);
		return false;
	}

	ReadFile(hFile, &ihead, sizeof(ihead), &dwBytes, NULL);
	if (dwBytes < sizeof(ihead) || ihead.biSize != sizeof(ihead) || (ihead.biBitCount != 24 && ihead.biBitCount != 32)){
		//文件损坏或者不是支持的Windows扩展位图
		CloseHandle(hFile);
		return false;
	}

	SetFilePointer(hFile, fhead.bfOffBits, NULL, FILE_BEGIN);
	buffer = (GLubyte*)VirtualAlloc(NULL, ihead.biSizeImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (buffer == NULL){
		CloseHandle(hFile);
		return false;
	}
	ReadFile(hFile, buffer, ihead.biSizeImage, &dwBytes, NULL);
	if (dwBytes < ihead.biSizeImage){
		VirtualFree(buffer, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return false;
	}

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	if (ihead.biBitCount == 24)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ihead.biWidth, ihead.biHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);
	}
	else{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ihead.biWidth, ihead.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线形滤波
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // 线形滤波

	VirtualFree(buffer, 0, MEM_RELEASE);
	CloseHandle(hFile);
	return true;
}