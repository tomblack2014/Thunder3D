#pragma once

#include "object.h"
#include "model.h"

class Bullet : public Object
{
public:
	/**
	*  \brief  构造方法
	*
	*  \param  pos 位置（世界坐标）
	*
	*  \param  velocity 速度（矢量）
	*
	*  \param  up 上方向向量
	*/
	static Bullet* create(
		_In_ const Vec4f& pos, 
		_In_ const Vec4f& velocity, 
		_In_ const float lifeTime,
		_In_ const float size);

	/**
	*  \brief	初始化（模型纹理）
	*
	*  \param  txtFile	模型文件名
	*
	*  \return 初始化是否成功
	*/
	//注：保证模型的朝向是z轴正向为前，y轴正向为上
	bool Initialize(const char*  txtFile, const char* bmpFile);

	/**
	*  \brief	为投射物添加运动时的旋转
	*
	*  \param  axis	旋转轴
	*
	*  \param speed 旋转速度
	*/
	void AddRotation(Vec4f axis, float rotSpeed);

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
	*  \brief  获取剩余生命周期
	*
	*  \return 生命周期
	*/
	float GetLifeTime();

private:
	Bullet(Vec4f pos, Vec4f velocity, float lifeTime, float size);

	Vec4f m_v;
	Model m_model;			/*模型纹理管理*/
	float m_lifeTime;
	Vec4f m_rotAxis;
	float m_rotSpeed;
	float m_rotAng;
	float m_size;
};

inline Bullet* Bullet::create(
	_In_ const Vec4f& pos, 
	_In_ const Vec4f& velocity, 
	_In_ const float lifeTime,
	_In_ const float size) {
	Bullet* ret = new Bullet(pos, velocity, lifeTime, size);
	ret->AddRef();
	return ret;
}