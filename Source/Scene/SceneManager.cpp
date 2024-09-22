#include "SceneManager.h"

// �X�V����
void SceneManager::Update(const float& elapsedTime)
{
    if (nextScene != nullptr)
    {
        // �Â��V�[�����I������
        Clear();

        // �V�����V�[����ݒ�
        currentScene = nextScene;
        nextScene = nullptr;

        // �V�[������������
        // currentScene��Isready��false�Ȃ珉����
        if(!currentScene->IsReady())
        currentScene->Initialize();
        
    }

    // �X�V����
    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

// �`�揈��
void SceneManager::Render()
{
    if (currentScene != nullptr)
    {
        currentScene->Render();
    }
}

// �V�[���N���A
void SceneManager::Clear()
{
    // �V�[���N���A
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

// �V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    // �V�����V�[����ݒ�
    nextScene = scene;
}