#include "RootJob.h"
#include "SceneManager.h"

RootJob::RootJob()
{
}

RootJob::RootJob(GameObject* parent):GameObject(parent, "RootJob")
{
	/*pParent_ = parent;
	objectName_ = name;*/
}

RootJob::~RootJob()
{
}

void RootJob::Initialize()
{

	Instantiate<SceneManager>(this);

}

void RootJob::Update()
{
}

void RootJob::Draw()
{
	/*for (auto& e : childList_) {
		e->Draw();
	}*/
}

void RootJob::Release()
{
}
