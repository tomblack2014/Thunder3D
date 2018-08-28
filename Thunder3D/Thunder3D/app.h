#ifndef _APP_H
#define _APP_H

#include "common.h"
#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <vector>

#include "basescene.h"
#include "Comment.h"


/**
* Ӧ�ó���ͳ�����
*/
class App
{
private:
	static LPCTSTR title;

	HWND hWnd;
	HINSTANCE hInstance;
	HDC hDC;
	HGLRC hRC;
	DWORD lastTick;

	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT WndProc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	bool CreateGLWindow(int width, int height);

	void KillGLWindow();

	void ReSizeGLScene(GLsizei width, GLsizei height);

	bool Init();
public:
	void Run();

	App();

	~App();

	HWND GetHwnd();

	HDC GetHDC();

private:

	std::vector<BaseScene*> m_sceneList;

	BaseScene* m_lastScene;
	BaseScene* m_curScene;

};

inline HWND App::GetHwnd(){
	return hWnd;
}

inline HDC App::GetHDC() {
	return hDC;
}

#endif