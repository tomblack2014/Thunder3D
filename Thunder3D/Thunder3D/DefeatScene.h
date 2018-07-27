#pragma once

#include"basescene.h"
#include "controller.h"
#include "Comment.h"

class DefeatScene : public BaseScene
{
public:
	DefeatScene(Comment* comment);

	~DefeatScene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	void end();

private:
	Comment* m_comment;
private:
	bool m_endFlag;

	float m_switchCDTime;
};

class DefeatSceneCtrl : public Controller
{
public:
	DefeatSceneCtrl(_In_ App* app, _In_ DefeatScene* scene);

	void Frame(_In_ float interval);

private:
	DefeatScene* m_scene;
};
