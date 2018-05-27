#ifndef _LIGHTCONE_H
#define _LIGHTCONE_H

#include "object.h"
#include "model.h"
#include "craft.h"

class LightCone : public Object{
private:
	//飞船
	Craft* craft;

	//模组
	Model model;

	LightCone(_In_ Craft* craft);

public:
	~LightCone();

	//初始化
	void Initialize(_In_ char* modelFile, _In_ char* texFile);

	//前进一帧
	void Frame(_In_ float interval);

	//绘制
	void Render();

	static LightCone* create(_In_ Craft* craft);
};

#endif