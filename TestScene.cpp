#include "TestScene.h"
#include "MapController.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Stage.h"


//�R���X�g���N�^
TestScene::TestScene(GameObject* parent)
    :GameObject(parent, "TestScene")
{

}

//�f�X�g���N�^
TestScene::~TestScene()
{
}

//������
void TestScene::Initialize()
{
 
    Instantiate<Stage>(this);
    //Instantiate<MapController>(this);
    
}

//�X�V
void TestScene::Update()
{

}

//�`��
void TestScene::Draw()
{
}

//�J��
void TestScene::Release()
{
}