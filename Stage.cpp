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
	hModel_[ARROW] = Model::Load("assets/Arrow.fbx");
	assert(hModel_[ARROW] >= 0);

	hModel_[BALL] = Model::Load("assets/Ball.fbx");
	assert(hModel_[BALL] >= 0);

	IntConstantBuffer_();
}

//更新
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


	CBUFF_STAGESCENE cb;
	cb.lightPosition = DEF_LIGHT_POSITION;
	XMStoreFloat4(&cb.eyePos, Camera::GetEyePosition());

	//D3D11_MAPPED_SUBRESOURCE pdata;
	//Direct3D::pContext_->Map(pCBStageScene_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	//memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

	//Direct3D::pContext_->Unmap(pCBStageScene_, 0);	//再開

	Direct3D::pContext_->UpdateSubresource(pCBStageScene_, 0, NULL, &cb, 0, 0);

	Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCBStageScene_);
	Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pCBStageScene_);
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
