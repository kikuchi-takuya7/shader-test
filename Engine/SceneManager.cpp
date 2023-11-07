#include "SceneManager.h"
#include "../TestScene.h"
#include "Model.h"

//�R���X�g���N�^
SceneManager::SceneManager(GameObject* parent)
    :GameObject(parent, "SceneManager")
{

}

//�f�X�g���N�^
SceneManager::~SceneManager()
{
}

//������
void SceneManager::Initialize()
{
    currentSceneID_ = SCENE_ID_TEST;
    nextSceneID_ = currentSceneID_;
    Instantiate<TestScene>(this);
}

//�X�V
void SceneManager::Update()
{
    //�V�[�������ۂɐ؂�ւ���
    // ���݂̃V�[���ƁA�l�N�X�g�V�[�����ʂ�������؂�ւ�
    if (currentSceneID_ != nextSceneID_) {

        //���̃V�[���̃I�u�W�F�N�g��S�폜
        auto scene = childList_.begin();
        (*scene)->ReleaseSub();
        SAFE_DELETE(*scene);
        childList_.clear();

        //���[�h�����f�[�^��S�폜
        Model::Release();
        //�ォ�������I��
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
    // ���݂̃V�[���̌ォ������
    // �V�����V�[���̏���
    //currentSceneID��nextSceneId
}

//�`��
void SceneManager::Draw()
{
}

//�J��
void SceneManager::Release()
{
}

/// <summary>
/// �w�肵���V�[����nextSceneId�ɐݒ肷��i���̃V�[�����A�v�f����j
/// </summary>
/// <param name="_next"></param>
void SceneManager::ChangeScene(SCENE_ID _next)
{
    nextSceneID_ = _next;
}
