#pragma once

#include "object.h"
#include "camera.h"
#include "Partical.h"

#include <list>

class CollisionPartSys : public Object
{
private:
	//粒子链表
	std::list<Particle> plist;

	//摄像机
	ICamera* camera;

	//临时变量
	Vec4f up, nVec;

	//当前时限
	float tim;

	//产生时限
	static const float TIME_THRES;

	//纹理贴图
	unsigned int texture;

	float m_life;

	float m_collisionR;

	float m_lifeTime;

	//添加一个粒子
	void AddParticle();

	CollisionPartSys(_In_ ICamera* camera, Vec4f pos);

public:
	~CollisionPartSys();

	//前进一帧
	void Frame(_In_ float interval);

	//渲染
	void Render();

	//初始化
	bool Initialize(_In_ char* filename);

	void SetPos(Vec4f pos);

	void SetCollisionR(float r);

	void SetLifeTime(float time);

	bool IsExist();

	static CollisionPartSys* create(_In_ ICamera* camera, Vec4f pos);
};