#pragma once

#include "vec4f.h"
#include "camera.h"

struct Particle {
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

	Particle() {};

	Particle(_In_ const Vec4f& pos, _In_ const Vec4f& speed, _In_ const Vec4f& force, _In_ const float fade);
};