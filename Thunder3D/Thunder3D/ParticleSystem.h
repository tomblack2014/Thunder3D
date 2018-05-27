#ifndef _PARTICLESYSTEM_H
#define _PARTICLESYSTEM_H

#include "object.h"
#include "camera.h"
#include "craft.h"
#include <list>

class ParticleSystem : public Object{
private:
	struct Particle{
		//位置
		Vec4f pos;

		//速度
		Vec4f speed;

		//受力
		Vec4f force;

		//生命期
		float life;

		//淡出时间
		float fade;

		void DoWrap(_In_ const Vec4f& force, _In_ float turbul, _In_ float resist);

		//前进一帧
		void Frame(_In_ float interval);

		//渲染
		void Render(_In_ ICamera* camera);

		Particle(){};

		Particle(_In_ const Vec4f& pos, _In_ const Vec4f& speed, _In_ const Vec4f& force, _In_ const float fade);
	};

	//粒子链表
	std::list<Particle> plist;

	//飞船
	Craft* craft;

	//摄像机
	ICamera* camera;

	//速度
	Vec4f speed;
	
	//临时变量
	Vec4f up, nVec;

	//当前时限
	float tim;

	//产生时限
	static const float TIME_THRES;

	//纹理贴图
	unsigned int texture;

	//添加一个粒子
	void AddParticle();

	ParticleSystem(_In_ Craft* craft, _In_ ICamera* camera);

public:
	~ParticleSystem();

	//前进一帧
	void Frame(_In_ float interval);

	//渲染
	void Render();

	//初始化
	bool Initialize(_In_ char* filename);

	//更换摄像机
	void UpdateCamera(_In_ ICamera* newcamera);

	static ParticleSystem* create(_In_ Craft* craft, _In_ ICamera* camera);
};

#endif