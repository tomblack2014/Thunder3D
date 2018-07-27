#pragma once

#include "basescene.h"

#include "camera.h"
#include "controller.h"
#include "Strip.h"
#include "Bullet.h"
#include "craft.h"
#include "ParticleSystem.h"
#include "CollisionPartSys.h"
#include "star.h"
#include "cubemap.h"
#include "SimpleEnemy.h"
#include "HurtAnimate.h"
#include "Comment.h"
#include <list>

class Chapter2Scene : public BaseScene
{
public:
	Chapter2Scene(Comment* comment);

	~Chapter2Scene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	void Shoot(Vec4f pos, Vec4f v, Vec4f up);

private:
	void CreateEnemy();

	App* m_app;
	Craft* m_craft;
	Star* sun;
	CubeMap* m_cubemap;
	ParticleSystem* m_partisys;
	Strip* m_HP;
	Strip* m_Speed;
	HurtAnimate* m_hurtAni;
	Comment* m_comment;

	std::list<SimpleEnemy*> m_enemys;//管理所有敌机
	std::list<Bullet*> m_bullets;//管理所有子弹
	std::list<CollisionPartSys*> m_collision;//管理所有爆炸特效

	const float GENE_ENEMY_CD;
	float m_geneEnemyCD;
	const int MAX_ENEMY_NUM;

	const int MAX_LIFE;
	int m_life;

	int m_leftEnemy;
};

class C2Controllor : public Controller
{
public:
	C2Controllor(
		_In_ App* app, 
		_In_ Craft* craft, 
		_In_ CraftCamera* camera, 
		_In_ Chapter2Scene* scene);

	~C2Controllor();

	void Frame(_In_ float interval);
private:
	//飞船
	Craft* m_craft;

	//相机
	CraftCamera* m_camera;

	Chapter2Scene* m_scene;

	//平移速度
	static const float TRANS_SPEED;

	//旋转速度
	static const float ROTATE_SPEED;

	float m_shootCD;
	const float SHOOT_CD;
};