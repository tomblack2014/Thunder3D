#ifndef _OBJECT_H
#define _OBJECT_H

#include "common.h"
#include "mat4f.h"
#include <string>
#include <Unknwn.h>

/**
* 一般对象接口类
*/
interface IObject : public IUnknown
{
public:
	static const GUID IID_IObject;

	/**
	* 获得物体所在位置
	*/
	virtual Vec4f GetPos() PURE;

	/**
	* 前进一帧
	* interval : 前进的时间（秒）
	*/
	virtual void Frame(_In_ float interval) PURE;

	/**
	* 渲染物体
	*/
	virtual void Render() PURE;

	/**
	* 按名字获得某个附加属性
	* 用于破坏不合理的封装
	* name : 属性的名字
	*/
	virtual void* operator[](_In_opt_ const std::string& name) PURE;
};

class Object : public IObject
{
protected:
	//引用计数
	ULONG refcnt;

	//所在位置
	Vec4f pos;

public:
	Object();

	virtual ~Object();

	ULONG STDMETHODCALLTYPE AddRef() ;

	ULONG STDMETHODCALLTYPE Release() ;

	HRESULT STDMETHODCALLTYPE QueryInterface(_In_ const IID&id, _Outptr_ void**ppv) ;

	Vec4f GetPos() ;

	void Frame(_In_ float interval) ;

	void Render() ;

	void* operator[](_In_opt_ const std::string& name);
};

#endif
