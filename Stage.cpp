#include "Stage.h"
#include "Engine/Model.h"

//コンストラクタ
Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage"),hModel_(-1)
{
}

//初期化
void Stage::Initialize()
{
	//モデルデータのロード
	hModel_ = Model::Load("assets/BoxDefault.fbx");
	assert(hModel_ >= 0);
}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

//開放
void Stage::Release()
{
}
