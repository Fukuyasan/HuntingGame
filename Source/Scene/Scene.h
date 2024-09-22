#pragma once

// �V�[��
class Scene
{
public:
    Scene()          = default;  // �R���X�g���N�^
    virtual ~Scene() = default;  // �f�X�g���N�^

    // ������
    virtual void Initialize() = 0;

    // �I����
    virtual void Finalize() = 0;

    // �X�V����
    virtual void Update(const float& elapsedTime) = 0;

    // �`�揈��
    virtual void Render() = 0;

    // �����������Ă��邩
    bool IsReady() const { return ready; }

    // ���������ݒ�
    void SetReady() { ready = true; }

private:
    bool ready = false;
};
