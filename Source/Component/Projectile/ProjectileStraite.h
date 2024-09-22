#pragma once

#include "Graphics/Model/Model.h"
#include "Projectile.h"

struct StraiteParam
{
    DirectX::XMFLOAT3 direction;
    DirectX::XMFLOAT3 position;
    float             speed;
    float             radius;
    Type              type;
};

// 直進弾丸
class ProjectileStraite : public Projectile
{
public:
    ProjectileStraite(std::shared_ptr<ModelResource> resource);  // コンストラクタ
    ~ProjectileStraite()override = default;  // デストラクタ

    // 更新処理
    void Update(const float& elapsedTime) override;
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // 発射
    void Launch(const StraiteParam& param);

    // GUI描画
    void DrawDebugGUI();

private:
    const float gravity = 0.15f;
};