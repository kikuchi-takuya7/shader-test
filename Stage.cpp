#include "Stage.h"
#include "Engine/Model.h"
#include"Engine/Input.h"

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
	
	if (Input::IsKey(DIK_W)) {
		lightPos_.z += 0.1f;
	}
	if (Input::IsKey(DIK_A)) {
		lightPos_.x -= 0.1f;
	}
	if (Input::IsKey(DIK_S)) {
		lightPos_.z -= 0.1f;
	}
	if (Input::IsKey(DIK_D)) {
		lightPos_.x += 0.1f;
	}

	//Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCBStageScene_);
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
