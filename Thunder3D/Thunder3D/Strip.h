#pragma once

#include "object.h"
#include "camera.h"

class Strip : public Object
{
public:
	/**
	*  \brief  构造方法
	*/
	static Strip* create(Camera* camera);

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

	void SetValue(float val);

	void SetPos(Vec4f p1, Vec4f p2);

	void SetColor(Vec4f color);

private:
	Strip(Camera* camera);

	Camera* m_camera;
	float m_value;
	Vec4f m_p1, m_p2;
	Vec4f m_color;

	static const float MAX_VALUE;
};

inline Strip* Strip::create(Camera* camera) {
	Strip* ret = new Strip(camera);
	ret->AddRef();
	return ret;
}