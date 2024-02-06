#include "Stage.h"
#include "Engine/Model.h"
#include"Engine/Input.h"


namespace {
	const XMFLOAT4 DEF_LIGHT_POSITION{ 1,2,1,0 };
}



//コンストラクタ
Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage"), hModel_{-1,-1},lightSourcePosition_(DEF_LIGHT_POSITION),pCBStageScene_(nullptr)
{
}

//初期化
void Stage::Initialize()
{
	//モデルデータのロード
	hModel_[DONUT] = Model::Load("assets/Ball.fbx");
	assert(hModel_[DONUT] >= 0);

	hModel_[BALL] = Model::Load("assets/Ball.fbx");
	assert(hModel_[BALL] >= 0);

	hModel_[DICE] = Model::Load("assets/Dice.fbx");
	assert(hModel_[DICE] >= 0);

	hModel_[WATER] = Model::Load("assets/waters.fbx");
	assert(hModel_[WATER] >= 0);

	transform_.scale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);

	transform_.position_.z += 2.0f;

	ballTrans_.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);

	boxTrans_.scale_ = XMFLOAT3(2.1f, 2.1f, 2.1f);

	waterTrans_.position_ = XMFLOAT3(-1.0f, -1.0f, -1.0f);

	waterTrans_.scale_ = XMFLOAT3(1.5f, 1.5f, 1.5f);

	spriteTrans_.scale_ = XMFLOAT3(0.5f, 0.5f, 0.5f);
	//ballTrans_.position_ = XMFLOAT3(0.1f, 0.1f, 0.1f);

	//pTexture_ = new Texture;
	//pTexture_->Load("Assets\\Toon.png");

	pSprite_ = new Sprite;
	pSprite_->Initialize();


	IntConstantBuffer_();
	lightPos_.z = -1;
}

//更新
void Stage::Update()
{
	
	if (Input::IsKey(DIK_UP)) {
		lightPos_.z += 0.1f;
	}
	if (Input::IsKey(DIK_LEFT)) {
		lightPos_.x -= 0.1f;
	}
	if (Input::IsKey(DIK_DOWN)) {
		lightPos_.z -= 0.1f;
	}
	if (Input::IsKey(DIK_RIGHT)) {
		lightPos_.x += 0.1f;
	}
	if (Input::IsKey(DIK_LSHIFT)) {
		lightPos_.y += 0.1f;
	}
	if (Input::IsKey(DIK_LCONTROL)) {
		lightPos_.y -= 0.1f;
	}


	SetLightPos(lightPos_);
	ballTrans_.position_ = XMFLOAT3(lightPos_.x, lightPos_.y, lightPos_.z);

	CBUFF_STAGESCENE cb;
	cb.lightPosition = lightSourcePosition_;
	XMStoreFloat4(&cb.eyePos, Camera::GetEyePosition());

	Direct3D::pContext_->UpdateSubresource(pCBStageScene_, 0, NULL, &cb, 0, 0);

	Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCBStageScene_);
	Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pCBStageScene_);

	//waterTrans_.rotate_.y += 1;
	//waterTrans_.rotate_.z += 1;
	//waterTrans_.rotate_.x += 1;
}

//描画
void Stage::Draw()
{
	
	Model::SetTransform(hModel_[DONUT], transform_);
	//Model::Draw(hModel_[DONUT]);

	Model::SetTransform(hModel_[BALL], ballTrans_);
	Model::Draw(hModel_[BALL]);

	Model::SetTransform(hModel_[DICE], boxTrans_);
	//Model::Draw(hModel_[DICE]);

	Model::SetTransform(hModel_[WATER], waterTrans_);
	Model::Draw(hModel_[WATER]);

	//pSprite_->Draw(waterTrans_);
}

//開放
void Stage::Release()
{
	SAFE_RELEASE(pSprite_);
	SAFE_DELETE(pSprite_);
	SAFE_DELETE(pTexture_);
}

void Stage::IntConstantBuffer_()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CBUFF_STAGESCENE);
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pCBStageScene_);
	if (FAILED(hr)) {
		MessageBox(nullptr, "コンストラクトバッファの作成に失敗しました", "エラー", MB_OK);

		//return E_FAIL;
	}
}
