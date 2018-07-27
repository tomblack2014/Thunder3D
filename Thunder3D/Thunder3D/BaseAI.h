#pragma once

#include "craft.h"
#include "Bullet.h"
#include <list>

class BaseAI : public Object
{
public:
	BaseAI(Craft* craft, int hp);

	~BaseAI();

	bool Damage(int damage);

	Vec4f GetPos();

	void SetPlayerPos(Vec4f pos);

	/**
	*  \brief  碰撞检测
	*
	*  \param  point 目标点
	*
	*  \param  radius 目标点碰撞半径
	*
	*  \param  isDead 有没有击杀this
	*
	*  \return 产生碰撞的数量，最小为0
	*/
	//判断point是否在飞船的表面
	//用半径什么的卡一下就行，不要求很精确
	int Collide(_In_ const Vec4f& point, float radius, bool& isDead);

	bool Collide(_In_ const Vec4f& point);

protected:
	Vec4f m_playerPos;

	Craft* m_craft;
	std::list<Bullet*> m_bullets;

	int m_MaxHP;
	int m_curHP;
};