#include "LightCone.h"
#include "gl/GL.h"

static const float PI = 3.1415926f;
static const float EPS = 1e-4f;

LightCone::LightCone(_In_ Craft* craft):
craft(craft)
{
	craft->AddRef();
}

LightCone::~LightCone(){
	SafeRelease(&craft);
}

void LightCone::Initialize(_In_ char* modelFile, _In_ char* texFile){
	model.Initialize(modelFile, texFile);

	//配置探照灯光源
	const float dif2[] = {2.0, 2.0, 2.0, 1.0};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, dif2);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 120.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 200.0);
}

void LightCone::Frame(_In_ float /*interval*/)
{
	//计算位置
	Vec4f speed = craft->GetSpeed();
	speed.normal();
	pos = craft->GetPos() + speed * 2.0f;
}

void LightCone::Render()
{
	glPushMatrix();

	//取得速度矢量
	Vec4f oriVec(0.f, 0.f, 1.f);
	Vec4f speed = craft->GetSpeed();
	speed.normal();

	//控制灯光位置
	glLightfv(GL_LIGHT2, GL_POSITION, pos.v);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, speed.v);

	glTranslatef(pos.x, pos.y, pos.z);

	float phi = acosf(speed.dot(oriVec));
	if(fabs(phi) > EPS){
		Vec4f nVec = - speed.cross(oriVec);
		glRotatef(phi * 180 / PI, nVec.x, nVec.y, nVec.z);
	}
	glScalef(0.03f, 0.03f, 0.03f);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	model.Render();
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

LightCone* LightCone::create(_In_ Craft* craft){
	LightCone* ret = new LightCone(craft);
	ret->AddRef();
	return ret;
}
