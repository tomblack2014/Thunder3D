#pragma once

#include "craft.h"
#include "Bullet.h"
#include "BaseAI.h"
#include "Strip.h"
#include <list>

///////////////////////////////////////////
//Boss行为模式：
//
//第一阶段(>50%)：位置缓缓漂移，子弹分两种模式
//模式一：极快速度转向player并射出一条指向player的追踪射线，一段时间后停止追踪，
//经过短暂停顿后向射线位置以螺旋向外扩展的形式射出大量子弹
//模式二：以极快速度转向player，短暂停顿后射出十字斩形状的子弹
//
//第二阶段(10%-50%)：每隔一段时间进行一次瞬移，瞬移地点进行提示，子弹分两种模式
//模式一：极快速度转向player并射出一条指向player的追踪射线，一段时间后停止追踪，
//经过短暂停顿后向除射线位置 以外 的区域螺旋向外扩展的形式射出大量子弹
//模式二：快速旋转进行子弹乱舞
//
//第三阶段（<10%，准备时间较长）：位置固定，中速旋转追踪player同时射出大量子弹，每隔5s停止2s
//////////////////////////////////////////

class Boss : public BaseAI
{
public:
	~Boss();

	static Boss* create(Craft* craft, int hp);

	/**
	*  \brief  刷新一帧
	*
	*  \param  interval	两帧之间的时间间隔
	*/
	void Frame(_In_ float interval);

	/**
	*  \brief  渲染
	*/
	void Render();

	int GetHP();

	int Collide(_In_ const Vec4f& point, float radius, bool& isDead);

	bool Collide(_In_ const Vec4f& point);

private:
	Boss(Craft* craft, int hp);

	void DrawLockBeam();

	void DrawPyramin(float time);

	void Shoot(Vec4f sPos, Vec4f v, float lifetime);

	void RectShoot(Vec4f up, Vec4f right, Vec4f v);

	void RanV();

	void Status1(float interval);
	void Status2(float interval);
	void Status3(float interval);

	int m_status;
	int m_subStatus;

	Vec4f m_v;

	float m_cdTime;
	int m_flag[3][3];	//各关卡用来标记状态
	float m_timer[3][4];//各关卡用来计时
	float m_shootCD[3][3];//各关卡计算射击cd

	Vec4f m_lastPPos;	//画追踪射线用

	//状态一
	int m_frameCount1_2;
	
	//状态二
	int m_movePosX, m_movePosY, m_movePosZ;
	float m_shootAng2_1;
	Vec4f m_tarVec2_2;
};

inline Boss* Boss::create(Craft* craft, int hp)
{
	if (!craft)
		return NULL;
	Boss* ret = new Boss(craft, hp);
	ret->AddRef();
	return ret;
}

inline int Boss::GetHP()
{
	return m_curHP;
}
