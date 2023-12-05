#pragma once
#include "Engine/GameObject.h"

enum OBJ {
	ARROW,
	BALL,
	NUM
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

private:
	int hModel_[NUM];
	XMFLOAT4 lightPos_;
};