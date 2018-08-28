#include "app.h"

#include "OpenScene.h"
#include "TestScene.h"
#include "Chapter1Scene.h"
#include "Chapter2Scene.h"
#include "Chapter3Scene.h"
#include "VictoryScene.h"
#include "DefeatScene.h"

LPCTSTR App::title = TEXT("Thunder3D");

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
		if(m_curScene){
			m_curScene->OnKeyDown(wParam);
		}
		return 0;
	case WM_KEYUP:
		if(m_curScene){
			m_curScene->OnKeyUp(wParam);
		}
		return 0;
	case WM_LBUTTONDOWN:
		if(m_curScene){
			m_curScene->OnMouseDown(true, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_LBUTTONUP:
		if(m_curScene){
			m_curScene->OnMouseUp(true, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_RBUTTONDOWN:
		if(m_curScene){
			m_curScene->OnMouseDown(false, LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_RBUTTONUP:
		if(m_curScene){
			m_curScene->OnMouseUp(false, LOWORD(lParam), HIWORD(lParam));
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
		MessageBox(NULL, TEXT("����ע�ᴰ����."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڳߴ�

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
		MessageBox(NULL, TEXT("���ڴ�������."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;								// ����ʧ��
	}

	hDC = GetDC(hWnd);
	if(!hDC)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("���ܴ���GL�豸������."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!PixelFormat)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("����ƥ�䵽���ʵ����ظ�ʽ."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("�����������ظ�ʽ."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	hRC = wglCreateContext(hDC);
	if(!hRC)
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("���ܴ���GL��Ⱦ������."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!wglMakeCurrent(hDC, hRC))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("���ܼ���GL��Ⱦ������."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
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
			LOGE(DC��RC�ͷ�ʧ��);
		}

		if(!wglDeleteContext(hRC))
		{
			LOGE(��Ⱦ�������ͷ�ʧ��);
		}
		hRC = NULL;
	}

	if(hDC && !ReleaseDC(hWnd, hDC))
	{
		LOGE(�豸�������ͷ�ʧ��);
	}
	hDC = NULL;

	if(hWnd && !DestroyWindow(hWnd))
	{
		LOGE(�����ͷ�hWnd);
	}
	hWnd = NULL;

	if(!UnregisterClass(title, hInstance))
	{
		LOGE(����ȡ��������);
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

	//���㴰�ڱ���
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 900.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool App::Init(){
	lastTick = GetTickCount();
	m_lastScene = NULL;
	m_curScene = NULL;

	////��ʼ��GL����
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//��ʼ�����ʣ�ȡ����������
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);

	//��ʼ����Դ
	glEnable(GL_LIGHTING);

	//�ӵ�ǰ����Դ
	//���Դ��ģ���������

	//��ʼ����������
	auto m_comment = Comment::create(this);
	if (m_comment)
		m_comment->Initialize();

	//��ʼ������
	m_sceneList.push_back(new OpenScene(m_comment));
	m_sceneList.push_back(new TestScene());
	m_sceneList.push_back(new Chapter1Scene(m_comment));
	m_sceneList.push_back(new Chapter2Scene(m_comment));
	m_sceneList.push_back(new Chapter3Scene(m_comment));
	m_sceneList.push_back(new VictoryScene(m_comment));
	m_sceneList.push_back(new DefeatScene(m_comment));

	m_curScene = m_sceneList[0];

	SafeRelease(&m_comment);

	for (int i = 0; i < m_sceneList.size(); i++) {
		if (!m_sceneList[i]->Init(this))
			return false;
	}

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

				//�����Ϣ�������������ڣ������Ҫ������һ֡����ʱ��
				if(msg.message == WM_NCLBUTTONDOWN){
					lastTick = GetTickCount();
					continue;;
				}
			}
		}
		//����ʱ��
		now = GetTickCount();
		interval = (now - lastTick) * 0.001f;
		lastTick = now;

		

		// �����Ļ����Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		if (m_curScene != m_lastScene) {
			if (m_lastScene) {
				m_lastScene->Reset();
			}
		}
		auto ret = m_curScene->Update(interval);
		if (ret > m_sceneList.size() || ret < 0) {
			//bug���ص�����
			m_lastScene = NULL;
			m_curScene = m_sceneList[0];
			continue;
		}
		m_lastScene = m_curScene;
		m_curScene = m_sceneList[ret];

		SwapBuffers(hDC);
	}
	KillGLWindow();
}

App::App()
{
}

App::~App()
{
	for (int i = 0; i < m_sceneList.size(); i++) {
		if (m_sceneList[i])
			delete m_sceneList[i];
	}
}