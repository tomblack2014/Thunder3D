#include "BaseAI.h"
#include "global.h"

BaseAI::BaseAI(Craft* craft, int hp):
	m_craft(craft),
	m_MaxHP(hp),
	m_curHP(hp)
{

}

BaseAI::~BaseAI()
{
	SafeRelease(&m_craft);

	for (auto iter : m_bullets) {
		SafeRelease(&iter);
	}
	m_bullets.clear();
}

bool BaseAI::Damage(int damage)
{
	m_curHP -= damage;
	if (m_curHP <= 0)
		return true;
	return false;
}

Vec4f BaseAI::GetPos()
{
	return m_craft->GetPos();
}

void BaseAI::SetPlayerPos(Vec4f pos)
{
	m_playerPos = pos;
}

int BaseAI::Collide(_In_ const Vec4f& point, float radius, bool& isDead)
{
	int ret = 0;

	isDead = false;
	if (point.dist(m_craft->GetPos()) < T3D::MODEL_SIZE + radius) {
		ret++;
		isDead = true;
	}

	for (auto iter2 = m_bullets.begin(); iter2 != m_bullets.end();) {
		if (point.dist((*iter2)->GetPos()) < radius) {
			SafeRelease(&(*iter2));
			iter2 = m_bullets.erase(iter2);
			ret++;
			continue;
		}
		iter2++;
	}

	return ret;
}

bool BaseAI::Collide(_In_ const Vec4f& point)
{
	//模型大小预估值

	return point.dist(m_craft->GetPos()) < T3D::MODEL_SIZE;
}