#include "Chapter2Scene.h"

#include <stdlib.h>
#include <time.h>

Chapter2Scene::Chapter2Scene(Comment* comment) :
	MAX_LIFE(100),
	sun(NULL),
	m_craft(NULL),
	m_cubemap(NULL),
	m_partisys(NULL),
	m_HP(NULL),
	m_Speed(NULL),
	m_hurtAni(NULL),
	GENE_ENEMY_CD(5.f),
	MAX_ENEMY_NUM(5),
	m_comment(comment)
{
	if (m_comment)
		m_comment->AddRef();
}

Chapter2Scene::~Chapter2Scene()
{
	//释放场景对象
	SafeRelease(&sun);
	SafeRelease(&m_craft);
	SafeRelease(&m_cubemap);
	SafeRelease(&m_partisys);
	SafeRelease(&m_HP);
	SafeRelease(&m_Speed);
	SafeRelease(&m_hurtAni);
	SafeRelease(&m_comment);

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();

	for (auto iter : m_enemys) {
		SafeRelease(&iter);
	}
	m_enemys.clear();

	for (auto iter : m_collision) {
		SafeRelease(&iter);
	}
	m_collision.clear();

	//释放摄像头和控制器
	SafeRelease(&m_camera);
}


bool Chapter2Scene::Init(App* app)
{
	SafeRelease(&m_craft);
	m_craft = Craft::create(Vec4f(3.f, 3.f, 3.f, 1.f), Vec4f(1.5f, 0.f, 0.f), Vec4f(0.f, 0.5f, 0.f));
	m_craft->Initialize("config/craft.bin");

	//初始化摄像头
	CraftCamera* ccamera = CraftCamera::create(m_craft);
	ccamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	//初始化场景对象
	sun = Star::create(Vec4f(0.f, 0.f, 0.f, 1.f), 4.f, 2.f);
	m_cubemap = CubeMap::create(500.0f);
	m_partisys = ParticleSystem::create(m_craft, m_camera);
	m_HP = Strip::create(ccamera);
	m_HP->SetPos(Vec4f(-8.5f, 7.f, 0.f), Vec4f(-3.f, 6.3f, 0.f));
	m_Speed = Strip::create(ccamera);
	m_Speed->SetPos(Vec4f(-7.f, 5.5f, 0.f), Vec4f(-3.f, 4.8f, 0.f));
	m_Speed->SetColor(Vec4f(1.f, 1.f, 0.f, 1.f));
	m_hurtAni = HurtAnimate::create(ccamera);

	sun->Initialize("config/sphere.bin", "config/red.bmp");
	m_cubemap->Initialize("config/1.bmp");
	m_partisys->Initialize("config/Particle.png");

	m_ctrl = new C2Controllor(app, m_craft, ccamera, this);
	
	Reset();

	//释放临时变量
	SafeRelease(&ccamera);

	return true;
}

T3D::SCENE Chapter2Scene::Update(float interval)
{
	char str[50];
	sprintf_s(str, "Enemy left:%d", m_leftEnemy);
	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	contexts.push_back(str);
	contexts.push_back("speed");
	contexts.push_back("HP");
	colors.push_back(Vec4f(1.f, 1.f, 1.f, 1));
	colors.push_back(Vec4f(1.f, 1.f, 0.f, 1));
	colors.push_back(Vec4f(1.f, 0.f, 0.f, 1));
	poss.push_back(Vec4f(0.15f, 0.32f, 0.f));
	poss.push_back(Vec4f(-0.5f, 0.25f, 0.f));
	poss.push_back(Vec4f(-0.5f, 0.32f, 0.f));
	m_comment->Set(contexts, colors, poss);
	m_comment->Render();

	//控制器处理
	//因为要处理点选，所以不能清屏
	m_camera->LoadMatrix();

	//前进一帧
	m_ctrl->Frame(interval);
	m_craft->Frame(interval);
	sun->Frame(interval);
	m_cubemap->Frame(interval);
	m_partisys->Frame(interval);
	m_hurtAni->Frame(interval);
	//m_partisys->SetPos(m_craft->GetPos() - m_craft->GetSpeed());
	for (auto & iter : m_bullets) {
		iter->Frame(interval);
	}
	for (auto & iter : m_enemys) {
		iter->Frame(interval);
		iter->SetPlayerPos(m_craft->GetPos());
	}
	for (auto & iter : m_collision) {
		iter->Frame(interval);
	}

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
	for (auto iter = m_enemys.begin(); iter != m_enemys.end();) {
		//撞到本体直接消失掉血
		bool isDead = false;
		int damage = (*iter)->Collide(m_craft->GetPos(), T3D::MODEL_SIZE, isDead);
		if (damage > 0) {
			m_life -= damage;
			m_hurtAni->NewHurt();
			//game over
			if (m_life <= 0) {
				return T3D::SCENE::DEFEAT;
			}
		}
		if (isDead) {
			CollisionPartSys* coll = CollisionPartSys::create(m_camera, (*iter)->GetPos());
			coll->Initialize("config/Particle.png");
			m_collision.push_back(coll);

			SafeRelease(&(*iter));
			iter = m_enemys.erase(iter);
			m_leftEnemy--;
			continue;
		}

		bool flag = false;
		for (auto iter2 = m_bullets.begin(); iter2 != m_bullets.end();) {
			if ((*iter)->Collide((*iter2)->GetPos())) {
				SafeRelease(&(*iter2));
				iter2 = m_bullets.erase(iter2);
				//暂时子弹造成1点伤害
				if ((*iter)->Damage(1)) {
					CollisionPartSys* coll = CollisionPartSys::create(m_camera, (*iter)->GetPos());
					coll->Initialize("config/Particle.png");
					m_collision.push_back(coll);

					SafeRelease(&(*iter));
					iter = m_enemys.erase(iter);
					m_leftEnemy--;
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

	for (auto & iter : m_enemys) {
		iter->Render();
	}

	//生成敌机
	m_geneEnemyCD -= interval;
	if (m_geneEnemyCD < 0.f && m_enemys.size() < MAX_ENEMY_NUM && m_enemys.size() < m_leftEnemy) {
		CreateEnemy();
		m_geneEnemyCD = GENE_ENEMY_CD;
	}

	//摄像机需要放在最后
	//不然会出现跟踪的抖动
	m_camera->Frame(interval);

	m_camera->LoadMatrix();
	m_craft->Render();
	sun->Render();
	m_cubemap->Render();
	m_partisys->Render();
	m_HP->Render();
	m_HP->SetValue(m_life);
	m_Speed->Render();
	m_Speed->SetValue(m_craft->GetSpeed().length() * 100.f / 5.f);
	for (auto & iter : m_collision) {
		iter->Render();
	}
	m_hurtAni->Render();

	if (m_leftEnemy <= 0) {
		return T3D::SCENE::CHAPTER3;
	}

	return T3D::SCENE::CHAPTER2;
}

void Chapter2Scene::Reset()
{
	if (m_ctrl)
		m_ctrl->ResetKeyState();

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();
	for (auto iter : m_enemys) {
		SafeRelease(&iter);
	}
	m_enemys.clear();
	for (auto iter : m_collision) {
		SafeRelease(&iter);
	}
	m_collision.clear();

	m_craft->SetPos(Vec4f(3.f, 3.f, 3.f, 1.f));

	m_life = MAX_LIFE;

	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	m_comment->Set(contexts, colors, poss);

	m_leftEnemy = 20;
}

void Chapter2Scene::Shoot(Vec4f pos, Vec4f v, Vec4f up)
{
	v.normal();
	Vec4f right = v.cross(up);
	right.normal();
	for (int i = 0; i < 2; i++) {
		auto bullet = Bullet::create(
			pos + right * 0.1 * (i == 0 ? 1 : -1),
			v * 3,
			6.f,
			0.02f);
		bullet->Initialize("config/sphere.bin", "config/red.bmp");
		m_bullets.push_back(bullet);
	}
}

void Chapter2Scene::CreateEnemy()
{
	srand(time(NULL));
	int range = 25;
	auto pos = Vec4f(rand() % range, rand() % range, rand() % range);
	auto v = Vec4f(rand() % range, rand() % range, rand() % range);
	Vec4f up = Vec4f(0.f, -1.f, 0.f);
	v.normal();

	auto craft = Craft::create(pos, v * 0.2, up);
	craft->Initialize("config/craft.bin");
	auto enemy = SimpleEnemy::create(craft, 1);
	m_enemys.push_back(enemy);
}

///////////////////////////////C2Controllor/////////////////////////////////////

const float C2Controllor::TRANS_SPEED = 0.3f;
const float C2Controllor::ROTATE_SPEED = 0.4f;

C2Controllor::C2Controllor(
	_In_ App* app,
	_In_ Craft* craft,
	_In_ CraftCamera* camera,
	_In_ Chapter2Scene* scene) :
	Controller(app),
	m_craft(craft),
	m_camera(camera),
	m_scene(scene),
	SHOOT_CD(0.2),
	m_shootCD(0.f)
{
	m_craft->AddRef();
}

C2Controllor::~C2Controllor()
{
	SafeRelease(&m_craft);
	SafeRelease(&m_camera);
}

void C2Controllor::Frame(_In_ float interval)
{
	float tspd = TRANS_SPEED * interval;
	float rspd = ROTATE_SPEED * interval;
	m_shootCD -= interval;

	//处理方向键控制事件
	if (keys['W'] == KEY_PRESS || keys['W'] == KEY_DOWN) {
		m_craft->RotateUD(-rspd);
	}
	if (keys['S'] == KEY_PRESS || keys['S'] == KEY_DOWN) {
		m_craft->RotateUD(rspd);
	}
	if (keys['A'] == KEY_PRESS || keys['A'] == KEY_DOWN) {
		m_craft->RotateLR(-rspd);
	}
	if (keys['D'] == KEY_PRESS || keys['D'] == KEY_DOWN) {
		m_craft->RotateLR(rspd);
	}
	if (keys['Q'] == KEY_PRESS || keys['Q'] == KEY_DOWN) {
		//m_craft->Accelerate(tspd);
		m_camera->SetDis(interval * 2.5);
	}
	if (keys['E'] == KEY_PRESS || keys['E'] == KEY_DOWN) {
		//m_craft->Accelerate(-tspd);
		m_camera->SetDis(-interval * 2.5);
	}
	if (keys['C'] == KEY_PRESS || keys['C'] == KEY_DOWN) {
		m_camera->ChangeMode();
	}
	if (keys['U'] == KEY_PRESS || keys['U'] == KEY_DOWN) {
		m_craft->Accelerate(tspd);
	}
	if (keys['I'] == KEY_PRESS || keys['I'] == KEY_DOWN) {
		m_craft->Accelerate(-tspd);
	}
	if (keys['J'] == KEY_PRESS || keys['J'] == KEY_DOWN) {
		if (m_shootCD <= 0.f) {
			m_scene->Shoot(m_craft->GetPos(), m_craft->GetSpeed(), m_craft->GetUp());
			m_shootCD = SHOOT_CD;
		}
	}
}