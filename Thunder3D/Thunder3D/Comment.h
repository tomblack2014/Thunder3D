#pragma once

#include "object.h"
#include "app.h"

#include <gl\gl.h>		// 包含最新的gl.h,glu.h库
#include <gl\glu.h>		// 包含OpenGL实用库

#include <string>
#include <vector>

class Comment : public Object
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
	static Comment* create(App* app);

	~Comment();

	/**
	*  \brief	初始化
	*
	*  \return 初始化是否成功
	*/
	bool Initialize();

	/**
	*  \brief  渲染
	*/
	void Render();

	void Set(
		std::vector<std::string> contexts,
		std::vector<Vec4f> colors,
		std::vector<Vec4f> pos,
		std::vector<float> scales = std::vector<float>());

private:
	Comment(App* app);

	GLuint	m_base;			// 绘制字体的显示列表的开始位置
	App* m_app;
	std::vector<std::string> m_contexts;
	std::vector<Vec4f> m_colors;
	std::vector<Vec4f> m_pos;
	std::vector<float> m_scales;
};


inline Comment* Comment::create(App* app) {
	Comment* ret = new Comment(app);
	ret->AddRef();

	return ret;
}