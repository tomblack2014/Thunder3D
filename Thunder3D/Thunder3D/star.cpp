#include "star.h"

#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

bool Star::Initialize(const char*  txtFile, const char* bmpFile)
{
	//配置恒星光源
	const float dif1[] = {2.0, 2.0, 2.0, 1.0};
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);

	return _model.Initialize(txtFile, bmpFile);
}

void Star::Frame(_In_ float interval)
{
	_rotation += _rotSpeed * interval;
}

void Star::Render()
{
	glPushMatrix();

	//设置光照
	glLightfv(GL_LIGHT1, GL_POSITION, pos.v);

	//调整姿态
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(_rotation, 0.0f, 1.0f, 0.0f);
	glScalef(_radius, _radius, _radius);

	//进行绘制
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glFrontFace(GL_CW);
	_model.Render();
	glFrontFace(GL_CCW);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glPopMatrix();
}

Star::Star(Vec4f pos, float speed, float radius) :
_rotSpeed(speed),
_rotation(0.f),
_radius(radius)
{
	this->pos = pos;
}

Star::~Star()
{
}

bool Star::Collide(_In_ const Vec4f& point){
	if(point.w < 0.001f){
		return false;
	}
	return pos.dist(point) < _radius * 1.02;
}

void* Star::operator[](_In_opt_ const std::string& name){
	if(name == "radius"){
		return &_radius;
	}else{
		return Object::operator[](name);
	}
}