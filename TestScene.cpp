#include "TestScene.h"
#include "MapController.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Stage.h"


//コンストラクタ
TestScene::TestScene(GameObject* parent)
    :GameObject(parent, "TestScene")
{

}

//デストラクタ
TestScene::~TestScene()
{
}

//初期化
void TestScene::Initialize()
{
 
    Instantiate<Stage>(this);
    //Instantiate<MapController>(this);
    
}

//更新
void TestScene::Update()
{

}

//描画
void TestScene::Draw()
{
}

//開放
void TestScene::Release()
{
}