#include "basescene.h"

BaseScene::BaseScene() :
	m_camera(NULL),
	m_ctrl(NULL)
{
}

//按键按下的事件回调
void BaseScene::OnKeyDown(_In_ UINT key) 
{
	m_ctrl->OnKeyDown(key); 
}

//按键松开的事件回调
void BaseScene::OnKeyUp(_In_ UINT key) 
{ 
	m_ctrl->OnKeyUp(key); 
}

//鼠标按下的事件回调
void BaseScene::OnMouseDown(_In_ bool isLeft, _In_ int x, _In_ int y) 
{ 
	m_ctrl->OnMouseDown(isLeft, x, y); 
}

//鼠标松开的事件回调
void BaseScene::OnMouseUp(_In_ bool isLeft, _In_ int x, _In_ int y) 
{ 
	m_ctrl->OnMouseUp(isLeft, x, y); 
}