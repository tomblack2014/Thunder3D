#include "TestScene.h"

TestScene::TestScene():
	sun(NULL),
	earth(NULL),
	moon(NULL),
	craft(NULL),
	surbox(NULL),
	partisys(NULL),
	cubemap(NULL),
	lightcone(NULL)
{
}

TestScene::~TestScene()
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
	SafeRelease(&m_camera);
};

bool TestScene::Init(App* app)
{
	m_app = app;

	//初始化摄像头和控制器
	UseStandardView();

	//初始化场景对象
	sun = Star::create(Vec4f(0.f, 0.f, 0.f, 1.f), 4.f, 2.f);
	earth = Planet::create(10.f, Vec4f(-0.15f, 1.f, 0.f), 0.f, 0.5f, sun, 1.0f);
	moon = Planet::create(3.f, Vec4f(0.1f, 1.f, 0.f), 0.f, 1.f, earth, 0.8f);
	craft = Craft::create(Vec4f(3.f, 3.f, 3.f, 1.f), Vec4f(0.5f, 0.f, 0.f), Vec4f(0.f, 0.5f, 0.f));
	partisys = ParticleSystem::create(craft, m_camera);
	lightcone = LightCone::create(craft);
	cubemap = CubeMap::create(500.0f);
	surbox = SurroundBox::create(NULL, m_camera);

	sun->Initialize("config/sphere.bin", "config/red.bmp");
	earth->Initialize("config/sphere.bin", "config/blue.bmp");
	moon->Initialize("config/sphere.bin", "config/gray.bmp");
	craft->Initialize("config/craft.bin");
	partisys->Initialize("config/Particle.png");
	lightcone->Initialize("config/cone.bin", "config/light.png");
	cubemap->Initialize("config/universeTexture.bmp");

	return true;
};

T3D::SCENE TestScene::Update(float interval)
{
	//控制器处理
	//因为要处理点选，所以不能清屏
	m_camera->LoadMatrix();

	//前进一帧
	sun->Frame(interval);
	earth->Frame(interval);
	moon->Frame(interval);
	craft->Frame(interval);
	surbox->Frame(interval);
	partisys->Frame(interval);
	cubemap->Frame(interval);
	lightcone->Frame(interval);

	m_ctrl->Frame(interval);

	//摄像机需要放在最后
	//不然会出现跟踪的抖动
	m_camera->Frame(interval);

	m_camera->LoadMatrix();
	cubemap->Render();
	sun->Render();
	earth->Render();
	moon->Render();
	craft->Render();
	surbox->Render();
	partisys->Render();
	lightcone->Render();

	return T3D::SCENE::TEST;
};

void TestScene::Reset()
{}

//创建标准控制器和摄像机
//使用自由视角
//这些函数的设计并不是最优设计
void TestScene::UseStandardView()
{
	//清除旧的摄像头
	SafeRelease(&m_camera);

	//初始化摄像头
	FreeCamera* fcamera = FreeCamera::create(Vec4f(0.f, 0.f, 35.f, 1.f), Vec4f(0.f, 0.f, 0.f, 1.f), Vec4f(0.f, 1.f, 0.f));
	fcamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	//初始化控制器
	Controller* newcontrol = new CameraController(fcamera, m_app,this);

	//修改相关对象的摄像头
	if (surbox) {
		surbox->UpdateCamera(m_camera);
	}
	if (partisys) {
		partisys->UpdateCamera(m_camera);
	}

	//释放临时变量
	SafeRelease(&fcamera);

	//交接控制器
	if (m_ctrl) {
		newcontrol->CopyKeyState(m_ctrl);
		delete m_ctrl;
	}
	m_ctrl = newcontrol;
};

//创建跟踪控制器和摄像机
void TestScene::UseTraceView(_In_ IObject* obj)
{
	//清理旧的摄像头
	SafeRelease(&m_camera);

	//初始化摄像头
	TraceCamera* tcamera = TraceCamera::create(obj);
	tcamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	//初始化控制器
	Controller* newcontrol = new TraceController(m_app, this);

	//修改相关对象的摄像头
	if (surbox) {
		surbox->UpdateCamera(m_camera);
	}
	if (partisys) {
		partisys->UpdateCamera(m_camera);
	}

	//释放临时变量
	SafeRelease(&tcamera);

	//交接控制器
	if (m_ctrl) {
		newcontrol->CopyKeyState(m_ctrl);
		delete m_ctrl;
	}
	m_ctrl = newcontrol;
};

//创建飞船控制器和摄像机
void TestScene::UseCraftView()
{
	//清理旧的摄像头
	SafeRelease(&m_camera);

	//初始化摄像头
	CraftCamera* ccamera = CraftCamera::create(craft);
	ccamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	//初始化控制器
	Controller* newcontrol = new CraftController(m_app, craft);

	//修改相关对象的摄像头
	if (surbox) {
		surbox->UpdateCamera(m_camera);
	}
	if (partisys) {
		partisys->UpdateCamera(m_camera);
	}

	//释放临时变量
	SafeRelease(&ccamera);

	//交接控制器
	if (m_ctrl) {
		newcontrol->CopyKeyState(m_ctrl);
		delete m_ctrl;
	}
	m_ctrl = newcontrol;
};

void TestScene::OnSelect(_In_ const Vec4f& point)
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

void TestScene::OnTrace(_In_ const Vec4f& point){
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