#pragma once

#include <Effekseer.h>
#include <DirectXMath.h>

// エフェクト
class Effect
{
public:
    Effect(const char* filename);  // コンストラクタ
    ~Effect() = default;           // デストラクタ

    // 再生
    Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);;

    // 停止
    void Stop(Effekseer::Handle handle);

    // 座標設定
    void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

    // スケール設定
    void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

private:
    Effekseer::EffectRef effekseerEffect;
};
