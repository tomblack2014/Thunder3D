#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "object.h"
#include "craft.h"

interface ICamera : public IObject
{
public:
	static const GUID IID_ICamera;

	/**
	* 将摄像机矩阵载入OpenGL
	*/
	virtual void LoadMatrix() PURE;

	/**
	* 计算Billboard矩阵，消除相机的旋转
	*/
	virtual void MakeBillBoard() PURE;
};

class Camera : public ICamera
{
protected:
	//引用计数
	ULONG refcnt;

	//所在位置
	Vec4f pos;

	Camera(_In_ const Vec4f& eye);

	virtual ~Camera(){};
public:
	ULONG STDMETHODCALLTYPE AddRef();

	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE QueryInterface(_In_ const IID&id, _Outptr_ void**ppv);

	Vec4f GetPos();

	void Frame(_In_ float interval);

	void Render();

	void* operator[](_In_opt_ const std::string& name);
};

//////////////////////////////////////////////////////////////////////////

//自由移动摄像机
class FreeCamera : public Camera
{
protected:
	//时间常数
	static const float SLERP_TIME;

	//视线注视点
	Vec4f at;

	//上方向矢量
	Vec4f up;

	//普通观察矩阵
	Mat4f lookMat;

	//摄像机运动矩阵
	Mat4f motionMat;

	//运动矩阵历史记录
	Mat4f motion0, motion1;

	//是否处于插值状态
	bool inSlerp;

	//插值时间
	float tim;

	FreeCamera(_In_ const Vec4f& eye, _In_ const Vec4f& at, _In_ const Vec4f& up);

public:
	/**
	* 摄像机沿屏幕坐标移动
	*/
	void MoveOnScreen(_In_ const Vec4f& trans);

	/**
	* 水平转动摄像头（屏幕坐标系）
	*/
	void RotateLR(_In_ float ang);

	/**
	* 垂直转动摄像头（屏幕坐标系）
	*/
	void RotateUD(_In_ float ang);

	/**
	* 设置一个历史位置
	*/
	void RecordPos();

	/**
	* 进行插值移动
	*/
	void StartSlerp();

	void Frame(_In_ float interval);

	void LoadMatrix();

	void MakeBillBoard();

	static FreeCamera* create(_In_ const Vec4f& eye, _In_ const Vec4f& at, _In_ const Vec4f& up);

	/**
	* 是否处于插值移动中
	*/
	bool InSlerp() const;
};

inline bool FreeCamera::InSlerp() const{
	return inSlerp;
}

//////////////////////////////////////////////////////////////////////////

//追踪对象摄像机
class TraceCamera : public Camera{
protected:
	//被追踪的对象
	IObject* obj;

	//观察矩阵
	Mat4f lookMat;

	TraceCamera(_In_ IObject* obj);

	~TraceCamera();

public:
	void Frame(_In_ float interval);

	void LoadMatrix();

	void MakeBillBoard();

	static TraceCamera* create(_In_ IObject* obj);
};

//////////////////////////////////////////////////////////////////////////

//跟踪飞船摄像机
class CraftCamera : public Camera{
protected:
	//被追踪的飞船
	Craft* craft;

	//观察矩阵
	Mat4f lookMat;

	CraftCamera(_In_ Craft* craft);

	~CraftCamera();

public:
	void Frame(_In_ float interval);

	void LoadMatrix();

	void MakeBillBoard();

	static CraftCamera* create(_In_ Craft* craft);
};

#endif
