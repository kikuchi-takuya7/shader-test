#include "Stage.h"
#include "Engine/Model.h"
#include"Engine/Input.h"


namespace {
	const XMFLOAT4 DEF_LIGHT_POSITION{ 1,2,-1,0 };
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

	transform_.scale_ = XMFLOAT3(0.8f, 0.8f, 0.8f);

	ballTrans_.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);

	IntConstantBuffer_();
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

	SetLightPos(lightPos_);

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
	
	Model::SetTransform(hModel_[DONUT], transform_);
	Model::Draw(hModel_[DONUT]);


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
