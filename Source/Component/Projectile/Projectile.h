#pragma once

#include "Graphics/Model/Model.h"
#include "Graphics/shader.h"

enum class Type
{
    Player,
    Dragon
};

//弾丸
class Projectile
{
public:
    Projectile()          = default;  // コンストラクタ
    virtual ~Projectile() = default;  // デストラクタ

    // 更新処理
    virtual void Update(const float& elapsedTime) = 0;

    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();
    virtual void DrawDebugGUI() {}

    void SetScale(float _scale) { this->scale.x = this->scale.y = this->scale.z = _scale; }

    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // 方向取得
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }
    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // 破棄
    void Destroy();

    // 寿命
    void EndLife(const float& elapsedTime);

    // 半径取得
    float GetRadius() const { return radius; }

    // タイプ取得
    Type GetType() const { return type; }

    // モデル取得
    Model* GetModel() { return model.get(); }
protected:
    // 行列更新処理
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3 position    = { 0,0,0 };  // 位置
    DirectX::XMFLOAT3 direction   = { 0,0,1 };  // 方向
    DirectX::XMFLOAT3 scale       = { 1,1,1 };  // スケール
    DirectX::XMFLOAT4X4 transform = {           // 行列    
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    float radius    = 0.5f;  // 半径
    float speed     = 1.0f;  // スピード
    float lifeTimer = 3.0f;  // 寿命  
    Type  type;              // タイプ

    std::unique_ptr<Model> model = nullptr;
};