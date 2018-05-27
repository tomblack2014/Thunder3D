#include "app.h"

LPCTSTR App::title = TEXT("Assignment");

typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;

LRESULT App::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	App* app;
	if(msg == WM_CREATE){
		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		app = reinterpret_cast<App*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(app));
		return 1;
	} else{
		app = reinterpret_cast<App*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return app->WndProc(hwnd, msg, wParam, lParam);
	}
}

LRESULT App::WndProc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam){
	switch(msg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if(control){
			control->OnKeyDown(wParam);
		}
		return 0;
	case WM_KEYUP:
		if(control){
			control->OnKeyUp(wParam);
		}
		return 0;
	case WM_LBUTTONDOWN:
		if(control){
			control->OnMouseDown(true, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_LBUTTONUP:
		if(control){
			control->OnMouseUp(true, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_RBUTTONDOWN:
		if(control){
			control->OnMouseDown(false, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_RBUTTONUP:
		if(control){
			control->OnMouseUp(false, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_SIZE:
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

bool App::CreateGLWindow(int width, int height){
	GLuint PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;
	static	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)StaticWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;

	if(!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("不能注册窗口类."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// 调整窗口尺寸

	// Create The Window
	hWnd = CreateWindowEx(dwExStyle,
		title,
		title,
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL,
		NULL,
		hInstance,
		this);
	if(!hWnd)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("窗口创建错误."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;								// 返回失败
	}

	hDC = GetDC(hWnd);
	if(!hDC)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("不能创建GL设备上下文."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!PixelFormat)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("不能匹配到合适的像素格式."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("不能设置像素格式."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	hRC = wglCreateContext(hDC);
	if(!hRC)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("不能创建GL渲染上下文."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!wglMakeCurrent(hDC, hRC))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("不能激活GL渲染上下文."), TEXT("错误"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(wglSwapIntervalEXT == NULL){
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
	}
	wglSwapIntervalEXT(1);

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(width, height);

	return true;
}

void App::KillGLWindow()
{
	if(hRC)
	{
		if(!wglMakeCurrent(NULL, NULL))
		{
			LOGE(DC和RC释放失败);
		}

		if(!wglDeleteContext(hRC))
		{
			LOGE(渲染上下文释放失败);
		}
		hRC = NULL;
	}

	if(hDC && !ReleaseDC(hWnd, hDC))
	{
		LOGE(设备上下文释放失败);
	}
	hDC = NULL;

	if(hWnd && !DestroyWindow(hWnd))
	{
		LOGE(不能释放hWnd);
	}
	hWnd = NULL;

	if(!UnregisterClass(title, hInstance))
	{
		LOGE(不能取消窗口类);
	}
}

void App::ReSizeGLScene(GLsizei width, GLsizei height)
{
	if(height == 0){
		height = 1;
	}

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//计算窗口比率
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 900.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool App::Init(){
	lastTick = GetTickCount();

	//初始化GL场景
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//初始化材质，取消材质生成
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);

	//初始化光源
	glEnable(GL_LIGHTING);

	//视点前方光源
	//用自带的，不设置了

	//初始化摄像头和控制器
	UseStandardView();

	//初始化场景对象
	sun = Star::create(Vec4f(0.f, 0.f, 0.f, 1.f), 4.f, 2.f);
	earth = Planet::create(10.f, Vec4f(-0.15f, 1.f, 0.f), 0.f, 0.5f, sun, 1.0f);
	moon = Planet::create(3.f, Vec4f(0.1f, 1.f, 0.f), 0.f, 1.f, earth, 0.8f);
	craft = Craft::create(Vec4f(3.f, 3.f, 3.f, 1.f), Vec4f(0.5f, 0.f, 0.f), Vec4f(0.f, 0.5f, 0.f));
	surbox = SurroundBox::create(NULL, camera);
	partisys = ParticleSystem::create(craft, camera);
	cubemap = CubeMap::create(500.0f);
	lightcone = LightCone::create(craft);

	sun->Initialize("config/sphere.bin", "config/red.bmp");
	earth->Initialize("config/sphere.bin", "config/blue.bmp");
	moon->Initialize("config/sphere.bin", "config/gray.bmp");
	craft->Initialize("config/craft.bin");
	partisys->Initialize("config/Particle.png");
	cubemap->Initialize("config/universe.bmp");
	lightcone->Initialize("config/cone.bin", "config/light.png");

	return true;
}

void App::Run(){
	DWORD now;
	float interval;


	MSG msg;
	CreateGLWindow(640, 480);
	Init();
	while(true){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			} else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				//这个消息会阻塞整个窗口，因此需要抛弃这一帧重算时间
				if(msg.message == WM_NCLBUTTONDOWN){
					lastTick = GetTickCount();
					continue;;
				}
			}
		}
		//计算时间
		now = GetTickCount();
		interval = (now - lastTick) * 0.001f;
		lastTick = now;

		//控制器处理
		//因为要处理点选，所以不能清屏
		camera->LoadMatrix();
		control->Frame(interval);

		// 清除屏幕和深度缓存
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		//前进一帧
		sun->Frame(interval);
		earth->Frame(interval);
		moon->Frame(interval);
		craft->Frame(interval);
		surbox->Frame(interval);
		partisys->Frame(interval);
		cubemap->Frame(interval);
		lightcone->Frame(interval);

		//摄像机需要放在最后
		//不然会出现跟踪的抖动
		camera->Frame(interval);

		camera->LoadMatrix();
		cubemap->Render();
		sun->Render();
		earth->Render();
		moon->Render();
		craft->Render();
		surbox->Render();
		partisys->Render();
		lightcone->Render();

		SwapBuffers(hDC);
	}
	KillGLWindow();
}

App::App() :
sun(NULL),
earth(NULL),
moon(NULL),
craft(NULL),
surbox(NULL),
partisys(NULL),
cubemap(NULL),
lightcone(NULL),
camera(NULL),
control(NULL)
{
}

App::~App()
{
	//释放场景对象
	SafeRelease(&sun);
	SafeRelease(&earth);
	SafeRelease(&moon);
	SafeRelease(&craft);
	SafeRelease(&surbox);
	SafeRelease(&partisys);
	SafeRelease(&cubemap);
	SafeRelease(&lightcone);

	//释放摄像头和控制器
	SafeRelease(&camera);
	if(control){
		delete control;
	}
}

void App::OnSelect(_In_ const Vec4f& point)
{
	IObject* obj = NULL;

	//无效的点选
	if(point.w <= 0.001){
		surbox->UpdateObject(NULL);
		return;
	}

	//判断碰撞对象
	if(sun->Collide(point)){
		sun->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else if(earth->Collide(point)){
		earth->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else if(moon->Collide(point)){
		moon->QueryInterface(IObject::IID_IObject, (void**)&obj);
	}

	//更新目标对象
	surbox->UpdateObject(obj);

	//释放临时变量
	SafeRelease(&obj);
}

void App::UseStandardView(){
	//清除旧的摄像头
	SafeRelease(&camera);

	//初始化摄像头
	FreeCamera* fcamera = FreeCamera::create(Vec4f(0.f, 0.f, 35.f, 1.f), Vec4f(0.f, 0.f, 0.f, 1.f), Vec4f(0.f, 1.f, 0.f));
	fcamera->QueryInterface(ICamera::IID_ICamera, (void**)&camera);

	//初始化控制器
	Controller* newcontrol = new CameraController(fcamera, this);

	//修改相关对象的摄像头
	if(surbox){
		surbox->UpdateCamera(camera);
	}
	if(partisys){
		partisys->UpdateCamera(camera);
	}

	//释放临时变量
	SafeRelease(&fcamera);

	//交接控制器
	if(control){
		newcontrol->CopyKeyState(control);
		delete control;
	}
	control = newcontrol;
}

void App::UseTraceView(_In_ IObject* obj){
	//清理旧的摄像头
	SafeRelease(&camera);

	//初始化摄像头
	TraceCamera* tcamera = TraceCamera::create(obj);
	tcamera->QueryInterface(ICamera::IID_ICamera, (void**)&camera);

	//初始化控制器
	Controller* newcontrol = new TraceController(this);

	//修改相关对象的摄像头
	if(surbox){
		surbox->UpdateCamera(camera);
	}
	if(partisys){
		partisys->UpdateCamera(camera);
	}

	//释放临时变量
	SafeRelease(&tcamera);

	//交接控制器
	if(control){
		newcontrol->CopyKeyState(control);
		delete control;
	}
	control = newcontrol;
}

void App::OnTrace(_In_ const Vec4f& point){
	IObject* obj = NULL;

	//无效的点选
	if(point.w <= 0.001){
		return;
	}

	//判断碰撞对象
	if(sun->Collide(point)){
		sun->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else if(earth->Collide(point)){
		earth->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else if(moon->Collide(point)){
		moon->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else if(craft->Collide(point)){
		craft->QueryInterface(IObject::IID_IObject, (void**)&obj);
	} else{
		return;
	}

	//更新摄像头控制器
	UseTraceView(obj);

	//释放临时变量
	SafeRelease(&obj);
}

void App::UseCraftView(){
	//清理旧的摄像头
	SafeRelease(&camera);

	//初始化摄像头
	CraftCamera* ccamera = CraftCamera::create(craft);
	ccamera->QueryInterface(ICamera::IID_ICamera, (void**)&camera);

	//初始化控制器
	Controller* newcontrol = new CraftController(this, craft);

	//修改相关对象的摄像头
	if(surbox){
		surbox->UpdateCamera(camera);
	}
	if(partisys){
		partisys->UpdateCamera(camera);
	}

	//释放临时变量
	SafeRelease(&ccamera);

	//交接控制器
	if(control){
		newcontrol->CopyKeyState(control);
		delete control;
	}
	control = newcontrol;
}
