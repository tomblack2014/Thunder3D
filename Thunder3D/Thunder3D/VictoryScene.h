#pragma once

#include"basescene.h"
#include "controller.h"
#include "Comment.h"

class VictoryScene : public BaseScene
{
public:
	VictoryScene(Comment* comment);

	~VictoryScene();

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

class VictorySceneCtrl : public Controller
{
public:
	VictorySceneCtrl(_In_ App* app, _In_ VictoryScene* scene);

	void Frame(_In_ float interval);

private:
	VictoryScene* m_scene;
};
