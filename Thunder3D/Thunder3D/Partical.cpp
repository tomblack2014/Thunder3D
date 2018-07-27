#include "Partical.h"

#include <random>
#include <gl\GL.h>
using namespace std;

static default_random_engine re(GetTickCount());
static const float PI = 3.1415926535f;

void Particle::DoWrap(_In_ const Vec4f& force, _In_ float turbul, _In_ float resist) {
	float theta = uniform_real_distribution<float>(0, PI)(re);
	float phi = uniform_real_distribution<float>(0, 2 * PI)(re);
	float v = uniform_real_distribution<float>(0, turbul)(re);
	Vec4f t(v * sin(theta) * cos(phi), v * sin(theta) * sin(phi), v * cos(theta));
	speed = speed * (1 - resist) + t + force;
}

void Particle::Frame(_In_ float interval) {
	//调整速度
	Vec4f f = force;
	f.normal();
	f = f * 0.3f;
	DoWrap(f, 0.6f, 0.15f);
	DoWrap(Vec4f(), 0.25f, 0.01f);

	//调整生命和位置
	life -= interval * fade;
	pos = pos + speed * interval * 0.1f;
}

Particle::Particle(_In_ const Vec4f& pos, _In_ const Vec4f& speed, _In_ const Vec4f& force, _In_ const float fade) :
	life(1.0f),
	pos(pos),
	speed(speed),
	force(force),
	fade(fade)
{
}

void Particle::Render(_In_ ICamera* camera) {
	float r, g, b;
	float rnd = 1.0f - life;
	//计算颜色
	if (rnd < 0.5f) {
		r = 0.92f * (1.0f - 2.0f * rnd) + 2.0f * rnd;
		g = 0.92f * (1.0f - 2.0f * rnd);
		b = 0;
	}
	else {
		r = 2.0f - 2.0f * rnd;
		g = 0;
		b = 0;
	}

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(0.3f, 0.3f, 0.3f);

	camera->MakeBillBoard();
	glBegin(GL_QUADS);
	glColor4f(r, g, b, life);
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-1.f, 1.f, 0.0f);
	glTexCoord2f(1.f, 0.f);
	glVertex3f(1.f, 1.f, 0.0f);
	glTexCoord2f(1.f, 1.f);
	glVertex3f(1.f, -1.f, 0.0f);
	glTexCoord2f(0.f, 1.f);
	glVertex3f(-1.f, -1.f, 0.0f);
	glEnd();
	glPopMatrix();
}