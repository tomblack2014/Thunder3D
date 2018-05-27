#pragma once

#include "object.h"

class CubeMap : public Object
{
public:
	/**
	*  \brief  创建对象方法
	*
	*  \param  size 天空盒尺寸 
	*/
	static CubeMap* create(float size);

	/**
	*  \brief  析构函数
	*/
	~CubeMap();

	/**
	*  \brief  渲染
	*/
	void Render();

	/**
	*  \brief	初始化（粒子纹理）
	*
	*  \param  bmpFile 纹理文件名
	*
	*  \return 初始化是否成功
	*/
	bool Initialize(_In_ const char*  bmpFile);

private:
	CubeMap(float size);

	float _size;		/*天空盒尺寸（正方体边长的一半）*/
	unsigned int _texture;	/*纹理*/
};

inline CubeMap* CubeMap::create(float size){
	CubeMap* ret = new CubeMap(size);
	ret->AddRef();
	return ret;
}