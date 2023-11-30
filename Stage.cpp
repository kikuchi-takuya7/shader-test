#include "Stage.h"
#include "Engine/Model.h"

//コンストラクタ
Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage"), hModel_{-1,-1}
{
}

//初期化
void Stage::Initialize()
{
	//モデルデータのロード
	hModel_[ARROW] = Model::Load("assets/Arrow.fbx");
	assert(hModel_[ARROW] >= 0);

	hModel_[BALL] = Model::Load("assets/Ball.fbx");
	assert(hModel_[BALL] >= 0);
}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
	
	Model::SetTransform(hModel_[ARROW], transform_);
	Model::Draw(hModel_[ARROW]);

	XMFLOAT3 rot = { 0,90,0 };
	transform_.rotate_ = rot;

	Model::SetTransform(hModel_[ARROW], transform_);
	Model::Draw(hModel_[ARROW]);

	rot = { 0,0,90 };
	transform_.rotate_ = rot;

	Model::SetTransform(hModel_[ARROW], transform_);
	Model::Draw(hModel_[ARROW]);

	rot = { 0,0,0 };
	transform_.rotate_ = rot;

	Transform ball = transform_;
	ball.position_.x = -2;
	ball.rotate_.x = -90;
	Model::SetTransform(hModel_[BALL], ball);
	Model::Draw(hModel_[BALL]);


}

//開放
void Stage::Release()
{
}
