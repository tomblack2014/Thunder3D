#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include "HurtAnimate.h"

const float PI = 3.141592654f;

HurtAnimate::HurtAnimate(Camera* camera):
	m_camera(camera),
	ANI_TIME(1.f),
	m_curAniTime(-0.1f)
{

}

void HurtAnimate::Frame(_In_ float interval)
{
	m_curAniTime -= interval;
}

void HurtAnimate::Render()
{
	if (m_curAniTime > 0.f) {
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
		glColor4f(1.0f, 0.0f, 0.0f, m_curAniTime / ANI_TIME * 0.7);

		glVertex3f(-15.f, 10.f, 0.0f);
		glVertex3f(15.f, 10.f, 0.0f);
		glVertex3f(15.f, -10.f, 0.0f);
		glVertex3f(-15.f, -10.f, 0.0f);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
}

void HurtAnimate::NewHurt()
{
	m_curAniTime = ANI_TIME;
}
