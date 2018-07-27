#ifndef _CRAFT_H
#define _CRAFT_H

#include "object.h"
#include "materialModel.h"
#include "model.h"

class Craft : public Object
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
	static Craft* create(_In_ const Vec4f& pos, _In_ const Vec4f& velocity, _In_ const Vec4f& up);

	/**
	*  \brief	初始化（模型纹理）
	*
	*  \param  txtFile	模型文件名
	*
	*  \return 初始化是否成功
	*/
	//注：保证模型的朝向是z轴正向为前，y轴正向为上
	bool Initialize(_In_ const char*  txtFile);

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
	*  \brief  取得飞船的速度向量(w=0)
	*/
	Vec4f GetSpeed();

	/**
	*  \brief  取得飞船的上方向向量(w=0)
	*/
	Vec4f GetUp();

	void SetPos(Vec4f pos);


	/**
	*  \brief  沿屏幕左右旋转
	*
	*  \param  ang  角度
	*/
	void RotateLR(_In_ float ang);

	/**
	*  \brief  沿屏幕上下旋转
	*
	*  \param  ang  角度
	*/
	void RotateUD(_In_ float ang);

	/**
	*  \brief  沿某个轴旋转飞船
	*
	*  \param  axis  旋转轴
	*
	*  \param  ang  角度
	*/
	void Rotate(Vec4f axis, float ang);

	/**
	*  \brief  前后调整速度
	*
	*  \param  speed  需要加减的速度，正加负减
	*/
	void Accelerate(_In_ float delta_speed);

	/**
	*  \brief  碰撞检测
	*
	*  \param  point 目标点
	*/
	//判断point是否在飞船的表面
	//用半径什么的卡一下就行，不要求很精确
	bool Collide(_In_ const Vec4f& point);

protected:
	Craft(const Vec4f& pos, const Vec4f& velocity, const Vec4f& up);

	//以下请自行实现其他内部需要的方法和成员
	Vec4f _velocity;		/*运行速度（矢量）*/
	Vec4f _up;				/*上方向向量*/
	MaterialModel _model;			/*模型纹理管理*/

	static const float _LOWEST_SPEED;	/*飞船运行的最低速度（防止减速导致的突然180度转向）*/
	static const float _HIGHEST_SPEED;	/*飞船运行的最高速度（和最低速度对称）*/
};

inline Craft* Craft::create(_In_ const Vec4f& pos, _In_ const Vec4f& velocity, _In_ const Vec4f& up){
	Craft* ret = new Craft(pos, velocity, up);
	ret->AddRef();
	return ret;
}

#endif