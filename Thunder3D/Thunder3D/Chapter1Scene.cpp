#include "Chapter1Scene.h"

#include <gl\GL.h>
#include <gl\GLU.h>
#include <stdlib.h>
#include <time.h>

Chapter1Scene::Chapter1Scene(Comment* comment):
	MAX_LIFE(100),
	GENE_ROCK_CD(0.5),
	m_frontSight(NULL),
	m_HP(NULL),
	m_hurtAni(NULL),
	m_comment(comment)
{
	if (m_comment)
		m_comment->AddRef();
}

Chapter1Scene::~Chapter1Scene()
{
	//释放场景对象
	SafeRelease(&m_camera);
	SafeRelease(&m_frontSight);
	SafeRelease(&m_HP);
	SafeRelease(&m_hurtAni);
	SafeRelease(&m_comment);

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();

	for (auto iter : m_rocks) {
		SafeRelease(&iter.first);
	}

	for (auto iter : m_collision) {
		SafeRelease(&iter);
	}
	m_collision.clear();
}

bool Chapter1Scene::Init(App* app)
{
	m_app = app;

	//初始化场景对象
	FreeCamera* fcamera = FreeCamera::create(Vec4f(0.f, 0.f, 35.f, 1.f), Vec4f(0.f, 0.f, 0.f, 1.f), Vec4f(0.f, 1.f, 0.f));
	fcamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	m_ctrl = new C1Controllor(fcamera, m_app, this);
	m_frontSight = FrontSight::create(fcamera);
	m_HP = Strip::create(fcamera);
	m_hurtAni = HurtAnimate::create(fcamera);

	Reset();

	//释放临时变量
	SafeRelease(&fcamera);

	return true;
}

T3D::SCENE Chapter1Scene::Update(float interval)
{
	char str[50];
	sprintf_s(str, "Time left:%d", (int)m_leftTime);
	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	contexts.push_back(str);
	colors.push_back(Vec4f(1.f, 1.f, 1.f, 1));
	poss.push_back(Vec4f(0.2f, 0.3f, 0.f));
	m_comment->Set(contexts, colors, poss);
	m_comment->Render();

	//控制器处理
	//因为要处理点选，所以不能清屏
	m_camera->LoadMatrix();

	//前进一帧
	m_ctrl->Frame(interval);
	for (auto & iter : m_bullets) {
		iter->Frame(interval);
	}
	for (auto & iter : m_rocks) {
		iter.first->Frame(interval);
	}
	for (auto & iter : m_collision) {
		iter->Frame(interval);
	}

	m_hurtAni->Frame(interval);

	//摄像机需要放在最后
	//不然会出现跟踪的抖动
	m_camera->Frame(interval);

	//子弹生命周期判断
	m_camera->LoadMatrix();
	for (auto iter = m_bullets.begin(); iter != m_bullets.end();) {
		if ((*iter)->GetLifeTime() < 0) {
			SafeRelease(&(*iter));
			iter = m_bullets.erase(iter);
			continue;
		}
		(*iter)->Render();
		iter++;
	}

	//碰撞判断
	for (auto iter = m_rocks.begin(); iter != m_rocks.end();) {
		//撞到本体直接消失掉血
		if (iter->first->GetPos().z > 34.7f) {
			m_life -= iter->second;
			SafeRelease(&iter->first);
			iter = m_rocks.erase(iter);
			m_hurtAni->NewHurt();
			//game over
			if (m_life <= 0) {
				return T3D::SCENE::DEFEAT;
			}
			continue;
		}

		bool flag = false;
		for (auto iter2 = m_bullets.begin(); iter2 != m_bullets.end();) {
			if (CheckCollide((*iter2)->GetPos(), iter->first->GetPos(), 0.05f)) {
				iter->second--;
				SafeRelease(&(*iter2));
				iter2 = m_bullets.erase(iter2);
				if (iter->second <= 0) {
					CollisionPartSys* coll = CollisionPartSys::create(m_camera, iter->first->GetPos());
					coll->Initialize("config/Particle.png");
					coll->SetCollisionR(15.f);
					coll->SetLifeTime(0.3f);
					m_collision.push_back(coll);
					SafeRelease(&iter->first);
					iter = m_rocks.erase(iter);
					flag = true;
					break;
				}
				continue;
			}
			iter2++;
		}
		if (!flag)
			iter++;
	}

	//collision生命周期判定
	for (auto iter = m_collision.begin(); iter != m_collision.end();) {
		if (!(*iter)->IsExist()) {
			SafeRelease(&(*iter));
			iter = m_collision.erase(iter);
			continue;
		}
		iter++;
	}

	for (auto iter : m_rocks) {
		iter.first->Render();
	}

	//生成陨石
	geneRockCD -= interval;
	if (geneRockCD < 0.f) {
		CreateRock();
		geneRockCD = GENE_ROCK_CD;
	}

	//准星&血条
	m_frontSight->Render();
	m_HP->Render();
	m_HP->SetValue(m_life);
	m_hurtAni->Render();
	for (auto & iter : m_collision) {
		iter->Render();
	}

	m_leftTime -= interval;

	if (m_leftTime <= 0.f) {
		return T3D::SCENE::CHAPTER2;
	}

	//判断是否死亡
	if (m_life <= 0)
		return T3D::SCENE::OPEN;

	return T3D::SCENE::CHAPTER1;
}

void Chapter1Scene::Reset()
{
	if (m_ctrl)
		m_ctrl->ResetKeyState();

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();

	for (auto iter : m_rocks) {
		SafeRelease(&iter.first);
	}
	m_rocks.clear();
	for (auto iter : m_collision) {
		SafeRelease(&iter);
	}
	m_collision.clear();

	geneRockCD = 0.f;

	m_life = MAX_LIFE;

	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	m_comment->Set(contexts, colors, poss);

	m_leftTime = 60.f;
}

void Chapter1Scene::Shoot(Vec4f pos, Vec4f v)
{
	v.normal();
	auto bullet = Bullet::create(pos + v * 0.2, v * 3, 4.f, 0.02f);
	bullet->Initialize("config/sphere.bin", "config/blue.bmp");
	bullet->AddRotation(Vec4f(1.f, 0.f, 0.f, 0.f), 50.f);
	m_bullets.push_back(bullet);
}

void Chapter1Scene::CreateRock()
{
	srand(time(NULL));
	auto pos = Vec4f(rand() % 25 / 10.f, rand() % 25 / 10.f, 30);
	Vec4f v = m_camera->GetPos() - pos;
	v.normal();

	int level = rand() % 3;
	auto rock = Bullet::create(pos, v * 0.2 * (3 - level), 20, 0.06);
	rock->Initialize("config/sphere.bin", "config/gray.bmp");
	rock->AddRotation(Vec4f(1.f, 0.f, 0.f, 0.f), 50.f);
	m_rocks.push_back(Rock_Life(rock, level + 1));
}

////////////////////////////////////////////////////C1Controllor/////////////////////////////////////

const float C1Controllor::ROTATE_LIMIT = 0.6f;
const float C1Controllor::ROTATE_SPEED = 0.25f;

C1Controllor::C1Controllor(_In_ FreeCamera* camera, _In_ App* app, _In_ Chapter1Scene* scene):
	Controller(app),
	m_camera(camera),
	m_curLRAng(0),
	m_curUDAng(0),
	m_scene(scene),
	SHOOT_CD(0.2),
	m_shootCD(0.f)
{
	camera->AddRef();
}

C1Controllor::~C1Controllor()
{
	SafeRelease(&m_camera);
}

void C1Controllor::Frame(_In_ float interval)
{
	float rspd = ROTATE_SPEED * interval;
	m_shootCD -= interval;

	//仅处理旋转
	if (keys['W'] == KEY_PRESS || keys['W'] == KEY_DOWN) {
		if (CheckLimit(m_curUDAng - rspd, m_curLRAng)) {
			m_curUDAng -= rspd;
			m_camera->RotateUD(-rspd);
		}
	}
	if (keys['S'] == KEY_PRESS || keys['S'] == KEY_DOWN) {
		if (CheckLimit(m_curUDAng + rspd, m_curLRAng)) {
			m_curUDAng += rspd;
			m_camera->RotateUD(rspd);
		}
	}
	if (keys['A'] == KEY_PRESS || keys['A'] == KEY_DOWN) {
		if (CheckLimit(m_curUDAng, m_curLRAng - rspd)) {
			m_curLRAng -= rspd;
			m_camera->RotateLR(-rspd);
		}
	}
	if (keys['D'] == KEY_PRESS || keys['D'] == KEY_DOWN) {
		if (CheckLimit(m_curUDAng, m_curLRAng + rspd)) {
			m_curLRAng += rspd;
			m_camera->RotateLR(rspd);
		}
	}
	if (keys['J'] == KEY_PRESS || keys['J'] == KEY_DOWN) {
		if (m_shootCD <= 0.f) {
			m_scene->Shoot(m_camera->GetPos(), m_camera->GetEye2At());
			m_shootCD = SHOOT_CD;
		}
	}
}