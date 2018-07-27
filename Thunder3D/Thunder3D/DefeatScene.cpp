#include "DefeatScene.h"
#include "common.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include <windowsx.h>


DefeatScene::DefeatScene(Comment* comment) :
	m_comment(comment)
{
	m_ctrl = NULL;
	m_comment->AddRef();
}

DefeatScene::~DefeatScene()
{
	//释放摄像头和控制器
	SafeRelease(&m_camera);
	SafeRelease(&m_comment);
}

bool DefeatScene::Init(App* app)
{
	Reset();

	if (!m_ctrl)
		m_ctrl = new DefeatSceneCtrl(app, this);

	//初始化摄像头
	FreeCamera* fcamera = FreeCamera::create(Vec4f(0.f, 0.f, 35.f, 1.f), Vec4f(0.f, 0.f, 0.f, 1.f), Vec4f(0.f, 1.f, 0.f));
	fcamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	return true;
}

T3D::SCENE DefeatScene::Update(float interval)
{
	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	std::vector<float> scales;
	contexts.push_back("DEFEAT");
	colors.push_back(Vec4f(1.f, 1.f, 1.f, 1.f));
	poss.push_back(Vec4f(-0.05f, 0.f, 0.f));
	m_comment->Set(contexts, colors, poss);

	m_comment->Render();

	m_ctrl->Frame(interval);

	m_switchCDTime -= interval;
	if (m_switchCDTime < 0.f) {
		if (m_endFlag) return T3D::SCENE::OPEN;
		else return T3D::SCENE::DEFEAT;
	}
	else {
		return T3D::SCENE::DEFEAT;
	}
};

void DefeatScene::Reset()
{
	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	m_comment->Set(contexts, colors, poss);

	m_endFlag = false;

	m_switchCDTime = 2.f;
}

void DefeatScene::end()
{
	m_endFlag = true;
}

////////////////////////////////////////////OpenSceneCtrl///////////////////////////////////////////////////////////////

DefeatSceneCtrl::DefeatSceneCtrl(_In_ App* app, _In_ DefeatScene* scene) :Controller(app)
{
	m_scene = scene;
}

void DefeatSceneCtrl::Frame(_In_ float interval)
{
	if (!m_scene)
		return;

	for (int i = 0; i < 256; i++) {
		if (keys[i] == KEY_PRESS || keys[i] == KEY_DOWN) {
			if (m_scene)
				m_scene->end();
		}
	}
}