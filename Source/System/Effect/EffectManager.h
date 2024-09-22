#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクトマネージャー
class EffectManager
{
private:
    EffectManager()  = default;  // コンストラクタ
    ~EffectManager() = default;
public:
    EffectManager(const EffectManager&) = delete;
    EffectManager(EffectManager&&)      = delete;
    EffectManager operator= (const EffectManager&) = delete;
    EffectManager operator= (EffectManager&&)      = delete;

  
    // 唯一のインスタンス取得
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }

    // 初期化
    void Initialize();

    // 終了化
    void Finalize() {}

    // 更新処理
    void Update(const float& elapsedTime);

    // 描画処理
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // Effekseerマネージャーの取得
    Effekseer::ManagerRef const& GetEffekseerManager() const 
    {
        //__debugbreak();
        return effekseerManager; 
    }

private:
    Effekseer::ManagerRef          effekseerManager;
    EffekseerRenderer::RendererRef effekseerRenderer;
};
