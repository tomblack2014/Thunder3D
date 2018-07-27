#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include "Strip.h"
#include "mat4f.h"

const float PI = 3.141592654f;

const float Strip::MAX_VALUE = 100.f;

Strip::Strip(Camera* camera) :
	m_camera(camera),
	m_value(MAX_VALUE),
	m_p1(Vec4f(-10.f, 7.f, 0.f)),
	m_p2(Vec4f(-3.f, 6.3f, 0.f)),
	m_color(Vec4f(1.f, 0.f, 0.f, 1.f))
{

}

void Strip::Frame(_In_ float interval)
{

}

void Strip::Render()
{
	Vec4f pos = m_camera->GetPos();
	Vec4f eye2at = m_camera->GetEye2At();
	Vec4f up = m_camera->GetUp();
	Vec4f axis = -eye2at.cross(Vec4f(0, 0, -1));
	float ang = acosf(eye2at.dot(Vec4f(0, 0, -1)) / eye2at.length());
	pos = pos + eye2at * 0.2f;

	Mat4f mat = Mat4f::rotate(axis, ang);
	Vec4f tmpUp = mat.transpose() * up;
	float ang2 = acosf(tmpUp.dot(Vec4f(0.f, 1.f, 0.f)) / tmpUp.length());
	Vec4f forAxis = tmpUp.cross(Vec4f(0.f, 1.f, 0.f));
	forAxis.z /= fabs(forAxis.z);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(0.01, 0.01, 0.01);
	glRotatef(ang * 180 / PI, axis.x, axis.y, axis.z);
	glRotatef(ang2 * 180 / PI, 0, 0, -forAxis.z);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(m_color.x, m_color.y, m_color.z, m_color.w);

	glVertex3f(m_p1.x, m_p1.y, 0.0f);
	glVertex3f(m_p1.x + m_value / MAX_VALUE * (m_p2.x - m_p1.x), m_p1.y, 0.0f);
	glVertex3f(m_p1.x + m_value / MAX_VALUE * (m_p2.x - m_p1.x), m_p2.y, 0.0f);
	glVertex3f(m_p1.x, m_p2.y, 0.0f);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Strip::SetValue(float val)
{
	if (val > MAX_VALUE)
		m_value = MAX_VALUE;
	else if (val < 0.f)
		m_value = 0.f;
	else
		m_value = val;
}

void Strip::SetPos(Vec4f p1, Vec4f p2)
{
	m_p1 = p1;
	m_p2 = p2;
}

void Strip::SetColor(Vec4f color)
{
	m_color = color;
}