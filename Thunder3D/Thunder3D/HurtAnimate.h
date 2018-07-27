#pragma once

#include "object.h"
#include "camera.h"

class HurtAnimate : public Object
{
public:
	/**
	*  \brief  构造方法
	*/
	static HurtAnimate* create(Camera* camera);

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

	void NewHurt();

private:
	HurtAnimate(Camera* camera);

	Camera* m_camera;
	float m_curAniTime;
	const float ANI_TIME;
};

inline HurtAnimate* HurtAnimate::create(Camera* camera) {
	HurtAnimate* ret = new HurtAnimate(camera);
	ret->AddRef();
	return ret;
}