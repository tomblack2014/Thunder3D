#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "planet.h"

Planet::Planet(float radius, const Vec4f& nVec, float speedR1, float speedR2, IObject* parent, float size) :
_radius(radius),
_nVec(nVec),
_rotation(0.f),
_revolution(0.f),
_revSpeed(speedR1),
_rotSpeed(speedR2),
_parent(parent),
_trace(Trace::create()),
_size(size)
{
}

Planet::~Planet()
{
	SafeRelease(&_trace);
}

bool Planet::Initialize(const char*  txtFile, const char* bmpFile)
{
	return _model.Initialize(txtFile, bmpFile);
}


void Planet::Frame(_In_ float interval)
{
	_rotation += _rotSpeed * interval;
	_revolution += _revSpeed * interval;
	UpdatePos();
	_trace->Frame(pos);
}

void Planet::Render()
{
	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(_rotation, _nVec.x, _nVec.y, _nVec.z);
	glScalef(_size, _size, _size);
	_model.Render();

	glPopMatrix();

	glDisable(GL_LIGHTING);
	_trace->Render();
	glEnable(GL_LIGHTING);
}

void Planet::UpdatePos()
{
	const float EPS = 0.000001f;
	Vec4f tmpPos(_radius * cos(_rotation), _radius * sin(_rotation), 0.f);
	float phi = acosf(_nVec.dot(Vec4f(0.f, 0.f, 1.f)) / _nVec.length());
	if (fabs(phi) < EPS){
		pos = tmpPos + _parent->GetPos();
	}
	else{
		Vec4f tmpNVec = _nVec.cross(Vec4f(0.f, 0.f, 1.f));
		Mat4f rotateMat = Mat4f::rotate(tmpNVec, phi);
		pos = rotateMat * tmpPos + _parent->GetPos();
	}
}

bool Planet::Collide(_In_ const Vec4f& point){
	if(point.w < 0.001f){
		return false;
	}
	return pos.dist(point) < _size * 1.02;
}

void* Planet::operator[](_In_opt_ const std::string& name){
	if(name == "radius"){
		return &_size;
	} else{
		return Object::operator[](name);
	}
}