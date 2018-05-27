#pragma once

#include "object.h"
#include <list>

class Trace : public Object
{
private:
	std::list<Vec4f> _posList;

	static UINT LIMIT;

	Trace(){};

public:
	void Frame(const Vec4f& pos);

	void Render();

	static Trace* create();
};

inline Trace* Trace::create(){
	Trace* ret = new Trace();
	ret->AddRef();
	return ret;
}
