#include "controller.h"
#include "app.h"
#include <gl\GL.h>
#include <gl\GLu.h>

Controller::Controller(_In_ App* app) :
app(app)
{
	for(int i = 0; i < 256; i ++)
		keys[i] = KEY_UP;
	lbutton = KEY_UP;
	rbutton = KEY_UP;
}

void Controller::OnKeyDown(_In_ UINT key){
	//逻辑控制
	if(keys[key] == KEY_UP){
		keys[key] = KEY_PRESS;
	}
	//防抖控制
	if(keys[key] == KEY_RELEASE){
		keys[key] = KEY_DOWN;
	}
}

void Controller::OnKeyUp(_In_ UINT key){
	//逻辑控制
	if(keys[key] == KEY_DOWN){
		keys[key] = KEY_RELEASE;
	}
	//防抖控制
	if(keys[key] == KEY_PRESS){
		keys[key] = KEY_UP;
	}
}

void Controller::Frame(_In_ float /*interval*/){
	for(int i = 0; i < 256; i ++){
		if(keys[i] == KEY_PRESS){
			keys[i] = KEY_DOWN;
		}
		if(keys[i] == KEY_RELEASE){
			keys[i] = KEY_UP;
		}
	}

	if(lbutton == KEY_PRESS){
		lbutton = KEY_DOWN;
	}
	if(lbutton == KEY_RELEASE){
		lbutton = KEY_UP;
	}
	if(rbutton == KEY_PRESS){
		rbutton = KEY_DOWN;
	}
	if(rbutton == KEY_RELEASE){
		rbutton = KEY_UP;
	}
}

void Controller::CopyKeyState(_In_ Controller* src)
{
	for(int i = 0; i < 256; i ++){
		if(src->keys[i] == KEY_PRESS || src->keys[i] == KEY_DOWN){
			keys[i] = KEY_DOWN;
		}else{
			keys[i] = KEY_UP;
		}
	}
	if(src->lbutton == KEY_PRESS || src->lbutton == KEY_DOWN){
		lbutton = KEY_DOWN;
	} else{
		lbutton = KEY_UP;
	}
	if(src->rbutton == KEY_PRESS || src->rbutton == KEY_DOWN){
		rbutton = KEY_DOWN;
	} else{
		rbutton = KEY_UP;
	}
}

void Controller::OnMouseDown(_In_ bool isLeft, _In_ int x, _In_ int y){
	if(isLeft){
		if(lbutton == KEY_UP){
			lbutton = KEY_PRESS;
		}
	} else{
		if(rbutton == KEY_UP){
			rbutton = KEY_PRESS;
		}
	}
	mouseX = x;
	mouseY = y;
}

void Controller::OnMouseUp(_In_ bool isLeft, _In_ int x, _In_ int y){
	if(isLeft){
		if(lbutton == KEY_DOWN){
			lbutton = KEY_RELEASE;
		}
	} else{
		if(rbutton == KEY_DOWN){
			rbutton = KEY_RELEASE;
		}
	}
	mouseX = x;
	mouseY = y;
}

Vec4f Controller::GetHitPoint(){
	int x = mouseX, y = mouseY;
	float val;
	double modelview[16], project[16], pos[3];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, project);
	glGetIntegerv(GL_VIEWPORT, viewport);
	y = viewport[3] - y;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &val);
	gluUnProject(x, y, val, modelview, project, viewport, &pos[0], &pos[1], &pos[2]);
	if(val >= 1.0f){
		return Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
	} else{
		return Vec4f((float)pos[0], (float)pos[1], (float)pos[2], 1.0f);
	}
}

void Controller::UpdateMousePos(){
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(app->GetHwnd(), &point);
	mouseX = point.x;
	mouseY = point.y;
}

//////////////////////////////////////////////////////////////////////////

const float CameraController::TRANS_SPEED = 3.0f;
const float CameraController::ROTATE_SPEED = 1.0f;

CameraController::CameraController(_In_ FreeCamera* camera, _In_ App* app) :
Controller(app),
camera(camera)
{
	camera->AddRef();
}

CameraController::~CameraController(){
	SafeRelease(&camera);
}

void CameraController::Frame(_In_ float interval){
	float tspd = TRANS_SPEED * interval;
	float rspd = ROTATE_SPEED * interval;

	//处理平移
	if(keys['W'] == KEY_PRESS || keys['W'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(0, tspd, 0.0f));
	}
	if(keys['S'] == KEY_PRESS || keys['S'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(0, - tspd, 0.0f));
	}
	if(keys['A'] == KEY_PRESS || keys['A'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(tspd, 0, 0.0f));
	}
	if(keys['D'] == KEY_PRESS || keys['D'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(- tspd, 0, 0.0f));
	}
	if(keys['Q'] == KEY_PRESS || keys['Q'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(0, 0, - tspd));
	}
	if(keys['E'] == KEY_PRESS || keys['E'] == KEY_DOWN){
		camera->MoveOnScreen(Vec4f(0, 0, tspd));
	}

	//处理旋转
	if(keys[VK_UP] == KEY_PRESS || keys[VK_UP] == KEY_DOWN){
		camera->RotateUD(- rspd);
	}
	if(keys[VK_DOWN] == KEY_PRESS || keys[VK_DOWN] == KEY_DOWN){
		camera->RotateUD(rspd);
	}
	if(keys[VK_LEFT] == KEY_PRESS || keys[VK_LEFT] == KEY_DOWN){
		camera->RotateLR(- rspd);
	}
	if(keys[VK_RIGHT] == KEY_PRESS || keys[VK_RIGHT] == KEY_DOWN){
		camera->RotateLR(rspd);
	}

	//插值转移
	if(!camera->InSlerp()){
		if(keys[VK_F1] == KEY_PRESS){
			camera->RecordPos();
		}
		if(keys[VK_F2] == KEY_PRESS){
			camera->StartSlerp();
		}
	}

	//恢复视角
	if(keys[VK_SPACE] == KEY_PRESS){
		app->UseStandardView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//飞船控制视角
	if(keys[VK_F3] == KEY_PRESS){
		app->UseCraftView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//处理点击事件（框选和跟踪）
	if(lbutton == KEY_PRESS){
		app->OnSelect(GetHitPoint());
	}
	if(rbutton == KEY_PRESS){
		app->OnTrace(GetHitPoint());
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//更新按键状态
	Controller::Frame(interval);
}

//////////////////////////////////////////////////////////////////////////

TraceController::TraceController(_In_ App* app):
Controller(app)
{
}

void TraceController::Frame(_In_ float interval){
	//恢复视角
	if(keys[VK_SPACE] == KEY_PRESS){
		app->UseStandardView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//飞船控制视角
	if(keys[VK_F3] == KEY_PRESS){
		app->UseCraftView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//处理点击事件（框选和跟踪）
	if(lbutton == KEY_PRESS){
		app->OnSelect(GetHitPoint());
	}
	if(rbutton == KEY_PRESS){
		app->OnTrace(GetHitPoint());
		return ; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//更新按键状态
	Controller::Frame(interval);
}

//////////////////////////////////////////////////////////////////////////

const float CraftController::TRANS_SPEED = 0.3f;
const float CraftController::ROTATE_SPEED = 1.0f;

CraftController::CraftController(_In_ App* app, _In_ Craft* craft):
Controller(app),
craft(craft)
{
	craft->AddRef();
}

void CraftController::Frame(_In_ float interval){
	float tspd = TRANS_SPEED * interval;
	float rspd = ROTATE_SPEED * interval;

	//处理方向键控制事件
	if(keys[VK_UP] == KEY_PRESS || keys[VK_UP] == KEY_DOWN){
		craft->RotateUD(- rspd);
	}
	if(keys[VK_DOWN] == KEY_PRESS || keys[VK_DOWN] == KEY_DOWN){
		craft->RotateUD(rspd);
	}
	if(keys[VK_LEFT] == KEY_PRESS || keys[VK_LEFT] == KEY_DOWN){
		craft->RotateLR(- rspd);
	}
	if(keys[VK_RIGHT] == KEY_PRESS || keys[VK_RIGHT] == KEY_DOWN){
		craft->RotateLR(rspd);
	}
	if(keys[VK_PRIOR] == KEY_PRESS || keys[VK_PRIOR] == KEY_DOWN){
		craft->Accelerate(tspd);
	}
	if(keys[VK_NEXT] == KEY_PRESS || keys[VK_NEXT] == KEY_DOWN){
		craft->Accelerate(- tspd);
	}

	//恢复视角
	if(keys[VK_SPACE] == KEY_PRESS){
		app->UseStandardView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//飞船控制视角
	if(keys[VK_F3] == KEY_PRESS){
		app->UseCraftView();
		return; //这里因为自身已经被释放了，所以必须立刻返回
	}

	//更新按键状态
	Controller::Frame(interval);
}

CraftController::~CraftController(){
	SafeRelease(&craft);
}