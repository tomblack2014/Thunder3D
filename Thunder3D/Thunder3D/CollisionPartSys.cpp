#include "CollisionPartSys.h"

#include <random>
#include <gl\GL.h>
using namespace std;

static default_random_engine re(GetTickCount());
static const float PI = 3.1415926535f;
const float CollisionPartSys::TIME_THRES = 0.005f;

void CollisionPartSys::AddParticle()
{
	float fade = uniform_real_distribution<float>(1 / m_lifeTime * 0.8, 1 / m_lifeTime * 1.2)(re);
	float vx = uniform_real_distribution<float>(-m_collisionR, m_collisionR)(re);
	float vy = uniform_real_distribution<float>(-m_collisionR, m_collisionR)(re);
	float vz = uniform_real_distribution<float>(-m_collisionR, m_collisionR)(re);
	Vec4f v = Vec4f(vx, vy, vz);

	plist.push_back(Particle(pos, v, -v*0.1f, fade));
}

CollisionPartSys::CollisionPartSys(_In_ ICamera* camera, Vec4f pos):
	camera(camera),
	tim(0),
	m_life(1.f),
	m_collisionR(20.5f),
	m_lifeTime(5.f)
{
	camera->AddRef();
	this->pos = pos;
}

CollisionPartSys::~CollisionPartSys()
{
	SafeRelease(&camera);
}

void CollisionPartSys::Frame(_In_ float interval)
{
	m_life -= interval;
	//创建粒子
	tim += interval;
	while (tim > TIME_THRES && m_life > 0.f) {
		tim -= TIME_THRES;
		AddParticle();
	}

	//刷新粒子
	for (list<Particle>::iterator it = plist.begin(); it != plist.end(); ) {
		it->Frame(interval);
		if (it->life <= 0.0f) {
			it = plist.erase(it);
		}
		else {
			++it;
		}
	}
}

void CollisionPartSys::Render()
{
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texture);
	for (auto it = plist.begin(); it != plist.end(); ++it) {
		it->Render(camera);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
}

bool CollisionPartSys::Initialize(_In_ char* filename)
{
	return Model::LoadGLTexture(filename, &texture);
}

void CollisionPartSys::SetPos(Vec4f pos)
{
	this->pos = pos;
}

void CollisionPartSys::SetCollisionR(float r)
{
	m_collisionR = r;
}

void CollisionPartSys::SetLifeTime(float time)
{
	m_lifeTime = time;
}


CollisionPartSys* CollisionPartSys::create(_In_ ICamera* camera, Vec4f pos)
{
	CollisionPartSys* ret = new CollisionPartSys(camera, pos);
	ret->AddRef();
	return ret;
}

bool CollisionPartSys::IsExist()
{
	/*if (plist.empty() && m_life < 0.f)
		return false;*/
	return true;
}