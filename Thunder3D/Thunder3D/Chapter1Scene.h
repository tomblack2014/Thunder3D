#pragma once

#include "basescene.h"

#include "star.h"
#include "camera.h"
#include "controller.h"
#include "Bullet.h"
#include "Comment.h"
#include "FrontSight.h"
#include "Strip.h"
#include "HurtAnimate.h"
#include "CollisionPartSys.h"
#include <list>

class Chapter1Scene : public BaseScene
{
public:
	Chapter1Scene(Comment* comment);

	~Chapter1Scene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	void Shoot(Vec4f pos, Vec4f v);

private:
	typedef std::pair<Bullet*, int> Rock_Life;

	void CreateRock();
	bool CheckCollide(Vec4f pos1, Vec4f pos2, float threshold);

	std::list<Bullet*> m_bullets;//管理所有子弹
	std::list<Rock_Life> m_rocks;//管理所有陨石
	std::list<CollisionPartSys*> m_collision;//管理所有爆炸特效

	FrontSight* m_frontSight;
	Strip* m_HP;
	HurtAnimate* m_hurtAni;
	Comment* m_comment;

	App* m_app;

	float geneRockCD;
	const float GENE_ROCK_CD;

	const int MAX_LIFE;
	int m_life;
	float m_leftTime;
};

inline bool Chapter1Scene::CheckCollide(Vec4f pos1, Vec4f pos2, float threshold)
{
	if ((pos1.x - pos2.x) * (pos1.x - pos2.x) + 
		(pos1.y - pos2.y) * (pos1.y - pos2.y) + 
		(pos1.y - pos2.y) * (pos1.y - pos2.y) < threshold * threshold)
		return true;
	return false;
}

class C1Controllor : public Controller
{
public:
	C1Controllor(_In_ FreeCamera* camera, _In_ App* app, _In_ Chapter1Scene* scene);

	~C1Controllor();

	void Frame(_In_ float interval);
private:
	bool CheckLimit(float UDAng, float LRAng) const;

	//控制的自由摄像机
	FreeCamera* m_camera;

	Chapter1Scene* m_scene;

	//旋转速度
	static const float ROTATE_SPEED;

	static const float ROTATE_LIMIT;

	float m_curUDAng;
	float m_curLRAng;
	float m_shootCD;
	const float SHOOT_CD;
};

inline bool C1Controllor::CheckLimit(float UDAng, float LRAng) const 
{
	if (UDAng * UDAng + LRAng * LRAng < ROTATE_LIMIT * ROTATE_LIMIT)
		return true;
	return false;
}