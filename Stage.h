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

//�����V�[�����Ǘ�����N���X
class Stage : public GameObject
{
public:
	//�R���X�g���N�^
	//�����Fparent  �e�I�u�W�F�N�g�iSceneManager�j
	Stage(GameObject* parent);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�J��
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