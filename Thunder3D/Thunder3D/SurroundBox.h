#ifndef _SURROUND_BOX
#define _SURROUND_BOX

#include "object.h"
#include "camera.h"

class SurroundBox : public Object{
private:
	IObject* obj;
	ICamera* camera;

	SurroundBox(_In_opt_ IObject* obj, _In_ ICamera* camera);

public:
	~SurroundBox();

	// 判断目标对象是否是当前选中的对象
	// target : 目标对象
	// return : 是否选中
	bool SameObject(_In_opt_ IObject* target);

	// 更新选中的对象
	// target : 目标对象
	void UpdateObject(_In_opt_ IObject* target);

	//更新摄像机
	// newcamera : 新摄像机
	void UpdateCamera(_In_opt_ ICamera* newcamera);

	// 进行绘制
	void Render();

	// 创建包围盒对象
	static SurroundBox* create(_In_opt_ IObject* obj, _In_ ICamera* camera);
};

inline bool SurroundBox::SameObject(_In_opt_ IObject* target){
	return Equal(obj, target);
}

inline SurroundBox* SurroundBox::create(_In_opt_ IObject* obj, _In_ ICamera* camera){
	SurroundBox* ret = new SurroundBox(obj, camera);
	ret->AddRef();
	return ret;
}

#endif