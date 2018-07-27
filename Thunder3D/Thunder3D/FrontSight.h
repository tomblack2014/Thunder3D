#pragma once

#include "object.h"
#include "camera.h"

class FrontSight : public Object
{
public:
	/**
	*  \brief  构造方法
	*/
	static FrontSight* create(FreeCamera* camera);

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

private:
	FrontSight(FreeCamera* camera);

	FreeCamera* m_camera;
};

inline FrontSight* FrontSight::create(FreeCamera* camera) {
	FrontSight* ret = new FrontSight(camera);
	ret->AddRef();
	return ret;
}