#pragma once
#include "Engine/GameObject.h"
#include "Engine/Fbx.h"

enum OBJ {
	ARROW,
	DONUT,
	BALL,
	DICE,
	NUM
};

struct CBUFF_STAGESCENE {

	XMFLOAT4	lightPosition;
	XMFLOAT4	eyePos;
};

//■■シーンを管理するクラス
class Stage : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Stage(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void SetLightPos(XMFLOAT4& _pos) { lightSourcePosition_ = _pos; }
	XMFLOAT4 GetLightPos() { return lightSourcePosition_; }

private:
	int hModel_[NUM];
	XMFLOAT4 lightPos_;

	ID3D11Buffer* pCBStageScene_;
	
	Transform ballTrans_;
	Transform boxTrans_;

	void IntConstantBuffer_();
	XMFLOAT4	lightSourcePosition_;

};