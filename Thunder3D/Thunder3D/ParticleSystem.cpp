#include "ParticleSystem.h"
#include <random>
#include <gl\GL.h>
using namespace std;

//临时措施：提供随机数生成源
static default_random_engine re(GetTickCount());
static const float PI = 3.1415926535f;
const float ParticleSystem::TIME_THRES = 0.005f;

void ParticleSystem::AddParticle(){
	float fade = uniform_real_distribution<float>(0.8f, 1.2f)(re);
	float dx = uniform_real_distribution<float>(-0.2f, 0.2f)(re);
	float dy = uniform_real_distribution<float>(-0.08f, 0.08f)(re) + 0.07f;
	Vec4f npos = pos + up * dy + nVec * dx;

	plist.push_back(
		Particle(npos, speed, - speed, fade)
		);
}

ParticleSystem::ParticleSystem(_In_ Craft* craft, _In_ ICamera* camera):
craft(craft),
camera(camera),
tim(0)
{
	craft->AddRef();
	camera->AddRef();

	pos = craft->GetPos();
	speed = craft->GetSpeed();
}

ParticleSystem::~ParticleSystem()
{
	SafeRelease(&craft);
	SafeRelease(&camera);
}

void ParticleSystem::Frame(_In_ float interval)
{
	//跟踪位置
	speed = craft->GetSpeed();
	pos = speed;
	pos.normal();
	up = craft->GetUp();
	up.normal();
	nVec = pos.cross(up);
	pos = craft->GetPos() - pos * 1.6f;

	//创建粒子
	tim += interval;
	while(tim > TIME_THRES){
		tim -= TIME_THRES;
		AddParticle();
	}

	//增加粒子
	for(list<Particle>::iterator it = plist.begin(); it != plist.end(); ){
		it->Frame(interval);
		if(it->life <= 0.0f){
			it = plist.erase(it);
		}else{
			++ it;
		}
	}
}

void ParticleSystem::Render(){
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texture);
	for(list<Particle>::iterator it = plist.begin(); it != plist.end(); ++ it){
		it->Render(camera);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
}

bool ParticleSystem::Initialize(_In_ char* filename)
{
	return Model::LoadGLTexture(filename, &texture);
}

void ParticleSystem::UpdateCamera(_In_ ICamera* newcamera)
{
	SafeRelease(&camera);
	camera = newcamera;
	camera->AddRef();
}

ParticleSystem* ParticleSystem::create(_In_ Craft* craft, _In_ ICamera* camera)
{
	ParticleSystem* ret = new ParticleSystem(craft, camera);
	ret->AddRef();
	return ret;
}
