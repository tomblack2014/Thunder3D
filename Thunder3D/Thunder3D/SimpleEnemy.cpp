#include "SimpleEnemy.h"
#include "global.h"

SimpleEnemy::~SimpleEnemy()
{

}

void SimpleEnemy::Frame(_In_ float interval)
{
	m_craft->Frame(interval);
	for (auto iter : m_bullets) {
		iter->Frame(interval);
	}

	//一直向目标点旋转
	const float ROTATE_SPEED = 0.4f;
	Vec4f moveTo = m_playerPos - GetPos();
	Vec4f v = m_craft->GetSpeed();
	float ang = moveTo.CalAng(v);
	if (fabs(ang) > 0.01f) {
		float rAng = interval * ROTATE_SPEED;
		if (rAng > fabs(ang))
			rAng = ang;
		Vec4f axis = moveTo.cross(v);
		m_craft->Rotate(axis, -rAng);
	}
	m_craft->Rotate(v, interval * ROTATE_SPEED);

	//子弹生命周期判断
	for (auto iter = m_bullets.begin(); iter != m_bullets.end();) {
		if ((*iter)->GetLifeTime() < 0) {
			SafeRelease(&(*iter));
			iter = m_bullets.erase(iter);
			continue;
		}
		(*iter)->Render();
		iter++;
	}

	//发射子弹
	m_shootCD -= interval;
	if (m_shootCD < 0.f) {
		Shoot();
		m_shootCD = SHOOT_CD;
	}
}

void SimpleEnemy::Render()
{
	m_craft->Render();
	for (auto iter : m_bullets) {
		iter->Render();
	}
}

void SimpleEnemy::Shoot()
{
	Vec4f pos = m_craft->GetPos();
	Vec4f v = m_craft->GetSpeed();
	v.normal();

	Bullet* bullet = Bullet::create(pos, v * 1.5, 6.f, 0.02f);
	bullet->Initialize("config/sphere.bin", "config/blue.bmp");
	m_bullets.push_back(bullet);
}

SimpleEnemy::SimpleEnemy(Craft* craft, int hp) :
	BaseAI(craft, hp),
	SHOOT_CD(1.f)
{

}