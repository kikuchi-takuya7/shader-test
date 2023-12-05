#pragma once
#include "Engine/GameObject.h"

enum OBJ {
	ARROW,
	BALL,
	NUM
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

private:
	int hModel_[NUM];
	XMFLOAT4 lightPos_;
};