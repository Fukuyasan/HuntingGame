#pragma once

#include "Scene.h"
#include "UI/Button/UIButton.h"
#include "UI/NowLoading/NowLoading.h"

#include <thread>

// ���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
    // �R���X�g���N�^�̏������q���X�g�Ŏ��̃V�[����ݒ�
    SceneLoading(Scene* nextScene) : nextScene(nextScene) {}  // �R���X�g���N�^
    ~SceneLoading() override = default;                       // �f�X�g���N�^

    // ������
    void Initialize() override;

    // �I����
    void Finalize() override;

    // �X�V����
    void Update(const float& elapsedTime) override;

    // �`�揈��
    void Render() override;
    
private:
    // ���[�f�B���O�X���b�h
    static void LoadingThread(SceneLoading* scene);

private:
    float angle    = 0.0f;

    Scene* nextScene    = nullptr;
    std::thread* thread = nullptr;

    // NOWLOADING ->10����
    std::unique_ptr<NowLoading> nowLoading[10] = {};
};