#pragma once

#include "Scene.h"

// �V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager( ) = default;  // �R���X�g���N�^
    ~SceneManager() = default;  // �f�X�g���N�^

public:
    // �B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    // �X�V����
    void Update(const float& elapsedTime);

    // �`�揈��
    void Render();

    // �V�[���N���A
    void Clear();

    // �� �Ǘ����Ă���V�[����
    //    �I���������s���֐�

    // �V�[���؂�ւ�
    void ChangeScene(Scene* scene);

private:
    Scene* currentScene = nullptr;
    Scene* nextScene    = nullptr;
};