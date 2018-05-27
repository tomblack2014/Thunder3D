#pragma once

#include "object.h"
#include "model.h"
#include "Trace.h"

class Planet : public Object
{
public:
	/**
	*  \brief	构造函数
	*
	*  \param  radius 轨道半径
	*
	*  \param  nVec	旋转轨道法向量
	*
	*  \param  speedR1  公转角速度
	*
	*  \param  speedR2  自转角速度
	*
	*  \param  parent 父节点（圆周运动中心）
	*
	*  \param  size 自身半径
	*/
	static Planet* create(float radius, const Vec4f& nVec, float speedR1, float speedR2, IObject* parent, float size);

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
	void UpdatePos();
	
	float _radius;			/*旋转轨道半径*/
	Vec4f _nVec;			/*旋转法向量*/	
	float _rotation;		/*当前旋转角度（自转）*/
	float _revolution;		/*当前旋转角度（公转）*/
	float _rotSpeed;		/*旋转速度*/
	float _revSpeed;		/*旋转速度*/
	
	IObject *_parent;		/*父节点（圆周运动中心）*/

	Model _model;			/*模型纹理管理*/
	Trace* _trace;			/*轨迹管理*/

	float _size; //自身半径

	Planet(float radius, const Vec4f& nVec, float speedR1, float speedR2, IObject* parent, float size);

	~Planet();
};

inline Planet* Planet::create(float radius, const Vec4f& nVec, float speedR1, float speedR2, IObject* parent, float size){
	Planet* ret = new Planet(radius, nVec, speedR1, speedR2, parent, size);
	ret->AddRef();
	return ret;
}
