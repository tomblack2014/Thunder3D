#include "Chapter3Scene.h"

Chapter3Scene::Chapter3Scene(Comment* comment):
	MAX_LIFE(100),
	m_craft(NULL),
	m_cubemap(NULL),
	m_partisys(NULL),
	m_HP(NULL),
	m_BossHP(NULL),
	m_Speed(NULL),
	m_hurtAni(NULL),
	m_boss(NULL),
	m_comment(comment)
{
	if (m_comment)
		m_comment->AddRef();
}

Chapter3Scene::~Chapter3Scene()
{
	//释放场景对象
	SafeRelease(&m_craft);
	SafeRelease(&m_cubemap);
	SafeRelease(&m_partisys);
	SafeRelease(&m_HP);
	SafeRelease(&m_BossHP);
	SafeRelease(&m_Speed);
	SafeRelease(&m_hurtAni);
	SafeRelease(&m_boss);

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();

	//释放摄像头和控制器
	SafeRelease(&m_camera);
}

bool Chapter3Scene::Init(App* app)
{
	SafeRelease(&m_craft);
	m_craft = Craft::create(Vec4f(3.f, 3.f, 3.f, 1.f), Vec4f(1.5f, 0.f, 0.f), Vec4f(0.f, 0.5f, 0.f));
	m_craft->Initialize("config/craft.bin");

	//初始化摄像头
	CraftCamera* ccamera = CraftCamera::create(m_craft);
	ccamera->QueryInterface(ICamera::IID_ICamera, (void**)&m_camera);

	//初始化场景对象
	m_cubemap = CubeMap::create(500.0f);
	m_partisys = ParticleSystem::create(m_craft, m_camera);
	m_HP = Strip::create(ccamera);
	m_HP->SetPos(Vec4f(-8.5f, 7.f, 0.f), Vec4f(-3.f, 6.3f, 0.f));
	m_BossHP = Strip::create(ccamera);
	m_BossHP->SetPos(Vec4f(3.f, 7.f, 0.f), Vec4f(10.f, 6.3f, 0.f));
	m_BossHP->SetColor(Vec4f(1.f, 1.f, 224.f / 255.f, 1.f));
	m_Speed = Strip::create(ccamera);
	m_Speed->SetPos(Vec4f(-7.f, 5.5f, 0.f), Vec4f(-3.f, 4.8f, 0.f));
	m_Speed->SetColor(Vec4f(1.f, 1.f, 0.f, 1.f));
	m_hurtAni = HurtAnimate::create(ccamera);

	m_cubemap->Initialize("config/1.bmp");
	m_partisys->Initialize("config/Particle.png");

	m_ctrl = new C3Controllor(app, m_craft, ccamera, this);
	
	Reset();

	//释放临时变量
	SafeRelease(&ccamera);

	return true;
}

T3D::SCENE Chapter3Scene::Update(float interval)
{
	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	contexts.push_back("speed");
	contexts.push_back("HP");
	contexts.push_back("BOSS");
	colors.push_back(Vec4f(1.f, 1.f, 0.f, 1));
	colors.push_back(Vec4f(1.f, 0.f, 0.f, 1));
	colors.push_back(Vec4f(1.f, 1.f, 1.f, 1));
	poss.push_back(Vec4f(-0.5f, 0.25f, 0.f));
	poss.push_back(Vec4f(-0.5f, 0.32f, 0.f));
	poss.push_back(Vec4f(0.04f, 0.32f, 0.f));
	m_comment->Set(contexts, colors, poss);
	m_comment->Render();

	//控制器处理
	//因为要处理点选，所以不能清屏
	m_camera->LoadMatrix();

	//前进一帧
	m_ctrl->Frame(interval);
	m_cubemap->Frame(interval);
	m_partisys->Frame(interval);
	m_hurtAni->Frame(interval);
	m_boss->SetPlayerPos(m_craft->GetPos());
	m_boss->Frame(interval);
	m_craft->Frame(interval);
	for (auto & iter : m_bullets) {
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

	//与boss碰撞(直接die)
	bool isDead = false;
	int damage = m_boss->Collide(m_craft->GetPos(), T3D::MODEL_SIZE, isDead);
	if (damage > 0) {
		//boss战两倍伤害
		m_life -= damage * 2;
		m_hurtAni->NewHurt();
		//game over
		if (m_life <= 0) {
			return T3D::SCENE::DEFEAT;
		}
	}

	//子弹打击boss判定
	for (auto iter2 = m_bullets.begin(); iter2 != m_bullets.end();) {
		if (m_boss->Collide((*iter2)->GetPos())) {
			SafeRelease(&(*iter2));
			iter2 = m_bullets.erase(iter2);
			//胜利
			if (m_boss->Damage(1)) {
				return T3D::SCENE::VICTORY;
			}
			continue;
		}
		iter2++;
	}

	//摄像机需要放在最后
	//不然会出现跟踪的抖动
	m_camera->Frame(interval);

	m_camera->LoadMatrix();
	m_craft->Render();
	m_cubemap->Render();
	m_partisys->Render();
	m_HP->Render();
	m_HP->SetValue(m_life);
	m_BossHP->Render();
	m_BossHP->SetValue(m_boss->GetHP() * 100.f / T3D::BOSS_HP);
	m_Speed->Render();
	m_Speed->SetValue(m_craft->GetSpeed().length() * 100.f / 5.f);
	m_boss->Render();
	m_hurtAni->Render();

	return T3D::SCENE::CHAPTER3;
}

void Chapter3Scene::Reset()
{
	if (m_ctrl)
		m_ctrl->ResetKeyState();

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();

	m_craft->SetPos(Vec4f(3.f, 3.f, 3.f, 1.f));

	SafeRelease(&m_boss);
	auto bossCraft = Craft::create(Vec4f(3.f, 3.f, 3.f, 1.f), Vec4f(1.5f, 0.f, 0.f), Vec4f(0.f, 0.5f, 0.f));
	bossCraft->Initialize("config/craft.bin");
	m_boss = Boss::create(bossCraft, T3D::BOSS_HP);

	m_life = MAX_LIFE;

	std::vector<std::string> contexts;
	std::vector<Vec4f> colors;
	std::vector<Vec4f> poss;
	m_comment->Set(contexts, colors, poss);
}

void Chapter3Scene::Shoot(Vec4f pos, Vec4f v, Vec4f up)
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

///////////////////////////////C3Controllor/////////////////////////////////////

const float C3Controllor::TRANS_SPEED = 0.8f;
const float C3Controllor::ROTATE_SPEED = 0.4f;

C3Controllor::C3Controllor(
	_In_ App* app,
	_In_ Craft* craft,
	_In_ CraftCamera* camera,
	_In_ Chapter3Scene* scene) :
	Controller(app),
	m_craft(craft),
	m_camera(camera),
	m_scene(scene),
	SHOOT_CD(0.2),
	m_shootCD(0.f)
{
	m_craft->AddRef();
}

C3Controllor::~C3Controllor()
{
	SafeRelease(&m_craft);
	SafeRelease(&m_camera);
}

void C3Controllor::Frame(_In_ float interval)
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