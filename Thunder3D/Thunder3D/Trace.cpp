#include "Trace.h"
#include "vec4f.h"

#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

UINT Trace::LIMIT = 400;

void Trace::Frame(const Vec4f& pos)
{
	_posList.push_back(pos);
	if (_posList.size() > LIMIT)
		_posList.pop_front();
}

void Trace::Render()
{

	int s = _posList.size();
	if (s < 2)
		return;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	
	glBegin(GL_LINES);
	int i = 0;
	std::list<Vec4f>::iterator prev = _posList.begin();
	std::list<Vec4f>::iterator next = prev;
	next++;
	for (; next != _posList.end();prev++, next++){
		glColor4f(1.f, 1.f, 1.f, 1.0f * i / s);
		glVertex3f(prev->x, prev->y, prev->z);
		glVertex3f(next->x, next->y, next->z);
		i++;
	}
	glEnd();

	glDepthMask(GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}