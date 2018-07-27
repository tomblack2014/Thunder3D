#include "Comment.h"

Comment::Comment(App* app):
	m_app(app)
{
}

Comment::~Comment()
{
	glDeleteLists(m_base, 96);					//删除96个显示列表
}

bool Comment::Initialize()
{
	HFONT	font;						// 字体句柄
	HFONT	oldfont;						// 旧的字体句柄

	m_base = glGenLists(96);
	font = CreateFont(-24,
		0,				// 字体宽度
		0,				// 字体的旋转角度 Angle Of Escapement
		0,				// 字体底线的旋转角度Orientation Angle
		FW_BOLD,				// 字体的重量
		FALSE,				// 是否使用斜体
		FALSE,				// 是否使用下划线
		FALSE,				// 是否使用删除线
		ANSI_CHARSET,			// 设置字符集
		OUT_TT_PRECIS,			// 输出精度
		CLIP_DEFAULT_PRECIS,		// 裁剪精度
		ANTIALIASED_QUALITY,		// 输出质量
		FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
		"Courier New");			// 字体名称
	oldfont = (HFONT)SelectObject(m_app->GetHDC(), font);			// 选择我们需要的字体
	wglUseFontBitmaps(m_app->GetHDC(), 32, 96, m_base);			// 创建96个显示列表，绘制从ASCII码为32-128的字符
	SelectObject(m_app->GetHDC(), oldfont);					// 选择原来的字体
	DeleteObject(font);					// 删除字体

	return true;
}

void Comment::Render()
{
	float tmp[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightfv(GL_LIGHT5, GL_AMBIENT, tmp);
	glEnable(GL_LIGHT5);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();							// 重置当前的模型观察矩阵
	glTranslatef(0.0f, 0.0f, -1.0f);				// 移入屏幕一个单位

	glPushAttrib(GL_LIST_BIT);					// 把显示列表属性压入属性堆栈
	glListBase(m_base - 32);					// 设置显示列表的基础值
	for (int i = 0; i < m_contexts.size(); i++) {
		glColor4f(m_colors[i].x, m_colors[i].y, m_colors[i].z, m_colors[i].w);
		// 设置光栅化位置，即字体的位置
		glRasterPos2f(m_pos[i].x, m_pos[i].y);
		glCallLists(m_contexts[i].size(), GL_UNSIGNED_BYTE, m_contexts[i].data());		// 调用显示列表绘制字符串
	}
	//glRasterPos2f(pos.x, pos.y + 0.1);
	//glCallLists(strlen("test"), GL_UNSIGNED_BYTE, "test");		// 调用显示列表绘制字符串
	glPopAttrib();						// 弹出属性堆栈

	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT5);
}

void Comment::Set(
	std::vector<std::string> contexts,
	std::vector<Vec4f> colors,
	std::vector<Vec4f> pos,
	std::vector<float> scales)
{
	if (m_contexts.size() != m_colors.size() || m_colors.size() != m_pos.size())
		return;
	if (scales.size() != 0 && m_scales.size() != m_contexts.size())
		return;

	m_contexts = contexts;
	m_colors = colors;
	m_pos = pos;

	if (scales.size() == 0) {
		m_scales.clear();
		for (int i = 0; i < m_contexts.size(); i++) {
			m_scales.push_back(1.f);
		}
	}
	else {
		m_scales = scales;
	}
}