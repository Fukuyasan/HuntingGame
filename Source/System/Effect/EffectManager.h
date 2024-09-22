#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// �G�t�F�N�g�}�l�[�W���[
class EffectManager
{
private:
    EffectManager()  = default;  // �R���X�g���N�^
    ~EffectManager() = default;
public:
    EffectManager(const EffectManager&) = delete;
    EffectManager(EffectManager&&)      = delete;
    EffectManager operator= (const EffectManager&) = delete;
    EffectManager operator= (EffectManager&&)      = delete;

  
    // �B��̃C���X�^���X�擾
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }

    // ������
    void Initialize();

    // �I����
    void Finalize() {}

    // �X�V����
    void Update(const float& elapsedTime);

    // �`�揈��
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // Effekseer�}�l�[�W���[�̎擾
    Effekseer::ManagerRef const& GetEffekseerManager() const 
    {
        //__debugbreak();
        return effekseerManager; 
    }

private:
    Effekseer::ManagerRef          effekseerManager;
    EffekseerRenderer::RendererRef effekseerRenderer;
};
