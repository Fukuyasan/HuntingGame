#include "Projectile.h"
#include "ProjectileManager.h"

#include "Graphics/ShaderState.h"

// デバッグプリミティブ描画
void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = ShaderState::Instance().GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    debugRenderer->AddSphere(
        position,
        radius,
        DirectX::XMFLOAT4(0, 0, 0, 1)
    );  
}

// 行列更新処理
void Projectile::UpdateTransform()
{
    DirectX::XMVECTOR Front, Up, Right;
    // 前ベクトルを算出
    Front = DirectX::XMLoadFloat3(&direction);

    // 仮の上ベクトルを算出
    DirectX::XMFLOAT3 provisionalUp = { 0,1,0 };
    Up = DirectX::XMLoadFloat3(&provisionalUp);

    // 右ベクトルを算出(外積)
    Right = DirectX::XMVector3Cross(Up, Front);

    // 上ベクトルを算出(外積)
    Up = DirectX::XMVector3Cross(Front, Right);

    // 正規化
    Front = DirectX::XMVector3Normalize(Front);
    Up    = DirectX::XMVector3Normalize(Up);
    Right = DirectX::XMVector3Normalize(Right);

    // scaleをかける
    Front = DirectX::XMVectorScale(Front,scale.z);
    Up    = DirectX::XMVectorScale(Up, scale.y);
    Right = DirectX::XMVectorScale(Right, scale.x);

    // 計算結果を取り出し
    DirectX::XMFLOAT3 front, up, right;
    DirectX::XMStoreFloat3(&front, Front);  // 前方向
    DirectX::XMStoreFloat3(&up, Up);        // 上方向
    DirectX::XMStoreFloat3(&right, Right);  // 右方向

    // 算出した軸ベクトルから行列を作成 
    // x軸   
    transform._11 = right.x;
    transform._12 = right.y;
    transform._13 = right.z;
    transform._14 = 0.0f;
    // y軸
    transform._21 = up.x;
    transform._22 = up.y;
    transform._23 = up.z;
    transform._24 = 0.0f;
    // z軸
    transform._31 = front.x;
    transform._32 = front.y;
    transform._33 = front.z;
    transform._34 = 0.0f;
    // 位置
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    // 発射方向
    this->direction = front;
}

// 破棄
void Projectile::Destroy()
{
    // マネージャーから自分を破棄する
    ProjectileManager::Instance().Remove(this);
}

// 寿命処理
void Projectile::EndLife(const float& elapsedTime)
{
    // 寿命が尽きたら自分は破棄する。
    lifeTimer -= elapsedTime;

   // 自分を削除 
   if (lifeTimer < 0) Destroy();    
}
