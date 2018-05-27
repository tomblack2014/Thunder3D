#include "SurroundBox.h"
#include <gl\GL.h>

SurroundBox::SurroundBox(_In_opt_ IObject* obj, _In_ ICamera* camera) :
obj(obj),
camera(camera)
{
	if(obj){
		obj->AddRef();
	}
	camera->AddRef();
}

SurroundBox::~SurroundBox()
{
	SafeRelease(&obj);
	SafeRelease(&camera);
}

void SurroundBox::Render()
{
	if(!obj)
		return ;

	float* radius = reinterpret_cast<float*>((*obj)["radius"]);
	Vec4f* newpos = reinterpret_cast<Vec4f*>((*obj)["pos"]);
	pos = *newpos;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	camera->MakeBillBoard();
	glScalef(*radius, *radius, *radius);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(- 1.2f, 1.2f, 0.0f);
	glVertex3f(1.2f, 1.2f, 0.0f);
	glVertex3f(1.2f, 1.2f, 0.0f);
	glVertex3f(1.2f, - 1.2f, 0.0f);
	glVertex3f(1.2f, - 1.2f, 0.0f);
	glVertex3f(- 1.2f, - 1.2f, 0.0f);
	glVertex3f(- 1.2f, - 1.2f, 0.0f);
	glVertex3f(- 1.2f, 1.2f, 0.0f);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void SurroundBox::UpdateObject(_In_opt_ IObject* target){
	SafeRelease(&obj);
	obj = target;
	if(obj)
		obj->AddRef();
}

void SurroundBox::UpdateCamera(_In_opt_ ICamera* newcamera){
	SafeRelease(&camera);
	camera = newcamera;
	camera->AddRef();
}
