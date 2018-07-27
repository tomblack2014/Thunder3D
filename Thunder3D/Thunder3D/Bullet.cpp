#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include "Bullet.h"

Bullet::Bullet(Vec4f pos, Vec4f velocity, float lifeTime, float size) :
	m_v(velocity),
	m_lifeTime(lifeTime),
	m_rotSpeed(0.f),
	m_size(size)
{
	this->pos = pos;
}

bool Bullet::Initialize(const char*  txtFile, const char* bmpFile)
{
	return m_model.Initialize(txtFile, bmpFile);
}

void Bullet::AddRotation(Vec4f axis, float rotSpeed)
{
	m_rotAxis = axis;
	m_rotSpeed = rotSpeed;
	m_rotAng = 0.f;
}

void Bullet::Frame(_In_ float interval)
{
	pos = pos + m_v * interval;
	if (pos.z < 30.f)
		pos.z = pos.z;
	m_lifeTime -= interval;

	if (fabs(m_rotSpeed - 0.f) > 0.001f) {
		m_rotAng += m_rotSpeed * interval;
		if (m_rotAng > 360.f)
			m_rotAng -= 360.f;
	}
}

void Bullet::Render()
{
	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(m_size, m_size, m_size);
	if (fabs(m_rotSpeed - 0.f) > 0.001f)
		glRotatef(m_rotAng, m_rotAxis.x, m_rotAxis.y, m_rotAxis.z);

	m_model.Render();

	glPopMatrix();
}

float Bullet::GetLifeTime()
{
	return m_lifeTime;
}
