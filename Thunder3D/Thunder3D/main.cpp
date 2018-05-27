#include "app.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

int WINAPI WinMain(
	_In_        HINSTANCE  /*hInstance*/,
	_In_opt_    HINSTANCE  /*hPrevInstance*/,
	_In_        LPSTR  /*lpCmdLine*/,
	_In_        int  /*nCmdShow*/
	)
{
	CoInitialize(NULL);

	App* app = new App();
	app->Run();
	delete app;

	CoUninitialize();

	ChkMem;
	ChkLeak;
	return 0;
}