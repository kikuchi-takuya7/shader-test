#include "SceneManager.h"
#include "../TestScene.h"
#include "Model.h"

//コンストラクタ
SceneManager::SceneManager(GameObject* parent)
    :GameObject(parent, "SceneManager")
{

}

//デストラクタ
SceneManager::~SceneManager()
{
}

//初期化
void SceneManager::Initialize()
{
    currentSceneID_ = SCENE_ID_TEST;
    nextSceneID_ = currentSceneID_;
    Instantiate<TestScene>(this);
}

//更新
void SceneManager::Update()
{
    //シーンを実際に切り替える
    // 現在のシーンと、ネクストシーンが別だったら切り替え
    if (currentSceneID_ != nextSceneID_) {

        //そのシーンのオブジェクトを全削除
        auto scene = childList_.begin();
        (*scene)->ReleaseSub();
        SAFE_DELETE(*scene);
        childList_.clear();

        //ロードしたデータを全削除
        Model::Release();
        //後かたずけ終了
        switch (nextSceneID_)
        {
        case SCENE_ID_TEST:
            Instantiate<TestScene>(this);
            break;
        default:
            break;
        }
        currentSceneID_ = nextSceneID_;
    }
    // 現在のシーンの後かたずけ
    // 新しいシーンの準備
    //currentSceneIDをnextSceneId
}

//描画
void SceneManager::Draw()
{
}

//開放
void SceneManager::Release()
{
}

/// <summary>
/// 指定したシーンをnextSceneIdに設定する（次のシーンをアプデする）
/// </summary>
/// <param name="_next"></param>
void SceneManager::ChangeScene(SCENE_ID _next)
{
    nextSceneID_ = _next;
}
