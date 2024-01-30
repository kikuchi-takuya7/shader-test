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
	hModel_[DONUT] = Model::Load("assets/Dice.fbx");
	assert(hModel_[DONUT] >= 0);

	hModel_[BALL] = Model::Load("assets/Ball.fbx");
	assert(hModel_[BALL] >= 0);

	transform_.scale_ = XMFLOAT3(2.0f, 2.0f, 2.0f);

	ballTrans_.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);

	IntConstantBuffer_();
	lightPos_.z = -1;

	sprite_ = new Sprite;

	sprite_->Load("outUV.png");
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

	transform_.rotate_.y += 1;
}

//描画
void Stage::Draw()
{
	
	Model::SetTransform(hModel_[DONUT], transform_);
	Model::Draw(hModel_[DONUT]);

	Model::SetTransform(hModel_[BALL], ballTrans_);
	Model::Draw(hModel_[BALL]);


	Transform t;
	t.position_ = { 0, 0, 0 };
	t.scale_ = { 1.0, 1.0, 1.0 };
	t.rotate_ = { 0,0,0 };
	RECT rec{ 0, 0, 300, 300 };
	sprite_->Draw(t, rec, 0.5f);

}

//開放
void Stage::Release()
{
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
