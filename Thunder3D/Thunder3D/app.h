#ifndef _APP_H
#define _APP_H

#include "common.h"
#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include "planet.h"
#include "star.h"
#include "craft.h"
#include "camera.h"
#include "controller.h"
#include "SurroundBox.h"
#include "ParticleSystem.h"
#include "cubemap.h"
#include "LightCone.h"

/**
* 应用程序和场景类
* 因为只有一个场景所以简化成一个类了
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

	//选择对象事件
	//point : 选择点
	void OnSelect(_In_ const Vec4f& point);

	//追踪对象事件
	//point : 选择点
	void OnTrace(_In_ const Vec4f& point);

	//创建标准控制器和摄像机
	//使用自由视角
	//这些函数的设计并不是最优设计
	void UseStandardView();

	//创建跟踪控制器和摄像机
	void UseTraceView(_In_ IObject* obj);

	//创建飞船控制器和摄像机
	void UseCraftView();

private:
	Star* sun;
	Planet* earth;
	Planet* moon;
	Craft* craft;
	SurroundBox* surbox;
	ParticleSystem* partisys;
	CubeMap* cubemap;
	LightCone* lightcone;

	ICamera* camera;
	Controller* control;
};

inline HWND App::GetHwnd(){
	return hWnd;
}

#endif