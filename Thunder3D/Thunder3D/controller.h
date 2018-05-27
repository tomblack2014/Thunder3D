#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "common.h"
#include "camera.h"
#include "craft.h"

class App;

class Controller{
protected:
	//按键的状态
	enum{
		KEY_PRESS,
		KEY_RELEASE,
		KEY_DOWN,
		KEY_UP
	} keys[256], lbutton, rbutton;

	//鼠标的位置（触发时限定）
	int mouseX, mouseY;

	//程序对象的指针
	App* app;

	Controller(_In_ App* app);
public:
	//按键按下的事件回调
	void OnKeyDown(_In_ UINT key);

	//按键松开的事件回调
	void OnKeyUp(_In_ UINT key);

	//鼠标按下的事件回调
	void OnMouseDown(_In_ bool isLeft, _In_ int x, _In_ int y);

	//鼠标松开的事件回调
	void OnMouseUp(_In_ bool isLeft, _In_ int x, _In_ int y);

	//转化鼠标坐标到命中点
	Vec4f GetHitPoint();

	//更新鼠标坐标
	//不被使用的方法
	void UpdateMousePos();

	//前进一帧
	virtual void Frame(_In_ float interval);

	//虚拟析构函数
	virtual ~Controller(){};

	//复制按键状态
	void CopyKeyState(_In_ Controller* src);
};

//////////////////////////////////////////////////////////////////////////

class CameraController : public Controller{
protected:
	//控制的自由摄像机
	FreeCamera* camera;

	//平移速度
	static const float TRANS_SPEED;

	//旋转速度
	static const float ROTATE_SPEED;

public:
	CameraController(_In_ FreeCamera* camera, _In_ App* app);
	~CameraController();

	void Frame(_In_ float interval);
};

//////////////////////////////////////////////////////////////////////////

class TraceController : public Controller{
public:
	TraceController(_In_ App* app);

	void Frame(_In_ float interval);
};

//////////////////////////////////////////////////////////////////////////

class CraftController : public Controller{
private:
	//飞船
	Craft* craft;

	//平移速度
	static const float TRANS_SPEED;

	//旋转速度
	static const float ROTATE_SPEED;

public:
	CraftController(_In_ App* app, _In_ Craft* craft);

	~CraftController();

	void Frame(_In_ float interval);
};

#endif
