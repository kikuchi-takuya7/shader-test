#include "Stage.h"
#include "Engine/Model.h"

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage"), hModel_{-1,-1}
{
}

//������
void Stage::Initialize()
{
	//���f���f�[�^�̃��[�h
	hModel_[ARROW] = Model::Load("assets/Arrow.fbx");
	assert(hModel_[ARROW] >= 0);

	hModel_[BALL] = Model::Load("assets/Ball.fbx");
	assert(hModel_[BALL] >= 0);
}

//�X�V
void Stage::Update()
{
}

//�`��
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

//�J��
void Stage::Release()
{
}
