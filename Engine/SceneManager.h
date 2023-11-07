#pragma once
#include "GameObject.h"

//ゲームのシーンを列挙
enum SCENE_ID {

    SCENE_ID_TEST = 0,
    SCENE_ID_PLAY,
};

//◆◆◆を管理するクラス
class SceneManager : public GameObject
{
    SCENE_ID currentSceneID_;
    SCENE_ID nextSceneID_;

public:

    SceneManager(GameObject* parent);
    ~SceneManager();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void ChangeScene(SCENE_ID _next);

};