#pragma once

#include "basescene.h"

#include "planet.h"
#include "star.h"
#include "craft.h"
#include "LightCone.h"
#include "ParticleSystem.h"
#include "SurroundBox.h"
#include "cubemap.h"
#include "controller.h"

class TestScene : public BaseScene
{
public:
	TestScene();

	~TestScene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	//创建标准控制器和摄像机
	//使用自由视角
	//这些函数的设计并不是最优设计
	void UseStandardView();

	//创建跟踪控制器和摄像机
	void UseTraceView(_In_ IObject* obj);

	//创建飞船控制器和摄像机
	void UseCraftView();

	//选择对象事件
	//point : 选择点
	void OnSelect(_In_ const Vec4f& point);

	//追踪对象事件
	//point : 选择点
	void OnTrace(_In_ const Vec4f& point);

private:
	Star* sun;
	Planet* earth;
	Planet* moon;
	Craft* craft;
	ParticleSystem* partisys;
	LightCone* lightcone;
	SurroundBox* surbox;
	CubeMap* cubemap;

	App* m_app;
};