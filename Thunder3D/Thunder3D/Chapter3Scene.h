#pragma once

#include "basescene.h"

#include "camera.h"
#include "controller.h"
#include "Boss.h"
#include "Strip.h"
#include "Bullet.h"
#include "craft.h"
#include "ParticleSystem.h"
#include "CollisionPartSys.h"
#include "cubemap.h"
#include "HurtAnimate.h"
#include "Comment.h"

#include <list>

class Chapter3Scene : public BaseScene
{
public:
	Chapter3Scene(Comment* comment);

	~Chapter3Scene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	void Shoot(Vec4f pos, Vec4f v, Vec4f up);

private:

	App* m_app;
	Craft* m_craft;
	CubeMap* m_cubemap;
	ParticleSystem* m_partisys;
	Strip* m_HP;
	Strip* m_Speed;
	Strip* m_BossHP;
	HurtAnimate* m_hurtAni;
	Comment* m_comment;
	Boss* m_boss;

	std::list<Bullet*> m_bullets;//管理所有子弹

	const int MAX_LIFE;
	int m_life;
};

class C3Controllor : public Controller
{
public:
	C3Controllor(
		_In_ App* app,
		_In_ Craft* craft,
		_In_ CraftCamera* camera,
		_In_ Chapter3Scene* scene);

	~C3Controllor();

	void Frame(_In_ float interval);
private:
	//飞船
	Craft* m_craft;

	//相机
	CraftCamera* m_camera;

	Chapter3Scene* m_scene;

	//平移速度
	static const float TRANS_SPEED;

	//旋转速度
	static const float ROTATE_SPEED;

	float m_shootCD;
	const float SHOOT_CD;
};