#pragma once

#include"basescene.h"
#include "controller.h"
#include "Comment.h"


class OpenScene : public BaseScene
{
public:
	OpenScene(Comment* comment);

	~OpenScene();

	bool Init(App* app) override;

	T3D::SCENE Update(float interval) override;

	void Reset() override;

	void end();

	bool AnimateFinished();

private:
	Craft* m_craft;
	Comment* m_comment;
private:
	bool m_endFlag;
	int m_status;
	int m_frames;
};

class OpenSceneCtrl : public Controller
{
public:
	OpenSceneCtrl(_In_ App* app, _In_ OpenScene* scene);

	void Frame(_In_ float interval);

private:
	OpenScene* m_scene;
};
