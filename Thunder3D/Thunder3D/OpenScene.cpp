#include "OpenScene.h"
#include "common.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include <windowsx.h>

OpenScene::OpenScene(Comment* comment):
	m_craft(NULL),
	m_comment(comment)
{
	m_ctrl = NULL;
	m_comment->AddRef();
}

OpenScene::~OpenScene()
{
	SafeRelease(&m_craft);

	//释放摄像头和控制器
	SafeRelease(&m_camera);

	SafeRelease(&m_comment);
}

bool OpenScene::Init(App* app)
{
	if (!m_ctrl)
		m_ctrl = new OpenSceneCtrl(app, this);

	//初始化摄像头
	FreeCamera* fcamera = FreeCamera::create(Vec4f(0.f, 0.f, 35.f, 1.f), Vec4f(0.f, 0.f, 0.f, 1.f), Vec4f(0.f, 1.f, 0.f));
	fcamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);
	
	Reset();

	return true;
}



T3D::SCENE OpenScene::Update(float interval)
{
	if (m_status == 2) {
		m_frames++;
		float tmp = m_frames % 200 * 1.f;
		tmp = abs(tmp - 100) / 100.f;
		std::vector<std::string> contexts;
		std::vector<Vec4f> colors;
		std::vector<Vec4f> poss;
		contexts.push_back("press any key to start");
		colors.push_back(Vec4f(1.f, 0.f, 0.f, tmp));
		poss.push_back(Vec4f(-0.3f, -0.3f, 0.f));
		m_comment->Set(contexts, colors, poss);

		m_comment->Render();
	}


	m_camera->LoadMatrix();

	m_craft->Frame(interval);
	m_ctrl->Frame(interval);
	m_camera->Frame(interval);

	m_camera->LoadMatrix();
	m_craft->Render();

	
	auto pos = m_craft->GetPos();
	if (m_status == 0 && pos.x >= 20.f) {
		SafeRelease(&m_craft);
		m_craft = Craft::create(Vec4f(5.f, 4.5f, 31.f, 1.f), Vec4f(-7.f, -5.6f, 0.f), Vec4f(-0.5f, 0.5f, 0.4f));
		m_craft->Initialize("config/craft.bin");
		m_status = 1;
	}

	if (m_status == 1 && pos.x <= 0) {
		m_craft->Accelerate(-m_craft->GetSpeed().length());
		m_status = 2;
	}
	
	if (m_endFlag) return T3D::SCENE::CHAPTER1;
	else return T3D::SCENE::OPEN;
};

void OpenScene::Reset()
{
	if (m_ctrl)
		m_ctrl->ResetKeyState();

	SafeRelease(&m_craft);
	m_craft = Craft::create(Vec4f(-5.f, -4.f, 0.f, 1.f), Vec4f(25.f, 25.f, 0.f), Vec4f(-0.5f, 0.5f, 0.f));
	m_craft->Initialize("config/craft.bin");

	m_endFlag = false;
	m_status = 0;
	m_frames = 0;

	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	m_comment->Set(contexts, colors, poss);
}

void OpenScene::end()
{
	m_endFlag = true;
}

bool OpenScene::AnimateFinished()
{
	if (m_status < 2)
		return false;
	return true;
}

////////////////////////////////////////////OpenSceneCtrl///////////////////////////////////////////////////////////////

OpenSceneCtrl::OpenSceneCtrl(_In_ App* app, _In_ OpenScene* scene) :Controller(app)
{
	m_scene = scene;
}

void OpenSceneCtrl::Frame(_In_ float interval)
{
	if (!m_scene)
		return;

	//开场动画放完后按任意键跳转场景
	if (m_scene->AnimateFinished()) {
		for (int i = 0; i < 256; i++) {
			if (keys[i] == KEY_PRESS || keys[i] == KEY_DOWN) {
				if (m_scene)
					m_scene->end();
			}
		}
	}
}