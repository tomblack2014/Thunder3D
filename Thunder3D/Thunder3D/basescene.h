#ifndef BASESCENE_H
#define BASESCENE_H

#include "global.h"
#include "camera.h"
#include "controller.h"

class APP;

class BaseScene
{
public:
	BaseScene();

    virtual bool Init(App* app) = 0;

    virtual T3D::SCENE Update(float interval) = 0;

    virtual void Reset() = 0;

	//按键按下的事件回调
	void OnKeyDown(_In_ UINT key);

	//按键松开的事件回调
	void OnKeyUp(_In_ UINT key);

	//鼠标按下的事件回调
	void OnMouseDown(_In_ bool isLeft, _In_ int x, _In_ int y);

	//鼠标松开的事件回调
	void OnMouseUp(_In_ bool isLeft, _In_ int x, _In_ int y);

protected:
	ICamera* m_camera;
	Controller* m_ctrl;
};

#endif // BASESCENE_H
