#pragma once

#include "object.h"
#include "model.h"
#include "camera.h"

class Star : public Object
{
public:
	/**
	*  \brief  构造方法
	*
	*  \param  pos 位置（世界坐标）
	*
	*  \param  speed 自转速度
	*
	*  \param  radius 半径
	*/
	static Star* create(Vec4f pos, float speed, float radius);

	/**
	*  \brief	初始化（模型纹理）
	*
	*  \param  txtFile	模型文件名
	*
	*  \param  bmpFile	bmp格式纹理文件名
	*
	*  \return 初始化是否成功
	*/
	bool Initialize(const char*  txtFile, const char* bmpFile);

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

	/**
	*  \brief  碰撞检测
	*
	*  \param  point 目标点
	*/
	bool Collide(_In_ const Vec4f& point);

	/**
	*  \brief  获得属性
	*  @override
	*/
	void* operator[](_In_opt_ const std::string& name);

private:
	Star(Vec4f pos, float speed, float radius);

	~Star();

	float _rotation;		/*当前旋转角度（自转）*/
	float _rotSpeed;		/*旋转速度（自转）*/

	float _radius; // 半径

	Model _model;			/*模型纹理管理*/
};


inline Star* Star::create(Vec4f pos, float speed, float radius){
	Star* ret = new Star(pos, speed, radius);
	ret->AddRef();
	return ret;
}